/****************************************************************************************************************************************
overview：20170519
        1、电池。。。
        
*****************************************************************************************************************************************/



#ifndef _BAT_CTRL_C_
#define _BAT_CTRL_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_protect.h"
#include "h_bat_ctrl.h"

//------------------------------------------------------------------------------
//函数声明
void bat_charge_deal(void);   //电池充电处理程序，在主循环程序中调用

void bat_charge_delaytime(void);   //电池充电延时程序，在1s定时程序中调用

void bat_charge_continue_delaytime(void);    //充电程序延时程序，在1min定时程序中调用

void bat_charge_space_delaytime(void);  //电池充电间隔延时程序，在1min定时程序中调用

void bat_discharge_deal(void);     //电池放电处理程序，在主循环程序中调用

void bat_discharge_delaytime(void);

void bat_type_check(void);

//------------------------------------------------------------------------------
//变量定义
flag_type flg_bat;

int16_t   gss_bat_charge_delaytimer;    //电池充电延时计时器

int16_t   gss_bat_pwm_value;  //电池充电pwm占空比值
int16_t   gss_bat_pwm_cnt;    //电池充电pwm占空比计数器

int16_t   gss_charge_continue_delaytimer;    //电池充电持续延时计时器

uint16_t  gus_bat_charge_space_delaytimer;   //电池充电间隔延时计时器

uint8_t   gss_discharge_delaytime; 





