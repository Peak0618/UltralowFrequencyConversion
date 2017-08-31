/********************************************************************************************************************************
overview:  -- 超低温变频主板程序

        1、系统初始化    
        2、主循环、定时器循环
        3、E2的读写、初始化
        4、密钥未解锁时的循环
*********************************************************************************************************************************/

#ifndef _MAIN_C_
#define _MAIN_C_

//------------------------------------------------------------------------------
//包含文件声明
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

#include "h_type_define.h"
#include "h_main.h"
#include "h_com.h"
#include "h_adg.h"
#include "h_ad.h"
#include "h_key_operation.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_protect.h"
#include "h_volt_ctrl.h"
#include "h_bat_ctrl.h"

//peak add
#include "M_buzz.h"
#include "M_RTC.h"
#include "m_test.h"
#include "m_peripheral.h"
#include "m_networking.h"
#include "h_key_disp.h"
//------------------------------------------------------------------------------
//函数声明
void system_init(void);  
void switch_init(void);   
void control_init(void);  
void system_start(void);  
void main_loop(void);  
void timer_op(void);   
void ad_convert_deal(void);    
void timer_int(void);  
void init_ram_para2(void);
void init_ram_para(void);
void eeprom2_read_deal(void); 
void eeprom_read_deal(void);   
void delay_10us(uint16_t _10us);    
void delay_1ms(uint16_t _1ms);   
void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data);       
int16_t eeprom_read(uint16_t lus_addr);  
void operation_parm_read(void);





//flag
flag_type flg_time;

//variate
uint8_t   guc_10ms_timer;     //10毫秒计时器
uint8_t   guc_100ms_timer;    //100ms定时器
uint8_t   guc_1s_timer;       //1s定时器
uint8_t   guc_1min_timer;     //1min定时器

//array
const int16_t gss_buzzer_register[10] = 
{
  900,    //1
  800,    //2
  900,    //3
  1000,   //4
  1100,   //5
  1200,   //6
  1300,   //7
  1400,   //8
  1500,   //9
  1600    //10
};



/*****************************************************************************************************************************************
函数功能：系统初初始化

函数位置：系统初始化(1)-----------------------------------ok
*******************************************************************************************************************************************/
void system_init(void)    
{
    int16_t    gss_E2_writed_flag;
        
    switch_init();            //拨码开关初始化

    gss_E2_writed_flag = eeprom_read(255);
    if(gss_E2_writed_flag != 0xaa)
    {      
        //eeprom_write(255,0xaa);    
        init_ram_para2();       
        init_ram_para();
        
        //delay_1ms(3);
        eeprom_write(255,0xaa);
    }
    
    
    //----------------------------------
    //init_ram_para2();       //若包工修改e2参数打开，正式关闭
    //init_ram_para();

    //读取E2    
    eeprom2_read_deal(); //eeprom2读取处理程序
    eeprom_read_deal();  //eeprom读取处理程序

    operation_parm_read();     //从读取完E2的数组中获取需要的值
    init_freq_ctrl();          //变频控制的参数，1、有固定值2、E2中读取的值
    //--------------------------------------------------
    R_ADC_Set_OperationOn();   //ad转换启动
    //--------------------------------------------------
    com_init();               //通讯初始化程序
    //--------------------------------------------------
    adg_init();               //热电偶ad模块初始化程序
    //--------------------------------------------------
    control_init();           //控制初始化程序
    //--------------------------------------------------
    RTC_init();               //RTC时钟初始化
}

/*****************************************************************************************************************************************
函数功能：拨码开关初始化

函数位置：系统初始化(1)--------------------------------------------ok
*******************************************************************************************************************************************/
void switch_init(void)        
{
    if (SW21_IN_PIN == 1)          //电路默认为高电平
    {
        bflg_comp2_volt_type = 0;  //220V类型
    }
    else
    {
        bflg_comp2_volt_type = 1;  //115V类型
    }
}

