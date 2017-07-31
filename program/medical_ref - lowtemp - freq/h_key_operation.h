#ifndef _KEY_OPERATION_H_
#define _KEY_OPERATION_H_
//------------------------------------------------------------------------------
//外部函数声明
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
//led显示编码宏定义
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
#define   data_all_off   0x00    //都不亮
#define   data_dot       0x80    //点
#define   data_hyphen    0x40    //负号

#define   data_0         0x3F    //数字0
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
#define   data_I         0x06    //peak 新加的大写I,和数字1的值一样
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
#define  data_E_dp       0XF9    // 显示 E 和 点；
#define  data_line       0X40    // 只显示 g 代表的那条横线 



//按键值_新版
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
//外部变量声明
extern flag_type flg_disp;         
          #define   bflg_key_locked            flg_disp.bits.bit0  //按键锁定标志
          #define   bflg_key_long_valid        flg_disp.bits.bit1  //长按键有效标志，现用于长按set和up时进入解锁、user这2个层
          #define   bflg_key_long_delaytime    flg_disp.bits.bit2  //长按键延时标志
          #define   bflg_disp1_val             flg_disp.bits.bit3  //数值显示有效标志
          #define   bflg_disp_minus            flg_disp.bits.bit4  //显示负数标志
          #define   bflg_read_key_delaytime    flg_disp.bits.bit5  //读键值延时标志
          #define   bflg_eep_init              flg_disp.bits.bit6  //EEP初始化标志       
		  #define   bflg_fac_bS_buz_valid      flg_disp.bits.bit7  // 长按 "蜂鸣取消" 按键有效标志
		  #define   flag_view_error_code       flg_disp.bits.bit8  // 显示错误代码的有效标志
		  #define   bflg_led_loop_set          flg_disp.bits.bit9  // led 闪烁的标志
		  #define   bflg_error_code_delaytime  flg_disp.bits.bit10 // 错误代码显示时间长度
		  #define   bflg_IC_register_state     flg_disp.bits.bit11 //是否进入注册状态
          
extern flag_type flg_disp1;   
         #define   bflg_disp1_flicker               flg_disp1.bits.bit0  // "箱内温度设定"闪烁标记
         #define   bflg_IC_pass_CL1                 flg_disp1.bits.bit1  //清除IC注销的参数标志
         #define   bflg_factory_AT_test             flg_disp1.bits.bit2  //在factory工厂模式中AT指令要执行的标志；
         #define   bflg_factory_CL4                 flg_disp1.bits.bit3  //在factory工厂模式中CL4指令要执行的标志--恢复出厂设置
         #define   bflg_IC_erasure_order            flg_disp1.bits.bit4  //指令CL1、CL3的注销所有已经注册的IC卡指令标志
         #define   bflg_key_long_down_valid         flg_disp1.bits.bit5  //锁定状态时长按down键是否达到5s的标志
         #define   bflg_key_long_down_delaytime     flg_disp1.bits.bit6  //锁定状态按下down键，开始计时标志；
         #define   bflg_unlocked_deal               flg_disp1.bits.bit7  //长按set键解锁后，需要松开一下set键才可以继续操作；
         #define   bflg_key_long_updown_delaytime   flg_disp1.bits.bit8  //长按up+down计时
         #define   bflg_key_long_updown_valid       flg_disp1.bits.bit9  //长按up+down有效
         #define   bflg_key_long_setdown_delaytime  flg_disp1.bits.bit10 //长按set+down计时 ,同下
         #define   bflg_key_long_setdown_valid      flg_disp1.bits.bit11 //长按up+down有效，现用作在user1长按down 5s
         #define   bflg_key_long_set_valid          flg_disp1.bits.bit12 //长按set键有效，锁定模式下长按set进入调试
         
extern flag_type flg_error_code_mark;   //错误代码已读标记
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
        #define   bflg_self_test                         flg_save1.bits.bit0  //系统自检标志
        #define   bflg_self_test_flicker                 flg_save1.bits.bit1  //系统自检闪烁标志
        #define   bflg_self_test_sound                   flg_save1.bits.bit2  //系统自检响一声
        #define   bflg_usb_set_time                      flg_save1.bits.bit3  //表示有设置时间动作让usb修改时间
        #define   bflg_usb_set_time_delaytime            flg_save1.bits.bit4  //usb设置时间延时
        #define   bflg_key_long_set_delaytime            flg_save1.bits.bit5  //长按set延时标志

        #define   bflg_parm_dis_form                     flg_save1.bits.bit6  //显示 0:参数标识 or 1:参数值
        #define   bflg_parm_save                         flg_save1.bits.bit7  //是否对参数保存0：未调节上下键按set不保存 1：调节了上下键按set要保存
        #define   bflg_key_password_long_valid           flg_save1.bits.bit8  //IC注册、解锁、工厂3类密码长按xs后按加10的步进增加
        #define   bflg_key_password_long_valid_delaytime flg_save1.bits.bit9  //输入密码时长按延时

//------------------------------------------------------------------------------
extern uint8_t guc_key_val;         //按键键值
extern uint8_t guc_led_buffer[12];  //数码管寄存器

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
extern int16_t gss_TS_old_value;              //设定前的旧值
extern int16_t gss_ALH_old_value;
extern int16_t gss_ALL_old_value;

//------------------------------------------------------------------------------
extern int16_t   gss_disp_see_buffer[6];
extern const uint8_t guc_nmb_table[];


