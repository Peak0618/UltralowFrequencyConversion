/**************************************************************************************************************************************************
overview:
    1、三色灯的控制输出
    2、风机控制
    3、平衡口加热丝
    4、门体加热丝
    5、柜口加热丝
    
    6、毛细管加热丝---未用
***************************************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"

#include "h_type_define.h"
#include "h_protect.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_key_operation.h"
#include "h_bat_ctrl.h"
#include "m_peripheral.h"
#include "h_main.h"
//函数声明
void three_led_deal(void);
void outfan2_ctrl_deal(void)  ;
void heat21_ctrl_delaytime(void);
void heat22_ctrl_deal(void);
void tank_mouth_heat(void);
void heat20_force_ctrl_deal(void);
void heat20_space_delaytime(void);
void heat20_ctrl_deal(void)   ;
void heat20_wait_delaytime(void);
void heat20_comp2_stop_delaytime(void);  
void heat20_run_delaytime(void);
void heat20_stop_delaytime(void);
void output_deal(void);   
void RL_output_deal(void);     
void RL_output_delaytime(void);     
void input_pin_delaytime(void);     
void trip_lampblink_deal(void);   


//flag
flag_type flg_peripheral_1,flg_heat2;

//variate
//风机
int16_t   gss_outfan2_normal_H_delaytimer;    //冷凝风机正常高速开延时计时器
int16_t   gss_outfan2_normal_L_delaytimer;    //冷凝风机正常低速开延时计时器
int16_t   gss_outfan2_fault_H_delaytimer;     //冷凝风机错误高速开延时计时器
int16_t   gss_outfan2_fault_L_delaytimer;     //冷凝风机错误低速开延时计时器

int16_t   gss_heat21_run_delaytimer;          //平衡口加热丝开延时计时器
int16_t   gss_heat21_stop_delaytimer;         //平衡口加热丝关延时计时器
int16_t   gss_heat22_run_delaytimer;          //门体加热丝开延时计时器
int16_t   gss_tank_mouth_run_delaytimer;      //柜口加热丝开延时计时器
uint8_t   guc_RL_output_delaytimer;           //继电器输出延时计时器

uint8_t   guc_door1_pin_high_delaytimer;      //门开关1引脚高延时计时器
uint8_t   guc_door1_pin_low_delaytimer;       //门开关1引脚低延时计时器
uint8_t   guc_bat_fed_pin_high_delaytimer;    //电池防插反引脚高延时计时器
uint8_t   guc_bat_fed_pin_low_delaytimer;     //电池防插反引脚低延时计时器

//加热丝控制相关变量
int16_t   gss_heat20_space_delaytimer;       //毛细管加热丝间隔延时计时器
int16_t   gss_heat20_wait_delaytimer;        //毛细管加热丝等待延时计时器
int16_t   gss_heat20_comp2_stop_delaytimer;  //毛细管加热丝使压缩机关延时计时器
int16_t   gss_heat20_run_delaytimer;         //毛细管加热丝开延时计时器
int16_t   gss_heat20_stop_delaytimer;        //毛细管加热丝关延时计时器

uint8_t   guc_trip_flash_cnt;                //故障灯闪烁计数器
uint8_t   guc_trip_flash_timer;              //故障灯闪烁计时器
uint8_t   guc_trip_code;                     //故障代码




//====三色灯_program====
/****************************************************************************************************************************************************
函数功能：三色灯根据报警的类型显示不同的颜色(低电平为开)

          黄色：红+绿

函数位置：主循环--------------------------------------未调
***************************************************************************************************************************************************/
void three_led_deal(void)
{
    if (bflg_test_mode == 0)
    {
        //如断电、高温及低温、开门红色
        if((bflg_alarm_power_off == 1)||(bflg_alarm_ALH == 1)||(bflg_alarm_ALL == 1)||(bflg_alarm_open_door == 1))
        {
            RED_LED_PIN = 0;
            GREEN_LED_PIN = 1;
            BLUE_LED_PIN = 1;
        }
        //如环温过高报警、传感器类报警、电池类报警、冷凝器脏报警）黄色；
        else if((bflg_alarm_THW_high == 1)||
               (bflg_alarm_T_fault == 1)  ||
               (bflg_alarm_bat_discnnt == 1)||(bflg_alarm_bat_fed == 1)||(bflg_alarm_bat_low == 1)||
               (bflg_alarm_cond_dirty == 1))
        {
            RED_LED_PIN = 0;
            GREEN_LED_PIN = 0;
            BLUE_LED_PIN = 1;
        }
        else
        {
            RED_LED_PIN = 1;
            GREEN_LED_PIN = 0;
            BLUE_LED_PIN = 1;
        }
    }
}