/*****************************************************************************************************************************************
函数功能：IO口初始化、各个标志初始化

函数位置：系统初始化(1)------------------------ok
*******************************************************************************************************************************************/
void control_init(void)  
{
    bflg_self_test = 1;       //初始上电时系统自检标志

    //12V初始化关闭
    LOCK_OUT_PIN = 0;   //锁关
    HTRMT_OUT_PIN = 0;  //门体加热关
    HTRMXG_OUT_PIN = 0; //预留关
    HTRPHK_OUT_PIN = 0; //平衡口关
    //------------------------------------------------------
    TRIP_LAMP_PIN = 0;   //灯亮(之前的毛细管指示灯，现不用也亮)
    //------------------------------------------------------
    guc_100ms_timer = 100; //定时器
    guc_1s_timer = 100;
    guc_1min_timer = 60;
    //------------------------------------------------------
    guc_ad_calc_cnt = 0;
    bflg_ad_calc_ok = 0;
    //------------------------------------------------------
    BAT_DISCHARGE_PIN = 0;     //控制电池输出——关
    
    bflg_bat_frist_charge = 1; //首次上电进行电池充电标志
    //------------------------------------------------------
    bflg_adg_reset = 1;        //ad模块复位
    //------------------------------------------------------
    bflg_comp2_first_run = 1;  //高压机首次运行标志
    
    bflg_comp2_power_on_delaytime = 1; //初始上电延时
    gss_comp2_power_on_delaytimer = 0;
    
    bflg_prot2_TA_first_delaytime = 1; //主传感器高温报警延时
    gss_prot2_TA_first_delaytimer = 0;
    
    bflg_prot2_TC_first_delaytime = 1; //冷凝器脏判断延时
    gss_prot2_TC_first_delaytimer = 0;
    
    bflg_prot2_bat_low_delaytime = 1;  //电池电量低判断延时
    gss_prot2_bat_low_delaytimer = 0;
    
    bflg_prot2_bat_space_delaytime = 1;
    gss_prot2_bat_space_delaytimer = 0;

    bflg_discharge_delaytime = 1;    //初始上电延时3s判电池是否放电
    gss_discharge_delaytime = 0;
    //----------------------------------
    bflg_usb_delaytime = 1;          // 初始上电usb事件延时判断
    gss_usb_delaytime = 0;
    
    bflg_comp2_first_power_on = 1;   //高压机初次上电4500运行标志
}

/*****************************************************************************************************************************************
函数功能： 启动

函数位置：系统初始化(1)------------------------ok
*******************************************************************************************************************************************/
void system_start(void)  //系统启动程序
{
    R_TAU0_Channel0_Start();  //电池充电pwm
    
    R_ADC_Start();            //AD转换启动
    
    R_TAU0_Channel2_Start();  //1ms定时器
    
    R_TAU0_Channel5_Start();  //模拟IIC
    
    R_TAU1_Channel2_Start();  //压机方波
    R_TAU1_Channel3_Start();  //压机方波
}

