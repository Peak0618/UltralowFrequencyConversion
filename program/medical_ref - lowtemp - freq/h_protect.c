/********************************************************************************************************************************
overview；
        1、对各种报警成立或是恢复的检测

        2、高温报警
        3、低温报警
        4、220断电
        5、12v坏
        6、电池未连接
        7、电池电量低
        8、开门报警
        9、所有传感器故障
        10、环温过高
        11、冷凝器脏
        12、交流电电压超标
        13、3位报警灯
        14、蜂鸣器报警
        15、电池温度过高
*********************************************************************************************************************************/

#ifndef _PROTECT_C_
#define _PROTECT_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_key_operation.h"
#include "h_protect.h"
#include "h_volt_ctrl.h"
#include "h_bat_ctrl.h"

//peak add
#include "M_buzz.h"
#include "h_main.h"
#include "h_comp2.h"
#include "h_com.h"
//------------------------------------------------------------------------------
//函数声明
void prot2_TA_first_delaytime(void);    //主传感器首次高低温保护延时程序，在1min定时程序中调用

void prot2_TA_high_delaytime(void);     //主传感器高温保护延时程序，在1s定时程序中调用

void prot2_TA_low_delaytime(void);      //主传感器低温保护延时程序，在1s定时程序中调用

void prot2_power_off_deal(void);   //断电保护处理程序，在主循环程序中调用

void port2_power_fault_deal(void); //电源板故障处理程序，在主循环程序中调用

void prot2_bat_discnnt_delaytime(void); //电池未连接保护延时程序，在100ms定时程序中调用

void prot2_bat_space_delaytime(void);   //电池未连接检测间隔延时程序，在100ms定时程序中调用

void prot2_bat_low_deal(void);     //电池电量低保护处理程序，在主循环程序中调用

void prot2_bat_low_delaytime(void);//电池电量低延迟程序，在1min定时程序中调用

void port2_door_delaytime(void);   //门开关保护延时程序，在1s定时程序中调用

void port2_T_fault_deal(void);     //传感器故障处理程序，在主循环程序中调用

void prot2_THW_high_delaytime(void);    //环温传感器过高保护延时程序，在1s定时程序中调用

void prot2_TC_first_delaytime(void);    //冷凝器脏首次保护延时程序，在1min定时程序中调用

void prot2_cond_dirty_delaytime(void);  //冷凝器脏保护延时程序，在1s定时程序中调用

void prot2_volt_deal(void);   //电压超标报警保护处理程序，在主循环程序中调用

void alarm_lamp_deal(void);   //报警灯处理程序，在主循环程序中调用

void alarm_buzz_deal(void);   //蜂鸣器报警处理程序，在主循环程序中调用

void alarm_buzz_off_delaytime(void);    //蜂鸣器报警取消延时程序，在1min定时程序中调用

void bat_T_hight_protect(void);             //电池温度过高判断

void alarm_power_off(void);


//------------------------------------------------------------------------------
//变量定义
flag_type flg_alarm,flg_alarm1,flg_alarm2,flg_prot2;

//peak add
flag_type   memory;

//------------------------------------------------------------------------------
int16_t   gss_prot2_TA_high_delaytimer; //主传感器高温保护延时计时器
int16_t   gss_prot2_TA_low_delaytimer;  //主传感器低温保护延时计时器
int16_t   gss_prot2_TA_first_delaytimer;//主传感器首次保护延时计时器

int16_t   gss_prot2_bat_discnnt_delaytimer;  //电池未连接保护延时计时器
int16_t   gss_prot2_bat_space_delaytimer;    //电池未连接检测间隔延时计时器
int16_t   gss_prot2_bat_low_delaytimer;      //电池电量低延时计时器

int16_t   gss_prot2_door_delaytimer;    //门开关保护延时计时器

int16_t   gss_prot2_THW_high_delaytimer;     //环温传感器过高保护延时计时器

int16_t   gss_prot2_TC_first_delaytimer;//冷凝器脏首次保护延时计时器
int16_t   gss_prot2_cond_dirty_delaytimer;   //冷凝器脏延时计时器

int16_t   gss_alarm_buzz_off_delaytimer;     //蜂鸣器报警取消延时计时器

uint16_t  gus_trip_code1;     //故障代码1
uint16_t  gus_trip_code2;     //故障代码2

