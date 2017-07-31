#ifndef _VOLT_CTRL_H_
#define _VOLT_CTRL_H_
//------------------------------------------------------------------------------
//外部函数声明
extern void comp2_volt_low_delaytime(void);  //高温压机电压过低延时程序，在1s定时程序中调用

extern void volt_ctrl_delaytime(void);  //电压控制延时程序，在1s定时程序中调用

extern void comp2_volt_delaytime(void); //高温压缩机电压控制延时程序，在1s定时程序中调用

extern void comp21_volt_delaytime(void);//低温压缩机电压控制延时程序，在1s定时程序中调用

//------------------------------------------------------------------------------
//外部变量声明
extern flag_type flg_volt;
          
          #define   bflg_comp2_volt_type          flg_volt.bits.bit0  //电压类型标志  通过拨码开关选择
          #define   bflg_comp2_volt_low           flg_volt.bits.bit1  //电压过低标志  0 无过低 1电压过低
          
          #define   bflg_comp2_volt_down          flg_volt.bits.bit2  //降压标志
          #define   bflg_comp2_volt_up            flg_volt.bits.bit3  //升压标志

          #define   bflg_comp2_volt_delaytime     flg_volt.bits.bit4  //高温压缩机电压控制延时标志     //peak 初始上电时，等到高温压机运行7min 后才对升降压进行判断，现在不用声降压了，但在压机启动时也将其置1，没有将其注释掉；
          #define   bflg_comp2_volt_first_end     flg_volt.bits.bit5  //高温压缩机首次电压控制结束标志
          
          #define   bflg_comp21_volt_delaytime    flg_volt.bits.bit6  //低温压缩机电压控制延时标志    //同上 不使用了
          #define   bflg_comp21_volt_first_end    flg_volt.bits.bit7  //低温压缩机首次电压控制结束标志
          
//------------------------------------------------------------------------------
extern int16_t gss_comp2_volt_delaytimer;    //高温压缩机电压控制延时计时器
extern int16_t gss_comp21_volt_delaytimer;   //低温压缩机电压控制延时计时器

//------------------------------------------------------------------------------
#endif
