#ifndef _BAT_CTRL_H_
#define _BAT_CTRL_H_
//------------------------------------------------------------------------------
//�ⲿ��������
extern void bat_charge_deal(void);      //��س�紦���������ѭ�������е���

extern void bat_charge_delaytime(void); //��س����ʱ������1s��ʱ�����е���

extern void bat_charge_continue_delaytime(void);  //��������ʱ������1min��ʱ�����е���

extern void bat_charge_space_delaytime(void);//��س������ʱ������1min��ʱ�����е���

extern void bat_discharge_deal(void);   //��طŵ紦���������ѭ�������е���

extern void bat_discharge_delaytime(void);

extern void bat_type_check(void);

//------------------------------------------------------------------------------
//�ⲿ��������
extern flag_type flg_bat;
          #define   bflg_bat_type            flg_bat.bits.bit0   //������ͱ�־  peak 0:Ǧ�� 1:﮵�
          #define   bflg_bat_charge_update   flg_bat.bits.bit1   //��س����±�־
          #define   bflg_bat_frist_charge    flg_bat.bits.bit2   //����״γ���־
          #define   bflg_bat_charging        flg_bat.bits.bit3   //��س���־        peak 0:�������� 1:������
          #define   bflg_bat_charging_end    flg_bat.bits.bit4   //��س������׶α�־���ǽ���ֻ�ǵ���һ���׶�
          
          #define   bflg_bat_charge_space_delaytime    flg_bat.bits.bit5   //��س������ʱ��־���������30/7h��30�������
          #define   bflg_bat_discharging               flg_bat.bits.bit6   //��طŵ��־  0:���ŵ�  1:�ŵ� ��ʼ��ʱΪ1��
          #define   bflg_discharge_delaytime           flg_bat.bits.bit7   //��ʼ�ϵ���ʱ3s�ٴ����طŵ�
          #define   bflg_bat_1_type                    flg_bat.bits.bit8   //�ж�CN22����Ǧ��or﮵�
          #define   bflg_bat_2_type                    flg_bat.bits.bit9   //�ж�CN23����Ǧ��or﮵�
//------------------------------------------------------------------------------
extern int16_t gss_bat_pwm_value;       //��س��pwmռ�ձ�ֵ
extern int16_t gss_bat_pwm_cnt;         //��س��pwmռ�ձȼ�����
extern uint8_t gss_discharge_delaytime; //��ʼ�ϵ���ʱ3s�ڽ��зŵ紦��

//macro
#define   LEAD_ACID_BAT     0           //Ǧ����
#define   LI_BAT            1           //﮵��
//------------------------------------------------------------------------------
#endif