/********************************************************************************************************************************************
函数功能：主循环函数

函数位置：主循环--------------------------------------ok
*********************************************************************************************************************************************/
void main_loop(void)     
{   
    //------------------------------------------------------
    timer_op();                    //定时程序
    //------------------------------------------------------
    if (bflg_allow_ad_calc == 1)   //如果允许ad计算
    {
        bflg_allow_ad_calc = 0;
        ad_temp_calc();           //ad温度计算程序
    }
    //------------------------------------------------------
    //控制程序
    if (bflg_comp2_power_on_delaytime == 0)   //初始上电多长时间后执行，之后一直为0；
    {
        comp2_ctrl_deal();     //高温压缩机控制处理程序
        
        comp21_ctrl_deal();    //低温压缩机控制处理程序
    }
    //------------------------------------------------------
    //输出程序
    if (bflg_test_mode == 1)       //如果是测试模式
    {
        test_mode_com_deal();      //测试模式通讯处理程序
        
        test_mode_output_deal();   //测试模式输出处理程序
        
        test_mode_input_deal();    //测试模式输入处理程序
    }
    else                           //如果是正常模式
    {
        output_deal();             //输出处理程序
        
        RL_output_deal();          //继电器输出处理程序        
    }

    //------------------------------------------------------
    if (bflg_com_allow_tx == 1)    //如果允许发送
    {
        bflg_com_allow_tx = 0;
        //------------------------------
        if(bflg_usb_delaytime == 0)
        {
            com_tx_usb_rank(); //usb报警事件的判定
        }
        R_UART0_Start();
        COM_TX_MODE;
        com_tx_init();        //通讯发送初始化程序
    }
    //----------------------------------
    if (bflg_com_tx_ok == 1)  //如果发送成功
    {
        bflg_com_tx_ok = 0;
        
        guc_IC_erasure_tx_ok_count++;
        if(guc_IC_erasure_tx_ok_count >= 4)    //发送给主控板2次之后再清0,防止发送过去主控没有收到；
        {
            guc_IC_erasure_tx_ok_count = 0;
            bflg_IC_erasure_order = 0;
        }
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_allow_rx == 1)    //如果允许接收
    {
        bflg_com_allow_rx = 0;
        //------------------------------
        com_rx_init();   //通讯接收初始化程序
        COM_RX_MODE;
        R_UART0_Start();
    }
    //----------------------------------
    if (bflg_com_rx_end == 1) //如果接收结束
    {
        bflg_com_rx_end = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_rx_ok == 1)  //如果接收成功
    {
        bflg_com_rx_ok = 0;
        //------------------------------
        R_UART0_Stop();
        com_rx_data_deal();   //通讯接收数据处理程序
    }
    //----------------------------------
    if (bflg_read_key_delaytime == 0)   //如果读键值时间间隔到
    {
        bflg_read_key_delaytime = 1;
        gss_read_key_delaytimer = 0;
        
        key_mode_select_deal();      //读键值处理程序
        key_buzz_cancel();           //报警取消30分钟
    }
    //----------------------------------
    if (bflg_ad_calc_ok == 1)      //采集5次结束
    {
        prot2_power_off_deal();    //断电保护处理程序
        
        port2_power_fault_deal();  //电源板故障处理程序
        
        prot2_bat_low_deal();      //电池电量低保护处理程序
        
        port2_T_fault_deal();      //传感器故障处理程序
        
        prot2_volt_deal();         //电压超标报警保护处理程序
       
        bat_T_hight_protect();     //电池温度过高判断peak
    }
    if((bflg_factory_AT_test == 0)&&(bflg_self_test == 0))    //没有自检和工厂指令检测时
    {      
        alarm_lamp_deal();   //报警灯处理程序
    }
    
    
    alarm_buzz_deal();   //蜂鸣器报警处理程序

    //----------------------------------
    if (bflg_test_mode == 1)  //如果是测试模式
    {
        RL_CON_PIN = 1;
        gss_bat_pwm_value = 50;  
        TDR01 = gss_bat_pwm_value;
    }
    else if ((bflg_alarm_bat_fed == 1) || (bflg_alarm_power_fault == 1) ||(bflg_alarm_bat_discnnt == 1)||
             (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight == 1))  //||(bflg_alarm_power_off == 1)
    {   
        bflg_bat_charging = 0;                     //若未达到充电情况则立即停充电不需10s了
        gss_bat_pwm_value = 0;
        TDR01 = gss_bat_pwm_value;
    }
    else if (bflg_comp2_power_on_delaytime == 0)   //上电延时到后才执行
    {
        if (bflg_bat_charge_update == 1)           //每10s更新一次,充电情况
        {
            bflg_bat_charge_update = 0;
            bat_charge_deal();                     //电池充电处理程序
            TDR01 = gss_bat_pwm_value;             
        }
    }
    
    //----------------------------------
    bat_con_check_again();  //peak add
    IC_unlock_deal();         //peak add
    key_factory_CL4_init();     //CL4恢复出厂值
    //----------------------------------
    RTC_continuous_w();
    RTC_continuous_r();
    //----------------------------------
    net_com_deal();
    three_led_deal();
    bat_type_check();
    
    buzz_sound_priority(); 
}

