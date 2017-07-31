#ifndef _COMP2_C_
#define _COMP2_C_
//------------------------------------------------------------------------------
//�����ļ�����
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
//��������
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
//��������
flag_type flg_comp2;

//----------------------------------------------------------
//ѹ����������ر���
int16_t   gss_comp2_power_on_delaytimer;     //�ϵ���ʱ��ʱ��

int16_t   gss_comp2_run_T_delaytimer;        //����ѹ���ﵽ�����¶ȳ���ʱ��
int16_t   gss_comp2_stop_T_delaytimer;       //            �ػ�

int16_t   gss_comp2_run_delaytimer;          //����ѹ����������ʱ��ʱ��
int16_t   gss_comp2_stop_delaytimer;         //����ѹ�����ػ���ʱ��ʱ��
//----------------------------------------------------------
int16_t   gss_comp2_TA_fault_run_delaytimer;      //�����������ϸ���ѹ����������ʱ��ʱ��
int16_t   gss_comp2_TA_fault_stop_delaytimer;     //�����������ϸ���ѹ�����ػ���ʱ��ʱ��

int16_t   gss_comp2_continue_run_delaytimer;      //����ѹ�����������м�ʱ��
int16_t   gss_comp2_force_stop_delaytimer;        //����ѹ����ǿ��ֹͣ��ʱ��

int16_t   gss_comp2_run_space_delaytimer;         //����ѹ�������м����ʱ��ʱ��
int8_t    gss_power_down_delaytime;               //�ϵ�������ϵ����ѹ����ʱ
uint8_t   guc_comp2_power_on_again;               //�ϵ�������ϵ��ѹ����4500����


//Ƶ�ʿ�����ر���
flag_type flg_freq;
int16_t   gss_para[27];

int16_t   gss_comp2_freq;     //����ѹ��Ƶ��
int16_t   gss_comp21_freq;    //����ѹ��Ƶ��   peak ��ת��

int16_t   gss_comp2_delt_freq;
int16_t   gss_comp21_delt_freq;

int16_t   gss_comp2_start_delaytimer;
int16_t   gss_comp2_update_delaytimer;

int16_t   gss_comp21_start_delaytimer;
int16_t   gss_comp21_update_delaytimer;

uint16_t  gus_comp2_freq_cnt;
uint16_t  gus_comp21_freq_cnt;





/***************************************************************************************************************************************
�������ܣ�     �ϵ���ʱ����,��ʼ�ϵ���ʱ 1min���ɵ��� ���ٶԸߡ���ѹ�����п���


����λ�ã�  1s-------------------------ok   
***************************************************************************************************************************************/
void comp2_power_on_delaytime(void)
{
    if (bflg_comp2_power_on_delaytime == 1) //�ϵ��ʼ��Ϊ1
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
��������:     ����Ҫ����¶ȿ���ͣ���㣬�жϸ���ѹ���Ƿ��п���ͣ��������
       
����λ��: 1s��ʱ����------------------------------------------------ok
*****************************************************************************************************************************************/
void comp2_run_stop_delaytime(void)
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))      //��ǿ�硢��12v           
    {
        gss_comp2_run_T_delaytimer = 0;
        gss_comp2_stop_T_delaytimer = 0;

        bflg_comp2_askfor_run = 0;
        bflg_comp2_askfor_stop = 0;
    }
    else
    {
        if (ram_para[num_comp2_mode] == 1)  //����Ǹ���������(Һ����LED��Ĭ�ϸ�����)     
        {
            if (bflg_comp2_running == 0)    //��ʱ�ػ�
            {
                if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_START_DELT_TEMP])   // ������
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
    	        else                                                                      //���� û�ж���
    	        {
    	        	  gss_comp2_run_T_delaytimer = 0;
    	        	  gss_comp2_stop_T_delaytimer = 0;
    	        	  bflg_comp2_askfor_run = 0;
    	        }
            }
            else   //��ʱ����
            {
                if (gss_TA <= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_STOP_DELT_TEMP])  //ͣ����
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
        else  //��������
        {
            if (bflg_comp2_running == 0)
            {
    		    if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + ram_para2[num_comp2_mode_T] + ram_para2[num_comp2_run_Tdelt])
    		    //if (gss_TA >= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_START_DELT_TEMP])  // 10 ����
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
                   //if (gss_TA <= gss_set_parm_value[DISP_SET_TS] + gss_para[COMP_STOP_DELT_TEMP])  // 10 ͣ��
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
            if ((gss_TA - gss_set_parm_value[DISP_SET_TS]) >= gss_para[COMP_DOOR_TDELT])     //5.0��
            {
                bflg_comp2_large_freq = 1;
                bflg_comp21_door_freq_tmp = 0;
            }
        }
    }
}

