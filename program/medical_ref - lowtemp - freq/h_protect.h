#ifndef _PROTECT_H_
#define _PROTECT_H_
//------------------------------------------------------------------------------
//外部函数声明

extern void prot2_TA_first_delaytime(void);  //主传感器首次高低温保护延时程序，在1min定时程序中调用

extern void prot2_TA_high_delaytime(void);   //主传感器高温保护延时程序，在1s定时程序中调用

extern void prot2_TA_low_delaytime(void);    //主传感器低温保护延时程序，在1s定时程序中调用

extern void prot2_power_off_deal(void);      //断电保护处理程序，在主循环程序中调用

extern void port2_power_fault_deal(void);    //电源板故障处理程序，在主循环程序中调用

extern void prot2_bat_discnnt_delaytime(void);    //电池未连接保护延时程序，在100ms定时程序中调用

extern void prot2_bat_space_delaytime(void);      //电池未连接检测间隔延时程序，在100ms定时程序中调用

extern void prot2_bat_low_deal(void);   //电池电量低保护处理程序，在主循环程序中调用

extern void prot2_bat_low_delaytime(void);   //电池电量低延迟程序，在1min定时程序中调用

extern void port2_door_delaytime(void);      //门开关保护延时程序，在1s定时程序中调用

extern void port2_T_fault_deal(void);        //传感器故障处理程序，在主循环程序中调用

extern void prot2_THW_high_delaytime(void);  //环温传感器过高保护延时程序，在1s定时程序中调用

extern void prot2_TC_first_delaytime(void);  //冷凝器脏首次保护延时程序，在1min定时程序中调用

extern void prot2_cond_dirty_delaytime(void);//冷凝器脏保护延时程序，在1s定时程序中调用

extern void prot2_volt_deal(void);      //电压超标报警保护处理程序，在主循环程序中调用

extern void alarm_lamp_deal(void);      //报警灯处理程序，在主循环程序中调用

extern void alarm_buzz_deal(void);      //蜂鸣器报警处理程序，在主循环程序中调用

extern void alarm_buzz_off_delaytime(void);       //蜂鸣器报警取消延时程序，在1min定时程序中调用

extern void bat_T_hight_protect(void);               //电池温度高判断程序

extern void alarm_power_off(void);

//------------------------------------------------------------------------------
//外部标志声明
extern flag_type flg_alarm;                  // 这些报警是滴滴响
          #define   bflg_alarm_ALH           flg_alarm.bits.bit0    //高温报警标志          
          #define   bflg_alarm_ALL           flg_alarm.bits.bit1    //低温报警标志         
          //#define   bflg_alarm_power_off     flg_alarm.bits.bit2    //断电报警标志         
          #define   bflg_alarm_bat_discnnt   flg_alarm.bits.bit2    //电池未连接报警标志   
          #define   bflg_alarm_T_fault	     flg_alarm.bits.bit3    //传感器故障报警标志: 主、环温、冷凝、热交换这4种传感器任意一种有故障这个就有故障
          #define   bflg_alarm_cond_dirty    flg_alarm.bits.bit4    //冷凝器脏报警标志	 
          #define   bflg_alarm_open_door	 flg_alarm.bits.bit5    //开门报警标志		 
          #define   bflg_THW_fault           flg_alarm.bits.bit6    //环温传感器故障标志
          #define   bflg_TC_fault            flg_alarm.bits.bit7    //冷凝器温度传感器故障标志
          #define   bflg_TE_fault            flg_alarm.bits.bit8    //热交换器温度传感器故障标志
          #define   bflg_TA_fault            flg_alarm.bits.bit9    //主传感器故障标志          
          //不能在添加其他位了，因为使用了 flg_alarm.uword
          //--xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//------------------------------------------------------------------------------
extern flag_type flg_alarm1;     //上面的3个报警灯指示，    因为没有滴滴响所以没有和上面的合并到一块，因为上面用到了.uword
         #define   bflg_alarm_bat_fed           flg_alarm1.bits.bit0   //电池接反报警标志    长响
         #define   bflg_alarm_bat_low           flg_alarm1.bits.bit1   //电池电量低报警标志  不响
         #define   bflg_alarm_THW_high          flg_alarm1.bits.bit2   //环温过高报警标志    不响
         #define   bflg_alarm_power_off         flg_alarm1.bits.bit3   //断电报警标志--蜂鸣报警特别，单独处理了     
         //不能在添加其他位了，因为使用了 flg_alarm1.uword
         //--xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

         
//------------------------------------------------------------------------------
extern flag_type flg_alarm2;  //因为去掉了这两个报警，但是程序中确判断了，所以单独出来；

        #define   bflg_alarm_power_fault   flg_alarm2.bits.bit0       //电源板故障报警标志   
        #define   bflg_alarm_volt          flg_alarm2.bits.bit1       //电压超标报警标志  
        #define   bflg_alarm_bat1_T_hight  flg_alarm2.bits.bit2       //电池1温度过高       
        #define   bflg_alarm_bat2_T_hight  flg_alarm2.bits.bit3       //电池2温度过高


