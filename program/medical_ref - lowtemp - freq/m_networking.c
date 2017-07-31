/****************************************************************************************************************************************
overview: 主板串口1，端口CN8的485组网接口

            1、此处主要用于测试时连接另一块显示板，用于显示环温、冷凝器温度、热交换器温度    
            2、此处的处理方式用485总线，可以接多个从机
            3、通讯策略： 主机一直发，不接收(接多个从机就循环的一直发)
                          从机一直收，不发送(只对自己的数据进行处理)
******************************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"

#include "h_type_define.h"
#include "h_comp2.h"
#include "h_key_operation.h"
#include "h_ad.h"
#include "M_RTC.h"
#include "h_bat_ctrl.h"
#include "m_networking.h"



//声明
void net_com_interval(void);
void net_com_tx_init(void);
void net_com_tx_int(void);
void net_com_deal(void);
void net_led_disp1_deal_slave_1(void);
void net_led_disp2_deal_slave_1(void);
void net_led_disp3_deal_slave_1(void);
void net_led_disp1_deal_slave_2(void);
void net_led_disp2_deal_slave_2(void);
void net_led_disp3_deal_slave_2(void);
void net_led_disp1_deal_slave_3(void);
void net_led_disp2_deal_slave_3(void);
void net_led_disp3_deal_slave_3(void);

void net_led1_disp_val_slave_1(int16_t lss_disp_val);
void net_led2_disp_val_slave_1(int16_t lss_disp_val);
void net_led3_disp_val_slave_1(int16_t lss_disp_val);
void net_led1_disp_val_slave_2(int16_t lss_disp_val);
void net_led2_disp_val_slave_2(int16_t lss_disp_val);
void net_led3_disp_val_slave_2(int16_t lss_disp_val);
void net_led1_disp_val_slave_3(int16_t lss_disp_val);
void net_led2_disp_val_slave_3(int16_t lss_disp_val);
void net_led3_disp_val_slave_3(int16_t lss_disp_val);

unsigned int net_CRC16(unsigned char *puchmsg, unsigned int usDataLen);    

//array
uint8_t   guc_led_buffer_slave_1[12];  
uint8_t   guc_led_buffer_slave_2[12]; 
uint8_t   guc_led_buffer_slave_3[12]; 

uint8_t   guc_net_com_tx_buffer[23];    //发送的数据


//variate
uint8_t guc_net_com_delaytime;          //数据发送间隔
uint8_t guc_net_slave_add;              //从机地址
uint8_t guc_net_com_tx_point;           //发送数据索引
uint8_t guc_net_disp_rate;              

//sign
flag_type flg_net_com;

//macro
#define     SLAVE_1_ADD    0                  //从机1地址 
#define     SLAVE_2_ADD    1                  //从机2地址
#define     SLAVE_3_ADD    2                  //从机3地址
#define     SLAVE_SUM      3                  //从机总数

#define     NET_COM_RX_MODE    (P0.4 = 0)     //接收模式
#define     NET_COM_TX_MODE    (P0.4 = 1)     //发送模式


#define   LED1_SLAVE_1      guc_led_buffer_slave_1[0]
#define   LED2_SLAVE_1      guc_led_buffer_slave_1[1]
#define   LED3_SLAVE_1      guc_led_buffer_slave_1[2]
#define   LED4_SLAVE_1      guc_led_buffer_slave_1[3]
#define   LED5_SLAVE_1      guc_led_buffer_slave_1[4]
#define   LED6_SLAVE_1      guc_led_buffer_slave_1[5]
#define   LED7_SLAVE_1      guc_led_buffer_slave_1[6]
#define   LED8_SLAVE_1      guc_led_buffer_slave_1[7]   //lamp
#define   LED9_SLAVE_1      guc_led_buffer_slave_1[8]
#define   LED10_SLAVE_1     guc_led_buffer_slave_1[9]
#define   LED11_SLAVE_1     guc_led_buffer_slave_1[10]
#define   LED12_SLAVE_1     guc_led_buffer_slave_1[11]  //lamp

#define   LED1_SLAVE_2      guc_led_buffer_slave_2[0]
#define   LED2_SLAVE_2      guc_led_buffer_slave_2[1]
#define   LED3_SLAVE_2      guc_led_buffer_slave_2[2]
#define   LED4_SLAVE_2      guc_led_buffer_slave_2[3]
#define   LED5_SLAVE_2      guc_led_buffer_slave_2[4]
#define   LED6_SLAVE_2      guc_led_buffer_slave_2[5]
#define   LED7_SLAVE_2      guc_led_buffer_slave_2[6]
#define   LED8_SLAVE_2      guc_led_buffer_slave_2[7]    
#define   LED9_SLAVE_2      guc_led_buffer_slave_2[8]
#define   LED10_SLAVE_2     guc_led_buffer_slave_2[9]
#define   LED11_SLAVE_2     guc_led_buffer_slave_2[10]
#define   LED12_SLAVE_2     guc_led_buffer_slave_2[11]  

#define   LED1_SLAVE_3      guc_led_buffer_slave_3[0]
#define   LED2_SLAVE_3      guc_led_buffer_slave_3[1]
#define   LED3_SLAVE_3      guc_led_buffer_slave_3[2]
#define   LED4_SLAVE_3      guc_led_buffer_slave_3[3]
#define   LED5_SLAVE_3      guc_led_buffer_slave_3[4]
#define   LED6_SLAVE_3      guc_led_buffer_slave_3[5]
#define   LED7_SLAVE_3      guc_led_buffer_slave_3[6]
#define   LED8_SLAVE_3      guc_led_buffer_slave_3[7]    
#define   LED9_SLAVE_3      guc_led_buffer_slave_3[8]
#define   LED10_SLAVE_3     guc_led_buffer_slave_3[9]
#define   LED11_SLAVE_3     guc_led_buffer_slave_3[10]
#define   LED12_SLAVE_3     guc_led_buffer_slave_3[11]  


/*********************************************************************************************************************************************
函数功能：组网485主机循环发送给不同从机数据的时间间隔

函数位置：1ms定时器--------------------------------ok
***********************************************************************************************************************************************/
void net_com_interval(void)
{
    guc_net_com_delaytime++;
    if(guc_net_com_delaytime >= 200)    //100ms
    {
        guc_net_com_delaytime = 0;

        guc_net_slave_add++;
        if(guc_net_slave_add >= SLAVE_SUM)
        {
            guc_net_slave_add = 0;
        }

        bflg_net_tx_allow = 1;
    }
}