//====风机_pragram====
/*****************************************************************************************************************************
函数功能: 风机的控制
              RL7 ----bflg_outfan20_running
              RL6 ----bflg_outfan21_running
              可调 -- 只是放到E2中，按键暂时不调
              
函数位置:     在1s中断中调用 -------------------------------ok
******************************************************************************************************************************/
void outfan2_ctrl_deal(void)  
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))     //断强电、12v
    {
        bflg_outfan20_running = 0;
        bflg_outfan21_running = 0;
    }
    else
    {
        if(bflg_TC_fault == 0)                          //冷凝传感器正常
        {
            gss_outfan2_fault_H_delaytimer = 0;
            gss_outfan2_fault_L_delaytimer = 0;
            
            if(gss_TC >= ram_para2[num_outfan2_T])      //>=35
            {
                gss_outfan2_normal_L_delaytimer = 0;
                
                gss_outfan2_normal_H_delaytimer++;
                if(gss_outfan2_normal_H_delaytimer >= ram_para2[num_outfan2_delaytime]) //60
                {   
                    gss_outfan2_normal_H_delaytimer = 0;
                    bflg_outfan20_running = 0;
                    bflg_outfan21_running = 1;
                }
            }
            else                                        //<35
            {
                gss_outfan2_normal_H_delaytimer = 0;

                gss_outfan2_normal_L_delaytimer++;
                if(gss_outfan2_normal_L_delaytimer >= ram_para2[num_outfan2_delaytime])
                {
                    gss_outfan2_normal_L_delaytimer = 0;
                    bflg_outfan20_running = 1;
                    bflg_outfan21_running = 1;
                }
            }
        }
        else                                            //冷凝传感器故障
        {
            gss_outfan2_normal_H_delaytimer = 0;
            gss_outfan2_normal_L_delaytimer = 0;

            if(gss_comp21_freq >= ram_para2[num_outfan2_rad])                 //>=3500
            {
                gss_outfan2_fault_L_delaytimer = 0;

                gss_outfan2_fault_H_delaytimer++;
                if(gss_outfan2_fault_H_delaytimer >= ram_para2[num_outfan2_delaytime])
                {
                    gss_outfan2_fault_H_delaytimer = 0;
                    bflg_outfan20_running = 0;
                    bflg_outfan21_running = 1;
                }
            }
            else                                        //<3500
            {
                gss_outfan2_fault_H_delaytimer = 0;

                gss_outfan2_fault_L_delaytimer++;
                if(gss_outfan2_fault_L_delaytimer >= ram_para2[num_outfan2_delaytime])
                {
                    gss_outfan2_fault_L_delaytimer = 0;
                    bflg_outfan20_running = 1;
                    bflg_outfan21_running = 1;
                }
            }       
        }
    }
}





