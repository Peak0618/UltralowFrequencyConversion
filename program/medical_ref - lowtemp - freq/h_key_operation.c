/************************************************************************************************************************************************
overview:
        1、 
*************************************************************************************************************************************************/

#ifndef _KEY_OPERATION_C_
#define _KEY_OPERATION_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"

#include "h_type_define.h"
#include "h_key_operation.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_protect.h"
#include "h_bat_ctrl.h"

//peak add

#include "M_buzz.h"
#include "r_cg_timer.h"
#include "h_com.h"
#include "h_main.h"
#include "M_RTC.h"
#include "m_test.h"
//------------------------------------------------------------------------------
//函数声明
void key_mode_select_deal(void);     
void key_locked_deal(void); 
void key_locked_password_deal();
void key_unlocked_deal(void);  
void key_fac_password_deal(void); 
void key_set_parm_deal(void);
void key_user_parm_deal(void);
void key_factory_parm_deal(void);
void key_IC_password_deal(void);
void key_IC_register_deal(void); 
void key_debug_select_deal();
void key_debug_parm_deal(void);   
void key_RTC_time_parm_deal(void);                    
void key_RTC_time_parm_sent_usb_delaytime(void);   
void key_locked_delaytime(void);
void key_long_delaytime(void);     
void key_long_down_delaytime(void);       
void key_long_set_delaytime(void);
void key_long_updown_delaytime(void);
void key_long_setdown_delaytime(void);
void read_key_delaytime(void);    

//peak add
void key_buzz_cancel(void);
void bat_con_check_again(void);     
void led_loop_delaytime(void);      
void led_disp1_flicker_delaytime(void);
void key_factory_AT_test(void);
void key_factory_CL4_init(void);        
void first_power_on_self_test(void);       

void IC_unlock_deal(void);                         
void IC_unlock_delaytime(void);             
void key_password_long_valid_delaytime(void);
void key_clear_password_long_falg(void);




//------------------------------------------------------------------------------
//变量定义
flag_type flg_disp,flg_disp1,flg_error_code_mark,flg_save1;


//----------------------------------------------------------
//按键宏定义
uint8_t   guc_key_val;        //按键键值

//----------------------------------------------------------
//显示数组宏定义
uint8_t   guc_led_buffer[12]; //数码管寄存器  //peak LDE1~LED12 对应 [0] ~ [11]
//----------------------------------------------------------
const uint8_t guc_nmb_table[]=
{
  data_0  // 0
, data_1  // 1
, data_2  // 2
, data_3  // 3
, data_4  // 4
, data_5  // 5
, data_6  // 6
, data_7  // 7
, data_8  // 8
, data_9  // 9
};
//------------------------------------------------------------------------------
int8_t    gsc_disp_user1_mode;      //显示user1模式下的几种不同显示方式
int8_t    gsc_disp_user2_mode;      //显示user2模式下的几种不同显示方式
int8_t    gsc_disp_factory_mode;    //显示factory模式下的几种不同显示方式
int8_t    gsc_disp_debug_mode;      //显示海尔调试模式下的几种不同显示方式
uint8_t   guc_disp1_mode;
int8_t    gsc_disp1_set_mode;       //显示设定模式
int8_t    gsc_disp1_see_mode;       //显示查询模式

int8_t    gsc_disp1_para_mode;     //显示参数模式

//------------------------------------------------------------------------------
int16_t   gss_key_long_delaytimer;         //长按键延时计时器
int16_t   gss_key_locked_delaytimer;       //按键锁定延时计时器
int16_t   gss_key_long_down_delaytimer;    //锁定状态时长按down键计时
int16_t   gss_key_long_updown_delaytimer;  //长按up+down键计时
int16_t   gss_key_long_setdown_delaytimer; //长按set+down键计时
int16_t   gss_key_long_set_delaytimer;     //长按set键计时

int8_t    gsc_password_val;             //密码值

int16_t   gss_disp_see_buffer[6];       //显示查询信息寄存器
int16_t   gss_read_key_delaytimer;      //读键值延时计时器

int16_t    gss_user_CL1_flicker_cnt;   //注销IC卡时CL1闪烁的次数
int16_t    gss_fac_AT_test_delaytime;  //测试AT指令时全亮的时间
int16_t    gss_fac_CL3_flicker_cnt;    //初始IC密码时CL3闪烁的次数
int16_t    gss_fac_CL4_flicker_cnt;    //恢复出厂设置CL4闪烁的次数

//peak add
int16_t   gss_key_long_buz_delaytimer;   //长按键延时计时器
int16_t   gss_led_loop_delaytime;        //LED的闪烁频率设定
int16_t   guc_IC_register_delaytime;     //判断长按组合键的的时间是否达到进入注册状态
int16_t   gsc_IC_password_val;           //IC注册模式的密码值
int8_t    guc_IC_erasure_delaytime;      //IC注销时间判断
int8_t    gss_disp1_flicker_delaytime;   //"箱内温度设定" "高温报警设定" "低温报警设定" 闪烁延时
int8_t    guc_IC_password_cnt;           //进入IC注册模式时输入错误密码次数计数
int8_t    guc_password_cnt;              //进入工厂模式时输入错误密码次数计数

uint8_t   gss_self_test_delaytime;       //系统自检时的闪烁延时
uint8_t   gss_self_test_cnt;             //系统自检时的闪烁次数计数

uint8_t  guc_lock_delaytimer;            // IC卡开锁的打开的时间


int8_t  guc_user_write_time_value;      //设置RTC时间的临时参数
uint8_t gss_usb_set_time_delaytime;     //usb设置时间延时
int8_t  gsc_disp_unlocked_password;     //解锁密码
int16_t  gss_key_password_long_valid_delaytime; //输入密码时长按计时，后可步进10增加

//包工新功能书增加的参数
int16_t gss_TS_old_value;               //设定前的旧值
int16_t gss_ALH_old_value;
int16_t gss_ALL_old_value;


uint8_t   guc_disp_mode;                                  //总的UI模式
int8_t    gsc_disp_parm_type;                             //不同的参数类型,所有的参数都用这个
int16_t   gss_key_temp_value;                             //参数的临时值,所有的参数都用这个

int16_t   gss_set_parm_value[DISP_SET_PARM_SUM];          //设定参数值   
int16_t   gss_user_parm_value[DISP_USER_PARM_SUM];        //用户参数值   
int16_t   gss_factory_parm_value[DISP_FACTORY_PARM_SUM];  //工厂参数值



/**************************************************************************************************************************************
函数功能：模式选择
          
          
函数位置: 主循环--------------------------ok
**************************************************************************************************************************************/
void  key_mode_select_deal(void)   //key_mode_select_deal       
{    
    if (bflg_test_type == 1)                     //如果是测试版显示板
    {
        if (guc_key_val == KEY_SET)              //如果是设置键
        {
            bflg_test_mode = 1;                  //置测试模式标志
        }
        else if(guc_key_val == KEY_BUZ)
        {
        	bflg_test_mode_RTC = 1;
        	test_mode_RTC_circuit();
        }
    }
    else if (guc_disp_mode == DISP_LOCKED)          //锁定模式
    {
        key_locked_deal();                    
    }    
    else if(guc_disp_mode == DISP_UNLOCKED_PASSWORD)//解锁密码
    {
        key_locked_password_deal();
    }
    else if(guc_disp_mode == DISP_UNLOCKED)         //解锁模式
    {
        key_unlocked_deal();  
    }
    else if(guc_disp_mode == DISP_SET)             //参数设置模式
    {
        key_set_parm_deal();
    }
    else if(guc_disp_mode == DISP_USER)            //用户模式
    {
        key_user_parm_deal();
    }
    else if (guc_disp_mode == DISP_FAC_PASSWORD)   //工厂密码模式
    {
        key_fac_password_deal();                  
    }
    else if(guc_disp_mode == DISP_FACTORY)         //工厂模式
    {
        key_factory_parm_deal();
    }     
    else if (guc_disp_mode == DISP_IC_PASSWORD)    //IC卡注册密码模式
    {        
        key_IC_password_deal();  
    }
    else if(guc_disp_mode == DISP_IC_REG)          //IC 注册模式
    {
        key_IC_register_deal();                
    }
    else if(guc_disp_mode == DISP_TIME_SET)         //RTC时间设置模式
    {
        key_RTC_time_parm_deal();
    }
    else if(guc_disp_mode == DISP_DEBUG_SELECT)    //选择调试模式
    {
        key_debug_select_deal();
    }
    else if(guc_disp_mode == DISP_DEBUG)            //调试模式
    {
        key_debug_parm_deal();                      //锁定set5s-->进入海尔使用的调试参数模式，当正式版本时去掉
    }
}

/***********************************************************************************************************************************
function: 锁定模式下可进行的操作
          1、按上下键进入解锁密码模式
          2、长按set键进入调试参数模式
          
函数位置: 在 read_key_deal()中，又在主循环中 ----ok          
***********************************************************************************************************************************/
void key_locked_deal(void)             
{
    //-------------------------------------------------------------
    if ((guc_key_val == KEY_UP)||(guc_key_val == KEY_DOWN)) //上或下键进入锁定密码        
    {
        guc_disp_mode = DISP_UNLOCKED_PASSWORD; 
        gsc_disp_unlocked_password = 0;
        key_clear_password_long_falg();         //输入密码时长按步进10
    }
    
    //----------------------------------------------------------------
    if (guc_key_val == KEY_SET)                //长按set进入调试参数      
    {
        if (bflg_key_long_set_valid == 0)             
        {   
             
            if (bflg_key_long_set_delaytime == 0)    
            {
                bflg_key_long_set_delaytime = 1;     
                gss_key_long_set_delaytimer = 0;
            }
        }
        else                                      
        {
            bflg_key_long_set_valid = 0;
            guc_disp_mode = DISP_DEBUG_SELECT;  //进入海尔参数调试模式，正式版本时去掉
        }
    }
    else
    {
        bflg_key_long_set_delaytime = 0;
        gss_key_long_set_delaytimer = 0;

        bflg_key_long_set_valid = 0;
    }
}

