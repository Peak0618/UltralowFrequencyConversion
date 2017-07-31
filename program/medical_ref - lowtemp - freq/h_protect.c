/********************************************************************************************************************************
overview��
        1���Ը��ֱ����������ǻָ��ļ��

        2�����±���
        3�����±���
        4��220�ϵ�
        5��12v��
        6�����δ����
        7����ص�����
        8�����ű���
        9�����д���������
        10�����¹���
        11����������
        12���������ѹ����
        13��3λ������
        14������������
        15������¶ȹ���
*********************************************************************************************************************************/

#ifndef _PROTECT_C_
#define _PROTECT_C_
//------------------------------------------------------------------------------
//�����ļ�����
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_key_operation.h"
#include "h_protect.h"
#include "h_volt_ctrl.h"
#include "h_bat_ctrl.h"

//peak add
#include "M_buzz.h"
#include "h_main.h"
#include "h_comp2.h"
#include "h_com.h"
//------------------------------------------------------------------------------
//��������
void prot2_TA_first_delaytime(void);    //���������״θߵ��±�����ʱ������1min��ʱ�����е���

void prot2_TA_high_delaytime(void);     //�����������±�����ʱ������1s��ʱ�����е���

void prot2_TA_low_delaytime(void);      //�����������±�����ʱ������1s��ʱ�����е���

void prot2_power_off_deal(void);   //�ϵ籣�������������ѭ�������е���

void port2_power_fault_deal(void); //��Դ����ϴ����������ѭ�������е���

void prot2_bat_discnnt_delaytime(void); //���δ���ӱ�����ʱ������100ms��ʱ�����е���

void prot2_bat_space_delaytime(void);   //���δ���Ӽ������ʱ������100ms��ʱ�����е���

void prot2_bat_low_deal(void);     //��ص����ͱ��������������ѭ�������е���

void prot2_bat_low_delaytime(void);//��ص������ӳٳ�����1min��ʱ�����е���

void port2_door_delaytime(void);   //�ſ��ر�����ʱ������1s��ʱ�����е���

void port2_T_fault_deal(void);     //���������ϴ����������ѭ�������е���

void prot2_THW_high_delaytime(void);    //���´��������߱�����ʱ������1s��ʱ�����е���

void prot2_TC_first_delaytime(void);    //���������״α�����ʱ������1min��ʱ�����е���

void prot2_cond_dirty_delaytime(void);  //�������ౣ����ʱ������1s��ʱ�����е���

void prot2_volt_deal(void);   //��ѹ���걨�����������������ѭ�������е���

void alarm_lamp_deal(void);   //�����ƴ����������ѭ�������е���

void alarm_buzz_deal(void);   //���������������������ѭ�������е���

void alarm_buzz_off_delaytime(void);    //����������ȡ����ʱ������1min��ʱ�����е���

void bat_T_hight_protect(void);             //����¶ȹ����ж�

void alarm_power_off(void);


//------------------------------------------------------------------------------
//��������
flag_type flg_alarm,flg_alarm1,flg_alarm2,flg_prot2;

//peak add
flag_type   memory;

//------------------------------------------------------------------------------
int16_t   gss_prot2_TA_high_delaytimer; //�����������±�����ʱ��ʱ��
int16_t   gss_prot2_TA_low_delaytimer;  //�����������±�����ʱ��ʱ��
int16_t   gss_prot2_TA_first_delaytimer;//���������״α�����ʱ��ʱ��

int16_t   gss_prot2_bat_discnnt_delaytimer;  //���δ���ӱ�����ʱ��ʱ��
int16_t   gss_prot2_bat_space_delaytimer;    //���δ���Ӽ������ʱ��ʱ��
int16_t   gss_prot2_bat_low_delaytimer;      //��ص�������ʱ��ʱ��

int16_t   gss_prot2_door_delaytimer;    //�ſ��ر�����ʱ��ʱ��

