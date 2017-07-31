/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.00.00.07 [22 Feb 2013]
* Device(s)    : R5F100PG
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 2017/2/21
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
/* Start user code for include. Do not edit comment generated here */
#include "h_type_define.h"
#include "h_main.h"
#include "h_key_operation.h"
#include "m_secret.h"
//debug
#include "M_RTC.h"

/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
    system_init();  //系统初始化程序
    
    system_start(); //系统启动程序
///* 
           //debug 单独写入一个年或月或...
           if(bflg_RTC_allow_write == 0)
           {
                guc_RTC_ADDR   =  CMD_W_S;
                
                guc_RTC_WDATA  =  0x1e;    //打开 30s,s的最高为表示使能/失能晶振
                //guc_RTC_WDATA  =  0x9e;  //关闭 30s
                
                bflg_RTC_allow_write = 1;
                R_TAU0_Channel4_Start(); 
           }
//*/
          
/*
            //debug 单独读出一个年或月或...
            if(bflg_RTC_allow_read == 0)
            {
                bflg_RTC_allow_read = 1;
                guc_RTC_ADDR  = CMD_R_Y;
                R_TAU0_Channel4_Start(); 
            }
*/

   
/*
    //debug  连续写年月日时分秒

    bflg_RTC_continuous_w_start = 1;
    guc_RTC_w_year   = 17;
    guc_RTC_w_month  = 3;
    guc_RTC_w_day    = 21;
    guc_RTC_w_hour   = 17;
    guc_RTC_w_minute = 26;
    guc_RTC_w_second = 12;
*/    

    //连续读年月日时分秒
    bflg_RTC_continuous_r_start = 1;  //生成密钥用
    
    while (1U)
    {    
    
        if(gss_factory_parm_value[DISP_FACTORY_LL] == 1)       //未破解密钥
        {   
            secret_key_main_loop();
            R_WDT_Restart();
        }
        else                          //密钥破解成功
        {
            main_loop();         
            R_WDT_Restart();
        }
     
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
