/****************************************************************************************************************************************
overview: ���崮��1���˿�CN8��485�����ӿ�

            1���˴���Ҫ���ڲ���ʱ������һ����ʾ�壬������ʾ���¡��������¶ȡ��Ƚ������¶�    
            2���˴��Ĵ���ʽ��485���ߣ����ԽӶ���ӻ�
            3��ͨѶ���ԣ� ����һֱ����������(�Ӷ���ӻ���ѭ����һֱ��)
                          �ӻ�һֱ�գ�������(ֻ���Լ������ݽ��д���)
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



//����
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

uint8_t   guc_net_com_tx_buffer[23];    //���͵�����


//variate
uint8_t guc_net_com_delaytime;          //���ݷ��ͼ��
uint8_t guc_net_slave_add;              //�ӻ���ַ
uint8_t guc_net_com_tx_point;           //������������
uint8_t guc_net_disp_rate;              

//sign
flag_type flg_net_com;

//macro
#define     SLAVE_1_ADD    0                  //�ӻ�1��ַ 
#define     SLAVE_2_ADD    1                  //�ӻ�2��ַ
#define     SLAVE_3_ADD    2                  //�ӻ�3��ַ
#define     SLAVE_SUM      3                  //�ӻ�����

#define     NET_COM_RX_MODE    (P0.4 = 0)     //����ģʽ
#define     NET_COM_TX_MODE    (P0.4 = 1)     //����ģʽ


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
�������ܣ�����485����ѭ�����͸���ͬ�ӻ����ݵ�ʱ����

����λ�ã�1ms��ʱ��--------------------------------ok
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
�������ܣ� �������ݳ�ʼ��

����λ�ã�net_com_deal(��ѭ��)--------------------------------ok
***********************************************************************************************************************************************/
void net_com_tx_init(void)
{
    int8_t i;
    word_type gss_tmp;
    word_type com_crc;

    if(guc_net_slave_add == SLAVE_1_ADD)
    {
        guc_net_com_tx_buffer[0] = SLAVE_1_ADD;     //�豸��ַ
        guc_net_com_tx_buffer[1] = 0x10;            //д����

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //��ʼ��ַ0

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
        guc_net_com_tx_buffer[0] = SLAVE_2_ADD;     //�豸��ַ
        guc_net_com_tx_buffer[1] = 0x10;            //д����

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //��ʼ��ַ0

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
        guc_net_com_tx_buffer[0] = SLAVE_3_ADD;     //�豸��ַ
        guc_net_com_tx_buffer[1] = 0x10;            //д����

        guc_net_com_tx_buffer[2] = 0x00;
        guc_net_com_tx_buffer[3] = 0x00;            //��ʼ��ַ0

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
�������ܣ� ��������

����λ�ã�ϵͳ�����ж�--------------------------------ok
***********************************************************************************************************************************************/
void net_com_tx_int(void)
{
    if (bflg_net_com_tx_busy == 1)
    {
        if (guc_net_com_tx_point < 23)//23
        {
            TXD1 = guc_net_com_tx_buffer[guc_net_com_tx_point++];     //��������
        }
        else
        {
            guc_net_com_tx_point = 0;    //�巢���ֽ�����
            bflg_net_com_tx_busy = 0;    //�巢��æ��־
            bflg_net_com_tx_ok = 1;
        }
    }
}

/*********************************************************************************************************************************************
�������ܣ� �ܵ����ݴ���

����λ�ã���ѭ��--------------------------------ok
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
//��ʾ����_��1���ӻ�
//10ms��ʱ�� --------
void net_led_disp1_deal_slave_1(void)
{
    guc_net_disp_rate = dot_0d1;                 
    net_led1_disp_val_slave_1(guc_bat_Vout);           //��ص�ѹ  __�Ŵ�10��
}
void net_led_disp2_deal_slave_1(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_1(gss_TC);                 //�������¶�
}
void net_led_disp3_deal_slave_1(void)         
{         
    guc_net_disp_rate = not_dot;
    net_led3_disp_val_slave_1(gss_TE);                 //�Ƚ����¶�
}

//==============================================================================================================================
//��ʾ����_��2���ӻ�
//10ms��ʱ�� --------
void net_led_disp1_deal_slave_2(void)
{
    guc_net_disp_rate = not_dot;
    net_led1_disp_val_slave_2(guc_RTC_r_second);        //��
}
void net_led_disp2_deal_slave_2(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_2(gss_comp2_freq / 10);     //��ѹ��ת��
}
void net_led_disp3_deal_slave_2(void)         
{         
    guc_net_disp_rate = not_dot;
    net_led3_disp_val_slave_2(gss_comp21_freq / 10);    //��ѹ��ת��
}

//==============================================================================================================================
//��ʾ����_��3���ӻ�
//10ms��ʱ�� --------
void net_led_disp1_deal_slave_3(void)
{
    guc_net_disp_rate = dot_0d1;
    net_led1_disp_val_slave_3(guc_bat_Vin);             //����ѹ  __�Ŵ�10��
}
void net_led_disp2_deal_slave_3(void)     
{
    guc_net_disp_rate = not_dot;
    net_led2_disp_val_slave_3(gss_bat_I);              //������ 
}
void net_led_disp3_deal_slave_3(void)         
{         
    guc_net_disp_rate = not_dot;                      //���ռ�ձȵ�ֵ(�ܵ�800)
    net_led3_disp_val_slave_3(gss_bat_pwm_value);      
}


//---------------------------------------------------------------------------------------------
void net_led1_disp_val_slave_1(int16_t lss_disp_val)     //�����1��ʾ��ֵ����
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //��ʾ������־   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak �������Ĳ���ת��Ϊ������Ҫ�Ĳ������ŵ�ԭ��
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak ȡ�̣�ȡ����λ��ֵ
    {
        LED1_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak ��ֵ����ʾ
    }
    else
    {
        LED1_SLAVE_1 = data_all_off;                             //peak ûֵ�Ͳ���
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak ȡ�࣬ʣ�µ���ֵ
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_1 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak �ж�LED2��λС�����Ƿ�
        {
            LED2_SLAVE_1 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak �ж�ǰһλ�Ƿ��и���
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
        if (guc_net_disp_rate == dot_0d01)                  //peak �ж�Ϊ0ʱ�Ƿ�С����
        {
            LED2_SLAVE_1 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_1 == data_all_off)                   //peak ǰһλû��ֵ����λΪ0û���壬Ҳ�ص�
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
            LED3_SLAVE_1 |= data_dot;               //peak LED3��С����Ҳ��ʾ��
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
 �м���������Ϊ3λ�����
 
*******************************************************************************/
void net_led2_disp_val_slave_1(int16_t lss_disp_val) //�����2��ʾ��ֵ����
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
 ��������������3λ�����  
*******************************************************************************/
void net_led3_disp_val_slave_1(int16_t lss_disp_val) //�����3��ʾ��ֵ����
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
void net_led1_disp_val_slave_2(int16_t lss_disp_val)     //�����1��ʾ��ֵ����
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //��ʾ������־   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak �������Ĳ���ת��Ϊ������Ҫ�Ĳ������ŵ�ԭ��
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak ȡ�̣�ȡ����λ��ֵ
    {
        LED1_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak ��ֵ����ʾ
    }
    else
    {
        LED1_SLAVE_2 = data_all_off;                             //peak ûֵ�Ͳ���
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak ȡ�࣬ʣ�µ���ֵ
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_2 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak �ж�LED2��λС�����Ƿ�
        {
            LED2_SLAVE_2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak �ж�ǰһλ�Ƿ��и���
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
        if (guc_net_disp_rate == dot_0d01)                  //peak �ж�Ϊ0ʱ�Ƿ�С����
        {
            LED2_SLAVE_2 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_2 == data_all_off)                   //peak ǰһλû��ֵ����λΪ0û���壬Ҳ�ص�
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
            LED3_SLAVE_2 |= data_dot;               //peak LED3��С����Ҳ��ʾ��
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
 �м���������Ϊ3λ�����
 
*******************************************************************************/
void net_led2_disp_val_slave_2(int16_t lss_disp_val) //�����2��ʾ��ֵ����
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
 ��������������3λ�����  
*******************************************************************************/
void net_led3_disp_val_slave_2(int16_t lss_disp_val) //�����3��ʾ��ֵ����
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
void net_led1_disp_val_slave_3(int16_t lss_disp_val)     //�����1��ʾ��ֵ����
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //��ʾ������־   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak �������Ĳ���ת��Ϊ������Ҫ�Ĳ������ŵ�ԭ��
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak ȡ�̣�ȡ����λ��ֵ
    {
        LED1_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak ��ֵ����ʾ
    }
    else
    {
        LED1_SLAVE_3 = data_all_off;                             //peak ûֵ�Ͳ���
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak ȡ�࣬ʣ�µ���ֵ
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2_SLAVE_3 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_net_disp_rate == dot_0d01)                   //peak �ж�LED2��λС�����Ƿ�
        {
            LED2_SLAVE_3 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak �ж�ǰһλ�Ƿ��и���
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
        if (guc_net_disp_rate == dot_0d01)                  //peak �ж�Ϊ0ʱ�Ƿ�С����
        {
            LED2_SLAVE_3 = data_0 | data_dot;
        }
        else
        {
            if (LED1_SLAVE_3 == data_all_off)                   //peak ǰһλû��ֵ����λΪ0û���壬Ҳ�ص�
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
            LED3_SLAVE_3 |= data_dot;               //peak LED3��С����Ҳ��ʾ��
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
 �м���������Ϊ3λ�����
 
*******************************************************************************/
void net_led2_disp_val_slave_3(int16_t lss_disp_val) //�����2��ʾ��ֵ����
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
 ��������������3λ�����  
*******************************************************************************/
void net_led3_disp_val_slave_3(int16_t lss_disp_val) //�����3��ʾ��ֵ����
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

unsigned int net_CRC16(unsigned char *puchmsg, unsigned int usDataLen)    //����CRCУ��ĳ���
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
            
            for (uIndex_y = 0; uIndex_y <= 7; uIndex_y++)   //peak 8��λ
            {
                if ((uchCRC & 0x0001) != 0)                 //peak ���һλΪ1
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
