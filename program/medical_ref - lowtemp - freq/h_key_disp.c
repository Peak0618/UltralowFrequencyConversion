/************************************************************************************************************************************************
overview:

*************************************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_timer.h"

#include "h_type_define.h"
#include "h_key_operation.h"
#include "h_key_disp.h"
#include "h_main.h"
#include "m_test.h"
#include "M_RTC.h"
#include "h_protect.h"
#include "h_ad.h"
#include "M_buzz.h"
#include "h_com.h"
#include "h_comp2.h"


//declaration
void led_disp1_deal(void); 
void led_disp2_deal(void); 
void led_disp3_deal(void);         
void led1_disp_val(int16_t lss_disp_val);     
void led2_disp_val(int16_t lss_disp_val);  
void led3_disp_val(int16_t lss_disp_val); 
void view_error_code(void);   
void view_error_code_delaytime (void);
void lamp_disp_deal(void);
void led_gss_TA_deal(void);
void led_power_off_disp_delaytime(void);


//variate
uint8_t   guc_disp_rate;                 //显示比率寄存器
int16_t   gss_error_code_delaytime;      //错误代码的显示延时时间
int16_t   gss_power_off_dis_delaytime;   //断电时数码管显示延时

//flag
flag_type flg_key_disp;

/******************************************************************************************************************
函数功能: 左面数码管1显示的内容:LED1~LED4
               
 
          LED8 xxxx xxxx --低4位从高到底对应: 锁定、箱内温度设定、高温报警设定、低温报警设定

函数位置：10ms----------------------------------------------ok
******************************************************************************************************************/
void led_disp1_deal(void)      
{
    if (bflg_test_mode == 1)              //如果是测试模式
    {
		R_TAU0_Channel3_Stop();
        if(bflg_test_mode_RTC == 0)        //未按下buzz键
        {
	        if (bflg_test_ok == 0)         //如果未测试完成
	        {
	            guc_disp_rate = dot_0d001;
	            led1_disp_val(SOFT_VERSION);
	        }
	        else                            //如果测试完成
	        {
	            if (guc_test_mode_code == 0)    
	            {
	                LED1 = data_P;
	                LED2 = data_A;
	                LED3 = data_S;
	                LED4 = data_S;
	            }
	            else                            //如果有测试故障
	            {
	                LED1 = data_all_off;
	                LED2 = data_E;
	                LED3 = guc_nmb_table[guc_test_mode_code / 10];
	                LED4 = guc_nmb_table[guc_test_mode_code % 10];  
	            }
	        }
        }
        else
        {
			LED1 = data_2;
			LED2 = data_0;
			LED3 = guc_nmb_table[guc_RTC_r_year / 10];
			LED4 = guc_nmb_table[guc_RTC_r_year % 10]; 
        }
    }
    else if (guc_disp_mode == DISP_LOCKED)     //如果是锁定模式
    {        
		if ((flag_view_error_code == 0) )	   //peak add  若没有错误代码正常显示   1、按下"蜂鸣取消"  2、有报警  3、锁定状态
        {                   
            if ((bflg_alarm_ALH == 1)          // 1
			  ||(bflg_alarm_ALL == 1)          // 2
			  ||(bflg_alarm_power_off == 1)    // 3
			  ||(bflg_alarm_bat_discnnt == 1)  // 5
			  ||(bflg_alarm_bat_fed == 1)      // 6
			  ||(bflg_alarm_bat_low == 1)      // 7
			  ||(bflg_TA_fault == 1)           // 8
			  ||(bflg_alarm_open_door == 1)    // 9
			  ||(bflg_alarm_THW_high == 1)     // 10 
              ||(bflg_THW_fault == 1)          // 11
              ||(bflg_TC_fault == 1)           // 12
              ||(bflg_TE_fault == 1)           // 13
			  ||(bflg_alarm_cond_dirty == 1))  // 14
			  //||(bflg_alarm_power_fault == 1)// 4          //这两个报警去掉
			  //|| (bflg_alarm_volt == 1)      // 15			
            {
	            if (guc_key_val == KEY_BUZ)                  // peak 如果按下 "蜂鸣取消"       				      
	            {
                    flag_view_error_code = 1;				
	            }
            }
            if(bflg_alarm_power_off == 1)      //断电
            {
                if(bflg_power_off_dis == 1)//显示5s
                {
                    led_gss_TA_deal();
                }
                else //黑屏30s
                {
                    LED1 = data_all_off;
                    LED2 = data_all_off;
                    LED3 = data_all_off;
                    LED4 = data_all_off;
                }
            }
            else
            {
                led_gss_TA_deal();
            }
        }
		else
		{
		    view_error_code();      //peak add显示错误代码
		}
        
        LED8 |= 0x08;               //peak 锁定状态 开锁定灯
        LED8 &= ~0x07;              //peak 关 "箱内温度设定" "高温报警设定" "低温报警设定"
    }
    else if(guc_disp_mode == DISP_UNLOCKED_PASSWORD)
    {
        if(gsc_disp_unlocked_password < 10)
        {
            LED1 = data_all_off;
            LED2 = data_all_off;
            LED3 = data_0;
            LED4 = guc_nmb_table[gsc_disp_unlocked_password];
        }
        else
        {
            led1_disp_val(gsc_disp_unlocked_password);
        }
    }
	else if(guc_disp_mode == DISP_UNLOCKED)           //如果是解锁状态，准备进入其他各个模式
	{
        led_gss_TA_deal();

        LED8 &= ~0x08;                                //关掉"锁定"灯
        LED8 &= ~0x07;                                //关 "箱内温度设定" "高温报警设定" "低温报警设定"
	}
    else if(guc_disp_mode == DISP_SET) 
    {   
        
        LED8 &= ~0x08;                      //关掉"锁定"灯
        if(gsc_disp_parm_type == DISP_SET_TS)
        {
            LED8 &= ~0x07;
            LED8 |= 0x01; 
        }
        else if(gsc_disp_parm_type == DISP_SET_ALH)
        {
            LED8 &= ~0x07;
            LED8 |= 0x02;  
        }
        else if(gsc_disp_parm_type == DISP_SET_ALL)
        {
            LED8 &= ~0x07;
            LED8 |= 0x04;
        }

        
        if(bflg_disp1_flicker == 1)               //闪烁
        {
            if(bflg_parm_save == 1)
            {
                led1_disp_val(gss_key_temp_value / 10);
            }
            else
            {
                led1_disp_val(gss_set_parm_value[gsc_disp_parm_type] / 10);
            }
        }
        else
        {
            LED1 = data_all_off;
            LED2 = data_all_off;
            LED3 = data_all_off;
            LED4 = data_all_off;
        }
    }
    else if(guc_disp_mode == DISP_USER) 
    {
        
        LED8 &= ~0x07;                   // 关 "TS" "ALH" "ALL"灯
        if(bflg_parm_dis_form == 0)
        {
            if(gsc_disp_parm_type == DISP_USER_dA)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_d;
                LED4 = data_A;
            }
            if(gsc_disp_parm_type == DISP_USER_T1)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_T;
                LED4 = data_1;
            }
            if(gsc_disp_parm_type == DISP_USER_T2)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_T;
                LED4 = data_2;
            }
            if(gsc_disp_parm_type == DISP_USER_P6)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_P;
                LED4 = data_6;
            }
            if(gsc_disp_parm_type == DISP_USER_IC)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_I;
                LED4 = data_C;
            }
            if(gsc_disp_parm_type == DISP_USER_PS1)
            {
                LED1 = data_all_off;
                LED2 = data_P;
                LED3 = data_S;
                LED4 = data_1;
            }
            if(gsc_disp_parm_type == DISP_USER_CL1)
            {
                LED1 = data_all_off;
                LED2 = data_C;
                LED3 = data_L;
                LED4 = data_1;
            }            
        }
        else
        {
            if(bflg_disp1_flicker == 1)         //闪烁
            {
                if(gsc_disp_parm_type == DISP_USER_T2) //special 因和保存无关
                {
                    LED1 = data_all_off;
                    LED2 = data_all_off;
                    LED3 = data_P;
                    LED4 = guc_nmb_table[gss_user_parm_value[DISP_USER_T2]+1];
                }
                else
                {
                    if(bflg_parm_save == 0)
                    {
                        guc_disp_rate = not_dot;
                        led1_disp_val(gss_user_parm_value[gsc_disp_parm_type]);
                    }
                    else                           //临时值
                    {
                        guc_disp_rate = not_dot;
                        led1_disp_val(gss_key_temp_value);
                    }
                }
            }
            else
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_all_off;
                LED4 = data_all_off;
            }
        }
    }
    else if (guc_disp_mode == DISP_FAC_PASSWORD)      //如果是工厂密码模式
    {
        guc_disp_rate = not_dot;
        led1_disp_val(gsc_password_val);
        if(gsc_password_val/10 == 0)                  //为了显示 00 
        {LED3 = data_0;}
        
        LED8 &= ~0x07;                                 //关 "箱内温度设定" "高温报警设定" "低温报警设定"
    }    
    else if(guc_disp_mode == DISP_FACTORY) 
    {
        if(bflg_parm_dis_form == 0)
        {
            if(gsc_disp_parm_type == DISP_FACTORY_CL2)   //CL2~CL4可闪烁
            {
                if(gss_factory_parm_value[DISP_FACTORY_CL2] == 1)
                {
                    if(bflg_disp1_flicker == 1)
                    {
                        LED1 = data_all_off;
                        LED2 = data_C;
                        LED3 = data_L;
                        LED4 = data_2;

                        gss_fac_CL3_flicker_cnt++;
                        if(gss_fac_CL3_flicker_cnt >= 150)            //接近闪烁3次
                        {
                            gss_fac_CL3_flicker_cnt = 0;
                            gss_factory_parm_value[DISP_FACTORY_CL2] = 0;
                        }
                    }
                    else
                    {
                        LED1 = data_all_off;
                        LED2 = data_all_off;
                        LED3 = data_all_off;
                        LED4 = data_all_off;
                    }
                }
                else
                {
                    LED1 = data_all_off;
                    LED2 = data_C;
                    LED3 = data_L;
                    LED4 = data_2;
                }
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_CL3)
            {
                if(gss_factory_parm_value[DISP_FACTORY_CL3] == 1)
                {
                    if(bflg_disp1_flicker == 1)
                    {
                        LED1 = data_all_off;
                        LED2 = data_C;
                        LED3 = data_L;
                        LED4 = data_3;

                        gss_fac_CL3_flicker_cnt++;
                        if(gss_fac_CL3_flicker_cnt >= 150)            //接近闪烁3次
                        {
                            gss_fac_CL3_flicker_cnt = 0;
                            gss_factory_parm_value[DISP_FACTORY_CL3] = 0;
                        }
                    }
                    else
                    {
                        LED1 = data_all_off;
                        LED2 = data_all_off;
                        LED3 = data_all_off;
                        LED4 = data_all_off;
                    }
                }
                else
                {
                    LED1 = data_all_off;
                    LED2 = data_C;
                    LED3 = data_L;
                    LED4 = data_3;
                }
            }   
            else if(gsc_disp_parm_type == DISP_FACTORY_CL4)
            {
                if(gss_factory_parm_value[DISP_FACTORY_CL4] == 1)
                {
                    if(bflg_disp1_flicker == 1)
                    {
                        LED1 = data_all_off;
                        LED2 = data_C;
                        LED3 = data_L;
                        LED4 = data_4;

                        gss_fac_CL3_flicker_cnt++;
                        if(gss_fac_CL3_flicker_cnt >= 150)            //接近闪烁3次
                        {
                            gss_fac_CL3_flicker_cnt = 0;
                            gss_factory_parm_value[DISP_FACTORY_CL4] = 0;
                        }
                    }
                    else
                    {
                        LED1 = data_all_off;
                        LED2 = data_all_off;
                        LED3 = data_all_off;
                        LED4 = data_all_off;
                    }
                }
                else
                {
                    LED1 = data_all_off;
                    LED2 = data_C;
                    LED3 = data_L;
                    LED4 = data_4;
                }
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_UL)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_U;
                LED4 = data_L;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_Ct)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_C;
                LED4 = data_t;
            }   
            else if(gsc_disp_parm_type == DISP_FACTORY_CU)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_C;
                LED4 = data_U;
            }  
            else if(gsc_disp_parm_type == DISP_FACTORY_CA)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_C;
                LED4 = data_A;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_Fd)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_F;
                LED4 = data_d;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_AT)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_A;
                LED4 = data_T;
            }   
            else if(gsc_disp_parm_type == DISP_FACTORY_SS)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_S;
                LED4 = data_S;
            } 
            else if(gsc_disp_parm_type == DISP_FACTORY_AA)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_A;
                LED4 = data_A;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_Cd)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_C;
                LED4 = data_d;
            } 
            else if(gsc_disp_parm_type == DISP_FACTORY_PS2)
            {
                LED1 = data_all_off;
                LED2 = data_P;
                LED3 = data_S;
                LED4 = data_2;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_bS)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_b;
                LED4 = data_S;
            }
            else if(gsc_disp_parm_type == DISP_FACTORY_LL)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_L;
                LED4 = data_L;
            }   
            else if(gsc_disp_parm_type == DISP_FACTORY_HC)
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_H;
                LED4 = data_C;
            }             
        }
        else
        {    
            if(bflg_disp1_flicker == 1)         //闪烁
            {
                if(bflg_parm_save == 0)
                {
                    guc_disp_rate = not_dot;
                    led1_disp_val(gss_factory_parm_value[gsc_disp_parm_type]);
                }
                else                           //临时值
                {
                    guc_disp_rate = not_dot;
                    led1_disp_val(gss_key_temp_value);
                }
            }
            else
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_all_off;
                LED4 = data_all_off;
            }
        }
    }    
    else if(guc_disp_mode == DISP_IC_PASSWORD) 
    {        
        if(bflg_IC_pass_CL1 == 1)
        {
            if(bflg_disp1_flicker == 1)                       //输入密码正确后，CL1闪烁3次            
            {   
                LED1 = data_all_off;
                LED2 = data_C;
                LED3 = data_L;
                LED4 = data_1;
              
                gss_user_CL1_flicker_cnt++;
                if(gss_user_CL1_flicker_cnt >= 150)            //接近闪烁3次
                {
                    gss_user_CL1_flicker_cnt = 0;
                    bflg_buzz_one_tick_set = 1;   
                    bflg_IC_pass_CL1 = 0;
                    
                    guc_disp_mode = DISP_USER;                  //返回到用户的CL1
                    gsc_disp_parm_type = DISP_USER_CL1;
                    gss_key_temp_value = 0;
                    bflg_parm_dis_form = 0;
                    bflg_parm_save = 0;
                }
            }
            else
            {
                LED1 = data_all_off;
                LED2 = data_all_off;
                LED3 = data_all_off;
                LED4 = data_all_off;
            }
        }
        else
        {
            guc_disp_rate = not_dot;
            led1_disp_val(gsc_IC_password_val);
            if(gsc_IC_password_val/100 == 0)                 //为了显示 000
            {
                LED2 = data_0;
            }
            if(gsc_IC_password_val/10 == 0)                
            {
                LED3 = data_0;
            }
        }
       
        LED8 &= ~0x07;              //peak 关 "箱内温度设定" "高温报警设定" "低温报警设定"
    }
    else if(guc_disp_mode == DISP_IC_REG) 
    {
        LED1 = data_line;
        LED2 = data_line;
        LED3 = guc_nmb_table[(guc_flash_add/4)/10];      //显示IC注册已用的地址；
        LED4 = guc_nmb_table[(guc_flash_add/4)%10];    
    }
    else if(guc_disp_mode == DISP_TIME_SET)                             //设定调试参数模式
    {
        if(bflg_disp1_flicker == 1)  
        {
            led1_disp_val(guc_user_write_time_value);
        }
        else
        {
            LED1 = data_all_off;
            LED2 = data_all_off;
            LED3 = data_all_off;
            LED4 = data_all_off;
        }
    }    
    else if(guc_disp_mode == DISP_DEBUG_SELECT)       //调试参数之间的切换模式
    {        
        if (gsc_disp_debug_mode == DISP_DEBUG_SEE)   
        {           
                LED1 = data_all_off;
                LED2 = data_r | data_dot;
                LED3 = data_hyphen;
                LED4 = data_hyphen;
                //LED3 = guc_nmb_table[(gsc_disp1_see_mode / 10)];
                //LED4 = guc_nmb_table[gsc_disp1_see_mode % 10];
        }
        else if (gsc_disp_debug_mode == DISP_DEBUG_PARA)   
        {
                LED1 = data_all_off;
                LED2 = data_P | data_dot;
                LED3 = data_hyphen;
                LED4 = data_hyphen;
                //LED3 = guc_nmb_table[(gsc_disp1_para_mode / 10)];
                //LED4 = guc_nmb_table[gsc_disp1_para_mode % 10];
        }
        
        LED8 &= ~0x08;                      //关掉"锁定"灯
    }
    else if(guc_disp_mode == DISP_DEBUG)                             //设定调试参数模式
    {
        if (gsc_disp_debug_mode == DISP_DEBUG_SEE)                   //如果是显示查询模式
        {
            if (bflg_disp1_val == 0)
            {
                LED1 = data_all_off;
                LED2 = data_r | data_dot;
                LED3 = guc_nmb_table[(gsc_disp1_see_mode / 10)];
                LED4 = guc_nmb_table[gsc_disp1_see_mode % 10];
            }
            else
            {
                guc_disp_rate = not_dot;
                led1_disp_val(gss_disp_see_buffer[gsc_disp1_see_mode]);
            }
        }
        else if (gsc_disp_debug_mode == DISP_DEBUG_PARA)               //如果是显示参数模式
        {
            if (bflg_disp1_val == 0)
            {
                LED1 = data_all_off;
                LED2 = data_P | data_dot;
                LED3 = guc_nmb_table[(gsc_disp1_para_mode / 10)];
                LED4 = guc_nmb_table[gsc_disp1_para_mode % 10];
            }
            else
            {
                //debug
                //guc_disp_rate = (uint8_t)(EEPROM_init2[gsc_disp1_para_mode][4]);
                //led1_disp_val(ram_para2[gsc_disp1_para_mode]);
                guc_disp_rate = 0;
                led1_disp_val(gss_para[gsc_disp1_para_mode]);
            }
        }
        
        LED8 &= ~0x08;                      //关掉"锁定"灯
    }


    //U盘状态的指示灯
    if(guc_U_disk_state == 0x00)       //空闲已拔出_灭
    {
        LED4 &= (~data_dot); 
    }
    else if(guc_U_disk_state == 0x01)  //忙_闪
    {
        if(bflg_disp1_flicker == 1) 
        {
            LED4 |= data_dot; 
        }
        else
        {
            LED4 &= (~data_dot); 
        }
    }
    else if(guc_U_disk_state == 0x02)  //读写完成_常亮
    {
        LED4 |= data_dot; 
    }
    /*else if(guc_U_disk_state == 0x03) //预留_读写失败_自定义右数第2个点闪
    {
        if(bflg_disp1_flicker == 1) 
        {
            LED3 |= data_dot; 
            LED4 |= data_dot; 
        }
        else
        {
            LED3 &= (~data_dot); 
            LED4 &= (~data_dot); 
        }
    }*/
    else
    {
        LED3 &= (~data_dot); 
        LED4 &= (~data_dot); 
    }
    /*if((guc_usb_state & 0x04) == 0x04) //预留_usb板故障_自定义usb板坏右数第3个点闪
    {
        if(bflg_disp1_flicker == 1) 
        {
            LED2 |= data_dot; 
        }
        else
        {
            LED2 &= (~data_dot); 
        }
    }
    else
    {
        LED2 &= (~data_dot); 
    }*/
    
}