/********************************************************************************************************************************************
函数功能：    定时程序，各个时间到达后执行的动作

函数位置：主循环--------------------------------------ok
*********************************************************************************************************************************************/
void timer_op(void) 
{
    //------------------------------------------------------
    if (bflg_1ms_reach == 1)  //如果1ms定时到
    {
        bflg_1ms_reach = 0;
        guc_100ms_timer--;
        //--------------------------------------------------
        //1ms定时调用程序
        //--------------------------------------------------
        ad_convert_deal();    //ad转换处理程序
        //--------------------------------------------------
        com_rx_delaytime();   //通讯接收延时程序
        
        com_tx_delaytime();   //通讯发送延时程序
        
        com_rx_end_delaytime();    //通讯接收完成延时程序
        //--------------------------------------------------
        RL_output_delaytime();     //继电器输出延时程序
        
	    RTC_write_read_enable();
	    
	    net_com_interval();
    }
    //------------------------------------------------------
    if (bflg_10ms_reach == 1)    //10ms
    {
        bflg_10ms_reach = 0;
        guc_1s_timer--;
        //--------------------------------------------------
        //10ms定时调用程序        
        if(bflg_self_test == 1)             //初始上电时自检
        {   
            if(bflg_com_type_ok == 1)
            {              
                first_power_on_self_test();
            }
        }
        else if(bflg_factory_AT_test==1)    //在工厂参数测试指令中
        {
            key_factory_AT_test();
        }
        else
        {
            led_disp1_deal();          //数码管1显示处理程序
            
            led_disp2_deal();          //数码管2显示处理程序
            
            led_disp3_deal();          //数码管3显示处理程序
            
            lamp_disp_deal();          //灯显示处理程序
        }
        
        read_key_delaytime();          //读键延时程序
        
        input_pin_delaytime();         //输入引脚延时程序
        if(bflg_discharge_delaytime == 0)
        {
            bat_discharge_deal();      //电池放电处理
        }

        //测试人员显示用_用组网口—正式删
        net_led_disp1_deal_slave_1();
        net_led_disp2_deal_slave_1();
        net_led_disp3_deal_slave_1();
        net_led_disp1_deal_slave_2();
        net_led_disp2_deal_slave_2();
        net_led_disp3_deal_slave_2();
        net_led_disp1_deal_slave_3();
        net_led_disp2_deal_slave_3();
        net_led_disp3_deal_slave_3();

        buzz_loop_tick_delaytime(); 
    }
    //------------------------------------------------------
    if (guc_100ms_timer == 0)       //100ms
    {
        guc_100ms_timer = 100;
        //--------------------------------------------------
        key_locked_delaytime();         //按键锁定延时程序
        
        key_long_delaytime();           //按键解锁延时程序
        key_long_down_delaytime();
        key_long_set_delaytime();
        key_long_updown_delaytime();
        key_long_setdown_delaytime();
		        
        adg_data_delaytime();           //ad模块数据处理延时程序
        
        prot2_bat_space_delaytime();    //电池未连接检测间隔延时程序
        
        prot2_bat_discnnt_delaytime();  //电池未连接保护延时程序      //peak  原来给注释掉了---

		//peak add
		led_loop_delaytime();               //led闪烁标志
        led_disp1_flicker_delaytime();      //设定时数据闪烁
        buzz_one_tick_delaytime();          //响一声延时        
        buzz_sound_cnt();
        view_error_code_delaytime();        //错误代码显示时间
        //--------------------------------------------------
        RTC_read_interval();
    }
    //------------------------------------------------------
    if (guc_1s_timer == 0)        //1s
    {
        guc_1s_timer = 100;
        guc_1min_timer--;
        //--------------------------------------------------
        //1s定时调用程序
        //--------------------------------------------------
        test_mode_LED_deal();      //测试模式三色灯处理程序
        //--------------------------------------------------
        com_fault_delaytime();     //通讯故障延时程序
        //--------------------------------------------------
        comp2_power_on_delaytime();//上电延时程序
        
        comp2_run_stop_delaytime();//高温压缩机开关延时程序
        
        comp2_run_delaytime();     //高温压缩机运行延时程序
        
        comp2_stop_delaytime();    //高温压缩机停止延时程序
        
        comp2_TA_fault_stop_delaytime();//主传感器故障高温压缩机关机延时程序
        
        comp2_force_stop_delaytime();   //高温压缩机强制停止延时程序
        
        comp2_run_space_delaytime();    //高温压缩机开机间隔延时程序
        //--------------------------------------------------
        freq_ctrl_deal();
        TDR12 = gus_comp2_freq_cnt;
        TDR13 = gus_comp21_freq_cnt;
        //------------------------------
        comp2_start_delaytime();
        comp2_update_delaytime();
        comp21_start_delaytime();
        comp21_update_delaytime();
        //--------------------------------------------------
        outfan2_ctrl_deal();   //冷凝双风机控制处理程序 
        //--------------------------------------------------
        heat21_ctrl_delaytime();   //平衡口加热丝控制延时程序
        //--------------------------------------------------
        heat22_ctrl_deal();      
        //--------------------------------------------------
        if (bflg_prot2_TA_first_delaytime == 0)
        {
            prot2_TA_high_delaytime();  //主传感器高温保护延时程序
            
            prot2_TA_low_delaytime();   //主传感器低温保护延时程序
        }
        //--------------------------------------------------
        prot2_THW_high_delaytime();     //环温传感器过高保护延时程序
        //--------------------------------------------------
        port2_door_delaytime();         //门开关保护延时程序
        //--------------------------------------------------
        prot2_cond_dirty_delaytime();   //冷凝器脏保护延时程序
        //--------------------------------------------------
        comp2_volt_low_delaytime();     //高温压机电压过低延时程序                        
        //--------------------------------------------------
        bat_charge_delaytime();         //电池充电延时程序

		IC_unlock_delaytime();
		
		com_usb_delay_time();
		comp2_power_down_delaytime();
		key_RTC_time_parm_sent_usb_delaytime();
        
        bat_discharge_delaytime();
		tank_mouth_heat();	  
        key_password_long_valid_delaytime();
        led_power_off_disp_delaytime();
        alarm_power_off();
    }
    //------------------------------------------------------
    if (guc_1min_timer == 0)             //1min
    {
        guc_1min_timer = 60;
        //--------------------------------------------------
        //1min定时调用程序
        comp2_TA_fault_run_delaytime(); //主传感器故障高温压缩机开机延时程序
        
        comp2_continue_run_delaytime(); //高温压缩机连续运行延时程序  
        //--------------------------------------------------
        prot2_TA_first_delaytime();     //主传感器首次高低温保护延时程序
        
        prot2_TC_first_delaytime();     //冷凝器脏首次保护延时程序
        
        prot2_bat_low_delaytime();      //电池电量低延迟程序
        
        alarm_buzz_off_delaytime();     //蜂鸣器报警取消延时程序
        //--------------------------------------------------
        bat_charge_continue_delaytime();  //充电程序延时程序
        
        bat_charge_space_delaytime();     //电池充电间隔延时程序
    }
}

