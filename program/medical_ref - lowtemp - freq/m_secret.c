/*****************************************************************************************************************************
overview:
        1、密钥的生成：将工厂参数LL设置成1后根据时间生成密钥，显示在显示板上；	
        2、破解密钥：根据公式输入正确的值后，破解；

        3、生成密钥后，程序中也会生成破解值，将程序生成的和外部输入的比较看是否正确


        公式：(x+2)/3 +(x%3) +256
        
*******************************************************************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"

#include "h_rom_para2.h"
#include "h_type_define.h"
#include "h_key_operation.h"
#include "h_com.h"
#include "M_RTC.h"
#include "h_main.h"
#include "m_secret.h"


void secret_key_main_loop(void);  
void secret_key_time_op(void); 
void secret_key_led_disp1_deal(void);     
void secret_key_read_key_deal(void);
void secret_key_decode_deal(void);
void secret_key_generate(void);


flag_type flg_secret;


int8_t  guc_secret_key_decode_1;        //破解密钥时的破解值,LED1 百位
int8_t  guc_secret_key_decode_2;        //破解密钥时的破解值,LED2 十位
int8_t  guc_secret_key_decode_3;        //破解密钥时的破解值,LED3 个位

uint16_t guc_RTC_secret_key;            //生成的密钥值
uint16_t guc_RTC_secret_decode;         //生成的破解密钥值




//====密钥未破解时执行的主循环====
/***************************************************************************************************************************
功能描述:  密钥未破解时的主循环

函数位置:   系统主循环中------ -----------------ok        
***************************************************************************************************************************/
void secret_key_main_loop(void)
{   
    secret_key_time_op();         //定时程序    

    //------------------------------------------------------
    if (bflg_com_allow_tx == 1)   //如果允许发送
    {        
        bflg_com_allow_tx = 0;
        //------------------------------
        R_UART0_Start();
        COM_TX_MODE;
        com_tx_init();            //通讯发送初始化程序
    }
    //----------------------------------
    if (bflg_com_tx_ok == 1)      //如果发送成功
    {
        bflg_com_tx_ok = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_allow_rx == 1)    //如果允许接收
    {
        bflg_com_allow_rx = 0;
        //------------------------------
        com_rx_init();            //通讯接收初始化程序
        COM_RX_MODE;
        R_UART0_Start();
    }
    //----------------------------------
    if (bflg_com_rx_end == 1)     //如果接收结束
    {
        bflg_com_rx_end = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_rx_ok == 1)     //如果接收成功
    {
        bflg_com_rx_ok = 0;
        //------------------------------
        R_UART0_Stop();
        com_rx_data_deal();      //通讯接收数据处理程序
    }
    //----------------------------------
    if (bflg_read_key_delaytime == 0)   //如果读键值时间间隔到
    {
        bflg_read_key_delaytime = 1;
        gss_read_key_delaytimer = 0;
        
        secret_key_read_key_deal();     //读键值处理程序
    }
    
    //----------------------------------
    if(bflg_RTC_continuous_r_ok == 1)// 获取时间完成
    {   
        bflg_RTC_continuous_r_ok = 0;
        secret_key_generate();        //生成密钥值和破解密钥值
    }
    //----------------------------------
    RTC_continuous_r();             //读取年月日时分秒
    secret_key_decode_deal();       //处理输入的破解密钥
}