/********************************************************************************************************************
函数功能: 铅酸电池、锂电池的充电管理；
          充电时对应的口输出PWM--------铅酸最大占空比 25%
                                       锂电最大占空比 31%

函数位置: 主循环(上电1min后判断)----受update影响也是每10s检测一次----------------------ok          
********************************************************************************************************************/
void bat_charge_deal(void)    
{
    //电池插反，12v故障，电池未连接、电池温度过高---停止充电
    if ((bflg_alarm_bat_fed == 1) || (bflg_alarm_power_fault == 1) ||(bflg_alarm_bat_discnnt == 1)||
        (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight == 1)) //(bflg_alarm_power_off == 1) 
    {   
        bflg_bat_charging = 0;
        gss_bat_pwm_value = 0;

        //bflg_bat_charging_end = 0;             //下次可能再上电会在第二阶段
        //bflg_bat_charge_space_delaytime = 0;   //下次上电会接着上次继续计时连续充电时间36/7 
        //gus_bat_charge_space_delaytimer = 0;
        
        //bflg_prot2_bat_low_delaytime = 1;      //24小时后检测电池电量低  
        //gss_prot2_bat_low_delaytimer = 0;
    }
    else
    {
        if (bflg_bat_frist_charge == 1) //如果是首次电池充电
        {
            bflg_bat_frist_charge = 0;  //清首次电池充电标志
            
            bflg_bat_charging = 1;      //电池充电启动
            bflg_bat_charging_end = 0;  //充电的另一个阶段
            gss_bat_pwm_value = 0;
            
            bflg_bat_charge_space_delaytime = 0;   //连续36/7h停止=1，30天后继续充
            gus_bat_charge_space_delaytimer = 0;
            
            bflg_prot2_bat_low_delaytime = 1;      //为了在充电时不检测电池电量低
            gss_prot2_bat_low_delaytimer = 0;
        }
        else
        {
            if (bflg_bat_type == 0)             //如果是铅酸电池
            {
                if (bflg_bat_charging == 1)     //如果当前在充电
                {
                    if (bflg_bat_charging_end == 0)
                    {
                        if (guc_bat_Vout < ram_para2[num_bat1_charge_V1])     //13.5V//第一阶段
                        {
                            if (gss_bat_I > ram_para2[num_bat1_charge_I1])    //610mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat1_charge_I2])    //590mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat1_charge_V2])     //14.3V
                        {
                            if (gss_bat_I > ram_para2[num_bat1_charge_I3])         //270mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat1_charge_I4])    //250mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else
                        {
                            bflg_bat_charging_end = 1;
                        }
                    }
                    else    //如果是充电结束阶段
                    {
                        if (guc_bat_Vout > ram_para2[num_bat1_charge_V4])       //13.9V
                        {
                            gss_bat_pwm_value--;
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat1_charge_V5])  //13.7V
                        {
                            gss_bat_pwm_value++;
                        }
                    }
                }
                else   //如果当前未充电
                {
                    if (guc_bat_Vout < ram_para2[num_bat1_charge_V3])    //12.4V
                    {
                        bflg_bat_charging = 1;  //电池充电启动
                        bflg_bat_charging_end = 0;
                        gss_bat_pwm_value = 0;
                        
                        bflg_bat_charge_space_delaytime = 0;
                        gus_bat_charge_space_delaytimer = 0;
                        
                        bflg_prot2_bat_low_delaytime = 1;          
                        gss_prot2_bat_low_delaytimer = 0;          
                    } 
                    else
                    {
                        if (bflg_bat_charge_space_delaytime == 0)  //peak 间隔30天到
                        {
                            bflg_bat_charging = 1;                 //电池充电启动
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                            
                            bflg_prot2_bat_low_delaytime = 1;   
                            gss_prot2_bat_low_delaytimer = 0;
                        }
                        else
                        {
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                        }
                    }
                }
                //------------------------------
                if (gss_bat_pwm_value < 0)
                {
                    gss_bat_pwm_value = 0;
                }
                else if (gss_bat_pwm_value > 200)      //铅酸最大占空比 200/800 = 25%
                {
                    gss_bat_pwm_value = 200;           //debug 改为400试试
                }
            }
            else  //如果是锂电池
            {
                if (bflg_bat_charging == 1)     //如果当前在充电
                {
                    if (bflg_bat_charging_end == 0)
                    {
                        if (guc_bat_Vout < ram_para2[num_bat2_charge_V1])     //12.0V
                        {
                            if (gss_bat_I > ram_para2[num_bat2_charge_I1])    //710mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat2_charge_I2])    //690mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat2_charge_V2])     //12.4V
                        {
                            if (gss_bat_I > ram_para2[num_bat2_charge_I3])         //330mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat2_charge_I4])    //310mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else
                        {
                            bflg_bat_charging_end = 1;
                        }
                    }
                    else    //如果是充电结束阶段
                    {
                        if (guc_bat_Vout > ram_para2[num_bat2_charge_V4])       //12.9V
                        {
                            gss_bat_pwm_value--;
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat2_charge_V5])  //12.7V
                        {
                            gss_bat_pwm_value++;
                        }
                    }
                }
                else   //如果当前未充电
                {
                    if (guc_bat_Vout < ram_para2[num_bat2_charge_V3])    //11.2V
                    {
                        bflg_bat_charging = 1;                            //电池充电启动
                        bflg_bat_charging_end = 0;
                        gss_bat_pwm_value = 0;
                        
                        bflg_bat_charge_space_delaytime = 0;
                        gus_bat_charge_space_delaytimer = 0;
                        
                        bflg_prot2_bat_low_delaytime = 1;      
                        gss_prot2_bat_low_delaytimer = 0;
                    }
                    else
                    {
                        if (bflg_bat_charge_space_delaytime == 0)
                        {
                            bflg_bat_charging = 1;   //电池充电启动
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                            
                            bflg_prot2_bat_low_delaytime = 1;   
                            gss_prot2_bat_low_delaytimer = 0;
                        }
                        else
                        {
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                        }
                    }
                }
                //------------------------------
                if (gss_bat_pwm_value < 0)
                {
                    gss_bat_pwm_value = 0;
                }
                else if (gss_bat_pwm_value > 250)  //锂电池最大占空比 250/800 = 31%
                {
                    gss_bat_pwm_value = 250;
                }
            }
        }
    }
}

/**************************************************************************************************************************
函数功能: 每间隔10s 对电池充电进行的状况进行一次更新，修改充电的占空比；
          
函数位置: 1s定时器中---------------------------------------------ok          
***************************************************************************************************************************/
void bat_charge_delaytime(void)    
{
    gss_bat_charge_delaytimer++;
    if (gss_bat_charge_delaytimer >= 10)     //10s
    {
        gss_bat_charge_delaytimer = 0;
        
        bflg_bat_charge_update = 1;
    }
}

/*******************************************************************************************************************************
函数功能: 铅酸电池连续充电36个小时后停止充电，开始计时30天后再充电的间隔延时
          锂电池连续充电7小时后后停止充电，开始计时30天后再充电的间隔延时
          
函数位置：1min  -------------------------------------------------ok
*******************************************************************************************************************************/
void bat_charge_continue_delaytime(void) 
{
    if (bflg_bat_charging == 1)
    {
        gss_charge_continue_delaytimer++;
        if (bflg_bat_type == 0)            //铅酸电池
        {
            if (gss_charge_continue_delaytimer >= (ram_para2[num_bat1_charge_continue_delaytime] * 60))   //36h *60 = min
            {
                gss_charge_continue_delaytimer = 0;
                
                bflg_bat_charging = 0;
                bflg_bat_charge_space_delaytime = 1;   //=1 开始计时30天
                gus_bat_charge_space_delaytimer = 0;
            }
        }
        else                               //锂电池
        {
            if (gss_charge_continue_delaytimer >= (ram_para2[num_bat2_charge_continue_delaytime] * 60))     //7h *60 =min
            {
                gss_charge_continue_delaytimer = 0;
                
                bflg_bat_charging = 0;
                bflg_bat_charge_space_delaytime = 1;
                gus_bat_charge_space_delaytimer = 0;
            }
        }
    }
}

/*******************************************************************************************************************************
函数功能: 电池充完一次电，计时30天的延时；30天后再进行充电

函数位置：1min------------------------------------------------ok
*******************************************************************************************************************************/
void bat_charge_space_delaytime(void)    
{
    if (bflg_bat_charge_space_delaytime == 1)
    {
        gus_bat_charge_space_delaytimer++;
        if (bflg_bat_type == 0)    //铅酸电池
        {
            if (gus_bat_charge_space_delaytimer >= (ram_para2[num_bat1_charge_sapce_delaytime] * 24 * 60))    //30天延时到
            {
                gus_bat_charge_space_delaytimer = 0;
                bflg_bat_charge_space_delaytime = 0;
            }
        }
        else                       //锂电池
        {
            if (gus_bat_charge_space_delaytimer >= (ram_para2[num_bat2_charge_sapce_delaytime] * 24 * 60))    //30天延时到
            {
                gus_bat_charge_space_delaytimer = 0;
                bflg_bat_charge_space_delaytime = 0;
            }
        }
    }
}

/*********************************************************************************************************************************
函数功能: 电池放电处理(电池对外供电口开，但不一定耗电，因为有硬件12v监控电路)

函数位置； 10ms定时中(上电3s后再对此函数调用)---------------------------------------------ok          
*********************************************************************************************************************************/
void bat_discharge_deal(void)   
{
    //电池插反、电池温度过高禁止放电
    if((bflg_alarm_bat_fed == 1) || (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight ==1))                             
    {
        bflg_bat_discharging = 0;
    }     
    else if ((bflg_alarm_power_off == 1) || (bflg_alarm_power_fault == 1)) //如果220v断电、12v无
    {
        if (bflg_bat_type == 0)         //铅酸电池
        {           
            if (guc_bat_Vout <= ram_para2[num_bat1_discharge_off_V])   //10.5V
            {
                bflg_bat_discharging = 0;      
            }
            else
            {   
                bflg_bat_discharging = 1;
            }
        }
        else                            //锂电池
        {
            if (guc_bat_Vout <= ram_para2[num_bat2_discharge_off_V])   //9.0V
            {
                bflg_bat_discharging = 0;   
            }
            else
            {
                bflg_bat_discharging = 1;
            }
        }
    }
    else
    {        
        bflg_bat_discharging = 1;
    }
}

/********************************************************************************************************************************
函数功能: 初始上电延时3s后进行电池放电的判断，防止闪烁;
          之前闪烁的处理：1、电池控制端口初始配置为关闭
                          2、上电延时3s后再处理放电的函数
                          
函数位置: 1s定时器--------------------------------------ok
**********************************************************************************************************************************/
void bat_discharge_delaytime(void)
{
   if(bflg_discharge_delaytime == 1)
    {
         gss_discharge_delaytime++;
        if(gss_discharge_delaytime > 3)
        {
            gss_discharge_delaytime = 0;
            bflg_discharge_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************
函数功能: 电池类型的自动检测，根据是否有温度传感器来判断是铅酸电池还是锂电池:0铅酸 1锂电
          (锂电池有温度传感器，铅酸电池没有温度传感器)
                          
函数位置: 主循环--------------------------------------ok
**********************************************************************************************************************************/
void bat_type_check(void)
{
    if((bflg_bat_1_type == LI_BAT) || (bflg_bat_2_type == LI_BAT)) //CN22 CN23任意一个口接锂电池则认为是锂电池
    {
        bflg_bat_type = LI_BAT;         //1锂电
    }
    else
    {
        bflg_bat_type = LEAD_ACID_BAT;  //0铅酸
    }
}



#endif
/*************************************END OF THE FILE*************************************************/