/*********************************************************************************************************************************************
函数功能： 发送数据初始化

函数位置：net_com_deal(主循环)--------------------------------ok
***********************************************************************************************************************************************/
void net_com_tx_init(void)
{
    int8_t i;
    word_type gss_tmp;
    word_type com_crc;

    if(guc_net_slave_add == SLAVE_1_ADD)
    {
        guc_net_com_tx_buffer[0] = SLAVE_1_ADD;     //设备地址
        guc_net_com_tx_buffer[1] = 0x10;            //写命令

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //起始地址0

        guc_net_com_tx_buffer[4] = 0x00;
        guc_net_com_tx_buffer[5] = 7;            
        guc_net_com_tx_buffer[6] = 14;            

        //LED1_SLAVE_1 = 1;
        
        for (i = 0; i < 12; i++)                          
        {
            guc_net_com_tx_buffer[i + 7] = guc_led_buffer_slave_1[i];     
        }  

        
        com_crc.uword = net_CRC16(guc_net_com_tx_buffer, 21);
        guc_net_com_tx_buffer[21] = com_crc.ubyte.low;
        guc_net_com_tx_buffer[22] = com_crc.ubyte.high;
    }
    else if(guc_net_slave_add == SLAVE_2_ADD)
    {
        guc_net_com_tx_buffer[0] = SLAVE_2_ADD;     //设备地址
        guc_net_com_tx_buffer[1] = 0x10;            //写命令

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //起始地址0

        guc_net_com_tx_buffer[4] = 0x00;
        guc_net_com_tx_buffer[5] = 7;            
        guc_net_com_tx_buffer[6] = 14;            

        for (i = 0; i < 12; i++)                          
        {
            guc_net_com_tx_buffer[i + 7] = guc_led_buffer_slave_2[i];     
        }  

        com_crc.uword = net_CRC16(guc_net_com_tx_buffer, 21);
        guc_net_com_tx_buffer[21] = com_crc.ubyte.low;
        guc_net_com_tx_buffer[22] = com_crc.ubyte.high;
    }
    else if(guc_net_slave_add == SLAVE_3_ADD)
    {
        guc_net_com_tx_buffer[0] = SLAVE_3_ADD;     //设备地址
        guc_net_com_tx_buffer[1] = 0x10;            //写命令

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //起始地址0

        guc_net_com_tx_buffer[4] = 0x00;
        guc_net_com_tx_buffer[5] = 7;            
        guc_net_com_tx_buffer[6] = 14;            

        for (i = 0; i < 12; i++)                          
        {
            guc_net_com_tx_buffer[i + 7] = guc_led_buffer_slave_3[i];     
        }  

        com_crc.uword = net_CRC16(guc_net_com_tx_buffer, 21);
        guc_net_com_tx_buffer[21] = com_crc.ubyte.low;
        guc_net_com_tx_buffer[22] = com_crc.ubyte.high;
    }
    
    guc_net_com_tx_point = 0;
    bflg_net_com_tx_busy = 1;
    TXD1 = guc_net_com_tx_buffer[guc_net_com_tx_point++];
}