int16_t   gss_prot2_THW_high_delaytimer;     //���´��������߱�����ʱ��ʱ��

int16_t   gss_prot2_TC_first_delaytimer;//���������״α�����ʱ��ʱ��
int16_t   gss_prot2_cond_dirty_delaytimer;   //����������ʱ��ʱ��

int16_t   gss_alarm_buzz_off_delaytimer;     //����������ȡ����ʱ��ʱ��

uint16_t  gus_trip_code1;     //���ϴ���1
uint16_t  gus_trip_code2;     //���ϴ���2

uint8_t   guc_power_down_lfreq_cnt;       //�ϵ���ϵ��ѹ����4500����
uint8_t   guc_power_down_hdelay_cnt;      //�ϵ���ϵ��ѹ����ʱ1min����
int16_t   gss_power_off_alarm_delaytime;  //�ϵ�������������ʱ





/**********************************************************************************************************************************
�������ܣ���������(PT100)�ĸߵ��±���������ʱ3h���ж�  
                   
����λ�ã�1min------------------------------------------------------ok        
**********************************************************************************************************************************/
void prot2_TA_first_delaytime(void)     
{
    if (bflg_prot2_TA_first_delaytime == 1)
    {
        gss_prot2_TA_first_delaytimer++;
        if (gss_prot2_TA_first_delaytimer >= ram_para[num_prot2_TA_first_delaytime])  //180min = 3h
        {
            gss_prot2_TA_first_delaytimer = 0;
            bflg_prot2_TA_first_delaytime = 0;
        }
    }
}

/*************************************************************************************************************************************
�������� : �����������±����ĳ����ͻָ�

����λ�ã�1s��ʱ��-----------------------------------ok
**************************************************************************************************************************************/
void prot2_TA_high_delaytime(void)
{
    if(bflg_TA_fault == 0)   //��������δ���ű����±���
    {
        if (bflg_alarm_ALH == 0)
        {
            if (gss_TA >= gss_set_parm_value[DISP_SET_ALH])       
            {
                gss_prot2_TA_high_delaytimer++;
                if (gss_prot2_TA_high_delaytimer >= ram_para2[num_prot2_TA_high_delaytime])   //600s=10min
                {
                    gss_prot2_TA_high_delaytimer = 0;
                    bflg_alarm_ALH = 1;
                    gus_trip_code1 |= 0x0001;    //peak �˹��ϴ�����Һ����Э���ϱ�ʾ��
                }
            }
            else
            {
                gss_prot2_TA_high_delaytimer = 0;
            }
        }
        else                                      
        {
            //if (gss_TA < gss_set_parm_value[DISP_SET_ALH] + ram_para2[num_prot2_TA_high_Tdelt])   //-20
            if (gss_TA < gss_set_parm_value[DISP_SET_ALH] )   
            {
                gss_prot2_TA_high_delaytimer++;
                if (gss_prot2_TA_high_delaytimer >= ram_para2[num_prot2_TA_high_delaytime])   //600s
                {
                    gss_prot2_TA_high_delaytimer = 0;
                    bflg_alarm_ALH = 0;
                    gus_trip_code1 &= ~0x0001;
                }
            }
            else
            {
                gss_prot2_TA_high_delaytimer = 0;
            }
        }
    }
    else
    {
        gss_prot2_TA_high_delaytimer = 0;
    }
}