uint8_t   guc_power_down_lfreq_cnt;       //断电后上电低压机已4500启动
uint8_t   guc_power_down_hdelay_cnt;      //断电后上电高压机延时1min启动
int16_t   gss_power_off_alarm_delaytime;  //断电后蜂鸣器报警延时





/**********************************************************************************************************************************
函数功能：主传感器(PT100)的高低温报警，需延时3h后判断  
                   
函数位置：1min------------------------------------------------------ok        
**********************************************************************************************************************************/
void prot2_TA_first_delaytime(void)     
{
    if (bflg_prot2_TA_first_delaytime == 1)
    {
        gss_prot2_TA_first_delaytimer++;
        if (gss_prot2_TA_first_delaytimer >= ram_para[num_prot2_TA_first_delaytime])  //180min = 3h
        {
            gss_prot2_TA_first_delaytimer = 0;
            bflg_prot2_TA_first_delaytime = 0;
        }
    }
}

/*************************************************************************************************************************************
函数功能 : 主传感器高温报警的成立和恢复

函数位置：1s定时器-----------------------------------ok
**************************************************************************************************************************************/
void prot2_TA_high_delaytime(void)
{
    if(bflg_TA_fault == 0)   //主传感器未坏才报高温报警
    {
        if (bflg_alarm_ALH == 0)
        {
            if (gss_TA >= gss_set_parm_value[DISP_SET_ALH])       
            {
                gss_prot2_TA_high_delaytimer++;
                if (gss_prot2_TA_high_delaytimer >= ram_para2[num_prot2_TA_high_delaytime])   //600s=10min
                {
                    gss_prot2_TA_high_delaytimer = 0;
                    bflg_alarm_ALH = 1;
                    gus_trip_code1 |= 0x0001;    //peak 此故障代码是液晶屏协议上表示的
                }
            }
            else
            {
                gss_prot2_TA_high_delaytimer = 0;
            }
        }
        else                                      
        {
            //if (gss_TA < gss_set_parm_value[DISP_SET_ALH] + ram_para2[num_prot2_TA_high_Tdelt])   //-20
            if (gss_TA < gss_set_parm_value[DISP_SET_ALH] )   
            {
                gss_prot2_TA_high_delaytimer++;
                if (gss_prot2_TA_high_delaytimer >= ram_para2[num_prot2_TA_high_delaytime])   //600s
                {
                    gss_prot2_TA_high_delaytimer = 0;
                    bflg_alarm_ALH = 0;
                    gus_trip_code1 &= ~0x0001;
                }
            }
            else
            {
                gss_prot2_TA_high_delaytimer = 0;
            }
        }
    }
    else
    {
        gss_prot2_TA_high_delaytimer = 0;
    }
}

/*******************************************************************************************************************************
函数功能： 主传感器低温报警的成立和恢复

函数位置：1s------------------------------ok
*******************************************************************************************************************************/
void prot2_TA_low_delaytime(void) 
{
    if(bflg_TA_fault == 0)   //主传感器未坏时才报低温报警
    {
        if (bflg_alarm_ALL == 0)
        {
            if (gss_TA <= gss_set_parm_value[DISP_SET_ALL])
            {
                gss_prot2_TA_low_delaytimer++;
                if (gss_prot2_TA_low_delaytimer >= ram_para2[num_prot2_TA_low_delaytime])     //600s =10min
                {
                    gss_prot2_TA_low_delaytimer = 0;
                    bflg_alarm_ALL = 1;
                    gus_trip_code1 |= 0x0002;
                }
            }
            else
            {
                gss_prot2_TA_low_delaytimer = 0;
            }
        }
        else
        {
            //if (gss_TA > gss_set_parm_value[DISP_SET_ALL] + ram_para2[num_prot2_TA_low_Tdelt])    // 20
            if (gss_TA > gss_set_parm_value[DISP_SET_ALL])   
            {
                gss_prot2_TA_low_delaytimer++;
                if (gss_prot2_TA_low_delaytimer >= ram_para2[num_prot2_TA_low_delaytime])    //600s = 10min
                {
                    gss_prot2_TA_low_delaytimer = 0;
                    bflg_alarm_ALL = 0;
                    gus_trip_code1 &= ~0x0002;
                }
            }
            else
            {
                gss_prot2_TA_low_delaytimer = 0;
            }
        }
    }
    else
    {
        gss_prot2_TA_low_delaytimer = 0;
    }
}