/*********************************************************************************************************************************************
函数功能： 发送数据

函数位置：系统发送中断--------------------------------ok
***********************************************************************************************************************************************/
void net_com_tx_int(void)
{
    if (bflg_net_com_tx_busy == 1)
    {
        if (guc_net_com_tx_point < 23)//23
        {
            TXD1 = guc_net_com_tx_buffer[guc_net_com_tx_point++];     //发送数据
        }
        else
        {
            guc_net_com_tx_point = 0;    //清发送字节索引
            bflg_net_com_tx_busy = 0;    //清发送忙标志
            bflg_net_com_tx_ok = 1;
        }
    }
}

/*********************************************************************************************************************************************
函数功能： 总的数据处理

函数位置：主循环--------------------------------ok
***********************************************************************************************************************************************/
void net_com_deal(void)
{
    if(bflg_net_com_tx_ok == 1)
    {
        bflg_net_com_tx_ok = 0;
        R_UART1_Stop();
    }

    if(bflg_net_tx_allow == 1)
    {
        bflg_net_tx_allow = 0;
        R_UART1_Start();
        NET_COM_TX_MODE;
        net_com_tx_init();
    }
}


//==============================================================================================================================
//显示处理_第1个从机
//10ms定时器 --------
void net_led_disp1_deal_slave_1(void)
{
    guc_net_disp_rate = dot_0d1;                 
    net_led1_disp_val_slave_1(guc_bat_Vout);           //电池电压  __放大10倍
}
void net_led_disp2_deal_slave_1(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_1(gss_TC);                 //冷凝器温度
}
void net_led_disp3_deal_slave_1(void)         
{         
    guc_net_disp_rate = not_dot;
    net_led3_disp_val_slave_1(gss_TE);                 //热交换温度
}

//==============================================================================================================================
//显示处理_第2个从机
//10ms定时器 --------
void net_led_disp1_deal_slave_2(void)
{
    guc_net_disp_rate = not_dot;
    net_led1_disp_val_slave_2(guc_RTC_r_second);        //秒
}
void net_led_disp2_deal_slave_2(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_2(gss_comp2_freq / 10);     //高压机转速
}
void net_led_disp3_deal_slave_2(void)         
{         
    guc_net_disp_rate = not_dot;
    net_led3_disp_val_slave_2(gss_comp21_freq / 10);    //低压机转速
}

