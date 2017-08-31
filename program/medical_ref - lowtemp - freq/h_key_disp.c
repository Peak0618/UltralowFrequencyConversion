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
uint8_t   guc_disp_rate;                 //��ʾ���ʼĴ���
int16_t   gss_error_code_delaytime;      //����������ʾ��ʱʱ��
int16_t   gss_power_off_dis_delaytime;   //�ϵ�ʱ�������ʾ��ʱ

//flag
flag_type flg_key_disp;

/******************************************************************************************************************
��������: ���������1��ʾ������:LED1~LED4
               
 
          LED8 xxxx xxxx --��4λ�Ӹߵ��׶�Ӧ: �����������¶��趨�����±����趨�����±����趨

����λ�ã�10ms----------------------------------------------ok
******************************************************************************************************************/
void led_disp1_deal(void)      
{
    if (bflg_test_mode == 1)              //����ǲ���ģʽ
    {
		R_TAU0_Channel3_Stop();
        if(bflg_test_mode_RTC == 0)        //δ����buzz��
        {
	        if (bflg_test_ok == 0)         //���δ�������
	        {
	            guc_disp_rate = dot_0d001;
	            led1_disp_val(SOFT_VERSION);
	        }
	        else                            //����������
	        {
	            if (guc_test_mode_code == 0)    
	            {
	                LED1 = data_P;
	                LED2 = data_A;
	                LED3 = data_S;
	                LED4 = data_S;
	            }
	            else                            //����в��Թ���
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
    else if (guc_disp_mode == DISP_LOCKED)     //���������ģʽ
    {        
		if ((flag_view_error_code == 0) )	   //peak add  ��û�д������������ʾ   1������"����ȡ��"  2���б���  3������״̬
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
			  //||(bflg_alarm_power_fault == 1)// 4          //����������ȥ��
			  //|| (bflg_alarm_volt == 1)      // 15			
            {
	            if (guc_key_val == KEY_BUZ)                  // peak ������� "����ȡ��"       				      
	            {
                    flag_view_error_code = 1;				
	            }
            }
            if(bflg_alarm_power_off == 1)      //�ϵ�
            {
                if(bflg_power_off_dis == 1)//��ʾ5s
                {
                    led_gss_TA_deal();
                }
                else //����30s
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
		    view_error_code();      //peak add��ʾ�������
		}
        
        LED8 |= 0x08;               //peak ����״̬ ��������
        LED8 &= ~0x07;              //peak �� "�����¶��趨" "���±����趨" "���±����趨"
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
	else if(guc_disp_mode == DISP_UNLOCKED)           //����ǽ���״̬��׼��������������ģʽ
	{
        led_gss_TA_deal();

        LED8 &= ~0x08;                                //�ص�"����"��
        LED8 &= ~0x07;                                //�� "�����¶��趨" "���±����趨" "���±����趨"
	}
    else if(guc_disp_mode == DISP_SET) 
    {   
        
        LED8 &= ~0x08;                      //�ص�"����"��
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

        
        if(bflg_disp1_flicker == 1)               //��˸
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
        
        LED8 &= ~0x07;                   // �� "TS" "ALH" "ALL"��
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
            if(bflg_disp1_flicker == 1)         //��˸
            {
                if(gsc_disp_parm_type == DISP_USER_T2) //special ��ͱ����޹�
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
                    else                           //��ʱֵ
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
    else if (guc_disp_mode == DISP_FAC_PASSWORD)      //����ǹ�������ģʽ
    {
        guc_disp_rate = not_dot;
        led1_disp_val(gsc_password_val);
        if(gsc_password_val/10 == 0)                  //Ϊ����ʾ 00 
        {LED3 = data_0;}
        
        LED8 &= ~0x07;                                 //�� "�����¶��趨" "���±����趨" "���±����趨"
    }    
    else if(guc_disp_mode == DISP_FACTORY) 
    {
        if(bflg_parm_dis_form == 0)
        {
            if(gsc_disp_parm_type == DISP_FACTORY_CL2)   //CL2~CL4����˸
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
                        if(gss_fac_CL3_flicker_cnt >= 150)            //�ӽ���˸3��
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
                        if(gss_fac_CL3_flicker_cnt >= 150)            //�ӽ���˸3��
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
                        if(gss_fac_CL3_flicker_cnt >= 150)            //�ӽ���˸3��
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
            if(bflg_disp1_flicker == 1)         //��˸
            {
                if(bflg_parm_save == 0)
                {
                    guc_disp_rate = not_dot;
                    led1_disp_val(gss_factory_parm_value[gsc_disp_parm_type]);
                }
                else                           //��ʱֵ
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
            if(bflg_disp1_flicker == 1)                       //����������ȷ��CL1��˸3��            
            {   
                LED1 = data_all_off;
                LED2 = data_C;
                LED3 = data_L;
                LED4 = data_1;
              
                gss_user_CL1_flicker_cnt++;
                if(gss_user_CL1_flicker_cnt >= 150)            //�ӽ���˸3��
                {
                    gss_user_CL1_flicker_cnt = 0;
                    bflg_buzz_one_tick_set = 1;   
                    bflg_IC_pass_CL1 = 0;
                    
                    guc_disp_mode = DISP_USER;                  //���ص��û���CL1
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
            if(gsc_IC_password_val/100 == 0)                 //Ϊ����ʾ 000
            {
                LED2 = data_0;
            }
            if(gsc_IC_password_val/10 == 0)                
            {
                LED3 = data_0;
            }
        }
       
        LED8 &= ~0x07;              //peak �� "�����¶��趨" "���±����趨" "���±����趨"
    }
    else if(guc_disp_mode == DISP_IC_REG) 
    {
        LED1 = data_line;
        LED2 = data_line;
        LED3 = guc_nmb_table[(guc_flash_add/4)/10];      //��ʾICע�����õĵ�ַ��
        LED4 = guc_nmb_table[(guc_flash_add/4)%10];    
    }
    else if(guc_disp_mode == DISP_TIME_SET)                             //�趨���Բ���ģʽ
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
    else if(guc_disp_mode == DISP_DEBUG_SELECT)       //���Բ���֮����л�ģʽ
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
        
        LED8 &= ~0x08;                      //�ص�"����"��
    }
    else if(guc_disp_mode == DISP_DEBUG)                             //�趨���Բ���ģʽ
    {
        if (gsc_disp_debug_mode == DISP_DEBUG_SEE)                   //�������ʾ��ѯģʽ
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
        else if (gsc_disp_debug_mode == DISP_DEBUG_PARA)               //�������ʾ����ģʽ
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
        
        LED8 &= ~0x08;                      //�ص�"����"��
    }


    //U��״̬��ָʾ��
    if(guc_U_disk_state == 0x00)       //�����Ѱγ�_��
    {
        LED4 &= (~data_dot); 
    }
    else if(guc_U_disk_state == 0x01)  //æ_��
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
    else if(guc_U_disk_state == 0x02)  //��д���_����
    {
        LED4 |= data_dot; 
    }
    /*else if(guc_U_disk_state == 0x03) //Ԥ��_��дʧ��_�Զ���������2������
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
    /*if((guc_usb_state & 0x04) == 0x04) //Ԥ��_usb�����_�Զ���usb�廵������3������
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
function: ��
          �м��LEDҪ��ʾ������
          
*******************************************************************************/
void led_disp2_deal(void)     //�����2��ʾ���������100ms��ʱ�����е���
{
    if (bflg_test_mode == 1)         //����ǲ���ģʽ
    {
        guc_disp_rate = not_dot;
        led2_disp_val(gss_TA / 10);
    }
    else
    {
        if(bflg_alarm_power_off == 1)           //�ϵ�
        {
            if(bflg_power_off_dis == 1)
            {
                guc_disp_rate = not_dot;
                led2_disp_val(gss_VAC);         //��ѹ
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
            led2_disp_val(gss_VAC);              //ѡ���ѹ��ʾ
            //led2_disp_val(guc_bat_Vout);
            //led2_disp_val(gss_adg_Temp[0]);
            //led2_disp_val(guc_RTC_r_minute);
            //led2_disp_val(gss_comp2_freq / 10);    //debug����ѹ��Ƶ��  
        }
    }      
}

//------------------------------------------------------------------------------
/*******************************************************************************
function: ��
-------------------��ʾ����-----------------------
 �ж������� ���£�35�棨 EEP�� ���� 30 �루 EEP����
 �����ʩ�� ��������˸��
 �ָ������� ���¡�34�棨 EEP�� ���� 30 �루 EEP����
		  ����ʱ�����ʾ���£�35�棬 ����ʾ 35�棩
---------------------------------------------------

 bflg_prot2_THW_high_disp ��protect��ѡ�� 0/1 (���ڵĻ���/�趨��)
 
*******************************************************************************/
void led_disp3_deal(void)        //�����3��ʾ���������100ms��ʱ�����е���
{    
    if (bflg_test_mode == 1)         //����ǲ���ģʽ
    {
        guc_disp_rate = not_dot;
        led3_disp_val(gss_VAC);
    }
    else
    {        
        if(bflg_alarm_power_off == 1)           //�ϵ�
        {
            if(bflg_power_off_dis == 1)
            {
                guc_disp_rate = not_dot;
                led3_disp_val(gss_THW);         //����
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
            led3_disp_val(gss_THW);                   //����
            //led3_disp_val(guc_RTC_r_second);
            //led3_disp_val(gss_comp21_freq / 10);    //debug��ѹ��ת��
        }
    }
}

//------------------------------------------------------------------------------
/*******************************************************************************
 ������������4λ�����:
 ��λ����ܵ�1~4 λ�ķֱ���ʾ:
*******************************************************************************/
void led1_disp_val(int16_t lss_disp_val)     //�����1��ʾ��ֵ����
{
    uint16_t lus_disp_abs_val;
    
    if (lss_disp_val < 0)
    {
        bflg_disp_minus = 1;      //��ʾ������־   
        lus_disp_abs_val = (~lss_disp_val) + 1;    // peak �������Ĳ���ת��Ϊ������Ҫ�Ĳ������ŵ�ԭ��
    }
    else
    {
        bflg_disp_minus = 0;
        lus_disp_abs_val = lss_disp_val;
    }
    //------------------------------------------------------ LED1
    if ((lus_disp_abs_val / 1000) != 0)                  //peak ȡ�̣�ȡ����λ��ֵ
    {
        LED1 = guc_nmb_table[lus_disp_abs_val / 1000];   //peak ��ֵ����ʾ
    }
    else
    {
        LED1 = data_all_off;                             //peak ûֵ�Ͳ���
    }
    //------------------------------------------------------ LED2   
    lus_disp_abs_val %= 1000;                            // peak ȡ�࣬ʣ�µ���ֵ
    if ((lus_disp_abs_val / 100) != 0)
    {
        LED2 = guc_nmb_table[lus_disp_abs_val / 100];
        //------------------------------
        if (guc_disp_rate == dot_0d01)                   //peak �ж�LED2��λС�����Ƿ�
        {
            LED2 |= data_dot;
        }
        //------------------------------
        if (bflg_disp_minus == 1)                        //peak �ж�ǰһλ�Ƿ��и���
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
        if (guc_disp_rate == dot_0d01)                  //peak �ж�Ϊ0ʱ�Ƿ�С����
        {
            LED2 = data_0 | data_dot;
        }
        else
        {
            if (LED1 == data_all_off)                   //peak ǰһλû��ֵ����λΪ0û���壬Ҳ�ص�
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
            LED3 |= data_dot;               //peak LED3��С����Ҳ��ʾ��
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
 �м���������Ϊ3λ�����
 
*******************************************************************************/
void led2_disp_val(int16_t lss_disp_val) //�����2��ʾ��ֵ����
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
 ��������������3λ�����  
*******************************************************************************/
void led3_disp_val(int16_t lss_disp_val) //�����3��ʾ��ֵ����
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
function: ��ʾ��δ����ʱ����һ�� ������ȡ�������ɲ�ѯ���ϴ�������:

����λ�ã� �� led_disp1_deal(void) ��,��10ms��ʱ����--------ok
******************************************************************************/
void view_error_code(void)    
{ 
     // �и��±��� �� û�б�����
    if ((bflg_alarm_ALH == 1) && (bflg_error_code_alarm_ALH_mark ==0))        //peak 1 ���±�����־ 
    {
        bflg_error_code_delaytime = 1;

        LED1 = data_all_off;
        LED2 = data_E_dp;
        LED3 = data_0;
        LED4 = data_0;
    } 	
    else if ((bflg_alarm_ALL == 1) && (bflg_error_code_alarm_ALL_mark ==0))  //peak 2 ���±�����־ 
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_1;
    }
    else if ((bflg_alarm_THW_high == 1) && (bflg_error_code_alarm_THW_high_mark ==0))    //peak 3 ���¹��߱�����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_2;
    }
    else if ((bflg_TA_fault == 1) && (bflg_error_code_alarm_TA_fault_mark ==0))     //peak 4 �����������ϱ�����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_0;
    }
    else if ((bflg_TE_fault == 1) && (bflg_error_code_alarm_TE_fault_mark ==0))     //peak 5 �Ƚ��������������� 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_1;
    }
    else if ((bflg_THW_fault == 1) && (bflg_error_code_alarm_THW_fault_mark ==0))     //peak 6 ���´��������ϱ��� 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_2;
    }
   else if ((bflg_TC_fault == 1) && (bflg_error_code_alarm_TC_fault_mark ==0))     //peak 7 �������������ϱ��� 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_3;
    }
    else if ((bflg_alarm_bat_discnnt == 1) && (bflg_error_code_alarm_bat_discnnt_mark ==0))  //peak 8 ���δ���ӱ�����־ 
    {
        bflg_error_code_delaytime = 1; 
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_0;
    }
    else if ((bflg_alarm_bat_fed == 1) && (bflg_error_code_alarm_bat_fed_mark ==0))     //peak 9 ��ؽӷ�������־ 
    { 
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_1;
    }
    else if ((bflg_alarm_bat_low == 1) && (bflg_error_code_alarm_bat_low_mark ==0))      //peak 10 ��ص����ͱ�����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_2;
		LED4 = data_2;
    }
    else if ((bflg_alarm_power_off == 1) && (bflg_error_code_alarm_power_off_mark ==0))    //peak 11 �ϵ籨����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_3;
		LED4 = data_0;
    }
    else if ((bflg_alarm_open_door == 1) && (bflg_error_code_alarm_open_door_mark ==0))    //peak 12 ���ű�����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_4;
		LED4 = data_0;
    }
    else if ((bflg_alarm_cond_dirty == 1) && (bflg_error_code_alarm_cond_dirty_mark ==0))   //peak 13 �������౨����־ 
    {
        bflg_error_code_delaytime = 1;
	
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_5;
		LED4 = data_0;
    }
    /*else if ((bflg_alarm_power_fault == 1) && (bflg_error_code_alarm_power_fault_mark ==0))  //peak  4 ��Դ����ϱ�����־ 
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_0;
		LED4 = data_4;
    }
    else if ((bflg_alarm_volt == 1) && (bflg_error_code_alarm_volt_mark ==0))   //peak 15 ��ѹ���걨����־ 15
    {
        bflg_error_code_delaytime = 1;
	 
    	LED1 = data_all_off;
		LED2 = data_E_dp;
		LED3 = data_1;
		LED4 = data_5;
    }*/
	else
	{
        flag_view_error_code = 0;   // ����ʾ�����¶���
	
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
�������� :

����λ��: �� 100ms �е���  ------ok
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

            if ((bflg_alarm_ALH == 1) && (bflg_error_code_alarm_ALH_mark ==0))		                //peak 1 ���±�����־ 1
            {
                bflg_error_code_alarm_ALH_mark = 1;                                                 // Ϊ1ʱ��ʾʱ�䵽��ѭ������һ����ʾ
            }
            else if ((bflg_alarm_ALL == 1) && (bflg_error_code_alarm_ALL_mark ==0))                 // 2 ���±���
            {
                bflg_error_code_alarm_ALL_mark = 1;
            }
            else if ((bflg_alarm_THW_high == 1) && (bflg_error_code_alarm_THW_high_mark ==0))       // 3 ���±���
            {
                bflg_error_code_alarm_THW_high_mark =1;
            }

            else if ((bflg_TA_fault == 1) && (bflg_error_code_alarm_TA_fault_mark ==0))              // 4 ������������
            {
                bflg_error_code_alarm_TA_fault_mark = 1;
            }
            else if ((bflg_TE_fault == 1) && (bflg_error_code_alarm_TE_fault_mark ==0))             // 5 �Ƚ���������������
            {
                bflg_error_code_alarm_TE_fault_mark = 1;
            }
            else if ((bflg_THW_fault == 1) && (bflg_error_code_alarm_THW_fault_mark ==0))           // 6 ���´���������
            {
                bflg_error_code_alarm_THW_fault_mark = 1;
            }
            else if ((bflg_TC_fault == 1) && (bflg_error_code_alarm_TC_fault_mark ==0))             // 7 ��������������
            {
                bflg_error_code_alarm_TC_fault_mark = 1;
            }  
            else if ((bflg_alarm_bat_discnnt == 1) && (bflg_error_code_alarm_bat_discnnt_mark ==0)) // 8 ���δ����
            {
                bflg_error_code_alarm_bat_discnnt_mark = 1;
            }

            else if ((bflg_alarm_bat_fed == 1) && (bflg_error_code_alarm_bat_fed_mark ==0))         // 9 ��ز巴
            {
                bflg_error_code_alarm_bat_fed_mark =1;
            }
            else if ((bflg_alarm_bat_low == 1) && (bflg_error_code_alarm_bat_low_mark ==0))         // 10 ��ص�����
            {
                bflg_error_code_alarm_bat_low_mark = 1;
            }

            else if ((bflg_alarm_power_off == 1) && (bflg_error_code_alarm_power_off_mark ==0))     // 11 ��Դ�ϵ�
            {
                bflg_error_code_alarm_power_off_mark = 1;
            }
            else if ((bflg_alarm_open_door == 1) && (bflg_error_code_alarm_open_door_mark ==0))     // 12 ����
            {
                bflg_error_code_alarm_open_door_mark = 1;
            }
            else if ((bflg_alarm_cond_dirty == 1) && (bflg_error_code_alarm_cond_dirty_mark ==0))   // 13 ��������
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
function: ��ʾ����led�Ƶ�ָʾ��
          1������ѹ�����еƵ�ָʾ
          2��8����������˸ָʾ
          3��������Զ�̱����Ƶ�ָʾ

����λ�ã�10ms-----------------------------------------------ok
*********************************************************************************************************************************/
void lamp_disp_deal(void)        //����ʾ���������100ms��ʱ�����е���
{
    //����ѹ�����е�ָʾ��
    if (bflg_comp21_running == 1)       // peak ����ѹ�������б�־
    {
        LED8 |= 0x20;
    }
    else
    {
        LED8 &= ~0x20; 
    }


	//��˸��8����
    //----------------------------------
    if (bflg_alarm_ALH == 1)             //peak  ���±�����־ 1 (ͳ�Ʊ���ʹ�õ�����)
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
    if (bflg_alarm_ALL == 1)             //peak ���±�����־ 2
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
    if (bflg_alarm_power_off == 1)       //peak �ϵ籨����־ 3
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
    if (bflg_alarm_open_door == 1)       //peak  ���ű�����־ 4 (����ע_��ѹ����)
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
    if (bflg_alarm_THW_high == 1)        //peak ���¹��߱�����־ 5
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
    if (bflg_alarm_cond_dirty == 1)      //peak  �������౨����־ 6
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
   // ���δ���� ����ص����͡���ز巴�����ֱ���������˸ͬһ����
    if ((bflg_alarm_bat_discnnt == 1) || (bflg_alarm_bat_low == 1) || (bflg_alarm_bat_fed == 1))  
    {                                     //peak  ���ع���  7 ��8 ��9
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
    //�������¡��������Ƚ�����4�ִ��������϶�����˸�����
    if (bflg_alarm_T_fault == 1)         //peak ���������ϱ�����־ 10��11��12��13 
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

    //Զ�̱�����
    //���¡����¡��ϵ硢���� ����һ�ֱ����Ϳ�Զ��
    if(bflg_alarm_ALH == 1 ||  bflg_alarm_ALL == 1 || bflg_alarm_power_off == 1 || bflg_alarm_open_door == 1)
    {
        RL_ALM_OUT_PIN = 0;    //1��  ������ôҪ���,����ʱ = 0��
    }
    else
    {
        RL_ALM_OUT_PIN = 1;    //0��
        //bflg_alarm_output = 0;֮ǰ�ı�־������δ��
    }
}

/*****************************************************************************************************************************
������������ʾ�����¶ȵ������ĸ�����

����λ��:  led_disp1_deal()-------------------------------------ok
*****************************************************************************************************************************/
void led_gss_TA_deal(void)
{
    guc_disp_rate = not_dot;
    
    if(gss_TA > 500)       //��������ʱ��ʾ����ֵ
    {
        led1_disp_val(50);  
    }
    else if(gss_TA < -990)
    {
        led1_disp_val(-99);  
    }
    else if(gss_TA > gss_set_parm_value[DISP_SET_TS])
    {
        led1_disp_val(gss_TA / 10 - 1);    //Ҫ����ô��ʾ,����С10������-1         
    }
    else
    {
        led1_disp_val(gss_TA / 10);            
    }
}

/*****************************************************************************************************************************
�����������ϵ�ʱ�������ʾ�Ͳ���ʾ����ʱ

����λ��:  1s-------------------------------------ok
*****************************************************************************************************************************/
void led_power_off_disp_delaytime(void)
{
    if(bflg_alarm_power_off == 1)   //�ϵ�
    {
        if(bflg_power_off_dis == 0) //��ʱ����30s����ʾ
        {
            gss_power_off_dis_delaytime++;
            if(gss_power_off_dis_delaytime >= 30)
            {
                gss_power_off_dis_delaytime = 0;
                bflg_power_off_dis = 1;
            }
        }
        else                       //��ʱ��ʾ5s�����
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
