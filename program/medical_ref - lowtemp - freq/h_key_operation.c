/************************************************************************************************************************************************
overview:
        1�� 
*************************************************************************************************************************************************/

#ifndef _KEY_OPERATION_C_
#define _KEY_OPERATION_C_
//------------------------------------------------------------------------------
//�����ļ�����
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
//��������
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
//��������
flag_type flg_disp,flg_disp1,flg_error_code_mark,flg_save1;


//----------------------------------------------------------
//�����궨��
uint8_t   guc_key_val;        //������ֵ

//----------------------------------------------------------
//��ʾ����궨��
uint8_t   guc_led_buffer[12]; //����ܼĴ���  //peak LDE1~LED12 ��Ӧ [0] ~ [11]
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
int8_t    gsc_disp_user1_mode;      //��ʾuser1ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
int8_t    gsc_disp_user2_mode;      //��ʾuser2ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
int8_t    gsc_disp_factory_mode;    //��ʾfactoryģʽ�µļ��ֲ�ͬ��ʾ��ʽ
int8_t    gsc_disp_debug_mode;      //��ʾ��������ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
uint8_t   guc_disp1_mode;
int8_t    gsc_disp1_set_mode;       //��ʾ�趨ģʽ
int8_t    gsc_disp1_see_mode;       //��ʾ��ѯģʽ

int8_t    gsc_disp1_para_mode;     //��ʾ����ģʽ

//------------------------------------------------------------------------------
int16_t   gss_key_long_delaytimer;         //��������ʱ��ʱ��
int16_t   gss_key_locked_delaytimer;       //����������ʱ��ʱ��
int16_t   gss_key_long_down_delaytimer;    //����״̬ʱ����down����ʱ
int16_t   gss_key_long_updown_delaytimer;  //����up+down����ʱ
int16_t   gss_key_long_setdown_delaytimer; //����set+down����ʱ
int16_t   gss_key_long_set_delaytimer;     //����set����ʱ

int8_t    gsc_password_val;             //����ֵ

int16_t   gss_disp_see_buffer[6];       //��ʾ��ѯ��Ϣ�Ĵ���
int16_t   gss_read_key_delaytimer;      //����ֵ��ʱ��ʱ��

int16_t    gss_user_CL1_flicker_cnt;   //ע��IC��ʱCL1��˸�Ĵ���
int16_t    gss_fac_AT_test_delaytime;  //����ATָ��ʱȫ����ʱ��
int16_t    gss_fac_CL3_flicker_cnt;    //��ʼIC����ʱCL3��˸�Ĵ���
int16_t    gss_fac_CL4_flicker_cnt;    //�ָ���������CL4��˸�Ĵ���

//peak add
int16_t   gss_key_long_buz_delaytimer;   //��������ʱ��ʱ��
int16_t   gss_led_loop_delaytime;        //LED����˸Ƶ���趨
int16_t   guc_IC_register_delaytime;     //�жϳ�����ϼ��ĵ�ʱ���Ƿ�ﵽ����ע��״̬
int16_t   gsc_IC_password_val;           //ICע��ģʽ������ֵ
int8_t    guc_IC_erasure_delaytime;      //ICע��ʱ���ж�
int8_t    gss_disp1_flicker_delaytime;   //"�����¶��趨" "���±����趨" "���±����趨" ��˸��ʱ
int8_t    guc_IC_password_cnt;           //����ICע��ģʽʱ������������������
int8_t    guc_password_cnt;              //���빤��ģʽʱ������������������

uint8_t   gss_self_test_delaytime;       //ϵͳ�Լ�ʱ����˸��ʱ
uint8_t   gss_self_test_cnt;             //ϵͳ�Լ�ʱ����˸��������

uint8_t  guc_lock_delaytimer;            // IC�������Ĵ򿪵�ʱ��


int8_t  guc_user_write_time_value;      //����RTCʱ�����ʱ����
uint8_t gss_usb_set_time_delaytime;     //usb����ʱ����ʱ
int8_t  gsc_disp_unlocked_password;     //��������
int16_t  gss_key_password_long_valid_delaytime; //��������ʱ������ʱ����ɲ���10����

//�����¹��������ӵĲ���
int16_t gss_TS_old_value;               //�趨ǰ�ľ�ֵ
int16_t gss_ALH_old_value;
int16_t gss_ALL_old_value;


uint8_t   guc_disp_mode;                                  //�ܵ�UIģʽ
int8_t    gsc_disp_parm_type;                             //��ͬ�Ĳ�������,���еĲ����������
int16_t   gss_key_temp_value;                             //��������ʱֵ,���еĲ����������

