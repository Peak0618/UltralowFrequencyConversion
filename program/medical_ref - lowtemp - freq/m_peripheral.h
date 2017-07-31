#ifndef _M_PERIPHERAL_H_
#define _M_PERIPHERAL_H_

//函数
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

/*  毛细管不使用了
void heat20_force_ctrl_deal(void);
void heat20_space_delaytime(void);
void heat20_ctrl_deal(void)   ;
void heat20_wait_delaytime(void);
void heat20_comp2_stop_delaytime(void);  
void heat20_run_delaytime(void);
void heat20_stop_delaytime(void);
*/

//macro
#define   RED_LED_PIN        P8.2      //红色灯_   ___低电平亮
#define   GREEN_LED_PIN      P8.1      //绿色灯
#define   BLUE_LED_PIN       P8.0      //蓝色灯
#define   RL_HTRGK_OUT_PIN   P4.1      //柜口加热丝继电器输出口  原来P13.0
#define   HTRMT_OUT_PIN      P11.1     //门体加热丝输出口   
#define   HTRMXG_OUT_PIN     P14.6     //毛细管加热丝输出口 
#define   HTRPHK_OUT_PIN     P14.7     //平衡口加热丝输出口 
#define   BAT_DISCHARGE_PIN  P1.7      //电池供电输出口     
#define   RL_COMP1_OUT_PIN   P7.4      //压缩机1继电器输出口     
#define   RL_COMP2_OUT_PIN   P7.5      //压缩机2继电器输出口      
          
#define   RL_FAN1_OUT_PIN    P7.3      //风机1继电器输出口           RL7  bflg_outfan20_running
#define   RL_FAN2_OUT_PIN    P4.2      //风机2继电器输出口         
          
#define   RL_HTRMXG_OUT_PIN  P13.0     //毛细管加热丝继电器输出口 原来P4.1
#define   RL_CON_PIN         P1.5      //电池充电控制输出口    // peak 控制电池的充电脚

#define   DOOR1_IN_PIN       P5.2      //门开关1输入口           
#define   BAT_FED_IN_PIN     P0.5      //电池插反输入口  

#define   TRIP_LAMP_PIN      P7.7      //故障灯输出口       
#define   TFT_OUT_PIN        P10.1     //液晶屏电源输出口_寄存器默认配置高，输出
#define   RL_DOWN_OUT_PIN    P15.6     //降压继电器输出口          
#define   RL_UP_OUT_PIN      P10.0     //升压继电器输出口          



//flag
extern flag_type flg_peripheral_1;          
        #define   bflg_outfan20_running         flg_peripheral_1.bits.bit0      //冷凝风机1开标志
        #define   bflg_outfan21_running         flg_peripheral_1.bits.bit1      //冷凝风机2开标志
        #define   bflg_tank_mouth_run           flg_peripheral_1.bits.bit2      //柜口加热丝延时
        #define   bflg_heat21_running           flg_peripheral_1.bits.bit3      //平衡口加热丝开标志
        #define   bflg_heat22_running           flg_peripheral_1.bits.bit4      //门体加热丝开标志
        #define   bflg_RL_output_delaytime      flg_peripheral_1.bits.bit5      //继电器输出延时标志

        
extern flag_type flg_heat2;          
        #define   bflg_heat20_first_run         flg_heat2.bits.bit0      //毛细管加热丝首次开标志
        #define   bflg_heat20_force_run         flg_heat2.bits.bit1      //毛细管加热丝强制加热标志  //peak 0自动加热 1强制(手动)  强制在通讯中接收数据设定
        #define   bflg_heat20_space_delaytime   flg_heat2.bits.bit2      //毛细管加热丝间隔延时标志  //peak 置0时毛细管才能加热，无论何种情况，只要毛细管加热后都会置1即再等16小时，置0的几种情况:                                                                                            // 1、强制毛细管加热  2、间隔16小时到毛细管自动加热
        #define   bflg_heat20_askfor_run        flg_heat2.bits.bit3      //毛细管加热丝请求开标志    //peak  有请求的几种情况: 1、强制开 2、出上电时PT100<0度  3、每间隔16小时的自动加热                                                                                                  //bflg_heat20_comp2_stop = 1; 也肯定置1
        #define   bflg_heat20_wait_delaytime    flg_heat2.bits.bit4      //毛细管加热丝等待延时标志  //peak如果压缩机连续 1 小时（ EEP） 都未停机， 则强制高温、 低温压缩机停机,停则清0                                                                                            //压缩机一直未停机，则一小时后 置 bflg_heat20_comp2_stop = 1;  关闭压缩机打开毛细管 
        #define   bflg_heat20_comp2_stop        flg_heat2.bits.bit5      //毛细管加热丝使压缩机关标志        //peak 关掉的目的---开毛细管。  关掉几种情况: 1、强制加热 2、初始上电时PT100小于0度 3、16小时间隔到 4、有毛细加热请求
        #define   bflg_heat20_comp2_stop_delaytime   flg_heat2.bits.bit6 //毛细管加热丝使压缩机关延时标志    //peak 毛细管使压缩机关闭，在压缩机关闭3分钟后 此标志=0 时 毛细管才能打开                                                                                                     //只要是关机就将此位置1 ，判断是否达到关机3分钟了，即使此时没有加热丝加热条件也没事，是为要是此时有加热丝请求时准备的
        #define   bflg_heat20_running           flg_heat2.bits.bit7      //毛细管加热丝开标志
        #define   bflg_heat20_run_delaytime     flg_heat2.bits.bit8      //毛细管加热丝开延时标志       //毛细管加热一段时间后，就关闭
        #define   bflg_heat20_stop_delaytime    flg_heat2.bits.bit9      //毛细管加热丝关延时标志









#endif 
/****************************************END OF THE FILE***********************************************/

