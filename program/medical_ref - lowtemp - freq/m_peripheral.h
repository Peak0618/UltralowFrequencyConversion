#ifndef _M_PERIPHERAL_H_
#define _M_PERIPHERAL_H_

//����
extern void three_led_deal(void);
extern void three_led_deal(void);
extern void outfan2_ctrl_deal(void)  ;
extern void heat21_ctrl_delaytime(void);
extern void heat22_ctrl_deal(void);
extern void tank_mouth_heat(void);
extern void output_deal(void);  
extern void RL_output_deal(void);     
extern void RL_output_delaytime(void);     
extern void input_pin_delaytime(void);     

/*  ëϸ�ܲ�ʹ����
void heat20_force_ctrl_deal(void);
void heat20_space_delaytime(void);
void heat20_ctrl_deal(void)   ;
void heat20_wait_delaytime(void);
void heat20_comp2_stop_delaytime(void);  
void heat20_run_delaytime(void);
void heat20_stop_delaytime(void);
*/

//macro
#define   RED_LED_PIN        P8.2      //��ɫ��_   ___�͵�ƽ��
#define   GREEN_LED_PIN      P8.1      //��ɫ��
#define   BLUE_LED_PIN       P8.0      //��ɫ��
#define   RL_HTRGK_OUT_PIN   P4.1      //��ڼ���˿�̵��������  ԭ��P13.0
#define   HTRMT_OUT_PIN      P11.1     //�������˿�����   
#define   HTRMXG_OUT_PIN     P14.6     //ëϸ�ܼ���˿����� 
#define   HTRPHK_OUT_PIN     P14.7     //ƽ��ڼ���˿����� 
#define   BAT_DISCHARGE_PIN  P1.7      //��ع��������     
#define   RL_COMP1_OUT_PIN   P7.4      //ѹ����1�̵��������     
#define   RL_COMP2_OUT_PIN   P7.5      //ѹ����2�̵��������      
          
#define   RL_FAN1_OUT_PIN    P7.3      //���1�̵��������           RL7  bflg_outfan20_running
#define   RL_FAN2_OUT_PIN    P4.2      //���2�̵��������         
          
#define   RL_HTRMXG_OUT_PIN  P13.0     //ëϸ�ܼ���˿�̵�������� ԭ��P4.1
#define   RL_CON_PIN         P1.5      //��س����������    // peak ���Ƶ�صĳ���

#define   DOOR1_IN_PIN       P5.2      //�ſ���1�����           
#define   BAT_FED_IN_PIN     P0.5      //��ز巴�����  

#define   TRIP_LAMP_PIN      P7.7      //���ϵ������       
#define   TFT_OUT_PIN        P10.1     //Һ������Դ�����_�Ĵ���Ĭ�����øߣ����
#define   RL_DOWN_OUT_PIN    P15.6     //��ѹ�̵��������          
#define   RL_UP_OUT_PIN      P10.0     //��ѹ�̵��������          



//flag
extern flag_type flg_peripheral_1;          
        #define   bflg_outfan20_running         flg_peripheral_1.bits.bit0      //�������1����־
        #define   bflg_outfan21_running         flg_peripheral_1.bits.bit1      //�������2����־
        #define   bflg_tank_mouth_run           flg_peripheral_1.bits.bit2      //��ڼ���˿��ʱ
        #define   bflg_heat21_running           flg_peripheral_1.bits.bit3      //ƽ��ڼ���˿����־
        #define   bflg_heat22_running           flg_peripheral_1.bits.bit4      //�������˿����־
        #define   bflg_RL_output_delaytime      flg_peripheral_1.bits.bit5      //�̵��������ʱ��־

        
extern flag_type flg_heat2;          
        #define   bflg_heat20_first_run         flg_heat2.bits.bit0      //ëϸ�ܼ���˿�״ο���־
        #define   bflg_heat20_force_run         flg_heat2.bits.bit1      //ëϸ�ܼ���˿ǿ�Ƽ��ȱ�־  //peak 0�Զ����� 1ǿ��(�ֶ�)  ǿ����ͨѶ�н��������趨
        #define   bflg_heat20_space_delaytime   flg_heat2.bits.bit2      //ëϸ�ܼ���˿�����ʱ��־  //peak ��0ʱëϸ�ܲ��ܼ��ȣ����ۺ��������ֻҪëϸ�ܼ��Ⱥ󶼻���1���ٵ�16Сʱ����0�ļ������:                                                                                            // 1��ǿ��ëϸ�ܼ���  2�����16Сʱ��ëϸ���Զ�����
        #define   bflg_heat20_askfor_run        flg_heat2.bits.bit3      //ëϸ�ܼ���˿���󿪱�־    //peak  ������ļ������: 1��ǿ�ƿ� 2�����ϵ�ʱPT100<0��  3��ÿ���16Сʱ���Զ�����                                                                                                  //bflg_heat20_comp2_stop = 1; Ҳ�϶���1
        #define   bflg_heat20_wait_delaytime    flg_heat2.bits.bit4      //ëϸ�ܼ���˿�ȴ���ʱ��־  //peak���ѹ�������� 1 Сʱ�� EEP�� ��δͣ���� ��ǿ�Ƹ��¡� ����ѹ����ͣ��,ͣ����0                                                                                            //ѹ����һֱδͣ������һСʱ�� �� bflg_heat20_comp2_stop = 1;  �ر�ѹ������ëϸ�� 
        #define   bflg_heat20_comp2_stop        flg_heat2.bits.bit5      //ëϸ�ܼ���˿ʹѹ�����ر�־        //peak �ص���Ŀ��---��ëϸ�ܡ�  �ص��������: 1��ǿ�Ƽ��� 2����ʼ�ϵ�ʱPT100С��0�� 3��16Сʱ����� 4����ëϸ��������
        #define   bflg_heat20_comp2_stop_delaytime   flg_heat2.bits.bit6 //ëϸ�ܼ���˿ʹѹ��������ʱ��־    //peak ëϸ��ʹѹ�����رգ���ѹ�����ر�3���Ӻ� �˱�־=0 ʱ ëϸ�ܲ��ܴ�                                                                                                     //ֻҪ�ǹػ��ͽ���λ��1 ���ж��Ƿ�ﵽ�ػ�3�����ˣ���ʹ��ʱû�м���˿��������Ҳû�£���ΪҪ�Ǵ�ʱ�м���˿����ʱ׼����
        #define   bflg_heat20_running           flg_heat2.bits.bit7      //ëϸ�ܼ���˿����־
        #define   bflg_heat20_run_delaytime     flg_heat2.bits.bit8      //ëϸ�ܼ���˿����ʱ��־       //ëϸ�ܼ���һ��ʱ��󣬾͹ر�
        #define   bflg_heat20_stop_delaytime    flg_heat2.bits.bit9      //ëϸ�ܼ���˿����ʱ��־









#endif 
/****************************************END OF THE FILE***********************************************/