extern int16_t   gss_read_key_delaytimer;      //读键值延时计时器



extern int8_t  guc_user_write_time_value;

//------------------------------------------------------------------------------
extern int16_t    gss_user_CL1_flicker_cnt;   //注销IC卡时CL1闪烁的次数

extern int16_t    gss_fac_AT_test_delaytime;  //测试AT指令时全亮的时间
extern int16_t    gss_fac_CL3_flicker_cnt;    //初始IC密码时CL3闪烁的次数
extern int16_t    gss_fac_CL4_flicker_cnt;    //恢复出厂设置CL4闪烁的次数

extern int16_t   gsc_IC_password_val;         //IC注册模式的密码值
extern int8_t    gsc_disp1_para_mode;         //显示参数模式


extern int8_t    gsc_disp_user1_mode;         //显示user1模式下的几种不同显示方式
extern int8_t    gsc_disp_user2_mode;         //显示user2模式下的几种不同显示方式
extern int8_t    gsc_disp_factory_mode;       //显示factory模式下的几种不同显示方式
extern int8_t    gsc_disp_debug_mode;         //显示海尔调试模式下的几种不同显示方式
extern uint8_t   guc_disp1_mode;
extern int8_t    gsc_disp1_set_mode;          //显示设定模式
extern int8_t    gsc_disp1_see_mode;          //显示查询模式

extern int8_t    gsc_disp_unlocked_password;  //解锁密码
extern int8_t    gsc_password_val;            //密码值


//总的不同的模式
#define   DISP_LOCKED            0   //锁定模式
#define   DISP_UNLOCKED_PASSWORD 1   //解锁密码模式
#define   DISP_UNLOCKED          2   //解锁模式
#define   DISP_SET               3   //设置参数模式
#define   DISP_USER              4   //用户模式
#define   DISP_FAC_PASSWORD      5   //工厂密码模式
#define   DISP_FACTORY           6   //工厂模式
#define   DISP_IC_PASSWORD       7   //IC密码模式
#define   DISP_IC_REG            8   //IC注册模式
#define   DISP_TIME_SET          9   //设置时钟时间的界面
#define   DISP_DEBUG_SELECT      10  //海尔调试时选择调试的参数
#define   DISP_DEBUG             11  //海尔调试时使用的调试参数那一层，在调试完成，正式版本时将此参数隐藏；

//设置层参数索引
#define   DISP_SET_TS            0  //设置温度
#define   DISP_SET_ALH           1  //高温报警值
#define   DISP_SET_ALL           2  //低温报警值
#define   DISP_SET_PARM_SUM      3  //设置参数的总数

//用户层参数索引
#define   DISP_USER_dA           0   //开门报警延时
#define   DISP_USER_T1           1   //usb取数周期
#define   DISP_USER_T2           2   //usb时间
#define   DISP_USER_P6           3   //usb数据导出模式
#define   DISP_USER_IC           4   //IC卡注册密码修改
#define   DISP_USER_PS1          5   //解锁密码
#define   DISP_USER_CL1          6   //IC卡注销
#define   DISP_USER_PARM_SUM     7   //用户参数的总数

//工厂层参数索引
#define   DISP_FACTORY_CL2       0   //usb板出厂数据清空
#define   DISP_FACTORY_UL        1   //USB导数数据的语言选择
#define   DISP_FACTORY_Ct        2   //PT100温度校准
#define   DISP_FACTORY_CU        3   //电压校准
#define   DISP_FACTORY_CA        4   //环温校准
#define   DISP_FACTORY_Fd        5   //蜂鸣报警延时
#define   DISP_FACTORY_AT        6   //报警测试
#define   DISP_FACTORY_SS        7   //热交换器故障屏蔽
#define   DISP_FACTORY_AA        8   //极限环温报警-环温过高
#define   DISP_FACTORY_Cd        9   //高温机开机延时
#define   DISP_FACTORY_CL3       10  //IC卡注册密码初始化
#define   DISP_FACTORY_CL4       11  //恢复出厂值
#define   DISP_FACTORY_PS2       12  //解锁密码-工厂密码
#define   DISP_FACTORY_bS        13  //电池未连接屏蔽
#define   DISP_FACTORY_LL        14  //密钥使能
#define   DISP_FACTORY_HC        15  //柜口加热开关
#define   DISP_FACTORY_PARM_SUM  16  //工厂参数的总数


//调试层参数
#define   DISP_DEBUG_PARA     0   //参数
#define   DISP_DEBUG_SEE      1   //查看

//调试层的查看参数
#define   DISP1_SEE_r00       0
#define   DISP1_SEE_r01       1
#define   DISP1_SEE_r02       2
#define   DISP1_SEE_r03       3
#define   DISP1_SEE_r04       4
#define   DISP1_SEE_r05       5

extern uint8_t   guc_disp_mode;                                 //总的UI模式
extern int8_t    gsc_disp_parm_type;                            //不同的参数类型,所有的参数都用这个
extern int16_t   gss_key_temp_value;                            //参数的临时值,所有的参数都用这个

extern int16_t   gss_set_parm_value[DISP_SET_PARM_SUM];         //设定参数值   
extern int16_t   gss_user_parm_value[DISP_USER_PARM_SUM];       //用户参数值   
extern int16_t   gss_factory_parm_value[DISP_FACTORY_PARM_SUM]; //工厂参数值

#endif