/**********************************************************************************************************************************
功能描述: 密钥未破解时的定时程序

函数位置:   secret_key_main_loop()中 -----------------ok        
**********************************************************************************************************************************/
void secret_key_time_op(void)
{
    if (bflg_1ms_reach == 1)  //如果1ms定时到
    {
        bflg_1ms_reach = 0;
        guc_100ms_timer--;
        //--------------------------------------------------
        //1ms定时调用程序
        //--------------------------------------------------
        com_rx_delaytime();     //通讯接收延时程序
        
        com_tx_delaytime();     //通讯发送延时程序
        
        com_rx_end_delaytime(); //通讯接收完成延时程序

        RTC_write_read_enable();
    }
    //------------------------------------------------------
    if (bflg_10ms_reach == 1)
    {
        bflg_10ms_reach = 0;
        guc_1s_timer--;
        //--------------------------------------------------
        //10ms定时调用程序
        
        secret_key_led_disp1_deal(); //数码管1显示处理程序
        read_key_delaytime();        //读键延时程序
    }
    //------------------------------------------------------
    if (guc_100ms_timer == 0)
    {
        guc_100ms_timer = 100;
        //--------------------------------------------------
        //100ms定时调用程序
        
        led_disp1_flicker_delaytime();  // 数码管闪烁
    }
    //------------------------------------------------------
    if (guc_1s_timer == 0)
    {
        guc_1s_timer = 100;
        guc_1min_timer--;
        //--------------------------------------------------
        //1s定时调用程序
        //--------------------------------------------------
    }
    //------------------------------------------------------
    if (guc_1min_timer == 0)
    {
        guc_1min_timer = 60;
        //--------------------------------------------------
        //1min定时调用程序
    }
}



/***************************下面为密钥未破解时的处理函数*******************************/

/******************************************************************************************************************************
功能描述: 密钥未破解时的数码管1的显示

函数位置: 密钥主循环中---------------------ok       
******************************************************************************************************************************/
void secret_key_led_disp1_deal(void)
{
    uint16_t gul_secret_key_sum;

    gul_secret_key_sum = guc_secret_key_decode_1*100 +
                         guc_secret_key_decode_2*10  +
                         guc_secret_key_decode_3;
    
   
    if(bflg_disp1_flicker == 1)    //闪烁
    {
        LED2 = guc_nmb_table[guc_secret_key_decode_1];  //显示数字
        LED3 = guc_nmb_table[guc_secret_key_decode_2];  //显示数字
        LED4 = guc_nmb_table[guc_secret_key_decode_3];  //显示数字
    }
    else
    {
        if(bflg_secret_key_1 == 0) //百位_数据未确定时要闪
        {
            LED2 = data_all_off;   //因为实际只有3位数码管所以LED1未用
        }
        else
        {
            LED2 = guc_nmb_table[guc_secret_key_decode_1];  //显示数字
        }

        if(bflg_secret_key_2 == 0) //十位
        {
            LED3 = data_all_off; 
        }
        else
        {
            LED3 = guc_nmb_table[guc_secret_key_decode_2]; 
        }

        if(bflg_secret_key_3 == 0) //个位
        {
            LED4 = data_all_off; 
        }
        else
        {
            LED4 = guc_nmb_table[guc_secret_key_decode_3];   
        }
    }


    LED1 = 0;
    LED5 = 0; LED6 = 0;  LED7 = 0;  LED8 = 0;
    LED9 = 0; LED10 = 0; LED11 = 0; LED12 = 0;   //其他的灯都灭
    //guc_com_tx_buffer[20] = 0x00;              //要改需要在 com_tx_init()中
}


/******************************************************************************************************************************
功能描述: 密钥未解锁时的按键处理

函数位置: 密钥主循环中---------------------ok       
******************************************************************************************************************************/
void secret_key_read_key_deal(void)
{
    if (guc_key_val == KEY_SET)         //如果是设置键，确认输入的密钥破解值 
    {       
        if(bflg_secret_key_1 == 0)
        {
            bflg_secret_key_1 = 1;
        }
        else if(bflg_secret_key_2 == 0)
        {
            bflg_secret_key_2 = 1;
        }
        else if(bflg_secret_key_3 == 0)   
        {
            bflg_secret_key_3 = 1;     
            bflg_secret_key_ok = 1;         //输入密钥破解完成
        }
    }
      
    else if (guc_key_val == KEY_UP)             
    {   
        if(bflg_secret_key_1 == 0)            //百位
        {
            guc_secret_key_decode_1++;     
            if(guc_secret_key_decode_1 >= 10)
            {
                guc_secret_key_decode_1 = 9;
            }
        }
        else if(bflg_secret_key_2 == 0)      //十位
        {
            guc_secret_key_decode_2++;
            if(guc_secret_key_decode_2 >= 10)
            {
                guc_secret_key_decode_2 = 9;
            }
        }
        else if(bflg_secret_key_3 == 0)     //个位
        {
            guc_secret_key_decode_3++;
            if(guc_secret_key_decode_3 >= 10)
            {
                guc_secret_key_decode_3 = 9;
            }
        }
    }
     
    else if (guc_key_val == KEY_DOWN)    
    {
        if(bflg_secret_key_1 == 0)
        {
            guc_secret_key_decode_1--;
            if(guc_secret_key_decode_1 <= 0)
            {
                guc_secret_key_decode_1 = 0;
            }
        }
        else if(bflg_secret_key_2 == 0)
        {
            guc_secret_key_decode_2--;
            if(guc_secret_key_decode_2 <= 0)
            {
                guc_secret_key_decode_2 = 0;
            }
        }
        else if(bflg_secret_key_3 == 0)
        {
            guc_secret_key_decode_3--;
            if(guc_secret_key_decode_3 <= 0)
            {
                guc_secret_key_decode_3 = 0;
            }
        }
    }                   
}