/******************************************************************************************************************************************************
函数功能：锁定的密码模式,输入正确的解锁密码后长按set键5s进入解锁模式

函数位置：read_key_deal()中-------------------ok
********************************************************************************************************************************************************/
void key_locked_password_deal(void)
{  
    if (guc_key_val == KEY_BUZ)
    {
        guc_disp_mode = DISP_LOCKED;   
    }
    else if (guc_key_val == KEY_UP)  //密码选择
    {
        if (bflg_key_password_long_valid == 0)
        {
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            gsc_disp_unlocked_password++;
            if (gsc_disp_unlocked_password > 99)
            {
                gsc_disp_unlocked_password = 0;
            }            
        }
        else
        {
            gsc_disp_unlocked_password += 10;
            if (gsc_disp_unlocked_password > 99)
            {
                gsc_disp_unlocked_password = 0;
            }  
        } 
    }
    else if (guc_key_val == KEY_DOWN)
    {
        if (bflg_key_password_long_valid == 0)
        {
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            gsc_disp_unlocked_password--;
            if (gsc_disp_unlocked_password < 0)
            {
                gsc_disp_unlocked_password = 99;
            }             
        }
        else
        {
            gsc_disp_unlocked_password -= 10;
            if (gsc_disp_unlocked_password < 0)
            {
                gsc_disp_unlocked_password = 99;
            } 
        }
    }
    else if (guc_key_val == KEY_SET)
    {
        key_clear_password_long_falg();
        
        if(gsc_disp_unlocked_password == gss_user_parm_value[DISP_USER_PS1])
        {
            if (bflg_key_long_valid == 0)           
            {
                if (bflg_key_long_delaytime == 0)    
                {
                    bflg_key_long_delaytime = 1;     
                    gss_key_long_delaytimer = 0;
                }
            }
            else                                     //如果长按set 5s
            {
                bflg_key_long_valid = 0;
                guc_disp_mode = DISP_UNLOCKED; 
                bflg_unlocked_deal = 1;
            }
        }
    }
    else
    {
        bflg_key_long_delaytime = 0;
        gss_key_long_delaytimer = 0;        
        bflg_key_long_valid = 0;  
        
        key_clear_password_long_falg();
    }
}

/******************************************************************************
 function: 解锁模式
           1、按一下set键进入set层 
           2、长按 up    5s，进入user层
           3、长按 dowe  5s，进入IC卡
           4、长按 up + down  5s,进入工厂参数；

函数位置:    read_key_deal()中-------------------------ok         
******************************************************************************/
void key_unlocked_deal(void)
{   
    if(bflg_unlocked_deal == 1)        
    {
        if (guc_key_val != KEY_SET)     //松开了set键或是按下其他的组合键，才可继续操作
        {
            bflg_unlocked_deal = 0;
        }   
    }
    else
    {
        if (guc_key_val == KEY_SET)                  //进入设置参数层
        {          
            guc_disp_mode = DISP_SET;   
            gsc_disp_parm_type = 0;
            gss_key_temp_value = 0;
            bflg_parm_dis_form = 0;
            bflg_parm_save = 0;
        }
        
        //------------------------------------------------------------------
        if (guc_key_val == KEY_UP)                   //长按up 5s进入user层
        { 
            if (bflg_key_long_valid == 0)            
            {
                if (bflg_key_long_delaytime == 0)    
                {
                    bflg_key_long_delaytime = 1;     
                    gss_key_long_delaytimer = 0;
                }
            }
            else                                     
            {
                bflg_key_long_valid = 0;
                
                guc_disp_mode = DISP_USER;          //进入用户模式
                gsc_disp_parm_type = 0;
                gss_key_temp_value = 0;
                bflg_parm_dis_form = 0;
                bflg_parm_save = 0;
                gss_user_parm_value[DISP_USER_T2] = 0; //都从P1开始
                
                key_clear_password_long_falg();        //为了IC、PS1参数长按步进10用
            }
        }    
        else
        {
            bflg_key_long_delaytime = 0;
            gss_key_long_delaytimer = 0;
            bflg_key_long_valid = 0;
        }
        
        //-----------------------------------------------------------------
        if (guc_key_val == KEY_UP_AND_DOWN)        //长按"up+down"5s进入工厂密码passward层  
        { 
            if (bflg_key_long_updown_valid == 0)            
            {
                if (bflg_key_long_updown_delaytime == 0)    
                {
                    bflg_key_long_updown_delaytime = 1;     
                    gss_key_long_updown_delaytimer = 0;
                }
            }
            else                                     
            {                   
                bflg_key_long_updown_valid = 0;              
                
                guc_disp_mode = DISP_FAC_PASSWORD;       
                
                gsc_password_val = 0;                //这个密码已经清0
                guc_password_cnt = 0;
                key_clear_password_long_falg();      //为了PS2参数长按步进10用
            }
        }
        else
        {
            bflg_key_long_updown_delaytime = 0;     
            gss_key_long_updown_delaytimer = 0;
            bflg_key_long_updown_valid = 0; 
        }
        
        //-----------------------------------------------------------------
        if(guc_key_val == KEY_DOWN)            //长按 down 5s进入IC密码注册层  
        {
            if (bflg_key_long_setdown_valid == 0)            
            {
                if (bflg_key_long_setdown_delaytime == 0)    
                {
                    bflg_key_long_setdown_delaytime = 1;     
                    gss_key_long_setdown_delaytimer = 0;
                }
            }
            else                                     
            {
                bflg_key_long_setdown_valid = 0; 
                
                bflg_IC_register_state = 1;         //表示进入注册状态
                
                guc_disp_mode = DISP_IC_PASSWORD;   //进入IC注册密码状态
                gsc_IC_password_val = 0;                              
                guc_IC_password_cnt = 0;

                key_clear_password_long_falg();    //输入密码时长按步进10
            }
        } 
        else
        {
            bflg_key_long_setdown_delaytime = 0;    
            gss_key_long_setdown_delaytimer = 0;    
            bflg_key_long_setdown_valid = 0; 
        }
    }  
}