/*******************************************************************************************************************************
�������ܣ� �����������±����ĳ����ͻָ�

����λ�ã�1s------------------------------ok
*******************************************************************************************************************************/
void prot2_TA_low_delaytime(void) 
{
    if(bflg_TA_fault == 0)   //��������δ��ʱ�ű����±���
    {
        if (bflg_alarm_ALL == 0)
        {
            if (gss_TA <= gss_set_parm_value[DISP_SET_ALL])
            {
                gss_prot2_TA_low_delaytimer++;
                if (gss_prot2_TA_low_delaytimer >= ram_para2[num_prot2_TA_low_delaytime])     //600s =10min
                {
                    gss_prot2_TA_low_delaytimer = 0;
                    bflg_alarm_ALL = 1;
                    gus_trip_code1 |= 0x0002;
                }
            }
            else
            {
                gss_prot2_TA_low_delaytimer = 0;
            }
        }
        else
        {
            //if (gss_TA > gss_set_parm_value[DISP_SET_ALL] + ram_para2[num_prot2_TA_low_Tdelt])    // 20
            if (gss_TA > gss_set_parm_value[DISP_SET_ALL])   
            {
                gss_prot2_TA_low_delaytimer++;
                if (gss_prot2_TA_low_delaytimer >= ram_para2[num_prot2_TA_low_delaytime])    //600s = 10min
                {
                    gss_prot2_TA_low_delaytimer = 0;
                    bflg_alarm_ALL = 0;
                    gus_trip_code1 &= ~0x0002;
                }
            }
            else
            {
                gss_prot2_TA_low_delaytimer = 0;
            }
        }
    }
    else
    {
        gss_prot2_TA_low_delaytimer = 0;
    }
}



/***************************************************************************************************************************
��������: �ϵ籨���ĳ����ͻָ����������ϵ���������ϵ��Ĵ���

����λ�ã� ��ѭ��-------------------------------ok
***************************************************************************************************************************/
void prot2_power_off_deal(void) 
{
    if (gss_VAC <= ram_para2[num_power_fautl_VAC1])       //80V  ��Դ�ϵ�
    {
        bflg_alarm_power_off = 1;
        gus_trip_code1 |= VAC_OFF_CODE;

        guc_power_down_lfreq_cnt = 1;      //�ϵ�
        guc_power_down_hdelay_cnt = 1;
        guc_comp2_power_on_again = 1;
    }
    else if (gss_VAC >= ram_para2[num_power_fautl_VAC2])   //90V  ��Դ����
    {
        bflg_alarm_power_off = 0;
        gus_trip_code1 &= ~VAC_OFF_CODE;

        if( guc_power_down_lfreq_cnt == 1) //�����ϵ��ѹ����4500����
        {
            guc_power_down_lfreq_cnt = 2;
        }
        
        //----------------------------------
        if(guc_power_down_hdelay_cnt == 1) //��ѹ����ʱ1min
        {
            guc_power_down_hdelay_cnt = 0;
            
            bflg_power_down_delaytime = 1;
            gss_power_down_delaytime = 0;
        }
        
        //----------------------------------
        if(guc_comp2_power_on_again == 1)
        {
            guc_comp2_power_on_again = 2;
        }
    }
}

/************************************************************************************************************************************
��������: ��Դ��(12v)���ϵĳ����ͻָ�
          ���������ﵽ����ֵ����ʾ��Ҳ�����б���ָʾ�ˣ�

����λ��:   ��ѭ��-----------------------------ok
************************************************************************************************************************************/
void port2_power_fault_deal(void)     
{
    if (guc_12V < ram_para2[num_power_off_12V])  //9v
    {
        /*if (bflg_alarm_power_off == 0)           //δ��ǿ��  --ԭ     
        {
            bflg_alarm_power_fault = 1;
            gus_trip_code1 |= 0x4000;
        }
        else
        {
            bflg_alarm_power_fault = 0;
            gus_trip_code1 &= ~0x4000;
        }*/
        
        bflg_alarm_power_fault = 1;
        gus_trip_code1 |= DC_F_CODE; 
    }
    else if (guc_12V > ram_para2[num_power_on_12V]) //11v
    {
        bflg_alarm_power_fault = 0;
        gus_trip_code1 &= ~DC_F_CODE;
    }
}

