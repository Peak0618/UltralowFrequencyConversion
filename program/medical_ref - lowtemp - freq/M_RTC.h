
#ifndef _M_RTC_H_
#define _M_RTC_H_


//外部函数声明
extern void RTC_init(void);                                        
extern void RTC_write_read_deal(void);
extern void RTC_read_interval(void);

extern uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp); //时间转换数据处理函数
extern uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp);  //数据转换时间处理函数
extern void RTC_continuous_w(void);
extern void RTC_continuous_r(void);
extern void RTC_write_read_enable(void);


//RTC PT7C4302 的地址相关
// 7        6                         5 4 3 2 1          0    --bits  
// x        x                         x x x x x          x
// 1   +   RAM/Clock select bit   +  5-bit addr.   +   Read/Write select bit 
#define   CMD_R_Y   0b10001101     //读年地址    
#define   CMD_R_MO  0b10001001     //读月地址
#define   CMD_R_D   0b10000111     //读日地址
#define   CMD_R_H   0b10000101     //读小时地址
#define   CMD_R_M   0b10000011     //读分钟地址
#define   CMD_R_S   0b10000001     //读秒钟地址
#define   CMD_R_W   0b10001011     //读星期地址


#define   CMD_W_Y   0b10001100     //写年地址
#define   CMD_W_MO  0b10001000     //写月地址
#define   CMD_W_D   0b10000110     //写日地址
#define   CMD_W_H   0b10000100     //写小时地址
#define   CMD_W_M   0b10000010     //写分钟地址
#define   CMD_W_S   0b10000000     //写秒钟地址
#define   CMD_W_W   0b10001010     //写星期地址



//外部变量声明
extern uint8_t guc_RTC_r_year;         //接收到的年
extern uint8_t guc_RTC_r_month;        //接收到的月
extern uint8_t guc_RTC_r_day;          //接收到的日
extern uint8_t guc_RTC_r_hour;         //接收到的时
extern uint8_t guc_RTC_r_minute;       //接收到的分
extern uint8_t guc_RTC_r_second;       //接收到的秒

extern uint8_t guc_RTC_w_year;         //发送的年
extern uint8_t guc_RTC_w_month;        //发送的月
extern uint8_t guc_RTC_w_day;          //发送的日
extern uint8_t guc_RTC_w_hour;         //发送的时
extern uint8_t guc_RTC_w_minute;       //发送的分
extern uint8_t guc_RTC_w_second;       //发送的秒

extern uint8_t guc_RTC_ADDR;           //总的发送地址
extern uint8_t guc_RTC_WDATA;          //总的发送的数据
extern uint8_t guc_RTC_RDATA;          //总的接收的数据
 


//外部标志声明
extern flag_type flg_RTC;
          
                #define   bflg_RTC_allow_write         flg_RTC.bits.bit0    //允许向钟芯片写数据
                #define   bflg_RTC_allow_read          flg_RTC.bits.bit1    //允许向时钟芯片读数据
                #define   bflg_RTC_addsend_ok          flg_RTC.bits.bit2    //写地址完成
                #define   bflg_RTC_continuous_w_start  flg_RTC.bits.bit3    //连续写入年月日时分秒的开始
                #define   bflg_RTC_year_w              flg_RTC.bits.bit4    //连续写入的年的标志
                #define   bflg_RTC_month_w             flg_RTC.bits.bit5    //连续写入的月的标志
                #define   bflg_RTC_day_w               flg_RTC.bits.bit6    //连续写入的日的标志
                #define   bflg_RTC_hour_w              flg_RTC.bits.bit7    //连续写入的时的标志
                #define   bflg_RTC_minute_w            flg_RTC.bits.bit8    //连续写入的分的标志 
                #define   bflg_RTC_second_w            flg_RTC.bits.bit9    //连续写入的秒的标志 

                #define   bflg_RTC_continuous_r_start  flg_RTC.bits.bit10
                #define   bflg_RTC_year_r              flg_RTC.bits.bit11
                #define   bflg_RTC_month_r             flg_RTC.bits.bit12
                #define   bflg_RTC_day_r               flg_RTC.bits.bit13
                #define   bflg_RTC_hour_r              flg_RTC.bits.bit14
                #define   bflg_RTC_minute_r            flg_RTC.bits.bit15
                
extern flag_type flg_RTC_1;

                #define   bflg_RTC_second_r            flg_RTC_1.bits.bit0
                #define   bflg_RTC_continuous_r_ok     flg_RTC_1.bits.bit1   //连续读取完成,用于密钥模式下生成密钥
                #define   bflg_RTC_enable_read         flg_RTC_1.bits.bit2   //使能读，打开定时器中断
                #define   bflg_RTC_enable_write        flg_RTC_1.bits.bit3   //使能写，打开定时器中断







#endif
/****************************************END OF THE FILE******************************************/