//====平衡口加热丝_program===
/*******************************************************************************
函数功能: 平衡口加热丝控制延时程序
          当主传感器温度≤-35.0℃  持续 5 秒时， 平衡口加热丝加热；
          当主传感器温度＞-35.0℃  持续 5 秒时，平衡口加热丝停止加热。

函数位置:1s定时器------------------------------------ok
********************************************************************************/
void heat21_ctrl_delaytime(void)   
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))  //断强电、12v
    {
        bflg_heat21_running = 0;
    }
    else 
    {
        if (gss_TA <= ram_para2[num_heat21_T])   //-35℃
        {
            gss_heat21_stop_delaytimer = 0;
            
            gss_heat21_run_delaytimer++;
            if (gss_heat21_run_delaytimer >= ram_para2[num_heat21_run_delaytime])   //5秒
            {
                gss_heat21_run_delaytimer = 0;
                
                bflg_heat21_running = 1;
            }
        }
        else
        {
            gss_heat21_run_delaytimer = 0;
            
            gss_heat21_stop_delaytimer++;
            if (gss_heat21_stop_delaytimer >= ram_para2[num_heat21_stop_delaytime]) //5秒
            {
                gss_heat21_stop_delaytimer = 0;
                
                bflg_heat21_running = 0;
            }
        }
    }
}



//====门体加热丝_program====
/***********************************************************************************************************************
新改功能:  门体加热丝控制

        压缩机开机且主传感器温度≤-35.0℃（ EEP）持续 5 秒（ EEP）时，开门体加热
        压缩机关机或主传感器温度＞-35.0℃（ EEP）持续 5 秒（ EEP）时，关门体加热

函数位置:1s定时器--------------------------------------ok        
***********************************************************************************************************************/
void heat22_ctrl_deal(void)     
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))     //断强电、12v
    {
         bflg_heat22_running = 0;
    }
    else
    {
        if (bflg_comp2_running == 1)                     //压机开机
        {
            if(bflg_heat22_running == 0)                 //加热丝关机
            {
                if (gss_TA <= ram_para2[num_heat22_T])   //-35℃
                {                
                    gss_heat22_run_delaytimer++;
                    if (gss_heat22_run_delaytimer >= ram_para2[num_heat22_run_delaytime])   //5秒
                    {
                        gss_heat22_run_delaytimer = 0;                    
                        bflg_heat22_running = 1;
                    }
                }
                else
                {   
                    gss_heat22_run_delaytimer = 0;
                }
            }
            else                           //此时开机
            {            
                if (gss_TA > ram_para2[num_heat22_T])   //-35℃
                {                
                    gss_heat22_run_delaytimer++;
                    if (gss_heat22_run_delaytimer >= ram_para2[num_heat22_run_delaytime])   //5秒
                    {
                        gss_heat22_run_delaytimer = 0;
                        bflg_heat22_running = 0;
                    }
                }
                else
                {
                    gss_heat22_run_delaytimer = 0;
                }
            }
        }
        else                                         //压缩机关机
        {
             bflg_heat22_running = 0;
        }
    }
}




//====柜口加热丝_program====
/*********************************************************************************************************************************
函数功能: 柜口加热丝加热控制

函数位置: 1ms-----------------------------------ok 
*********************************************************************************************************************************/
void tank_mouth_heat(void)
{
    if(bflg_tank_mouth_run == 0)  //此时未运行
    {
        if((gss_factory_parm_value[DISP_FACTORY_HC] == 1) && (bflg_alarm_power_off == 0) && (gss_TA < -600)&&(bflg_alarm_power_fault == 0))
        {
            gss_tank_mouth_run_delaytimer++;
            if(gss_tank_mouth_run_delaytimer >= 5) //5s
            {
                gss_tank_mouth_run_delaytimer = 0;
                bflg_tank_mouth_run = 1;
            }
        }
    }
    else
    {
        if(gss_factory_parm_value[DISP_FACTORY_HC] == 0)  
        {
            bflg_tank_mouth_run = 0;
        }
        else if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))
        {
            bflg_tank_mouth_run = 0;
        }
        else if(gss_TA > -600)
        {
            gss_tank_mouth_run_delaytimer++;
            if(gss_tank_mouth_run_delaytimer >= 5)   //5s
            {
                gss_tank_mouth_run_delaytimer = 0;
                bflg_tank_mouth_run = 0;
            }
        }
    }
}





