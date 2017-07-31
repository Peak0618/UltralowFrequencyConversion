/**************************************************************************************************************************************************
overview:
    1����ɫ�ƵĿ������
    2���������
    3��ƽ��ڼ���˿
    4���������˿
    5����ڼ���˿
    
    6��ëϸ�ܼ���˿---δ��
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
//��������
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
//���
int16_t   gss_outfan2_normal_H_delaytimer;    //��������������ٿ���ʱ��ʱ��
int16_t   gss_outfan2_normal_L_delaytimer;    //��������������ٿ���ʱ��ʱ��
int16_t   gss_outfan2_fault_H_delaytimer;     //�������������ٿ���ʱ��ʱ��
int16_t   gss_outfan2_fault_L_delaytimer;     //�������������ٿ���ʱ��ʱ��

int16_t   gss_heat21_run_delaytimer;          //ƽ��ڼ���˿����ʱ��ʱ��
int16_t   gss_heat21_stop_delaytimer;         //ƽ��ڼ���˿����ʱ��ʱ��
int16_t   gss_heat22_run_delaytimer;          //�������˿����ʱ��ʱ��
int16_t   gss_tank_mouth_run_delaytimer;      //��ڼ���˿����ʱ��ʱ��
uint8_t   guc_RL_output_delaytimer;           //�̵��������ʱ��ʱ��

uint8_t   guc_door1_pin_high_delaytimer;      //�ſ���1���Ÿ���ʱ��ʱ��
uint8_t   guc_door1_pin_low_delaytimer;       //�ſ���1���ŵ���ʱ��ʱ��
uint8_t   guc_bat_fed_pin_high_delaytimer;    //��ط��巴���Ÿ���ʱ��ʱ��
uint8_t   guc_bat_fed_pin_low_delaytimer;     //��ط��巴���ŵ���ʱ��ʱ��

//����˿������ر���
int16_t   gss_heat20_space_delaytimer;       //ëϸ�ܼ���˿�����ʱ��ʱ��
int16_t   gss_heat20_wait_delaytimer;        //ëϸ�ܼ���˿�ȴ���ʱ��ʱ��
int16_t   gss_heat20_comp2_stop_delaytimer;  //ëϸ�ܼ���˿ʹѹ��������ʱ��ʱ��
int16_t   gss_heat20_run_delaytimer;         //ëϸ�ܼ���˿����ʱ��ʱ��
int16_t   gss_heat20_stop_delaytimer;        //ëϸ�ܼ���˿����ʱ��ʱ��

uint8_t   guc_trip_flash_cnt;                //���ϵ���˸������
uint8_t   guc_trip_flash_timer;              //���ϵ���˸��ʱ��
uint8_t   guc_trip_code;                     //���ϴ���




//====��ɫ��_program====
/****************************************************************************************************************************************************
�������ܣ���ɫ�Ƹ��ݱ�����������ʾ��ͬ����ɫ(�͵�ƽΪ��)

          ��ɫ����+��

����λ�ã���ѭ��--------------------------------------δ��
***************************************************************************************************************************************************/
void three_led_deal(void)
{
    if (bflg_test_mode == 0)
    {
        //��ϵ硢���¼����¡����ź�ɫ
        if((bflg_alarm_power_off == 1)||(bflg_alarm_ALH == 1)||(bflg_alarm_ALL == 1)||(bflg_alarm_open_door == 1))
        {
            RED_LED_PIN = 0;
            GREEN_LED_PIN = 1;
            BLUE_LED_PIN = 1;
        }
        //�绷�¹��߱������������౨��������౨�����������౨������ɫ��
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




//====���_pragram====
/*****************************************************************************************************************************
��������: ����Ŀ���
              RL7 ----bflg_outfan20_running
              RL6 ----bflg_outfan21_running
              �ɵ� -- ֻ�Ƿŵ�E2�У�������ʱ����
              
����λ��:     ��1s�ж��е��� -------------------------------ok
******************************************************************************************************************************/
void outfan2_ctrl_deal(void)  
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))     //��ǿ�硢12v
    {
        bflg_outfan20_running = 0;
        bflg_outfan21_running = 0;
    }
    else
    {
        if(bflg_TC_fault == 0)                          //��������������
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
        else                                            //��������������
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





//====ƽ��ڼ���˿_program===
/*******************************************************************************
��������: ƽ��ڼ���˿������ʱ����
          �����������¶ȡ�-35.0��  ���� 5 ��ʱ�� ƽ��ڼ���˿���ȣ�
          �����������¶ȣ�-35.0��  ���� 5 ��ʱ��ƽ��ڼ���˿ֹͣ���ȡ�

����λ��:1s��ʱ��------------------------------------ok
********************************************************************************/
void heat21_ctrl_delaytime(void)   
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))  //��ǿ�硢12v
    {
        bflg_heat21_running = 0;
    }
    else 
    {
        if (gss_TA <= ram_para2[num_heat21_T])   //-35��
        {
            gss_heat21_stop_delaytimer = 0;
            
            gss_heat21_run_delaytimer++;
            if (gss_heat21_run_delaytimer >= ram_para2[num_heat21_run_delaytime])   //5��
            {
                gss_heat21_run_delaytimer = 0;
                
                bflg_heat21_running = 1;
            }
        }
        else
        {
            gss_heat21_run_delaytimer = 0;
            
            gss_heat21_stop_delaytimer++;
            if (gss_heat21_stop_delaytimer >= ram_para2[num_heat21_stop_delaytime]) //5��
            {
                gss_heat21_stop_delaytimer = 0;
                
                bflg_heat21_running = 0;
            }
        }
    }
}



