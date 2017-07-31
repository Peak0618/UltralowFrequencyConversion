#ifndef _BUZZ_C_
#define _BUZZ_C_


//-----------------------------------------------------------------------------------
//�������ļ�����
#include "h_type_define.h"
#include "r_cg_timer.h"
#include "h_protect.h"

#include "M_buzz.h"
#include "h_main.h"
//----------------------------------------------------------------------------------
//��������
void buzz_loop_tick(void);                //�̵�ѭ��
void buzz_loop_tick_delaytime(void);  //�̵����ʱ��
void buzz_one_tick(void);                 //����һ��
void buzz_one_tick_delaytime(void);   //�������ʱ��
void buzz_always_sound(void);            //����
void buzz_sound_priority(void);         //�̵�ѭ��������һ������������ȼ�
void buzz_sound_cnt(void);



//---------------------------------------------------------------------------------
//��������
flag_type flg_buzz;

//------------------------------------------------------------------------------
int16_t   gss_buzz_loop_tick_delaytime;  
int16_t   gss_buzz_one_tick_delaytime;

uint8_t   guc_buzz_sound_cnt;            //�Զ������������������͸�ֵ���ٸ���
uint8_t   guc_buzz_sound_cnt_delaytime;




/****************************************************************************************************************************************
��������: �ε���
 
����λ��: buzz_sound_priority()����ѭ��-------------------------ok
****************************************************************************************************************************************/
void buzz_loop_tick(void)
{
    if (bflg_buzz_loop_tick_output == 1)
    {    
        if(bflg_buzz_loop_tick_runing == 0)
        {
            bflg_buzz_loop_tick_runing = 1;
            R_TAU0_Channel3_Start();                //�������: 1���б��� 2��δ����"����ȡ��"�� 
        }
    }
    else
    {
        bflg_buzz_loop_tick_runing = 0;
        R_TAU0_Channel3_Stop();  
    }
}

/*********************************************************************************************************************************
��������: �ε�������ʱ
  
����λ�ã�10ms---------------------------ok
*********************************************************************************************************************************/
void buzz_loop_tick_delaytime(void)
{
    if (bflg_buzz_loop_tick_set == 1)    
    {
        gss_buzz_loop_tick_delaytime++;
        if (gss_buzz_loop_tick_delaytime > 10)     //�趨ʱ�� 10ms * 10 = 100 ms
        {
            bflg_buzz_loop_tick_output = ~bflg_buzz_loop_tick_output;
            gss_buzz_loop_tick_delaytime = 0;
        }
    }
    else
    {
        bflg_buzz_loop_tick_output = 0;
        gss_buzz_loop_tick_delaytime = 0;
    }
}


/***************************************************************************************************************************************
��������: ��һ��

����λ��: buzz_sound_priority()����ѭ��-----------------------------ok
***************************************************************************************************************************************/
void buzz_one_tick(void)
{
    if (bflg_buzz_one_tick_set == 1)
    {
        if (bflg_buzz_one_tick_run == 0)
        {
            bflg_buzz_one_tick_run = 1;
            R_TAU0_Channel3_Start();
        }
    }
    else
    {
        if(bflg_buzz_one_tick_stop == 1)
        {
            bflg_buzz_one_tick_stop = 0;
            bflg_buzz_one_tick_run = 0��
            R_TAU0_Channel3_Stop();  
        }
    }
}


/***********************************************************************************************************************************
��������: ��һ������ʱ

����λ��: 100ms-------------------------------------ok       
***********************************************************************************************************************************/
void buzz_one_tick_delaytime(void)
{
    if (bflg_buzz_one_tick_set == 1)	  
    {
        gss_buzz_one_tick_delaytime++;
        if (gss_buzz_one_tick_delaytime >= 2)   // 100 * 2 = 200ms
        {
            bflg_buzz_one_tick_stop = 1;
            bflg_buzz_one_tick_set = 0;
            gss_buzz_one_tick_delaytime = 0;
        }
    }
}

/********************************************************************************************************************************************
��������: ����������

����λ��: buzz_sound_priority()����ѭ��-------------------------ok
********************************************************************************************************************************************/
void buzz_always_sound(void)
{
    if (bflg_buzz_always_sound_set == 1)
    {      
        if (bfla_buzz_always_sound_run == 0)
        {
            bfla_buzz_always_sound_run = 1;
            R_TAU0_Channel3_Start();        
        }
    }
    else
    {   
        if(bfla_buzz_always_sound_run == 1)
        {
            bfla_buzz_always_sound_run = 0; 
            R_TAU0_Channel3_Stop();    
        }
    }
}

/***********************************************************************************************************************************
��������:���������졢�εΡ���һ�������ȼ�˳�� 
         �ڳ���ʱ���������йرշ���������Ϊ�����������еε��������������Ῠ��

����λ��: ��ѭ��--------------------------------ok
***********************************************************************************************************************************/
void buzz_sound_priority(void)
{
    if (bflg_test_mode == 0) 
    {
        buzz_always_sound();
        
        if(bflg_buzz_always_sound_set == 0)  //�޳���
        {
            buzz_one_tick();
            
            if(bflg_buzz_one_tick_set == 0)  //����һ��ʱ
            {
                //����ע��
                buzz_loop_tick();
            }
        }

    }
}

/*************************************************************************************************************************************
��������:factory ָ���AT����ģʽ�еķ������죻


����λ��: 100ms��ʱ����----------ok
*************************************************************************************************************************************/
 void buzz_sound_cnt(void)
{    
    if(guc_buzz_sound_cnt > 0)
    {
        guc_buzz_sound_cnt_delaytime++;
        if(guc_buzz_sound_cnt_delaytime >= 10)  //1s
        {
            guc_buzz_sound_cnt--;
            guc_buzz_sound_cnt_delaytime = 0;
            
            bflg_buzz_one_tick_set = 1;         //��һ��
        }
    }   
}


#endif
/***************************************ENG OF THE FILE*********************************************/
