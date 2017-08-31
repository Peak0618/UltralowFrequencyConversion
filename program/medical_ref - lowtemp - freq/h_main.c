/********************************************************************************************************************************
overview:  -- �����±�Ƶ�������

        1��ϵͳ��ʼ��    
        2����ѭ������ʱ��ѭ��
        3��E2�Ķ�д����ʼ��
        4����Կδ����ʱ��ѭ��
*********************************************************************************************************************************/

#ifndef _MAIN_C_
#define _MAIN_C_

//------------------------------------------------------------------------------
//�����ļ�����
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
//��������
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
uint8_t   guc_10ms_timer;     //10�����ʱ��
uint8_t   guc_100ms_timer;    //100ms��ʱ��
uint8_t   guc_1s_timer;       //1s��ʱ��
uint8_t   guc_1min_timer;     //1min��ʱ��

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
�������ܣ�ϵͳ����ʼ��

����λ�ã�ϵͳ��ʼ��(1)-----------------------------------ok
*******************************************************************************************************************************************/
void system_init(void)    
{
    int16_t    gss_E2_writed_flag;
        
    switch_init();            //���뿪�س�ʼ��

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
    //init_ram_para2();       //�������޸�e2�����򿪣���ʽ�ر�
    //init_ram_para();

    //��ȡE2    
    eeprom2_read_deal(); //eeprom2��ȡ�������
    eeprom_read_deal();  //eeprom��ȡ�������

    operation_parm_read();     //�Ӷ�ȡ��E2�������л�ȡ��Ҫ��ֵ
    init_freq_ctrl();          //��Ƶ���ƵĲ�����1���й̶�ֵ2��E2�ж�ȡ��ֵ
    //--------------------------------------------------
    R_ADC_Set_OperationOn();   //adת������
    //--------------------------------------------------
    com_init();               //ͨѶ��ʼ������
    //--------------------------------------------------
    adg_init();               //�ȵ�żadģ���ʼ������
    //--------------------------------------------------
    control_init();           //���Ƴ�ʼ������
    //--------------------------------------------------
    RTC_init();               //RTCʱ�ӳ�ʼ��
}

/*****************************************************************************************************************************************
�������ܣ����뿪�س�ʼ��

����λ�ã�ϵͳ��ʼ��(1)--------------------------------------------ok
*******************************************************************************************************************************************/
void switch_init(void)        
{
    if (SW21_IN_PIN == 1)          //��·Ĭ��Ϊ�ߵ�ƽ
    {
        bflg_comp2_volt_type = 0;  //220V����
    }
    else
    {
        bflg_comp2_volt_type = 1;  //115V����
    }
}

