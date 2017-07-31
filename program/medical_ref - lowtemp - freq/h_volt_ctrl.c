/*********************************************************************************************************************************
overview:
        1、检测交流电的电压是否过低

        2、检测电压是否需要进行升降压控制----要求不用了
**********************************************************************************************************************************/


#ifndef _VOLT_CTRL_C_
#define _VOLT_CTRL_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_volt_ctrl.h"
#include "h_protect.h"
//------------------------------------------------------------------------------
//函数声明
void comp2_volt_low_delaytime(void);    //高温压机电压过低延时程序，在1s定时程序中调用

void volt_ctrl_delaytime(void);    //电压控制延时程序，在1s定时程序中调用

void comp2_volt_delaytime(void);   //高温压缩机电压控制延时程序，在1s定时程序中调用

void comp21_volt_delaytime(void);  //低温压缩机电压控制延时程序，在1s定时程序中调用

//------------------------------------------------------------------------------
//变量定义
flag_type flg_volt;
//------------------------------------------------------------------------------
int16_t   gss_comp2_volt_delaytimer;    //电压过低延时计时器
int16_t   gss_volt_down_delaytimer;     //升压延时计时器
int16_t   gss_volt_up_delaytimer;       //降压延时计时器

int16_t   gss_comp2_volt_delaytimer;    //高温压缩机电压控制延时计时器
int16_t   gss_comp21_volt_delaytimer;   //低温压缩机电压控制延时计时器





