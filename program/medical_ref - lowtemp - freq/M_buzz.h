#ifndef _M_BUZZ_H_
#define _M_BUZZ_H_

/****************************************************************************************************
  ����ӿ�:
            �̵�ѭ��: bflg_buzz_loop_tick_set       
            ��һ��  : bflg_buzz_one_tick_set        ----"���δ����" ʱ����"����ȡ��" 5��
            ����    : bflg_buzz_always_sound_set    ----ֻ�ڵ�ز巴��

            ԭ������--- bflg_alarm_buzz


 �������:  buzz_one_tick_delaytime();   ������100ms ��
            buzz_loop_tick_delaytime();
            buzz_sound_priority();        

****************************************************************************************************/

//�ⲿ��������
extern void buzz_loop_tick_delaytime(void);   //  �ε�����ʱ����
extern void buzz_one_tick_delaytime(void);    //  ��һ����ʱ
extern void buzz_sound_priority(void);          // ��ͬ���������ȼ�
extern void buzz_sound_cnt(void);                 //�Զ����������




//�ⲿ��־����

extern flag_type flg_buzz;
          
          #define   bflg_buzz_loop_tick_output     flg_buzz.bits.bit0   
          #define   bflg_buzz_loop_tick_set        flg_buzz.bits.bit1   //�������ε���ı�־
          #define   bflg_buzz_one_tick_output      flg_buzz.bits.bit2  
		  
          #define   bflg_buzz_one_tick_set         flg_buzz.bits.bit3    //��������һ����־
		  
          #define   bflg_buzz_always_sound_set     flg_buzz.bits.bit4  
          #define   bflg_buzz_always_sound_output  flg_buzz.bits.bit5
          #define   bfla_buzz_always_sound_run     flg_buzz.bits.bit6    // �������Ѿ����У������ٿ������������������

          #define   bflg_buzz_one_tick_run         flg_buzz.bits.bit7   //ͬ��
          #define   bflg_buzz_loop_tick_runing     flg_buzz.bits.bit8
          #define   bflg_buzz_one_tick_stop        flg_buzz.bits.bit9

//�ⲿ��������
extern uint8_t   guc_buzz_sound_cnt;                //����Ҫ��Ĵ���














#endif
/****************************************END OF THE FILE******************************************/
