#ifndef _ADG_H_
#define _ADG_H_
//------------------------------------------------------------------------------
//�ⲿ��������
extern void adg_init(void);        //adģ���ʼ�������ڳ�ʼ�������е���

extern void adg_data_delaytime(void);   //adģ�����ݴ�����ʱ������100ms��ʱ�����е���

extern void adg_temp_calc(void);   //adģ���¶ȼ��㴦���������ѭ�������е���

extern void adg_iic_int(void);     //adģ��iicģ���жϳ����ڶ�ʱ�жϵ���

//------------------------------------------------------------------------------
extern flag_type flg_adg;
          #define   bflg_adg_reset           flg_adg.bits.bit0   //adģ�鸴λ��־
          #define   bflg_adg_channel         flg_adg.bits.bit1   //adģ��ͨ����־
          #define   bflg_write_config_adg    flg_adg.bits.bit2   //adģ��д���ñ�־
          #define   bflg_write_read_adg      flg_adg.bits.bit3   //adģ���д��־
          #define   bflg_adg_data_ok         flg_adg.bits.bit4   //adģ������ok��־
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