/*****************************************************************************************************************************************
�������ܣ�IO�ڳ�ʼ����������־��ʼ��

����λ�ã�ϵͳ��ʼ��(1)------------------------ok
*******************************************************************************************************************************************/
void control_init(void)  
{
    bflg_self_test = 1;       //��ʼ�ϵ�ʱϵͳ�Լ��־

    //12V��ʼ���ر�
    LOCK_OUT_PIN = 0;   //����
    HTRMT_OUT_PIN = 0;  //������ȹ�
    HTRMXG_OUT_PIN = 0; //Ԥ����
    HTRPHK_OUT_PIN = 0; //ƽ��ڹ�
    //------------------------------------------------------
    TRIP_LAMP_PIN = 0;   //����(֮ǰ��ëϸ��ָʾ�ƣ��ֲ���Ҳ��)
    //------------------------------------------------------
    guc_100ms_timer = 100; //��ʱ��
    guc_1s_timer = 100;
    guc_1min_timer = 60;
    //------------------------------------------------------
    guc_ad_calc_cnt = 0;
    bflg_ad_calc_ok = 0;
    //------------------------------------------------------
    BAT_DISCHARGE_PIN = 0;     //���Ƶ�����������
    
    bflg_bat_frist_charge = 1; //�״��ϵ���е�س���־
    //------------------------------------------------------
    bflg_adg_reset = 1;        //adģ�鸴λ
    //------------------------------------------------------
    bflg_comp2_first_run = 1;  //��ѹ���״����б�־
    
    bflg_comp2_power_on_delaytime = 1; //��ʼ�ϵ���ʱ
    gss_comp2_power_on_delaytimer = 0;
    
    bflg_prot2_TA_first_delaytime = 1; //�����������±�����ʱ
    gss_prot2_TA_first_delaytimer = 0;
    
    bflg_prot2_TC_first_delaytime = 1; //���������ж���ʱ
    gss_prot2_TC_first_delaytimer = 0;
    
    bflg_prot2_bat_low_delaytime = 1;  //��ص������ж���ʱ
    gss_prot2_bat_low_delaytimer = 0;
    
    bflg_prot2_bat_space_delaytime = 1;
    gss_prot2_bat_space_delaytimer = 0;

    bflg_discharge_delaytime = 1;    //��ʼ�ϵ���ʱ3s�е���Ƿ�ŵ�
    gss_discharge_delaytime = 0;
    //----------------------------------
    bflg_usb_delaytime = 1;          // ��ʼ�ϵ�usb�¼���ʱ�ж�
    gss_usb_delaytime = 0;
    
    bflg_comp2_first_power_on = 1;   //��ѹ�������ϵ�4500���б�־
}

/*****************************************************************************************************************************************
�������ܣ� ����

����λ�ã�ϵͳ��ʼ��(1)------------------------ok
*******************************************************************************************************************************************/
void system_start(void)  //ϵͳ��������
{
    R_TAU0_Channel0_Start();  //��س��pwm
    
    R_ADC_Start();            //ADת������
    
    R_TAU0_Channel2_Start();  //1ms��ʱ��
    
    R_TAU0_Channel5_Start();  //ģ��IIC
    
    R_TAU1_Channel2_Start();  //ѹ������
    R_TAU1_Channel3_Start();  //ѹ������
}