//------------------------------------------------------------------------------
/*******************************************************************************
function: 中
          中间的LED要显示的内容
          
*******************************************************************************/
void led_disp2_deal(void)     //数码管2显示处理程序，在100ms定时程序中调用
{
    if (bflg_test_mode == 1)         //如果是测试模式
    {
        guc_disp_rate = not_dot;
        led2_disp_val(gss_TA / 10);
    }
    else
    {
        if(bflg_alarm_power_off == 1)           //断电
        {
            if(bflg_power_off_dis == 1)
            {
                guc_disp_rate = not_dot;
                led2_disp_val(gss_VAC);         //电压
            }
            else
            {
                LED5 = data_all_off;
                LED6 = data_all_off;
                LED7 = data_all_off;
            }
        }
        else
        {
            guc_disp_rate = not_dot;
            //debug
            led2_disp_val(gss_VAC);              //选择电压显示
            //led2_disp_val(guc_bat_Vout);
            //led2_disp_val(gss_adg_Temp[0]);
            //led2_disp_val(guc_RTC_r_minute);
            //led2_disp_val(gss_comp2_freq / 10);    //debug高温压机频率  
        }
    }      
}

//------------------------------------------------------------------------------
/*******************************************************************************
function: 右
-------------------显示依据-----------------------
 判断条件： 环温＞35℃（ EEP） 持续 30 秒（ EEP）；
 处理措施： 报警灯闪烁；
 恢复条件： 环温≤34℃（ EEP） 持续 30 秒（ EEP）；
		  （此时如果显示环温＜35℃， 则显示 35℃）
---------------------------------------------------

 bflg_prot2_THW_high_disp 在protect中选择 0/1 (现在的环温/设定的)
 
*******************************************************************************/
void led_disp3_deal(void)        //数码管3显示处理程序，在100ms定时程序中调用
{    
    if (bflg_test_mode == 1)         //如果是测试模式
    {
        guc_disp_rate = not_dot;
        led3_disp_val(gss_VAC);
    }
    else
    {        
        if(bflg_alarm_power_off == 1)           //断电
        {
            if(bflg_power_off_dis == 1)
            {
                guc_disp_rate = not_dot;
                led3_disp_val(gss_THW);         //环温
            }
            else
            {
                LED9  = data_all_off;
                LED10 = data_all_off;
                LED11 = data_all_off;
            }
        }
        else
        {
            guc_disp_rate = not_dot;
            
            //led3_disp_val(guc_bat_Vin);
            //led3_disp_val(gss_bat_I);
            //led3_disp_val(gss_adg_Temp[1]);
            //led3_disp_val(gss_bat_pwm_value);
            //led3_disp_val(gss_adg_Temp2);
            led3_disp_val(gss_THW);                   //环温
            //led3_disp_val(guc_RTC_r_second);
            //led3_disp_val(gss_comp21_freq / 10);    //debug低压机转速
        }
    }
}