//====毛细管加热丝_program====
/********************************************************************************************************************************
功能 :有强制(手动)加热的指令目的: 1、设置请求开毛细管标志 --开毛细管
                                  2、置关掉压缩机的标志   --关压机
                                  
函数位置: -------------删除加热丝功能去掉的 ----------------未调                            
*********************************************************************************************************************************/
void heat20_force_ctrl_deal(void)  //毛细管加热丝强制加热控制处理程序，在主循环程序中调用
{
    if (bflg_heat20_force_run == 1)           //有强制加热要求
    {
        bflg_heat20_space_delaytime = 0;
        gss_heat20_space_delaytimer = 0;
        
        bflg_heat20_askfor_run = 1;     //置请求毛细管加热丝开标志
        bflg_heat20_comp2_stop = 1;     //置毛细管加热丝使压缩机关标志
    }
}

/******************************************************************************************************************************
功能: 每间隔 16 小时判断毛细管是否有加热，既毛细管自动加热最少16个小时进行一次加热


函数位置: --------------------删除加热丝功能去掉的------------------未调
*******************************************************************************************************************************/
void heat20_space_delaytime(void)  //毛细管加热丝间隔延时程序，在1分钟延时程序中调用
{
    if (bflg_heat20_space_delaytime == 1)
    {
        //debug
        //gss_heat20_space_delaytimer++;
        if (gss_heat20_space_delaytimer >= ram_para2[num_heat20_space_delaytime])    //960   16小时加热一次
        {
            gss_heat20_space_delaytimer = 0;
            bflg_heat20_space_delaytime = 0;
        }
    }
}

/*******************************************************************************************************************************
功能: 毛细管加热控制  

     间隔16个小时对毛细管进行检测，目的就是让毛细管16小时后必须加热一次

函数位置: ------------------删除加热丝功能去掉的 -----------------未调     
*******************************************************************************************************************************/
void heat20_ctrl_deal(void)   //毛细管加热丝控制处理程序，在主循环程序中调用
{
    if (bflg_heat20_askfor_run == 0)    //如果无请求毛细管加热丝开
    {
        if (bflg_heat20_space_delaytime == 0)      // peak 每隔16小时判断 =0是16小时后了 或初始上电
        {
            if (bflg_heat20_first_run == 1)  //如果首次运行
            {
                bflg_heat20_first_run = 0;
                
                if (gss_TA <= ram_para2[num_heat20_T])   // peak 0度 初始上电， 如果主传感器温度≤0℃， 则加热 6 分钟
                {
                    bflg_heat20_askfor_run = 1;   //置请求毛细管加热丝开标志
                    bflg_heat20_comp2_stop = 1;   //置毛细管加热丝使压缩机关标志
                }
                else
                {
                    bflg_heat20_space_delaytime = 1;
                    gss_heat20_space_delaytimer = 0;
                }
            }
            else
            {
                bflg_heat20_askfor_run = 1;  //置请求毛细管加热丝开标志
                
                bflg_heat20_wait_delaytime = 1;  //peak 压缩机一直未停机，则一小时后 置 bflg_heat20_comp2_stop = 1;  关闭压缩机打开毛细管 
                gss_heat20_wait_delaytimer = 0;
            }
        }
    }
    else  //如果请求毛细管加热丝开  //peak   bflg_heat20_askfor_run == 1--->bflg_heat20_comp2_stop = 1(下数8 line)
                                    //       ----->comp2_ctrl_deal(void) 中关机
    {
        if (bflg_comp2_running == 0)    //如果压缩机停机
        {
            bflg_heat20_wait_delaytime = 0;
            gss_heat20_wait_delaytimer = 0;
            
            bflg_heat20_comp2_stop = 1;                   // peak 如果满足加热丝加热条件， 压缩机停机延时 3 分钟（ EEP） 内，
                                                                 //       不进行压缩机开停判断， 待加热过程完成后， 压缩机正常控制。
            //----------------------------------------------
            if (bflg_heat20_running == 0)    //如果毛细管加热丝关
            {
                if (bflg_heat20_comp2_stop_delaytime == 0) //如果毛细管加热丝使压缩机关延时到
                {
                    bflg_heat20_running = 1;     //毛细管加热丝开
                    
                    bflg_heat20_run_delaytime = 1;
                    gss_heat20_run_delaytimer = 0;
                }
            }
            else    //如果毛细管加热丝开
            {
                if (bflg_heat20_run_delaytime == 0)   //加热6或10分钟完成
                {
                    bflg_heat20_running = 0;          //毛细管加热丝关
                    
                    bflg_heat20_stop_delaytime = 1;
                    gss_heat20_stop_delaytimer = 0;
                    
                    bflg_heat20_force_run = 0;
                    bflg_heat20_askfor_run = 0;    // peak 只此一处置0
                    
                    bflg_heat20_space_delaytime = 1;
                    gss_heat20_space_delaytimer = 0;
                }
            }
        }
    }
}

