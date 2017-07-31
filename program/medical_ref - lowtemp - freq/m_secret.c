/*****************************************************************************************************************************
overview:
        1����Կ�����ɣ�����������LL���ó�1�����ʱ��������Կ����ʾ����ʾ���ϣ�	
        2���ƽ���Կ�����ݹ�ʽ������ȷ��ֵ���ƽ⣻

        3��������Կ�󣬳�����Ҳ�������ƽ�ֵ�����������ɵĺ��ⲿ����ıȽϿ��Ƿ���ȷ


        ��ʽ��(x+2)/3 +(x%3) +256
        
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


int8_t  guc_secret_key_decode_1;        //�ƽ���Կʱ���ƽ�ֵ,LED1 ��λ
int8_t  guc_secret_key_decode_2;        //�ƽ���Կʱ���ƽ�ֵ,LED2 ʮλ
int8_t  guc_secret_key_decode_3;        //�ƽ���Կʱ���ƽ�ֵ,LED3 ��λ

uint16_t guc_RTC_secret_key;            //���ɵ���Կֵ
uint16_t guc_RTC_secret_decode;         //���ɵ��ƽ���Կֵ




//====��Կδ�ƽ�ʱִ�е���ѭ��====
/***************************************************************************************************************************
��������:  ��Կδ�ƽ�ʱ����ѭ��

����λ��:   ϵͳ��ѭ����------ -----------------ok        
***************************************************************************************************************************/
void secret_key_main_loop(void)
{   
    secret_key_time_op();         //��ʱ����    

    //------------------------------------------------------
    if (bflg_com_allow_tx == 1)   //���������
    {        
        bflg_com_allow_tx = 0;
        //------------------------------
        R_UART0_Start();
        COM_TX_MODE;
        com_tx_init();            //ͨѶ���ͳ�ʼ������
    }
    //----------------------------------
    if (bflg_com_tx_ok == 1)      //������ͳɹ�
    {
        bflg_com_tx_ok = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_allow_rx == 1)    //����������
    {
        bflg_com_allow_rx = 0;
        //------------------------------
        com_rx_init();            //ͨѶ���ճ�ʼ������
        COM_RX_MODE;
        R_UART0_Start();
    }
    //----------------------------------
    if (bflg_com_rx_end == 1)     //������ս���
    {
        bflg_com_rx_end = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_rx_ok == 1)     //������ճɹ�
    {
        bflg_com_rx_ok = 0;
        //------------------------------
        R_UART0_Stop();
        com_rx_data_deal();      //ͨѶ�������ݴ������
    }
    //----------------------------------
    if (bflg_read_key_delaytime == 0)   //�������ֵʱ������
    {
        bflg_read_key_delaytime = 1;
        gss_read_key_delaytimer = 0;
        
        secret_key_read_key_deal();     //����ֵ�������
    }
    
    //----------------------------------
    if(bflg_RTC_continuous_r_ok == 1)// ��ȡʱ�����
    {   
        bflg_RTC_continuous_r_ok = 0;
        secret_key_generate();        //������Կֵ���ƽ���Կֵ
    }
    //----------------------------------
    RTC_continuous_r();             //��ȡ������ʱ����
    secret_key_decode_deal();       //����������ƽ���Կ
}

/**********************************************************************************************************************************
��������: ��Կδ�ƽ�ʱ�Ķ�ʱ����

����λ��:   secret_key_main_loop()�� -----------------ok        
**********************************************************************************************************************************/
void secret_key_time_op(void)
{
    if (bflg_1ms_reach == 1)  //���1ms��ʱ��
    {
        bflg_1ms_reach = 0;
        guc_100ms_timer--;
        //--------------------------------------------------
        //1ms��ʱ���ó���
        //--------------------------------------------------
        com_rx_delaytime();     //ͨѶ������ʱ����
        
        com_tx_delaytime();     //ͨѶ������ʱ����
        
        com_rx_end_delaytime(); //ͨѶ���������ʱ����

        RTC_write_read_enable();
    }
    //------------------------------------------------------
    if (bflg_10ms_reach == 1)
    {
        bflg_10ms_reach = 0;
        guc_1s_timer--;
        //--------------------------------------------------
        //10ms��ʱ���ó���
        
        secret_key_led_disp1_deal(); //�����1��ʾ�������
        read_key_delaytime();        //������ʱ����
    }
    //------------------------------------------------------
    if (guc_100ms_timer == 0)
    {
        guc_100ms_timer = 100;
        //--------------------------------------------------
        //100ms��ʱ���ó���
        
        led_disp1_flicker_delaytime();  // �������˸
    }
    //------------------------------------------------------
    if (guc_1s_timer == 0)
    {
        guc_1s_timer = 100;
        guc_1min_timer--;
        //--------------------------------------------------
        //1s��ʱ���ó���
        //--------------------------------------------------
    }
    //------------------------------------------------------
    if (guc_1min_timer == 0)
    {
        guc_1min_timer = 60;
        //--------------------------------------------------
        //1min��ʱ���ó���
    }
}



/***************************����Ϊ��Կδ�ƽ�ʱ�Ĵ�����*******************************/