/******************************************************************************************************************************
��������: ���δ���ӹ��ϵĳ����ͻָ�

����λ�ã�100ms--------------------------------------------------ok
*********************************************************************************************************************************************/
void prot2_bat_discnnt_delaytime(void)   
{
    if(bflg_alarm_bat_fed == 0)              //���δ�ӷ�ʱ
    {
        if (bflg_bat_charging == 0)              //�ǳ��״̬
        {
            if (bflg_alarm_bat_discnnt == 0)     
            {
                if (guc_bat_Vout_instant < ram_para2[num_port2_bat_discnnt_V1])   //20 = 2v
                {
                    bflg_alarm_bat_discnnt = 1;
                    gus_trip_code1 |= BAT_NO_CODE; 
                }
            }
            else   
            {
                if (guc_bat_Vout_instant > ram_para2[num_port2_bat_discnnt_V2])   //50 = 5V
                {
                    bflg_alarm_bat_discnnt = 0;
                    gus_trip_code1 &= ~BAT_NO_CODE;
                }
            }
        }
        else   //���״̬
        {
            if (bflg_alarm_bat_discnnt == 0)      
            {
                //if (guc_bat_Vout_instant < 90)//ram_para2[num_port2_bat_discnnt_V1]) //peak ������Ϊ 9v
                if (guc_bat_Vout_instant < 90)       
                {
                    bflg_alarm_bat_discnnt = 1;
                    gus_trip_code1 |= BAT_NO_CODE;
                }
                if(bflg_bat_type == LEAD_ACID_BAT)      //Ǧ���׼��﮵��+      
                {
                    if (guc_bat_Vout_instant > 151)      
                    {
                        bflg_alarm_bat_discnnt = 1;
                        gus_trip_code1 |= BAT_NO_CODE;
                    }
                }
                
            }
            else    
            {
                if (guc_bat_Vout_instant > 100)//ram_para2[num_port2_bat_discnnt_V2])       // 10v
                {
                    bflg_alarm_bat_discnnt = 0;
                    gus_trip_code1 &= ~BAT_NO_CODE;
                }
            }
        }    
    }
    else
    {
        bflg_alarm_bat_discnnt = 0;
        gus_trip_code1 &= ~BAT_NO_CODE;    
    }


}


/*****************************************************************************************************************************
function: ���δ���Ӽ������ʱ����----δ��

����λ�ã�100ms-----------------------------ok
******************************************************************************************************************************/
void prot2_bat_space_delaytime(void)
{
    if (bflg_prot2_bat_space_delaytime == 1)
    {
        gss_prot2_bat_space_delaytimer++;
        if (gss_prot2_bat_space_delaytimer >= 200) //5s
        {
            gss_prot2_bat_space_delaytimer = 0;
            bflg_prot2_bat_space_delaytime = 0;
            
            //bflg_prot2_bat_space_reach = 1;
        }
    }
    else
    {
        gss_prot2_bat_space_delaytimer++;
        if (gss_prot2_bat_space_delaytimer >= 50) //5s
        {
            gss_prot2_bat_space_delaytimer = 0;
            bflg_prot2_bat_space_delaytime = 1;
            
            //bflg_prot2_bat_space_reach = 1;
        }
    }
}