/******************************************************************************
函数功能: 工厂密码模式

函数位置：主循环-----------------------------------------ok
******************************************************************************/
void key_fac_password_deal(void)   
{  
    if (guc_key_val == KEY_BUZ)
    {
        guc_disp_mode = DISP_UNLOCKED;   
    }
    else if (guc_key_val == KEY_UP)
    {
        if (bflg_key_password_long_valid == 0)   
        {                                        
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            
            gsc_password_val++;                   
            if (gsc_password_val > 99)
            {
                gsc_password_val = 0;
            }
        }
        else
        {
            gsc_password_val += 10;    // ??
            if (gsc_password_val > 99)
            {
                gsc_password_val = 0;
            }
        }
    }
    else if (guc_key_val == KEY_DOWN)
    {
        if (bflg_key_password_long_valid == 0)
        {
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            
            gsc_password_val--;
            if (gsc_password_val < 0)
            {
                gsc_password_val = 99;
            }
        }
        else
        {
            gsc_password_val -= 10;
            if (gsc_password_val < 0)
            {
                gsc_password_val = 99;
            }
        }
    }
    else if (guc_key_val == KEY_SET)
    {
        key_clear_password_long_falg();
        
        guc_password_cnt++;
        
        if(guc_password_cnt <= 3)
        {
            if (gsc_password_val == gss_factory_parm_value[DISP_FACTORY_PS2]) //改为密码可调 
            {
                key_clear_password_long_falg();                    //PS2长按也步进10
                
                guc_disp_mode = DISP_FACTORY;                      //正常模式
                gsc_disp_parm_type = 0;
                gss_key_temp_value = 0;
                bflg_parm_dis_form = 0;
                bflg_parm_save = 0;
                
                gsc_password_val = 0;                              //运行进入时已清此处再次清
                guc_password_cnt = 0;                              //若密码正确则清错误计数
            }
            else if(guc_password_cnt >= 3)                         //输入3次错误则进入锁定
            {
                gsc_password_val = 0;
                guc_password_cnt = 0;
                
                guc_disp_mode = DISP_LOCKED;
                gsc_disp_user1_mode = DISP_SET_TS;    
                gsc_disp_user2_mode = DISP_USER_dA; 
            }
        }
    }
    else
    {
        key_clear_password_long_falg();
    }
}
/******************************************************************************************************************************
函数功能:    设置参数层对 TS、ALH、ALL 这三个参数操作


函数位置:   read_key_deal()中---------------------------ok
*******************************************************************************************************************************/
void key_set_parm_deal(void)
{
    int16_t lss_disp1_TS_temp;
    lss_disp1_TS_temp = gss_set_parm_value[DISP_SET_TS];  
    //-----------------------------------------------------------------
    if (guc_key_val == KEY_BUZ)                    
    {
        guc_disp_mode = DISP_UNLOCKED;              //返回上一层
        gsc_disp_parm_type = 0;
        gss_key_temp_value = 0;
        bflg_parm_dis_form = 0;
        bflg_parm_save = 0;
    }
    //-----------------------------------------------------------------
    else if(guc_key_val == KEY_SET)               //切换TS ALH ALL三个参数
    {
        if(bflg_parm_save == 1)    
        {   
            bflg_parm_save = 0;
            
            if(gsc_disp_parm_type == DISP_SET_TS)
            {
                if(gss_set_parm_value[DISP_SET_TS] != gss_key_temp_value)    
                {
                    gss_TS_old_value = gss_set_parm_value[DISP_SET_TS];
                    gss_set_parm_value[DISP_SET_TS] = gss_key_temp_value;
                    ram_para[num_comp2_TS] = gss_set_parm_value[DISP_SET_TS];                                                                    //!!!!!!!
                    eeprom_write(num_comp2_TS, ram_para[num_comp2_TS]);  //现在需要按下set键才保存

                    bflg_sent_usb_TS = 1;      // 6
                }

                if(gss_set_parm_value[DISP_SET_TS] != lss_disp1_TS_temp)                    //如果设定值改变了
                {
                    gss_set_parm_value[DISP_SET_ALH] = gss_set_parm_value[DISP_SET_TS] + (gss_set_parm_value[DISP_SET_ALH] - lss_disp1_TS_temp);
                    if(gss_set_parm_value[DISP_SET_ALH] >= 0)
                    {
                        gss_set_parm_value[DISP_SET_ALH] = 0;
                    }
                    else if(gss_set_parm_value[DISP_SET_ALH] <= (gss_set_parm_value[DISP_SET_TS] + 50))
                    {
                        gss_set_parm_value[DISP_SET_ALH] = gss_set_parm_value[DISP_SET_TS] + 50;
                    }

                    gss_set_parm_value[DISP_SET_ALL] = gss_set_parm_value[DISP_SET_TS] - (lss_disp1_TS_temp - gss_set_parm_value[DISP_SET_ALL]);
                    if(gss_set_parm_value[DISP_SET_ALL] >= (gss_set_parm_value[DISP_SET_TS] - 50))
                    {
                        gss_set_parm_value[DISP_SET_ALL] = gss_set_parm_value[DISP_SET_TS] - 50;
                    }
                    else if(gss_set_parm_value[DISP_SET_ALL] <= -990)
                    {
                        gss_set_parm_value[DISP_SET_ALL] = -990;
                    }

                    ram_para[num_comp2_ALH] = gss_set_parm_value[DISP_SET_ALH];                                                                    //!!!!!!!
                    eeprom_write(num_comp2_ALH, ram_para[num_comp2_ALH]); 
                    
                    ram_para[num_comp2_ALL] = gss_set_parm_value[DISP_SET_ALL];                                                                    //!!!!!!!
                    eeprom_write(num_comp2_ALL, ram_para[num_comp2_ALL]);                
                }
            }
            else if(gsc_disp_parm_type == DISP_SET_ALH)
            {
                if(gss_set_parm_value[DISP_SET_ALH] != gss_key_temp_value)
                {
                    gss_ALH_old_value = gss_set_parm_value[DISP_SET_ALH];
                    gss_set_parm_value[DISP_SET_ALH] = gss_key_temp_value;
                    ram_para[num_comp2_ALH] = gss_set_parm_value[DISP_SET_ALH];                                                                    //!!!!!!!
                    eeprom_write(num_comp2_ALH, ram_para[num_comp2_ALH]);  
                
                    bflg_sent_usb_ALHS = 1;  // 4
                }
            }
            else if(gsc_disp_parm_type == DISP_SET_ALL)
            {
                if(gss_set_parm_value[DISP_SET_ALL] != gss_key_temp_value)
                {
                    gss_ALL_old_value = gss_set_parm_value[DISP_SET_ALL];
                    gss_set_parm_value[DISP_SET_ALL] = gss_key_temp_value;
                    ram_para[num_comp2_ALL] = gss_set_parm_value[DISP_SET_ALL];                                                                    //!!!!!!!
                    eeprom_write(num_comp2_ALL, ram_para[num_comp2_ALL]);   
                
                    bflg_sent_usb_ALLS = 1; // 5
                }
            }
        }
        
        gsc_disp_parm_type++;                     
        if (gsc_disp_parm_type > DISP_SET_ALL)
        {
            gsc_disp_parm_type = DISP_SET_TS;
        }
    }
    //-----------------------------------------------------------------
    else if (guc_key_val == KEY_UP)          
    {
        if(bflg_parm_save == 0)
        {
            bflg_parm_save = 1;
            gss_key_temp_value = gss_set_parm_value[gsc_disp_parm_type];
        }
        //gss_key_temp_value++;         

        if(gsc_disp_parm_type == DISP_SET_TS)
        {
            gss_key_temp_value += EEPROM_init[num_comp2_TS][3];
            if (gss_key_temp_value > ram_para2[num_comp2_TS_max])   
            {
                gss_key_temp_value = ram_para2[num_comp2_TS_min];
            }
        }
        else if(gsc_disp_parm_type == DISP_SET_ALH)
        {
            gss_key_temp_value += EEPROM_init[num_comp2_ALH][3];
            if (gss_key_temp_value > 0)   
            {
                gss_key_temp_value = gss_set_parm_value[DISP_SET_TS]+50;
            }
        }
        else if(gsc_disp_parm_type == DISP_SET_ALL)
        {
            gss_key_temp_value += EEPROM_init[num_comp2_ALL][3];
            if (gss_key_temp_value > (gss_set_parm_value[DISP_SET_TS]-50))        //放大10倍
            {
                gss_key_temp_value = -990;
            }
        }
    }
    //-----------------------------------------------------------------
    else if(guc_key_val == KEY_DOWN)
    {
        if(bflg_parm_save == 0)
        {
            bflg_parm_save = 1;
            gss_key_temp_value = gss_set_parm_value[gsc_disp_parm_type];
        }
        //gss_key_temp_value++;         

        if(gsc_disp_parm_type == DISP_SET_TS)
        {
            gss_key_temp_value -= EEPROM_init[num_comp2_TS][3];
            if (gss_key_temp_value < ram_para2[num_comp2_TS_min])   
            {
                gss_key_temp_value = ram_para2[num_comp2_TS_max];
            }

        }
        else if(gsc_disp_parm_type == DISP_SET_ALH)
        {
            gss_key_temp_value -= EEPROM_init[num_comp2_ALH][3];
            if (gss_key_temp_value < (gss_set_parm_value[DISP_SET_TS]+50))   
            {
                gss_key_temp_value = 0;
            }
        }
        else if(gsc_disp_parm_type == DISP_SET_ALL)
        {
            gss_key_temp_value -= EEPROM_init[num_comp2_ALL][3];
            if (gss_key_temp_value < -990)                      //海尔只有3位数码管
            {
                gss_key_temp_value = gss_set_parm_value[DISP_SET_TS]-50;
            }
        }
    }
}