/******************************************************************************************************************************
功能描述: 破解密钥值输入完成后，检测输入的破解密钥值是否正确

函数位置: 密钥主循环中---------------------ok      
******************************************************************************************************************************/
void secret_key_decode_deal(void)
{
    if(bflg_secret_key_ok == 1)    //破解密钥输入完成
    {
        bflg_secret_key_ok = 0;
        
        //破解密钥输入正确
        if(guc_RTC_secret_decode == ((uint16_t)guc_secret_key_decode_1*100 +  
                                     (uint16_t)guc_secret_key_decode_2*10  +
                                     (uint16_t)guc_secret_key_decode_3))
        {
            bflg_secret_key_1 = 0;
            bflg_secret_key_2 = 0;
            bflg_secret_key_3 = 0;
            
            gss_factory_parm_value[DISP_FACTORY_LL] = 0;                                      //变为0
             
            ram_para[num_secret_key] = gss_factory_parm_value[DISP_FACTORY_LL];               //保存                                                          //!!!!!!!
            eeprom_write(num_secret_key, ram_para[num_secret_key]); 

            
            //bflg_RTC_continuous_r_start = 1;                         //破解成功后又读时间
        }
        else
        {
            //等待
        }
    } 
}


/******************************************************************************************************************************************
函数作用:1、 获取密钥的原始数据，计算此刻的时分秒距00:00:00总共走了多少秒，
             密钥值为所有秒数的和的后3位；

         2、 生成密钥破解值的方式:密钥值除以2加1024后再除以2。 范围 512~761                 

函数位置: 密钥的主循环中---------------------------------------ok
*******************************************************************************************************************************************/
void secret_key_generate(void)
{
    uint32_t gul_secret_key_temp = 0; 

    
    //-----------------------------------------------------------------------
    //密钥值的生成方式:所有的秒数和的后3位 ,  // 23:59:59 最大86399
    gul_secret_key_temp =    ((uint32_t)guc_RTC_r_hour*60*60 + 
                              (uint32_t)guc_RTC_r_minute*60  + 
                              (uint32_t)guc_RTC_r_second);        
                                     
    
    gul_secret_key_temp %= 10000;
    gul_secret_key_temp %= 1000;  

    guc_RTC_secret_key = (uint16_t) gul_secret_key_temp;             //密钥值 范围 0~999

    

    //---------------------------------------------------------------------------------
    //密钥破解值的生成方式: 范围 256~591
    guc_RTC_secret_decode = ((guc_RTC_secret_key +2)/ 3 +(guc_RTC_secret_key%3)) +256 ;  //破解密钥值

    
    //----------------------------------------------------------------------------------
    //将密钥值分别赋值需要输入的破解密钥值，是为了按键调节时破解值从显示的密钥值开始调节
    guc_secret_key_decode_1 = (uint8_t)(guc_RTC_secret_key/100);      //对应百位 
    guc_secret_key_decode_2 = (uint8_t)((guc_RTC_secret_key%100)/10); //对应十位
    guc_secret_key_decode_3 = (uint8_t)((guc_RTC_secret_key%100)%10); //对应个位
}












/**********************************END OF THE FILE*****************************************************************************/