//------------------------------------------------------------------------------
/*******************************************************************************
 左面的数码管是4位数码管:
 四位数码管的1~4 位的分别显示:
*******************************************************************************/
void led1_disp_val(int16_t lss_disp_val)     //数码管1显示数值程序
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //显示负数标志   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak 将负数的补码转换为我们想要的不带负号的原码
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak 取商，取第四位的值
    {
        LED1 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak 有值就显示
    }
    else
    {
        LED1 = data_all_off;                             //peak 没值就不亮
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak 取余，剩下的数值
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_disp_rate == dot_0d01)                   //peak 判断LED2此位小数点是否开
        {
            LED2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak 判断前一位是否有负号
        {
            if (LED1 == data_all_off)
            {
                LED1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_disp_rate == dot_0d01)                  //peak 判断为0时是否开小数点
        {
            LED2 = data_0 | data_dot;
        }
        else
        {
            if (LED1 == data_all_off)                   //peak 前一位没有值，此位为0没意义，也关掉
            {
                LED2 = data_all_off;
            }
            else
            {
                LED2 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED3
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED3 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_disp_rate == dot_0d1)
        {
            LED3 |= data_dot;               //peak LED3的小数点也显示上
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED2 == data_all_off)
            {
                LED2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1 == data_all_off)
            {
                LED1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_disp_rate == dot_0d1)
        {
            LED3 = data_0 | data_dot;
        }
        else
        {
            if (LED2 == data_all_off)
            {
                LED3 = data_all_off;
            }
            else
            {
                LED3 = data_0;
            }
        }
    }
    //------------------------------------------------------ LED4
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED4 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED3 == data_all_off)
            {
                LED3 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED2 == data_all_off)
            {
                LED2 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED1 == data_all_off)
            {
                LED1 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED4 = data_0;
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
 中间这个数码管为3位数码管
 
*******************************************************************************/
void led2_disp_val(int16_t lss_disp_val) //数码管2显示数值程序
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
        LED5 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED5 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED6 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_disp_rate == dot_0d1)
        {
            LED6 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED5 == data_all_off)
            {
                LED5 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_disp_rate == dot_0d1)
        {
            LED6 = data_0 | data_dot;
        }
        else
        {
            if (LED5 == data_all_off)
            {
                LED6 = data_all_off;
            }
            else
            {
                LED6 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED7 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED6 == data_all_off)
            {
                LED6 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED5 == data_all_off)
            {
                LED5 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED7 = data_0;
    }
}
//------------------------------------------------------------------------------
 /*******************************************************************************
 右面这个数码管是3位数码管  
*******************************************************************************/
void led3_disp_val(int16_t lss_disp_val) //数码管3显示数值程序
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
        LED9 = guc_nmb_table[lus_disp_abs_val / 100];
    }
    else
    {
        LED9 = data_all_off;
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 100;
    if ((lus_disp_abs_val / 10) != 0)
    {
        LED10 = guc_nmb_table[lus_disp_abs_val / 10];
        //------------------------------
        if (guc_disp_rate == dot_0d1)
        {
            LED10 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)
        {
            if (LED9 == data_all_off)
            {
                LED9 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        if (guc_disp_rate == dot_0d1)
        {
            LED10 = data_0 | data_dot;
        }
        else
        {
            if (LED9 == data_all_off)
            {
                LED10 = data_all_off;
            }
            else
            {
                LED10 = data_0;
            }
        }
    }
    //------------------------------------------------------
    lus_disp_abs_val %= 10;
    if (lus_disp_abs_val != 0)
    {
        LED11 = guc_nmb_table[lus_disp_abs_val];
        if (bflg_disp_minus == 1)
        {
            if (LED10 == data_all_off)
            {
                LED10 = data_hyphen;
                bflg_disp_minus = 0;
            }
            else if (LED9 == data_all_off)
            {
                LED9 = data_hyphen;
                bflg_disp_minus = 0;
            }
        }
    }
    else
    {
        LED11 = data_0;
    }
}



/******************************************************************************
function: 显示板未解锁时，按一下 “蜂鸣取消”键可查询故障代码如下:

函数位置； 在 led_disp1_deal(void) 中,在10ms定时器中--------ok
******************************************************************************/
void view_error_code(void)    
{ 
     // 有高温报警 且 没有被读过
    if ((bflg_alarm_ALH == 1) && (bflg_error_code_alarm_ALH_mark ==0))        //peak 1 高温报警标志 
    {
        bflg_error_code_delaytime = 1;

        LED1 = data_all_off;
        LED2 = data_E_dp;
        LED3 = data_0;
        LED4 = data_0;
    } 	
    else if ((bflg_alarm_ALL == 1) && (bflg_error_code_alarm_ALL_mark ==0))  //peak 2 低温报警标志 
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_1;
    }
    else if ((bflg_alarm_THW_high == 1) && (bflg_error_code_alarm_THW_high_mark ==0))    //peak 3 环温过高报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_2;
    }
    else if ((bflg_TA_fault == 1) && (bflg_error_code_alarm_TA_fault_mark ==0))     //peak 4 主传感器故障报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_0;
    }
    else if ((bflg_TE_fault == 1) && (bflg_error_code_alarm_TE_fault_mark ==0))     //peak 5 热交换器传感器故障 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_1;
    }
    else if ((bflg_THW_fault == 1) && (bflg_error_code_alarm_THW_fault_mark ==0))     //peak 6 环温传感器故障报警 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_2;
    }
   else if ((bflg_TC_fault == 1) && (bflg_error_code_alarm_TC_fault_mark ==0))     //peak 7 冷凝传感器故障报警 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_3;
    }
    else if ((bflg_alarm_bat_discnnt == 1) && (bflg_error_code_alarm_bat_discnnt_mark ==0))  //peak 8 电池未连接报警标志 
    {
        bflg_error_code_delaytime = 1; 
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_0;
    }
    else if ((bflg_alarm_bat_fed == 1) && (bflg_error_code_alarm_bat_fed_mark ==0))     //peak 9 电池接反报警标志 
    { 
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_1;
    }
    else if ((bflg_alarm_bat_low == 1) && (bflg_error_code_alarm_bat_low_mark ==0))      //peak 10 电池电量低报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_2;
    }
    else if ((bflg_alarm_power_off == 1) && (bflg_error_code_alarm_power_off_mark ==0))    //peak 11 断电报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_3;
		LED4 = data_0;
    }
    else if ((bflg_alarm_open_door == 1) && (bflg_error_code_alarm_open_door_mark ==0))    //peak 12 开门报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_4;
		LED4 = data_0;
    }
    else if ((bflg_alarm_cond_dirty == 1) && (bflg_error_code_alarm_cond_dirty_mark ==0))   //peak 13 冷凝器脏报警标志 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_5;
		LED4 = data_0;
    }
    /*else if ((bflg_alarm_power_fault == 1) && (bflg_error_code_alarm_power_fault_mark ==0))  //peak  4 电源板故障报警标志 
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_4;
    }
    else if ((bflg_alarm_volt == 1) && (bflg_error_code_alarm_volt_mark ==0))   //peak 15 电压超标报警标志 15
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_5;
    }*/
	else
	{
        flag_view_error_code = 0;   // 可显示正常温度了
	
	    bflg_error_code_alarm_ALH_mark = 0;         // 1
		bflg_error_code_alarm_ALL_mark = 0;         // 2
		bflg_error_code_alarm_power_off_mark = 0;   // 3
		//bflg_error_code_alarm_power_fault_mark = 0; // 4
		bflg_error_code_alarm_bat_discnnt_mark = 0; // 4 
		bflg_error_code_alarm_bat_fed_mark = 0;     // 5
		bflg_error_code_alarm_bat_low_mark = 0;     // 6
		bflg_error_code_alarm_TA_fault_mark = 0;    // 7 
		bflg_error_code_alarm_open_door_mark =0;    // 8
		bflg_error_code_alarm_THW_high_mark = 0;    // 9
        bflg_error_code_alarm_THW_fault_mark = 0;   // 10
		bflg_error_code_alarm_TC_fault_mark = 0;    // 11
		bflg_error_code_alarm_TE_fault_mark = 0;    // 12
		bflg_error_code_alarm_cond_dirty_mark = 0;  // 13
		//bflg_error_code_alarm_volt_mark = 0;        // 15	
	}
}