/********************************************************************************************************************************************
�������ܣ���ѭ������

����λ�ã���ѭ��--------------------------------------ok
*********************************************************************************************************************************************/
void main_loop(void)     
{   
    //------------------------------------------------------
    timer_op();                    //��ʱ����
    //------------------------------------------------------
    if (bflg_allow_ad_calc == 1)   //�������ad����
    {
        bflg_allow_ad_calc = 0;
        ad_temp_calc();           //ad�¶ȼ������
    }
    //------------------------------------------------------
    //���Ƴ���
    if (bflg_comp2_power_on_delaytime == 0)   //��ʼ�ϵ�೤ʱ���ִ�У�֮��һֱΪ0��
    {
        comp2_ctrl_deal();     //����ѹ�������ƴ������
        
        comp21_ctrl_deal();    //����ѹ�������ƴ������
    }
    //------------------------------------------------------
    //�������
    if (bflg_test_mode == 1)       //����ǲ���ģʽ
    {
        test_mode_com_deal();      //����ģʽͨѶ�������
        
        test_mode_output_deal();   //����ģʽ����������
        
        test_mode_input_deal();    //����ģʽ���봦�����
    }
    else                           //���������ģʽ
    {
        output_deal();             //����������
        
        RL_output_deal();          //�̵�������������        
    }

    //------------------------------------------------------
    if (bflg_com_allow_tx == 1)    //���������
    {
        bflg_com_allow_tx = 0;
        //------------------------------
        if(bflg_usb_delaytime == 0)
        {
            com_tx_usb_rank(); //usb�����¼����ж�
        }
        R_UART0_Start();
        COM_TX_MODE;
        com_tx_init();        //ͨѶ���ͳ�ʼ������
    }
    //----------------------------------
    if (bflg_com_tx_ok == 1)  //������ͳɹ�
    {
        bflg_com_tx_ok = 0;
        
        guc_IC_erasure_tx_ok_count++;
        if(guc_IC_erasure_tx_ok_count >= 4)    //���͸����ذ�2��֮������0,��ֹ���͹�ȥ����û���յ���
        {
            guc_IC_erasure_tx_ok_count = 0;
            bflg_IC_erasure_order = 0;
        }
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_allow_rx == 1)    //����������
    {
        bflg_com_allow_rx = 0;
        //------------------------------
        com_rx_init();   //ͨѶ���ճ�ʼ������
        COM_RX_MODE;
        R_UART0_Start();
    }
    //----------------------------------
    if (bflg_com_rx_end == 1) //������ս���
    {
        bflg_com_rx_end = 0;
        //------------------------------
        R_UART0_Stop();
    }
    //----------------------------------
    if (bflg_com_rx_ok == 1)  //������ճɹ�
    {
        bflg_com_rx_ok = 0;
        //------------------------------
        R_UART0_Stop();
        com_rx_data_deal();   //ͨѶ�������ݴ������
    }
    //----------------------------------
    if (bflg_read_key_delaytime == 0)   //�������ֵʱ������
    {
        bflg_read_key_delaytime = 1;
        gss_read_key_delaytimer = 0;
        
        key_mode_select_deal();      //����ֵ�������
        key_buzz_cancel();           //����ȡ��30����
    }
    //----------------------------------
    if (bflg_ad_calc_ok == 1)      //�ɼ�5�ν���
    {
        prot2_power_off_deal();    //�ϵ籣���������
        
        port2_power_fault_deal();  //��Դ����ϴ������
        
        prot2_bat_low_deal();      //��ص����ͱ����������
        
        port2_T_fault_deal();      //���������ϴ������
        
        prot2_volt_deal();         //��ѹ���걨�������������
       
        bat_T_hight_protect();     //����¶ȹ����ж�peak
    }
    if((bflg_factory_AT_test == 0)&&(bflg_self_test == 0))    //û���Լ�͹���ָ����ʱ
    {      
        alarm_lamp_deal();   //�����ƴ������
    }
    
    
    alarm_buzz_deal();   //�����������������

    //----------------------------------
    if (bflg_test_mode == 1)  //����ǲ���ģʽ
    {
        RL_CON_PIN = 1;
        gss_bat_pwm_value = 50;  
        TDR01 = gss_bat_pwm_value;
    }
    else if ((bflg_alarm_bat_fed == 1) || (bflg_alarm_power_fault == 1) ||(bflg_alarm_bat_discnnt == 1)||
             (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight == 1))  //||(bflg_alarm_power_off == 1)
    {   
        bflg_bat_charging = 0;                     //��δ�ﵽ������������ͣ��粻��10s��
        gss_bat_pwm_value = 0;
        TDR01 = gss_bat_pwm_value;
    }
    else if (bflg_comp2_power_on_delaytime == 0)   //�ϵ���ʱ�����ִ��
    {
        if (bflg_bat_charge_update == 1)           //ÿ10s����һ��,������
        {
            bflg_bat_charge_update = 0;
            bat_charge_deal();                     //��س�紦�����
            TDR01 = gss_bat_pwm_value;             
        }
    }
    
    //----------------------------------
    bat_con_check_again();  //peak add
    IC_unlock_deal();         //peak add
    key_factory_CL4_init();     //CL4�ָ�����ֵ
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
�������ܣ�    ��ʱ���򣬸���ʱ�䵽���ִ�еĶ���

����λ�ã���ѭ��--------------------------------------ok
*********************************************************************************************************************************************/
void timer_op(void) 
{
    //------------------------------------------------------
    if (bflg_1ms_reach == 1)  //���1ms��ʱ��
    {
        bflg_1ms_reach = 0;
        guc_100ms_timer--;
        //--------------------------------------------------
        //1ms��ʱ���ó���
        //--------------------------------------------------
        ad_convert_deal();    //adת���������
        //--------------------------------------------------
        com_rx_delaytime();   //ͨѶ������ʱ����
        
        com_tx_delaytime();   //ͨѶ������ʱ����
        
        com_rx_end_delaytime();    //ͨѶ���������ʱ����
        //--------------------------------------------------
        RL_output_delaytime();     //�̵��������ʱ����
        
	    RTC_write_read_enable();
	    
	    net_com_interval();
    }
    //------------------------------------------------------
    if (bflg_10ms_reach == 1)    //10ms
    {
        bflg_10ms_reach = 0;
        guc_1s_timer--;
        //--------------------------------------------------
        //10ms��ʱ���ó���        
        if(bflg_self_test == 1)             //��ʼ�ϵ�ʱ�Լ�
        {   
            if(bflg_com_type_ok == 1)
            {              
                first_power_on_self_test();
            }
        }
        else if(bflg_factory_AT_test==1)    //�ڹ�����������ָ����
        {
            key_factory_AT_test();
        }
        else
        {
            led_disp1_deal();          //�����1��ʾ�������
            
            led_disp2_deal();          //�����2��ʾ�������
            
            led_disp3_deal();          //�����3��ʾ�������
            
            lamp_disp_deal();          //����ʾ�������
        }
        
        read_key_delaytime();          //������ʱ����
        
        input_pin_delaytime();         //����������ʱ����
        if(bflg_discharge_delaytime == 0)
        {
            bat_discharge_deal();      //��طŵ紦��
        }

        //������Ա��ʾ��_�������ڡ���ʽɾ
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
        key_locked_delaytime();         //����������ʱ����
        
        key_long_delaytime();           //����������ʱ����
        key_long_down_delaytime();
        key_long_set_delaytime();
        key_long_updown_delaytime();
        key_long_setdown_delaytime();
		        
        adg_data_delaytime();           //adģ�����ݴ�����ʱ����
        
        prot2_bat_space_delaytime();    //���δ���Ӽ������ʱ����
        
        prot2_bat_discnnt_delaytime();  //���δ���ӱ�����ʱ����      //peak  ԭ����ע�͵���---

		//peak add
		led_loop_delaytime();               //led��˸��־
        led_disp1_flicker_delaytime();      //�趨ʱ������˸
        buzz_one_tick_delaytime();          //��һ����ʱ        
        buzz_sound_cnt();
        view_error_code_delaytime();        //���������ʾʱ��
        //--------------------------------------------------
        RTC_read_interval();
    }
    //------------------------------------------------------
    if (guc_1s_timer == 0)        //1s
    {
        guc_1s_timer = 100;
        guc_1min_timer--;
        //--------------------------------------------------
        //1s��ʱ���ó���
        //--------------------------------------------------
        test_mode_LED_deal();      //����ģʽ��ɫ�ƴ������
        //--------------------------------------------------
        com_fault_delaytime();     //ͨѶ������ʱ����
        //--------------------------------------------------
        comp2_power_on_delaytime();//�ϵ���ʱ����
        
        comp2_run_stop_delaytime();//����ѹ����������ʱ����
        
        comp2_run_delaytime();     //����ѹ����������ʱ����
        
        comp2_stop_delaytime();    //����ѹ����ֹͣ��ʱ����
        
        comp2_TA_fault_stop_delaytime();//�����������ϸ���ѹ�����ػ���ʱ����
        
        comp2_force_stop_delaytime();   //����ѹ����ǿ��ֹͣ��ʱ����
        
        comp2_run_space_delaytime();    //����ѹ�������������ʱ����
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
        outfan2_ctrl_deal();   //����˫������ƴ������ 
        //--------------------------------------------------
        heat21_ctrl_delaytime();   //ƽ��ڼ���˿������ʱ����
        //--------------------------------------------------
        heat22_ctrl_deal();      
        //--------------------------------------------------
        if (bflg_prot2_TA_first_delaytime == 0)
        {
            prot2_TA_high_delaytime();  //�����������±�����ʱ����
            
            prot2_TA_low_delaytime();   //�����������±�����ʱ����
        }
        //--------------------------------------------------
        prot2_THW_high_delaytime();     //���´��������߱�����ʱ����
        //--------------------------------------------------
        port2_door_delaytime();         //�ſ��ر�����ʱ����
        //--------------------------------------------------
        prot2_cond_dirty_delaytime();   //�������ౣ����ʱ����
        //--------------------------------------------------
        comp2_volt_low_delaytime();     //����ѹ����ѹ������ʱ����                        
        //--------------------------------------------------
        bat_charge_delaytime();         //��س����ʱ����

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
        //1min��ʱ���ó���
        comp2_TA_fault_run_delaytime(); //�����������ϸ���ѹ����������ʱ����
        
        comp2_continue_run_delaytime(); //����ѹ��������������ʱ����  
        //--------------------------------------------------
        prot2_TA_first_delaytime();     //���������״θߵ��±�����ʱ����
        
        prot2_TC_first_delaytime();     //���������״α�����ʱ����
        
        prot2_bat_low_delaytime();      //��ص������ӳٳ���
        
        alarm_buzz_off_delaytime();     //����������ȡ����ʱ����
        //--------------------------------------------------
        bat_charge_continue_delaytime();  //��������ʱ����
        
        bat_charge_space_delaytime();     //��س������ʱ����
    }
}

/************************************************************************************************************************************
��������:       �ֱ�ɼ�����ͨ����adֵ//adת���������

���ʽ���: ADCR: ת������Ĵ���
          ADCR  �Ĵ����� A/D ת�������������� 10 λ���� 6 λ�̶�Ϊ 0��;
                ��Ϊ�õ���10λadת����
          
          ADS: ģ������ͨ��ѡ��Ĵ���
          
����λ��: 1ms��ʱ��--------------------------------------------------ok
*************************************************************************************************************************************/
void ad_convert_deal(void) 
{
    gus_ad_val = (uint16_t)(ADCR >> 6U);
    //------------------------------------------------------
    ad_val_deal();  //adֵ�������
    //------------------------------------------------------
    ADS = (uint8_t)(guc_ad_index + 2); //adͨ��ѡ��peak��ΪӲ���ӵ�2ͨ����ʼ��
    //------------------------------------------------------
    R_ADC_Start();  //����adת�� peak ÿ�ζ�Ҫ��ʼ����ÿ�ı�һ��ͨ����Ҫ��ʼ��
}

/************************************************************************************************************************************
�������ܣ����10ms��ʱ�ﵽ

����λ�ã�   ϵͳ1ms�ж�  ----------------------------ok
************************************************************************************************************************************/
void timer_int(void)  
{
    bflg_1ms_reach = 1;       //��1ms����־λ
    //----------------------------------
    guc_10ms_timer++;
    if (guc_10ms_timer >= 10) //10ms��ʱ��
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
void eeprom2_read_deal(void)  //eeprom2��ȡ��������ڳ�ʼ�������е���
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //�������EEP��־
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER2; i++)
        {
            ram_para2[i] = eeprom_read(i + 256); //��EEP��������RAM��
            R_WDT_Restart();
            
            if ((ram_para2[i] < EEPROM_init2[i][min_row])
               || (ram_para2[i] > EEPROM_init2[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //�ö�EEPROM���־
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //����ж�EEPROM���־
        {
            bflg_read_eeprom_error = 0;      //���EEPROM���־
            
            luc_read_para_cnt++;   //��EEP������1
            if (luc_read_para_cnt >= 3)      //�����3�δ�
            {
                //EEPROM���ϴ���
                bflg_eeprom_fault = 1;  //��EEP���ϱ�־
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //�������EEPROM��־
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void eeprom_read_deal(void)   //eeprom��ȡ��������ڳ�ʼ�������е���
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //�������EEP��־
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER; i++)
        {
            ram_para[i] = eeprom_read(i);     //��EEP��������RAM��
            R_WDT_Restart();
            
            if ((ram_para[i] < EEPROM_init[i][min_row])
               || (ram_para[i] > EEPROM_init[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //�ö�EEPROM���־
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //����ж�EEPROM���־
        {
            bflg_read_eeprom_error = 0;      //���EEPROM���־
            
            luc_read_para_cnt++;   //��EEP������1
            if (luc_read_para_cnt >= 3)      //�����3�δ�
            {
                //EEPROM���ϴ���
                bflg_eeprom_fault = 1;  //��EEP���ϱ�־
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //�������EEPROM��־
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void delay_10us(uint16_t _10us)    //10us��ʱ������eeprom��д�����е���
{
    uint16_t i,j;
    for (i = 0; i < (_10us * 2); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void delay_1ms(uint16_t _1ms)      //1ms��ʱ������eeprom��д�����е���
{
    uint16_t i,j;
    for (i = 0; i < (_1ms * 159); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data)    //дeeprom����
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
    while (IICBSY0 == 1) {}        //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_unit_addr;         //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_eep_data_H;            
    delay_10us(20);                //�ȴ�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_eep_data_L;
    delay_10us(20);                //�ȴ�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
}
//------------------------------------------------------------------------------
int16_t eeprom_read(uint16_t lus_addr)  //��eeprom����
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
    while (IICBSY0 == 1) {}        //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_unit_addr;         //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    STT0 = 1;                      //����start�ź�
    IICA0 = luc_driver_read;       //��������ַ    
    delay_10us(20);                //�ȴ�
    WREL0 = 1;                     //����ģʽ
    
    delay_10us(20);                //�ȴ�
    luc_eep_data_H = IICA0;        //�õ�����
    ACKE0 = 1;                     //����ACK�ź�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    STT0 = 1;                      //����start�ź�
    IICA0 = luc_driver_read;       //��������ַ
    delay_10us(20);                //�ȴ�
    WREL0 = 1;                     //����ģʽ
    
    delay_10us(20);                //�ȴ�
    luc_eep_data_L = IICA0;        //������
    ACKE0 = 1;                     //����ACK�ź�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    return ((int16_t) (luc_eep_data_H) * 256 + luc_eep_data_L);
}

/****************************************************************************************************************************
�������ܣ�������������ϵ�ʱ�Ķ�ȡ�����ò㡢�û��㡢������

����λ�ã�1��ϵͳ��ʼ�� 2���ָ���ʼ����------------------δ��
******************************************************************************************************************************/
void operation_parm_read(void)
{
    //3�����ò���
    gss_set_parm_value[DISP_SET_TS]  = ram_para[num_comp2_TS];
    gss_set_parm_value[DISP_SET_ALH] = ram_para[num_comp2_ALH];
    gss_set_parm_value[DISP_SET_ALL] = ram_para[num_comp2_ALL];
    //--------------------------------------------------------------
    //�û�����
    gss_user_parm_value[DISP_USER_dA]  = ram_para[num_door_alarm_delaytime];
    gss_user_parm_value[DISP_USER_T1]  = ram_para[num_usb_read_delaytime];
    //gss_user_parm_value[DISP_USER_T2]  = no save
    gss_user_parm_value[DISP_USER_P6]  = ram_para[num_usb_P6];
    gss_user_parm_value[DISP_USER_IC]  = ram_para[num_IC_password];
    gss_user_parm_value[DISP_USER_PS1] = ram_para[num_password_PS1];
    //gss_user_parm_value[DISP_USER_CL1] = no save
    //-------------------------------------------------------------
    //��������
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