/******************************************************************************************************************************
��������: ��ص����͹��ϵĳ����ͻָ�

�ָ�����:1�����δ���ӣ�2����ѹ>��ֵ  (3���ڳ��ʱ���ܼ���ѱ�ȥ�� )

����λ�ã���ѭ��-----------------------------------ok
******************************************************************************************************************************/
void prot2_bat_low_deal(void)       
{    
    if ((bflg_prot2_bat_low_delaytime == 0) && (bflg_alarm_bat_discnnt == 0))   //���24Сʱ���ҵ��������
	{  
        if (bflg_bat_type == 0)    //�����Ǧ����
        {
            if (guc_bat_Vout < ram_para2[num_bat_low_V1])       //10.5V
            {
                bflg_alarm_bat_low = 1;
                gus_trip_code1 |= BAT_L_CODE; 
            }
            else if (guc_bat_Vout > ram_para2[num_bat_low_V2])   //11.0V
            {
                bflg_alarm_bat_low = 0;
                gus_trip_code1 &= ~BAT_L_CODE;
            }
        }
        else   //�����﮵��
        {
            if (guc_bat_Vout < ram_para2[num_bat_low_V3])       //9.0V
            {
                bflg_alarm_bat_low = 1;
                gus_trip_code1 |= BAT_L_CODE;
            }
            else if (guc_bat_Vout > ram_para2[num_bat_low_V4])   //9.5V
            {
                bflg_alarm_bat_low = 0;
                gus_trip_code1 &= ~BAT_L_CODE;
            }
        }
    }
    else
    {
        bflg_alarm_bat_low = 0;
        gus_trip_code1 &= ~BAT_L_CODE;
    }
}

/********************************************************************************************************************************
��������: ��ʼ�ϵ���24Сʱ���ٶԵ�ص����ͽ��м��
         
����λ�ã� 1min-------------------ok         
********************************************************************************************************************************/
void prot2_bat_low_delaytime(void) //��ص������ӳٳ�����1min��ʱ�����е���
{
    if (bflg_prot2_bat_low_delaytime == 1)
    {
        gss_prot2_bat_low_delaytimer++;
        if (gss_prot2_bat_low_delaytimer >= (ram_para2[num_prot2_bat_low_delaytime] * 60)) //24Сʱ
        {
            gss_prot2_bat_low_delaytimer = 0;
            bflg_prot2_bat_low_delaytime = 0;
        }
    }
}

/*****************************************************************************************************************************
function: ���ű����ĳ����ͻָ���������⿪��5min�򱨿��ű���

����λ�ã� 1s---------------------------------ok
*****************************************************************************************************************************/
void port2_door_delaytime(void)    
{
    if ((bflg_prot2_door1_pin == 1))     //����
    {
        if (bflg_alarm_open_door == 0)
        {
            gss_prot2_door_delaytimer++;
            //if (gss_prot2_door_delaytimer >= (ram_para[num_prot2_door_delaytime] * 60))  //5����
            if (gss_prot2_door_delaytimer >= (gss_user_parm_value[DISP_USER_dA] * 60))        //5���� �ÿɵ���gss_user_parm_value[DISP_USER_dA]�滻֮ǰ�Ĺ̶�ֵ
            {
                gss_prot2_door_delaytimer = 0;
                
                bflg_alarm_open_door = 1; //���ű���
                gus_trip_code1 |= DOOR_OP_CODE;
            }
        }
    }
    else
    {
        gss_prot2_door_delaytimer = 0;
        bflg_alarm_open_door = 0;
        gus_trip_code1 &= ~DOOR_OP_CODE;
    }
}

/******************************************************************************************************************************
��������: ���¡��Ƚ�����������pt100���������϶�ͳһ��Ϊ bflg_alarm_T_fault ����;

����λ�ã���ѭ��---------------------------------ok
*******************************************************************************************************************************/
void port2_T_fault_deal(void)       
{
    if (bflg_THW_fault == 1)    // ���±���
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TE_fault == 1) // �Ƚ���      
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TC_fault == 1) // ������
    {
        bflg_alarm_T_fault = 1;
    }
    else if (bflg_TA_fault == 1) // ��������pt100
    {
        bflg_alarm_T_fault = 1;
    }
    else
    {
        bflg_alarm_T_fault = 0;
    }
}