/***********************************************************************************************
功能描述 :

函数位置: 在 100ms 中调用  ------ok
***********************************************************************************************/
void view_error_code_delaytime (void)
{
    if (bflg_error_code_delaytime == 1) 
    {
        gss_error_code_delaytime ++;
        if (gss_error_code_delaytime >= 20)       // 100 * 20 = 2s
        {
            bflg_error_code_delaytime = 0;
            gss_error_code_delaytime = 0;

            if ((bflg_alarm_ALH == 1) && (bflg_error_code_alarm_ALH_mark ==0))		                //peak 1 高温报警标志 1
            {
                bflg_error_code_alarm_ALH_mark = 1;                                                 // 为1时显示时间到，循环到下一个显示
            }
            else if ((bflg_alarm_ALL == 1) && (bflg_error_code_alarm_ALL_mark ==0))                 // 2 低温报警
            {
                bflg_error_code_alarm_ALL_mark = 1;
            }
            else if ((bflg_alarm_THW_high == 1) && (bflg_error_code_alarm_THW_high_mark ==0))       // 3 环温报警
            {
                bflg_error_code_alarm_THW_high_mark =1;
            }

            else if ((bflg_TA_fault == 1) && (bflg_error_code_alarm_TA_fault_mark ==0))              // 4 主传感器故障
            {
                bflg_error_code_alarm_TA_fault_mark = 1;
            }
            else if ((bflg_TE_fault == 1) && (bflg_error_code_alarm_TE_fault_mark ==0))             // 5 热交换器传感器故障
            {
                bflg_error_code_alarm_TE_fault_mark = 1;
            }
            else if ((bflg_THW_fault == 1) && (bflg_error_code_alarm_THW_fault_mark ==0))           // 6 环温传感器故障
            {
                bflg_error_code_alarm_THW_fault_mark = 1;
            }
            else if ((bflg_TC_fault == 1) && (bflg_error_code_alarm_TC_fault_mark ==0))             // 7 冷凝传感器故障
            {
                bflg_error_code_alarm_TC_fault_mark = 1;
            }  
            else if ((bflg_alarm_bat_discnnt == 1) && (bflg_error_code_alarm_bat_discnnt_mark ==0)) // 8 电池未连接
            {
                bflg_error_code_alarm_bat_discnnt_mark = 1;
            }

            else if ((bflg_alarm_bat_fed == 1) && (bflg_error_code_alarm_bat_fed_mark ==0))         // 9 电池插反
            {
                bflg_error_code_alarm_bat_fed_mark =1;
            }
            else if ((bflg_alarm_bat_low == 1) && (bflg_error_code_alarm_bat_low_mark ==0))         // 10 电池电量低
            {
                bflg_error_code_alarm_bat_low_mark = 1;
            }

            else if ((bflg_alarm_power_off == 1) && (bflg_error_code_alarm_power_off_mark ==0))     // 11 电源断电
            {
                bflg_error_code_alarm_power_off_mark = 1;
            }
            else if ((bflg_alarm_open_door == 1) && (bflg_error_code_alarm_open_door_mark ==0))     // 12 开门
            {
                bflg_error_code_alarm_open_door_mark = 1;
            }
            else if ((bflg_alarm_cond_dirty == 1) && (bflg_error_code_alarm_cond_dirty_mark ==0))   // 13 冷凝器脏
            {
                bflg_error_code_alarm_cond_dirty_mark = 1;
            }
            /*else if ((bflg_alarm_power_fault == 1) && (bflg_error_code_alarm_power_fault_mark ==0))// 4
            {
                bflg_error_code_alarm_power_fault_mark = 1;
            }
            else if ((bflg_alarm_volt == 1) && (bflg_error_code_alarm_volt_mark ==0))   // 15
            {
                bflg_error_code_alarm_volt_mark = 1;
            }*/
        }
    }
}