/************************************************************************************************************************************
函数功能:       分别采集单个通道的ad值//ad转换处理程序

名词解析: ADCR: 转换结果寄存器
          ADCR  寄存器将 A/D 转换结果保持在其高 10 位（低 6 位固定为 0）;
                因为用的是10位ad转换；
          
          ADS: 模拟输入通道选择寄存器
          
函数位置: 1ms定时中--------------------------------------------------ok
*************************************************************************************************************************************/
void ad_convert_deal(void) 
{
    gus_ad_val = (uint16_t)(ADCR >> 6U);
    //------------------------------------------------------
    ad_val_deal();  //ad值处理程序
    //------------------------------------------------------
    ADS = (uint8_t)(guc_ad_index + 2); //ad通道选择，peak因为硬件从第2通道开始的
    //------------------------------------------------------
    R_ADC_Start();  //启动ad转换 peak 每次都要开始还是每改变一次通道都要开始；
}

/************************************************************************************************************************************
函数功能：检测10ms定时达到

函数位置；   系统1ms中断  ----------------------------ok
************************************************************************************************************************************/
void timer_int(void)  
{
    bflg_1ms_reach = 1;       //置1ms到标志位
    //----------------------------------
    guc_10ms_timer++;
    if (guc_10ms_timer >= 10) //10ms计时到
    {
        guc_10ms_timer = 0;
        bflg_10ms_reach = 1;
    }
}