int16_t   gss_set_parm_value[DISP_SET_PARM_SUM];          //�趨����ֵ   
int16_t   gss_user_parm_value[DISP_USER_PARM_SUM];        //�û�����ֵ   
int16_t   gss_factory_parm_value[DISP_FACTORY_PARM_SUM];  //��������ֵ



/**************************************************************************************************************************************
�������ܣ�ģʽѡ��
          
          
����λ��: ��ѭ��--------------------------ok
**************************************************************************************************************************************/
void  key_mode_select_deal(void)   //key_mode_select_deal       
{    
    if (bflg_test_type == 1)                     //����ǲ��԰���ʾ��
    {
        if (guc_key_val == KEY_SET)              //��������ü�
        {
            bflg_test_mode = 1;                  //�ò���ģʽ��־
        }
        else if(guc_key_val == KEY_BUZ)
        {
        	bflg_test_mode_RTC = 1;
        	test_mode_RTC_circuit();
        }
    }
    else if (guc_disp_mode == DISP_LOCKED)          //����ģʽ
    {
        key_locked_deal();                    
    }    
    else if(guc_disp_mode == DISP_UNLOCKED_PASSWORD)//��������
    {
        key_locked_password_deal();
    }
    else if(guc_disp_mode == DISP_UNLOCKED)         //����ģʽ
    {
        key_unlocked_deal();  
    }
    else if(guc_disp_mode == DISP_SET)             //��������ģʽ
    {
        key_set_parm_deal();
    }
    else if(guc_disp_mode == DISP_USER)            //�û�ģʽ
    {
        key_user_parm_deal();
    }
    else if (guc_disp_mode == DISP_FAC_PASSWORD)   //��������ģʽ
    {
        key_fac_password_deal();                  
    }
    else if(guc_disp_mode == DISP_FACTORY)         //����ģʽ
    {
        key_factory_parm_deal();
    }     
    else if (guc_disp_mode == DISP_IC_PASSWORD)    //IC��ע������ģʽ
    {        
        key_IC_password_deal();  
    }
    else if(guc_disp_mode == DISP_IC_REG)          //IC ע��ģʽ
    {
        key_IC_register_deal();                
    }
    else if(guc_disp_mode == DISP_TIME_SET)         //RTCʱ������ģʽ
    {
        key_RTC_time_parm_deal();
    }
    else if(guc_disp_mode == DISP_DEBUG_SELECT)    //ѡ�����ģʽ
    {
        key_debug_select_deal();
    }
    else if(guc_disp_mode == DISP_DEBUG)            //����ģʽ
    {
        key_debug_parm_deal();                      //����set5s-->���뺣��ʹ�õĵ��Բ���ģʽ������ʽ�汾ʱȥ��
    }
}

