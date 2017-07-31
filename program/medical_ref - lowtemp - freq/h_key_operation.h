#ifndef _KEY_OPERATION_H_
#define _KEY_OPERATION_H_
//------------------------------------------------------------------------------
//�ⲿ��������
extern void key_mode_select_deal(void);    
extern void key_locked_delaytime(void);  
extern void key_long_delaytime(void);    
extern void key_long_down_delaytime(void);
extern void key_long_set_delaytime(void);
extern void key_long_updown_delaytime(void);
extern void key_long_setdown_delaytime(void);
extern void read_key_delaytime(void);    

//peak add
extern void key_buzz_cancel(void);
extern void bat_con_check_again(void);  
extern void led_loop_delaytime(void);        
extern void led_disp1_flicker_delaytime(void);    
extern void key_factory_AT_test(void);
extern void key_factory_CL4_init(void);        
extern void first_power_on_self_test(void);        
extern void IC_unlock_deal(void);                         
extern void IC_unlock_delaytime(void);  
extern void key_RTC_time_parm_sent_usb_delaytime(void);    
extern void key_password_long_valid_delaytime(void);

        

//------------------------------------------------------------------------------
//led��ʾ����궨��
//---------------------
//|        a          |
//|       ----        |
//|    f | g  | b     |
//|       ----        |
//|    e |    | c     |
//|       ---- .      |
//|        d    dp    |
//---------------------
//----------------------------------------------------------
//    dp   |  g   |  f   |  e   |  d   |  c   |  b   |  a
//   bit7  | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0
//----------------------------------------------------------
#define   data_all_off   0x00    //������
#define   data_dot       0x80    //��
#define   data_hyphen    0x40    //����

#define   data_0         0x3F    //����0
#define   data_1         0x06
#define   data_2         0x5B
#define   data_3         0x4F
#define   data_4         0x66
#define   data_5         0x6D
#define   data_6         0x7D
#define   data_7         0x07
#define   data_8         0x7F
#define   data_9         0x6F

#define   data_A         0x77
#define   data_b         0x7C
#define   data_C         0x39
#define   data_c         0x58
#define   data_d         0x5E
#define   data_E         0x79
#define   data_F         0x71
#define   data_G         0x3D
#define   data_H         0x76
#define   data_I         0x06    //peak �¼ӵĴ�дI,������1��ֵһ��
#define   data_i         0x04
#define   data_J         0x0E
#define   data_L         0x38
#define   data_N         0x37
#define   data_n         0x54
#define   data_o         0x5C
#define   data_P         0x73
#define   data_q         0x67
#define   data_r         0x50
#define   data_S         0x6D
#define   data_T         0x31
#define   data_t         0x78
#define   data_U         0x3E
#define   data_u         0x1C
#define   data_y         0x6E
//peak add
#define  data_E_dp       0XF9    // ��ʾ E �� �㣻
#define  data_line       0X40    // ֻ��ʾ g ������������� 



//����ֵ_�°�
#define   KEY_NONE            0x00
#define   KEY_SET             0x20
#define   KEY_UP              0x08
#define   KEY_DOWN            0x10
#define   KEY_BUZ             0x40 
#define   KEY_SET_AND_UP      0x28 
#define   KEY_SET_AND_DOWN    0x30 
#define   KEY_SET_AND_BUZ     0x60 
#define   KEY_UP_AND_DOWN     0x18 
#define   KEY_UP_AND_BUZ      0x48 
#define   KEY_DOWN_AND_BUZ    0x50 