/******************************************************************************************************************************
功能: 间隔16个小时后，没有请求开毛细管加热的要求，则开请求加热毛细管的标志，若此时
      压缩机没有停机，则等待1小时后压缩机仍没有停机，则强制压缩机停机；

函数位置: --------------删除加热丝功能去掉的 -------------------------注释未调      
*******************************************************************************************************************************/
void heat20_wait_delaytime(void)   //毛细管加热丝等待延时程序，在1分钟定时程序中调用
{
    if (bflg_heat20_wait_delaytime == 1)
    {
        gss_heat20_wait_delaytimer++;
        if (gss_heat20_wait_delaytimer >= ram_para2[num_heat20_wait_delaytime]) //60 16个小时间隔到了，但过1小时后压机还是在开机，则强制关机；
        {
            gss_heat20_wait_delaytimer = 0;
            bflg_heat20_wait_delaytime = 0;
            
            bflg_heat20_comp2_stop = 1;  //置毛细管加热丝使压缩机关标志
        }
    }
}

/******************************************************************************************************************************
功能: 毛细管强制关压缩机180秒后，bflg_heat20_comp2_stop_delaytime 置0，毛细管才能打开进行加热；
         毛细管开-->压缩机必须关
         压缩机关-->毛细管必须关吗 ??   程序中每次关压机-->都 bflg_heat20_comp2_stop_delaytime = 1

函数位置:　－－－－－－－－－－－－－－删除加热丝功能去掉的  －---------------未调         
******************************************************************************************************************************/
void heat20_comp2_stop_delaytime(void)  //毛细管加热丝使压缩机关延时程序，在1秒定时程序中调用
{
    if (bflg_heat20_comp2_stop_delaytime == 1)
    {
        gss_heat20_comp2_stop_delaytimer++;
        if (gss_heat20_comp2_stop_delaytimer >= ram_para2[num_heat20_comp2_stop_delaytime])    //180
        {                                                      //压缩机停机满3分钟到
            gss_heat20_comp2_stop_delaytimer = 0;
            bflg_heat20_comp2_stop_delaytime = 0;
        }
    }
}

/*****************************************************************************************************************************
功能: 毛细管加热开x分钟后 ，关闭毛细管

函数位置: 1s---------删除加热丝功能去掉的   ------------------------- 未调
******************************************************************************************************************************/
void heat20_run_delaytime(void)    
{
    if (bflg_heat20_run_delaytime == 1)
    {
        gss_heat20_run_delaytimer++;
        if (bflg_heat20_force_run == 0) //如果是自动加热
        {
            if (gss_heat20_run_delaytimer >= ram_para2[num_heat20_run_delaytime])    //360   自动加热6分钟
            {                                                                        //毛细管加热完成自动停止
                gss_heat20_run_delaytimer = 0;        
                bflg_heat20_run_delaytime = 0;
            }
        }
        else   //如果是强制加热        // peak 既是手动加热  
        {
            if (gss_heat20_run_delaytimer >= ram_para2[num_heat20_force_run_delaytime])   //600  手动加热10分钟
            {
                gss_heat20_run_delaytimer = 0;
                bflg_heat20_run_delaytime = 0;
            }
        }
    }
}