/***********************************************************************************************************************************
function: ����ģʽ�¿ɽ��еĲ���
          1�������¼������������ģʽ
          2������set��������Բ���ģʽ
          
����λ��: �� read_key_deal()�У�������ѭ���� ----ok          
***********************************************************************************************************************************/
void key_locked_deal(void)             
{
    //-------------------------------------------------------------
    if ((guc_key_val == KEY_UP)||(guc_key_val == KEY_DOWN)) //�ϻ��¼�������������        
    {
        guc_disp_mode = DISP_UNLOCKED_PASSWORD; 
        gsc_disp_unlocked_password = 0;
        key_clear_password_long_falg();         //��������ʱ��������10
    }
    
    //----------------------------------------------------------------
    if (guc_key_val == KEY_SET)                //����set������Բ���      
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
            guc_disp_mode = DISP_DEBUG_SELECT;  //���뺣����������ģʽ����ʽ�汾ʱȥ��
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
�������ܣ�����������ģʽ,������ȷ�Ľ�������󳤰�set��5s�������ģʽ

����λ�ã�read_key_deal()��-------------------ok
********************************************************************************************************************************************************/
void key_locked_password_deal(void)
{  
    if (guc_key_val == KEY_BUZ)
    {
        guc_disp_mode = DISP_LOCKED;   
    }
    else if (guc_key_val == KEY_UP)  //����ѡ��
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
            else                                     //�������set 5s
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
 function: ����ģʽ
           1����һ��set������set�� 
           2������ up    5s������user��
           3������ dowe  5s������IC��
           4������ up + down  5s,���빤��������

����λ��:    read_key_deal()��-------------------------ok         
******************************************************************************/
void key_unlocked_deal(void)
{   
    if(bflg_unlocked_deal == 1)        
    {
        if (guc_key_val != KEY_SET)     //�ɿ���set�����ǰ�����������ϼ����ſɼ�������
        {
            bflg_unlocked_deal = 0;
        }   
    }
    else
    {
        if (guc_key_val == KEY_SET)                  //�������ò�����
        {          
            guc_disp_mode = DISP_SET;   
            gsc_disp_parm_type = 0;
            gss_key_temp_value = 0;
            bflg_parm_dis_form = 0;
            bflg_parm_save = 0;
        }
        
        //------------------------------------------------------------------
        if (guc_key_val == KEY_UP)                   //����up 5s����user��
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
                
                guc_disp_mode = DISP_USER;          //�����û�ģʽ
                gsc_disp_parm_type = 0;
                gss_key_temp_value = 0;
                bflg_parm_dis_form = 0;
                bflg_parm_save = 0;
                gss_user_parm_value[DISP_USER_T2] = 0; //����P1��ʼ
                
                key_clear_password_long_falg();        //Ϊ��IC��PS1������������10��
            }
        }    
        else
        {
            bflg_key_long_delaytime = 0;
            gss_key_long_delaytimer = 0;
            bflg_key_long_valid = 0;
        }
        
        //-----------------------------------------------------------------
        if (guc_key_val == KEY_UP_AND_DOWN)        //����"up+down"5s���빤������passward��  
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
                
                gsc_password_val = 0;                //��������Ѿ���0
                guc_password_cnt = 0;
                key_clear_password_long_falg();      //Ϊ��PS2������������10��
            }
        }
        else
        {
            bflg_key_long_updown_delaytime = 0;     
            gss_key_long_updown_delaytimer = 0;
            bflg_key_long_updown_valid = 0; 
        }
        
        //-----------------------------------------------------------------
        if(guc_key_val == KEY_DOWN)            //���� down 5s����IC����ע���  
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
                
                bflg_IC_register_state = 1;         //��ʾ����ע��״̬
                
                guc_disp_mode = DISP_IC_PASSWORD;   //����ICע������״̬
                gsc_IC_password_val = 0;                              
                guc_IC_password_cnt = 0;

                key_clear_password_long_falg();    //��������ʱ��������10
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
��������: ��������ģʽ

����λ�ã���ѭ��-----------------------------------------ok
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
            if (gsc_password_val == gss_factory_parm_value[DISP_FACTORY_PS2]) //��Ϊ����ɵ� 
            {
                key_clear_password_long_falg();                    //PS2����Ҳ����10
                
                guc_disp_mode = DISP_FACTORY;                      //����ģʽ
                gsc_disp_parm_type = 0;
                gss_key_temp_value = 0;
                bflg_parm_dis_form = 0;
                bflg_parm_save = 0;
                
                gsc_password_val = 0;                              //���н���ʱ����˴��ٴ���
                guc_password_cnt = 0;                              //��������ȷ����������
            }
            else if(guc_password_cnt >= 3)                         //����3�δ������������
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
��������:    ���ò������ TS��ALH��ALL ��������������


����λ��:   read_key_deal()��---------------------------ok
*******************************************************************************************************************************/
void key_set_parm_deal(void)
{
    int16_t lss_disp1_TS_temp;
    lss_disp1_TS_temp = gss_set_parm_value[DISP_SET_TS];  
    //-----------------------------------------------------------------
    if (guc_key_val == KEY_BUZ)                    
    {
        guc_disp_mode = DISP_UNLOCKED;              //������һ��
        gsc_disp_parm_type = 0;
        gss_key_temp_value = 0;
        bflg_parm_dis_form = 0;
        bflg_parm_save = 0;
    }
    //-----------------------------------------------------------------
    else if(guc_key_val == KEY_SET)               //�л�TS ALH ALL��������
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
                    eeprom_write(num_comp2_TS, ram_para[num_comp2_TS]);  //������Ҫ����set���ű���

                    bflg_sent_usb_TS = 1;      // 6
                }

                if(gss_set_parm_value[DISP_SET_TS] != lss_disp1_TS_temp)                    //����趨ֵ�ı���
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
            if (gss_key_temp_value > (gss_set_parm_value[DISP_SET_TS]-50))        //�Ŵ�10��
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
            if (gss_key_temp_value < -990)                      //����ֻ��3λ�����
            {
                gss_key_temp_value = gss_set_parm_value[DISP_SET_TS]-50;
            }
        }
    }
}