/***************************************************************************************************************************
函数功能: 断电报警的成立和恢复、及其他断电后又重新上电后的处理；

函数位置： 主循环-------------------------------ok
***************************************************************************************************************************/
void prot2_power_off_deal(void) 
{
    if (gss_VAC <= ram_para2[num_power_fautl_VAC1])       //80V  电源断电
    {
        bflg_alarm_power_off = 1;
        gus_trip_code1 |= VAC_OFF_CODE;

        guc_power_down_lfreq_cnt = 1;      //断电
        guc_power_down_hdelay_cnt = 1;
        guc_comp2_power_on_again = 1;
    }
    else if (gss_VAC >= ram_para2[num_power_fautl_VAC2])   //90V  电源正常
    {
        bflg_alarm_power_off = 0;
        gus_trip_code1 &= ~VAC_OFF_CODE;

        if( guc_power_down_lfreq_cnt == 1) //重新上电低压机以4500启动
        {
            guc_power_down_lfreq_cnt = 2;
        }
        
        //----------------------------------
        if(guc_power_down_hdelay_cnt == 1) //高压机延时1min
        {
            guc_power_down_hdelay_cnt = 0;
            
            bflg_power_down_delaytime = 1;
            gss_power_down_delaytime = 0;
        }
        
        //----------------------------------
        if(guc_comp2_power_on_again == 1)
        {
            guc_comp2_power_on_again = 2;
        }
    }
}

/************************************************************************************************************************************
函数功能: 电源板(12v)故障的成立和恢复
          ————达到报警值，显示板也不进行报警指示了；

函数位置:   主循环-----------------------------ok
************************************************************************************************************************************/
void port2_power_fault_deal(void)     
{
    if (guc_12V < ram_para2[num_power_off_12V])  //9v
    {
        /*if (bflg_alarm_power_off == 0)           //未断强电  --原     
        {
            bflg_alarm_power_fault = 1;
            gus_trip_code1 |= 0x4000;
        }
        else
        {
            bflg_alarm_power_fault = 0;
            gus_trip_code1 &= ~0x4000;
        }*/
        
        bflg_alarm_power_fault = 1;
        gus_trip_code1 |= DC_F_CODE; 
    }
    else if (guc_12V > ram_para2[num_power_on_12V]) //11v
    {
        bflg_alarm_power_fault = 0;
        gus_trip_code1 &= ~DC_F_CODE;
    }
}

/******************************************************************************************************************************
函数功能: 电池未连接故障的成立和恢复

函数位置：100ms--------------------------------------------------ok
*********************************************************************************************************************************************/
void prot2_bat_discnnt_delaytime(void)   
{
    if(bflg_alarm_bat_fed == 0)              //电池未接反时
    {
        if (bflg_bat_charging == 0)              //非充电状态
        {
            if (bflg_alarm_bat_discnnt == 0)     
            {
                if (guc_bat_Vout_instant < ram_para2[num_port2_bat_discnnt_V1])   //20 = 2v
                {
                    bflg_alarm_bat_discnnt = 1;
                    gus_trip_code1 |= BAT_NO_CODE; 
                }
            }
            else   
            {
                if (guc_bat_Vout_instant > ram_para2[num_port2_bat_discnnt_V2])   //50 = 5V
                {
                    bflg_alarm_bat_discnnt = 0;
                    gus_trip_code1 &= ~BAT_NO_CODE;
                }
            }
        }
        else   //充电状态
        {
            if (bflg_alarm_bat_discnnt == 0)      
            {
                //if (guc_bat_Vout_instant < 90)//ram_para2[num_port2_bat_discnnt_V1]) //peak 功能书为 9v
                if (guc_bat_Vout_instant < 90)       
                {
                    bflg_alarm_bat_discnnt = 1;
                    gus_trip_code1 |= BAT_NO_CODE;
                }
                if(bflg_bat_type == LEAD_ACID_BAT)      //铅酸标准，锂电待+      
                {
                    if (guc_bat_Vout_instant > 151)      
                    {
                        bflg_alarm_bat_discnnt = 1;
                        gus_trip_code1 |= BAT_NO_CODE;
                    }
                }
                
            }
            else    
            {
                if (guc_bat_Vout_instant > 100)//ram_para2[num_port2_bat_discnnt_V2])       // 10v
                {
                    bflg_alarm_bat_discnnt = 0;
                    gus_trip_code1 &= ~BAT_NO_CODE;
                }
            }
        }    
    }
    else
    {
        bflg_alarm_bat_discnnt = 0;
        gus_trip_code1 &= ~BAT_NO_CODE;    
    }


}