/********************************************************************************************************************************
�������ܣ���ѹ�������Ƿ��п�ͣ��������С��ͣ����ʱ�Ƿ������ǿ��ͣ���Ƿ������
          �������Ƿ���п�ͣ��


����λ��:��ѭ����-------------------------------------------------------ok          
********************************************************************************************************************************/
void comp2_ctrl_deal(void)    
{
    if((bflg_alarm_power_off == 1)||(bflg_alarm_power_fault == 1))   //��ǿ�硢12v
    {
        bflg_comp2_running = 0;                  //ͣ��״̬

        bflg_comp2_run_space_delaytime = 0;
        gss_comp2_run_space_delaytimer = 0;

        bflg_comp2_run_delaytime = 0;            //�ÿ�����ʱ��־
        gss_comp2_run_delaytimer = 0;            //�忪����ʱ��ʱ��

        bflg_comp2_continue_run_delaytime = 0;
        gss_comp2_continue_run_delaytimer = 0;

        bflg_comp2_first_run = 0;                //���״����б�־

        bflg_comp2_force_stop_delaytime = 0;     //��ǿ��ֹͣ��ʱ��־  //������Ҫ��ǿ��ֹͣ
        gss_comp2_force_stop_delaytimer = 0;     //��ǿ��ֹͣ��ʱ��ʱ��

        bflg_comp2_stop_delaytime = 0;           //�ùػ���ʱ��־
        gss_comp2_stop_delaytimer = 0;           //��ػ���ʱ��ʱ��

        //--------------������ʱ
        bflg_comp2_TA_fault_run_delaytime = 0;   //�������������Ͽ�����ʱ��־        // peak ��30����--������ ��ע3
        gss_comp2_TA_fault_run_delaytimer = 0;   //�������������Ͽ�����ʱ��ʱ��

        bflg_comp2_TA_fault_stop_delaytime = 0;  //�������������Ϲػ���ʱ��־
        gss_comp2_TA_fault_stop_delaytimer = 0;
    }
    else
    {
        if(bflg_power_down_delaytime == 0)      //���δ�ϵ���Ƕϵ����ʱ����
        {
            if (bflg_TA_fault == 1)             //���������������   
            {
                if (bflg_comp2_running == 0)    //����ػ�
                {
                    if (bflg_comp2_TA_fault_stop_delaytime == 0)    //��������������Ϲػ���ʱʱ�����   //peak ��10���ӽ���--������ ��ע3
                    {
                        bflg_comp2_running = 1; //����
                        
                        bflg_comp2_TA_fault_run_delaytime = 1;      //�������������Ͽ�����ʱ��־        // peak ��30����--������ ��ע3
                        gss_comp2_TA_fault_run_delaytimer = 0;      //�������������Ͽ�����ʱ��ʱ��
                        
                        bflg_comp2_run_space_delaytime = 1;        //����ѹ�������м����ʱ��־  //��ע1
                        gss_comp2_run_space_delaytimer = 0;
                    }
                }
                else   //�������
                {
                    if (bflg_comp2_TA_fault_run_delaytime == 0)     //��������������Ͽ�����ʱʱ�����
                    {
                        if (bflg_comp2_volt_low == 0)     //���û�е�ѹ����
                        {
                            bflg_comp2_running = 0;       //�ػ�
                            
                            bflg_comp2_first_run = 0;     //���״����б�־
                            
                            bflg_comp2_TA_fault_stop_delaytime = 1; //�������������Ϲػ���ʱ��־
                            gss_comp2_TA_fault_stop_delaytimer = 0; //�������������Ϲػ���ʱ��ʱ��
                            
                            //bflg_heat20_comp2_stop_delaytime = 1;   //��ëϸ�ܼ���˿ʹѹ��������ʱ��־----ɾ������˿����ȥ����   
                            //gss_heat20_comp2_stop_delaytimer = 0;   //��ëϸ�ܼ���˿ʹѹ��������ʱ��ʱ��
                        }
                    }
                }
            }
            else       //���������������
            {
                if (bflg_comp2_running == 0)     //�����ǰ�ǹػ�״̬
                {
                    if (bflg_comp2_askfor_run == 1)   //������󿪻�  
                    {
                        if ((bflg_comp2_stop_delaytime == 0) && (bflg_comp2_force_stop_delaytime == 0))//ͣ����ʱʱ�����
                        {
                            bflg_comp2_askfor_run = 0;    //�����󿪻���־
                            bflg_comp2_running = 1;       //����
                            
                            bflg_comp2_run_space_delaytime = 1;
                            gss_comp2_run_space_delaytimer = 0;
                            
                            bflg_comp2_run_delaytime = 1; //�ÿ�����ʱ��־
                            gss_comp2_run_delaytimer = 0; //�忪����ʱ��ʱ��
                            
                            bflg_comp2_continue_run_delaytime = 1;
                            gss_comp2_continue_run_delaytimer = 0;
                        }
                    }
                }
                else    //�������
                {
                    if (bflg_comp2_continue_run_delaytime == 0)     //�������ѹ������������ʱ�����
                    {
                        if (bflg_comp2_volt_low == 0)     //���û�е�ѹ����
                        {
                            bflg_comp2_running = 0;  //�ػ�
                            
                            bflg_comp2_first_run = 0;     //���״����б�־
                            
                            bflg_comp2_force_stop_delaytime = 1;    //��ǿ��ֹͣ��ʱ��־  //������Ҫ��ǿ��ֹͣ
                            gss_comp2_force_stop_delaytimer = 0;    //��ǿ��ֹͣ��ʱ��ʱ��
                        }
                        
                    }
                    else     //���ѹ����������ʱ��δ����
                    {
                        if (bflg_comp2_askfor_stop == 1)  //�������ػ�  //peak ��com2_run_stop_delaytime()������ͨ���ж��¶���Ҫ�ػ��� 
                        {
                            if (bflg_comp2_run_delaytime == 0) //������ʱʱ�����
                            {
                                if (bflg_comp2_volt_low == 0)       //���û�е�ѹ����
                                {
                                    bflg_comp2_askfor_stop = 0;     //������ػ���־
                                    bflg_comp2_running = 0;         //�ػ�
                                    
                                    bflg_comp2_first_run = 0;     //���״����б�־
                            
                                    bflg_comp2_stop_delaytime = 1;  //�ùػ���ʱ��־
                                    gss_comp2_stop_delaytimer = 0;  //��ػ���ʱ��ʱ��                                    
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
��������:����ѹ����������ʱ���򣬸���ѹ�����ٿ���3min���ܹػ�

����λ�ã�1s��ʱ��--------------------------------------ok  
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
��������: ����ѹ�����ػ���ʱ�������ٹػ�3min�������¿���

����λ�ã�1s��ʱ��-------------------------------ok
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
��������:������������ʱ������ѹ����������ʱ����30min��� 

����λ�ã�1min--------------------------------ok
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
��������:�����������ϣ�����ѹ�����ػ���ʱ���ػ�10min����ܿ�

����λ�ã�1s��ʱ��---------------------------------------ok
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
��������:��ѹ���״��ϵ�����16Сʱ������ʱ����������5Сʱ��ǿ��ͣ�� 

����λ�ã�1min---------------------------------ok
******************************************************************************************************************************/
void comp2_continue_run_delaytime(void)
{
    if (bflg_comp2_continue_run_delaytime == 1)
    {
        gss_comp2_continue_run_delaytimer++;
        if (bflg_comp2_first_run == 1)  //������״�����
        {
            if (gss_comp2_continue_run_delaytimer >= ram_para2[num_comp2_continue_first_run_delaytime])  //840min = 16h
            {
                gss_comp2_continue_run_delaytimer = 0;
                bflg_comp2_continue_run_delaytime = 0;
            }
        }
        else   //�������������
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
��������: ��ѹ��ǿ��ͣ����ʱ��ǿ��ͣ������Ҫ��������ʱ10min�ٿ���

����λ�ã�1s��ʱ��-----------------------------------ok
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
��������:����ѹ��������ʱ�󣬲��ܿ�����ѹ��
         ��ѹ������1min�� 10min(��ʼ�ϵ�)�󣬲��ܴ򿪵���ѹ��


����λ�ã�1s��ʱ��--------------------------------ok
****************************************************************************************************************************/
void comp2_run_space_delaytime(void)     
{
    if (bflg_comp2_run_space_delaytime == 1)
    {
        gss_comp2_run_space_delaytimer++;
        if (bflg_comp2_first_run == 1)  //������״�����
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
��������: ����ѹ���Ŀ���ͣ�����ж��Ƿ�ﵽ��ͣ������

����λ�ã���ѭ��---------------------------------ok
********************************************************************************************************************************/
void comp21_ctrl_deal(void) 
{
    if (bflg_comp2_running == 0)   //�������ѹ�����ػ�
    {
        bflg_comp21_running = 0;   //����ѹ�����ػ�
    }
    else  //�������ѹ��������
    {
        if (bflg_comp2_run_space_delaytime == 0)  //�������ѹ�������������ʱ�� 
        {
            if(bflg_TA_fault == 1)                //������������
            {
                bflg_comp21_running = 1;
            }
            else
            {
                if (bflg_TE_fault == 0)               //�Ƚ�������
                {
                    if (gss_TE <= ram_para2[num_comp21_run_T])  //-30��
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
�������ܣ���������ʱ����2000ת����1min

����λ�ã�1s��ʱ��--------------------------ok
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
�������ܣ�ѹ����ת��ÿ���80s����һ�θ���

����λ�ã�1s��ʱ��--------------------------------------ok
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
�������ܣ�����ѹ����������ʱ��2000ת����1min ��ϵ����������4500����1min

����λ�ã�1s��ʱ��------------------------------------ok
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
�������ܣ�����ѹ��ת��ÿ80s����һ��

����λ�ã�1s--------------------------------------ok
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
��������: �ϵ�������ϵ磬�ﵽ��������������ѹ����Ҫ��ʱ1min��������

����λ��: 1s��ʱ����------------------------------------ok
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
�������ܣ��ߡ���ѹ����Ƶ�ʿ���

����λ�ã�1s��ʱ����------------------------------------------------ok
***********************************************************************************************************************/
void freq_ctrl_deal(void)
{
    int32_t lss_tmp;
    //----------------------------------
    if (bflg_comp2_running == 0)   //δ����
    {
        if (gss_comp2_freq != 0)
        {
            gss_comp2_freq = 0;
        }
    }
    else                           //����
    {
        if((bflg_comp2_first_power_on == 1)||(guc_comp2_power_on_again == 2)) //�����ϵ�/�ϵ����ϵ�
        {
            bflg_comp2_first_power_on = 0;
            guc_comp2_power_on_again = 0;
            gss_comp2_freq = gss_para[COMP2_FREQ_MAX];   //4500

            bflg_comp2_update_delaytime = 1;             //80s���ڸ���
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
    //����ѹ��
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
            gss_comp21_freq = 4500;          //�ϵ��������ϵ���ѹ����һ��������4500ת

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
                    if (bflg_comp2_large_freq == 1)                       //����1s ��>5��
                    {
                        gss_comp21_freq = gss_para[COMP21_FREQ_MAX];      //4500
                        
                        bflg_comp2_large_freq = 0;                        //bflg_comp2_large_freq ��ϰ��Ӧ��Ϊ--->bflg_comp21_large_freq����Ϊֻ������ѹ��
                    }
                    else if (bflg_comp21_door_freq == 1)                   //����1s
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
��������: �ߡ���ѹ���Ĳ���

����λ�ã���ʼ��---------------------------------ok
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
    gss_para[COMP_START_DELT_TEMP] = ram_para[num_debug_P20]; // P20 ����������ֿɱ���  Ĭ�� 5
    gss_para[COMP_STOP_DELT_TEMP]  = ram_para[num_debug_P21]; // P21 ͣ�������               Ĭ��   0
    gss_para[PT100_DELT_TEMP] = 0;            // 22
    gss_para[COMP_STOP_DELAYTIME] = 180;      // 23 �ػ�3min����ܿ���
    gss_para[COMP_DOOR_TDELT] = 50;           // 24
    gss_para[COMP_DOOR_FREQ] = 3500;          // 25
    gss_para[COMP_RUN_DELAYTIME] = 180;       // 26 ����3min����ܹػ�
}
#endif
/***************************************END OF THE FILE*********************************************/