/********************************************************************************************************************************
��������: �û�������Ĳ���        

guc_disp_mode                     
gsc_disp_parm_type             
gss_key_temp_value
bflg_parm_dis_form
bflg_parm_save

����λ��:   read_key_deal()��---------------------------ok
*********************************************************************************************************************************/
void key_user_parm_deal(void)
{
    if (guc_key_val == KEY_BUZ)                     
    {
        if(bflg_parm_dis_form == 0)            //��ʾ��������
        {
            guc_disp_mode = DISP_UNLOCKED;         
            gsc_disp_parm_type = 0;            //�ĸ�����
            gss_key_temp_value = 0;            //��ʱδ�����ֵ
            bflg_parm_dis_form = 0;            //��ʾ��ʾ����ֵ
            bflg_parm_save = 0;                //�Ƿ��豣��
        }
        else                                        //�����ʱ��ʾ���ǲ�����ֵ�������ڴ˲㣬���ص��˲�Ĳ�������                              
        {    
            gss_key_temp_value = 0;             
            bflg_parm_dis_form = 0;                
            bflg_parm_save = 0;
        }
    }
    else
    {
        if (bflg_parm_dis_form == 0)         //��ʾ������ʾ   
        {
            if (guc_key_val == KEY_SET)         
            {
                if(gsc_disp_parm_type == DISP_USER_CL1)   //special ����ICע��
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
        else   //�������ʾ�趨ֵ                            
        {
            if (guc_key_val == KEY_SET) //��������ü�   
            {
                key_clear_password_long_falg();
                
                if(gsc_disp_parm_type == DISP_USER_T2)                     //special
                {
                    bflg_parm_save = 0;
                    guc_disp_mode = DISP_TIME_SET;                         //����ʱ�����ò�
                    
                    if(gss_user_parm_value[DISP_USER_T2] == 0)                   //P1 ��      //��ȡ��ʱ��ʱ��ֵ
                    {
                        guc_user_write_time_value = guc_RTC_r_year;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 1)              //P2 ��
                    {
                        guc_user_write_time_value = guc_RTC_r_month;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 2)              //P3 ��
                    {
                        guc_user_write_time_value = guc_RTC_r_day;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 3)              //P4 ʱ
                    {
                        guc_user_write_time_value = guc_RTC_r_hour;
                    }
                    else if(gss_user_parm_value[DISP_USER_T2] == 4)              //P5 ��
                    {
                        guc_user_write_time_value = guc_RTC_r_minute;
                    }
                }
                else
                {
                    bflg_parm_dis_form = 0;      //��ʾ��ʾ
                    
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
                                //������
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
                        if(gss_user_parm_value[DISP_USER_T2] > 4)  //P1-P5��ʾʱ+1
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
                        //�����
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
                        //�����
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
��������: ����������Ĳ���        
             
����λ��:   read_key_deal()��---------------------------ok
********************************************************************************************************************************/
void key_factory_parm_deal(void)
{
    if (guc_key_val == KEY_BUZ)            
    {  
        if(bflg_parm_dis_form == 0)             
        {
            guc_disp_mode = DISP_UNLOCKED;         
            gsc_disp_parm_type = 0;            //�ĸ�����
            gss_key_temp_value = 0;            //��ʱδ�����ֵ
            bflg_parm_dis_form = 0;            //��ʾ0��ʾ 1ֵ
            bflg_parm_save = 0;                //�Ƿ��豣��
        }
        else                          
        {
            //gsc_disp_parm_type = 0;          //��ʾ�Ĳ�����־����
            gss_key_temp_value = 0;           
            bflg_parm_dis_form = 0;            
            bflg_parm_save = 0;                
        }        
    }
    else
    {
        if(bflg_parm_dis_form == 0)    //��ʾ
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
        else                           //bflg_parm_dis_form = 1 �Բ���ֵ 
        {
            if(guc_key_val == KEY_SET)
            {
                key_clear_password_long_falg();
                bflg_parm_dis_form = 0;
                
                if(bflg_parm_save == 1)
                {
                    bflg_parm_save = 0;

                    if((gsc_disp_parm_type == DISP_FACTORY_CL2) || //CL2~CL4��˸
                       (gsc_disp_parm_type == DISP_FACTORY_CL3) ||
                       (gsc_disp_parm_type == DISP_FACTORY_CL4))
                    {
                        gss_factory_parm_value[gsc_disp_parm_type] = gss_key_temp_value;

                        if(gss_factory_parm_value[gsc_disp_parm_type] == 1)
                        {
                            if(gsc_disp_parm_type == DISP_FACTORY_CL2)   
                            {
                                //gss_factory_parm_value[DISP_FACTORY_CL2] = 0;//��˸��ʾ����0
                                bflg_sent_usb_CL2 = 1;                  //�÷�����usbָ��
                                
                                guc_buzz_sound_cnt = 1;                 //����1�� 
                            }
                            else if(gsc_disp_parm_type == DISP_FACTORY_CL3)
                            {
                                bflg_IC_erasure_order = 1;                            //֪ͨ��ʾ��ע������IC���ı�־
                                
                                gss_user_parm_value[DISP_USER_IC] = EEPROM_init[num_IC_password][0];       //�����ʼ��      
                                ram_para[num_IC_password] = gss_user_parm_value[DISP_USER_IC];                                                                    //!!!!!!!
                                eeprom_write(num_IC_password, ram_para[num_IC_password]);                                    //�����ʼ��ͬʱע������IC�� ��+  
                                guc_buzz_sound_cnt = 1;                                        
                            }
                            else if(gsc_disp_parm_type == DISP_FACTORY_CL4)
                            {
                                bflg_factory_CL4 = 1;               //�ָ������������б�־
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
                                guc_buzz_sound_cnt = 6;             //����6�� ,����6s���ض�������
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
                                bflg_fac_bS_buz_valid = 1;        //���δ���ӱ�������             
                                guc_buzz_sound_cnt = 1;           //��һ��  
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
                             gss_key_temp_value += 10;   //��������+10
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
                             gss_key_temp_value -= 10;   //��������-10
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
��������: IC������ģʽ
          
����λ��:��  read_key_deal();     -----------------------OK    
********************************************************************************************************************/
void key_IC_password_deal(void)  //�������봦�����
{
    if (guc_key_val == KEY_BUZ)
    {
        if(gsc_disp_parm_type == DISP_USER_CL1)     //��user��CL1������
        {           
            guc_disp_mode = DISP_USER;   
            gss_key_temp_value = 0;
            bflg_parm_dis_form = 0;
            bflg_parm_save = 0;
        }
        else
        {
            guc_disp_mode = DISP_UNLOCKED;   
            
            bflg_IC_register_state = 0;              //��� ����֤
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
            if (gsc_IC_password_val == gss_user_parm_value[DISP_USER_IC])      // ����������Ƿ�����õ�һ��
            {        
                guc_IC_password_cnt = 0;
                
                if(gsc_disp_parm_type == DISP_USER_CL1) //�����user2��� ICע������
                {
                    bflg_IC_erasure_order = 1;            //ע������IC��
                    
                    bflg_IC_pass_CL1 = 1;                 //CL1��˸��ʾ�ı�־

                    bflg_sent_usb_IC = 1;                 //13
                }
                else
                {              
                    guc_disp_mode  = DISP_IC_REG;
                }
            }
            else if(guc_IC_password_cnt >= 3)
            {
                bflg_IC_register_state = 0;           //��ɽ���IC����״̬
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
        key_clear_password_long_falg();         //��������ʱ��������10
    }
}

/**************************************************************************************************************************
��������:  ICע��ʱ����������ֻ��buzz��������

����λ��:  key_mode_select_deal----------------------------------OK
**************************************************************************************************************************/
void key_IC_register_deal(void)
{
    if(guc_key_val == KEY_BUZ)                // ����SET�����˳������� 
    {
       bflg_IC_register_state = 0;            // �˳� ICע��״̬
       guc_temp_flash_add = 0;
       guc_flash_add = 0;
       
       guc_disp_mode = DISP_UNLOCKED;         //�ص��ϲ����ʾ״̬
    }
}

/*****************************************************************************
��������: �ڽ���debug��ʱ���е�һЩ����        
����������1�� ѡ����һ������para����see
             
����λ��:   read_key_deal()��---------------------------ok
******************************************************************************/
void key_debug_select_deal()
{
    if (guc_key_val == KEY_BUZ)  
    {
        guc_disp_mode = DISP_LOCKED;               //�ص��ϲ�����
        
        gsc_disp_debug_mode = DISP_DEBUG_PARA;     //�ٴν���˲�ʱ��ʾĬ��para
    }
    else if (guc_key_val == KEY_SET)
    {  
        guc_disp_mode = DISP_DEBUG;               //����debugģʽ
        gsc_disp1_para_mode = 0;
        gsc_disp1_see_mode = 0;
    }
    else if (guc_key_val == KEY_UP)               //��up��down ����ѡ��para��seeģʽ
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
��������: �ڽ���debug��ʱ���е�һЩ����        
����������1����������ʹ�õļ������������޸�--para
          2���������ԵĲ�����ֻ�ɿ�--see
          
����λ��:   read_key_deal()��---------------------------ok
******************************************************************************/
void key_debug_parm_deal(void)
{   
    if(gsc_disp_debug_mode == DISP_DEBUG_PARA)     //���Բ���
    {
        if (guc_key_val == KEY_BUZ)
        {
            guc_disp_mode = DISP_DEBUG_SELECT;           //�ص��ϲ�
            gsc_disp1_para_mode = COMP2_START_FREQ;      //�ٴλص��˲�ʱ��ʾp00;
            bflg_disp1_val = 0;
        }
        else
        {
            if (bflg_disp1_val == 0)          //�������ʾ�趨ֵ
            {
                if (guc_key_val == KEY_SET)    
                {
                    bflg_disp1_val = 1;       //��ʾ�趨ֵ
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
            else                                 //�������ʾ�趨ֵ
            {
                if (guc_key_val == KEY_SET)       
                {
                    bflg_disp1_val = 0;          //����ʾ�趨ֵ
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
    else if(gsc_disp_debug_mode == DISP_DEBUG_SEE)    //�鿴����
    {
        if (guc_key_val == KEY_BUZ)
        {
            guc_disp_mode = DISP_DEBUG_SELECT;       //�ص���һ��
            gsc_disp1_see_mode = DISP1_SEE_r00;      //�ٴλص��˲�ʱ��ʾr00;
            bflg_disp1_val = 0;
        }
        else
        {
            if (bflg_disp1_val == 0)           //�������ʾ�趨ֵ
            {
                if (guc_key_val == KEY_SET)    //��������ü�
                {
                    bflg_disp1_val = 1;        //��ʾ�趨ֵ
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
            else   //�������ʾ��ѯֵ
            {
                if (guc_key_val == KEY_SET) //��������ü�
                {
                    bflg_disp1_val = 0;     //����ʾ�趨ֵ
                }
            }
        }
    }
}

/******************************************************************************************************************************
��������:  ���û�����������RTCʱ��ʱ��Ĳ���:P1-P5

����λ��: read_key_deal() ����ѭ��------------------------ok
******************************************************************************************************************************/
void key_RTC_time_parm_deal(void)  
{
    if (guc_key_val == KEY_BUZ)    //��USER�е�T2����������
    {
        guc_disp_mode = DISP_USER;        
        gsc_disp_parm_type = DISP_USER_T2;
        gss_key_temp_value = 0;
        bflg_parm_dis_form = 0;
        bflg_parm_save = 0;
        //gss_user_parm_value[DISP_USER_T2]= 0;//��Ҫ��P1��ʼ
    }
    else if (guc_key_val == KEY_UP)             
    {
        guc_user_write_time_value++;
        
        if(gss_user_parm_value[DISP_USER_T2] == 0)           //P1_��
        {
            if(guc_user_write_time_value > 99)
            {
                guc_user_write_time_value = 10;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P2_��
        {
            if(guc_user_write_time_value > 12)
            {
                guc_user_write_time_value = 1;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P3_��
        {
            if(guc_user_write_time_value > 31)
            {
                guc_user_write_time_value = 1;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P4_ʱ
        {
            if(guc_user_write_time_value > 23)
            {
                guc_user_write_time_value = 0;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P5_��
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
        if(gss_user_parm_value[DISP_USER_T2] == 0)           //P1_��
        {
            if(guc_user_write_time_value < 10)
            {
                guc_user_write_time_value = 99;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P2_��
        {
            if(guc_user_write_time_value < 1)
            {
                guc_user_write_time_value = 12;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P3_��
        {
            if(guc_user_write_time_value < 1)
            {
                guc_user_write_time_value = 31;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P4_ʱ
        {
            if(guc_user_write_time_value < 0)
            {
                guc_user_write_time_value = 23;
            }
        }
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P5_��
        {
            if(guc_user_write_time_value < 0)
            {
                guc_user_write_time_value = 59;
            }
        }
    }
    else if(guc_key_val == KEY_SET)  //ȷ���޸�ʱ��
    {
        guc_disp_mode = DISP_USER;
        gsc_disp_parm_type = DISP_USER_T2;
        //gss_key_temp_value = 0;
        bflg_parm_dis_form = 1;
        bflg_parm_save = 0;
            
        if(gss_user_parm_value[DISP_USER_T2] == 0)      //P1��
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
        else if(gss_user_parm_value[DISP_USER_T2] == 1)      //P1��
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
        else if(gss_user_parm_value[DISP_USER_T2] == 2)      //P1��
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
        else if(gss_user_parm_value[DISP_USER_T2] == 3)      //P1ʱ
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
        else if(gss_user_parm_value[DISP_USER_T2] == 4)      //P1��
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
��������:  �޸�RTCʱ�Ӻ�����ʱ2s�ٷ����޸�usbʱ�䣬��ֹ���廹δ��ȡ���޸ĺ��ʱ��

����λ��:  1s��ʱ��---------------------------------------------------ok
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
�������� :   ����20sû�ж�����������״̬

����λ�ã�100ms---------------------------------ok           
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
            
            bflg_key_locked = 1;        //�ð���������־  peak δ��
            bflg_IC_register_state = 0; //�˳�ICע��״̬
            guc_temp_flash_add = 0;
            guc_flash_add = 0;
        }
    }
    else
    {
        gss_key_locked_delaytimer = 0;
    }

    //------------------------------------------------------------------------------
    if(bflg_IC_register_state == 1)      //��ע��״̬ʱ���п�ע��������ʱ����0��
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
function: ����״̬���� "�趨" �� 5s ,��ʾ��������Ч���ɽ�������ģʽ

����λ��: 100ms��ʱ��----------------ok
*******************************************************************************/
void key_long_delaytime(void) //����������ʱ������100ms��ʱ�����е���
{
    if (bflg_key_long_delaytime == 1)
    {
        gss_key_long_delaytimer++;
        if (gss_key_long_delaytimer >= ram_para2[num_key_long_delaytime])  //50
        {
            gss_key_long_delaytimer = 0;
            bflg_key_long_delaytime = 0;
            
            bflg_key_long_valid = 1;    //�ó�������Ч��־
        }
    }
}

/*******************************************************************************
function: ����״̬���� down �� 5s ,��ʾ��������Ч���ɽ��뺣������ʱʹ�õĲ���
          ģʽ������ʽ�汾ʱ����ȥ��

����λ��:  100ms ��ʱ�� ------------ok       
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

            bflg_key_long_down_valid = 1;    //�ó���down����Ч��־
        }
    }
}
//����ģʽ����set
void key_long_set_delaytime(void)
{
    if (bflg_key_long_set_delaytime == 1)
    {
        gss_key_long_set_delaytimer++;
        if (gss_key_long_set_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_set_delaytimer = 0;
            bflg_key_long_set_delaytime = 0;

            bflg_key_long_set_valid = 1;    //�ó���set����Ч��־
        }
    }
}
//����up+down ��ϼ���ʱ   
void key_long_updown_delaytime(void)
{
    if (bflg_key_long_updown_delaytime == 1)
    {
        gss_key_long_updown_delaytimer++;
        if (gss_key_long_updown_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_updown_delaytimer = 0;
            bflg_key_long_updown_delaytime = 0;

            bflg_key_long_updown_valid = 1;    //�ó���down����Ч��־
        }
    }
}

//����set+down��ϼ���ʱ --��������user1����down 5s
void key_long_setdown_delaytime(void)
{
    if (bflg_key_long_setdown_delaytime == 1)
    {
        gss_key_long_setdown_delaytimer++;
        if (gss_key_long_setdown_delaytimer >= ram_para2[num_key_long_delaytime])  //50 *100ms=5s
        {
            gss_key_long_setdown_delaytimer = 0;
            bflg_key_long_setdown_delaytime = 0;

            bflg_key_long_setdown_valid = 1;    //�ó���down����Ч��־
        }
    }
}



//------------------------------------------------------------------------------
/**************************************************************************************************************************************
��������: ���������°���ʱ��ÿ����೤ʱ��԰�����ȡ

����λ�ã�10ms----------------------------------ok
***************************************************************************************************************************************/
void read_key_delaytime(void) //������ʱ������10ms��ʱ�����е���
{    
    if (bflg_read_key_delaytime == 1)
    {
        gss_read_key_delaytimer++;
        if (gss_read_key_delaytimer >= 100)  // 10 * 200 = 2000ms = 2s  ԭ�� 
        {                                    // 10 *100 = 1000ms = 1s   ����1s������
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
��������: �б���ʱ���� buzz ����30min�ڲ��ٱ���


����λ��: ��ѭ�� ------------------------------------------ok
*********************************************************************************************************/
void  key_buzz_cancel(void)
{
    if (guc_key_val == KEY_BUZ)  
    {
        if (bflg_buzz_loop_tick_set == 1)         // �����ʱ�з��������������ǰ�����"����ȡ��"�����������ʱ30 min �����죬
        {                                         // 30 min ��ָ��������ƣ�
            if (bflg_alarm_buzz_off_delaytime == 0)//��������Ҫ����30min��������ټ�ʱ
            {
                bflg_alarm_buzz_off_delaytime = 1;
                gss_alarm_buzz_off_delaytimer = 0;
            }       
        }
    }
}

/*******************************************************************************************************************************           
�������ܣ�     ���δ���ӣ�����5s������һ�μ�⵽������ʱ���ڲ��죬
          ���ٴμ�⵽��أ��ָ��������ƣ�
          
����λ��: ��ѭ��--------------------------------------ok          
*******************************************************************************************************************************/
void bat_con_check_again(void)    
{
	if (bflg_fac_bS_buz_valid == 1)        //���õ��δ�������β���bS=1
	{
		if (bflg_alarm_bat_discnnt == 0)   //����ּ�⵽���
		{    
			 bflg_fac_bS_buz_valid = 0;    //�ָ���������
			 gss_factory_parm_value[DISP_FACTORY_bS] = 0;
		}
	}
}


/**********************************************************************************************************************************************
��������: ָʾ����˸ʱ���־���趨,��һֱ������ѭ��

          У��: >=1  ----->  >1

����λ�ã�100ms------------------------------------ok
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
��������: ���ò���ʱ��������˸Ƶ��

����λ��:    100ms ��ʱ����--------------------------------ok
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
��������:  ����ģʽ��ATָ��Ĳ���
           
����λ��:  10ms��ʱ��---------------------------------------ok
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
    
    RL_ALM_OUT_PIN = 1;                       //Զ�̱����ƿ�
    bflg_alarm_lamp = 1;                      //3λ������
    
    gss_fac_AT_test_delaytime++;
    if(gss_fac_AT_test_delaytime >= 600)  //10ms * 600 = 6s ��ָ�����
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
��������:��������  CL4�ָ�����ֵ����
           
����λ��: ��ѭ��---------------------------------------ok
******************************************************************************************************************************/
void key_factory_CL4_init(void)
{
    if(bflg_factory_CL4 == 1)
    {
        bflg_factory_CL4 = 0;
        
        init_ram_para2();             //��дE2
        init_ram_para();
        
        eeprom2_read_deal();          //eeprom2��ȡ�������
        eeprom_read_deal();           //eeprom��ȡ�������
       
        operation_parm_read();               //�Ӷ�ȡ��E2�������л�ȡ��Ҫ��ֵ   
        init_freq_ctrl();
    }
}

/******************************************************************************************************************************
��������:  �����ϵ��Լ죬���е�����3s,��1��
           
����λ��:  10ms��ʱ��----------------------------------ok
******************************************************************************************************************************/
void first_power_on_self_test(void) //first_power_on
{
    if(bflg_self_test_sound == 0)
    {
        bflg_self_test_sound = 1;
        guc_buzz_sound_cnt = 1;     //��1��
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
    {   //ȫ��
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
    {   //ȫ��
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
��������: ��IC��ע��״̬ʱ�����п����Ķ���

����λ��: ��������   ---------------------------ok
*********************************************************************************************************************/
void IC_unlock_deal(void)
{
    if(guc_disp_mode != DISP_IC_REG)   //��IC��ע��״̬ʱ�����п����Ķ���
    {
        if(guc_IC_access_state == 1)
        {
            LOCK_OUT_PIN = 1;          //����
            guc_IC_access_state = 0;

            bflg_lock_delaytimer = 1;  //��������ʱ��־
            guc_lock_delaytimer = 0;
        }
    }
}

/********************************************************************************************************************
��������: IC��������ʱ

����λ��: 1s ��ʱ -----ok
*********************************************************************************************************************/
void IC_unlock_delaytime(void)  
{
    if(bflg_lock_delaytimer == 1)
    {
        guc_lock_delaytimer++;
        if(guc_lock_delaytimer >= 10)    // ����10s
        {
            bflg_lock_delaytimer = 0;
            guc_lock_delaytimer = 0;

            LOCK_OUT_PIN = 0;
        }
    }
}

/********************************************************************************************************************
��������: ICע��(IC)����������(PS1)����������(PS2) ������ʱ,Ȼ�󰴲���10����

          ÿ�����ǲ�ʱ��Ҫ��0��

����λ��: 1s ��ʱ -----------------------------------ok
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
��������: �����볤��ʱ�ı�־:1.��������ʱ 2.�޸�����ʱ ������ͬ������


����λ�ã����ļ��еĸ���������------------------------------------------ok
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
