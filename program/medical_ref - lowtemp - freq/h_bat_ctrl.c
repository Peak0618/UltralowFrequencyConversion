/****************************************************************************************************************************************
overview��20170519
        1����ء�����
        
*****************************************************************************************************************************************/



#ifndef _BAT_CTRL_C_
#define _BAT_CTRL_C_
//------------------------------------------------------------------------------
//�����ļ�����
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_protect.h"
#include "h_bat_ctrl.h"

//------------------------------------------------------------------------------
//��������
void bat_charge_deal(void);   //��س�紦���������ѭ�������е���

void bat_charge_delaytime(void);   //��س����ʱ������1s��ʱ�����е���

void bat_charge_continue_delaytime(void);    //��������ʱ������1min��ʱ�����е���

void bat_charge_space_delaytime(void);  //��س������ʱ������1min��ʱ�����е���

void bat_discharge_deal(void);     //��طŵ紦���������ѭ�������е���

void bat_discharge_delaytime(void);

void bat_type_check(void);

//------------------------------------------------------------------------------
//��������
flag_type flg_bat;

int16_t   gss_bat_charge_delaytimer;    //��س����ʱ��ʱ��

int16_t   gss_bat_pwm_value;  //��س��pwmռ�ձ�ֵ
int16_t   gss_bat_pwm_cnt;    //��س��pwmռ�ձȼ�����

int16_t   gss_charge_continue_delaytimer;    //��س�������ʱ��ʱ��

uint16_t  gus_bat_charge_space_delaytimer;   //��س������ʱ��ʱ��

uint8_t   gss_discharge_delaytime; 