//------------------------------------------------------------------------------
extern flag_type flg_prot2;
          
          #define   bflg_alarm_lamp          flg_prot2.bits.bit0 //报警灯标志
          #define   bflg_alarm_buzz          flg_prot2.bits.bit1 //蜂鸣器报警标志
          #define   bflg_alarm_buzz_output   flg_prot2.bits.bit2 //蜂鸣器输出标志
          #define   bflg_alarm_output        flg_prot2.bits.bit3 //远程报警输出标志
          
          #define   bflg_prot2_door1_pin     flg_prot2.bits.bit4 //门开关1引脚状态标志
          #define   bflg_prot2_door2_pin     flg_prot2.bits.bit5 //门开关2引脚状态标志
          #define   bflg_prot2_bat_fed_pin   flg_prot2.bits.bit6 //锂电池防插反标志
          
          #define   bflg_prot2_TA_first_delaytime flg_prot2.bits.bit7 //主传感器首次保护延时标志
          #define   bflg_prot2_TC_first_delaytime flg_prot2.bits.bit8 //冷凝器脏首次保护延时标志
          
          #define   bflg_alarm_buzz_off_delaytime flg_prot2.bits.bit9 //蜂鸣器报警取消延时标志  =1表示已按下"蜂鸣取消"键可30min内不响
          
          //#define   bflg_prot2_THW_high_disp      flg_prot2.bits.bit10//环温过高温度显示标志
          
          #define   bflg_prot2_bat_low_delaytime         flg_prot2.bits.bit11   //电池电量低延时标志    //peak 初始上电对电池充电24小时；
          #define   bflg_prot2_bat_space_delaytime       flg_prot2.bits.bit12   //电池未连接检测间隔延时标志
          #define   bflg_prot2_bat_space_reach           flg_prot2.bits.bit13   //电池未连接检测间隔延时到标志

		  /****peak add*********/
          #define   bflg_alarm_bat_low_led               flg_prot2.bits.bit14  // 检测到电池电量低时，指示灯只是闪烁，不蜂鸣；
          #define   bflg_power_off_alarm                 flg_prot2.bits.bit15  //断电时蜂鸣器报警间隔延时 



//------------------------------------------------------------------------------
//macro
//gus_trip_code1
#define   H_T_CODE       0x0001   //bit0  高温报警
#define   L_T_CODE       0x0002   //bit1  低温报警
#define   VAC_OFF_CODE   0x0004   //bit2  断电报警
#define   DOOR_OP_CODE   0x0008   //bit3  开门报警
#define   THW_S_CODE     0x0010   //bit4  环温传感器故障 S = sensor
#define   TA_S_CODE      0x0020   //bit5  主传感器故障
#define   TE_S_CODE      0x0040   //bit6  热交换器故障
#define   TC_S_CODE      0x0080   //bit7  冷凝器传感器故障
#define   BACK_S_CODE    0x0100   //bit8  后备系统传感器故障
#define   TC_COND_CODE   0x0200   //bit9  冷凝器脏
#define   THW_H_CODE     0x0400   //bit10 环温过高
#define   BAT_FED_CODE   0x0800   //bit11 电池插反
#define   BAT_NO_CODE    0x1000   //bit12 电池未连接
#define   BAT_L_CODE     0x2000   //bit13 电池电量低
#define   DC_F_CODE      0x4000   //bit14 电源板故障
#define   VAC_OVER_CODE  0x8000   //bit15 电压超标

//gus_trip_code2
#define   BACK_COM_CODE  0x0001   //bit0 后备系统通信故障
#define   BACK_L_CODE    0x0002   //bit1 后备系统冷媒不足报警


//------------------------------------------------------------------------------
extern int16_t   gss_prot2_TA_first_delaytimer;   //主传感器首次保护延时计时器
extern int16_t   gss_prot2_TC_first_delaytimer;   //冷凝器脏首次保护延时计时器

extern int16_t   gss_prot2_bat_space_delaytimer;  //电池未连接检测间隔延时计时器

extern int16_t   gss_prot2_bat_low_delaytimer;    //电池电量低延时计时器

extern int16_t   gss_alarm_buzz_off_delaytimer;   //蜂鸣器报警取消延时计时器

extern uint16_t  gus_trip_code1;   //故障代码1
extern uint16_t  gus_trip_code2;   //故障代码2

extern uint8_t   guc_power_down_lfreq_cnt;  //断电后上电低压机已4500启动
extern uint8_t   guc_power_down_hdelay_cnt; //断电后上电高压机延时1min启动


//------------------------------------------------------------------------------
#endif
