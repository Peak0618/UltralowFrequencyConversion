#ifndef _BUZZ_C_
#define _BUZZ_C_


//-----------------------------------------------------------------------------------
//包含的文件声明
#include "h_type_define.h"
#include "r_cg_timer.h"
#include "h_protect.h"

#include "M_buzz.h"
#include "h_main.h"
//----------------------------------------------------------------------------------
//函数声明
void buzz_loop_tick(void);                //短滴循环
void buzz_loop_tick_delaytime(void);  //短滴响的时间
void buzz_one_tick(void);                 //长滴一声
void buzz_one_tick_delaytime(void);   //长滴响的时间
void buzz_always_sound(void);            //长响
void buzz_sound_priority(void);         //短滴循环、长滴一声、长响的优先级
void buzz_sound_cnt(void);



//---------------------------------------------------------------------------------
//变量定义
flag_type flg_buzz;

//------------------------------------------------------------------------------
int16_t   gss_buzz_loop_tick_delaytime;  
int16_t   gss_buzz_one_tick_delaytime;

uint8_t   guc_buzz_sound_cnt;            //自定义蜂鸣器响多少声，就赋值多少给他
uint8_t   guc_buzz_sound_cnt_delaytime;




/****************************************************************************************************************************************
函数功能: 滴滴响
 
函数位置: buzz_sound_priority()即主循环-------------------------ok
****************************************************************************************************************************************/
void buzz_loop_tick(void)
{
    if (bflg_buzz_loop_tick_output == 1)
    {    
        if(bflg_buzz_loop_tick_runing == 0)
        {
            bflg_buzz_loop_tick_runing = 1;
            R_TAU0_Channel3_Start();                //响的条件: 1、有报警 2、未按下"蜂鸣取消"键 
        }
    }
    else
    {
        bflg_buzz_loop_tick_runing = 0;
        R_TAU0_Channel3_Stop();  
    }
}

/*********************************************************************************************************************************
功能描述: 滴滴响间隔延时
  
函数位置：10ms---------------------------ok
*********************************************************************************************************************************/
void buzz_loop_tick_delaytime(void)
{
    if (bflg_buzz_loop_tick_set == 1)    
    {
        gss_buzz_loop_tick_delaytime++;
        if (gss_buzz_loop_tick_delaytime > 10)     //设定时间 10ms * 10 = 100 ms
        {
            bflg_buzz_loop_tick_output = ~bflg_buzz_loop_tick_output;
            gss_buzz_loop_tick_delaytime = 0;
        }
    }
    else
    {
        bflg_buzz_loop_tick_output = 0;
        gss_buzz_loop_tick_delaytime = 0;
    }
}


/***************************************************************************************************************************************
功能描述: 响一声

函数位置: buzz_sound_priority()即主循环-----------------------------ok
***************************************************************************************************************************************/
void buzz_one_tick(void)
{
    if (bflg_buzz_one_tick_set == 1)
    {
        if (bflg_buzz_one_tick_run == 0)
        {
            bflg_buzz_one_tick_run = 1;
            R_TAU0_Channel3_Start();
        }
    }
    else
    {
        if(bflg_buzz_one_tick_stop == 1)
        {
            bflg_buzz_one_tick_stop = 0;
            bflg_buzz_one_tick_run = 0；
            R_TAU0_Channel3_Stop();  
        }
    }
}


/***********************************************************************************************************************************
功能描述: 响一声的延时

函数位置: 100ms-------------------------------------ok       
***********************************************************************************************************************************/
void buzz_one_tick_delaytime(void)
{
    if (bflg_buzz_one_tick_set == 1)	  
    {
        gss_buzz_one_tick_delaytime++;
        if (gss_buzz_one_tick_delaytime >= 2)   // 100 * 2 = 200ms
        {
            bflg_buzz_one_tick_stop = 1;
            bflg_buzz_one_tick_set = 0;
            gss_buzz_one_tick_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************************
函数功能: 蜂鸣器长响

函数位置: buzz_sound_priority()即主循环-------------------------ok
********************************************************************************************************************************************/
void buzz_always_sound(void)
{
    if (bflg_buzz_always_sound_set == 1)
    {      
        if (bfla_buzz_always_sound_run == 0)
        {
            bfla_buzz_always_sound_run = 1;
            R_TAU0_Channel3_Start();        
        }
    }
    else
    {   
        if(bfla_buzz_always_sound_run == 1)
        {
            bfla_buzz_always_sound_run = 0; 
            R_TAU0_Channel3_Stop();    
        }
    }
}

/***********************************************************************************************************************************
功能描述:蜂鸣器长响、滴滴、响一声的优先级顺序 
         在长响时，不能再有关闭蜂鸣器的行为，即不能再有滴滴声，否则声音会卡；

函数位置: 主循环--------------------------------ok
***********************************************************************************************************************************/
void buzz_sound_priority(void)
{
    if (bflg_test_mode == 0) 
    {
        buzz_always_sound();
        
        if(bflg_buzz_always_sound_set == 0)  //无长响
        {
            buzz_one_tick();
            
            if(bflg_buzz_one_tick_set == 0)  //无响一声时
            {
                //包工注释
                buzz_loop_tick();
            }
        }

    }
}

/*************************************************************************************************************************************
功能描述:factory 指令的AT测试模式中的蜂鸣器响；


函数位置: 100ms定时器中----------ok
*************************************************************************************************************************************/
 void buzz_sound_cnt(void)
{    
    if(guc_buzz_sound_cnt > 0)
    {
        guc_buzz_sound_cnt_delaytime++;
        if(guc_buzz_sound_cnt_delaytime >= 10)  //1s
        {
            guc_buzz_sound_cnt--;
            guc_buzz_sound_cnt_delaytime = 0;
            
            bflg_buzz_one_tick_set = 1;         //响一次
        }
    }   
}


#endif
/***************************************ENG OF THE FILE*********************************************/
