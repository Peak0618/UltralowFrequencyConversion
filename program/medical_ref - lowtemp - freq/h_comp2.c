#ifndef _COMP2_C_
#define _COMP2_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "r_cg_macrodriver.h"

#include "h_type_define.h"
#include "h_ad.h"
#include "h_key_operation.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_volt_ctrl.h"
#include "h_protect.h"
#include "h_main.h"
//------------------------------------------------------------------------------
//函数声明
void comp2_power_on_delaytime(void); 
void comp2_run_stop_delaytime(void); 
void comp2_ctrl_deal(void);    
void comp2_run_delaytime(void);    
void comp2_stop_delaytime(void); 
void comp2_TA_fault_run_delaytime(void);      
void comp2_TA_fault_stop_delaytime(void);     
void comp2_continue_run_delaytime(void);      
void comp2_force_stop_delaytime(void);        
void comp2_run_space_delaytime(void);    
void comp21_ctrl_deal(void);   
void comp2_start_delaytime(void);
void comp2_update_delaytime(void);
void comp21_start_delaytime(void);
void comp21_update_delaytime(void);
void comp2_power_down_delaytime(void);
void freq_ctrl_deal(void);
void init_freq_ctrl(void);


//------------------------------------------------------------------------------
//变量定义
flag_type flg_comp2;

//----------------------------------------------------------
//压缩机控制相关变量
int16_t   gss_comp2_power_on_delaytimer;     //上电延时定时器

int16_t   gss_comp2_run_T_delaytimer;        //高温压机达到开机温度持续时间
int16_t   gss_comp2_stop_T_delaytimer;       //            关机

int16_t   gss_comp2_run_delaytimer;          //高温压缩机开机延时定时器
int16_t   gss_comp2_stop_delaytimer;         //高温压缩机关机延时定时器
//----------------------------------------------------------
int16_t   gss_comp2_TA_fault_run_delaytimer;      //主传感器故障高温压缩机开机延时定时器
int16_t   gss_comp2_TA_fault_stop_delaytimer;     //主传感器故障高温压缩机关机延时定时器

int16_t   gss_comp2_continue_run_delaytimer;      //高温压缩机连续运行计时器
int16_t   gss_comp2_force_stop_delaytimer;        //高温压缩机强制停止计时器

int16_t   gss_comp2_run_space_delaytimer;         //高温压缩机运行间隔延时定时器
int8_t    gss_power_down_delaytime;               //断电后重新上电高温压机延时
uint8_t   guc_comp2_power_on_again;               //断电后重新上电高压机以4500启动


//频率控制相关变量
flag_type flg_freq;
int16_t   gss_para[27];

int16_t   gss_comp2_freq;     //高温压机频率
int16_t   gss_comp21_freq;    //低温压机频率   peak 即转速

int16_t   gss_comp2_delt_freq;
int16_t   gss_comp21_delt_freq;

int16_t   gss_comp2_start_delaytimer;
int16_t   gss_comp2_update_delaytimer;

int16_t   gss_comp21_start_delaytimer;
int16_t   gss_comp21_update_delaytimer;

uint16_t  gus_comp2_freq_cnt;
uint16_t  gus_comp21_freq_cnt;





/***************************************************************************************************************************************
函数功能：     上电延时程序,初始上电延时 1min（可调） 后，再对高、低压机进行控制


函数位置：  1s-------------------------ok   
***************************************************************************************************************************************/
void comp2_power_on_delaytime(void)
{
    if (bflg_comp2_power_on_delaytime == 1) //上电初始化为1
    {
        gss_comp2_power_on_delaytimer++;
        if (gss_comp2_power_on_delaytimer >= gss_factory_parm_value[DISP_FACTORY_Cd]*60) //min *60 = s                       
        {
            gss_comp2_power_on_delaytimer = 0;
            bflg_comp2_power_on_delaytime = 0;
        }
    }
}