/*********************************************************************************************************************************
function: 显示板上led灯的指示：
          1、低温压机运行灯的指示
          2、8个报警的闪烁指示
          3、新增的远程报警灯的指示

函数位置：10ms-----------------------------------------------ok
*********************************************************************************************************************************/
void lamp_disp_deal(void)        //灯显示处理程序，在100ms定时程序中调用
{
    //低温压机运行的指示灯
    if (bflg_comp21_running == 1)       // peak 低温压缩机运行标志
    {
        LED8 |= 0x20;
    }
    else
    {
        LED8 &= ~0x20; 
    }


	//闪烁的8个灯
    //----------------------------------
    if (bflg_alarm_ALH == 1)             //peak  高温报警标志 1 (统计报警使用的数量)
    { 
        if (bflg_led_loop_set == 1)
        {                                
            LED12 |= 0x01;                 
        }
        else
        {
            LED12 &= ~0x01;
        }
    }
    else
    {
        LED12 &= ~0x01;
    }
    //----------------------------------
    if (bflg_alarm_ALL == 1)             //peak 低温报警标志 2
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x02;
        }
        else
        {
            LED12 &= ~0x02;
        }
    }
    else
    {
        LED12 &= ~0x02;
    }
    //----------------------------------
    if (bflg_alarm_power_off == 1)       //peak 断电报警标志 3
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x04;
        }
        else
        {
            LED12 &= ~0x04;
        }
    }
    else
    {
        LED12 &= ~0x04;
    }
    //----------------------------------
    if (bflg_alarm_open_door == 1)       //peak  开门报警标志 4 (面板标注_电压超标)
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x08;
        }
        else
        {
            LED12 &= ~0x08;
        }
    }
    else
    {
        LED12 &= ~0x08;
    }
    //----------------------------------
    if (bflg_alarm_THW_high == 1)        //peak 环温过高报警标志 5
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x10;
        }
        else
        {
            LED12 &= ~0x10;
        }
    }
    else
    {
        LED12 &= ~0x10;
    }
    //----------------------------------
    if (bflg_alarm_cond_dirty == 1)      //peak  冷凝器脏报警标志 6
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x20;
        }
        else
        {
            LED12 &= ~0x20;
        }
    }
    else
    {
        LED12 &= ~0x20;
    }               
   // 电池未连接 、电池电量低、电池插反这三种报警都是闪烁同一个灯
    if ((bflg_alarm_bat_discnnt == 1) || (bflg_alarm_bat_low == 1) || (bflg_alarm_bat_fed == 1))  
    {                                     //peak  蓄电池故障  7 、8 、9
        if ((bflg_led_loop_set == 1))  
        {
            LED12 |= 0x40;
        }
        else
        {
            LED12 &= ~0x40;
        }
    }
    else
    {
        LED12 &= ~0x40;
    }     
    //----------------------------------
    //主、环温、冷凝、热交换这4种传感器故障都是闪烁这个灯
    if (bflg_alarm_T_fault == 1)         //peak 传感器故障报警标志 10、11、12、13 
    {
        if (bflg_led_loop_set == 1)
        {
            LED12 |= 0x80;
        }
        else
        {
            LED12 &= ~0x80;
        }
    }
    else
    {
        LED12 &= ~0x80;
    }

    //远程报警灯
    //高温、低温、断电、开门 任意一种报警就开远程
    if(bflg_alarm_ALH == 1 ||  bflg_alarm_ALL == 1 || bflg_alarm_power_off == 1 || bflg_alarm_open_door == 1)
    {
        RL_ALM_OUT_PIN = 0;    //1开  海尔这么要求的,报警时 = 0；
    }
    else
    {
        RL_ALM_OUT_PIN = 1;    //0关
        //bflg_alarm_output = 0;之前的标志，现在未用
    }
}