/*****************************************************************************************************************************
function: 电池未连接检测间隔延时程序----未用

函数位置：100ms-----------------------------ok
******************************************************************************************************************************/
void prot2_bat_space_delaytime(void)
{
    if (bflg_prot2_bat_space_delaytime == 1)
    {
        gss_prot2_bat_space_delaytimer++;
        if (gss_prot2_bat_space_delaytimer >= 200) //5s
        {
            gss_prot2_bat_space_delaytimer = 0;
            bflg_prot2_bat_space_delaytime = 0;
            
            //bflg_prot2_bat_space_reach = 1;
        }
    }
    else
    {
        gss_prot2_bat_space_delaytimer++;
        if (gss_prot2_bat_space_delaytimer >= 50) //5s
        {
            gss_prot2_bat_space_delaytimer = 0;
            bflg_prot2_bat_space_delaytime = 1;
            
            //bflg_prot2_bat_space_reach = 1;
        }
    }
}

/******************************************************************************************************************************
函数功能: 电池电量低故障的成立和恢复

恢复条件:1、电池未连接，2、电压>阀值  (3、在充电时不能检测已被去掉 )

函数位置：主循环-----------------------------------ok
******************************************************************************************************************************/
void prot2_bat_low_deal(void)       
{    
    if ((bflg_prot2_bat_low_delaytime == 0) && (bflg_alarm_bat_discnnt == 0))   //充电24小时后，且电池已连接
	{  
        if (bflg_bat_type == 0)    //如果是铅酸电池
        {
            if (guc_bat_Vout < ram_para2[num_bat_low_V1])       //10.5V
            {
                bflg_alarm_bat_low = 1;
                gus_trip_code1 |= BAT_L_CODE; 
            }
            else if (guc_bat_Vout > ram_para2[num_bat_low_V2])   //11.0V
            {
                bflg_alarm_bat_low = 0;
                gus_trip_code1 &= ~BAT_L_CODE;
            }
        }
        else   //如果是锂电池
        {
            if (guc_bat_Vout < ram_para2[num_bat_low_V3])       //9.0V
            {
                bflg_alarm_bat_low = 1;
                gus_trip_code1 |= BAT_L_CODE;
            }
            else if (guc_bat_Vout > ram_para2[num_bat_low_V4])   //9.5V
            {
                bflg_alarm_bat_low = 0;
                gus_trip_code1 &= ~BAT_L_CODE;
            }
        }
    }
    else
    {
        bflg_alarm_bat_low = 0;
        gus_trip_code1 &= ~BAT_L_CODE;
    }
}

/********************************************************************************************************************************
函数功能: 初始上电充电24小时后再对电池电量低进行检测
         
函数位置： 1min-------------------ok         
********************************************************************************************************************************/
void prot2_bat_low_delaytime(void) //电池电量低延迟程序，在1min定时程序中调用
{
    if (bflg_prot2_bat_low_delaytime == 1)
    {
        gss_prot2_bat_low_delaytimer++;
        if (gss_prot2_bat_low_delaytimer >= (ram_para2[num_prot2_bat_low_delaytime] * 60)) //24小时
        {
            gss_prot2_bat_low_delaytimer = 0;
            bflg_prot2_bat_low_delaytime = 0;
        }
    }
}

/*****************************************************************************************************************************
function: 开门报警的成立和恢复，持续检测开门5min则报开门报警

函数位置： 1s---------------------------------ok
*****************************************************************************************************************************/
void port2_door_delaytime(void)    
{
    if ((bflg_prot2_door1_pin == 1))     //开门
    {
        if (bflg_alarm_open_door == 0)
        {
            gss_prot2_door_delaytimer++;
            //if (gss_prot2_door_delaytimer >= (ram_para[num_prot2_door_delaytime] * 60))  //5分钟
            if (gss_prot2_door_delaytimer >= (gss_user_parm_value[DISP_USER_dA] * 60))        //5分钟 用可调的gss_user_parm_value[DISP_USER_dA]替换之前的固定值
            {
                gss_prot2_door_delaytimer = 0;
                
                bflg_alarm_open_door = 1; //开门报警
                gus_trip_code1 |= DOOR_OP_CODE;
            }
        }
    }
    else
    {
        gss_prot2_door_delaytimer = 0;
        bflg_alarm_open_door = 0;
        gus_trip_code1 &= ~DOOR_OP_CODE;
    }
}