//------------------------------------------------------------------------------
//�ⲿ��������
extern flag_type flg_disp;         
          #define   bflg_key_locked            flg_disp.bits.bit0  //����������־
          #define   bflg_key_long_valid        flg_disp.bits.bit1  //��������Ч��־�������ڳ���set��upʱ���������user��2����
          #define   bflg_key_long_delaytime    flg_disp.bits.bit2  //��������ʱ��־
          #define   bflg_disp1_val             flg_disp.bits.bit3  //��ֵ��ʾ��Ч��־
          #define   bflg_disp_minus            flg_disp.bits.bit4  //��ʾ������־
          #define   bflg_read_key_delaytime    flg_disp.bits.bit5  //����ֵ��ʱ��־
          #define   bflg_eep_init              flg_disp.bits.bit6  //EEP��ʼ����־       
		  #define   bflg_fac_bS_buz_valid      flg_disp.bits.bit7  // ���� "����ȡ��" ������Ч��־
		  #define   flag_view_error_code       flg_disp.bits.bit8  // ��ʾ����������Ч��־
		  #define   bflg_led_loop_set          flg_disp.bits.bit9  // led ��˸�ı�־
		  #define   bflg_error_code_delaytime  flg_disp.bits.bit10 // ���������ʾʱ�䳤��
		  #define   bflg_IC_register_state     flg_disp.bits.bit11 //�Ƿ����ע��״̬
          
extern flag_type flg_disp1;   
         #define   bflg_disp1_flicker               flg_disp1.bits.bit0  // "�����¶��趨"��˸���
         #define   bflg_IC_pass_CL1                 flg_disp1.bits.bit1  //���ICע���Ĳ�����־
         #define   bflg_factory_AT_test             flg_disp1.bits.bit2  //��factory����ģʽ��ATָ��Ҫִ�еı�־��
         #define   bflg_factory_CL4                 flg_disp1.bits.bit3  //��factory����ģʽ��CL4ָ��Ҫִ�еı�־--�ָ���������
         #define   bflg_IC_erasure_order            flg_disp1.bits.bit4  //ָ��CL1��CL3��ע�������Ѿ�ע���IC��ָ���־
         #define   bflg_key_long_down_valid         flg_disp1.bits.bit5  //����״̬ʱ����down���Ƿ�ﵽ5s�ı�־
         #define   bflg_key_long_down_delaytime     flg_disp1.bits.bit6  //����״̬����down������ʼ��ʱ��־��
         #define   bflg_unlocked_deal               flg_disp1.bits.bit7  //����set����������Ҫ�ɿ�һ��set���ſ��Լ���������
         #define   bflg_key_long_updown_delaytime   flg_disp1.bits.bit8  //����up+down��ʱ
         #define   bflg_key_long_updown_valid       flg_disp1.bits.bit9  //����up+down��Ч
         #define   bflg_key_long_setdown_delaytime  flg_disp1.bits.bit10 //����set+down��ʱ ,ͬ��
         #define   bflg_key_long_setdown_valid      flg_disp1.bits.bit11 //����up+down��Ч����������user1����down 5s
         #define   bflg_key_long_set_valid          flg_disp1.bits.bit12 //����set����Ч������ģʽ�³���set�������
         
extern flag_type flg_error_code_mark;   //��������Ѷ����
         #define   bflg_error_code_alarm_ALH_mark         flg_error_code_mark.bits.bit0   // 1
		 #define   bflg_error_code_alarm_ALL_mark         flg_error_code_mark.bits.bit1   // 2
         #define   bflg_error_code_alarm_power_off_mark   flg_error_code_mark.bits.bit2   // 3
		 #define   bflg_error_code_alarm_power_fault_mark flg_error_code_mark.bits.bit3   // 4
		 #define   bflg_error_code_alarm_bat_discnnt_mark flg_error_code_mark.bits.bit4   // 5
		 #define   bflg_error_code_alarm_bat_fed_mark     flg_error_code_mark.bits.bit5   // 6
		 #define   bflg_error_code_alarm_bat_low_mark     flg_error_code_mark.bits.bit6   // 7
		 #define   bflg_error_code_alarm_TA_fault_mark    flg_error_code_mark.bits.bit7   // 8
		 #define   bflg_error_code_alarm_open_door_mark   flg_error_code_mark.bits.bit8   // 9
		 #define   bflg_error_code_alarm_THW_high_mark    flg_error_code_mark.bits.bit9   // 10 
		 #define   bflg_error_code_alarm_THW_fault_mark   flg_error_code_mark.bits.bit10  // 11      
         #define   bflg_error_code_alarm_TC_fault_mark    flg_error_code_mark.bits.bit11  // 12	 
         #define   bflg_error_code_alarm_TE_fault_mark    flg_error_code_mark.bits.bit12  // 13 
		 #define   bflg_error_code_alarm_cond_dirty_mark  flg_error_code_mark.bits.bit13  // 14
         #define   bflg_error_code_alarm_volt_mark        flg_error_code_mark.bits.bit14  // 15

