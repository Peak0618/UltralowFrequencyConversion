
#ifndef _M_RTC_H_
#define _M_RTC_H_


//�ⲿ��������
extern void RTC_init(void);                                        
extern void RTC_write_read_deal(void);
extern void RTC_read_interval(void);

extern uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp); //ʱ��ת�����ݴ�����
extern uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp);  //����ת��ʱ�䴦����
extern void RTC_continuous_w(void);
extern void RTC_continuous_r(void);
extern void RTC_write_read_enable(void);


//RTC PT7C4302 �ĵ�ַ���
// 7        6                         5 4 3 2 1          0    --bits  
// x        x                         x x x x x          x
// 1   +   RAM/Clock select bit   +  5-bit addr.   +   Read/Write select bit 
#define   CMD_R_Y   0b10001101     //�����ַ    
#define   CMD_R_MO  0b10001001     //���µ�ַ
#define   CMD_R_D   0b10000111     //���յ�ַ
#define   CMD_R_H   0b10000101     //��Сʱ��ַ
#define   CMD_R_M   0b10000011     //�����ӵ�ַ
#define   CMD_R_S   0b10000001     //�����ӵ�ַ
#define   CMD_R_W   0b10001011     //�����ڵ�ַ


#define   CMD_W_Y   0b10001100     //д���ַ
#define   CMD_W_MO  0b10001000     //д�µ�ַ
#define   CMD_W_D   0b10000110     //д�յ�ַ
#define   CMD_W_H   0b10000100     //дСʱ��ַ
#define   CMD_W_M   0b10000010     //д���ӵ�ַ
#define   CMD_W_S   0b10000000     //д���ӵ�ַ
#define   CMD_W_W   0b10001010     //д���ڵ�ַ



//�ⲿ��������
extern uint8_t guc_RTC_r_year;         //���յ�����
extern uint8_t guc_RTC_r_month;        //���յ�����
extern uint8_t guc_RTC_r_day;          //���յ�����
extern uint8_t guc_RTC_r_hour;         //���յ���ʱ
extern uint8_t guc_RTC_r_minute;       //���յ��ķ�
extern uint8_t guc_RTC_r_second;       //���յ�����

extern uint8_t guc_RTC_w_year;         //���͵���
extern uint8_t guc_RTC_w_month;        //���͵���
extern uint8_t guc_RTC_w_day;          //���͵���
extern uint8_t guc_RTC_w_hour;         //���͵�ʱ
extern uint8_t guc_RTC_w_minute;       //���͵ķ�
extern uint8_t guc_RTC_w_second;       //���͵���

extern uint8_t guc_RTC_ADDR;           //�ܵķ��͵�ַ
extern uint8_t guc_RTC_WDATA;          //�ܵķ��͵�����
extern uint8_t guc_RTC_RDATA;          //�ܵĽ��յ�����
 


//�ⲿ��־����
extern flag_type flg_RTC;
          
                #define   bflg_RTC_allow_write         flg_RTC.bits.bit0    //��������оƬд����
                #define   bflg_RTC_allow_read          flg_RTC.bits.bit1    //������ʱ��оƬ������
                #define   bflg_RTC_addsend_ok          flg_RTC.bits.bit2    //д��ַ���
                #define   bflg_RTC_continuous_w_start  flg_RTC.bits.bit3    //����д��������ʱ����Ŀ�ʼ
                #define   bflg_RTC_year_w              flg_RTC.bits.bit4    //����д�����ı�־
                #define   bflg_RTC_month_w             flg_RTC.bits.bit5    //����д����µı�־
                #define   bflg_RTC_day_w               flg_RTC.bits.bit6    //����д����յı�־
                #define   bflg_RTC_hour_w              flg_RTC.bits.bit7    //����д���ʱ�ı�־
                #define   bflg_RTC_minute_w            flg_RTC.bits.bit8    //����д��ķֵı�־ 
                #define   bflg_RTC_second_w            flg_RTC.bits.bit9    //����д�����ı�־ 

                #define   bflg_RTC_continuous_r_start  flg_RTC.bits.bit10
                #define   bflg_RTC_year_r              flg_RTC.bits.bit11
                #define   bflg_RTC_month_r             flg_RTC.bits.bit12
                #define   bflg_RTC_day_r               flg_RTC.bits.bit13
                #define   bflg_RTC_hour_r              flg_RTC.bits.bit14
                #define   bflg_RTC_minute_r            flg_RTC.bits.bit15
                
extern flag_type flg_RTC_1;

                #define   bflg_RTC_second_r            flg_RTC_1.bits.bit0
                #define   bflg_RTC_continuous_r_ok     flg_RTC_1.bits.bit1   //������ȡ���,������Կģʽ��������Կ
                #define   bflg_RTC_enable_read         flg_RTC_1.bits.bit2   //ʹ�ܶ����򿪶�ʱ���ж�
                #define   bflg_RTC_enable_write        flg_RTC_1.bits.bit3   //ʹ��д���򿪶�ʱ���ж�







#endif
/****************************************END OF THE FILE******************************************/