/******************************************************************************************************************************
函数功能: 环温、热交换、冷凝、pt100传感器故障都统一称为 bflg_alarm_T_fault 故障;

函数位置：主循环---------------------------------ok
*******************************************************************************************************************************/
void port2_T_fault_deal(void)       
{
    if (bflg_THW_fault == 1)    // 环温报警
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TE_fault == 1) // 热交换      
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TC_fault == 1) // 冷凝器
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TA_fault == 1) // 主传感器pt100
    {
        bflg_alarm_T_fault = 1;
    }
    else
    {
        bflg_alarm_T_fault = 0;
    }
}

/*************************************************************************************************************************
函数功能: 环温过高报警的成立和恢复
         
函数位置：1s----------------------------------------ok
**************************************************************************************************************************/
void prot2_THW_high_delaytime(void)  
{
    if (bflg_alarm_THW_high == 0)
    {
        //if (gss_THW > ram_para[num_prot2_THW_T])     //35原来的       
        if (gss_THW > gss_factory_parm_value[DISP_FACTORY_AA])                  //可调默认35
        {
            gss_prot2_THW_high_delaytimer++;
            if (gss_prot2_THW_high_delaytimer >= ram_para2[num_prot2_THW_high_delaytime]) //30s
            {
                gss_prot2_THW_high_delaytimer = 0;
                bflg_alarm_THW_high = 1;
                gus_trip_code1 |= THW_H_CODE;
            }
        }
        else
        {
            gss_prot2_THW_high_delaytimer = 0;
        }
    }
    else
    {
        //if (gss_THW <= (ram_para[num_prot2_THW_T] + ram_para2[num_prot2_THW_Tdelt]))     //34原来的
        if (gss_THW <= (gss_factory_parm_value[DISP_FACTORY_AA] + ram_para2[num_prot2_THW_Tdelt]))  //-1            //可调
        {
            gss_prot2_THW_high_delaytimer++;
            if (gss_prot2_THW_high_delaytimer >= ram_para2[num_prot2_THW_high_delaytime]) //30
            {
                gss_prot2_THW_high_delaytimer = 0;
                bflg_alarm_THW_high = 0;
                gus_trip_code1 &= ~THW_H_CODE;
            }
        }
        else
        {
            gss_prot2_THW_high_delaytimer = 0;
        }
    }
}


