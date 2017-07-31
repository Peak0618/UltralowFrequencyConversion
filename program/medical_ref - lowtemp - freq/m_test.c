/************************************************************************************************************************************************
overview:
         1������ģʽʱִ�еĲ���
            1)���ذ�ͨѶ�ڵĲ���
              �����������ʾ�巢�͹̶����ݣ���ʾ��أ��жϽ��յ��Ƿ���ȷ
            2)��ɫ��
            3)�����IO��(ͨ����ӵĿ��ؿ���)
            4)�����IO��(�ⲿ�ӵĵ��Ƿ���)
*************************************************************************************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"

#include "h_type_define.h"
#include "h_com.h"
#include "h_main.h"
#include "h_ad.h"
#include "M_RTC.H"
#include "m_peripheral.h"
#include "m_test.h"

//��������
void test_mode_com_deal(void);
void test_mode_LED_deal(void);     
void test_mode_output_deal(void);  
void test_mode_input_deal(void);   
void test_mode_RTC_circuit(void);

//�궨��
#define   NET_RX_MODE    (P0.4 = 0)     //����ģʽ-����ģ��
#define   NET_TX_MODE    (P0.4 = 1)     //����ģʽ

//�������
uint8_t   guc_test_mode_code;           //���Թ��ϴ���

uint8_t   guc_test_uart1_buf;           //����1���յ�һ���ֽ�
uint8_t   guc_test_uart2_buf;           //����2...
uint8_t   guc_test_uart3_buf;           //����3...
uint8_t   guc_test_uart1_data;          //����1���մ�����ۼ�
uint8_t   guc_test_uart2_data;          //����2...
uint8_t   guc_test_uart3_data;          //����3...

/***********************************************************************************************************************************************
��������: �����������ͨѶ��

����λ��: ��ѭ��(���Ե�λ��)-------------------------OK
************************************************************************************************************************************************/
void test_mode_com_deal(void)      
{
    if (bflg_com_allow_tx == 1)
    {
        R_UART1_Start();
        NET_TX_MODE;
        TXD1 = 0xA3; 
        //--------------------
        R_UART2_Start();
        TXD2 = 0xA1;
        //--------------------
        R_UART3_Start();
        TXD3 = 0xA2;
    }
    //----------------------------------
    if (bflg_com_tx_ok == 1)
    {
        R_UART1_Stop();
        R_UART2_Stop();
        R_UART3_Stop();
    }
    //----------------------------------
    if (bflg_com_allow_rx == 1)
    {
        NET_RX_MODE;
        R_UART1_Start();
        //--------------------
        R_UART2_Start();
        //--------------------
        R_UART3_Start();
    }
    //----------------------------------
    if (bflg_com_rx_ok == 1)
    {
        R_UART1_Stop();
        R_UART2_Stop();
        R_UART3_Stop();
        //------------------------------
        if (guc_test_uart1_buf != 0x3A)
        {
        	  guc_test_uart1_data++;
        	  if (guc_test_uart1_data >= 20)
        	  {
        	  	  guc_test_uart1_data = 20;
        	  }
        }
        else
        {
        	  guc_test_uart1_data = 0;
        }
        //------------------------------
        if (guc_test_uart2_buf != 0x1A)
        {
        	  guc_test_uart2_data++;
        	  if (guc_test_uart2_data >= 20)
        	  {
        	  	  guc_test_uart2_data = 20;
        	  }
        }
        else
        {
        	  guc_test_uart2_data = 0;
        }
        //------------------------------
        if (guc_test_uart3_buf != 0x2A)
        {
        	  guc_test_uart3_data++;
        	  if (guc_test_uart3_data >= 20)
        	  {
        	  	  guc_test_uart3_data = 20;
        	  }
        }
        else
        {
        	  guc_test_uart3_data = 0;
        }
        //------------------------------
        //guc_test_uart1_data = guc_test_uart1_buf;
        //guc_test_uart2_data = guc_test_uart2_buf;
        //guc_test_uart3_data = guc_test_uart3_buf;
        
        //guc_test_uart1_buf = 0;
        //guc_test_uart2_buf = 0;
        //guc_test_uart3_buf = 0;
    }
}