extern flag_type   flg_save1;
        #define   bflg_self_test                         flg_save1.bits.bit0  //ϵͳ�Լ��־
        #define   bflg_self_test_flicker                 flg_save1.bits.bit1  //ϵͳ�Լ���˸��־
        #define   bflg_self_test_sound                   flg_save1.bits.bit2  //ϵͳ�Լ���һ��
        #define   bflg_usb_set_time                      flg_save1.bits.bit3  //��ʾ������ʱ�䶯����usb�޸�ʱ��
        #define   bflg_usb_set_time_delaytime            flg_save1.bits.bit4  //usb����ʱ����ʱ
        #define   bflg_key_long_set_delaytime            flg_save1.bits.bit5  //����set��ʱ��־

        #define   bflg_parm_dis_form                     flg_save1.bits.bit6  //��ʾ 0:������ʶ or 1:����ֵ
        #define   bflg_parm_save                         flg_save1.bits.bit7  //�Ƿ�Բ�������0��δ�������¼���set������ 1�����������¼���setҪ����
        #define   bflg_key_password_long_valid           flg_save1.bits.bit8  //ICע�ᡢ����������3�����볤��xs�󰴼�10�Ĳ�������
        #define   bflg_key_password_long_valid_delaytime flg_save1.bits.bit9  //��������ʱ������ʱ

//------------------------------------------------------------------------------
extern uint8_t guc_key_val;         //������ֵ
extern uint8_t guc_led_buffer[12];  //����ܼĴ���

#define   LED1      guc_led_buffer[0]
#define   LED2      guc_led_buffer[1]
#define   LED3      guc_led_buffer[2]
#define   LED4      guc_led_buffer[3]
#define   LED5      guc_led_buffer[4]
#define   LED6      guc_led_buffer[5]
#define   LED7      guc_led_buffer[6]
#define   LED8      guc_led_buffer[7]   //lamp
#define   LED9      guc_led_buffer[8]
#define   LED10     guc_led_buffer[9]
#define   LED11     guc_led_buffer[10]
#define   LED12     guc_led_buffer[11]  //lamp


//------------------------------------------------------------------------------
extern int16_t gss_TS_old_value;              //�趨ǰ�ľ�ֵ
extern int16_t gss_ALH_old_value;
extern int16_t gss_ALL_old_value;

//------------------------------------------------------------------------------
extern int16_t   gss_disp_see_buffer[6];
extern const uint8_t guc_nmb_table[];


extern int16_t   gss_read_key_delaytimer;      //����ֵ��ʱ��ʱ��



extern int8_t  guc_user_write_time_value;

//------------------------------------------------------------------------------
extern int16_t    gss_user_CL1_flicker_cnt;   //ע��IC��ʱCL1��˸�Ĵ���

extern int16_t    gss_fac_AT_test_delaytime;  //����ATָ��ʱȫ����ʱ��
extern int16_t    gss_fac_CL3_flicker_cnt;    //��ʼIC����ʱCL3��˸�Ĵ���
extern int16_t    gss_fac_CL4_flicker_cnt;    //�ָ���������CL4��˸�Ĵ���

extern int16_t   gsc_IC_password_val;         //ICע��ģʽ������ֵ
extern int8_t    gsc_disp1_para_mode;         //��ʾ����ģʽ


extern int8_t    gsc_disp_user1_mode;         //��ʾuser1ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
extern int8_t    gsc_disp_user2_mode;         //��ʾuser2ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
extern int8_t    gsc_disp_factory_mode;       //��ʾfactoryģʽ�µļ��ֲ�ͬ��ʾ��ʽ
extern int8_t    gsc_disp_debug_mode;         //��ʾ��������ģʽ�µļ��ֲ�ͬ��ʾ��ʽ
extern uint8_t   guc_disp1_mode;
extern int8_t    gsc_disp1_set_mode;          //��ʾ�趨ģʽ
extern int8_t    gsc_disp1_see_mode;          //��ʾ��ѯģʽ

