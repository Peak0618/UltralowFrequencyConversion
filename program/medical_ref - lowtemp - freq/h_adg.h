#ifndef _ADG_H_
#define _ADG_H_
//------------------------------------------------------------------------------
//外部函数声明
extern void adg_init(void);        //ad模块初始化程序，在初始化程序中调用

extern void adg_data_delaytime(void);   //ad模块数据处理延时程序，在100ms定时程序中调用

extern void adg_temp_calc(void);   //ad模块温度计算处理程序，在主循环程序中调用

extern void adg_iic_int(void);     //ad模块iic模拟中断程序，在定时中断调用

//------------------------------------------------------------------------------
extern flag_type flg_adg;
          #define   bflg_adg_reset           flg_adg.bits.bit0   //ad模块复位标志
          #define   bflg_adg_channel         flg_adg.bits.bit1   //ad模块通道标志
          #define   bflg_write_config_adg    flg_adg.bits.bit2   //ad模块写配置标志
          #define   bflg_write_read_adg      flg_adg.bits.bit3   //ad模块读写标志
          #define   bflg_adg_data_ok         flg_adg.bits.bit4   //ad模块数据ok标志
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
