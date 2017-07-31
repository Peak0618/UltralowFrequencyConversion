/*********************************************************************************************************************************
overview:
        1����⽻����ĵ�ѹ�Ƿ����

        2������ѹ�Ƿ���Ҫ��������ѹ����----Ҫ������
**********************************************************************************************************************************/


#ifndef _VOLT_CTRL_C_
#define _VOLT_CTRL_C_
//------------------------------------------------------------------------------
//�����ļ�����
#include "h_type_define.h"
#include "h_ad.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_volt_ctrl.h"
#include "h_protect.h"
//------------------------------------------------------------------------------
//��������
void comp2_volt_low_delaytime(void);    //����ѹ����ѹ������ʱ������1s��ʱ�����е���

void volt_ctrl_delaytime(void);    //��ѹ������ʱ������1s��ʱ�����е���

void comp2_volt_delaytime(void);   //����ѹ������ѹ������ʱ������1s��ʱ�����е���

void comp21_volt_delaytime(void);  //����ѹ������ѹ������ʱ������1s��ʱ�����е���

//------------------------------------------------------------------------------
//��������
flag_type flg_volt;
//------------------------------------------------------------------------------
int16_t   gss_comp2_volt_delaytimer;    //��ѹ������ʱ��ʱ��
int16_t   gss_volt_down_delaytimer;     //��ѹ��ʱ��ʱ��
int16_t   gss_volt_up_delaytimer;       //��ѹ��ʱ��ʱ��

int16_t   gss_comp2_volt_delaytimer;    //����ѹ������ѹ������ʱ��ʱ��
int16_t   gss_comp21_volt_delaytimer;   //����ѹ������ѹ������ʱ��ʱ��





/****************************************************************************************************************************
�������ܣ� ��ѹ(220����)���͵ĳ����ͻָ���
           ��Դ��ѹ����ʱ�����ܹر�ѹ�������������󣬻��ѹ����ɳ����

����λ�ã� 1s--------------------------------------------------ok        
*****************************************************************************************************************************/
void comp2_volt_low_delaytime(void)      
{
    if(bflg_alarm_power_off == 0)           //δ�ϵ�ʱ��
    {
        if (bflg_comp2_volt_type == 0)      //�����220V��Դ
        {
            if (bflg_comp2_volt_low == 0)   //�����ǰ�޵�ѹ����
            {
                if (gss_VAC <= ram_para2[num_comp2_volt_low_220V1])   //184V
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 1;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
            else
            {
                if (gss_VAC > ram_para2[num_comp2_volt_low_220V1])   //253V---->184
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 0;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
        }
        else  //�����115V��Դ
        {
            if (bflg_comp2_volt_low == 0)   //�����ǰ�޵�ѹ����
            {
                if (gss_VAC <= ram_para2[num_comp2_volt_low_115V1])   //93V
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 1;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
            else
            {
                if (gss_VAC > ram_para2[num_comp2_volt_low_115V1])   //136V-->93
                {
                    gss_comp2_volt_delaytimer++;
                    if (gss_comp2_volt_delaytimer >= ram_para2[num_comp2_volt_low_delaytime])  //10s
                    {
                        gss_comp2_volt_delaytimer = 0;
                        
                        bflg_comp2_volt_low = 0;
                    }
                }
                else
                {
                    gss_comp2_volt_delaytimer = 0;
                }
            }
        }
    }
    else
    {
        gss_comp2_volt_delaytimer = 0;
        bflg_comp2_volt_low = 0;
    }
}


/**************************************************************************************************************************
�������� :  ��Ӧ�������ϵ�--"����ѹ����(�̵�������)"
           ֻ�ǿ��ƶ�Ӧ�ļ̵����Ŀ��ϣ��̵�����������ѹ�豸��

����λ��:    1s----------------����Ҫ��ʹ������ѹ��---------------δ��
**************************************************************************************************************************/
void volt_ctrl_delaytime(void)     //��ѹ������ʱ������1s��ʱ�����е���
{
    if (bflg_comp2_volt_type == 0) //�����220V
    {
        if (bflg_comp2_volt_down == 0)  //�����ǰ�޽�ѹ
        {
            if (gss_VAC >= ram_para2[num_volt_down_220V1]) //242V
            {
                gss_volt_down_delaytimer++;
                if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime])     //10s
                {
                    gss_volt_down_delaytimer = 0;
                    
                    bflg_comp2_volt_down = 1;
                }
            }
            else
            {
                gss_volt_down_delaytimer = 0;
            }
        } 
        else   //�����ǰ��ѹ
        {
            if (gss_VAC <= ram_para2[num_volt_down_220V2]) //230V
            {
                gss_volt_down_delaytimer++;
                if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime])     //10s
                {
                    gss_volt_down_delaytimer = 0;
                    
                    bflg_comp2_volt_down = 0;
                }
            }
            else
            {
                gss_volt_down_delaytimer = 0;
            }
        }
        //--------------------------------------------------
        if (bflg_comp2_volt_up == 0)  //�����ǰ�޽�ѹ
        {
            if (gss_VAC <= ram_para2[num_volt_up_220V1]) //205V
            {
                gss_volt_up_delaytimer++;
                if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                {
                    gss_volt_up_delaytimer = 0;
                    
                    bflg_comp2_volt_up = 1;
                }
            }
            else
            {
                gss_volt_up_delaytimer = 0;
            }
        }
        else   //�����ǰ��ѹ
        {
            if (gss_VAC >= ram_para2[num_volt_up_220V2]) //218V
            {
                gss_volt_up_delaytimer++;
                if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])  //10s
                {
                    gss_volt_up_delaytimer = 0;
                    
                    bflg_comp2_volt_up = 0;
                }
            }
            else
            {
                gss_volt_up_delaytimer = 0;
            }
        }
    }
    else  //�����115V
    {
        if (bflg_comp2_volt_first_end == 1)  //��ʼ�ϵ磬����ѹ��������7���Ӻ�
        {
            if (bflg_comp2_volt_down == 0)   //�����ǰ�޽�ѹ
            {
                if (gss_VAC >= ram_para2[num_volt_down_115V1])    //122V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 1;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            else    //�����ǰ��ѹ
            {
                if (gss_VAC <= ram_para2[num_volt_down_115V2])    //115V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 0;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
        }
        else if (bflg_comp21_volt_first_end == 1)      //��ʼ�ϵ磬����ѹ��������30���
        {
            if (bflg_comp2_volt_down == 0)   //�����ǰ�޽�ѹ
            {
                if (gss_VAC >= ram_para2[num_volt_down_115V3])    //125V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 1;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            else    //�����ǰ��ѹ
            {
                if (gss_VAC <= ram_para2[num_volt_down_115V4])    //120V
                {
                    gss_volt_down_delaytimer++;
                    if (gss_volt_down_delaytimer >= ram_para2[num_volt_up_down_delaytime]) //10s
                    {
                        gss_volt_down_delaytimer = 0;
                        
                        bflg_comp2_volt_down = 0;
                    }
                }
                else
                {
                    gss_volt_down_delaytimer = 0;
                }
            }
            //--------------------------------------------------
            if (bflg_comp2_volt_up == 0)     //�����ǰ�޽�ѹ
            {
                if (gss_VAC <= ram_para2[num_volt_up_115V1]) //98V
                {
                    gss_volt_up_delaytimer++;
                    if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                    {
                        gss_volt_up_delaytimer = 0;
                        
                        bflg_comp2_volt_up = 1;
                    }
                }
                else
                {
                    gss_volt_up_delaytimer = 0;
                }
            }
            else    //�����ǰ��ѹ
            {
                if (gss_VAC >= ram_para2[num_volt_up_115V2]) //105V
                {
                    gss_volt_up_delaytimer++;
                    if (gss_volt_up_delaytimer >= ram_para2[num_volt_up_down_delaytime])   //10s
                    {
                        gss_volt_up_delaytimer = 0;
                        
                        bflg_comp2_volt_up = 0;
                    }
                }
                else
                {
                    gss_volt_up_delaytimer = 0;
                }
            }
        }
    }
}

/******************************************************************************************************************************
��������:     ��115V��ѹ�����У�����ѹ��������7���Ӻ���жϵ�ѹ�ĸߵ�,�Ƿ��������ѹ��

����λ�ã� 1s--------------------------��������ѹ------------------------δ��          
*******************************************************************************************************************************/
void comp2_volt_delaytime(void)    //����ѹ������ѹ������ʱ������1s��ʱ�����е���
{
    if (bflg_comp2_volt_delaytime == 1)
    {
        gss_comp2_volt_delaytimer++;
        if (gss_comp2_volt_delaytimer >= (ram_para2[num_comp2_volt_delaytime] * 60)) //7����
        {
            gss_comp2_volt_delaytimer = 0;
            bflg_comp2_volt_delaytime = 0;
            
            bflg_comp2_volt_first_end = 1;
        }
    }
}

/***********************************************************************************************************************************
��������: ��115V�У�����ѹ�������� 30s ����жϵ�ѹ�ĸߵͣ��ж��Ƿ�����ѹ

����λ�ã�1s----------------------------------------------------δ��
***********************************************************************************************************************************/
void comp21_volt_delaytime(void)   //����ѹ������ѹ������ʱ������1s��ʱ�����е���
{
    if (bflg_comp21_volt_delaytime == 1)
    {
        gss_comp21_volt_delaytimer++;
        if (gss_comp21_volt_delaytimer >= ram_para2[num_comp21_volt_delaytime]) //30��
        {
            gss_comp21_volt_delaytimer = 0;
            bflg_comp21_volt_delaytime = 0;
            
            bflg_comp21_volt_first_end = 1;
        }
    }
}


#endif
/******************************************END OF THE FILE***************************************************/