/********************************************************************************************************************
��������: Ǧ���ء�﮵�صĳ�����
          ���ʱ��Ӧ�Ŀ����PWM--------Ǧ�����ռ�ձ� 25%
                                       ﮵����ռ�ձ� 31%

����λ��: ��ѭ��(�ϵ�1min���ж�)----��updateӰ��Ҳ��ÿ10s���һ��----------------------ok          
********************************************************************************************************************/
void bat_charge_deal(void)    
{
    //��ز巴��12v���ϣ����δ���ӡ�����¶ȹ���---ֹͣ���
    if ((bflg_alarm_bat_fed == 1) || (bflg_alarm_power_fault == 1) ||(bflg_alarm_bat_discnnt == 1)||
        (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight == 1)) //(bflg_alarm_power_off == 1) 
    {   
        bflg_bat_charging = 0;
        gss_bat_pwm_value = 0;

        //bflg_bat_charging_end = 0;             //�´ο������ϵ���ڵڶ��׶�
        //bflg_bat_charge_space_delaytime = 0;   //�´��ϵ������ϴμ�����ʱ�������ʱ��36/7 
        //gus_bat_charge_space_delaytimer = 0;
        
        //bflg_prot2_bat_low_delaytime = 1;      //24Сʱ�����ص�����  
        //gss_prot2_bat_low_delaytimer = 0;
    }
    else
    {
        if (bflg_bat_frist_charge == 1) //������״ε�س��
        {
            bflg_bat_frist_charge = 0;  //���״ε�س���־
            
            bflg_bat_charging = 1;      //��س������
            bflg_bat_charging_end = 0;  //������һ���׶�
            gss_bat_pwm_value = 0;
            
            bflg_bat_charge_space_delaytime = 0;   //����36/7hֹͣ=1��30��������
            gus_bat_charge_space_delaytimer = 0;
            
            bflg_prot2_bat_low_delaytime = 1;      //Ϊ���ڳ��ʱ������ص�����
            gss_prot2_bat_low_delaytimer = 0;
        }
        else
        {
            if (bflg_bat_type == 0)             //�����Ǧ����
            {
                if (bflg_bat_charging == 1)     //�����ǰ�ڳ��
                {
                    if (bflg_bat_charging_end == 0)
                    {
                        if (guc_bat_Vout < ram_para2[num_bat1_charge_V1])     //13.5V//��һ�׶�
                        {
                            if (gss_bat_I > ram_para2[num_bat1_charge_I1])    //610mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat1_charge_I2])    //590mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat1_charge_V2])     //14.3V
                        {
                            if (gss_bat_I > ram_para2[num_bat1_charge_I3])         //270mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat1_charge_I4])    //250mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else
                        {
                            bflg_bat_charging_end = 1;
                        }
                    }
                    else    //����ǳ������׶�
                    {
                        if (guc_bat_Vout > ram_para2[num_bat1_charge_V4])       //13.9V
                        {
                            gss_bat_pwm_value--;
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat1_charge_V5])  //13.7V
                        {
                            gss_bat_pwm_value++;
                        }
                    }
                }
                else   //�����ǰδ���
                {
                    if (guc_bat_Vout < ram_para2[num_bat1_charge_V3])    //12.4V
                    {
                        bflg_bat_charging = 1;  //��س������
                        bflg_bat_charging_end = 0;
                        gss_bat_pwm_value = 0;
                        
                        bflg_bat_charge_space_delaytime = 0;
                        gus_bat_charge_space_delaytimer = 0;
                        
                        bflg_prot2_bat_low_delaytime = 1;          
                        gss_prot2_bat_low_delaytimer = 0;          
                    } 
                    else
                    {
                        if (bflg_bat_charge_space_delaytime == 0)  //peak ���30�쵽
                        {
                            bflg_bat_charging = 1;                 //��س������
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                            
                            bflg_prot2_bat_low_delaytime = 1;   
                            gss_prot2_bat_low_delaytimer = 0;
                        }
                        else
                        {
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                        }
                    }
                }
                //------------------------------
                if (gss_bat_pwm_value < 0)
                {
                    gss_bat_pwm_value = 0;
                }
                else if (gss_bat_pwm_value > 200)      //Ǧ�����ռ�ձ� 200/800 = 25%
                {
                    gss_bat_pwm_value = 200;           //debug ��Ϊ400����
                }
            }
            else  //�����﮵��
            {
                if (bflg_bat_charging == 1)     //�����ǰ�ڳ��
                {
                    if (bflg_bat_charging_end == 0)
                    {
                        if (guc_bat_Vout < ram_para2[num_bat2_charge_V1])     //12.0V
                        {
                            if (gss_bat_I > ram_para2[num_bat2_charge_I1])    //710mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat2_charge_I2])    //690mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat2_charge_V2])     //12.4V
                        {
                            if (gss_bat_I > ram_para2[num_bat2_charge_I3])         //330mA
                            {
                                gss_bat_pwm_value--;
                            }
                            else if (gss_bat_I < ram_para2[num_bat2_charge_I4])    //310mA
                            {
                                gss_bat_pwm_value++;
                            }
                        }
                        else
                        {
                            bflg_bat_charging_end = 1;
                        }
                    }
                    else    //����ǳ������׶�
                    {
                        if (guc_bat_Vout > ram_para2[num_bat2_charge_V4])       //12.9V
                        {
                            gss_bat_pwm_value--;
                        }
                        else if (guc_bat_Vout < ram_para2[num_bat2_charge_V5])  //12.7V
                        {
                            gss_bat_pwm_value++;
                        }
                    }
                }
                else   //�����ǰδ���
                {
                    if (guc_bat_Vout < ram_para2[num_bat2_charge_V3])    //11.2V
                    {
                        bflg_bat_charging = 1;                            //��س������
                        bflg_bat_charging_end = 0;
                        gss_bat_pwm_value = 0;
                        
                        bflg_bat_charge_space_delaytime = 0;
                        gus_bat_charge_space_delaytimer = 0;
                        
                        bflg_prot2_bat_low_delaytime = 1;      
                        gss_prot2_bat_low_delaytimer = 0;
                    }
                    else
                    {
                        if (bflg_bat_charge_space_delaytime == 0)
                        {
                            bflg_bat_charging = 1;   //��س������
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                            
                            bflg_prot2_bat_low_delaytime = 1;   
                            gss_prot2_bat_low_delaytimer = 0;
                        }
                        else
                        {
                            bflg_bat_charging_end = 0;
                            gss_bat_pwm_value = 0;
                        }
                    }
                }
                //------------------------------
                if (gss_bat_pwm_value < 0)
                {
                    gss_bat_pwm_value = 0;
                }
                else if (gss_bat_pwm_value > 250)  //﮵�����ռ�ձ� 250/800 = 31%
                {
                    gss_bat_pwm_value = 250;
                }
            }
        }
    }
}

/**************************************************************************************************************************
��������: ÿ���10s �Ե�س����е�״������һ�θ��£��޸ĳ���ռ�ձȣ�
          
����λ��: 1s��ʱ����---------------------------------------------ok          
***************************************************************************************************************************/
void bat_charge_delaytime(void)    
{
    gss_bat_charge_delaytimer++;
    if (gss_bat_charge_delaytimer >= 10)     //10s
    {
        gss_bat_charge_delaytimer = 0;
        
        bflg_bat_charge_update = 1;
    }
}

/*******************************************************************************************************************************
��������: Ǧ�����������36��Сʱ��ֹͣ��磬��ʼ��ʱ30����ٳ��ļ����ʱ
          ﮵���������7Сʱ���ֹͣ��磬��ʼ��ʱ30����ٳ��ļ����ʱ
          
����λ�ã�1min  -------------------------------------------------ok
*******************************************************************************************************************************/
void bat_charge_continue_delaytime(void) 
{
    if (bflg_bat_charging == 1)
    {
        gss_charge_continue_delaytimer++;
        if (bflg_bat_type == 0)            //Ǧ����
        {
            if (gss_charge_continue_delaytimer >= (ram_para2[num_bat1_charge_continue_delaytime] * 60))   //36h *60 = min
            {
                gss_charge_continue_delaytimer = 0;
                
                bflg_bat_charging = 0;
                bflg_bat_charge_space_delaytime = 1;   //=1 ��ʼ��ʱ30��
                gus_bat_charge_space_delaytimer = 0;
            }
        }
        else                               //﮵��
        {
            if (gss_charge_continue_delaytimer >= (ram_para2[num_bat2_charge_continue_delaytime] * 60))     //7h *60 =min
            {
                gss_charge_continue_delaytimer = 0;
                
                bflg_bat_charging = 0;
                bflg_bat_charge_space_delaytime = 1;
                gus_bat_charge_space_delaytimer = 0;
            }
        }
    }
}

/*******************************************************************************************************************************
��������: ��س���һ�ε磬��ʱ30�����ʱ��30����ٽ��г��

����λ�ã�1min------------------------------------------------ok
*******************************************************************************************************************************/
void bat_charge_space_delaytime(void)    
{
    if (bflg_bat_charge_space_delaytime == 1)
    {
        gus_bat_charge_space_delaytimer++;
        if (bflg_bat_type == 0)    //Ǧ����
        {
            if (gus_bat_charge_space_delaytimer >= (ram_para2[num_bat1_charge_sapce_delaytime] * 24 * 60))    //30����ʱ��
            {
                gus_bat_charge_space_delaytimer = 0;
                bflg_bat_charge_space_delaytime = 0;
            }
        }
        else                       //﮵��
        {
            if (gus_bat_charge_space_delaytimer >= (ram_para2[num_bat2_charge_sapce_delaytime] * 24 * 60))    //30����ʱ��
            {
                gus_bat_charge_space_delaytimer = 0;
                bflg_bat_charge_space_delaytime = 0;
            }
        }
    }
}

/*********************************************************************************************************************************
��������: ��طŵ紦��(��ض��⹩��ڿ�������һ���ĵ磬��Ϊ��Ӳ��12v��ص�·)

����λ�ã� 10ms��ʱ��(�ϵ�3s���ٶԴ˺�������)---------------------------------------------ok          
*********************************************************************************************************************************/
void bat_discharge_deal(void)   
{
    //��ز巴������¶ȹ��߽�ֹ�ŵ�
    if((bflg_alarm_bat_fed == 1) || (bflg_alarm_bat1_T_hight == 1) || (bflg_alarm_bat2_T_hight ==1))                             
    {
        bflg_bat_discharging = 0;
    }     
    else if ((bflg_alarm_power_off == 1) || (bflg_alarm_power_fault == 1)) //���220v�ϵ硢12v��
    {
        if (bflg_bat_type == 0)         //Ǧ����
        {           
            if (guc_bat_Vout <= ram_para2[num_bat1_discharge_off_V])   //10.5V
            {
                bflg_bat_discharging = 0;      
            }
            else
            {   
                bflg_bat_discharging = 1;
            }
        }
        else                            //﮵��
        {
            if (guc_bat_Vout <= ram_para2[num_bat2_discharge_off_V])   //9.0V
            {
                bflg_bat_discharging = 0;   
            }
            else
            {
                bflg_bat_discharging = 1;
            }
        }
    }
    else
    {        
        bflg_bat_discharging = 1;
    }
}

/********************************************************************************************************************************
��������: ��ʼ�ϵ���ʱ3s����е�طŵ���жϣ���ֹ��˸;
          ֮ǰ��˸�Ĵ���1����ؿ��ƶ˿ڳ�ʼ����Ϊ�ر�
                          2���ϵ���ʱ3s���ٴ���ŵ�ĺ���
                          
����λ��: 1s��ʱ��--------------------------------------ok
**********************************************************************************************************************************/
void bat_discharge_delaytime(void)
{
   if(bflg_discharge_delaytime == 1)
    {
         gss_discharge_delaytime++;
        if(gss_discharge_delaytime > 3)
        {
            gss_discharge_delaytime = 0;
            bflg_discharge_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************
��������: ������͵��Զ���⣬�����Ƿ����¶ȴ��������ж���Ǧ���ػ���﮵��:0Ǧ�� 1﮵�
          (﮵�����¶ȴ�������Ǧ����û���¶ȴ�����)
                          
����λ��: ��ѭ��--------------------------------------ok
**********************************************************************************************************************************/
void bat_type_check(void)
{
    if((bflg_bat_1_type == LI_BAT) || (bflg_bat_2_type == LI_BAT)) //CN22 CN23����һ���ڽ�﮵������Ϊ��﮵��
    {
        bflg_bat_type = LI_BAT;         //1﮵�
    }
    else
    {
        bflg_bat_type = LEAD_ACID_BAT;  //0Ǧ��
    }
}



#endif
/*************************************END OF THE FILE*************************************************/