/***********************************************************************************************************************************************
��������:  ����ģʽ��ɫ�ƴ������

����λ��: 1s��ʱ����-------------------------OK
************************************************************************************************************************************************/
void test_mode_LED_deal(void)     
{
    if (bflg_test_mode == 1)
    {
        if (BLUE_LED_PIN == 0)    //��
        {
            BLUE_LED_PIN = 1;     
            GREEN_LED_PIN = 1;         
            RED_LED_PIN = 0;
        }
        else if (RED_LED_PIN == 0)  //��
        {
            BLUE_LED_PIN = 1;     
            GREEN_LED_PIN = 0;         
            RED_LED_PIN = 1;
    
        }
        else if (GREEN_LED_PIN == 0)  //��
        {
            BLUE_LED_PIN = 0;     
            GREEN_LED_PIN = 1;         
            RED_LED_PIN = 1;
    
        }
    }
}
/***********************************************************************************************************************************************
��������:  ����IO�ڵ�����/���(ͨ���������ĵ�ƽ��ȷ���Ƿ�Ҫ���)

����λ��: ��ѭ��-------------------------OK
************************************************************************************************************************************************/
void test_mode_output_deal(void)  
{
    //-----------------------------------------------------
    //��طŵ���һֱ�򿪵ģ�Ϊ��֮����֤��ض˿ڼ��
    BAT_DISCHARGE_PIN = 1;

    //-----------------------------------------------------
    //���뿪�صĲ���
    if (SW24_IN_PIN == 1)         //sw2 OK
    {
        RL_HTRMXG_OUT_PIN = 0;
    }
    else
    {
        RL_HTRMXG_OUT_PIN = 1;
    }
    //------------------------
    if (SW23_IN_PIN == 1)        //OK
    {
        RL_FAN1_OUT_PIN = 0;
    }
    else
    {
        RL_FAN1_OUT_PIN = 1;
    }
    //------------------------
    if (SW22_IN_PIN == 1)       //OK
    {
        RL_FAN2_OUT_PIN = 0;
    }
    else
    {
        RL_FAN2_OUT_PIN = 1;
    }
    //------------------------
    if (SW21_IN_PIN == 1)       //OK
    {
        RL_HTRGK_OUT_PIN = 0;
    }
    else
    {
        RL_HTRGK_OUT_PIN = 1;
    }

    
    //------------------------  sw1
    if (SW18_IN_PIN == 1)           // P8.3
    {
        RL_COMP1_OUT_PIN = 0;       // P7.4   
    }
    else
    {
        RL_COMP1_OUT_PIN = 1;
    }
    //------------------------
    if (SW17_IN_PIN == 1)
    {
        RL_COMP2_OUT_PIN = 0;
    }
    else
    {
        RL_COMP2_OUT_PIN = 1;
    }
    //------------------------
    if (SW16_IN_PIN == 1)
    {
        RL_DOWN_OUT_PIN = 0;
    }
    else
    {
        RL_DOWN_OUT_PIN = 1;   //P15.6 = 1;
    }
    //------------------------
    if (SW15_IN_PIN == 1)
    {
        RL_UP_OUT_PIN = 0;
    }
    else
    {
       RL_UP_OUT_PIN = 1;//P10.0 = 1;   //P15.6 = 1;
      
    }
    //------------------------
    if (SW14_IN_PIN == 1)    //ok
    {
        HTRPHK_OUT_PIN = 0;
    }
    else
    {
        HTRPHK_OUT_PIN = 1;
    }
    //------------------------
    if (SW13_IN_PIN == 1)    //ok
    {
        HTRMXG_OUT_PIN = 0;
    }
    else
    {
        HTRMXG_OUT_PIN = 1;
    }
    //------------------------
    if (SW12_IN_PIN == 1)     //ok
    {
        HTRMT_OUT_PIN = 0;    
    }
    else
    {
        HTRMT_OUT_PIN = 1;
    }
    //------------------------
    if (SW11_IN_PIN == 1)
    {
        LOCK_OUT_PIN = 0;    //ok CN16 2���� 
    }
    else
    {
        LOCK_OUT_PIN = 1;
    }
    
    //------------------------
    if (DOOR1_IN_PIN == 1)     //cn4   P5.2   ��������ź�
    {
        RL_ALM_OUT_PIN = 0;    //cn28  P6.5   ����ź������ӵ�
    }
    else
    {
        RL_ALM_OUT_PIN = 1;
    }
}
/***********************************************************************************************************************************************
��������: ����AD�ɼ��ڽ��յ������Ƿ���Ԥ��ֵ(�ⲿ���й̶��ĵ���)

����λ��: ��ѭ��-------------------------OK
************************************************************************************************************************************************/
void test_mode_input_deal(void)    
{
    if ((gus_ad_buffer[0] < 130) || (gus_ad_buffer[0] > 190))
    {
        guc_test_mode_code = 1;    //THW����
    }
    else if ((gus_ad_buffer[1] < 130) || (gus_ad_buffer[1] > 190))
    {
        guc_test_mode_code = 2;    //TE1����
    }
    else if ((gus_ad_buffer[2] < 130) || (gus_ad_buffer[2] > 190))
    {
        guc_test_mode_code = 3;    //TC����
    }
    else if ((gus_ad_buffer[3] < 130) || (gus_ad_buffer[3] > 190))
    {
        guc_test_mode_code = 4;    //TE2����
    }
    else if ((guc_12V < 115) || (guc_12V > 125))
    {
        guc_test_mode_code = 5;    //12V����
    }
    else if (gus_ad_buffer[6] < 1000 )     // else if ((gus_ad_buffer[6] < 500) || (gus_ad_buffer[6] > 520))
    {
        guc_test_mode_code = 6;    //CT����  ������� 5v =1023
    }
    //else if ((guc_bat_Vin < 110) || (guc_bat_Vin > 120))
    //{
        //guc_test_mode_code = 7;    //BAT-V����
    //}
    else if ((gus_ad_buffer[9] < 660) || (gus_ad_buffer[9] > 700))//else if ((gus_ad_buffer[9] < 660) || (gus_ad_buffer[9] > 680))
    {
        guc_test_mode_code = 8;    //���2�¶ȹ���
    }
    else if ((gus_ad_buffer[10] < 660) && (gus_ad_buffer[10] > 700))// else if ((gus_ad_buffer[10] < 660) && (gus_ad_buffer[10] > 680))
    {
        guc_test_mode_code = 9;    //���1�¶ȹ���
    }
    //else if ((guc_bat_Vout < 110) || (guc_bat_Vout > 120))
    //{
        //guc_test_mode_code = 10;   //��س���ѹ����
    //}
    

    //֮ǰע�͵�
    /*else if (gss_adg_Temp[0] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 11;   //�ȵ�ż1����
    }
    else if (gss_adg_Temp[1] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 12;   //�ȵ�ż2����
    }
    else if (gss_adg_Temp[2] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 13;   //�ȵ�ż3����
    }
    else if (gss_adg_Temp[3] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 14;   //�ȵ�ż4����
    }
    else if (gss_adg_Temp[4] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 15;   //�ȵ�ż5����
    }
    else if (gss_adg_Temp[5] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 16;   //�ȵ�ż6����
    }
    else if (gss_adg_Temp[6] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 17;   //�ȵ�ż7����
    }
    else if (gss_adg_Temp[7] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 18;   //�ȵ�ż8����
    }
    else if (gss_adg_Temp[8] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 19;   //�ȵ�ż9����
    }
    else if (gss_adg_Temp[9] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 20;   //�ȵ�ż10����
    }*/
   
    //else if (guc_test_uart1_data >= 10)
    //{
    	  //guc_test_mode_code = 21;   //����ͨѶ����
    //}

    
    else if (guc_test_uart2_data >= 10)
    {
    	  guc_test_mode_code = 22;   //����ͨѶ����
    }
    
    else if (guc_test_uart3_data >= 10)
    {
    	  guc_test_mode_code = 23;   //��ӡ��ͨѶ����   peak ��û������ʱ���β�����
    }
    else
    {
    	  guc_test_mode_code = 0;    //�޹���
    }
}

/***********************************************************************************************************************************************
��������: ����RTCʱ�ӵ�·������buzz��д����2017��Ȼ��LED1��ʾ2017

����λ��: ��ѭ��-------------------------ok
************************************************************************************************************************************************/
void test_mode_RTC_circuit(void)
{
	//debug ����д��һ������»�...
	guc_RTC_ADDR   =  CMD_W_Y;
	guc_RTC_WDATA  =  0x17;     
	bflg_RTC_enable_write = 1;
}






/*********************************END OF THE FILE*********************************************************************/