/************************************************************************************************************************
函数功能: 上电2h后再进行判断冷凝器脏报警，冷凝器脏首次保护延时；

函数位置：1min--------------------------------------ok
************************************************************************************************************************/
void prot2_TC_first_delaytime(void)     
{
    if (bflg_prot2_TC_first_delaytime == 1)
    {
        gss_prot2_TC_first_delaytimer++;
        if (gss_prot2_TC_first_delaytimer >= ram_para2[num_prot2_TC_first_delaytime]) //120min = 2h    //120
        {
            gss_prot2_TC_first_delaytimer = 0;
            bflg_prot2_TC_first_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************
函数功能: 冷凝器脏报警的成立和恢复

函数位置：1s-----------------------------------------------ok
********************************************************************************************************************************/
void prot2_cond_dirty_delaytime(void) 
{
    if (bflg_prot2_TC_first_delaytime == 0)          //初始上电已延时2小时
    {
        if (bflg_alarm_cond_dirty == 0)              
        {
            if ((gss_TC - gss_THW) >= ram_para2[num_prot2_cond_dirty_TC1])   //13℃
            {
                gss_prot2_cond_dirty_delaytimer++;
                if (gss_prot2_cond_dirty_delaytimer >= ram_para2[num_prot2_cond_dirty_delaytime])   //300s = 5min
                {
                    gss_prot2_cond_dirty_delaytimer = 0;
                    
                    bflg_alarm_cond_dirty = 1;
                    gus_trip_code1 |= TC_COND_CODE;
                }
            }
            else                                    
            {
                gss_prot2_cond_dirty_delaytimer = 0;
            }
        }
        else                                          //有报警    
        {
            if ((gss_TC - gss_THW) < ram_para2[num_prot2_cond_dirty_TC2])    //10℃   
            {
                gss_prot2_cond_dirty_delaytimer++;
                if (gss_prot2_cond_dirty_delaytimer >= ram_para2[num_prot2_cond_dirty_delaytime])   //300s
                {
                    gss_prot2_cond_dirty_delaytimer = 0;
                    
                    bflg_alarm_cond_dirty = 0;
                    gus_trip_code1 &= ~TC_COND_CODE;
                }
            }
            else
            {
                gss_prot2_cond_dirty_delaytimer = 0;
            }
        }
    }
}

/*****************************************************************************************************************************
函数功能: (交流电)电压超标报警的成立和恢复
           ————达到报警值，显示板也不进行报警指示了；

函数位置：主循环----------------------------实际未用--------------------ok
******************************************************************************************************************************/
void prot2_volt_deal(void) 
{
    if(bflg_alarm_power_off == 0)           //未断电时判
    {
        if (bflg_comp2_volt_type == 0) //如果是220V电源
        {
            if ((gss_VAC <= ram_para2[num_prot2_volt_V1]) || (gss_VAC >= ram_para2[num_prot2_volt_V2]))     //184V    253V
            {
                bflg_alarm_volt = 1;
                gus_trip_code1 |= VAC_OVER_CODE;  
            }
            else if ((gss_VAC >= ram_para2[num_prot2_volt_V3]) && (gss_VAC <= ram_para2[num_prot2_volt_V4])) //194V   243V
            {
                bflg_alarm_volt = 0;
                gus_trip_code1 &= ~VAC_OVER_CODE;
            }
        }
        else                          //如果是115V电源
        {
            if ((gss_VAC <= ram_para2[num_prot2_volt_V5]) || (gss_VAC >= ram_para2[num_prot2_volt_V6]))        //93V   136V
            {
                bflg_alarm_volt = 1;
                gus_trip_code1 |= VAC_OVER_CODE;
            }
            else if ((gss_VAC >= ram_para2[num_prot2_volt_V7]) && (gss_VAC <= ram_para2[num_prot2_volt_V8]))   //98V   131V
            {
                bflg_alarm_volt = 0;
                gus_trip_code1 &= ~VAC_OVER_CODE;
            }
        }
    }
    else
    {
        bflg_alarm_volt = 0;
        gus_trip_code1 &= ~VAC_OVER_CODE;
    }
}

/*********************************************************************************************************************
函数位置：显示板上面的那3个灯的处理，只要有任意一种报警，显示板上的3位报警灯都会常亮；
          
          
函数位置：主循环----------------------------------------------ok          
*********************************************************************************************************************/
void alarm_lamp_deal(void)              
{
    if (((flg_alarm.uword & 0xff) != 0)||     //有报警标志为1的
        ((flg_alarm1.uword & 0xff) != 0))
    {        
        bflg_alarm_lamp = 1;
    }
    else
    {
        bflg_alarm_lamp = 0;
    }
}

/******************************************************************************************************************************
函数功能: 蜂鸣器报警的处理，根据报警的类型确定是滴滴响，还是长响；

函数位置；主循环－－－－－－－－－－－ok
******************************************************************************************************************************/
void alarm_buzz_deal(void)     
{   
    flag_type memory_temp;      

    if((bflg_test_mode == 0) && (bflg_com_type == 0))          //非测试模式 && LED屏    
    {
        if(bflg_alarm_power_off == 0)  //无断电报警
        {
            memory_temp.uword = memory.uword;  //memory_temp存储上一次的故障 
                                               //flg_alarm 最新的故障，用来判断蜂鸣器滴滴响  
            memory.uword= flg_alarm.uword;     //memory 将flg_alarm最新的故障赋给它值，用来和上次的存储的故障值 memory_temp对比，判断是否有新的故障变化，若按下'蜂鸣取消'键，则可以清30内不响

            //--------------------------------------------------------------------------------       
            if((flg_alarm.uword  == 0)||                                      //没有报警
            (bflg_alarm_buzz_off_delaytime == 1)||                            //有报警，但按下"蜂鸣取消"
            ((flg_alarm.uword == 0x0008 )&&(bflg_fac_bS_buz_valid == 1)))     //只有电池未连接报警，且设置电池未连接屏蔽参数bS = 1)
            {                                                                   
                bflg_buzz_loop_tick_set = 0;                                  //以上3种情况不滴滴          

                if(memory_temp.uword != memory.uword)                         //有新的报警时，清按"蜂鸣取消"30min不响，又开始响
                {
                    bflg_alarm_buzz_off_delaytime = 0;
                    gss_alarm_buzz_off_delaytimer = 0;
                }
            }           
            else
            {
                bflg_buzz_loop_tick_set = 1;                                    //其他情况滴滴响
            }
        }
        else
        {
            bflg_buzz_loop_tick_set = 0;
            //bflg_alarm_buzz_off_delaytime = 0; //若断电前有报警按下buzz，在上电要重新报警则开此
            //gss_alarm_buzz_off_delaytimer = 0;

            if(bflg_power_off_alarm == 1)
            {
                bflg_power_off_alarm = 0;
                bflg_buzz_one_tick_set = 1;
            }
        }


        //------------------------------------------------------------------------------------
        //电池插反
        if (bflg_alarm_bat_fed == 1)               //peak 电池接反报警标志 6
        {
            bflg_buzz_always_sound_set = 1;        //peak 蜂鸣器长响
        }
        else
        {
            bflg_buzz_always_sound_set = 0;
        }	
    }
    else
    {
        bflg_buzz_always_sound_set = 0;
        bflg_buzz_loop_tick_set = 0;                                
    }
}

/*******************************************************************************************************************************
函数位置:滴滴响时，按下buzz键，停止30min响的延时；            
            
函数位置:1min定时程序中调用      -------------------------------ok        
********************************************************************************************************************************/
void alarm_buzz_off_delaytime(void)     
{
    if (bflg_alarm_buzz_off_delaytime == 1)
    {
        gss_alarm_buzz_off_delaytimer++;
        //if (gss_alarm_buzz_off_delaytimer >= ram_para[num_alarm_buzz_off_delaytime])     //30分钟
        if (gss_alarm_buzz_off_delaytimer >= gss_factory_parm_value[DISP_FACTORY_Fd])                               //改为可调
        {
            gss_alarm_buzz_off_delaytimer = 0;
            bflg_alarm_buzz_off_delaytime = 0;
        }
    }
}


/***********************************************************************************************************************************
函数功能 : 电池温度过高报警判断 

函数位置: 主循环中-----------------------------------------------ok
***********************************************************************************************************************************/
void bat_T_hight_protect(void)
{
    //bat1_CN22
    if (gss_Tbat1 >= ram_para2[num_bat2_discharge_off_T])       //65℃   电池1 、2都用这个标准
    {
        bflg_alarm_bat1_T_hight = 1;
    }
    else if (gss_Tbat1 <= ram_para2[num_bat2_discharge_on_T])   //55℃
    {
        bflg_alarm_bat1_T_hight = 0;
    }

    //bat2_CN23
    //--------------------------------------------------------------------
    if (gss_Tbat2 >= ram_para2[num_bat2_discharge_off_T])       //65℃
    {
        bflg_alarm_bat2_T_hight = 1;
    }
    else if (gss_Tbat2 <= ram_para2[num_bat2_discharge_on_T])   //55℃
    {
        bflg_alarm_bat2_T_hight = 0;
    }
}

/***********************************************************************************************************************************
函数功能 : 断电时蜂鸣器每间隔3s响一声

函数位置: 1s----------------------------------------------ok
***********************************************************************************************************************************/
void alarm_power_off(void)
{
    if(bflg_alarm_power_off == 1)  //断电
    {
        if(bflg_power_off_alarm == 0)
        {
            gss_power_off_alarm_delaytime++;
            if(gss_power_off_alarm_delaytime >= 3)
            {
                gss_power_off_alarm_delaytime = 0;
                bflg_power_off_alarm = 1;
            }
        }
    }
    else
    {
        bflg_power_off_alarm = 0;
        gss_power_off_alarm_delaytime = 0;
        
    }
}

//------------------------------------------------------------------------------
#endif
