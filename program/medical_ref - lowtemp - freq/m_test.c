/************************************************************************************************************************************************
overview:
         1、测试模式时执行的操作
            1)主控板通讯口的测试
              主板给测试显示板发送固定数据，显示板回，判断接收的是否正确
            2)三色灯
            3)输入的IO口(通过外接的开关控制)
            4)输出的IO口(外部接的灯是否亮)
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

//函数声明
void test_mode_com_deal(void);
void test_mode_LED_deal(void);     
void test_mode_output_deal(void);  
void test_mode_input_deal(void);   
void test_mode_RTC_circuit(void);

//宏定义
#define   NET_RX_MODE    (P0.4 = 0)     //接收模式-组网模块
#define   NET_TX_MODE    (P0.4 = 1)     //发送模式

//定义变量
uint8_t   guc_test_mode_code;           //测试故障代码

uint8_t   guc_test_uart1_buf;           //串口1接收的一个字节
uint8_t   guc_test_uart2_buf;           //串口2...
uint8_t   guc_test_uart3_buf;           //串口3...
uint8_t   guc_test_uart1_data;          //串口1接收错误的累计
uint8_t   guc_test_uart2_data;          //串口2...
uint8_t   guc_test_uart3_data;          //串口3...

/***********************************************************************************************************************************************
函数功能: 测试主板各个通讯口

函数位置: 主循环(测试的位置)-------------------------OK
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
函数功能:  测试模式三色灯处理程序

函数位置: 1s定时器中-------------------------OK
************************************************************************************************************************************************/
void test_mode_LED_deal(void)     
{
    if (bflg_test_mode == 1)
    {
        if (BLUE_LED_PIN == 0)    //蓝
        {
            BLUE_LED_PIN = 1;     
            GREEN_LED_PIN = 1;         
            RED_LED_PIN = 0;
        }
        else if (RED_LED_PIN == 0)  //红
        {
            BLUE_LED_PIN = 1;     
            GREEN_LED_PIN = 0;         
            RED_LED_PIN = 1;
    
        }
        else if (GREEN_LED_PIN == 0)  //绿
        {
            BLUE_LED_PIN = 0;     
            GREEN_LED_PIN = 1;         
            RED_LED_PIN = 1;
    
        }
    }
}
/***********************************************************************************************************************************************
函数功能:  测试IO口的输入/输出(通过检测输入的电平，确定是否要输出)

函数位置: 主循环-------------------------OK
************************************************************************************************************************************************/
void test_mode_output_deal(void)  
{
    //-----------------------------------------------------
    //电池放电是一直打开的，为了之后验证电池端口检测
    BAT_DISCHARGE_PIN = 1;

    //-----------------------------------------------------
    //拨码开关的测试
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
        LOCK_OUT_PIN = 0;    //ok CN16 2个灯 
    }
    else
    {
        LOCK_OUT_PIN = 1;
    }
    
    //------------------------
    if (DOOR1_IN_PIN == 1)     //cn4   P5.2   检测输入信号
    {
        RL_ALM_OUT_PIN = 0;    //cn28  P6.5   输出信号外连接灯
    }
    else
    {
        RL_ALM_OUT_PIN = 1;
    }
}
/***********************************************************************************************************************************************
函数功能: 测试AD采集口接收的数据是否是预期值(外部连有固定的电阻)

函数位置: 主循环-------------------------OK
************************************************************************************************************************************************/
void test_mode_input_deal(void)    
{
    if ((gus_ad_buffer[0] < 130) || (gus_ad_buffer[0] > 190))
    {
        guc_test_mode_code = 1;    //THW故障
    }
    else if ((gus_ad_buffer[1] < 130) || (gus_ad_buffer[1] > 190))
    {
        guc_test_mode_code = 2;    //TE1故障
    }
    else if ((gus_ad_buffer[2] < 130) || (gus_ad_buffer[2] > 190))
    {
        guc_test_mode_code = 3;    //TC故障
    }
    else if ((gus_ad_buffer[3] < 130) || (gus_ad_buffer[3] > 190))
    {
        guc_test_mode_code = 4;    //TE2故障
    }
    else if ((guc_12V < 115) || (guc_12V > 125))
    {
        guc_test_mode_code = 5;    //12V故障
    }
    else if (gus_ad_buffer[6] < 1000 )     // else if ((gus_ad_buffer[6] < 500) || (gus_ad_buffer[6] > 520))
    {
        guc_test_mode_code = 6;    //CT故障  电流检测 5v =1023
    }
    //else if ((guc_bat_Vin < 110) || (guc_bat_Vin > 120))
    //{
        //guc_test_mode_code = 7;    //BAT-V故障
    //}
    else if ((gus_ad_buffer[9] < 660) || (gus_ad_buffer[9] > 700))//else if ((gus_ad_buffer[9] < 660) || (gus_ad_buffer[9] > 680))
    {
        guc_test_mode_code = 8;    //电池2温度故障
    }
    else if ((gus_ad_buffer[10] < 660) && (gus_ad_buffer[10] > 700))// else if ((gus_ad_buffer[10] < 660) && (gus_ad_buffer[10] > 680))
    {
        guc_test_mode_code = 9;    //电池1温度故障
    }
    //else if ((guc_bat_Vout < 110) || (guc_bat_Vout > 120))
    //{
        //guc_test_mode_code = 10;   //电池充电电压故障
    //}
    

    //之前注释的
    /*else if (gss_adg_Temp[0] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 11;   //热电偶1故障
    }
    else if (gss_adg_Temp[1] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 12;   //热电偶2故障
    }
    else if (gss_adg_Temp[2] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 13;   //热电偶3故障
    }
    else if (gss_adg_Temp[3] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 14;   //热电偶4故障
    }
    else if (gss_adg_Temp[4] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 15;   //热电偶5故障
    }
    else if (gss_adg_Temp[5] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 16;   //热电偶6故障
    }
    else if (gss_adg_Temp[6] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 17;   //热电偶7故障
    }
    else if (gss_adg_Temp[7] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 18;   //热电偶8故障
    }
    else if (gss_adg_Temp[8] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 19;   //热电偶9故障
    }
    else if (gss_adg_Temp[9] != (gss_adg_Temp2 + 1))
    {
        guc_test_mode_code = 20;   //热电偶10故障
    }*/
   
    //else if (guc_test_uart1_data >= 10)
    //{
    	  //guc_test_mode_code = 21;   //组网通讯故障
    //}

    
    else if (guc_test_uart2_data >= 10)
    {
    	  guc_test_mode_code = 22;   //冷链通讯故障
    }
    
    else if (guc_test_uart3_data >= 10)
    {
    	  guc_test_mode_code = 23;   //打印机通讯故障   peak 因没有线暂时屏蔽不测了
    }
    else
    {
    	  guc_test_mode_code = 0;    //无故障
    }
}

/***********************************************************************************************************************************************
函数功能: 测试RTC时钟电路，按下buzz键写入年2017，然后LED1显示2017

函数位置: 主循环-------------------------ok
************************************************************************************************************************************************/
void test_mode_RTC_circuit(void)
{
	//debug 单独写入一个年或月或...
	guc_RTC_ADDR   =  CMD_W_Y;
	guc_RTC_WDATA  =  0x17;     
	bflg_RTC_enable_write = 1;
}






/*********************************END OF THE FILE*********************************************************************/