/********************************************************************************************************************************
函数功能: 用户参数层的操作        

guc_disp_mode                     
gsc_disp_parm_type             
gss_key_temp_value
bflg_parm_dis_form
bflg_parm_save

函数位置:   read_key_deal()中---------------------------ok
*********************************************************************************************************************************/
void key_user_parm_deal(void)
{
    if (guc_key_val == KEY_BUZ)                     
    {
        if(bflg_parm_dis_form == 0)            //显示参数代号
        {
            guc_disp_mode = DISP_UNLOCKED;         
            gsc_disp_parm_type = 0;            //哪个参数
            gss_key_temp_value = 0;            //临时未保存的值
            bflg_parm_dis_form = 0;            //显示标示还是值
            bflg_parm_save = 0;                //是否需保存
        }
        else                                        //如果此时显示的是参数的值，则还是在此层，返回到此层的参数代码                              
        {    
            gss_key_temp_value = 0;             
            bflg_parm_dis_form = 0;                
            bflg_parm_save = 0;
        }
    }
    else
    {
        if (bflg_parm_dis_form == 0)         //显示参数标示   
        {
            if (guc_key_val == KEY_SET)         
            {
                if(gsc_disp_parm_type == DISP_USER_CL1)   //special 进入IC注销
                {                   
                    guc_disp_mode = DISP_IC_PASSWORD;      
                    gsc_IC_password_val = 0;                       
                    guc_IC_password_cnt = 0;             
                }
                else
                {
                    bflg_parm_dis_form = 1;        
                }
            }
            else
            {
                if (guc_key_val == KEY_UP)             
                {
                    gsc_disp_parm_type++;
                    if (gsc_disp_parm_type > DISP_USER_CL1)
                    {
                        gsc_disp_parm_type = DISP_USER_dA;
                    }
                }
                else if (guc_key_val == KEY_DOWN)    
                {
                    gsc_disp_parm_type--;
                    if (gsc_disp_parm_type < DISP_USER_dA) 
                    {
                        gsc_disp_parm_type = DISP_USER_CL1;
                    }
                }
            }
        }
        else   //如果是显示设定值                            
        {
            if (guc_key_val == KEY_SET) //如果是设置键   
            {
                key_clear_password_long_falg();
                
                if(gsc_disp_parm_type == DISP_USER_T2)                     //special
                {
                    bflg_parm_save = 0;
                    guc_disp_mode = DISP_TIME_SET;                         //进入时间设置层
                    
                    if(gss_user_parm_value[DISP_USER_T2] == 0)                   //P1 年      //获取此时的时间值
                    {
                        guc_user_write_time_value = guc_RTC_r_year;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 1)              //P2 月
                    {
                        guc_user_write_time_value = guc_RTC_r_month;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 2)              //P3 日
                    {
                        guc_user_write_time_value = guc_RTC_r_day;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 3)              //P4 时
                    {
                        guc_user_write_time_value = guc_RTC_r_hour;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 4)              //P5 分
                    {
                        guc_user_write_time_value = guc_RTC_r_minute;
                    }
                }
                else
                {
                    bflg_parm_dis_form = 0;      //显示标示
                    
                    if(bflg_parm_save == 1)
                    {
                        bflg_parm_save = 0;
                        if(gss_user_parm_value[gsc_disp_parm_type] != gss_key_temp_value)
                        {
                            gss_user_parm_value[gsc_disp_parm_type] = gss_key_temp_value;
                            
                            if(gsc_disp_parm_type == DISP_USER_dA)
                            {
                                ram_para[num_door_alarm_delaytime] = gss_user_parm_value[gsc_disp_parm_type];                                                                    //!!!!!!!
                                eeprom_write(num_door_alarm_delaytime, ram_para[num_door_alarm_delaytime]);
                            }
                            else if(gsc_disp_parm_type == DISP_USER_T1)
                            {
                                ram_para[num_usb_read_delaytime] = gss_user_parm_value[gsc_disp_parm_type];                                                                    //!!!!!!!
                                eeprom_write(num_usb_read_delaytime, ram_para[num_usb_read_delaytime]);
                                
                                bflg_sent_usb_T1 = 1;
                            }
                            else if(gsc_disp_parm_type == DISP_USER_P6)
                            {
                                ram_para[num_usb_P6] = gss_user_parm_value[gsc_disp_parm_type];                                                                    //!!!!!!!
                                eeprom_write(num_usb_P6, ram_para[num_usb_P6]);
                                
                                bflg_sent_usb_P6 = 1;
                            }
                            else if(gsc_disp_parm_type == DISP_USER_IC)
                            {
                                ram_para[num_IC_password] = gss_user_parm_value[gsc_disp_parm_type];                                                                    //!!!!!!!
                                eeprom_write(num_IC_password, ram_para[num_IC_password]);
                            }
                            else if(gsc_disp_parm_type == DISP_USER_PS1)
                            {
                                ram_para[num_password_PS1] = gss_user_parm_value[gsc_disp_parm_type];                                                                    //!!!!!!!
                                eeprom_write(num_password_PS1, ram_para[num_password_PS1]);
                            }
                            else if(gsc_disp_parm_type == DISP_USER_CL1)
                            {
                                //不保存
                            }                        
                        }
                    }
                }
            }
            else
            {
                if (guc_key_val == KEY_UP)             
                {
                    if(bflg_parm_save == 0)
                    {
                        bflg_parm_save = 1;
                        gss_key_temp_value = gss_user_parm_value[gsc_disp_parm_type];
                    }
                    //gss_key_temp_value++;         
                    
                    if(gsc_disp_parm_type == DISP_USER_dA)
                    {
                        gss_key_temp_value += EEPROM_init[num_door_alarm_delaytime][3];
                        if (gss_key_temp_value > ram_para2[num_door_alarm_delaytime_max])   
                        {
                            gss_key_temp_value = ram_para2[num_door_alarm_delaytime_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_T1)
                    {
                        gss_key_temp_value += EEPROM_init[num_usb_read_delaytime][3];
                        if (gss_key_temp_value > ram_para2[num_usb_read_delaytime_max])   
                        {
                            gss_key_temp_value = ram_para2[num_usb_read_delaytime_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_T2)
                    {
                        gss_user_parm_value[DISP_USER_T2] ++;
                        if(gss_user_parm_value[DISP_USER_T2] > 4)  //P1-P5显示时+1
                        {
                            gss_user_parm_value[DISP_USER_T2] = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_P6)
                    {
                        gss_key_temp_value += EEPROM_init[num_usb_P6][3];
                        if (gss_key_temp_value > ram_para2[num_usb_P6_max])   
                        {
                            gss_key_temp_value = ram_para2[num_usb_P6_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_IC)
                    {
                        if (bflg_key_password_long_valid == 0)   
                        {
                            if(bflg_key_password_long_valid_delaytime == 0)
                            {
                                bflg_key_password_long_valid_delaytime = 1;
                                gss_key_password_long_valid_delaytime = 0;
                            }
                            gss_key_temp_value += EEPROM_init[num_IC_password][3];
                            if (gss_key_temp_value > ram_para2[num_IC_password_max])   
                            {
                                gss_key_temp_value = ram_para2[num_IC_password_min];
                            }                            
                        }
                        else
                        {
                            gss_key_temp_value += 10;
                            if (gss_key_temp_value > ram_para2[num_IC_password_max])   
                            {
                                gss_key_temp_value = ram_para2[num_IC_password_min];
                            } 
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_PS1)
                    {
                        if (bflg_key_password_long_valid == 0)   
                        {
                            if(bflg_key_password_long_valid_delaytime == 0)
                            {
                                bflg_key_password_long_valid_delaytime = 1;
                                gss_key_password_long_valid_delaytime = 0;
                            }
                            gss_key_temp_value ++;
                            if (gss_key_temp_value > 99)   
                            {
                                gss_key_temp_value = 0;
                            }
                        }
                        else
                        {
                            gss_key_temp_value += 10;
                            if (gss_key_temp_value > 99)   
                            {
                                gss_key_temp_value = 0;
                            }
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_CL1)
                    {
                        //不会进
                    }           
                }
                else if (guc_key_val == KEY_DOWN)       //DOWN
                {
                    if(bflg_parm_save == 0)
                    {
                        bflg_parm_save = 1;
                        gss_key_temp_value = gss_user_parm_value[gsc_disp_parm_type];
                    }
                    //gss_key_temp_value++;         
                    
                    if(gsc_disp_parm_type == DISP_USER_dA)
                    {
                        gss_key_temp_value -= EEPROM_init[num_door_alarm_delaytime][3];
                        if (gss_key_temp_value < ram_para2[num_door_alarm_delaytime_min])   
                        {
                            gss_key_temp_value = ram_para2[num_door_alarm_delaytime_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_T1)
                    {
                        gss_key_temp_value -= EEPROM_init[num_usb_read_delaytime][3];
                        if (gss_key_temp_value < ram_para2[num_usb_read_delaytime_min])   
                        {
                            gss_key_temp_value = ram_para2[num_usb_read_delaytime_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_T2)
                    {
                        gss_user_parm_value[DISP_USER_T2] --;
                        if(gss_user_parm_value[DISP_USER_T2] < 0)
                        {
                            gss_user_parm_value[DISP_USER_T2] = 4;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_P6)
                    {
                        gss_key_temp_value -= EEPROM_init[num_usb_P6][3];
                        if (gss_key_temp_value < ram_para2[num_usb_P6_min])   
                        {
                            gss_key_temp_value = ram_para2[num_usb_P6_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_IC)
                    {
                        if (bflg_key_password_long_valid == 0)   
                        {
                            if(bflg_key_password_long_valid_delaytime == 0)
                            {
                                bflg_key_password_long_valid_delaytime = 1;
                                gss_key_password_long_valid_delaytime = 0;
                            }
                            gss_key_temp_value -= EEPROM_init[num_IC_password][3];
                            if (gss_key_temp_value < ram_para2[num_IC_password_min])   
                            {
                            gss_key_temp_value = ram_para2[num_IC_password_max];
                            }
                        }
                        else
                        {
                            gss_key_temp_value -= 10;
                            if (gss_key_temp_value < ram_para2[num_IC_password_min])   
                            {
                            gss_key_temp_value = ram_para2[num_IC_password_max];
                            }
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_USER_PS1)
                    {
                        if (bflg_key_password_long_valid == 0)   
                        {
                            if(bflg_key_password_long_valid_delaytime == 0)
                            {
                                bflg_key_password_long_valid_delaytime = 1;
                                gss_key_password_long_valid_delaytime = 0;
                            }
                            gss_key_temp_value --;
                            if (gss_key_temp_value < 0)   
                            {
                                gss_key_temp_value = 99;
                            }                            
                        }
                        else
                        {
                            gss_key_temp_value -= 10;
                            if (gss_key_temp_value < 0)   
                            {
                                gss_key_temp_value = 99;
                            }  
                        }
                    }                    
                    else if(gsc_disp_parm_type == DISP_USER_CL1)
                    {
                        //不会进
                    } 
                }
                else
                {
                    key_clear_password_long_falg();
                }                
            }
        }
    }
}


/*******************************************************************************************************************************
函数功能: 工厂参数层的操作        
             
函数位置:   read_key_deal()中---------------------------ok
********************************************************************************************************************************/
void key_factory_parm_deal(void)
{
    if (guc_key_val == KEY_BUZ)            
    {  
        if(bflg_parm_dis_form == 0)             
        {
            guc_disp_mode = DISP_UNLOCKED;         
            gsc_disp_parm_type = 0;            //哪个参数
            gss_key_temp_value = 0;            //临时未保存的值
            bflg_parm_dis_form = 0;            //显示0标示 1值
            bflg_parm_save = 0;                //是否需保存
        }
        else                          
        {
            //gsc_disp_parm_type = 0;          //显示的参数标志不变
            gss_key_temp_value = 0;           
            bflg_parm_dis_form = 0;            
            bflg_parm_save = 0;                
        }        
    }
    else
    {
        if(bflg_parm_dis_form == 0)    //标示
        {
            if(guc_key_val == KEY_SET)
            {
                bflg_parm_dis_form = 1;
            }
            else
            {
                if (guc_key_val == KEY_UP)             
                {
                    gsc_disp_parm_type++;
                    if (gsc_disp_parm_type > DISP_FACTORY_HC)
                    {
                        gsc_disp_parm_type = DISP_FACTORY_CL2;
                    }
                }
                else if (guc_key_val == KEY_DOWN)    
                {
                    gsc_disp_parm_type--;
                    if (gsc_disp_parm_type < DISP_FACTORY_CL2) 
                    {
                        gsc_disp_parm_type = DISP_FACTORY_HC;
                    }
                }
            }
        }
        else                           //bflg_parm_dis_form = 1 显参数值 
        {
            if(guc_key_val == KEY_SET)
            {
                key_clear_password_long_falg();
                bflg_parm_dis_form = 0;
                
                if(bflg_parm_save == 1)
                {
                    bflg_parm_save = 0;

                    if((gsc_disp_parm_type == DISP_FACTORY_CL2) || //CL2~CL4闪烁
                       (gsc_disp_parm_type == DISP_FACTORY_CL3) ||
                       (gsc_disp_parm_type == DISP_FACTORY_CL4))
                    {
                        gss_factory_parm_value[gsc_disp_parm_type] = gss_key_temp_value;

                        if(gss_factory_parm_value[gsc_disp_parm_type] == 1)
                        {
                            if(gsc_disp_parm_type == DISP_FACTORY_CL2)   
                            {
                                //gss_factory_parm_value[DISP_FACTORY_CL2] = 0;//闪烁显示完清0
                                bflg_sent_usb_CL2 = 1;                  //置发送清usb指令
                                
                                guc_buzz_sound_cnt = 1;                 //蜂鸣1声 
                            }
                            else if(gsc_disp_parm_type == DISP_FACTORY_CL3)
                            {
                                bflg_IC_erasure_order = 1;                            //通知显示板注销所有IC卡的标志
                                
                                gss_user_parm_value[DISP_USER_IC] = EEPROM_init[num_IC_password][0];       //密码初始化      
                                ram_para[num_IC_password] = gss_user_parm_value[DISP_USER_IC];                                                                    //!!!!!!!
                                eeprom_write(num_IC_password, ram_para[num_IC_password]);                                    //密码初始化同时注销所有IC卡 待+  
                                guc_buzz_sound_cnt = 1;                                        
                            }
                            else if(gsc_disp_parm_type == DISP_FACTORY_CL4)
                            {
                                bflg_factory_CL4 = 1;               //恢复出厂设置运行标志
                                guc_buzz_sound_cnt = 1;        
                            }
                        }
                    }
                    else 
                    {
                        if(gss_factory_parm_value[gsc_disp_parm_type] != gss_key_temp_value)
                        {
                            gss_factory_parm_value[gsc_disp_parm_type] = gss_key_temp_value;
                        }
                        
                        if(gsc_disp_parm_type == DISP_FACTORY_UL)
                        {
                            ram_para[num_usb_language] = gss_factory_parm_value[DISP_FACTORY_UL];                                                                    //!!!!!!!
                            eeprom_write(num_usb_language, ram_para[num_usb_language]); 
                        
                            bflg_sent_usb_UL = 1;
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_Ct)
                        {
                            ram_para[num_pt100_calibration] = gss_factory_parm_value[DISP_FACTORY_Ct];                                                                    //!!!!!!!
                            eeprom_write(num_pt100_calibration, ram_para[num_pt100_calibration]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_CU)
                        {
                            ram_para[num_voltage_calibration] = gss_factory_parm_value[DISP_FACTORY_CU];                                                                    //!!!!!!!
                            eeprom_write(num_voltage_calibration, ram_para[num_voltage_calibration]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_CA)
                        {
                            ram_para[num_THW_calibration] = gss_factory_parm_value[DISP_FACTORY_CA];                                                                    //!!!!!!!
                            eeprom_write(num_THW_calibration, ram_para[num_THW_calibration]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_Fd)
                        {
                            ram_para[num_buzz_alarm_delaytime] = gss_factory_parm_value[DISP_FACTORY_Fd];                                                                    //!!!!!!!
                            eeprom_write(num_buzz_alarm_delaytime, ram_para[num_buzz_alarm_delaytime]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_AT)
                        {
                            if(gss_factory_parm_value[DISP_FACTORY_AT] == 1)
                            {
                                gss_factory_parm_value[DISP_FACTORY_AT] = 0;
                                bflg_factory_AT_test = 1;            
                                guc_buzz_sound_cnt = 6;             //蜂鸣6声 ,灯亮6s在特定函数中
                            }
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_SS)
                        {
                            ram_para[num_TE_fault_shield] = gss_factory_parm_value[DISP_FACTORY_SS];                                                                    //!!!!!!!
                            eeprom_write(num_TE_fault_shield, ram_para[num_TE_fault_shield]); 
                        }        
                        else if(gsc_disp_parm_type == DISP_FACTORY_AA)
                        {
                            ram_para[num_THW_hight_alarm] = gss_factory_parm_value[DISP_FACTORY_AA];                                                                    //!!!!!!!
                            eeprom_write(num_THW_hight_alarm, ram_para[num_THW_hight_alarm]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_Cd)
                        {
                            ram_para[num_comp_startup_delaytime] = gss_factory_parm_value[DISP_FACTORY_Cd];                                                                    //!!!!!!!
                            eeprom_write(num_comp_startup_delaytime, ram_para[num_comp_startup_delaytime]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_PS2)
                        {
                            ram_para[num_password_val] = gss_factory_parm_value[DISP_FACTORY_PS2];                                                                    //!!!!!!!
                            eeprom_write(num_password_val, ram_para[num_password_val]); 
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_bS)
                        {
                            if(gss_factory_parm_value[DISP_FACTORY_bS] == 1)
                            {
                                bflg_fac_bS_buz_valid = 1;        //电池未连接报警屏蔽             
                                guc_buzz_sound_cnt = 1;           //响一声  
                            }
                            else 
                            {
                                bflg_fac_bS_buz_valid = 0;                     
                                guc_buzz_sound_cnt = 1;   
                            }
                        }
                        else if(gsc_disp_parm_type == DISP_FACTORY_LL)
                        {                            
                            ram_para[num_secret_key] = gss_factory_parm_value[DISP_FACTORY_LL];                                                                    //!!!!!!!
                            eeprom_write(num_secret_key, ram_para[num_secret_key]); 
                        } 
                        else if(gsc_disp_parm_type == DISP_FACTORY_HC)
                        {
                            ram_para[num_tank_mouth_heat] = gss_factory_parm_value[DISP_FACTORY_HC];                                                                    //!!!!!!!
                            eeprom_write(num_tank_mouth_heat, ram_para[num_tank_mouth_heat]); 
                        }
                    }
                }
            }
            else
            {
                if(guc_key_val == KEY_UP)
                {
                    if(bflg_parm_save == 0)
                    {
                        bflg_parm_save = 1;
                        gss_key_temp_value = gss_factory_parm_value[gsc_disp_parm_type];
                    }
                    
                    //NO else 
                    if(gsc_disp_parm_type == DISP_FACTORY_CL2)
                    {
                        gss_key_temp_value ++;
                        if (gss_key_temp_value > 1)   
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CL3)
                    {
                        gss_key_temp_value ++;
                        if (gss_key_temp_value > 1)   
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CL4)
                    {
                        gss_key_temp_value ++;
                        if (gss_key_temp_value > 1)   
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    

                    
                    else if(gsc_disp_parm_type == DISP_FACTORY_UL)
                    {
                        gss_key_temp_value ++;
                        if (gss_key_temp_value > 1)   
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Ct)
                    {
                        gss_key_temp_value += EEPROM_init[num_pt100_calibration][3];
                        if (gss_key_temp_value > ram_para2[num_pt100_calibration_max])   
                        {
                            gss_key_temp_value = ram_para2[num_pt100_calibration_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CU)
                    {
                        gss_key_temp_value += EEPROM_init[num_voltage_calibration][3];
                        if (gss_key_temp_value > ram_para2[num_voltage_calibration_max])   
                        {
                            gss_key_temp_value = ram_para2[num_voltage_calibration_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CA)
                    {
                        gss_key_temp_value += EEPROM_init[num_THW_calibration][3];
                        if (gss_key_temp_value > ram_para2[num_THW_calibration_max])   
                        {
                            gss_key_temp_value = ram_para2[num_THW_calibration_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Fd)
                    {
                        gss_key_temp_value += gss_key_temp_value;  //30 60 120
                        if (gss_key_temp_value > ram_para2[num_buzz_alarm_delaytime_max])   
                        {
                            gss_key_temp_value = ram_para2[num_buzz_alarm_delaytime_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_AT)
                    {
                        gss_key_temp_value ++;
                        if (gss_key_temp_value > 1)   
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_SS)
                    {
                        gss_key_temp_value += EEPROM_init[num_TE_fault_shield][3];
                        if (gss_key_temp_value > ram_para2[num_TE_fault_shield_max])   
                        {
                            gss_key_temp_value = ram_para2[num_TE_fault_shield_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_AA)
                    {
                        gss_key_temp_value += EEPROM_init[num_THW_hight_alarm][3];
                        if (gss_key_temp_value > ram_para2[num_THW_hight_alarm_max])   
                        {
                            gss_key_temp_value = ram_para2[num_THW_hight_alarm_min];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Cd)
                    {
                        gss_key_temp_value += EEPROM_init[num_comp_startup_delaytime][3];
                        if (gss_key_temp_value > ram_para2[num_comp_startup_delaytime_max])   
                        {
                            gss_key_temp_value = ram_para2[num_comp_startup_delaytime_min];
                        }
                    }
                     else if(gsc_disp_parm_type == DISP_FACTORY_PS2)
                     {
                         if (bflg_key_password_long_valid == 0)   
                         {
                             if(bflg_key_password_long_valid_delaytime == 0)
                             {
                                 bflg_key_password_long_valid_delaytime = 1;
                                 gss_key_password_long_valid_delaytime = 0;
                             }
                             gss_key_temp_value += EEPROM_init[num_password_val][3];
                             if (gss_key_temp_value > ram_para2[num_password_val_max])   
                             {
                                 gss_key_temp_value = ram_para2[num_password_val_min];
                             }                            
                         }                                                
                         else
                         {
                             gss_key_temp_value += 10;   //长按步进+10
                             if (gss_key_temp_value > ram_para2[num_password_val_max])   
                             {
                                 gss_key_temp_value = ram_para2[num_password_val_min];
                             }
                         }
                     }
                    else if(gsc_disp_parm_type == DISP_FACTORY_bS)
                    {
                        gss_key_temp_value++;
                        if(gss_key_temp_value > 1)
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_LL)
                    {
                        gss_key_temp_value++;
                        if(gss_key_temp_value > 1)
                        {
                            gss_key_temp_value = 0;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_HC)
                    {
                        gss_key_temp_value++;
                        if(gss_key_temp_value > 1)
                        {
                            gss_key_temp_value = 0;
                        }
                    }                    
                }
                else if(guc_key_val == KEY_DOWN)
                {
                    if(bflg_parm_save == 0)
                    {
                        bflg_parm_save = 1;
                        gss_key_temp_value = gss_factory_parm_value[gsc_disp_parm_type];
                    }
                    //else NO
                    
                    if(gsc_disp_parm_type == DISP_FACTORY_CL2)
                    {
                        gss_key_temp_value --;
                        if (gss_key_temp_value < 0)   
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CL3)
                    {
                        gss_key_temp_value --;
                        if (gss_key_temp_value < 0)   
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CL4)
                    {
                        gss_key_temp_value --;
                        if (gss_key_temp_value < 0)   
                        {
                            gss_key_temp_value = 1;
                        }
                    }

                    
                    else if(gsc_disp_parm_type == DISP_FACTORY_UL)
                    {
                        gss_key_temp_value --;
                        if (gss_key_temp_value < 0)   
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Ct)
                    {
                        gss_key_temp_value -= EEPROM_init[num_pt100_calibration][3];
                        if (gss_key_temp_value < ram_para2[num_pt100_calibration_min])   
                        {
                            gss_key_temp_value = ram_para2[num_pt100_calibration_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CU)
                    {
                        gss_key_temp_value -= EEPROM_init[num_voltage_calibration][3];
                        if (gss_key_temp_value < ram_para2[num_voltage_calibration_min])   
                        {
                            gss_key_temp_value = ram_para2[num_voltage_calibration_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_CA)
                    {
                        gss_key_temp_value -= EEPROM_init[num_THW_calibration][3];
                        if (gss_key_temp_value < ram_para2[num_THW_calibration_min])   
                        {
                            gss_key_temp_value = ram_para2[num_THW_calibration_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Fd)
                    {
                        gss_key_temp_value -= (gss_key_temp_value/2);// 30 60 120
                        if (gss_key_temp_value < ram_para2[num_buzz_alarm_delaytime_min])   
                        {
                            gss_key_temp_value = ram_para2[num_buzz_alarm_delaytime_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_AT)
                    {
                        gss_key_temp_value --;
                        if (gss_key_temp_value < 0)   
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_SS)
                    {
                        gss_key_temp_value -= EEPROM_init[num_TE_fault_shield][3];
                        if (gss_key_temp_value < ram_para2[num_TE_fault_shield_min])   
                        {
                            gss_key_temp_value = ram_para2[num_TE_fault_shield_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_AA)
                    {
                        gss_key_temp_value -= EEPROM_init[num_THW_hight_alarm][3];
                        if (gss_key_temp_value < ram_para2[num_THW_hight_alarm_min])   
                        {
                            gss_key_temp_value = ram_para2[num_THW_hight_alarm_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_Cd)
                    {
                        gss_key_temp_value -= EEPROM_init[num_comp_startup_delaytime][3];
                        if (gss_key_temp_value < ram_para2[num_comp_startup_delaytime_min])   
                        {
                            gss_key_temp_value = ram_para2[num_comp_startup_delaytime_max];
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_PS2)
                    {
                         if (bflg_key_password_long_valid == 0)   
                         {
                             if(bflg_key_password_long_valid_delaytime == 0)
                             {
                                 bflg_key_password_long_valid_delaytime = 1;
                                 gss_key_password_long_valid_delaytime = 0;
                             }
                            gss_key_temp_value -= EEPROM_init[num_password_val][3];
                            if (gss_key_temp_value < ram_para2[num_password_val_min])   
                            {
                                gss_key_temp_value = ram_para2[num_password_val_max];
                            }                           
                         }                                                
                         else
                         {
                             gss_key_temp_value -= 10;   //长按步进-10
                             if (gss_key_temp_value < ram_para2[num_password_val_min])   
                             {
                                 gss_key_temp_value = ram_para2[num_password_val_max];
                             }
                         }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_bS)
                    {
                        gss_key_temp_value--;
                        if(gss_key_temp_value < 0)
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_LL)
                    {
                        gss_key_temp_value--;
                        if(gss_key_temp_value < 0)
                        {
                            gss_key_temp_value = 1;
                        }
                    }
                    else if(gsc_disp_parm_type == DISP_FACTORY_HC)
                    {
                        gss_key_temp_value--;
                        if(gss_key_temp_value < 0)
                        {
                            gss_key_temp_value = 1;
                        }
                    }                    
                }
                else
                {
                    key_clear_password_long_falg();
                }
            }
        }
    }
}

/********************************************************************************************************************
函数功能: IC卡密码模式
          
函数位置:　  read_key_deal();     -----------------------OK    
********************************************************************************************************************/
void key_IC_password_deal(void)  //按键密码处理程序
{
    if (guc_key_val == KEY_BUZ)
    {
        if(gsc_disp_parm_type == DISP_USER_CL1)     //从user的CL1过来的
        {           
            guc_disp_mode = DISP_USER;   
            gss_key_temp_value = 0;
            bflg_parm_dis_form = 0;
            bflg_parm_save = 0;
        }
        else
        {
            guc_disp_mode = DISP_UNLOCKED;   
            
            bflg_IC_register_state = 0;              //后加 待验证
        }
    }
    else if (guc_key_val == KEY_UP)
    {  
        if (bflg_key_password_long_valid == 0)
        {
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            gsc_IC_password_val++;
            if (gsc_IC_password_val > 999)
            {
                gsc_IC_password_val = 0;
            }             
        }
        else
        {
            gsc_IC_password_val += 10;
            if (gsc_IC_password_val > 999)
            {
                gsc_IC_password_val = 0;
            } 
        }    
    }
    else if (guc_key_val == KEY_DOWN)
    {       
        if (bflg_key_password_long_valid == 0)
        {
            if(bflg_key_password_long_valid_delaytime == 0)
            {
                bflg_key_password_long_valid_delaytime = 1;
                gss_key_password_long_valid_delaytime = 0;
            }
            gsc_IC_password_val--;
            if (gsc_IC_password_val < 0)
            {
                gsc_IC_password_val = 999;
            }             
        }
        else
        {
            gsc_IC_password_val -= 10;
            if (gsc_IC_password_val < 0)
            {
                gsc_IC_password_val = 999;
            } 
        }     
    }    
    else if (guc_key_val == KEY_SET)
    {       
        key_clear_password_long_falg();   
        
        guc_IC_password_cnt++;

        if(guc_IC_password_cnt <= 3)
        {
            if (gsc_IC_password_val == gss_user_parm_value[DISP_USER_IC])      // 输入的密码是否和设置的一样
            {        
                guc_IC_password_cnt = 0;
                
                if(gsc_disp_parm_type == DISP_USER_CL1) //如果是user2层的 IC注销参数
                {
                    bflg_IC_erasure_order = 1;            //注销所有IC卡
                    
                    bflg_IC_pass_CL1 = 1;                 //CL1闪烁显示的标志

                    bflg_sent_usb_IC = 1;                 //13
                }
                else
                {              
                    guc_disp_mode  = DISP_IC_REG;
                }
            }
            else if(guc_IC_password_cnt >= 3)
            {
                bflg_IC_register_state = 0;           //清可进入IC密码状态
                gsc_IC_password_val = 0;
                guc_IC_password_cnt = 0;
                guc_disp_mode = DISP_LOCKED;
                gsc_disp_user1_mode = DISP_SET_TS;    
                gsc_disp_user2_mode = DISP_USER_dA; 
            }
        }
    }
    else
    {
        key_clear_password_long_falg();         //输入密码时长按步进10
    }
}

/**************************************************************************************************************************
函数描述:  IC注册时按键操作，只有buzz键有作用

函数位置:  key_mode_select_deal----------------------------------OK
**************************************************************************************************************************/
void key_IC_register_deal(void)
{
    if(guc_key_val == KEY_BUZ)                // 按下SET键，退出并锁定 
    {
       bflg_IC_register_state = 0;            // 退出 IC注册状态
       guc_temp_flash_add = 0;
       guc_flash_add = 0;
       
       guc_disp_mode = DISP_UNLOCKED;         //回到上层的显示状态
    }
}

/*****************************************************************************
函数功能: 在进入debug层时进行的一些操作        
　　　　　1、 选择哪一个参数para或是see
             
函数位置:   read_key_deal()中---------------------------ok
******************************************************************************/
void key_debug_select_deal()
{
    if (guc_key_val == KEY_BUZ)  
    {
        guc_disp_mode = DISP_LOCKED;               //回到上层锁定
        
        gsc_disp_debug_mode = DISP_DEBUG_PARA;     //再次进入此层时显示默认para
    }
    else if (guc_key_val == KEY_SET)
    {  
        guc_disp_mode = DISP_DEBUG;               //进入debug模式
        gsc_disp1_para_mode = 0;
        gsc_disp1_see_mode = 0;
    }
    else if (guc_key_val == KEY_UP)               //按up或down 按键选择para和see模式
    {
        gsc_disp_debug_mode++;
        if (gsc_disp_debug_mode > DISP_DEBUG_SEE)
        {
            gsc_disp_debug_mode = DISP_DEBUG_PARA;
        }
    }
    else if (guc_key_val == KEY_DOWN)    
    {
        gsc_disp_debug_mode--;
        if (gsc_disp_debug_mode < DISP_DEBUG_PARA) 
        {
            gsc_disp_debug_mode = DISP_DEBUG_SEE;
        }
    }
}

/*****************************************************************************
函数功能: 在进入debug层时进行的一些操作        
　　　　　1、海尔调试使用的几个参数，可修改--para
          2、海尔调试的参数，只可看--see
          
函数位置:   read_key_deal()中---------------------------ok
******************************************************************************/
void key_debug_parm_deal(void)
{   
    if(gsc_disp_debug_mode == DISP_DEBUG_PARA)     //调试参数
    {
        if (guc_key_val == KEY_BUZ)
        {
            guc_disp_mode = DISP_DEBUG_SELECT;           //回到上层
            gsc_disp1_para_mode = COMP2_START_FREQ;      //再次回到此层时显示p00;
            bflg_disp1_val = 0;
        }
        else
        {
            if (bflg_disp1_val == 0)          //如果不显示设定值
            {
                if (guc_key_val == KEY_SET)    
                {
                    bflg_disp1_val = 1;       //显示设定值
                }
                else
                {
                    if (guc_key_val == KEY_UP)
                    {
                        gsc_disp1_para_mode++; 
                        if (gsc_disp1_para_mode > COMP_RUN_DELAYTIME)  
                        {
                            gsc_disp1_para_mode = COMP2_START_FREQ;
                        }
                    }
                    else if (guc_key_val == KEY_DOWN)
                    {
                        gsc_disp1_para_mode--;
                        if (gsc_disp1_para_mode < 0)
                        {
                            gsc_disp1_para_mode = COMP_RUN_DELAYTIME;
                        }
                    }
                }
            }
            else                                 //如果是显示设定值
            {
                if (guc_key_val == KEY_SET)       
                {
                    bflg_disp1_val = 0;          //不显示设定值
                }
                else
                {
                    if (guc_key_val == KEY_UP)
                    { 
                        if ((gsc_disp1_para_mode == COMP2_START_FREQ) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_K1) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_MIN) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_MAX) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_MIN) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_MAX) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_K2) 
                        || (gsc_disp1_para_mode == COMP21_START_FREQ) 
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_K)
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_MIN)
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_MAX)
                        || (gsc_disp1_para_mode == COMP21_FREQ_MIN)
                        || (gsc_disp1_para_mode == COMP21_FREQ_MAX)
                        || (gsc_disp1_para_mode == COMP_STOP_DELAYTIME)
                        || (gsc_disp1_para_mode == COMP_DOOR_FREQ)
                        || (gsc_disp1_para_mode == COMP_RUN_DELAYTIME))
                        {
                            gss_para[gsc_disp1_para_mode] += 10;
                        }
                        else
                        {
                            gss_para[gsc_disp1_para_mode]++;
                            
                            if(gsc_disp1_para_mode == COMP_START_DELT_TEMP)
                            {   
                                eeprom_write(num_debug_P20, gss_para[gsc_disp1_para_mode]);  
                            }
                            else if(gsc_disp1_para_mode == COMP_STOP_DELT_TEMP)
                            {   
                                eeprom_write(num_debug_P21, gss_para[gsc_disp1_para_mode]);  
                            }
                        }
                    }
                    else if (guc_key_val == KEY_DOWN)
                    {  
                        if ((gsc_disp1_para_mode == COMP2_START_FREQ) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_K1) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_MIN) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_MAX) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_MIN) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_MAX) 
                        || (gsc_disp1_para_mode == COMP2_FREQ_DELT_K2) 
                        || (gsc_disp1_para_mode == COMP21_START_FREQ) 
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_K)
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_MIN)
                        || (gsc_disp1_para_mode == COMP21_FREQ_DELT_MAX)
                        || (gsc_disp1_para_mode == COMP21_FREQ_MIN)
                        || (gsc_disp1_para_mode == COMP21_FREQ_MAX)
                        || (gsc_disp1_para_mode == COMP_STOP_DELAYTIME)
                        || (gsc_disp1_para_mode == COMP_DOOR_FREQ)
                        || (gsc_disp1_para_mode == COMP_RUN_DELAYTIME))
                        {
                            gss_para[gsc_disp1_para_mode] -= 10;
                        }
                        else
                        {
                            gss_para[gsc_disp1_para_mode]--;

                            if(gsc_disp1_para_mode == COMP_START_DELT_TEMP)
                            {   
                                eeprom_write(num_debug_P20, gss_para[gsc_disp1_para_mode]);  
                            }
                            else if(gsc_disp1_para_mode == COMP_STOP_DELT_TEMP)
                            {   
                                eeprom_write(num_debug_P21, gss_para[gsc_disp1_para_mode]);  
                            }
                        }
                    }
                }
            }
        }
    }
    else if(gsc_disp_debug_mode == DISP_DEBUG_SEE)    //查看参数
    {
        if (guc_key_val == KEY_BUZ)
        {
            guc_disp_mode = DISP_DEBUG_SELECT;       //回到上一层
            gsc_disp1_see_mode = DISP1_SEE_r00;      //再次回到此层时显示r00;
            bflg_disp1_val = 0;
        }
        else
        {
            if (bflg_disp1_val == 0)           //如果不显示设定值
            {
                if (guc_key_val == KEY_SET)    //如果是设置键
                {
                    bflg_disp1_val = 1;        //显示设定值
                }
                else
                {
                    if (guc_key_val == KEY_UP)
                    {
                        gsc_disp1_see_mode++;
                        
                        if (gsc_disp1_see_mode >= DISP1_SEE_r05)
                        {
                            gsc_disp1_see_mode = DISP1_SEE_r00;
                        }
                    }
                    else if (guc_key_val == KEY_DOWN)
                    {
                        gsc_disp1_see_mode--;
                        
                        if (gsc_disp1_see_mode <= DISP1_SEE_r00)
                        {
                            gsc_disp1_see_mode = DISP1_SEE_r05;
                        }
                    }
                }
            }
            else   //如果是显示查询值
            {
                if (guc_key_val == KEY_SET) //如果是设置键
                {
                    bflg_disp1_val = 0;     //不显示设定值
                }
            }
        }
    }
}

/******************************************************************************************************************************
功能描述:  在用户参数中设置RTC时钟时间的操作:P1-P5

函数位置: read_key_deal() 即主循环------------------------ok
******************************************************************************************************************************/
void key_RTC_time_parm_deal(void)  
{
    if (guc_key_val == KEY_BUZ)    //从USER中的T2参数进来的
    {
        guc_disp_mode = DISP_USER;        
        gsc_disp_parm_type = DISP_USER_T2;
        gss_key_temp_value = 0;
        bflg_parm_dis_form = 0;
        bflg_parm_save = 0;
        //gss_user_parm_value[DISP_USER_T2]= 0;//若要从P1开始
    }
    else if (guc_key_val == KEY_UP)             
    {
        guc_user_write_time_value++;
        
        if(gss_user_parm_value[DISP_USER_T2] == 0)           //P1_年
        {
            if(guc_user_write_time_value > 99)
            {
                guc_user_write_time_value = 10;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P2_月
        {
            if(guc_user_write_time_value > 12)
            {
                guc_user_write_time_value = 1;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P3_日
        {
            if(guc_user_write_time_value > 31)
            {
                guc_user_write_time_value = 1;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P4_时
        {
            if(guc_user_write_time_value > 23)
            {
                guc_user_write_time_value = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P5_分
        {
            if(guc_user_write_time_value > 59)
            {
                guc_user_write_time_value = 0;
            }
        }
    }
    else if(guc_key_val == KEY_DOWN)
    {
        guc_user_write_time_value--;
        if(gss_user_parm_value[DISP_USER_T2] == 0)           //P1_年
        {
            if(guc_user_write_time_value < 10)
            {
                guc_user_write_time_value = 99;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P2_月
        {
            if(guc_user_write_time_value < 1)
            {
                guc_user_write_time_value = 12;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P3_日
        {
            if(guc_user_write_time_value < 1)
            {
                guc_user_write_time_value = 31;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P4_时
        {
            if(guc_user_write_time_value < 0)
            {
                guc_user_write_time_value = 23;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P5_分
        {
            if(guc_user_write_time_value < 0)
            {
                guc_user_write_time_value = 59;
            }
        }
    }
    else if(guc_key_val == KEY_SET)  //确认修改时间
    {
        guc_disp_mode = DISP_USER;
        gsc_disp_parm_type = DISP_USER_T2;
        //gss_key_temp_value = 0;
        bflg_parm_dis_form = 1;
        bflg_parm_save = 0;
            
        if(gss_user_parm_value[DISP_USER_T2] == 0)      //P1年
        {
            if(guc_user_write_time_value != guc_RTC_r_year)    
            {
                guc_RTC_ADDR   =  CMD_W_Y;
                guc_RTC_WDATA = func_timer_4302_deal(guc_user_write_time_value);  
                bflg_RTC_enable_write = 1;
                
                bflg_usb_set_time_delaytime = 1;
                gss_usb_set_time_delaytime = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P1月
        {
            if(guc_user_write_time_value != guc_RTC_r_month)    
            {
                guc_RTC_ADDR   =  CMD_W_MO;
                guc_RTC_WDATA = func_timer_4302_deal(guc_user_write_time_value);  
                bflg_RTC_enable_write = 1;
                
                bflg_usb_set_time_delaytime = 1;
                gss_usb_set_time_delaytime = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P1日
        {
            if(guc_user_write_time_value != guc_RTC_r_day)    
            {
                guc_RTC_ADDR   =  CMD_W_D;
                guc_RTC_WDATA = func_timer_4302_deal(guc_user_write_time_value);  
                bflg_RTC_enable_write = 1;
                
                bflg_usb_set_time_delaytime = 1;
                gss_usb_set_time_delaytime = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P1时
        {
            if(guc_user_write_time_value != guc_RTC_r_hour)    
            {
                guc_RTC_ADDR   =  CMD_W_H;
                guc_RTC_WDATA = func_timer_4302_deal(guc_user_write_time_value);  
                bflg_RTC_enable_write = 1;
                
                bflg_usb_set_time_delaytime = 1;
                gss_usb_set_time_delaytime = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P1分
        {
            if(guc_user_write_time_value != guc_RTC_r_minute) 
            {
                guc_RTC_ADDR   =  CMD_W_M;
                guc_RTC_WDATA = func_timer_4302_deal(guc_user_write_time_value);  
                bflg_RTC_enable_write = 1;
                
                bflg_usb_set_time_delaytime = 1;
                gss_usb_set_time_delaytime = 0;
            }
        }
    }
}

/******************************************************************************************************************************
功能描述:  修改RTC时钟后，需延时2s再发送修改usb时间，防止主板还未读取到修改后的时间

函数位置:  1s定时器---------------------------------------------------ok
******************************************************************************************************************************/
void key_RTC_time_parm_sent_usb_delaytime(void) 
{
    if(bflg_usb_set_time_delaytime == 1)
    {   
        gss_usb_set_time_delaytime++;
        if(gss_usb_set_time_delaytime >= 2)
        {
            gss_usb_set_time_delaytime = 0;
            bflg_usb_set_time_delaytime = 0;
            
            bflg_usb_set_time = 1;
        }
    }
}



/*******************************************************************************************************************************
函数功能 :   按键20s没有动作进入锁定状态

函数位置：100ms---------------------------------ok           
*******************************************************************************************************************************/
void key_locked_delaytime(void)    
{
    if (guc_key_val == KEY_NONE)
    {
        gss_key_locked_delaytimer++;
        if (gss_key_locked_delaytimer >= ram_para2[num_key_locked_delaytime])    //200*100ms=20s
        {
            gss_key_locked_delaytimer = 0;
            guc_disp_mode = DISP_LOCKED;       
            
            bflg_key_locked = 1;        //置按键锁定标志  peak 未用
            bflg_IC_register_state = 0; //退出IC注册状态
            guc_temp_flash_add = 0;
            guc_flash_add = 0;
        }
    }
    else
    {
        gss_key_locked_delaytimer = 0;
    }

    //------------------------------------------------------------------------------
    if(bflg_IC_register_state == 1)      //在注册状态时，有卡注册则锁屏时间清0；
    {        
        if(guc_temp_flash_add != guc_flash_add)
        {
            guc_temp_flash_add = guc_flash_add;
            gss_key_locked_delaytimer = 0;
        }   
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
function: 锁定状态长按 "设定" 键 5s ,表示长按键有效，可进入密码模式

函数位置: 100ms定时器----------------ok
*******************************************************************************/
void key_long_delaytime(void) //按键解锁延时程序，在100ms定时程序中调用
{
    if (bflg_key_long_delaytime == 1)
    {
        gss_key_long_delaytimer++;
        if (gss_key_long_delaytimer >= ram_para2[num_key_long_delaytime])  //50
        {
            gss_key_long_delaytimer = 0;
            bflg_key_long_delaytime = 0;
            
            bflg_key_long_valid = 1;    //置长按键有效标志
        }
    }
}

/*******************************************************************************
function: 锁定状态长按 down 键 5s ,表示长按键有效，可进入海尔调试时使用的参数
          模式，在正式版本时将此去掉

函数位置:  100ms 定时器 ------------ok       
*******************************************************************************/
void key_long_down_delaytime(void)
{
    if (bflg_key_long_down_delaytime == 1)
    {
        gss_key_long_down_delaytimer++;
        if (gss_key_long_down_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_down_delaytimer = 0;
            bflg_key_long_down_delaytime = 0;

            bflg_key_long_down_valid = 1;    //置长按down键有效标志
        }
    }
}
//锁定模式长按set
void key_long_set_delaytime(void)
{
    if (bflg_key_long_set_delaytime == 1)
    {
        gss_key_long_set_delaytimer++;
        if (gss_key_long_set_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_set_delaytimer = 0;
            bflg_key_long_set_delaytime = 0;

            bflg_key_long_set_valid = 1;    //置长按set键有效标志
        }
    }
}
//长按up+down 组合键计时   
void key_long_updown_delaytime(void)
{
    if (bflg_key_long_updown_delaytime == 1)
    {
        gss_key_long_updown_delaytimer++;
        if (gss_key_long_updown_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_updown_delaytimer = 0;
            bflg_key_long_updown_delaytime = 0;

            bflg_key_long_updown_valid = 1;    //置长按down键有效标志
        }
    }
}

//长按set+down组合键计时 --现用作在user1长按down 5s
void key_long_setdown_delaytime(void)
{
    if (bflg_key_long_setdown_delaytime == 1)
    {
        gss_key_long_setdown_delaytimer++;
        if (gss_key_long_setdown_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_setdown_delaytimer = 0;
            bflg_key_long_setdown_delaytime = 0;

            bflg_key_long_setdown_valid = 1;    //置长按down键有效标志
        }
    }
}



//------------------------------------------------------------------------------
/**************************************************************************************************************************************
函数功能: 当连续按下按键时，每间隔多长时间对按键读取

函数位置：10ms----------------------------------ok
***************************************************************************************************************************************/
void read_key_delaytime(void) //读键延时程序，在10ms定时程序中调用
{    
    if (bflg_read_key_delaytime == 1)
    {
        gss_read_key_delaytimer++;
        if (gss_read_key_delaytimer >= 100)  // 10 * 200 = 2000ms = 2s  原来 
        {                                    // 10 *100 = 1000ms = 1s   现在1s在增加
            gss_read_key_delaytimer = 0;
            bflg_read_key_delaytime = 0;
        }
    }
}





//------------------------------------------------------------------------------
/*    |   The following code is added on my own
	  |
	  |
	  |
	  |    
	 \|/                                                                     */


/*********************************************************************************************************
函数功能: 有报警时按下 buzz 键可30min内不再报警


函数位置: 主循环 ------------------------------------------ok
*********************************************************************************************************/
void  key_buzz_cancel(void)
{
    if (guc_key_val == KEY_BUZ)  
    {
        if (bflg_buzz_loop_tick_set == 1)         // 如果此时有蜂鸣器报警，但是按下了"蜂鸣取消"按键，则会延时30 min 不会响，
        {                                         // 30 min 后恢复正常控制；
            if (bflg_alarm_buzz_off_delaytime == 0)//若按下则要将这30min进行完后再计时
            {
                bflg_alarm_buzz_off_delaytime = 1;
                gss_alarm_buzz_off_delaytimer = 0;
            }       
        }
    }
}

/*******************************************************************************************************************************           
函数功能：     电池未连接，长按5s后，在下一次检测到电池这段时间内不响，
          当再次检测到电池，恢复正常控制；
          
函数位置: 主循环--------------------------------------ok          
*******************************************************************************************************************************/
void bat_con_check_again(void)    
{
	if (bflg_fac_bS_buz_valid == 1)        //设置电池未连接屏蔽参数bS=1
	{
		if (bflg_alarm_bat_discnnt == 0)   //如果又检测到电池
		{    
			 bflg_fac_bS_buz_valid = 0;    //恢复正常控制
			 gss_factory_parm_value[DISP_FACTORY_bS] = 0;
		}
	}
}


/**********************************************************************************************************************************************
功能描述: 指示灯闪烁时间标志的设定,可一直在这里循环

          校正: >=1  ----->  >1

函数位置：100ms------------------------------------ok
**********************************************************************************************************************************************/
void led_loop_delaytime(void)    
{
    gss_led_loop_delaytime++;
	if (gss_led_loop_delaytime > 1)      // 100ms * 1 = 100ms 
	{
		gss_led_loop_delaytime = 0;
	    bflg_led_loop_set = ~bflg_led_loop_set;
	}
}

/**********************************************************************************************************************************************
功能描述: 设置参数时参数的闪烁频率

函数位置:    100ms 定时器中--------------------------------ok
**********************************************************************************************************************************************/
void led_disp1_flicker_delaytime(void)
{
    gss_disp1_flicker_delaytime++;
    if(gss_disp1_flicker_delaytime > 5)
    {       
        gss_disp1_flicker_delaytime = 0;
        bflg_disp1_flicker = ~bflg_disp1_flicker;      
    }
}




/*********************************************************************************************************************************
函数描述:  工厂模式的AT指令的测试
           
函数位置:  10ms定时器---------------------------------------ok
*********************************************************************************************************************************/
void key_factory_AT_test(void)
{
    LED1 = 0xff;
    LED2 = 0xff;
    LED3 = 0xff;
    LED4 = 0xff;
    LED5 = 0xff;
    LED6 = 0xff;
    LED7 = 0xff;
    LED8 = 0xff;
    LED9 = 0xff;
    LED10 = 0xff;
    LED11 = 0xff;
    LED12 = 0xff;    
    
    RL_ALM_OUT_PIN = 1;                       //远程报警灯开
    bflg_alarm_lamp = 1;                      //3位报警灯
    
    gss_fac_AT_test_delaytime++;
    if(gss_fac_AT_test_delaytime >= 600)  //10ms * 600 = 6s 后恢复正常
    {
        gss_fac_AT_test_delaytime = 0;
        bflg_factory_AT_test = 0;

        LED1 = 0x00;
        LED2 = 0x00;
        LED3 = 0x00;
        LED4 = 0x00;
        LED5 = 0x00;
        LED6 = 0x00;
        LED7 = 0x00;
        LED8 = 0x00;
        LED9 = 0x00;
        LED10 = 0x00;
        LED11 = 0x00;
        LED12 = 0x00;  
    }
}

/******************************************************************************************************************************
函数描述:工厂参数  CL4恢复出厂值功能
           
函数位置: 主循环---------------------------------------ok
******************************************************************************************************************************/
void key_factory_CL4_init(void)
{
    if(bflg_factory_CL4 == 1)
    {
        bflg_factory_CL4 = 0;
        
        init_ram_para2();             //烧写E2
        init_ram_para();
        
        eeprom2_read_deal();          //eeprom2读取处理程序
        eeprom_read_deal();           //eeprom读取处理程序
       
        operation_parm_read();               //从读取完E2的数组中获取需要的值   
        init_freq_ctrl();
    }
}

/******************************************************************************************************************************
函数描述:  初次上电自检，所有灯亮灭3s,响1声
           
函数位置:  10ms定时器----------------------------------ok
******************************************************************************************************************************/
void first_power_on_self_test(void) //first_power_on
{
    if(bflg_self_test_sound == 0)
    {
        bflg_self_test_sound = 1;
        guc_buzz_sound_cnt = 1;     //响1声
    }

    gss_self_test_delaytime++;
    if(gss_self_test_delaytime >= 100)    //10ms*100 = 1s
    {
        gss_self_test_delaytime = 0;
        bflg_self_test_flicker = ~bflg_self_test_flicker;

        gss_self_test_cnt++;
        if(gss_self_test_cnt > 4 )
        {
            gss_self_test_cnt = 0;
            bflg_self_test = 0;
        }
    }


    if(bflg_self_test_flicker == 0)
    {   //全亮
        LED1 = 0xff;
        LED2 = 0xff;
        LED3 = 0xff;
        LED4 = 0xff;
        LED5 = 0xff;
        LED6 = 0xff;
        LED7 = 0xff;
        LED8 = 0xff;
        LED9 = 0xff;
        LED10 = 0xff;
        LED11 = 0xff;
        LED12 = 0xff; 
        
        bflg_alarm_lamp = 1;
    }
    else
    {   //全灭
        LED1 = 0x00;
        LED2 = 0x00;
        LED3 = 0x00;
        LED4 = 0x00;
        LED5 = 0x00;
        LED6 = 0x00;
        LED7 = 0x00;
        LED8 = 0x00;
        LED9 = 0x00;
        LED10 = 0x00;
        LED11 = 0x00;
        LED12 = 0x00; 

        bflg_alarm_lamp = 0;
    }
}

/********************************************************************************************************************
函数描述: 在IC卡注册状态时不进行开锁的动作

函数位置: 主函数中   ---------------------------ok
*********************************************************************************************************************/
void IC_unlock_deal(void)
{
    if(guc_disp_mode != DISP_IC_REG)   //在IC卡注册状态时不进行开锁的动作
    {
        if(guc_IC_access_state == 1)
        {
            LOCK_OUT_PIN = 1;          //锁打开
            guc_IC_access_state = 0;

            bflg_lock_delaytimer = 1;  //开锁的延时标志
            guc_lock_delaytimer = 0;
        }
    }
}

/********************************************************************************************************************
函数描述: IC卡开锁延时

函数位置: 1s 定时 -----ok
*********************************************************************************************************************/
void IC_unlock_delaytime(void)  
{
    if(bflg_lock_delaytimer == 1)
    {
        guc_lock_delaytimer++;
        if(guc_lock_delaytimer >= 10)    // 锁开10s
        {
            bflg_lock_delaytimer = 0;
            guc_lock_delaytimer = 0;

            LOCK_OUT_PIN = 0;
        }
    }
}

/********************************************************************************************************************
函数描述: IC注册(IC)、解锁密码(PS1)、工厂密码(PS2) 长按计时,然后按步进10增加

          每进入那层时需要清0；

函数位置: 1s 定时 -----------------------------------ok
*********************************************************************************************************************/
void key_password_long_valid_delaytime(void)
{
    if (bflg_key_password_long_valid_delaytime == 1)
    {
        gss_key_password_long_valid_delaytime++;
        if (gss_key_password_long_valid_delaytime >= 5)  
        {
            gss_key_password_long_valid_delaytime = 0;
            bflg_key_password_long_valid_delaytime = 0;
            
            bflg_key_password_long_valid = 1;   
        }
    }
}
/********************************************************************************************************************
函数描述: 清密码长按时的标志:1.输入密码时 2.修改密码时 都是相同的作用


函数位置：此文件中的各个函数中------------------------------------------ok
*********************************************************************************************************************/
void key_clear_password_long_falg(void)
{
    bflg_key_password_long_valid = 0;
    bflg_key_password_long_valid_delaytime = 0;
    gss_key_password_long_valid_delaytime = 0;  
}






//----------------------------------------------------------------------------------
/* 
        /|\
         |
         |
         |
         |  The above is my own code
************************************************************************************/



#endif