//====�������˿_program====
/***********************************************************************************************************************
�¸Ĺ���:  �������˿����

        ѹ�������������������¶ȡ�-35.0�棨 EEP������ 5 �루 EEP��ʱ�����������
        ѹ�����ػ������������¶ȣ�-35.0�棨 EEP������ 5 �루 EEP��ʱ�����������

����λ��:1s��ʱ��--------------------------------------ok        
***********************************************************************************************************************/
void heat22_ctrl_deal(void)     
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))     //��ǿ�硢12v
    {
         bflg_heat22_running = 0;
    }
    else
    {
        if (bflg_comp2_running == 1)                     //ѹ������
        {
            if(bflg_heat22_running == 0)                 //����˿�ػ�
            {
                if (gss_TA <= ram_para2[num_heat22_T])   //-35��
                {                
                    gss_heat22_run_delaytimer++;
                    if (gss_heat22_run_delaytimer >= ram_para2[num_heat22_run_delaytime])   //5��
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
            else                           //��ʱ����
            {            
                if (gss_TA > ram_para2[num_heat22_T])   //-35��
                {                
                    gss_heat22_run_delaytimer++;
                    if (gss_heat22_run_delaytimer >= ram_para2[num_heat22_run_delaytime])   //5��
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
        else                                         //ѹ�����ػ�
        {
             bflg_heat22_running = 0;
        }
    }
}




//====��ڼ���˿_program====
/*********************************************************************************************************************************
��������: ��ڼ���˿���ȿ���

����λ��: 1ms-----------------------------------ok 
*********************************************************************************************************************************/
void tank_mouth_heat(void)
{
    if(bflg_tank_mouth_run == 0)  //��ʱδ����
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





//====ëϸ�ܼ���˿_program====
/********************************************************************************************************************************
���� :��ǿ��(�ֶ�)���ȵ�ָ��Ŀ��: 1����������ëϸ�ܱ�־ --��ëϸ��
                                  2���ùص�ѹ�����ı�־   --��ѹ��
                                  
����λ��: -------------ɾ������˿����ȥ���� ----------------δ��                            
*********************************************************************************************************************************/
void heat20_force_ctrl_deal(void)  //ëϸ�ܼ���˿ǿ�Ƽ��ȿ��ƴ����������ѭ�������е���
{
    if (bflg_heat20_force_run == 1)           //��ǿ�Ƽ���Ҫ��
    {
        bflg_heat20_space_delaytime = 0;
        gss_heat20_space_delaytimer = 0;
        
        bflg_heat20_askfor_run = 1;     //������ëϸ�ܼ���˿����־
        bflg_heat20_comp2_stop = 1;     //��ëϸ�ܼ���˿ʹѹ�����ر�־
    }
}

/******************************************************************************************************************************
����: ÿ��� 16 Сʱ�ж�ëϸ���Ƿ��м��ȣ���ëϸ���Զ���������16��Сʱ����һ�μ���


����λ��: --------------------ɾ������˿����ȥ����------------------δ��
*******************************************************************************************************************************/
void heat20_space_delaytime(void)  //ëϸ�ܼ���˿�����ʱ������1������ʱ�����е���
{
    if (bflg_heat20_space_delaytime == 1)
    {
        //debug
        //gss_heat20_space_delaytimer++;
        if (gss_heat20_space_delaytimer >= ram_para2[num_heat20_space_delaytime])    //960   16Сʱ����һ��
        {
            gss_heat20_space_delaytimer = 0;
            bflg_heat20_space_delaytime = 0;
        }
    }
}

/*******************************************************************************************************************************
����: ëϸ�ܼ��ȿ���  

     ���16��Сʱ��ëϸ�ܽ��м�⣬Ŀ�ľ�����ëϸ��16Сʱ��������һ��

����λ��: ------------------ɾ������˿����ȥ���� -----------------δ��     
*******************************************************************************************************************************/
void heat20_ctrl_deal(void)   //ëϸ�ܼ���˿���ƴ����������ѭ�������е���
{
    if (bflg_heat20_askfor_run == 0)    //���������ëϸ�ܼ���˿��
    {
        if (bflg_heat20_space_delaytime == 0)      // peak ÿ��16Сʱ�ж� =0��16Сʱ���� ���ʼ�ϵ�
        {
            if (bflg_heat20_first_run == 1)  //����״�����
            {
                bflg_heat20_first_run = 0;
                
                if (gss_TA <= ram_para2[num_heat20_T])   // peak 0�� ��ʼ�ϵ磬 ������������¶ȡ�0�棬 ����� 6 ����
                {
                    bflg_heat20_askfor_run = 1;   //������ëϸ�ܼ���˿����־
                    bflg_heat20_comp2_stop = 1;   //��ëϸ�ܼ���˿ʹѹ�����ر�־
                }
                else
                {
                    bflg_heat20_space_delaytime = 1;
                    gss_heat20_space_delaytimer = 0;
                }
            }
            else
            {
                bflg_heat20_askfor_run = 1;  //������ëϸ�ܼ���˿����־
                
                bflg_heat20_wait_delaytime = 1;  //peak ѹ����һֱδͣ������һСʱ�� �� bflg_heat20_comp2_stop = 1;  �ر�ѹ������ëϸ�� 
                gss_heat20_wait_delaytimer = 0;
            }
        }
    }
    else  //�������ëϸ�ܼ���˿��  //peak   bflg_heat20_askfor_run == 1--->bflg_heat20_comp2_stop = 1(����8 line)
                                    //       ----->comp2_ctrl_deal(void) �йػ�
    {
        if (bflg_comp2_running == 0)    //���ѹ����ͣ��
        {
            bflg_heat20_wait_delaytime = 0;
            gss_heat20_wait_delaytimer = 0;
            
            bflg_heat20_comp2_stop = 1;                   // peak ����������˿���������� ѹ����ͣ����ʱ 3 ���ӣ� EEP�� �ڣ�
                                                                 //       ������ѹ������ͣ�жϣ� �����ȹ�����ɺ� ѹ�����������ơ�
            //----------------------------------------------
            if (bflg_heat20_running == 0)    //���ëϸ�ܼ���˿��
            {
                if (bflg_heat20_comp2_stop_delaytime == 0) //���ëϸ�ܼ���˿ʹѹ��������ʱ��
                {
                    bflg_heat20_running = 1;     //ëϸ�ܼ���˿��
                    
                    bflg_heat20_run_delaytime = 1;
                    gss_heat20_run_delaytimer = 0;
                }
            }
            else    //���ëϸ�ܼ���˿��
            {
                if (bflg_heat20_run_delaytime == 0)   //����6��10�������
                {
                    bflg_heat20_running = 0;          //ëϸ�ܼ���˿��
                    
                    bflg_heat20_stop_delaytime = 1;
                    gss_heat20_stop_delaytimer = 0;
                    
                    bflg_heat20_force_run = 0;
                    bflg_heat20_askfor_run = 0;    // peak ֻ��һ����0
                    
                    bflg_heat20_space_delaytime = 1;
                    gss_heat20_space_delaytimer = 0;
                }
            }
        }
    }
}

/******************************************************************************************************************************
����: ���16��Сʱ��û������ëϸ�ܼ��ȵ�Ҫ�����������ëϸ�ܵı�־������ʱ
      ѹ����û��ͣ������ȴ�1Сʱ��ѹ������û��ͣ������ǿ��ѹ����ͣ����

����λ��: --------------ɾ������˿����ȥ���� -------------------------ע��δ��      
*******************************************************************************************************************************/
void heat20_wait_delaytime(void)   //ëϸ�ܼ���˿�ȴ���ʱ������1���Ӷ�ʱ�����е���
{
    if (bflg_heat20_wait_delaytime == 1)
    {
        gss_heat20_wait_delaytimer++;
        if (gss_heat20_wait_delaytimer >= ram_para2[num_heat20_wait_delaytime]) //60 16��Сʱ������ˣ�����1Сʱ��ѹ�������ڿ�������ǿ�ƹػ���
        {
            gss_heat20_wait_delaytimer = 0;
            bflg_heat20_wait_delaytime = 0;
            
            bflg_heat20_comp2_stop = 1;  //��ëϸ�ܼ���˿ʹѹ�����ر�־
        }
    }
}

/******************************************************************************************************************************
����: ëϸ��ǿ�ƹ�ѹ����180���bflg_heat20_comp2_stop_delaytime ��0��ëϸ�ܲ��ܴ򿪽��м��ȣ�
         ëϸ�ܿ�-->ѹ���������
         ѹ������-->ëϸ�ܱ������ ??   ������ÿ�ι�ѹ��-->�� bflg_heat20_comp2_stop_delaytime = 1

����λ��:������������������������������ɾ������˿����ȥ����  ��---------------δ��         
******************************************************************************************************************************/
void heat20_comp2_stop_delaytime(void)  //ëϸ�ܼ���˿ʹѹ��������ʱ������1�붨ʱ�����е���
{
    if (bflg_heat20_comp2_stop_delaytime == 1)
    {
        gss_heat20_comp2_stop_delaytimer++;
        if (gss_heat20_comp2_stop_delaytimer >= ram_para2[num_heat20_comp2_stop_delaytime])    //180
        {                                                      //ѹ����ͣ����3���ӵ�
            gss_heat20_comp2_stop_delaytimer = 0;
            bflg_heat20_comp2_stop_delaytime = 0;
        }
    }
}

/*****************************************************************************************************************************
����: ëϸ�ܼ��ȿ�x���Ӻ� ���ر�ëϸ��

����λ��: 1s---------ɾ������˿����ȥ����   ------------------------- δ��
******************************************************************************************************************************/
void heat20_run_delaytime(void)    
{
    if (bflg_heat20_run_delaytime == 1)
    {
        gss_heat20_run_delaytimer++;
        if (bflg_heat20_force_run == 0) //������Զ�����
        {
            if (gss_heat20_run_delaytimer >= ram_para2[num_heat20_run_delaytime])    //360   �Զ�����6����
            {                                                                        //ëϸ�ܼ�������Զ�ֹͣ
                gss_heat20_run_delaytimer = 0;        
                bflg_heat20_run_delaytime = 0;
            }
        }
        else   //�����ǿ�Ƽ���        // peak �����ֶ�����  
        {
            if (gss_heat20_run_delaytimer >= ram_para2[num_heat20_force_run_delaytime])   //600  �ֶ�����10����
            {
                gss_heat20_run_delaytimer = 0;
                bflg_heat20_run_delaytime = 0;
            }
        }
    }
}

/******************************************************************************************************************************
����: ëϸ�ܼ���180������ bflg_heat20_comp2_stop = 0; ֹͣ��ѹ������ǿ��ͣ������
      ��ص������Ŀ��ƣ�ͨ��������趨�ǿ����ǹأ��н� comp2_ctrl_deal(void) ���� 

����λ��: 1s----------------ɾ������˿����ȥ���� --------------------------------------δ��      
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
            
            bflg_heat20_comp2_stop = 0;            //����6���Ӻ�ֹͣ��ѹ������ǿ��ͣ�����ָ��Զ�ѹ���Զ�����
        }
    }
}

/**************************************************************************************************************************************************
�������ܣ��������Ÿ��ݿ����Ƿ����

����λ�ã���ѭ��-----------------------------------ok
***************************************************************************************************************************************************/
void output_deal(void)      
{
    if (bflg_heat21_running == 0)  //ƽ���
    {
        HTRPHK_OUT_PIN = 0;      
    }
    else
    {
        HTRPHK_OUT_PIN = 1;
    }
    //----------------------------------
    if (bflg_heat22_running == 0) //�������˿
    {
        HTRMT_OUT_PIN = 0;         
    }
    else
    {
        HTRMT_OUT_PIN = 1;
    }
    //----------------------------------
    if (bflg_bat_discharging == 1) //��طŵ�                              
    {
        BAT_DISCHARGE_PIN = 1;
    }
    else
    {
        BAT_DISCHARGE_PIN = 0;
    }
    //----------------------------------
    if(bflg_tank_mouth_run == 1)   //��ڼ���˿
    {
        RL_HTRGK_OUT_PIN = 1;
    }
    else
    {
        RL_HTRGK_OUT_PIN = 0;
    }
}
/*************************************************************************************************************************************
�������ܣ�1���̵�������������
          2��ULN2003��������������������ͣ�
          3����Ƶѹ�������������ͬһ��оƬ�ϣ���������ͬʱ����ÿ���200ms����

����λ�ã���ѭ��---------------------------------------ok
**************************************************************************************************************************************/
void RL_output_deal(void)     
{
    if (bflg_RL_output_delaytime == 0)
    {
        if (RL_COMP1_OUT_PIN != bflg_comp2_running)     //��Ƶѹ��1
        {
            RL_COMP1_OUT_PIN = bflg_comp2_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_COMP2_OUT_PIN != bflg_comp21_running) //��Ƶѹ��2
        {
            RL_COMP2_OUT_PIN = bflg_comp21_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_FAN1_OUT_PIN != bflg_outfan20_running) //���1
        {
            RL_FAN1_OUT_PIN = bflg_outfan20_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_FAN2_OUT_PIN != bflg_outfan21_running) //���2
        {
            RL_FAN2_OUT_PIN = bflg_outfan21_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_HTRMXG_OUT_PIN != bflg_heat20_running) //ëϸ��
        {
            RL_HTRMXG_OUT_PIN = bflg_heat20_running;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
        else if (RL_CON_PIN != bflg_bat_charging)          //���
        {
            RL_CON_PIN = bflg_bat_charging;
            
            bflg_RL_output_delaytime = 1;
            guc_RL_output_delaytimer = 0;
        }
    }
}
/***************************************************************************************************************************************
�������ܣ��̵��������ʱ

����λ�ã�1ms----------------------------------------------ok
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
��������:     �������ŵĵ�ƽ��⣺1������ 2����ز巴

����λ�ã�  10ms---------------------------------------ok          
******************************************************************************************************************************/
void input_pin_delaytime(void)      
{
    if (DOOR1_IN_PIN == 0)                   // 0����
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
    else                                      // 1����      
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
	// ��ط��Ӵ���
    if (BAT_FED_IN_PIN == 1)  //���δ����
    {
        guc_bat_fed_pin_high_delaytimer = 0;
        
        if (bflg_alarm_bat_fed == 1)       // ���ӱ�����־
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
    else     // ��ط���  
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
�������ܣ�  ���ϵ���˸����

����λ�ã� 10ms----------------------------------------δ��
*********************************************************************************************************************************/
void trip_lampblink_deal(void)     
{
    if (guc_trip_code != 0)             //���������ͣ����ת
    {
        if (guc_trip_flash_cnt > 0)     //�����˸�����Ѿ������㣬��ʼ3������ʱ
        {
            guc_trip_flash_timer++;
            if (guc_trip_flash_timer >= 30)  //ÿ0.3����˸һ��
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
            if (guc_trip_flash_timer >= 200)   //2������ʱ
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
