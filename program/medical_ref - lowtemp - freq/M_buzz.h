#ifndef _M_BUZZ_H_
#define _M_BUZZ_H_

/****************************************************************************************************
  输入接口:
            短滴循环: bflg_buzz_loop_tick_set       
            响一声  : bflg_buzz_one_tick_set        ----"电池未连接" 时长按"蜂鸣取消" 5后；
            长响    : bflg_buzz_always_sound_set    ----只在电池插反用

            原来都是--- bflg_alarm_buzz


 输出函数:  buzz_one_tick_delaytime();   都放在100ms 中
            buzz_loop_tick_delaytime();
            buzz_sound_priority();        

****************************************************************************************************/

//外部函数声明
extern void buzz_loop_tick_delaytime(void);   //  滴滴声延时程序
extern void buzz_one_tick_delaytime(void);    //  响一声延时
extern void buzz_sound_priority(void);          // 不同响声的优先级
extern void buzz_sound_cnt(void);                 //自定义响多少声




//外部标志声明

extern flag_type flg_buzz;
          
          #define   bflg_buzz_loop_tick_output     flg_buzz.bits.bit0   
          #define   bflg_buzz_loop_tick_set        flg_buzz.bits.bit1   //蜂鸣器滴滴响的标志
          #define   bflg_buzz_one_tick_output      flg_buzz.bits.bit2  
		  
          #define   bflg_buzz_one_tick_set         flg_buzz.bits.bit3    //蜂鸣器响一声标志
		  
          #define   bflg_buzz_always_sound_set     flg_buzz.bits.bit4  
          #define   bflg_buzz_always_sound_output  flg_buzz.bits.bit5
          #define   bfla_buzz_always_sound_run     flg_buzz.bits.bit6    // 蜂鸣器已经运行，不能再开蜂鸣器，否则断续响

          #define   bflg_buzz_one_tick_run         flg_buzz.bits.bit7   //同上
          #define   bflg_buzz_loop_tick_runing     flg_buzz.bits.bit8
          #define   bflg_buzz_one_tick_stop        flg_buzz.bits.bit9

//外部变量声明
extern uint8_t   guc_buzz_sound_cnt;                //设置要响的次数














#endif
/****************************************END OF THE FILE******************************************/
