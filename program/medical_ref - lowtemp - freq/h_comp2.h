#ifndef _COMP2_H_
#define _COMP2_H_
//------------------------------------------------------------------------------
//外部函数声明
extern void comp2_power_on_delaytime(void);   
extern void comp2_run_stop_delaytime(void);   
extern void comp2_ctrl_deal(void);      
extern void comp2_run_delaytime(void);   
extern void comp2_stop_delaytime(void); 
extern void comp2_TA_fault_run_delaytime(void);   
extern void comp2_TA_fault_stop_delaytime(void);   
extern void comp2_continue_run_delaytime(void);    
extern void comp2_force_stop_delaytime(void); 
extern void comp2_run_space_delaytime(void);  
extern void comp21_ctrl_deal(void); 
//------------------------------------------------------------------------------
extern void comp2_start_delaytime(void);
extern void comp2_update_delaytime(void);
extern void comp21_start_delaytime(void);
extern void comp21_update_delaytime(void);
extern void comp2_power_down_delaytime(void);
extern void freq_ctrl_deal(void);
extern void init_freq_ctrl(void);

//------------------------------------------------------------------------------
//外部变量声明
extern flag_type flg_comp2;
          
          #define   bflg_comp2_first_run          flg_comp2.bits.bit0 //首次运行标志
          #define   bflg_comp2_power_on_delaytime flg_comp2.bits.bit1 //上电延时标志
          #define   bflg_comp2_askfor_run         flg_comp2.bits.bit2 //高温压缩机请求开机标志    // peak达到功能书的开机条件  //peak承接comp2_run_stop_delaytime()函数中的标志进行的；
          #define   bflg_comp2_askfor_stop        flg_comp2.bits.bit3 //高温压缩机请求关机标志    //peak达到功能书的关机条件
          
          #define   bflg_comp2_running            flg_comp2.bits.bit4 //高温压缩机运行标志  peak 0 关机 1 开机
          
          #define   bflg_comp2_run_delaytime      flg_comp2.bits.bit5 //高温压缩机开机延时标志   //开机 10分钟 才能停机
          #define   bflg_comp2_stop_delaytime     flg_comp2.bits.bit6 //高温压缩机关机延时标志   //关机8分钟才能再开机
          
          #define   bflg_comp2_TA_fault_run_delaytime      flg_comp2.bits.bit7  //主传感器故障高温压缩机开机延时标志    //peak 主传感器有故障开30分钟后关机
          #define   bflg_comp2_TA_fault_stop_delaytime     flg_comp2.bits.bit8  //主传感器故障高温压缩机关机延时标志    //peak 主传感器有故障关10分钟后开机
          #define   bflg_comp2_continue_run_delaytime      flg_comp2.bits.bit9  //高温压缩机连续运行标志               //peak 功能书要求连续运行一段时间后要强制关机 初始上电和之后连续时间不同
          #define   bflg_comp2_force_stop_delaytime        flg_comp2.bits.bit10 //高温压缩机强制停止标志               //peak 也是停机一段时间后，要是有需要开机的才能开
          #define   bflg_comp2_run_space_delaytime         flg_comp2.bits.bit11 //高温压缩机运行间隔延时标志   //peak 备注1 功能书 高温压缩机开机延时1或10(初始上电)后，再开低温压缩机 
          #define   bflg_comp21_running                    flg_comp2.bits.bit12 //低温压缩机运行标志
//------------------------------------------------------------------------------
extern flag_type flg_freq;
          #define   bflg_comp2_start_delaytime    flg_freq.bits.bit0
          #define   bflg_comp2_update_delaytime   flg_freq.bits.bit1
          #define   bflg_comp21_start_delaytime   flg_freq.bits.bit2
          #define   bflg_comp21_update_delaytime  flg_freq.bits.bit3
          
          #define   bflg_comp21_door_freq         flg_freq.bits.bit4   //peak 检测到开门1s  低温压机开时 就清0,新加断电清0
          #define   bflg_comp21_door_freq_tmp     flg_freq.bits.bit5   //peak 开门1s，当PT100温度大于设定温度5℃时 清0；
                                                                       //或PT100温度小于设定温度时  清0。  新加断电清0
          #define   bflg_comp2_large_freq         flg_freq.bits.bit6
          #define   bflg_power_down_delaytime     flg_freq.bits.bit7   //断电后重新上电时高温压机延时
          #define   bflg_comp2_first_power_on     flg_freq.bits.bit8   //初次上电时高温压机以4500启动标志

//------------------------------------------------------------------------------
extern int16_t   gss_para[27];
#define   COMP2_START_FREQ         0  //2000   P01
#define   COMP2_START_TIME         1  //60     P02
#define   COMP2_UPDATE_TIME        2  //80     P03
#define   COMP2_SET_TEMP1          3  //-30    ...
#define   COMP2_FREQ_DELT_K1       4  //50
#define   COMP2_FREQ_DELT_MIN      5  //-20
#define   COMP2_FREQ_DELT_MAX      6  //20
#define   COMP2_FREQ_MIN           7  //2000
#define   COMP2_FREQ_MAX           8  //4500
#define   COMP2_SET_TEMP2          9  //-30
#define   COMP2_FREQ_DELT_K2       10 //50
#define   COMP21_START_FREQ        11 //2000
#define   COMP21_START_TIME        12 //60
#define   COMP21_UPDATE_TIME       13 //80
#define   COMP21_FREQ_DELT_K       14 //50
#define   COMP21_FREQ_DELT_MIN     15 //-20
#define   COMP21_FREQ_DELT_MAX     16 //20
#define   COMP21_FREQ_MIN          17 //2000
#define   COMP21_FREQ_MAX          18 //4500
#define   COMP21_FREQ_DELT_TEMP    19 //0
#define   COMP_START_DELT_TEMP     20 //开机点+的值
#define   COMP_STOP_DELT_TEMP      21 //停机点+的值
#define   PT100_DELT_TEMP          22 //0
#define   COMP_STOP_DELAYTIME      23 //300
#define   COMP_DOOR_TDELT          24 //50
#define   COMP_DOOR_FREQ           25 //3500
#define   COMP_RUN_DELAYTIME       26 //300


extern int16_t   gss_comp2_freq;   //高温压机频率
extern int16_t   gss_comp21_freq;  //低温压机频率
extern int16_t   gss_comp2_power_on_delaytimer;   //上电延时定时器

extern uint16_t  gus_comp2_freq_cnt;
extern uint16_t  gus_comp21_freq_cnt;
extern int8_t    gss_power_down_delaytime;               
extern uint8_t   guc_comp2_power_on_again;



//------------------------------------------------------------------------------
#endif