/****************************************************************************************************************************
函数功能： 电压(220交流)过低的成立和恢复；
           电源电压过低时，不能关闭压缩机，电流过大，会对压机造成冲击；

函数位置： 1s--------------------------------------------------ok        
*****************************************************************************************************************************/
void comp2_volt_low_delaytime(void)      
{
    if(bflg_alarm_power_off == 0)           //未断电时判
    {
        if (bflg_comp2_volt_type == 0)      //如果是220V电源
        {
            if (bflg_comp2_volt_low == 0)   //如果当前无电压过低
            {
                if (gss_VAC <= ram_para2[num_comp2_volt_low_220V1])   //184V
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 1;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
            else
            {
                if (gss_VAC > ram_para2[num_comp2_volt_low_220V1])   //253V---->184
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 0;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
        }
        else  //如果是115V电源
        {
            if (bflg_comp2_volt_low == 0)   //如果当前无电压过低
            {
                if (gss_VAC <= ram_para2[num_comp2_volt_low_115V1])   //93V
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 1;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
            else
            {
                if (gss_VAC > ram_para2[num_comp2_volt_low_115V1])   //136V-->93
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 0;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
        }
    }
    else
    {
        gss_comp2_volt_delaytimer = 0;
        bflg_comp2_volt_low = 0;
    }
}


/**************************************************************************************************************************
函数功能 :  对应功能书上的--"升降压控制(继电器控制)"
           只是控制对应的继电器的开合，继电器控制升降压设备；

函数位置:    1s----------------海尔要求不使用升降压了---------------未调
**************************************************************************************************************************/
void volt_ctrl_delaytime(void)     //电压控制延时程序，在1s定时程序中调用
{
    if (bflg_comp2_volt_type == 0) //如果是220V
    {
        if (bflg_comp2_volt_down == 0)  //如果当前无降压
        {
            if (gss_VAC >= ram_para2[num_volt_down_220V1]) //242V
            {
                gss_volt_down_delaytimer++;
                if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime])     //10s
                {
                    gss_volt_down_delaytimer = 0;
                    
                    bflg_comp2_volt_down = 1;
                }
            }
            else
            {
                gss_volt_down_delaytimer = 0;
            }
        } 
        else   //如果当前降压
        {
            if (gss_VAC <= ram_para2[num_volt_down_220V2]) //230V
            {
                gss_volt_down_delaytimer++;
                if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime])     //10s
                {
                    gss_volt_down_delaytimer = 0;
                    
                    bflg_comp2_volt_down = 0;
                }
            }
            else
            {
                gss_volt_down_delaytimer = 0;
            }
        }
        //--------------------------------------------------
        if (bflg_comp2_volt_up == 0)  //如果当前无降压
        {
            if (gss_VAC <= ram_para2[num_volt_up_220V1]) //205V
            {
                gss_volt_up_delaytimer++;
                if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                {
                    gss_volt_up_delaytimer = 0;
                    
                    bflg_comp2_volt_up = 1;
                }
            }
            else
            {
                gss_volt_up_delaytimer = 0;
            }
        }
        else   //如果当前降压
        {
            if (gss_VAC >= ram_para2[num_volt_up_220V2]) //218V
            {
                gss_volt_up_delaytimer++;
                if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])  //10s
                {
                    gss_volt_up_delaytimer = 0;
                    
                    bflg_comp2_volt_up = 0;
                }
            }
            else
            {
                gss_volt_up_delaytimer = 0;
            }
        }
    }
    else  //如果是115V
    {
        if (bflg_comp2_volt_first_end == 1)  //初始上电，高温压缩机启动7分钟后
        {
            if (bflg_comp2_volt_down == 0)   //如果当前无降压
            {
                if (gss_VAC >= ram_para2[num_volt_down_115V1])    //122V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 1;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            else    //如果当前降压
            {
                if (gss_VAC <= ram_para2[num_volt_down_115V2])    //115V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 0;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
        }
        else if (bflg_comp21_volt_first_end == 1)      //初始上电，低温压缩机启动30秒后
        {
            if (bflg_comp2_volt_down == 0)   //如果当前无降压
            {
                if (gss_VAC >= ram_para2[num_volt_down_115V3])    //125V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 1;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            else    //如果当前降压
            {
                if (gss_VAC <= ram_para2[num_volt_down_115V4])    //120V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 0;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            //--------------------------------------------------
            if (bflg_comp2_volt_up == 0)     //如果当前无降压
            {
                if (gss_VAC <= ram_para2[num_volt_up_115V1]) //98V
                {
                    gss_volt_up_delaytimer++;
                    if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                    {
                        gss_volt_up_delaytimer = 0;
                        
                        bflg_comp2_volt_up = 1;
                    }
                }
                else
                {
                    gss_volt_up_delaytimer = 0;
                }
            }
            else    //如果当前降压
            {
                if (gss_VAC >= ram_para2[num_volt_up_115V2]) //105V
                {
                    gss_volt_up_delaytimer++;
                    if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                    {
                        gss_volt_up_delaytimer = 0;
                        
                        bflg_comp2_volt_up = 0;
                    }
                }
                else
                {
                    gss_volt_up_delaytimer = 0;
                }
            }
        }
    }
}

/******************************************************************************************************************************
函数功能:     在115V电压控制中，高温压缩机启动7分钟后才判断电压的高低,是否进行升降压，

函数位置： 1s--------------------------控制升降压------------------------未调          
*******************************************************************************************************************************/
void comp2_volt_delaytime(void)    //高温压缩机电压控制延时程序，在1s定时程序中调用
{
    if (bflg_comp2_volt_delaytime == 1)
    {
        gss_comp2_volt_delaytimer++;
        if (gss_comp2_volt_delaytimer >= (ram_para2[num_comp2_volt_delaytime] * 60)) //7分钟
        {
            gss_comp2_volt_delaytimer = 0;
            bflg_comp2_volt_delaytime = 0;
            
            bflg_comp2_volt_first_end = 1;
        }
    }
}

/***********************************************************************************************************************************
函数功能: 在115V中，低温压缩机启动 30s 后才判断电压的高低，判断是否升降压

函数位置：1s----------------------------------------------------未调
***********************************************************************************************************************************/
void comp21_volt_delaytime(void)   //低温压缩机电压控制延时程序，在1s定时程序中调用
{
    if (bflg_comp21_volt_delaytime == 1)
    {
        gss_comp21_volt_delaytimer++;
        if (gss_comp21_volt_delaytimer >= ram_para2[num_comp21_volt_delaytime]) //30秒
        {
            gss_comp21_volt_delaytimer = 0;
            bflg_comp21_volt_delaytime = 0;
            
            bflg_comp21_volt_first_end = 1;
        }
    }
}


#endif
/******************************************END OF THE FILE***************************************************/
