#ifndef _AD_H_
#define _AD_H_
//------------------------------------------------------------------------------
//�ⲿ��������
extern void ad_val_deal(void);     //adֵ���������ADת�������е���

extern void ad_temp_calc(void);    //ad�¶ȼ����������ѭ�������е���

//------------------------------------------------------------------------------
//�ⲿ��������
extern flag_type flg_adc;
          #define   bflg_allow_ad_calc  flg_adc.bits.bit0   //adת��ok��־
          
          #define   bflg_Tbat1_fault    flg_adc.bits.bit1   //���1�¶ȴ��������ϱ�־
          #define   bflg_Tbat2_fault    flg_adc.bits.bit2   //���2�¶ȴ��������ϱ�־
          
          #define   bflg_ad_calc_ok     flg_adc.bits.bit3   //ad������ɱ�־
           

//------------------------------------------------------------------------------
extern uint16_t  gus_ad_val;

extern uint16_t  gus_ad_buffer[12];
extern uint16_t  gus_ad_aver_buffer[12];

extern uint8_t   guc_ad_index;

extern int16_t   gss_TA;      //��������
extern int16_t   gss_THW;     //���´�����
extern int16_t   gss_TC;      //�������¶ȴ�����
extern int16_t   gss_TE;      //�Ƚ������¶ȴ�����

extern int16_t   gss_VAC;     //��Դ��ѹ
extern uint8_t   guc_12V;     //12V��ѹ

extern int16_t   gss_Tbat1;   //﮵���¶�1
extern int16_t   gss_Tbat2;   //﮵���¶�2

extern uint8_t   guc_bat_Vin; //��س���ѹ
extern uint8_t   guc_bat_Vout;//��طŵ��ѹ
extern uint8_t   guc_bat_Vout_instant;  //��طŵ��ѹ˲ʱֵ
extern int16_t   gss_bat_I;   //��س�����

extern int16_t   gss_adg_Temp[10];
extern int16_t   gss_adg_Temp2;

extern uint8_t   guc_ad_calc_cnt;

//------------------------------------------------------------------------------
#endif