/****************************************************************************************************************************************
函数功能:     根据要求的温度开、停机点，判断高温压机是否有开、停机的请求
       
函数位置: 1s定时器中------------------------------------------------ok
*****************************************************************************************************************************************/
void comp2_run_stop_delaytime(void)
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))      //断强电、断12v           
    {
        gss_comp2_run_T_delaytimer = 0;
        gss_comp2_stop_T_delaytimer = 0;

        bflg_comp2_askfor_run = 0;
        bflg_comp2_askfor_stop = 0;
    }
    else
    {
        if (ram_para[num_comp2_mode] == 1)  //如果是高性能运行(液晶、LED都默认高性能)     
        {
            if (bflg_comp2_running == 0)    //此时关机
            {
                if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_START_DELT_TEMP])   // 开机点
    	        {
    	            gss_comp2_stop_T_delaytimer = 0;  
    	            
    	            if (bflg_comp2_askfor_run == 0)
    	            {
    	                gss_comp2_run_T_delaytimer++;
    	                if (gss_comp2_run_T_delaytimer >= ram_para2[num_comp2_run_T_delaytime])  //30s
    	                {
    	                    gss_comp2_run_T_delaytimer = 0;
    	                    bflg_comp2_askfor_run = 1;
    	                }
    	            }
    	        }
    	        else                                                                      //否则 没有动作
    	        {
    	        	  gss_comp2_run_T_delaytimer = 0;
    	        	  gss_comp2_stop_T_delaytimer = 0;
    	        	  bflg_comp2_askfor_run = 0;
    	        }
            }
            else   //此时开机
            {
                if (gss_TA <= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_STOP_DELT_TEMP])  //停机点
    	        {
    	            gss_comp2_run_T_delaytimer = 0;
    	            
    	            if (bflg_comp2_askfor_stop == 0)
    	            {
    	                gss_comp2_stop_T_delaytimer++;
    	                if (gss_comp2_stop_T_delaytimer >= ram_para2[num_comp2_stop_T_delaytime]) //30s
    	                {
    	                    gss_comp2_stop_T_delaytimer = 0;
    	                    bflg_comp2_askfor_stop = 1;
    	                }
    	            }
    	        }
    	        else
    	        {
    	        	gss_comp2_run_T_delaytimer = 0;
    	            gss_comp2_stop_T_delaytimer = 0;
    	            bflg_comp2_askfor_stop = 0;
    	        }
            }
        }
        else  //节能运行
        {
            if (bflg_comp2_running == 0)
            {
    		    if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + ram_para2[num_comp2_mode_T] + ram_para2[num_comp2_run_Tdelt])
    		    //if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_START_DELT_TEMP])  // 10 开机
                {
    		            gss_comp2_stop_T_delaytimer = 0;
    		            
    		            if (bflg_comp2_askfor_run == 0)
    		            {
    		                gss_comp2_run_T_delaytimer++;
    		                if (gss_comp2_run_T_delaytimer >= ram_para2[num_comp2_run_T_delaytime])
    		                {
    		                    gss_comp2_run_T_delaytimer = 0;
    		                    bflg_comp2_askfor_run = 1;
    		                }
    		            }
    		        }
    		        else
    		        {
    		        	  gss_comp2_run_T_delaytimer = 0;
    		        	  gss_comp2_stop_T_delaytimer = 0;
    		        	  bflg_comp2_askfor_run = 0;
    		        }
            }
            else
            {
                    if (gss_TA <= gss_set_parm_value[DISP_SET_TS] + ram_para2[num_comp2_mode_T] + ram_para2[num_comp2_stop_Tdelt])
                   //if (gss_TA <= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_STOP_DELT_TEMP])  // 10 停机
    		        {
    		            gss_comp2_run_T_delaytimer = 0;
    		            
    		            if (bflg_comp2_askfor_stop == 0)
    		            {
    		                gss_comp2_stop_T_delaytimer++;
    		                if (gss_comp2_stop_T_delaytimer >= ram_para2[num_comp2_stop_T_delaytime])
    		                {
    		                    gss_comp2_stop_T_delaytimer = 0;
    		                    bflg_comp2_askfor_stop = 1;
    		                }
    		            }
    		        }
    		        else
    		        {
    		            gss_comp2_run_T_delaytimer = 0;
    		            gss_comp2_stop_T_delaytimer = 0;
    		            bflg_comp2_askfor_stop = 0;
    		        }
            }
        }
        //------------------------------------------------------
        if (bflg_comp21_door_freq == 1)
        {
            bflg_comp2_askfor_run = 1;
            
            bflg_comp2_stop_delaytime = 0;
            gss_comp2_stop_delaytimer = 0;
            
            bflg_comp2_force_stop_delaytime = 0;
            gss_comp2_force_stop_delaytimer = 0;
        }
        //------------------------------------------------------
        if (bflg_comp21_door_freq_tmp == 1)
        {
            if ((gss_TA - gss_set_parm_value[DISP_SET_TS]) >= gss_para[COMP_DOOR_TDELT])     //5.0℃
            {
                bflg_comp2_large_freq = 1;
                bflg_comp21_door_freq_tmp = 0;
            }
        }
    }
}