/******************************************************************************************************************************
功能: 毛细管加热180秒后，最后 bflg_heat20_comp2_stop = 0; 停止对压缩机的强制停机，让
      其回到正常的控制，通过检测来设定是开还是关；承接 comp2_ctrl_deal(void) 函数 

函数位置: 1s----------------删除加热丝功能去掉的 --------------------------------------未调      
******************************************************************************************************************************/
void heat20_stop_delaytime(void)    
{
    if (bflg_heat20_stop_delaytime == 1)
    {
        gss_heat20_stop_delaytimer++;
        if (gss_heat20_stop_delaytimer >= ram_para2[num_heat20_stop_delaytime]) //180
        {
            gss_heat20_stop_delaytimer = 0;
            bflg_heat20_stop_delaytime = 0;
            
            bflg_heat20_comp2_stop = 0;            //加热6分钟后，停止对压缩机的强制停机，恢复自动压机自动控制
        }
    }
}

/**************************************************************************************************************************************************
函数功能：各个引脚根据控制是否输出

函数位置：主循环-----------------------------------ok
***************************************************************************************************************************************************/
void output_deal(void)      
{
    if (bflg_heat21_running == 0)  //平衡口
    {
        HTRPHK_OUT_PIN = 0;      
    }
    else
    {
        HTRPHK_OUT_PIN = 1;
    }
    //----------------------------------
    if (bflg_heat22_running == 0) //门体加热丝
    {
        HTRMT_OUT_PIN = 0;         
    }
    else
    {
        HTRMT_OUT_PIN = 1;
    }
    //----------------------------------
    if (bflg_bat_discharging == 1) //电池放电                              
    {
        BAT_DISCHARGE_PIN = 1;
    }
    else
    {
        BAT_DISCHARGE_PIN = 0;
    }
    //----------------------------------
    if(bflg_tank_mouth_run == 1)   //柜口加热丝
    {
        RL_HTRGK_OUT_PIN = 1;
    }
    else
    {
        RL_HTRGK_OUT_PIN = 0;
    }
}
/*************************************************************************************************************************************
函数功能：1、继电器输出处理程序，
          2、ULN2003反向输出，输入高则输出低；
          3、定频压机、风机都接在同一个芯片上，不让他们同时开，每间隔200ms开；

函数位置：主循环---------------------------------------ok
**************************************************************************************************************************************/
void RL_output_deal(void)     
{
    if (bflg_RL_output_delaytime == 0)
    {
        if (RL_COMP1_OUT_PIN != bflg_comp2_running)     //定频压机1
        {
            RL_COMP1_OUT_PIN = bflg_comp2_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_COMP2_OUT_PIN != bflg_comp21_running) //定频压机2
        {
            RL_COMP2_OUT_PIN = bflg_comp21_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_FAN1_OUT_PIN != bflg_outfan20_running) //风机1
        {
            RL_FAN1_OUT_PIN = bflg_outfan20_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_FAN2_OUT_PIN != bflg_outfan21_running) //风机2
        {
            RL_FAN2_OUT_PIN = bflg_outfan21_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_HTRMXG_OUT_PIN != bflg_heat20_running) //毛细管
        {
            RL_HTRMXG_OUT_PIN = bflg_heat20_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_CON_PIN != bflg_bat_charging)          //充电
        {
            RL_CON_PIN = bflg_bat_charging;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
    }
}
/***************************************************************************************************************************************
函数功能；继电器输出延时

函数位置：1ms----------------------------------------------ok
****************************************************************************************************************************************/
void RL_output_delaytime(void)     
{
    if (bflg_RL_output_delaytime == 1)
    {
        guc_RL_output_delaytimer++;
        if (guc_RL_output_delaytimer >= 200) //200ms
        {
            guc_RL_output_delaytimer = 0;
            bflg_RL_output_delaytime = 0;
        }
    }
}

/******************************************************************************************************************************
函数功能:     输入引脚的电平检测：1、开门 2、电池插反

函数位置：  10ms---------------------------------------ok          
******************************************************************************************************************************/
void input_pin_delaytime(void)      
{
    if (DOOR1_IN_PIN == 0)                   // 0关门
    {
        guc_door1_pin_high_delaytimer = 0;
        
        if (bflg_prot2_door1_pin == 1)
        {
            guc_door1_pin_low_delaytimer++;
            if (guc_door1_pin_low_delaytimer >= 100)   //1s
            {
                guc_door1_pin_low_delaytimer = 0;
                
                bflg_prot2_door1_pin = 0;
            }
        }
    }
    else                                      // 1开门      
    { 
        guc_door1_pin_low_delaytimer = 0;
        
        if (bflg_prot2_door1_pin == 0)
        {
            guc_door1_pin_high_delaytimer++;
            if (guc_door1_pin_high_delaytimer >= 100)  //1s
            {
                guc_door1_pin_high_delaytimer = 0;
                
                bflg_prot2_door1_pin = 1;
                
                bflg_comp21_door_freq = 1;
                bflg_comp21_door_freq_tmp = 1;
            }
        }
    }
    
    //------------------------------------------------------
	// 电池反接处理
    if (BAT_FED_IN_PIN == 1)  //电池未反接
    {
        guc_bat_fed_pin_high_delaytimer = 0;
        
        if (bflg_alarm_bat_fed == 1)       // 反接报警标志
        {
            guc_bat_fed_pin_low_delaytimer++;
            if (guc_bat_fed_pin_low_delaytimer >= 100) //1s
            {
                guc_bat_fed_pin_low_delaytimer = 0;
                
                bflg_alarm_bat_fed = 0;    
                gus_trip_code1 &= ~BAT_FED_CODE;
            }  
        }
    }
    else     // 电池反接  
    {
        guc_bat_fed_pin_low_delaytimer = 0;
        
        if (bflg_alarm_bat_fed == 0)
        {
            guc_bat_fed_pin_high_delaytimer++;
            if (guc_bat_fed_pin_high_delaytimer >= 100) //1s
            {
                guc_bat_fed_pin_high_delaytimer = 0;
                
                bflg_alarm_bat_fed = 1;
                gus_trip_code1 |= BAT_FED_CODE;
            }
        }
    }
}

/********************************************************************************************************************************
函数功能：  故障灯闪烁程序

函数位置： 10ms----------------------------------------未调
*********************************************************************************************************************************/
void trip_lampblink_deal(void)     
{
    if (guc_trip_code != 0)             //如果不是跳停，则转
    {
        if (guc_trip_flash_cnt > 0)     //如果闪烁次数已经减到零，则开始3秒间隔延时
        {
            guc_trip_flash_timer++;
            if (guc_trip_flash_timer >= 30)  //每0.3秒闪烁一次
            {
                guc_trip_flash_timer = 0;
                TRIP_LAMP_PIN = ~TRIP_LAMP_PIN;
                guc_trip_flash_cnt--;
            }
        }
        else
        {
            TRIP_LAMP_PIN = 0;
            guc_trip_flash_timer++;
            if (guc_trip_flash_timer >= 200)   //2秒间隔延时
            {
                guc_trip_flash_timer = 0;
                guc_trip_flash_cnt = guc_trip_code;
                guc_trip_flash_cnt <<= 1;
            }
        }
    }
    else
    {
        TRIP_LAMP_PIN = 0;
        guc_trip_flash_timer = 0;
        guc_trip_flash_cnt = 0;
    }
}
/****************************************END OF THE FILE***********************************************/