/******************************************************************************************************************************
��������: ��Կδ�ƽ�ʱ�������1����ʾ

����λ��: ��Կ��ѭ����---------------------ok       
******************************************************************************************************************************/
void secret_key_led_disp1_deal(void)
{
    uint16_t gul_secret_key_sum;

    gul_secret_key_sum = guc_secret_key_decode_1*100 +
                         guc_secret_key_decode_2*10  +
                         guc_secret_key_decode_3;
    
   
    if(bflg_disp1_flicker == 1)    //��˸
    {
        LED2 = guc_nmb_table[guc_secret_key_decode_1];  //��ʾ����
        LED3 = guc_nmb_table[guc_secret_key_decode_2];  //��ʾ����
        LED4 = guc_nmb_table[guc_secret_key_decode_3];  //��ʾ����
    }
    else
    {
        if(bflg_secret_key_1 == 0) //��λ_����δȷ��ʱҪ��
        {
            LED2 = data_all_off;   //��Ϊʵ��ֻ��3λ���������LED1δ��
        }
        else
        {
            LED2 = guc_nmb_table[guc_secret_key_decode_1];  //��ʾ����
        }

        if(bflg_secret_key_2 == 0) //ʮλ
        {
            LED3 = data_all_off; 
        }
        else
        {
            LED3 = guc_nmb_table[guc_secret_key_decode_2]; 
        }

        if(bflg_secret_key_3 == 0) //��λ
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
    LED9 = 0; LED10 = 0; LED11 = 0; LED12 = 0;   //�����ĵƶ���
    //guc_com_tx_buffer[20] = 0x00;              //Ҫ����Ҫ�� com_tx_init()��
}


/******************************************************************************************************************************
��������: ��Կδ����ʱ�İ�������

����λ��: ��Կ��ѭ����---------------------ok       
******************************************************************************************************************************/
void secret_key_read_key_deal(void)
{
    if (guc_key_val == KEY_SET)         //��������ü���ȷ���������Կ�ƽ�ֵ 
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
            bflg_secret_key_ok = 1;         //������Կ�ƽ����
        }
    }
      
    else if (guc_key_val == KEY_UP)             
    {   
        if(bflg_secret_key_1 == 0)            //��λ
        {
            guc_secret_key_decode_1++;     
            if(guc_secret_key_decode_1 >= 10)
            {
                guc_secret_key_decode_1 = 9;
            }
        }
        else if(bflg_secret_key_2 == 0)      //ʮλ
        {
            guc_secret_key_decode_2++;
            if(guc_secret_key_decode_2 >= 10)
            {
                guc_secret_key_decode_2 = 9;
            }
        }
        else if(bflg_secret_key_3 == 0)     //��λ
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
��������: �ƽ���Կֵ������ɺ󣬼��������ƽ���Կֵ�Ƿ���ȷ

����λ��: ��Կ��ѭ����---------------------ok      
******************************************************************************************************************************/
void secret_key_decode_deal(void)
{
    if(bflg_secret_key_ok == 1)    //�ƽ���Կ�������
    {
        bflg_secret_key_ok = 0;
        
        //�ƽ���Կ������ȷ
        if(guc_RTC_secret_decode == ((uint16_t)guc_secret_key_decode_1*100 +  
                                     (uint16_t)guc_secret_key_decode_2*10  +
                                     (uint16_t)guc_secret_key_decode_3))
        {
            bflg_secret_key_1 = 0;
            bflg_secret_key_2 = 0;
            bflg_secret_key_3 = 0;
            
            gss_factory_parm_value[DISP_FACTORY_LL] = 0;                                      //��Ϊ0
             
            ram_para[num_secret_key] = gss_factory_parm_value[DISP_FACTORY_LL];               //����                                                          //!!!!!!!
            eeprom_write(num_secret_key, ram_para[num_secret_key]); 

            
            //bflg_RTC_continuous_r_start = 1;                         //�ƽ�ɹ����ֶ�ʱ��
        }
        else
        {
            //�ȴ�
        }
    } 
}


/******************************************************************************************************************************************
��������:1�� ��ȡ��Կ��ԭʼ���ݣ�����˿̵�ʱ�����00:00:00�ܹ����˶����룬
             ��ԿֵΪ���������ĺ͵ĺ�3λ��

         2�� ������Կ�ƽ�ֵ�ķ�ʽ:��Կֵ����2��1024���ٳ���2�� ��Χ 512~761                 

����λ��: ��Կ����ѭ����---------------------------------------ok
*******************************************************************************************************************************************/
void secret_key_generate(void)
{
    uint32_t gul_secret_key_temp = 0; 

    
    //-----------------------------------------------------------------------
    //��Կֵ�����ɷ�ʽ:���е������͵ĺ�3λ ,  // 23:59:59 ���86399
    gul_secret_key_temp =    ((uint32_t)guc_RTC_r_hour*60*60 + 
                              (uint32_t)guc_RTC_r_minute*60  + 
                              (uint32_t)guc_RTC_r_second);        
                                     
    
    gul_secret_key_temp %= 10000;
    gul_secret_key_temp %= 1000;  

    guc_RTC_secret_key = (uint16_t) gul_secret_key_temp;             //��Կֵ ��Χ 0~999

    

    //---------------------------------------------------------------------------------
    //��Կ�ƽ�ֵ�����ɷ�ʽ: ��Χ 256~591
    guc_RTC_secret_decode = ((guc_RTC_secret_key +2)/ 3 +(guc_RTC_secret_key%3)) +256 ;  //�ƽ���Կֵ

    
    //----------------------------------------------------------------------------------
    //����Կֵ�ֱ�ֵ��Ҫ������ƽ���Կֵ����Ϊ�˰�������ʱ�ƽ�ֵ����ʾ����Կֵ��ʼ����
    guc_secret_key_decode_1 = (uint8_t)(guc_RTC_secret_key/100);      //��Ӧ��λ 
    guc_secret_key_decode_2 = (uint8_t)((guc_RTC_secret_key%100)/10); //��Ӧʮλ
    guc_secret_key_decode_3 = (uint8_t)((guc_RTC_secret_key%100)%10); //��Ӧ��λ
}












/**********************************END OF THE FILE*****************************************************************************/
