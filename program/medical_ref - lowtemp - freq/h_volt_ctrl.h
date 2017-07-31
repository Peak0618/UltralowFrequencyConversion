#ifndef _VOLT_CTRL_H_
#define _VOLT_CTRL_H_
//------------------------------------------------------------------------------
//�ⲿ��������
extern void comp2_volt_low_delaytime(void);  //����ѹ����ѹ������ʱ������1s��ʱ�����е���

extern void volt_ctrl_delaytime(void);  //��ѹ������ʱ������1s��ʱ�����е���

extern void comp2_volt_delaytime(void); //����ѹ������ѹ������ʱ������1s��ʱ�����е���

extern void comp21_volt_delaytime(void);//����ѹ������ѹ������ʱ������1s��ʱ�����е���

//------------------------------------------------------------------------------
//�ⲿ��������
extern flag_type flg_volt;
          
          #define   bflg_comp2_volt_type          flg_volt.bits.bit0  //��ѹ���ͱ�־  ͨ�����뿪��ѡ��
          #define   bflg_comp2_volt_low           flg_volt.bits.bit1  //��ѹ���ͱ�־  0 �޹��� 1��ѹ����
          
          #define   bflg_comp2_volt_down          flg_volt.bits.bit2  //��ѹ��־
          #define   bflg_comp2_volt_up            flg_volt.bits.bit3  //��ѹ��־

          #define   bflg_comp2_volt_delaytime     flg_volt.bits.bit4  //����ѹ������ѹ������ʱ��־     //peak ��ʼ�ϵ�ʱ���ȵ�����ѹ������7min ��Ŷ�����ѹ�����жϣ����ڲ�������ѹ�ˣ�����ѹ������ʱҲ������1��û�н���ע�͵���
          #define   bflg_comp2_volt_first_end     flg_volt.bits.bit5  //����ѹ�����״ε�ѹ���ƽ�����־
          
          #define   bflg_comp21_volt_delaytime    flg_volt.bits.bit6  //����ѹ������ѹ������ʱ��־    //ͬ�� ��ʹ����
          #define   bflg_comp21_volt_first_end    flg_volt.bits.bit7  //����ѹ�����״ε�ѹ���ƽ�����־
          
//------------------------------------------------------------------------------
extern int16_t gss_comp2_volt_delaytimer;    //����ѹ������ѹ������ʱ��ʱ��
extern int16_t gss_comp21_volt_delaytimer;   //����ѹ������ѹ������ʱ��ʱ��

//------------------------------------------------------------------------------
#endif