/********************************************************************************************************************************
函数功能：高压机根据是否有开停机请求、最小开停机延时是否结束、强制停机是否结束，
          来控制是否进行开停机


函数位置:主循环中-------------------------------------------------------ok          
********************************************************************************************************************************/
void comp2_ctrl_deal(void)    
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))   //断强电、12v
    {
        bflg_comp2_running = 0;                  //停机状态

        bflg_comp2_run_space_delaytime = 0;
        gss_comp2_run_space_delaytimer = 0;

        bflg_comp2_run_delaytime = 0;            //置开机延时标志
        gss_comp2_run_delaytimer = 0;            //清开机延时计时器

        bflg_comp2_continue_run_delaytime = 0;
        gss_comp2_continue_run_delaytimer = 0;

        bflg_comp2_first_run = 0;                //清首次运行标志

        bflg_comp2_force_stop_delaytime = 0;     //置强制停止延时标志  //功能书要求强制停止
        gss_comp2_force_stop_delaytimer = 0;     //清强制停止延时计时器

        bflg_comp2_stop_delaytime = 0;           //置关机延时标志
        gss_comp2_stop_delaytimer = 0;           //清关机延时计时器

        //--------------主故障时
        bflg_comp2_TA_fault_run_delaytime = 0;   //置主传感器故障开机延时标志        // peak 开30分钟--功能书 备注3
        gss_comp2_TA_fault_run_delaytimer = 0;   //清主传感器故障开机延时计时器

        bflg_comp2_TA_fault_stop_delaytime = 0;  //置主传感器故障关机延时标志
        gss_comp2_TA_fault_stop_delaytimer = 0;
    }
    else
    {
        if(bflg_power_down_delaytime == 0)      //如果未断电或是断电后延时结束
        {
            if (bflg_TA_fault == 1)             //如果主传感器故障   
            {
                if (bflg_comp2_running == 0)    //如果关机
                {
                    if (bflg_comp2_TA_fault_stop_delaytime == 0)    //如果主传感器故障关机延时时间结束   //peak 关10分钟结束--功能书 备注3
                    {
                        bflg_comp2_running = 1; //开机
                        
                        bflg_comp2_TA_fault_run_delaytime = 1;      //置主传感器故障开机延时标志        // peak 开30分钟--功能书 备注3
                        gss_comp2_TA_fault_run_delaytimer = 0;      //清主传感器故障开机延时计时器
                        
                        bflg_comp2_run_space_delaytime = 1;        //高温压缩机运行间隔延时标志  //备注1
                        gss_comp2_run_space_delaytimer = 0;
                    }
                }
                else   //如果开机
                {
                    if (bflg_comp2_TA_fault_run_delaytime == 0)     //如果主传感器故障开机延时时间结束
                    {
                        if (bflg_comp2_volt_low == 0)     //如果没有电压过低
                        {
                            bflg_comp2_running = 0;       //关机
                            
                            bflg_comp2_first_run = 0;     //清首次运行标志
                            
                            bflg_comp2_TA_fault_stop_delaytime = 1; //置主传感器故障关机延时标志
                            gss_comp2_TA_fault_stop_delaytimer = 0; //清主传感器故障关机延时计时器
                            
                            //bflg_heat20_comp2_stop_delaytime = 1;   //置毛细管加热丝使压缩机关延时标志----删除加热丝功能去掉的   
                            //gss_heat20_comp2_stop_delaytimer = 0;   //清毛细管加热丝使压缩机关延时计时器
                        }
                    }
                }
            }
            else       //如果主传感器正常
            {
                if (bflg_comp2_running == 0)     //如果当前是关机状态
                {
                    if (bflg_comp2_askfor_run == 1)   //如果请求开机  
                    {
                        if ((bflg_comp2_stop_delaytime == 0) && (bflg_comp2_force_stop_delaytime == 0))//停机延时时间结束
                        {
                            bflg_comp2_askfor_run = 0;    //清请求开机标志
                            bflg_comp2_running = 1;       //开机
                            
                            bflg_comp2_run_space_delaytime = 1;
                            gss_comp2_run_space_delaytimer = 0;
                            
                            bflg_comp2_run_delaytime = 1; //置开机延时标志
                            gss_comp2_run_delaytimer = 0; //清开机延时计时器
                            
                            bflg_comp2_continue_run_delaytime = 1;
                            gss_comp2_continue_run_delaytimer = 0;
                        }
                    }
                }
                else    //如果开机
                {
                    if (bflg_comp2_continue_run_delaytime == 0)     //如果高温压缩机连续运行时间结束
                    {
                        if (bflg_comp2_volt_low == 0)     //如果没有电压过低
                        {
                            bflg_comp2_running = 0;  //关机
                            
                            bflg_comp2_first_run = 0;     //清首次运行标志
                            
                            bflg_comp2_force_stop_delaytime = 1;    //置强制停止延时标志  //功能书要求强制停止
                            gss_comp2_force_stop_delaytimer = 0;    //清强制停止延时计时器
                        }
                        
                    }
                    else     //如果压机连续运行时间未结束
                    {
                        if (bflg_comp2_askfor_stop == 1)  //如果请求关机  //peak 在com2_run_stop_delaytime()函数中通过判断温度需要关机； 
                        {
                            if (bflg_comp2_run_delaytime == 0) //开机延时时间结束
                            {
                                if (bflg_comp2_volt_low == 0)       //如果没有电压过低
                                {
                                    bflg_comp2_askfor_stop = 0;     //清请求关机标志
                                    bflg_comp2_running = 0;         //关机
                                    
                                    bflg_comp2_first_run = 0;     //清首次运行标志
                            
                                    bflg_comp2_stop_delaytime = 1;  //置关机延时标志
                                    gss_comp2_stop_delaytimer = 0;  //清关机延时计时器                                    
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/********************************************************************************************************************************
函数功能:高温压缩机开机延时程序，高温压机至少开机3min才能关机

函数位置：1s定时器--------------------------------------ok  
********************************************************************************************************************************/
void comp2_run_delaytime(void)      
{
    if (bflg_comp2_run_delaytime == 1)
    {
        gss_comp2_run_delaytimer++;
        if (gss_comp2_run_delaytimer >= gss_para[COMP_RUN_DELAYTIME])   //180s = 3min*60
        {
            gss_comp2_run_delaytimer = 0;
            bflg_comp2_run_delaytime = 0;
        }
    }
}
/********************************************************************************************************************************
函数功能: 高温压缩机关机延时程序，至少关机3min才能重新开机

函数位置：1s定时器-------------------------------ok
*********************************************************************************************************************************/
void comp2_stop_delaytime(void)     
{
    if (bflg_comp2_stop_delaytime == 1)
    {
        gss_comp2_stop_delaytimer++;
        if (gss_comp2_stop_delaytimer >= gss_para[COMP_STOP_DELAYTIME])   //180s=3min*60
        {
            gss_comp2_stop_delaytimer = 0;
            bflg_comp2_stop_delaytime = 0;
        }
    }
}
/***********************************************************************************************************************************
函数功能:主传感器故障时，高温压缩机开机延时，开30min后关 

函数位置：1min--------------------------------ok
************************************************************************************************************************************/
void comp2_TA_fault_run_delaytime(void)
{
    if (bflg_comp2_TA_fault_run_delaytime == 1)
    {
        gss_comp2_TA_fault_run_delaytimer++;
        if (gss_comp2_TA_fault_run_delaytimer >= ram_para2[num_comp2_TA_fault_run_delaytime])  //30
        {
            gss_comp2_TA_fault_run_delaytimer = 0;
            bflg_comp2_TA_fault_run_delaytime = 0;
        }
    }
}
/***************************************************************************************************************************
函数功能:主传感器故障，高温压缩机关机延时，关机10min后才能开

函数位置：1s定时器---------------------------------------ok
****************************************************************************************************************************/
void comp2_TA_fault_stop_delaytime(void)
{
    if (bflg_comp2_TA_fault_stop_delaytime == 1)
    {
        gss_comp2_TA_fault_stop_delaytimer++;
        if (gss_comp2_TA_fault_stop_delaytimer >= ram_para2[num_comp2_TA_fault_stop_delaytime])  //600s = 10min
        {
            gss_comp2_TA_fault_stop_delaytimer = 0;
            bflg_comp2_TA_fault_stop_delaytime = 0;
        }
    }
}

/*****************************************************************************************************************************
函数功能:高压机首次上电连续16小时、其他时间连续运行5小时后强制停机 

函数位置：1min---------------------------------ok
******************************************************************************************************************************/
void comp2_continue_run_delaytime(void)
{
    if (bflg_comp2_continue_run_delaytime == 1)
    {
        gss_comp2_continue_run_delaytimer++;
        if (bflg_comp2_first_run == 1)  //如果是首次运行
        {
            if (gss_comp2_continue_run_delaytimer >= ram_para2[num_comp2_continue_first_run_delaytime])  //840min = 16h
            {
                gss_comp2_continue_run_delaytimer = 0;
                bflg_comp2_continue_run_delaytime = 0;
            }
        }
        else   //如果是正常运行
        {
            if (gss_comp2_continue_run_delaytimer >= ram_para2[num_comp2_continue_run_delaytime])  //300min= 5h
            {
                gss_comp2_continue_run_delaytimer = 0;
                bflg_comp2_continue_run_delaytime = 0;
            }
        }
    }
}

/****************************************************************************************************************************
函数功能: 高压机强制停机延时，强制停机后，若要开机需延时10min再开机

函数位置：1s定时器-----------------------------------ok
*****************************************************************************************************************************/
void comp2_force_stop_delaytime(void)
{
    if (bflg_comp2_force_stop_delaytime == 1)
    {
        gss_comp2_force_stop_delaytimer++;
        if (gss_comp2_force_stop_delaytimer >= ram_para2[num_comp2_force_stop_delaytime]) //600s = 10min
        {
            gss_comp2_force_stop_delaytimer = 0;
            bflg_comp2_force_stop_delaytime = 0;
        }
    }
}
/***************************************************************************************************************************
函数功能:高温压机开机延时后，才能开低温压机
         高压机开机1min或 10min(初始上电)后，才能打开低温压机


函数位置：1s定时器--------------------------------ok
****************************************************************************************************************************/
void comp2_run_space_delaytime(void)     
{
    if (bflg_comp2_run_space_delaytime == 1)
    {
        gss_comp2_run_space_delaytimer++;
        if (bflg_comp2_first_run == 1)  //如果是首次运行
        {
            if (gss_comp2_run_space_delaytimer >= ram_para2[num_comp2_first_run_space_delaytime])   //600s = 10min
            {
                gss_comp2_run_space_delaytimer = 0;
                bflg_comp2_run_space_delaytime = 0;
            }
        }
        else
        {
            if (gss_comp2_run_space_delaytimer >= ram_para2[num_comp2_run_space_delaytime])   //60s=1min
            {
                gss_comp2_run_space_delaytimer = 0;
                bflg_comp2_run_space_delaytime = 0;
            }
        }
    }
}
/*******************************************************************************************************************************
函数功能: 低温压机的开、停处理，判断是否达到开停机条件

函数位置：主循环---------------------------------ok
********************************************************************************************************************************/
void comp21_ctrl_deal(void) 
{
    if (bflg_comp2_running == 0)   //如果高温压缩机关机
    {
        bflg_comp21_running = 0;   //低温压缩机关机
    }
    else  //如果高温压缩机运行
    {
        if (bflg_comp2_run_space_delaytime == 0)  //如果高温压缩机开机间隔延时到 
        {
            if(bflg_TA_fault == 1)                //主传感器故障
            {
                bflg_comp21_running = 1;
            }
            else
            {
                if (bflg_TE_fault == 0)               //热交换正常
                {
                    if (gss_TE <= ram_para2[num_comp21_run_T])  //-30℃
                    {
                        bflg_comp21_running = 1;
                    }
                }
                else
                {
                    bflg_comp21_running = 1;
                }
            }
        }
    }
}



/*********************************************************************************************************************************************************
函数功能：正常开机时，以2000转运行1min

函数位置：1s定时器--------------------------ok
*********************************************************************************************************************************************************/
void comp2_start_delaytime(void)
{
    if (bflg_comp2_start_delaytime == 1)
    {
        gss_comp2_start_delaytimer++;
        if (gss_comp2_start_delaytimer >= gss_para[COMP2_START_TIME])
        {
            gss_comp2_start_delaytimer = 0;
            bflg_comp2_start_delaytime = 0;
        }
    }
}
/*********************************************************************************************************************************************************
函数功能：压机的转速每间隔80s进行一次更新

函数位置：1s定时器--------------------------------------ok
*********************************************************************************************************************************************************/
void comp2_update_delaytime(void)
{
    if (bflg_comp2_update_delaytime == 1)
    {
        gss_comp2_update_delaytimer++;
        if (gss_comp2_update_delaytimer >= gss_para[COMP2_UPDATE_TIME])
        {
            gss_comp2_update_delaytimer = 0;
            bflg_comp2_update_delaytime = 0;
        }
    }
}
/*********************************************************************************************************************************************************
函数功能：低温压机正常启动时以2000转运行1min 或断电后再启动以4500运行1min

函数位置：1s定时器------------------------------------ok
*********************************************************************************************************************************************************/
void comp21_start_delaytime(void)
{
    if (bflg_comp21_start_delaytime == 1)
    {
        gss_comp21_start_delaytimer++;
        if (gss_comp21_start_delaytimer >= gss_para[COMP21_START_TIME])
        {
            gss_comp21_start_delaytimer = 0;
            bflg_comp21_start_delaytime = 0;
        }
    }
}
/*********************************************************************************************************************************************************
函数功能：低温压机转速每80s更新一次

函数位置：1s--------------------------------------ok
*********************************************************************************************************************************************************/
void comp21_update_delaytime(void)
{
    if (bflg_comp21_update_delaytime == 1)
    {
        gss_comp21_update_delaytimer++;
        if (gss_comp21_update_delaytimer >= gss_para[COMP21_UPDATE_TIME])
        {
            gss_comp21_update_delaytimer = 0;
            bflg_comp21_update_delaytime = 0;
        }
    }
}

/*********************************************************************************************************************************
函数功能: 断电后重新上电，达到开机条件，高温压机需要延时1min后再启动

函数位置: 1s定时器中------------------------------------ok
*********************************************************************************************************************************/
void comp2_power_down_delaytime(void)
{
    if(bflg_power_down_delaytime  == 1)
    {
        gss_power_down_delaytime++;
        if(gss_power_down_delaytime >= 60)
        {
            gss_power_down_delaytime = 0;
            bflg_power_down_delaytime = 0;
        }
    }
}
/***********************************************************************************************************************
函数功能：高、低压机的频率控制

函数位置；1s定时器中------------------------------------------------ok
***********************************************************************************************************************/
void freq_ctrl_deal(void)
{
    int32_t lss_tmp;
    //----------------------------------
    if (bflg_comp2_running == 0)   //未开机
    {
        if (gss_comp2_freq != 0)
        {
            gss_comp2_freq = 0;
        }
    }
    else                           //开机
    {
        if((bflg_comp2_first_power_on == 1)||(guc_comp2_power_on_again == 2)) //初次上电/断电再上电
        {
            bflg_comp2_first_power_on = 0;
            guc_comp2_power_on_again = 0;
            gss_comp2_freq = gss_para[COMP2_FREQ_MAX];   //4500

            bflg_comp2_update_delaytime = 1;             //80s后在更新
            gss_comp2_update_delaytimer = 0;
        }
        else
        {   
            if (gss_comp2_freq == 0)
            {
                if(gss_THW > 30)
                {
                    gss_comp2_freq = gss_para[COMP2_FREQ_MAX];
                }
                else
                {
                    gss_comp2_freq = gss_para[COMP2_START_FREQ];
                }
                
                bflg_comp2_start_delaytime = 1;
                gss_comp2_start_delaytimer = 0;
            }
            else
            {
                if (bflg_comp2_start_delaytime == 0)
                {
                    if (bflg_comp2_update_delaytime == 0)
                    {
                        bflg_comp2_update_delaytime = 1;
                        gss_comp2_update_delaytimer = 0;
                        
                        if (bflg_comp21_running == 0)
                        {
                            lss_tmp = gss_TE;
                            lss_tmp -= gss_para[COMP2_SET_TEMP1];
                            lss_tmp *= gss_para[COMP2_FREQ_DELT_K1];
                        }
                        else
                        {
                            lss_tmp = gss_TE;
                            lss_tmp -= gss_para[COMP2_SET_TEMP2];
                            lss_tmp *= gss_para[COMP2_FREQ_DELT_K2];
                        }
                        
                        if(lss_tmp < gss_para[COMP2_FREQ_DELT_MIN])
                        {
                            lss_tmp = gss_para[COMP2_FREQ_DELT_MIN];
                        }
                        
                        if (lss_tmp > gss_para[COMP2_FREQ_DELT_MAX])
                        {
                            lss_tmp = gss_para[COMP2_FREQ_DELT_MAX];
                        }
                        
                        gss_comp2_delt_freq = (int16_t) (lss_tmp);
                        gss_comp2_freq += gss_comp2_delt_freq;
                        
                        if (gss_comp2_freq >= gss_para[COMP2_FREQ_MAX])
                        {
                            gss_comp2_freq = gss_para[COMP2_FREQ_MAX];
                        }
                        if (gss_comp2_freq <= gss_para[COMP2_FREQ_MIN])
                        {
                            gss_comp2_freq = gss_para[COMP2_FREQ_MIN];
                        }
                        
                    }
                }
            }
        }
    }
    //----------------------------------
    //低温压机
    if (bflg_comp21_running == 0)
    {
        if (gss_comp21_freq != 0)
        {
            gss_comp21_freq = 0;
        }
    }
    else
    {
        if(guc_power_down_lfreq_cnt == 2)
        {
            guc_power_down_lfreq_cnt = 0;
            gss_comp21_freq = 4500;          //断电又重新上电后低压机第一次启动以4500转

            bflg_comp21_start_delaytime = 1;
            gss_comp21_start_delaytimer = 0;
        }
        else
        {
            if (gss_comp21_freq == 0)
            {
                if(gss_THW > 30)
                {
                    gss_comp21_freq = 3500;
                }
                else
                {
                    gss_comp21_freq = gss_para[COMP21_START_FREQ];
                }
                
                bflg_comp21_start_delaytime = 1;
                gss_comp21_start_delaytimer = 0;
            }
            else
            {
                if (bflg_comp21_start_delaytime == 0)
                {
                    if (bflg_comp21_update_delaytime == 0)
                    {
                        bflg_comp21_update_delaytime = 1;
                        gss_comp21_update_delaytimer = 0;
                        
                        lss_tmp = gss_TA;
                        lss_tmp -= gss_set_parm_value[DISP_SET_TS];
                        lss_tmp += (gss_para[COMP21_FREQ_DELT_TEMP] * 10);
                        lss_tmp *= gss_para[COMP21_FREQ_DELT_K];
                        lss_tmp /= 10;
                        
                        if(lss_tmp < gss_para[COMP21_FREQ_DELT_MIN])
                        {
                            lss_tmp = gss_para[COMP21_FREQ_DELT_MIN];
                        }
                        
                        if (lss_tmp > gss_para[COMP21_FREQ_DELT_MAX])
                        {
                            lss_tmp = gss_para[COMP21_FREQ_DELT_MAX];
                        }
                        
                        gss_comp21_delt_freq = (int16_t) (lss_tmp);
                        gss_comp21_freq += gss_comp21_delt_freq;
                        
                        if (gss_comp21_freq >= gss_para[COMP21_FREQ_MAX])
                        {
                            gss_comp21_freq = gss_para[COMP21_FREQ_MAX];
                        }
                        if (gss_comp21_freq <= gss_para[COMP21_FREQ_MIN])
                        {
                            gss_comp21_freq = gss_para[COMP21_FREQ_MIN];
                        }
                        //--------------------------------------
                        if (gss_comp21_delt_freq < 0)
                        {
                        	  bflg_comp21_door_freq_tmp = 0;
                        }
                    }
                    //------------------------------------------
                    if (bflg_comp2_large_freq == 1)                       //开门1s 且>5°
                    {
                        gss_comp21_freq = gss_para[COMP21_FREQ_MAX];      //4500
                        
                        bflg_comp2_large_freq = 0;                        //bflg_comp2_large_freq 按习惯应改为--->bflg_comp21_large_freq，因为只调低温压机
                    }
                    else if (bflg_comp21_door_freq == 1)                   //开门1s
                    {
                        if (gss_comp21_freq < gss_para[COMP_DOOR_FREQ])
                        {
                        	  gss_comp21_freq = gss_para[COMP_DOOR_FREQ];  //3500
                        }
                        
                        bflg_comp21_door_freq = 0;
                    }
                }
            }
        }
    }
    //----------------------------------
    if (gss_comp2_freq == 0)
    {
    	  gus_comp2_freq_cnt = (60000 - 1);
    }
    else
    {
    	  gus_comp2_freq_cnt = (uint16_t) (60000000 / gss_comp2_freq);
    }
    //----------------------------------
    if (gss_comp21_freq == 0)
    {
    	  gus_comp21_freq_cnt = (60000 - 1);
    }
    else
    {
    	  gus_comp21_freq_cnt = (uint16_t) (60000000 / gss_comp21_freq);
    }
}
/*******************************************************************************************************************************
函数功能: 高、低压机的参数

函数位置：初始化---------------------------------ok
********************************************************************************************************************************/
void init_freq_ctrl(void)
{
    gss_para[COMP2_START_FREQ] = 2000;        // 0
    gss_para[COMP2_START_TIME] = 60;          // 1
    gss_para[COMP2_UPDATE_TIME] = 80;         // 2
    gss_para[COMP2_SET_TEMP1] = -30;          // 3
    gss_para[COMP2_FREQ_DELT_K1] = 50;        // 4
    gss_para[COMP2_FREQ_DELT_MIN] = -20;      // 5
    gss_para[COMP2_FREQ_DELT_MAX] = 20;       // 6
    gss_para[COMP2_FREQ_MIN] = 2000;          // 7
    gss_para[COMP2_FREQ_MAX] = 4500;          // 8
    gss_para[COMP2_SET_TEMP2] = -30;          // 9
    gss_para[COMP2_FREQ_DELT_K2] = 50;        // 10
    gss_para[COMP21_START_FREQ] = 2000;       // 11
    gss_para[COMP21_START_TIME] = 60;         // 12
    gss_para[COMP21_UPDATE_TIME] = 80;        // 13
    gss_para[COMP21_FREQ_DELT_K] = 50;        // 14
    gss_para[COMP21_FREQ_DELT_MIN] = -20;     // 15
    gss_para[COMP21_FREQ_DELT_MAX] = 20;      // 16
    gss_para[COMP21_FREQ_MIN] = 2000;         // 17
    gss_para[COMP21_FREQ_MAX] = 4500;         // 18
    gss_para[COMP21_FREQ_DELT_TEMP] = 0;      // 19
    //gss_para[COMP_START_DELT_TEMP] = 15;    // 20    
    //gss_para[COMP_STOP_DELT_TEMP] = 0;      // 21    
    gss_para[COMP_START_DELT_TEMP] = ram_para[num_debug_P20]; // P20 开机点调整现可保存  默认 5
    gss_para[COMP_STOP_DELT_TEMP]  = ram_para[num_debug_P21]; // P21 停机点调整               默认   0
    gss_para[PT100_DELT_TEMP] = 0;            // 22
    gss_para[COMP_STOP_DELAYTIME] = 180;      // 23 关机3min后才能开机
    gss_para[COMP_DOOR_TDELT] = 50;           // 24
    gss_para[COMP_DOOR_FREQ] = 3500;          // 25
    gss_para[COMP_RUN_DELAYTIME] = 180;       // 26 开机3min后才能关机
}
#endif
/***************************************END OF THE FILE*********************************************/
