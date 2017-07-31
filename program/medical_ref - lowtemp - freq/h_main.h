#ifndef _MAIN_H_
#define _MAIN_H_
//------------------------------------------------------------------------------
//�ⲿ��������//
extern void system_init(void);     //ϵͳ��ʼ������

extern void system_start(void);    //ϵͳ��������

extern void main_loop(void);  //��ѭ������

extern void timer_int(void);  //1ms��ʱ�жϳ���

extern void adc_int(void);        //ADת���жϳ���

extern void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data); //дeeprom����

extern int16_t eeprom_read(uint16_t lus_addr);                           //��eeprom����

extern void init_ram_para2(void);

extern void init_ram_para(void);

extern void eeprom2_read_deal(void); //eeprom2��ȡ��������ڳ�ʼ�������е���

extern void eeprom_read_deal(void);  //eeprom��ȡ��������ڳ�ʼ�������е���

extern void operation_parm_read(void);

//------------------------------------------------------------------------------
//�ⲿ��������//
extern flag_type flg_time;
          #define   bflg_1ms_reach          flg_time.bits.bit0    //1ms��ʱ����־
          #define   bflg_10ms_reach         flg_time.bits.bit1    //10ms��ʱ����׼
          
          #define   bflg_askfor_read_eeprom  flg_time.bits.bit2    //�����eeprom��־
          #define   bflg_read_eeprom_error   flg_time.bits.bit3    //��eeprom���־
          #define   bflg_eeprom_fault        flg_time.bits.bit4    //eeprom���ϱ�־
                    
          #define   bflg_test_type            flg_time.bits.bit6   //���԰���ʾ���־
          #define   bflg_test_mode            flg_time.bits.bit7   //����ģʽ��־
          #define   bflg_test_ok              flg_time.bits.bit8   //������ɱ�־
          #define   bflg_test_mode_RTC        flg_time.bits.bit9   //����ģʽ�Ĳ���ʱ�ӵ�·


//----------------------------------------------------------
//���뿪������ں궨��
#define   SW11_IN_PIN         P5.1      //���뿪�������
#define   SW12_IN_PIN         P5.0      //���뿪�������
#define   SW13_IN_PIN         P3.0      //���뿪�������
#define   SW14_IN_PIN         P8.7      //���뿪�������
#define   SW15_IN_PIN         P8.6      //���뿪�������
#define   SW16_IN_PIN         P8.5      //���뿪�������
#define   SW17_IN_PIN         P8.4      //���뿪�������
#define   SW18_IN_PIN         P8.3      //���뿪�������
          
#define   SW21_IN_PIN         P0.6      //���뿪�������
#define   SW22_IN_PIN         P7.0      //���뿪�������
#define   SW23_IN_PIN         P7.1      //���뿪�������
#define   SW24_IN_PIN         P7.2      //���뿪�������

#define   LOCK_OUT_PIN        P11.0    //����������                
#define   RL_ALM_OUT_PIN      P6.5     //Զ�̱����̵��������      =1 �Ǵ�Զ�̱�����  

//------------------------------------------------------------------------------
#define   SOFT_VERSION        1000      //����汾��
//------------------------------------------------------------------------------
//macro 485_ͨѶ
#define   COM_RX_MODE    (P1.0 = 0)     //����ģʽ
#define   COM_TX_MODE    (P1.0 = 1)     //����ģʽ

//------------------------------------------------------------------------------
extern uint8_t   guc_10ms_timer;     //10�����ʱ��
extern uint8_t   guc_100ms_timer;    //100ms��ʱ��
extern uint8_t   guc_1s_timer;       //1s��ʱ��
extern uint8_t   guc_1min_timer;     //1min��ʱ��


//------------------------------------------------------------------------------
#endif