/*************************************************************************************************************************
��������: ���¹��߱����ĳ����ͻָ�
         
����λ�ã�1s----------------------------------------ok
**************************************************************************************************************************/
void prot2_THW_high_delaytime(void)  
{
    if (bflg_alarm_THW_high == 0)
    {
        //if (gss_THW > ram_para[num_prot2_THW_T])     //35ԭ����       
        if (gss_THW > gss_factory_parm_value[DISP_FACTORY_AA])                  //�ɵ�Ĭ��35
        {
            gss_prot2_THW_high_delaytimer++;
            if (gss_prot2_THW_high_delaytimer >= ram_para2[num_prot2_THW_high_delaytime]) //30s
            {
                gss_prot2_THW_high_delaytimer = 0;
                bflg_alarm_THW_high = 1;
                gus_trip_code1 |= THW_H_CODE;
            }
        }
        else
        {
            gss_prot2_THW_high_delaytimer = 0;
        }
    }
    else
    {
        //if (gss_THW <= (ram_para[num_prot2_THW_T] + ram_para2[num_prot2_THW_Tdelt]))     //34ԭ����
        if (gss_THW <= (gss_factory_parm_value[DISP_FACTORY_AA] + ram_para2[num_prot2_THW_Tdelt]))  //-1            //�ɵ�
        {
            gss_prot2_THW_high_delaytimer++;
            if (gss_prot2_THW_high_delaytimer >= ram_para2[num_prot2_THW_high_delaytime]) //30
            {
                gss_prot2_THW_high_delaytimer = 0;
                bflg_alarm_THW_high = 0;
                gus_trip_code1 &= ~THW_H_CODE;
            }
        }
        else
        {
            gss_prot2_THW_high_delaytimer = 0;
        }
    }
}