/*****************************************************************************************************************************
功能描述：显示箱内温度单独做的个函数

函数位置:  led_disp1_deal()-------------------------------------ok
*****************************************************************************************************************************/
void led_gss_TA_deal(void)
{
    guc_disp_rate = not_dot;
    
    if(gss_TA > 500)       //超过极限时显示极限值
    {
        led1_disp_val(50);  
    }
    else if(gss_TA < -990)
    {
        led1_disp_val(-99);  
    }
    else if(gss_TA > gss_set_parm_value[DISP_SET_TS])
    {
        led1_disp_val(gss_TA / 10 - 1);    //要求这么显示,已缩小10倍所以-1         
    }
    else
    {
        led1_disp_val(gss_TA / 10);            
    }
}

/*****************************************************************************************************************************
功能描述：断电时数码管显示和不显示的延时

函数位置:  1s-------------------------------------ok
*****************************************************************************************************************************/
void led_power_off_disp_delaytime(void)
{
    if(bflg_alarm_power_off == 1)   //断电
    {
        if(bflg_power_off_dis == 0) //此时黑屏30s后显示
        {
            gss_power_off_dis_delaytime++;
            if(gss_power_off_dis_delaytime >= 30)
            {
                gss_power_off_dis_delaytime = 0;
                bflg_power_off_dis = 1;
            }
        }
        else                       //此时显示5s后黑屏
        {
            gss_power_off_dis_delaytime++;
            if(gss_power_off_dis_delaytime >= 5)
            {
                gss_power_off_dis_delaytime = 0;
                bflg_power_off_dis = 0;
            }
        }
    }
    else
    {
        gss_power_off_dis_delaytime = 0;
        bflg_power_off_dis = 0;
    }
}








/****************************************END OF THE FILE***************************************************************************/