extern int8_t    gsc_disp_unlocked_password;  //��������
extern int8_t    gsc_password_val;            //����ֵ


//�ܵĲ�ͬ��ģʽ
#define   DISP_LOCKED            0   //����ģʽ
#define   DISP_UNLOCKED_PASSWORD 1   //��������ģʽ
#define   DISP_UNLOCKED          2   //����ģʽ
#define   DISP_SET               3   //���ò���ģʽ
#define   DISP_USER              4   //�û�ģʽ
#define   DISP_FAC_PASSWORD      5   //��������ģʽ
#define   DISP_FACTORY           6   //����ģʽ
#define   DISP_IC_PASSWORD       7   //IC����ģʽ
#define   DISP_IC_REG            8   //ICע��ģʽ
#define   DISP_TIME_SET          9   //����ʱ��ʱ��Ľ���
#define   DISP_DEBUG_SELECT      10  //��������ʱѡ����ԵĲ���
#define   DISP_DEBUG             11  //��������ʱʹ�õĵ��Բ�����һ�㣬�ڵ�����ɣ���ʽ�汾ʱ���˲������أ�

//���ò��������
#define   DISP_SET_TS            0  //�����¶�
#define   DISP_SET_ALH           1  //���±���ֵ
#define   DISP_SET_ALL           2  //���±���ֵ
#define   DISP_SET_PARM_SUM      3  //���ò���������

//�û����������
#define   DISP_USER_dA           0   //���ű�����ʱ
#define   DISP_USER_T1           1   //usbȡ������
#define   DISP_USER_T2           2   //usbʱ��
#define   DISP_USER_P6           3   //usb���ݵ���ģʽ
#define   DISP_USER_IC           4   //IC��ע�������޸�
#define   DISP_USER_PS1          5   //��������
#define   DISP_USER_CL1          6   //IC��ע��
#define   DISP_USER_PARM_SUM     7   //�û�����������

//�������������
#define   DISP_FACTORY_CL2       0   //usb������������
#define   DISP_FACTORY_UL        1   //USB�������ݵ�����ѡ��
#define   DISP_FACTORY_Ct        2   //PT100�¶�У׼
#define   DISP_FACTORY_CU        3   //��ѹУ׼
#define   DISP_FACTORY_CA        4   //����У׼
#define   DISP_FACTORY_Fd        5   //����������ʱ
#define   DISP_FACTORY_AT        6   //��������
#define   DISP_FACTORY_SS        7   //�Ƚ�������������
#define   DISP_FACTORY_AA        8   //���޻��±���-���¹���
#define   DISP_FACTORY_Cd        9   //���»�������ʱ
#define   DISP_FACTORY_CL3       10  //IC��ע�������ʼ��
#define   DISP_FACTORY_CL4       11  //�ָ�����ֵ
#define   DISP_FACTORY_PS2       12  //��������-��������
#define   DISP_FACTORY_bS        13  //���δ��������
#define   DISP_FACTORY_LL        14  //��Կʹ��
#define   DISP_FACTORY_HC        15  //��ڼ��ȿ���
#define   DISP_FACTORY_PARM_SUM  16  //��������������


//���Բ����
#define   DISP_DEBUG_PARA     0   //����
#define   DISP_DEBUG_SEE      1   //�鿴

//���Բ�Ĳ鿴����
#define   DISP1_SEE_r00       0
#define   DISP1_SEE_r01       1
#define   DISP1_SEE_r02       2
#define   DISP1_SEE_r03       3
#define   DISP1_SEE_r04       4
#define   DISP1_SEE_r05       5

extern uint8_t   guc_disp_mode;                                 //�ܵ�UIģʽ
extern int8_t    gsc_disp_parm_type;                            //��ͬ�Ĳ�������,���еĲ����������
extern int16_t   gss_key_temp_value;                            //��������ʱֵ,���еĲ����������

extern int16_t   gss_set_parm_value[DISP_SET_PARM_SUM];         //�趨����ֵ   
extern int16_t   gss_user_parm_value[DISP_USER_PARM_SUM];       //�û�����ֵ   
extern int16_t   gss_factory_parm_value[DISP_FACTORY_PARM_SUM]; //��������ֵ

#endif