//==============================================================================================================================
//显示处理_第3个从机
//10ms定时器 --------
void net_led_disp1_deal_slave_3(void)
{
    guc_net_disp_rate = dot_0d1;
    net_led1_disp_val_slave_3(guc_bat_Vin);             //充电电压  __放大10倍
}
void net_led_disp2_deal_slave_3(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_3(gss_bat_I);              //充电电流 
}
void net_led_disp3_deal_slave_3(void)         
{         
    guc_net_disp_rate = not_dot;                      //充电占空比的值(总的800)
    net_led3_disp_val_slave_3(gss_bat_pwm_value);      
}


//---------------------------------------------------------------------------------------------
void net_led1_disp_val_slave_1(int16_t lss_disp_val)     //数码管1显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //显示负数标志   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak 将负数的补码转换为我们想要的不带负号的原码
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak 取商，取第四位的值
    {
        LED1_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak 有值就显示
    }
    else
    {
        LED1_SLAVE_1 = data_all_off;                             //peak 没值就不亮
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak 取余，剩下的数值
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak 判断LED2此位小数点是否开
        {
            LED2_SLAVE_1 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak 判断前一位是否有负号
        {
            if (LED1_SLAVE_1 == data_all_off)
            {
                LED1_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d01)                  //peak 判断为0时是否开小数点
        {
            LED2_SLAVE_1 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_1 == data_all_off)                   //peak 前一位没有值，此位为0没意义，也关掉
            {
                LED2_SLAVE_1 = data_all_off;
            }
            else
            {
                LED2_SLAVE_1 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED3
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED3_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_1 |= data_dot;               //peak LED3的小数点也显示上
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED2_SLAVE_1 == data_all_off)
            {
                LED2_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_1 == data_all_off)
            {
                LED1_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_1 = data_0 | data_dot;
        }
        else
        {
            if (LED2_SLAVE_1 == data_all_off)
            {
                LED3_SLAVE_1 = data_all_off;
            }
            else
            {
                LED3_SLAVE_1 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED4
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED4_SLAVE_1 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED3_SLAVE_1 == data_all_off)
            {
                LED3_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED2_SLAVE_1 == data_all_off)
            {
                LED2_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_1 == data_all_off)
            {
                LED1_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED4_SLAVE_1 = data_0;
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
 中间这个数码管为3位数码管
 
*******************************************************************************/
void net_led2_disp_val_slave_1(int16_t lss_disp_val) //数码管2显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED5_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED5_SLAVE_1 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED6_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_1 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED5_SLAVE_1 == data_all_off)
            {
                LED5_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_1 = data_0 | data_dot;
        }
        else
        {
            if (LED5_SLAVE_1 == data_all_off)
            {
                LED6_SLAVE_1 = data_all_off;
            }
            else
            {
                LED6_SLAVE_1 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED7_SLAVE_1 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED6_SLAVE_1 == data_all_off)
            {
                LED6_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED5_SLAVE_1 == data_all_off)
            {
                LED5_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED7_SLAVE_1 = data_0;
    }
}
//------------------------------------------------------------------------------
 /*******************************************************************************
 右面这个数码管是3位数码管  
*******************************************************************************/
void net_led3_disp_val_slave_1(int16_t lss_disp_val) //数码管3显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED9_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED9_SLAVE_1 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED10_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_1 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED9_SLAVE_1 == data_all_off)
            {
                LED9_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_1 = data_0 | data_dot;
        }
        else
        {
            if (LED9_SLAVE_1 == data_all_off)
            {
                LED10_SLAVE_1 = data_all_off;
            }
            else
            {
                LED10_SLAVE_1 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED11_SLAVE_1 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED10_SLAVE_1 == data_all_off)
            {
                LED10_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED9_SLAVE_1 == data_all_off)
            {
                LED9_SLAVE_1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED11_SLAVE_1 = data_0;
    }
}


//2-----------------------------------------------------------------------------------------------
void net_led1_disp_val_slave_2(int16_t lss_disp_val)     //数码管1显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //显示负数标志   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak 将负数的补码转换为我们想要的不带负号的原码
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak 取商，取第四位的值
    {
        LED1_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak 有值就显示
    }
    else
    {
        LED1_SLAVE_2 = data_all_off;                             //peak 没值就不亮
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak 取余，剩下的数值
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak 判断LED2此位小数点是否开
        {
            LED2_SLAVE_2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak 判断前一位是否有负号
        {
            if (LED1_SLAVE_2 == data_all_off)
            {
                LED1_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d01)                  //peak 判断为0时是否开小数点
        {
            LED2_SLAVE_2 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_2 == data_all_off)                   //peak 前一位没有值，此位为0没意义，也关掉
            {
                LED2_SLAVE_2 = data_all_off;
            }
            else
            {
                LED2_SLAVE_2 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED3
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED3_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_2 |= data_dot;               //peak LED3的小数点也显示上
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED2_SLAVE_2 == data_all_off)
            {
                LED2_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_2 == data_all_off)
            {
                LED1_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_2 = data_0 | data_dot;
        }
        else
        {
            if (LED2_SLAVE_2 == data_all_off)
            {
                LED3_SLAVE_2 = data_all_off;
            }
            else
            {
                LED3_SLAVE_2 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED4
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED4_SLAVE_2 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED3_SLAVE_2 == data_all_off)
            {
                LED3_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED2_SLAVE_2 == data_all_off)
            {
                LED2_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_2 == data_all_off)
            {
                LED1_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED4_SLAVE_2 = data_0;
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
 中间这个数码管为3位数码管
 
*******************************************************************************/
void net_led2_disp_val_slave_2(int16_t lss_disp_val) //数码管2显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED5_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED5_SLAVE_2 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED6_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED5_SLAVE_2 == data_all_off)
            {
                LED5_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_2 = data_0 | data_dot;
        }
        else
        {
            if (LED5_SLAVE_2 == data_all_off)
            {
                LED6_SLAVE_2 = data_all_off;
            }
            else
            {
                LED6_SLAVE_2 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED7_SLAVE_2 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED6_SLAVE_2 == data_all_off)
            {
                LED6_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED5_SLAVE_2 == data_all_off)
            {
                LED5_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED7_SLAVE_2 = data_0;
    }
}
//------------------------------------------------------------------------------
 /*******************************************************************************
 右面这个数码管是3位数码管  
*******************************************************************************/
void net_led3_disp_val_slave_2(int16_t lss_disp_val) //数码管3显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED9_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED9_SLAVE_2 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED10_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED9_SLAVE_2 == data_all_off)
            {
                LED9_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_2 = data_0 | data_dot;
        }
        else
        {
            if (LED9_SLAVE_2 == data_all_off)
            {
                LED10_SLAVE_2 = data_all_off;
            }
            else
            {
                LED10_SLAVE_2 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED11_SLAVE_2 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED10_SLAVE_2 == data_all_off)
            {
                LED10_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED9_SLAVE_2 == data_all_off)
            {
                LED9_SLAVE_2 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED11_SLAVE_2 = data_0;
    }
}




//3-----------------------------------------------------------------------------------------------
void net_led1_disp_val_slave_3(int16_t lss_disp_val)     //数码管1显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //显示负数标志   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak 将负数的补码转换为我们想要的不带负号的原码
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak 取商，取第四位的值
    {
        LED1_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak 有值就显示
    }
    else
    {
        LED1_SLAVE_3 = data_all_off;                             //peak 没值就不亮
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak 取余，剩下的数值
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak 判断LED2此位小数点是否开
        {
            LED2_SLAVE_3 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak 判断前一位是否有负号
        {
            if (LED1_SLAVE_3 == data_all_off)
            {
                LED1_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d01)                  //peak 判断为0时是否开小数点
        {
            LED2_SLAVE_3 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_3 == data_all_off)                   //peak 前一位没有值，此位为0没意义，也关掉
            {
                LED2_SLAVE_3 = data_all_off;
            }
            else
            {
                LED2_SLAVE_3 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED3
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED3_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_3 |= data_dot;               //peak LED3的小数点也显示上
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED2_SLAVE_3 == data_all_off)
            {
                LED2_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_3 == data_all_off)
            {
                LED1_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED3_SLAVE_3 = data_0 | data_dot;
        }
        else
        {
            if (LED2_SLAVE_3 == data_all_off)
            {
                LED3_SLAVE_3 = data_all_off;
            }
            else
            {
                LED3_SLAVE_3 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED4
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED4_SLAVE_3 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED3_SLAVE_3 == data_all_off)
            {
                LED3_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED2_SLAVE_3 == data_all_off)
            {
                LED2_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1_SLAVE_3 == data_all_off)
            {
                LED1_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED4_SLAVE_3 = data_0;
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
 中间这个数码管为3位数码管
 
*******************************************************************************/
void net_led2_disp_val_slave_3(int16_t lss_disp_val) //数码管2显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED5_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED5_SLAVE_3 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED6_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_3 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED5_SLAVE_3 == data_all_off)
            {
                LED5_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED6_SLAVE_3 = data_0 | data_dot;
        }
        else
        {
            if (LED5_SLAVE_3 == data_all_off)
            {
                LED6_SLAVE_3 = data_all_off;
            }
            else
            {
                LED6_SLAVE_3 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED7_SLAVE_3 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED6_SLAVE_3 == data_all_off)
            {
                LED6_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED5_SLAVE_3 == data_all_off)
            {
                LED5_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED7_SLAVE_3 = data_0;
    }
}
//------------------------------------------------------------------------------
 /*******************************************************************************
 右面这个数码管是3位数码管  
*******************************************************************************/
void net_led3_disp_val_slave_3(int16_t lss_disp_val) //数码管3显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;
        lus_disp_abs_val = (~lss_disp_val) + 1;
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED9_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED9_SLAVE_3 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED10_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_3 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED9_SLAVE_3 == data_all_off)
            {
                LED9_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_net_disp_rate == dot_0d1)
        {
            LED10_SLAVE_3 = data_0 | data_dot;
        }
        else
        {
            if (LED9_SLAVE_3 == data_all_off)
            {
                LED10_SLAVE_3 = data_all_off;
            }
            else
            {
                LED10_SLAVE_3 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED11_SLAVE_3 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED10_SLAVE_3 == data_all_off)
            {
                LED10_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED9_SLAVE_3 == data_all_off)
            {
                LED9_SLAVE_3 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED11_SLAVE_3 = data_0;
    }
}

unsigned int net_CRC16(unsigned char *puchmsg, unsigned int usDataLen)    //进行CRC校验的程序
{
    unsigned int uchCRC;
    unsigned int uIndex_x, uIndex_y;
    
    uchCRC = 0xFFFF;
    
    if (usDataLen > 0)
    {
        for (uIndex_x = 0; uIndex_x <= (usDataLen - 1); uIndex_x++)
        {
            uchCRC = uchCRC ^ (unsigned int) (*puchmsg);
            puchmsg++;
            
            for (uIndex_y = 0; uIndex_y <= 7; uIndex_y++)   //peak 8个位
            {
                if ((uchCRC & 0x0001) != 0)                 //peak 最后一位为1
                {
                    uchCRC = (uchCRC >> 1) ^ 0xA001;        
                }
                else
                {
                    uchCRC = uchCRC >> 1;
                }
            }
        }
    }
    else
    {
        uchCRC = 0;
    }
    
    return uchCRC;
}




/*************************************END OF THE FILE****************************************************************/