/************************************************************************************************************************
��������: �ϵ�2h���ٽ����ж��������౨�������������״α�����ʱ��

����λ�ã�1min--------------------------------------ok
************************************************************************************************************************/
void prot2_TC_first_delaytime(void)     
{
    if (bflg_prot2_TC_first_delaytime == 1)
    {
        gss_prot2_TC_first_delaytimer++;
        if (gss_prot2_TC_first_delaytimer >= ram_para2[num_prot2_TC_first_delaytime]) //120min = 2h    //120
        {
            gss_prot2_TC_first_delaytimer = 0;
            bflg_prot2_TC_first_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************
��������: �������౨���ĳ����ͻָ�

����λ�ã�1s-----------------------------------------------ok
********************************************************************************************************************************/
void prot2_cond_dirty_delaytime(void) 
{
    if (bflg_prot2_TC_first_delaytime == 0)          //��ʼ�ϵ�����ʱ2Сʱ
    {
        if (bflg_alarm_cond_dirty == 0)              
        {
            if ((gss_TC - gss_THW) >= ram_para2[num_prot2_cond_dirty_TC1])   //13��
            {
                gss_prot2_cond_dirty_delaytimer++;
                if (gss_prot2_cond_dirty_delaytimer >= ram_para2[num_prot2_cond_dirty_delaytime])   //300s = 5min
                {
                    gss_prot2_cond_dirty_delaytimer = 0;
                    
                    bflg_alarm_cond_dirty = 1;
                    gus_trip_code1 |= TC_COND_CODE;
                }
            }
            else                                    
            {
                gss_prot2_cond_dirty_delaytimer = 0;
            }
        }
        else                                          //�б���    
        {
            if ((gss_TC - gss_THW) < ram_para2[num_prot2_cond_dirty_TC2])    //10��   
            {
                gss_prot2_cond_dirty_delaytimer++;
                if (gss_prot2_cond_dirty_delaytimer >= ram_para2[num_prot2_cond_dirty_delaytime])   //300s
                {
                    gss_prot2_cond_dirty_delaytimer = 0;
                    
                    bflg_alarm_cond_dirty = 0;
                    gus_trip_code1 &= ~TC_COND_CODE;
                }
            }
            else
            {
                gss_prot2_cond_dirty_delaytimer = 0;
            }
        }
    }
}

/*****************************************************************************************************************************
��������: (������)��ѹ���걨���ĳ����ͻָ�
           ���������ﵽ����ֵ����ʾ��Ҳ�����б���ָʾ�ˣ�

����λ�ã���ѭ��----------------------------ʵ��δ��--------------------ok
******************************************************************************************************************************/
void prot2_volt_deal(void) 
{
    if(bflg_alarm_power_off == 0)           //δ�ϵ�ʱ��
    {
        if (bflg_comp2_volt_type == 0) //�����220V��Դ
        {
            if ((gss_VAC <= ram_para2[num_prot2_volt_V1]) || (gss_VAC >= ram_para2[num_prot2_volt_V2]))     //184V    253V
            {
                bflg_alarm_volt = 1;
                gus_trip_code1 |= VAC_OVER_CODE;  
            }
            else if ((gss_VAC >= ram_para2[num_prot2_volt_V3]) && (gss_VAC <= ram_para2[num_prot2_volt_V4])) //194V   243V
            {
                bflg_alarm_volt = 0;
                gus_trip_code1 &= ~VAC_OVER_CODE;
            }
        }
        else                          //�����115V��Դ
        {
            if ((gss_VAC <= ram_para2[num_prot2_volt_V5]) || (gss_VAC >= ram_para2[num_prot2_volt_V6]))        //93V   136V
            {
                bflg_alarm_volt = 1;
                gus_trip_code1 |= VAC_OVER_CODE;
            }
            else if ((gss_VAC >= ram_para2[num_prot2_volt_V7]) && (gss_VAC <= ram_para2[num_prot2_volt_V8]))   //98V   131V
            {
                bflg_alarm_volt = 0;
                gus_trip_code1 &= ~VAC_OVER_CODE;
            }
        }
    }
    else
    {
        bflg_alarm_volt = 0;
        gus_trip_code1 &= ~VAC_OVER_CODE;
    }
}

/*********************************************************************************************************************
����λ�ã���ʾ���������3���ƵĴ���ֻҪ������һ�ֱ�������ʾ���ϵ�3λ�����ƶ��᳣����
          
          
����λ�ã���ѭ��----------------------------------------------ok          
*********************************************************************************************************************/
void alarm_lamp_deal(void)              
{
    if (((flg_alarm.uword & 0xff) != 0)||     //�б�����־Ϊ1��
        ((flg_alarm1.uword & 0xff) != 0))
    {        
        bflg_alarm_lamp = 1;
    }
    else
    {
        bflg_alarm_lamp = 0;
    }
}

/******************************************************************************************************************************
��������: �����������Ĵ������ݱ���������ȷ���ǵε��죬���ǳ��죻

����λ�ã���ѭ������������������������ok
******************************************************************************************************************************/
void alarm_buzz_deal(void)     
{   
    flag_type memory_temp;      

    if((bflg_test_mode == 0) && (bflg_com_type == 0))          //�ǲ���ģʽ && LED��    
    {
        if(bflg_alarm_power_off == 0)  //�޶ϵ籨��
        {
            memory_temp.uword = memory.uword;  //memory_temp�洢��һ�εĹ��� 
                                               //flg_alarm ���µĹ��ϣ������жϷ������ε���  
            memory.uword= flg_alarm.uword;     //memory ��flg_alarm���µĹ��ϸ�����ֵ���������ϴεĴ洢�Ĺ���ֵ memory_temp�Աȣ��ж��Ƿ����µĹ��ϱ仯��������'����ȡ��'�����������30�ڲ���

            //--------------------------------------------------------------------------------       
            if((flg_alarm.uword  == 0)||                                      //û�б���
            (bflg_alarm_buzz_off_delaytime == 1)||                            //�б�����������"����ȡ��"
            ((flg_alarm.uword == 0x0008 )&&(bflg_fac_bS_buz_valid == 1)))     //ֻ�е��δ���ӱ����������õ��δ�������β���bS = 1)
            {                                                                   
                bflg_buzz_loop_tick_set = 0;                                  //����3��������ε�          

                if(memory_temp.uword != memory.uword)                         //���µı���ʱ���尴"����ȡ��"30min���죬�ֿ�ʼ��
                {
                    bflg_alarm_buzz_off_delaytime = 0;
                    gss_alarm_buzz_off_delaytimer = 0;
                }
            }           
            else
            {
                bflg_buzz_loop_tick_set = 1;                                    //��������ε���
            }
        }
        else
        {
            bflg_buzz_loop_tick_set = 0;
            //bflg_alarm_buzz_off_delaytime = 0; //���ϵ�ǰ�б�������buzz�����ϵ�Ҫ���±����򿪴�
            //gss_alarm_buzz_off_delaytimer = 0;

            if(bflg_power_off_alarm == 1)
            {
                bflg_power_off_alarm = 0;
                bflg_buzz_one_tick_set = 1;
            }
        }


        //------------------------------------------------------------------------------------
        //��ز巴
        if (bflg_alarm_bat_fed == 1)               //peak ��ؽӷ�������־ 6
        {
            bflg_buzz_always_sound_set = 1;        //peak ����������
        }
        else
        {
            bflg_buzz_always_sound_set = 0;
        }	
    }
    else
    {
        bflg_buzz_always_sound_set = 0;
        bflg_buzz_loop_tick_set = 0;                                
    }
}

/*******************************************************************************************************************************
����λ��:�ε���ʱ������buzz����ֹͣ30min�����ʱ��            
            
����λ��:1min��ʱ�����е���      -------------------------------ok        
********************************************************************************************************************************/
void alarm_buzz_off_delaytime(void)     
{
    if (bflg_alarm_buzz_off_delaytime == 1)
    {
        gss_alarm_buzz_off_delaytimer++;
        //if (gss_alarm_buzz_off_delaytimer >= ram_para[num_alarm_buzz_off_delaytime])     //30����
        if (gss_alarm_buzz_off_delaytimer >= gss_factory_parm_value[DISP_FACTORY_Fd])                               //��Ϊ�ɵ�
        {
            gss_alarm_buzz_off_delaytimer = 0;
            bflg_alarm_buzz_off_delaytime = 0;
        }
    }
}


/***********************************************************************************************************************************
�������� : ����¶ȹ��߱����ж� 

����λ��: ��ѭ����-----------------------------------------------ok
***********************************************************************************************************************************/
void bat_T_hight_protect(void)
{
    //bat1_CN22
    if (gss_Tbat1 >= ram_para2[num_bat2_discharge_off_T])       //65��   ���1 ��2���������׼
    {
        bflg_alarm_bat1_T_hight = 1;
    }
    else if (gss_Tbat1 <= ram_para2[num_bat2_discharge_on_T])   //55��
    {
        bflg_alarm_bat1_T_hight = 0;
    }

    //bat2_CN23
    //--------------------------------------------------------------------
    if (gss_Tbat2 >= ram_para2[num_bat2_discharge_off_T])       //65��
    {
        bflg_alarm_bat2_T_hight = 1;
    }
    else if (gss_Tbat2 <= ram_para2[num_bat2_discharge_on_T])   //55��
    {
        bflg_alarm_bat2_T_hight = 0;
    }
}

/***********************************************************************************************************************************
�������� : �ϵ�ʱ������ÿ���3s��һ��

����λ��: 1s----------------------------------------------ok
***********************************************************************************************************************************/
void alarm_power_off(void)
{
    if(bflg_alarm_power_off == 1)  //�ϵ�
    {
        if(bflg_power_off_alarm == 0)
        {
            gss_power_off_alarm_delaytime++;
            if(gss_power_off_alarm_delaytime >= 3)
            {
                gss_power_off_alarm_delaytime = 0;
                bflg_power_off_alarm = 1;
            }
        }
    }
    else
    {
        bflg_power_off_alarm = 0;
        gss_power_off_alarm_delaytime = 0;
        
    }
}

//------------------------------------------------------------------------------
#endif