//------------------------------------------------------------------------------
void init_ram_para2(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARA_NUMBER2; i++)
    {
        //ram_para2[i] = EEPROM_init2[i][0];
        eeprom_write(i + 256, EEPROM_init2[i][0]);
    }
}
//------------------------------------------------------------------------------
void init_ram_para(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARA_NUMBER; i++)
    {
        //ram_para[i] = EEPROM_init[i][0];
        eeprom_write(i, EEPROM_init[i][0]);
    }
}
//------------------------------------------------------------------------------
void eeprom2_read_deal(void)  //eeprom2读取处理程序，在初始化程序中调用
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //清请求读EEP标志
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER2; i++)
        {
            ram_para2[i] = eeprom_read(i + 256); //从EEP读参数到RAM中
            R_WDT_Restart();
            
            if ((ram_para2[i] < EEPROM_init2[i][min_row])
               || (ram_para2[i] > EEPROM_init2[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //置读EEPROM错标志
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //如果有读EEPROM错标志
        {
            bflg_read_eeprom_error = 0;      //清读EEPROM错标志
            
            luc_read_para_cnt++;   //读EEP次数加1
            if (luc_read_para_cnt >= 3)      //如果读3次错
            {
                //EEPROM故障处理
                bflg_eeprom_fault = 1;  //置EEP故障标志
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //置请求读EEPROM标志
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void eeprom_read_deal(void)   //eeprom读取处理程序，在初始化程序中调用
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //清请求读EEP标志
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER; i++)
        {
            ram_para[i] = eeprom_read(i);     //从EEP读参数到RAM中
            R_WDT_Restart();
            
            if ((ram_para[i] < EEPROM_init[i][min_row])
               || (ram_para[i] > EEPROM_init[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //置读EEPROM错标志
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //如果有读EEPROM错标志
        {
            bflg_read_eeprom_error = 0;      //清读EEPROM错标志
            
            luc_read_para_cnt++;   //读EEP次数加1
            if (luc_read_para_cnt >= 3)      //如果读3次错
            {
                //EEPROM故障处理
                bflg_eeprom_fault = 1;  //置EEP故障标志
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //置请求读EEPROM标志
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void delay_10us(uint16_t _10us)    //10us延时程序，在eeprom读写程序中调用
{
    uint16_t i,j;
    for (i = 0; i < (_10us * 2); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void delay_1ms(uint16_t _1ms)      //1ms延时程序，在eeprom读写程序中调用
{
    uint16_t i,j;
    for (i = 0; i < (_1ms * 159); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data)    //写eeprom程序
{
    uint8_t luc_page_addr;
    uint8_t luc_unit_addr;
    uint8_t luc_driver_write;
    
    uint8_t luc_eep_data_H;
    uint8_t luc_eep_data_L;
    
    luc_page_addr = (uint8_t) (lus_addr * 2 / 256);
    luc_unit_addr = (uint8_t) (lus_addr * 2 % 256);
    luc_driver_write = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA0);
    
    luc_eep_data_H = (uint8_t) (((uint16_t) lss_eep_data) / 256);
    luc_eep_data_L = (uint8_t) (((uint16_t) lss_eep_data) % 256);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1) {}        //总线忙，则等待
    
    STT0 = 1;                      //产生start信号
    WREL0 = 0;                     //发送模式
    ACKE0 = 0U;                    //无ACK信号
    IICA0 = luc_driver_write;      //写驱动地址
    delay_10us(20);                //等待
    
    IICA0 = luc_unit_addr;         //单元地址
    delay_10us(20);                //等待
    
    IICA0 = luc_eep_data_H;            
    delay_10us(20);                //等待
    SPT0 = 1;                      //产生stop信号
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //总线忙，则等待
    
    STT0 = 1;                      //产生start信号
    WREL0 = 0;                     //发送模式
    ACKE0 = 0U;                    //无ACK信号
    IICA0 = luc_driver_write;      //写驱动地址
    delay_10us(20);                //等待
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //单元地址
    delay_10us(20);                //等待
    
    IICA0 = luc_eep_data_L;
    delay_10us(20);                //等待
    SPT0 = 1;                      //产生stop信号
    //------------------------------------------------------
    delay_1ms(3);
}
//------------------------------------------------------------------------------
int16_t eeprom_read(uint16_t lus_addr)  //读eeprom程序
{
    uint8_t luc_page_addr;
    uint8_t luc_unit_addr;
    uint8_t luc_driver_write;
    uint8_t luc_driver_read;
    uint8_t luc_eep_data_H;
    uint8_t luc_eep_data_L;
    
    luc_page_addr = (uint8_t) (lus_addr * 2 / 256);
    luc_unit_addr = (uint8_t) (lus_addr * 2 % 256);
    luc_driver_write = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA0);
    luc_driver_read = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA1);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1) {}        //总线忙，则等待
    
    STT0 = 1;                      //产生start信号
    WREL0 = 0;                     //发送模式
    ACKE0 = 0U;                    //无ACK信号
    IICA0 = luc_driver_write;      //写驱动地址
    delay_10us(20);                //等待
    
    IICA0 = luc_unit_addr;         //单元地址
    delay_10us(20);                //等待
    
    STT0 = 1;                      //产生start信号
    IICA0 = luc_driver_read;       //读驱动地址    
    delay_10us(20);                //等待
    WREL0 = 1;                     //接收模式
    
    delay_10us(20);                //等待
    luc_eep_data_H = IICA0;        //得到数据
    ACKE0 = 1;                     //产生ACK信号
    SPT0 = 1;                      //产生stop信号
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //总线忙，则等待
    
    STT0 = 1;                      //产生start信号
    WREL0 = 0;                     //发送模式
    ACKE0 = 0U;                    //无ACK信号
    IICA0 = luc_driver_write;      //写驱动地址
    delay_10us(20);                //等待
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //单元地址
    delay_10us(20);                //等待
    
    STT0 = 1;                      //产生start信号
    IICA0 = luc_driver_read;       //读驱动地址
    delay_10us(20);                //等待
    WREL0 = 1;                     //接收模式
    
    delay_10us(20);                //等待
    luc_eep_data_L = IICA0;        //读数据
    ACKE0 = 1;                     //产生ACK信号
    SPT0 = 1;                      //产生stop信号
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    return ((int16_t) (luc_eep_data_H) * 256 + luc_eep_data_L);
}

/****************************************************************************************************************************
函数功能；操作层参数刚上电时的读取：设置层、用户层、工厂层

函数位置：1、系统初始化 2、恢复初始设置------------------未调
******************************************************************************************************************************/
void operation_parm_read(void)
{
    //3个设置参数
    gss_set_parm_value[DISP_SET_TS]  = ram_para[num_comp2_TS];
    gss_set_parm_value[DISP_SET_ALH] = ram_para[num_comp2_ALH];
    gss_set_parm_value[DISP_SET_ALL] = ram_para[num_comp2_ALL];
    //--------------------------------------------------------------
    //用户参数
    gss_user_parm_value[DISP_USER_dA]  = ram_para[num_door_alarm_delaytime];
    gss_user_parm_value[DISP_USER_T1]  = ram_para[num_usb_read_delaytime];
    //gss_user_parm_value[DISP_USER_T2]  = no save
    gss_user_parm_value[DISP_USER_P6]  = ram_para[num_usb_P6];
    gss_user_parm_value[DISP_USER_IC]  = ram_para[num_IC_password];
    gss_user_parm_value[DISP_USER_PS1] = ram_para[num_password_PS1];
    //gss_user_parm_value[DISP_USER_CL1] = no save
    //-------------------------------------------------------------
    //工厂参数
    //gss_factory_parm_value[DISP_FACTORY_CL2]    
    gss_factory_parm_value[DISP_FACTORY_UL] = ram_para[num_usb_language]; 
    gss_factory_parm_value[DISP_FACTORY_Ct] = ram_para[num_pt100_calibration];     
    gss_factory_parm_value[DISP_FACTORY_CU] = ram_para[num_voltage_calibration];  
    gss_factory_parm_value[DISP_FACTORY_CA] = ram_para[num_THW_calibration];  
    gss_factory_parm_value[DISP_FACTORY_Fd] = ram_para[num_buzz_alarm_delaytime];  
    //gss_factory_parm_value[DISP_FACTORY_AT]  
    gss_factory_parm_value[DISP_FACTORY_SS] = ram_para[num_TE_fault_shield];  
    gss_factory_parm_value[DISP_FACTORY_AA] = ram_para[num_THW_hight_alarm];  
    gss_factory_parm_value[DISP_FACTORY_Cd] = ram_para[num_comp_startup_delaytime];  
    //gss_factory_parm_value[DISP_FACTORY_CL3] 
    //gss_factory_parm_value[DISP_FACTORY_CL4]  
    gss_factory_parm_value[DISP_FACTORY_PS2] =  ram_para[num_password_val];  
    //gss_factory_parm_value[DISP_FACTORY_bS]  
    gss_factory_parm_value[DISP_FACTORY_LL]  =  ram_para[num_secret_key]; 
    gss_factory_parm_value[DISP_FACTORY_HC]  =  ram_para[num_tank_mouth_heat];
}















#endif
/*************************************END OF THE FILE*********************************************************/
