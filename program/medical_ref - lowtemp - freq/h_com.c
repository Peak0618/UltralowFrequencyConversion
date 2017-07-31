#ifndef _COM_C_
#define _COM_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

#include "h_type_define.h"
#include "h_com.h"
#include "h_ad.h"
#include "h_key_operation.h"
#include "h_rom_para2.h"
#include "h_comp2.h"
#include "h_protect.h"
#include "h_volt_ctrl.h"
#include "h_bat_ctrl.h"

//peak 
#include "m_peripheral.h"
#include "h_main.h"
#include "M_RTC.h"
#include "M_buzz.h"
//------------------------------------------------------------------------------
//函数声明
void com_init(void);//通讯初始化程序，在初始化程序中调用

void com_tx_init(void);  //通讯发送初始化程序，在主循环程序中调用

void com_rx_init(void);  //通讯接收初始化程序，在主循环程序中调用

void com_rx_data_deal(void);  //通讯接收数据处理程序，在主循环程序中调用

void com_tx_delaytime(void);  //通讯发送延时程序，在1ms定时程序中调用

void com_rx_delaytime(void);  //通讯接收延时程序，在1ms定时程序中调用

void com_rx_end_delaytime(void);   //通讯接收完成延时程序，在1ms定时程序中调用

void com_fault_delaytime(void);    //通讯故障延时程序，在1s定时程序中调用

void com_tx_int(void);   //通讯发送中断程序

void com_rx_int(uint8_t rx_data);  //通讯接收中断程序

unsigned int CRC16(unsigned char *puchmsg, unsigned int usDataLen);    //进行CRC校验的程序
void com_tx_usb_rank(void);
void com_usb_delay_time(void);  // 1s延时函数中 --未


//------------------------------------------------------------------------------
//标志定义
flag_type flg_com;
flag_type flg_read_write,flg_sent_usb_control,flg_sent_usb_event,flg_sent_usb_event_1,
          flg_usb_event_temp,flg_usb_event_temp_1,flg_usb_event_attribute;

//------------------------------------------------------------------------------
//变量定义
uint8_t   guc_com_tx_buffer[120];        //通讯发送缓存器
uint8_t   guc_com_rx_buffer[80];         //通讯接收缓存器

int16_t   gss_com_tx_delaytimer;        //通讯发送延时计时器
int16_t   gss_com_rx_delaytimer;        //通讯接收延时计时器

int16_t   gss_com_rx_end_delaytimer;    //通讯接收完标志
int16_t   gss_com_fault_delaytimer;     //通讯故障延时计时器

uint8_t   guc_com_tx_point;   //通讯发送索引
uint8_t   guc_com_rx_point;   //通讯接收索引

uint8_t   guc_com_type_cnt;   //通讯类型计数器

//peak add
uint8_t  guc_IC_access_state = 0;    // 接收的IC卡的指令

uint16_t guc_flash_add;              // 接收的IC卡注册已用的地址，用来显示有多少张卡注册
uint16_t guc_temp_flash_add;         // 接收的IC卡注册临时保存的地址，用来和现在的地址对比是否有新的IC卡注册，锁屏时间好重重新
uint8_t  guc_IC_erasure_tx_ok_count; // 注销IC卡指令，在bflg_com_tx_ok中设置发送多次计数,防止丢包

uint8_t gss_usb_delaytime;
uint8_t guc_U_disk_state;           //显示板发送过来的U盘状态
uint8_t guc_usb_state;              //显示板发送过来的usb板的状态

#define   WRITE_STATE_CNT     50

#define   READ_USER_CNT       30
#define   READ_ADMIN_CNT      30


/******************************************************************************************************************************
函数功能:  初始化时(仅一次)，刚上电为主机，发送数据
    
函数位置：系统初始化-----------------------------------------ok
******************************************************************************************************************************/
void com_init(void) //通讯初始化程序，在初始化程序中调用
{
    bflg_com_tx_delaytime = 1;     //peak 延时时间到，打开发送中断，发送数据；
    gss_com_tx_delaytimer = 3000;
    
    bflg_com_rx_delaytime = 0;
    gss_com_rx_delaytimer = 0;
    
    bflg_read_write_data = 1; //首次写数据
    
    //bflg_read_write_data = 0; //debug 首次读数据
    //bflg_read_user_admin = 1; 
}
//------------------------------------------------------------------------------
/*******************************************************************************
function: 在每一次发送数据时，对其初始化；
         LED: 发送显示板上所有要显示的内容(txt)

         初始化内容:
           1、发送的查询指令(固定)
           2、发送的设定数据---->|a、 "工作状态"、"报警"、"故障指示"、"电压"、"环温"
                                 |b、"温度设定值" "高温报警设定值" "低温报警设定值"

           a---根据主控板本身的数据采集来判断；
           
           b---是显示板按键设置好的值； 过程如下
               显示板按键设置后--->发送给主板-->主板再将此值发送给显示板显示；  
               
********************************************************************************/
void com_tx_init(void)   //通讯发送初始化程序，在主循环程序中调用
{
    int8_t i;
    word_type gss_tmp;
    word_type com_crc;
    //----------------------------------------------------------LED屏-----------------------------------
    //dubug
    //bflg_com_type = 1;
    
    if (bflg_com_type == 0)   //如果是LED显示屏通讯
    {
        if (bflg_read_write_data == 0)  //如果是读数据  //peak 读按键值
        {
            guc_com_tx_buffer[0] = 0x00;     //设备地址
            guc_com_tx_buffer[1] = 0x03;     //读命令
            
            guc_com_tx_buffer[2] = 0x00;
            guc_com_tx_buffer[3] = 0x07;     //起始地址7
            
            guc_com_tx_buffer[4] = 0x00;
            guc_com_tx_buffer[5] = 2;        //地址个数
            //----------------------------------------------
            com_crc.uword = CRC16(guc_com_tx_buffer, 6);
            guc_com_tx_buffer[6] = com_crc.ubyte.low;
            guc_com_tx_buffer[7] = com_crc.ubyte.high;
        }
        else   //如果是写数据   //peak 设定数码管、LED显示内容
        {
            guc_com_tx_buffer[0] = 0x00;     //设备地址
            guc_com_tx_buffer[1] = 0x10;     //写命令
            
            guc_com_tx_buffer[2] = 0x00;
            guc_com_tx_buffer[3] = 0x00;     //起始地址0
            
            guc_com_tx_buffer[4] = 0x00;
            guc_com_tx_buffer[5] = 22;   //7     //地址个数  peak 要写几个寄存器，每个寄存器占2个字节；7*2 = 14 个字节；
            
            guc_com_tx_buffer[6] = 44;   //14     //字节数    peak [7]--[20] = 14 具体的内容

            for (i = 0; i < 12; i++)         // peak [7]~[18] = 12 ;
            {
                guc_com_tx_buffer[i + 7] = guc_led_buffer[i];    //peak 数码管寄存器
            }           
            
            //----------------------------------------------
            if(bflg_IC_erasure_order == 1)  //如有注销指令,注销IC卡指令_在bflg_com_tx_ok中设置发送多次计数,防止丢包
            {               
                guc_com_tx_buffer[19] = 1;
            }
            else
            {
                guc_com_tx_buffer[19] = 0;
            }
            
            if (bflg_alarm_lamp == 0)         //peak 显示板上的三位报警灯 
            {
                //guc_com_tx_buffer[19] = 0;
                guc_com_tx_buffer[20] = 0x00;
            }
            else
            {
                //guc_com_tx_buffer[19] = 0;
                guc_com_tx_buffer[20] = 0x07;     //peak 显示板上面的 "报警" 的三个灯 
            }
            
            //--------------------------------------------------------------------------------------------
            //usb使用的数据帧
            guc_com_tx_buffer[21] = (uint8_t)(gss_set_parm_value[DISP_SET_TS]*10);         //箱内设定温度低8位  放大100，但TS已放大过10了
            guc_com_tx_buffer[22] = (uint8_t)((gss_set_parm_value[DISP_SET_TS]*10) >> 8);
            guc_com_tx_buffer[23] = (uint8_t)(gss_TA*10);               //箱内温度
            guc_com_tx_buffer[24] = (uint8_t)((gss_TA*10) >> 8);
            guc_com_tx_buffer[25] = (uint8_t)(gss_set_parm_value[DISP_SET_ALH]*10);        //高温报警值
            guc_com_tx_buffer[26] = (uint8_t)((gss_set_parm_value[DISP_SET_ALH]*10) >> 8);
            guc_com_tx_buffer[27] = (uint8_t)(gss_set_parm_value[DISP_SET_ALL]*10);        //低温报警值
            guc_com_tx_buffer[28] = (uint8_t)((gss_set_parm_value[DISP_SET_ALL]*10) >> 8); 
            guc_com_tx_buffer[29] = (uint8_t)(gss_THW*100);              //环温值
            guc_com_tx_buffer[30] = (uint8_t)((gss_THW*100) >> 8);
            guc_com_tx_buffer[31] = (uint8_t)(gss_VAC*100);              //电压值
            guc_com_tx_buffer[32] = (uint8_t)((gss_VAC*100) >> 8); 

            //--------------------------------------------------------------------------------------------
            //事件帧 
            if(bflg_sent_usb_ALH == 1)
            {
                bflg_sent_usb_ALH = 0;
                guc_com_tx_buffer[20] |= 0x80;  //[20]的第7位代表是否有事件帧的标志
                
                guc_com_tx_buffer[33] = 0x00;                      //事件类代码  
                guc_com_tx_buffer[34] = (uint8_t)(gss_TA*10);      //事件值1 (温湿度、电压值需*100,因为TA已放大10,小端)
                guc_com_tx_buffer[35] = (uint8_t)((gss_TA*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;                      //事件值2 需*100
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;
                /*if(bflg_usb_ALH_start == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;                  //事件属性
                }
                else
                {
                    guc_com_tx_buffer[38] = 0x02;      
                }*/
            }
            else if(bflg_sent_usb_ALL == 1)
            {
                bflg_sent_usb_ALL = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x01;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_TA*10);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_TA*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;
                /*if(bflg_usb_ALL_start == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;                 
                }
                else
                {
                    guc_com_tx_buffer[38] = 0x02;      
                }*/
            }
            else if(bflg_sent_usb_THW == 1)
            {
                bflg_sent_usb_THW = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x06;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_THW*100);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_THW*100)>>8);    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;
                
                /*if(bflg_usb_THW_start == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;                 
                }
                else
                {
                    guc_com_tx_buffer[38] = 0x02;      
                }  */
            }
            else if(bflg_sent_usb_ALHS == 1)
            {
                bflg_sent_usb_ALHS = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x10;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_set_parm_value[DISP_SET_ALH]*10);    
                guc_com_tx_buffer[35] = (uint8_t)((gss_set_parm_value[DISP_SET_ALH]*10)>>8);    
                guc_com_tx_buffer[36] = (uint8_t)(gss_ALH_old_value*10);    
                guc_com_tx_buffer[37] = (uint8_t)((gss_ALH_old_value*10)>>8);
                guc_com_tx_buffer[38] = 0x00;    
            }
            else if(bflg_sent_usb_ALLS == 1)
            {
                bflg_sent_usb_ALLS = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x11;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_set_parm_value[DISP_SET_ALL]*10);    
                guc_com_tx_buffer[35] = (uint8_t)((gss_set_parm_value[DISP_SET_ALL]*10)>>8);    
                guc_com_tx_buffer[36] = (uint8_t)(gss_ALL_old_value*10);    
                guc_com_tx_buffer[37] = (uint8_t)((gss_ALL_old_value*10)>>8);
                guc_com_tx_buffer[38] = 0x00;     
            }
            else if(bflg_sent_usb_TS == 1)
            {
                bflg_sent_usb_TS = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x14;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_set_parm_value[DISP_SET_TS]*10);    
                guc_com_tx_buffer[35] = (uint8_t)((gss_set_parm_value[DISP_SET_TS]*10)>>8);    
                guc_com_tx_buffer[36] = (uint8_t)(gss_TS_old_value*10);    
                guc_com_tx_buffer[37] = (uint8_t)((gss_TS_old_value*10)>>8);
                guc_com_tx_buffer[38] = 0x00;   
            }
            else if(bflg_sent_usb_DOR == 1)
            {
                bflg_sent_usb_DOR = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x20;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_TA*10);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_TA*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                if(bflg_usb_DOR_open == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;    
                }
                else
                {
                    guc_com_tx_buffer[38] = 0x02;    
                }
            }
            else if(bflg_sent_usb_POW == 1)  //只是上电/断电状态
            {
                bflg_sent_usb_POW = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x24;    
                guc_com_tx_buffer[34] = 0x00;    
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                if(bflg_usb_POW_on == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;    
                }
                else
                {
                    guc_com_tx_buffer[38] = 0x02;    
                }
            }
            else if(bflg_sent_usb_BAT == 1)
            {
                bflg_sent_usb_BAT = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x25;    
                guc_com_tx_buffer[34] = (uint8_t)(guc_bat_Vout*10);    
                guc_com_tx_buffer[35] = (uint8_t)((guc_bat_Vout*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                if(bflg_alarm_bat_discnnt == 1)
                {
                    guc_com_tx_buffer[38] = 0x01;   
                }
                else if(bflg_alarm_bat_fed == 1)
                {
                    guc_com_tx_buffer[38] = 0x02;   
                }
                else if(bflg_alarm_bat_low == 1)
                {
                    guc_com_tx_buffer[38] = 0x03;   
                }
                /*else if()//换电池
                {
                    guc_com_tx_buffer[38] = 0x04;   
                }*/
            }
            else if(bflg_sent_usb_FAT_THW == 1)    //以下4个为传感器
            {
                bflg_sent_usb_FAT_THW = 0;
                guc_com_tx_buffer[20] |= 0x80;  
                
                guc_com_tx_buffer[33] = 0x28;  
                guc_com_tx_buffer[34] = 0x12;     //根据功能书    
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;  
            }
            else if(bflg_sent_usb_FAT_TE == 1)
            {
                bflg_sent_usb_FAT_TE = 0;
                guc_com_tx_buffer[20] |= 0x80;    
                
                guc_com_tx_buffer[33] = 0x28;  
                guc_com_tx_buffer[34] = 0x11;   
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;  
            }
            else if(bflg_sent_usb_FAT_TC == 1)
            {
                bflg_sent_usb_FAT_TC = 0;
                guc_com_tx_buffer[20] |= 0x80;   
                
                guc_com_tx_buffer[33] = 0x28;  
                guc_com_tx_buffer[34] = 0x13; 
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;  
            }
            else if(bflg_sent_usb_FAT_TA == 1)
            {
                bflg_sent_usb_FAT_TA = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x28;  
                guc_com_tx_buffer[34] = 0x10;   
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;  
            }
            else if(bflg_sent_usb_IC == 1)
            {
                bflg_sent_usb_IC = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x2b;    
                guc_com_tx_buffer[34] = (uint8_t)(guc_flash_add>>2);       //guc_flash_add>>2 = 除以4，将地址转换为张数
                guc_com_tx_buffer[35] = (uint8_t)((guc_flash_add>>2)>>8);  
                guc_com_tx_buffer[36] = 0x00;   
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;    
            }
            else if(bflg_sent_usb_BAK == 1)
            {
                bflg_sent_usb_BAK = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x31;    
                guc_com_tx_buffer[34] = 0x00;    
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;//暂时自定    
            }
            else if(bflg_sent_usb_TC == 1)
            {
                bflg_sent_usb_TC = 0;
                guc_com_tx_buffer[20] |= 0x80; 
                
                guc_com_tx_buffer[33] = 0x32;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_TC*100);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_TC*100)>>8);    
                guc_com_tx_buffer[36] = (uint8_t)(gss_THW*100);    
                guc_com_tx_buffer[37] = (uint8_t)((gss_THW*100)>>8);
                guc_com_tx_buffer[38] = 0x00;    
            }
            else if(bflg_sent_usb_POW_OFF == 1)  //断电报警
            {
                bflg_sent_usb_POW_OFF = 0;
                guc_com_tx_buffer[20] |= 0x80; 

                guc_com_tx_buffer[33] = 0x33;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_TA*10);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_TA*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;                 
            }
            else if(bflg_sent_usb_DOOR_OPEN == 1)  //开门报警
            {
                bflg_sent_usb_DOOR_OPEN = 0;
                guc_com_tx_buffer[20] |= 0x80; 

                guc_com_tx_buffer[33] = 0x34;    
                guc_com_tx_buffer[34] = (uint8_t)(gss_TA*10);   
                guc_com_tx_buffer[35] = (uint8_t)((gss_TA*10)>>8);    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;  
            }
            else 
            {
                guc_com_tx_buffer[20] &= 0x7f; 
                
                guc_com_tx_buffer[33] = 0x00;    
                guc_com_tx_buffer[34] = 0x00;   
                guc_com_tx_buffer[35] = 0x00;    
                guc_com_tx_buffer[36] = 0x00;    
                guc_com_tx_buffer[37] = 0x00;
                guc_com_tx_buffer[38] = 0x00;    
            }

            //--------------------------------------------------------------------------------------------
            //有修改设置时间/常规的也是一直发送时间
            if(bflg_usb_set_time == 1)
            {
                bflg_usb_set_time = 0;
                guc_com_tx_buffer[20] |= 0x20;   //[20]的第5位代表是否有修改设置时间的标志
            }
            guc_com_tx_buffer[39] = guc_RTC_r_year;    //年
            guc_com_tx_buffer[40] = guc_RTC_r_month;   //月
            guc_com_tx_buffer[41] = guc_RTC_r_day;     //日
            guc_com_tx_buffer[42] = guc_RTC_r_hour;    //时
            guc_com_tx_buffer[43] = guc_RTC_r_minute;  //分
            guc_com_tx_buffer[44] = guc_RTC_r_second;  //秒
            
            //--------------------------------------------------------------------------------------------
            //控制帧    
            if(bflg_sent_usb_CL2 == 1)         //清除usb数据
            {
                bflg_sent_usb_CL2 = 0;
                guc_com_tx_buffer[20] |= 0x40;   //[20]的第6位代表是否有控制帧的标志
                
                guc_com_tx_buffer[45] = 0x01;    //控制代码
                guc_com_tx_buffer[46] = 0x32;    //值1
                guc_com_tx_buffer[47] = 0x33;    //值2
                guc_com_tx_buffer[48] = 0x34;    //值3
            }
            else if(bflg_sent_usb_T1 == 1)    //usb记录频率
            {
                bflg_sent_usb_T1 = 0;
                guc_com_tx_buffer[20] |= 0x40; 
                
                guc_com_tx_buffer[45] = 0x02;   
                guc_com_tx_buffer[46] = (uint8_t)gss_user_parm_value[DISP_USER_T1];  
                guc_com_tx_buffer[47] = 0x00;    
                guc_com_tx_buffer[48] = 0x00;    
            }
            else if(bflg_sent_usb_P6 == 1)     //usb 导数模式
            {
                bflg_sent_usb_P6 = 0;
                guc_com_tx_buffer[20] |= 0x40;
                
                guc_com_tx_buffer[45] = 0x03;   
                guc_com_tx_buffer[46] = (uint8_t)gss_user_parm_value[DISP_USER_P6];
                guc_com_tx_buffer[47] = 0x00;    
                guc_com_tx_buffer[48] = 0x00;    
            }
            /*else if()                       //读取usb从机信息 待+
            {
            }*/
            else if(bflg_sent_usb_UL == 1)   //usb语言类型
            {
            	bflg_sent_usb_UL = 0;
                guc_com_tx_buffer[20] |= 0x40;
                
                guc_com_tx_buffer[45] = 0x05;   
                guc_com_tx_buffer[46] = (uint8_t)gss_factory_parm_value[DISP_FACTORY_UL];
                guc_com_tx_buffer[47] = 0x00;    
                guc_com_tx_buffer[48] = 0x00; 
            }
            else
            {
                guc_com_tx_buffer[20] &= 0xbf;
                
                guc_com_tx_buffer[45] = 0x00;    
                guc_com_tx_buffer[46] = 0x00;    
                guc_com_tx_buffer[47] = 0x00;    
                guc_com_tx_buffer[48] = 0x00;    
            }

            //--------------------------------------------------------------------------------------------
            //机型帧
            guc_com_tx_buffer[49] = 0x00;    //机型识别码 超低温为 00
			guc_com_tx_buffer[50] = 0x00;    //预留
            
            //----------------------------------------------
            com_crc.uword = CRC16(guc_com_tx_buffer, 51);  //21
            guc_com_tx_buffer[51] = com_crc.ubyte.low;     //21
            guc_com_tx_buffer[52] = com_crc.ubyte.high;    //22
        }
    }
//---------------------------------------------------------------------液晶屏------------------
    else  //如果是液晶屏通讯
    {
        if (bflg_read_write_data == 0)  //如果是读数据
        {
            if (bflg_read_user_admin == 0)   //如果是读用户信息
            {
                guc_com_tx_buffer[0] = 0x01;      //设备地址
                guc_com_tx_buffer[1] = 0x03;      //读命令
                
                guc_com_tx_buffer[2] = 0xA4;
                guc_com_tx_buffer[3] = 0x1F;      //起始地址8100
                
                guc_com_tx_buffer[4] = READ_USER_CNT;  //地址个数
                guc_com_tx_buffer[5] = 0x00;      
                //------------------------------------------
                com_crc.uword = CRC16(guc_com_tx_buffer, 6);
                guc_com_tx_buffer[6] = com_crc.ubyte.low;
                guc_com_tx_buffer[7] = com_crc.ubyte.high;
            }
            else    //如果是读参数信息
            {
                guc_com_tx_buffer[0] = 0x01;      //设备地址
                guc_com_tx_buffer[1] = 0x03;      //读命令
                
                guc_com_tx_buffer[2] = 0xD6;     //起始地址8150
                guc_com_tx_buffer[3] = 0x1F;      
                
                guc_com_tx_buffer[4] = READ_ADMIN_CNT; //地址个数
                guc_com_tx_buffer[5] = 0x00;       
                //------------------------------------------
                com_crc.uword = CRC16(guc_com_tx_buffer, 6);
                guc_com_tx_buffer[6] = com_crc.ubyte.low;
                guc_com_tx_buffer[7] = com_crc.ubyte.high;
            }
        }
        else   //如果是写数据
        {
            guc_com_tx_buffer[0] = 0x01;     //设备地址
            guc_com_tx_buffer[1] = 0x10;     //写命令
            
            guc_com_tx_buffer[2] = 0x40;
            guc_com_tx_buffer[3] = 0x1F;     //起始地址10000
            
            guc_com_tx_buffer[4] = WRITE_STATE_CNT;//地址个数
            guc_com_tx_buffer[5] = 0x00;    
            
            guc_com_tx_buffer[6] = (WRITE_STATE_CNT * 2);   //字节数
            //----------------------------------------------
            {
                gss_tmp.sword = gss_TA;  //箱内温度 
                guc_com_tx_buffer[7] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[8] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_THW; //环境温度
                guc_com_tx_buffer[9] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[10] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_TC;  //冷凝器温度
                guc_com_tx_buffer[11] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[12] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_TE;  //热交换器温度
                guc_com_tx_buffer[13] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[14] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_VAC; //电源电压
                guc_com_tx_buffer[15] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[16] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = guc_bat_Vout; //电池电压
                guc_com_tx_buffer[17] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[18] = gss_tmp.ubyte.high;
                //------------------------------------------
                if (bflg_prot2_door1_pin == 0)    //门开关1状态
                {
                    guc_com_tx_buffer[19] = 0;
                    guc_com_tx_buffer[20] = 0;
                }
                else
                {
                    guc_com_tx_buffer[19] = 1;
                    guc_com_tx_buffer[20] = 0;
                }
                //------------------------------------------
                if (bflg_prot2_door2_pin == 0)    //门开关2状态
                {
                    guc_com_tx_buffer[21] = 0;
                    guc_com_tx_buffer[22] = 0;
                }
                else
                {
                    guc_com_tx_buffer[21] = 1;
                    guc_com_tx_buffer[22] = 0;
                }
                //------------------------------------------
                if (bflg_bat_charging == 0)       //电池充电状态
                {
                    guc_com_tx_buffer[23] = 0;
                    guc_com_tx_buffer[24] = 0;
                }
                else
                {
                    guc_com_tx_buffer[23] = 1;
                    guc_com_tx_buffer[24] = 0;
                }
                //------------------------------------------
                if (bflg_alarm_output == 0)       //远程报警状态
                {
                    guc_com_tx_buffer[25] = 0;
                    guc_com_tx_buffer[26] = 0;
                }
                else
                {
                    guc_com_tx_buffer[25] = 1;
                    guc_com_tx_buffer[26] = 0;
                }
                //------------------------------------------
                if (bflg_comp2_running == 0)      //高温压机状态
                {
                    guc_com_tx_buffer[27] = 0;
                    guc_com_tx_buffer[28] = 0;
                }
                else
                {
                    guc_com_tx_buffer[27] = 1;
                    guc_com_tx_buffer[28] = 0;
                }
                //------------------------------------------
                if (bflg_comp21_running == 0)     //低温压机状态
                {
                    guc_com_tx_buffer[29] = 0;
                    guc_com_tx_buffer[30] = 0;
                }
                else
                {
                    guc_com_tx_buffer[29] = 1;
                    guc_com_tx_buffer[30] = 0;
                }
                //------------------------------------------
                if (bflg_outfan20_running == 0)   //风机1状态（高温风机）
                {
                    guc_com_tx_buffer[31] = 0;
                    guc_com_tx_buffer[32] = 0;
                }
                else
                {
                    guc_com_tx_buffer[31] = 1;
                    guc_com_tx_buffer[32] = 0;
                }
                //------------------------------------------
                if (bflg_outfan21_running == 0)   //风机2状态（低温风机）
                {
                    guc_com_tx_buffer[33] = 0;
                    guc_com_tx_buffer[34] = 0;
                }
                else
                {
                    guc_com_tx_buffer[33] = 1;
                    guc_com_tx_buffer[34] = 0;
                }
                //------------------------------------------
                if (bflg_comp2_volt_up == 0)      //升压状态
                {
                    guc_com_tx_buffer[35] = 0;
                    guc_com_tx_buffer[36] = 0;
                }
                else
                {
                    guc_com_tx_buffer[35] = 1;
                    guc_com_tx_buffer[36] = 0;
                }
                //------------------------------------------
                if (bflg_comp2_volt_down == 0)    //降压状态
                {
                    guc_com_tx_buffer[37] = 0;
                    guc_com_tx_buffer[38] = 0;
                }
                else
                {
                    guc_com_tx_buffer[37] = 1;
                    guc_com_tx_buffer[38] = 0;
                }
                //------------------------------------------
                if (bflg_heat20_running == 0)     //交流毛细管加热丝状态
                {
                    guc_com_tx_buffer[39] = 0;
                    guc_com_tx_buffer[40] = 0;
                }
                else
                {
                    guc_com_tx_buffer[39] = 1;
                    guc_com_tx_buffer[40] = 0;
                }
                //------------------------------------------
                if(bflg_tank_mouth_run == 0)    //柜口加热丝状态
                {
                    guc_com_tx_buffer[41] = 0;
                    guc_com_tx_buffer[42] = 0;
                }
                else
                {
                    guc_com_tx_buffer[41] = 1;
                    guc_com_tx_buffer[42] = 0;
                }
                //------------------------------------------
                if (bflg_heat22_running == 0)     //门体加热丝状态
                {
                    guc_com_tx_buffer[43] = 0;
                    guc_com_tx_buffer[44] = 0;
                }
                else
                {
                    guc_com_tx_buffer[43] = 1;
                    guc_com_tx_buffer[44] = 0;
                }
                //------------------------------------------
                if (bflg_heat21_running == 0)     //平衡口加热丝状态
                {
                    guc_com_tx_buffer[45] = 0;
                    guc_com_tx_buffer[46] = 0;
                }
                else
                {
                    guc_com_tx_buffer[45] = 1;
                    guc_com_tx_buffer[46] = 0;
                }
                //------------------------------------------
                //预留加热丝状态
                guc_com_tx_buffer[47] = 0;
                guc_com_tx_buffer[48] = 0;
                //------------------------------------------
                if(bflg_lock_delaytimer == 0)  //电磁锁状态
                {
                    guc_com_tx_buffer[49] = 0;
                    guc_com_tx_buffer[50] = 0;
                }
                else
                {
                    guc_com_tx_buffer[49] = 1;
                    guc_com_tx_buffer[50] = 0;
                }
                //------------------------------------------
                //蜂鸣器输出状态
                //if (bflg_alarm_buzz == 0)    //如果无蜂鸣器响
                if(bflg_buzz_loop_tick_set == 0)
                {
                    guc_com_tx_buffer[51] = 0;
                    guc_com_tx_buffer[52] = 0;
                }
                else
                {
                    guc_com_tx_buffer[51] = 1;
                    guc_com_tx_buffer[52] = 0;
                }
                //------------------------------------------
                gss_tmp.sword = gus_trip_code1;   //报警状态信息1
                guc_com_tx_buffer[53] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[54] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gus_trip_code2;   //报警状态信息2
                guc_com_tx_buffer[55] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[56] = gss_tmp.ubyte.high;
                //------------------------------------------
                //后备系统检测箱内温度
                guc_com_tx_buffer[57] = 0;
                guc_com_tx_buffer[58] = 0;
                //------------------------------------------
                //后备系统喷射输出状态
                guc_com_tx_buffer[59] = 0;
                guc_com_tx_buffer[60] = 0;

                
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[0];  //热电偶温度1
                guc_com_tx_buffer[61] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[62] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[1];  //热电偶温度2
                guc_com_tx_buffer[63] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[64] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[2];  //热电偶温度3
                guc_com_tx_buffer[65] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[66] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[3];  //热电偶温度4
                guc_com_tx_buffer[67] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[68] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[4];  //热电偶温度5
                guc_com_tx_buffer[69] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[70] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[5];  //热电偶温度6
                guc_com_tx_buffer[71] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[72] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[6];  //热电偶温度7
                guc_com_tx_buffer[73] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[74] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[7];  //热电偶温度8
                guc_com_tx_buffer[75] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[76] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[8];  //热电偶温度9
                guc_com_tx_buffer[77] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[78] = gss_tmp.ubyte.high;
                //------------------------------------------
                gss_tmp.sword = gss_adg_Temp[9];  //热电偶温度10
                guc_com_tx_buffer[79] = gss_tmp.ubyte.low;
                guc_com_tx_buffer[80] = gss_tmp.ubyte.high;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[81] = 0;
                guc_com_tx_buffer[82] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[83] = 0;
                guc_com_tx_buffer[84] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[85] = 0;
                guc_com_tx_buffer[86] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[87] = 0;
                guc_com_tx_buffer[88] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[89] = 0;
                guc_com_tx_buffer[90] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[91] = 0;
                guc_com_tx_buffer[92] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[93] = 0;
                guc_com_tx_buffer[94] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[95] = 0;
                guc_com_tx_buffer[96] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[97] = 0;
                guc_com_tx_buffer[98] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[99] = 0;
                guc_com_tx_buffer[100] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[101] = 0;
                guc_com_tx_buffer[102] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[103] = 0;
                guc_com_tx_buffer[104] = 0;
                //------------------------------------------
                //预留
                guc_com_tx_buffer[105] = 0;
                guc_com_tx_buffer[106] = 0;
            }
            //----------------------------------------------
            com_crc.uword = CRC16(guc_com_tx_buffer, ((WRITE_STATE_CNT * 2) + 7));   //107
            guc_com_tx_buffer[(WRITE_STATE_CNT * 2) + 7] = com_crc.ubyte.low;
            guc_com_tx_buffer[(WRITE_STATE_CNT * 2) + 7 + 1] = com_crc.ubyte.high;
        }
    }
    //------------------------------------------------------
    guc_com_tx_point = 0;
    bflg_com_tx_busy = 1;
    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];
}
//------------------------------------------------------------------------------
/******************************************************************************
function:  在每次接收数据时，对其初始化；
           接收时，将要接收到缓存清空，准备接收新数据；

******************************************************************************/
void com_rx_init(void)   //通讯接收初始化程序，在主循环程序中调用
{
    uint8_t i;
    
    for (i = 0; i < 80; i++)        //20
    {
        guc_com_rx_buffer[i] = 0;  //清接收缓存器
    }
    
    bflg_com_rx_busy = 0;
    guc_com_rx_point = 0;
}
//------------------------------------------------------------------------------
/*******************************************************************************
function : 接收成功后，对数据进行处理 ；

           bflg_read_write_data--- 主机发送给显示板按照:发送查询指令、发送设定值
      这两个操作循环发送的方式；

        因为主板需要不停的判断此时按下的按键，比如增加"高温报警设定"后要改变判断的阀值了；
            显示板也需要知道此时主板有哪些故障，来进行显示；
        所以读/写要循环执行；
********************************************************************************/
void com_rx_data_deal(void)   //通讯接收数据处理程序，在主循环程序中调用
{
    uint8_t i,msb_byte = 0,lsb_byte = 0;
    uint16_t lus_tmp;
    word_type com_crc;
    word_type gss_tmp;
    //------------------------------------------------------
    if (bflg_com_type == 0)   //如果是LED显示屏通讯
    {
        if ((guc_com_rx_buffer[1] == 0x03))  //如果是读数据  //peak 读按键的值
        {
            com_crc.uword = CRC16(guc_com_rx_buffer, 7);
            if ((com_crc.ubyte.low == guc_com_rx_buffer[7])
             && (com_crc.ubyte.high == guc_com_rx_buffer[8]))
            {
                bflg_com_type_ok = 1;   //通讯类型判断ok
                
                bflg_com_tx_delaytime = 1;        //置发送延时标志
                gss_com_tx_delaytimer = 10;       //发送延时计时器赋值
                
                bflg_com_fault = 0;               //清通讯故障标志
                gss_com_fault_delaytimer = 0;     //清通讯故障计时器
                //------------------------------------------
                //数据处理
                bflg_read_write_data = 1;    //变为写数据
                
                if (guc_key_val != guc_com_rx_buffer[3])
                {
                    bflg_read_key_delaytime = 0; //peak 若键值有改变，则不需要等待一段时间后在判断键值，而是直接处理 read_key_deal()
                    gss_read_key_delaytimer = 0;
                }
                guc_key_val = guc_com_rx_buffer[3];    

                //注册状态时显示的IC已用注册的地址，显示需转换为卡片的张数 = xx/4;
                guc_flash_add = 0;
                msb_byte = guc_com_rx_buffer[4];
                lsb_byte = guc_com_rx_buffer[5];
                guc_flash_add = ((guc_flash_add | msb_byte)<<8) | lsb_byte;
                 
                //peak add 开锁指令
                if ((guc_com_rx_buffer[6] & 0x80) != 0)
                {
                      guc_IC_access_state = 1;
                }
                else
                {
                      guc_IC_access_state = 0;
                }
                //U盘状态
                guc_U_disk_state = (uint8_t)(guc_com_rx_buffer[6]&0x03);
                guc_usb_state    = (uint8_t)(guc_com_rx_buffer[6]&0x04);
                //------------------------------------------
                //测试版本显示板确认
                if ((guc_com_rx_buffer[6] & 0x40) != 0)
                {
                	  bflg_test_type = 1;
                }
                else
                {
                	  bflg_test_type = 0;
                }
            }
        }
        else if (guc_com_rx_buffer[1] == 0x10)    //如果是写数据  //peak 设定数码管、LED显示  打算IC卡在此接收
        {
            com_crc.uword = CRC16(guc_com_rx_buffer, 6);
            if ((com_crc.ubyte.low == guc_com_rx_buffer[6])        //peak 如果CRC校验正确
             && (com_crc.ubyte.high == guc_com_rx_buffer[7]))
            {

               //peak add  改
               //guc_IC_access_state = guc_com_rx_buffer[2];

            
                bflg_com_type_ok = 1;           //通讯类型判断ok      //peak ??
                
                bflg_com_tx_delaytime = 1;      //置发送延时标志      //peak 之后时间到将再次允许发送
                gss_com_tx_delaytimer = 10;     //发送延时计时器赋值
                
                bflg_com_fault = 0;             //清通讯故障标志      //peak ?? 未用
                gss_com_fault_delaytimer = 0;   //清通讯故障计时器
                //------------------------------------------
                //数据处理
                bflg_read_write_data = 0;       //变为读数据          //peak 
            }
        }
    }
    else  //如果是液晶屏通讯
    {
        if ((guc_com_rx_buffer[1] == 0x03))  //如果是读数据
        {
            if (bflg_read_user_admin == 0)   //如果是读用户信息
            {
                com_crc.uword = CRC16(guc_com_rx_buffer, ((READ_USER_CNT * 2) + 3));
                if ((com_crc.ubyte.low == guc_com_rx_buffer[(READ_USER_CNT * 2) + 3])
                 && (com_crc.ubyte.high == guc_com_rx_buffer[(READ_USER_CNT * 2) + 4]))
                {
                    bflg_com_type_ok = 1;    //通讯类型判断ok
                    
                    bflg_com_tx_delaytime = 1;         //置发送延时标志
                    gss_com_tx_delaytimer = 10;        //发送延时计时器赋值
                    
                    bflg_com_fault = 0;                //清通讯故障标志
                    gss_com_fault_delaytimer = 0;      //清通讯故障计时器
                    //--------------------------------------
                    //数据处理
                    //bflg_read_write_data = 1;     //变为写数据
                    bflg_read_user_admin = 1;       //变为读管理员信息
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[3];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[4];
                    gss_set_parm_value[DISP_SET_TS] = gss_tmp.sword; //得到设定温度
                    ram_para[num_comp2_TS] = gss_set_parm_value[DISP_SET_TS];
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[5];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[6];
                    gss_set_parm_value[DISP_SET_ALH] = gss_tmp.sword;//得到高温报警温度
                    ram_para[num_comp2_ALH] = gss_set_parm_value[DISP_SET_ALH];
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[7];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[8];
                    gss_set_parm_value[DISP_SET_ALL] = gss_tmp.sword;//得到低温报警温度
                    ram_para[num_comp2_ALL] = gss_set_parm_value[DISP_SET_ALL];
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[9];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[10];
                    ram_para[num_comp2_mode] = gss_tmp.sword;    //得到运行模式
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[11];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[12];
                    gss_factory_parm_value[DISP_FACTORY_Cd] = gss_tmp.sword; 
                    ram_para[num_comp2_power_on_delaytime] = gss_tmp.sword;     //得到高温机开机延时时间
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[13];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[14];
                    gss_factory_parm_value[DISP_FACTORY_Fd] = gss_tmp.sword;
                    ram_para[num_alarm_buzz_off_delaytime] = gss_tmp.sword;     //得到蜂鸣器报警延时时间
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[15];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[16];
                    ram_para[num_alarm_buzz_level] = gss_tmp.sword;   //得到蜂鸣器音量
                    //--------------------------------------
                    //蜂鸣取消
                    gss_tmp.ubyte.low = guc_com_rx_buffer[17];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[18];
                    if (gss_tmp.sword != 0)
                    {
                        if (bflg_alarm_buzz == 1)
                        {
                            if (bflg_alarm_buzz_off_delaytime == 0)
                            {
                                bflg_alarm_buzz_off_delaytime = 1;
                                gss_alarm_buzz_off_delaytimer = 0;
                            }
                        }
                    }
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[19];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[20];
                    gss_factory_parm_value[DISP_FACTORY_Ct] = gss_tmp.sword;
                    ram_para[num_comp2_PT100_ref] = gss_tmp.sword;    //得到PT100补偿值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[21];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[22];
                    //待+
                    ram_para[num_comp2_PT100_ref_backup] = gss_tmp.sword;  //得到后备系统PT100补偿值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[23];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[24];
                    //待+
                    ram_para[num_comp2_backup] = gss_tmp.sword;  //得到后备系统开关设定值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[25];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[26];
                    //待+
                    ram_para[num_comp2_backup_TS] = gss_tmp.sword;    //得到后备系统喷射温度设定值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[27];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[28];
                    //待+
                                                                     //得到后备系统喷射开关设定值                    
                    //--------------------------------------
                    //得到三色灯的控制值
                    gss_tmp.ubyte.low = guc_com_rx_buffer[29];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[30];
                    //--------------------------------------
                    
                    
                }
            }
            else    //如果是读管理员参数信息
            {
                com_crc.uword = CRC16(guc_com_rx_buffer, ((READ_ADMIN_CNT * 2) + 3));
                if ((com_crc.ubyte.low == guc_com_rx_buffer[(READ_ADMIN_CNT * 2) + 3])
                 && (com_crc.ubyte.high == guc_com_rx_buffer[(READ_ADMIN_CNT * 2) + 4]))
                {
                    bflg_com_type_ok = 1;    //通讯类型判断ok
                    
                    bflg_com_tx_delaytime = 1;         //置发送延时标志
                    gss_com_tx_delaytimer = 10;        //发送延时计时器赋值
                    
                    bflg_com_fault = 0;                //清通讯故障标志
                    gss_com_fault_delaytimer = 0;      //清通讯故障计时器
                    //--------------------------------------
                    //数据处理
                    bflg_read_write_data = 1;     //变为写数据
                    bflg_read_user_admin = 0;     //变为读用户信息
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[3];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[4];
                    ram_para[num_prot2_TA_first_delaytime] = gss_tmp.sword;     //得到高低温报警延时时间
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[5];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[6];
                    gss_user_parm_value[DISP_USER_dA] = gss_tmp.sword;
                    ram_para[num_prot2_door_delaytime] = gss_tmp.sword;     //得到开门报警延时时间
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[7];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[8];
                    gss_factory_parm_value[DISP_FACTORY_CU] = gss_tmp.sword;
                    ram_para[num_comp2_VAC_ref] = gss_tmp.sword;      //得到电源电压校准值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[9];
                    gss_tmp.ubyte.high= guc_com_rx_buffer[10];
                    gss_factory_parm_value[DISP_FACTORY_CA] = gss_tmp.sword;
                    ram_para[num_comp2_THW_ref] = gss_tmp.sword;      //得到环境温度校准值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[11];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[12];
                    gss_factory_parm_value[DISP_FACTORY_AA] = gss_tmp.sword;
                    ram_para[num_prot2_THW_T] = gss_tmp.sword;        //得到环境超温报警设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[13];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[14];
                    ram_para[num_comp2_prot_T] = gss_tmp.sword;       //得到压机寿命保护温度设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[15];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[16];
                    ram_para[num_comp2_PT100_shield] = gss_tmp.sword; //得到主传感器温度故障屏蔽设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[17];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[18];
                    ram_para[num_comp2_THW_shield] = gss_tmp.sword;   //得到环境温度故障屏蔽设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[19];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[20];
                    ram_para[num_comp2_TC_shield] = gss_tmp.sword;    //得到冷凝器温度故障屏蔽设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[21];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[22];
                    ram_para[num_comp2_TE_shield] = gss_tmp.sword;    //得到热交换器温度故障屏蔽设置值
                    //--------------------------------------
                    gss_tmp.ubyte.low = guc_com_rx_buffer[23];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[24];
                    ram_para[num_comp2_PT100_backup_shield] = gss_tmp.sword;    //得到后备系统温度故障屏蔽设置值
                    //--------------------------------------
                    //得到毛细管加热丝加热设置
                    gss_tmp.ubyte.low = guc_com_rx_buffer[25];
                    gss_tmp.ubyte.high = guc_com_rx_buffer[26];
                    if (gss_tmp.sword != 0)
                    {
                    	  bflg_heat20_force_run = 1;
                    }
                }
            }
        }
        else if (guc_com_rx_buffer[1] == 0x10)    //如果是写数据
        {
            com_crc.uword = CRC16(guc_com_rx_buffer, 6);
            if ((com_crc.ubyte.low == guc_com_rx_buffer[6])
             && (com_crc.ubyte.high == guc_com_rx_buffer[7]))
            {
                bflg_com_type_ok = 1;   //通讯类型判断ok
                
                bflg_com_tx_delaytime = 1;   //置发送延时标志
                gss_com_tx_delaytimer = 10;  //发送延时计时器赋值
                
                bflg_com_fault = 0;               //清通讯故障标志
                gss_com_fault_delaytimer = 0;     //清通讯故障计时器
                //------------------------------------------
                //数据处理
                bflg_read_write_data = 0;   //变为读数据
            }
        }
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
function:  bflg_com_rx_delaytime 置1后，延时gss_com_rx_delaytimer 设定的时间后，
           可以打开中断允许接收；
*******************************************************************************/
void com_rx_delaytime(void)     //通讯接收延时程序，在1ms定时程序中调用
{
    if (bflg_com_rx_delaytime == 1)
    {
        gss_com_rx_delaytimer--;
        if (gss_com_rx_delaytimer <= 0)
        {
            gss_com_rx_delaytimer = 0;
            bflg_com_rx_delaytime = 0;
            
            bflg_com_allow_rx = 1;
        }
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
function:  bflg_com_tx_delaytime 此标志置1后，延时gss_com_tx_delaytimer 设定的时间
           后，打开发送中断，允许发送；
          
*******************************************************************************/
void com_tx_delaytime(void)     //通讯发送延时程序，在1ms定时程序中调用
{
    if (bflg_com_tx_delaytime == 1)
    {
        gss_com_tx_delaytimer--;
        if (gss_com_tx_delaytimer <= 0)
        {
            gss_com_tx_delaytimer = 0;
            bflg_com_tx_delaytime = 0;
            
            bflg_com_allow_tx = 1;
        }
    }
}
//------------------------------------------------------------------------------
/*******************************************************************************
function: 接收数据时，多长时间后，仍未正确接收到数据，或是没有接收完，
          则停止数据的接收，也不对接收的数据进行任何的处理；
          
*******************************************************************************/
void com_rx_end_delaytime(void) //通讯接收完成延时程序，在1ms定时程序中调用
{
    if (bflg_com_rx_busy == 1)  //如果当前接收忙
    {
        gss_com_rx_end_delaytimer++;
        if (gss_com_rx_end_delaytimer >= 50)
        {
            gss_com_rx_end_delaytimer = 0;
            bflg_com_rx_busy = 0;
            
            bflg_com_rx_end = 1;
            
            bflg_com_tx_delaytime = 1;
            gss_com_tx_delaytimer = 10;
        }
    }
}
//------------------------------------------------------------------------------
/******************************************************************************
function: 没有对发生故障时的情况下进行相应的处理；
            

函数位置:1s定时器中-------------------------ok
******************************************************************************/
void com_fault_delaytime(void)     //通讯故障延时程序，在1s定时程序中调用
{
    if (bflg_com_fault == 0)                //peak 这个判断了并没有起作用
    {
        gss_com_fault_delaytimer++;
        if (gss_com_fault_delaytimer >= 30)  //30
        {
            gss_com_fault_delaytimer = 0;
            bflg_com_fault = 1;
        }
    }
    //----------------------------------------------
    //通讯类型再判断
    if (bflg_com_type_ok == 0)               //通讯正确后 = 1；不正确则为0；
    {   
        guc_com_type_cnt++;                  //peak 开始通讯时，发送5次数据
        if (guc_com_type_cnt >= 5)
        {                                   
            bflg_com_type = ~bflg_com_type;  //peak 清0 ???
        }
    }
}
//------------------------------------------------------------------------------
/******************************************************************************
function :中断中处理；
          发送要发送的内容----> |1、发送读显示板数据的指令  (8个字节)
                                |2、发送向显示板写的数据    (23个字节)
    
******************************************************************************/
void com_tx_int(void)    //通讯发送中断程序
{
    if (bflg_com_tx_busy == 1)
    {
        if (bflg_com_type == 0)    //如果是LED显示屏通讯
        {
            if (bflg_read_write_data == 0)   //如果是读数据
            {
                if (guc_com_tx_point < 8)
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];     //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;    //清发送字节索引
                    bflg_com_tx_busy = 0;    //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;    //置com通讯接收延时标志
                    gss_com_rx_delaytimer = 5;    //com通讯接收延时计时器赋值
                    
                    bflg_com_tx_delaytime = 1;
                    gss_com_tx_delaytimer = 100;
                }
            }
            else    //如果是写数据
            {
                if (guc_com_tx_point < 53)
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];     //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;    //清发送字节索引
                    bflg_com_tx_busy = 0;    //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;    //置通讯接收延时标志
                    gss_com_rx_delaytimer = 5;    //通讯接收延时计时器赋值
                    
                    bflg_com_tx_delaytime = 1;
                    gss_com_tx_delaytimer = 100;
                }
            }
        }
        else   //如果是液晶屏通讯
        {
            if (bflg_read_write_data == 0)   //如果是读数据
            {
                if (guc_com_tx_point < 8)
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];     //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;    //清发送字节索引
                    bflg_com_tx_busy = 0;    //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;    //置com通讯接收延时标志
                    gss_com_rx_delaytimer = 5;    //com通讯接收延时计时器赋值
                    
                    bflg_com_tx_delaytime = 1;
                    gss_com_tx_delaytimer = 1000;
                }
            }
            else    //如果是写数据
            {
                if (guc_com_tx_point < ((WRITE_STATE_CNT * 2) + 9))   //109
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];     //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;     //清发送字节索引
                    bflg_com_tx_busy = 0;     //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;     //置通讯接收延时标志
                    gss_com_rx_delaytimer = 5;     //通讯接收延时计时器赋值
                    
                    bflg_com_tx_delaytime = 1;
                    gss_com_tx_delaytimer = 1000;
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
/******************************************************************************
function: 中断接收的数据

          *****LED****
          如果收到的是发送完查询命令后返回的值:
          共收到9个字节。功能书中可以无限扩展的，但是此程序发送查询设定为固定的     
      查询2个寄存器，所以返回时也是固定的4个字节(一个寄存器返回2个字节);
      
       ----------------------------------------   
          如果收到的是发送完设定命令后返回的值:
          共收到8个字节，功能书中就是8个；

*******************************************************************************/
void com_rx_int(uint8_t rx_data)   //通讯接收中断程序
{
    uint8_t luc_com_rx_byte;  //接收字节临时缓冲区
    
    gss_com_rx_end_delaytimer = 0; //清接收完延时计时器
    //--------------------------------------------------
    luc_com_rx_byte = rx_data;
    
    if (bflg_com_type == 0)   //如果是LED显示屏通讯
    {
        if ((bflg_com_rx_busy == 0) && (luc_com_rx_byte == 0x00))     //如果无接收忙，且从地址正确
        {
            bflg_com_rx_busy = 1;
            
            guc_com_rx_buffer[0] = luc_com_rx_byte;
            guc_com_rx_point = 1;
        }
        else if (bflg_com_rx_busy == 1)      //如果接收忙
        {
            guc_com_rx_buffer[guc_com_rx_point++] = luc_com_rx_byte;
            
            if ((guc_com_rx_buffer[1] == 0x03) && (guc_com_rx_point >= 9))       //如果是查询命令，且接收数据完毕
            {
                guc_com_rx_point = 0;   //清接收字节索引
                bflg_com_rx_busy = 0;   //清接收忙标志
                bflg_com_rx_ok = 1;     //置接收成功标志
            }
            else if ((guc_com_rx_buffer[1] == 0x10) && (guc_com_rx_point >= 8))  //如果是多个设定命令，且接收到字节数数据帧
            {
                guc_com_rx_point = 0;   
                bflg_com_rx_busy = 0;    
                bflg_com_rx_ok = 1;    
            }
            else if (guc_com_rx_point >= 9)   //防止数组溢出
            {
                guc_com_rx_point = 0;    
                bflg_com_rx_busy = 0;   
                bflg_com_rx_ok = 1;          //还是借用接收成功的处理方式
            }
        }
    }
    else  //如果是液晶屏通讯
    {
        if ((bflg_com_rx_busy == 0) && (luc_com_rx_byte == 0x01))     //如果无接收忙，且从地址正确
        {
            bflg_com_rx_busy = 1;
            
            guc_com_rx_buffer[0] = luc_com_rx_byte;
            guc_com_rx_point = 1;
        }
        else if (bflg_com_rx_busy == 1)      //如果接收忙
        {
            guc_com_rx_buffer[guc_com_rx_point++] = luc_com_rx_byte;
            
            if (guc_com_rx_buffer[1] == 0x10)     //如果是多个设定命令
            {
                if (guc_com_rx_point >= 8)   //接收到字节数数据帧
                {
                    guc_com_rx_point = 0;    //清接收字节索引
                    bflg_com_rx_busy = 0;    //清接收忙标志
                    bflg_com_rx_ok = 1;      //置接收成功标志
                }
            }
            else if (guc_com_rx_buffer[1] == 0x03)     //如果是读信息
            {
                if (bflg_read_user_admin == 0)    //如果是读用户信息
                {
                    if (guc_com_rx_point >= ((READ_USER_CNT * 2) + 5))      //65
                    {
                        guc_com_rx_point = 0;     //清接收字节索引
                        bflg_com_rx_busy = 0;     //清接收忙标志
                        bflg_com_rx_ok = 1;       //置接收成功标志
                    }
                }
                else     //如果是读管理员信息
                {
                    if (guc_com_rx_point >= ((READ_ADMIN_CNT * 2) + 5))     //65
                    {
                        guc_com_rx_point = 0;     //清接收字节索引
                        bflg_com_rx_busy = 0;     //清接收忙标志
                        bflg_com_rx_ok = 1;       //置接收成功标志
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
unsigned int CRC16(unsigned char *puchmsg, unsigned int usDataLen)    //进行CRC校验的程序
{
    unsigned int uchCRC;
    unsigned int uIndex_x, uIndex_y;
    
    uchCRC = 0xFFFF;
    
    if (usDataLen > 0)
    {
        for (uIndex_x = 0; uIndex_x <= (usDataLen - 1); uIndex_x++)
        {
            uchCRC = uchCRC ^ (unsigned int) (*puchmsg);
            puchmsg++;
            
            for (uIndex_y = 0; uIndex_y <= 7; uIndex_y++)   //peak 8个位
            {
                if ((uchCRC & 0x0001) != 0)                 //peak 最后一位为1
                {
                    uchCRC = (uchCRC >> 1) ^ 0xA001;        
                }
                else
                {
                    uchCRC = uchCRC >> 1;
                }
            }
        }
    }
    else
    {
        uchCRC = 0;
    }
    
    return uchCRC;
}

//------------------------------------------------------------------------------------------------
//peak add
/**************************************************************************************************
函数功能: 判断是否有报警事件发送，不像设置类的，通过手动操作就知道是否发送

函数位置: bflg_com_allow_tx == 1 中 ----------------------------ok
***************************************************************************************************/
void com_tx_usb_rank(void)
{   
    //---------------------------------------------------------------------------高温报警 1
    if(bflg_alarm_ALH != bflg_usb_ALH_temp)          
    {  
        bflg_usb_ALH_temp = bflg_alarm_ALH;

        if(bflg_alarm_ALH == 1)           //要求只在报警时发送
        {
            bflg_sent_usb_ALH = 1;        //发送初始化此事件
        }
        
       /* if(bflg_alarm_ALH == 1)
        {
            bflg_usb_ALH_start = 1;       //开始报警
        }
        else
        {
            bflg_usb_ALH_start = 0;       //结束报警
        }*/
    }
    //---------------------------------------------------------------------------低温报警 2
    else if(bflg_alarm_ALL != bflg_usb_ALL_temp)    
    {
        bflg_usb_ALL_temp  = bflg_alarm_ALL;

        if(bflg_alarm_ALL == 1)
        {
            bflg_sent_usb_ALL = 1;
        }
        
       /* if(bflg_alarm_ALL == 1)
        {
            bflg_usb_ALL_start = 1;
        }
        else
        {
            bflg_usb_ALL_start = 0;
        }*/
    }
    //---------------------------------------------------------------------------环温超标报警 3
    else if(bflg_alarm_THW_high != bflg_usb_THW_temp) 
    {
        bflg_usb_THW_temp  = bflg_alarm_THW_high;

        if(bflg_alarm_THW_high == 1)
        {
            bflg_sent_usb_THW = 1;
        }

       /* if(bflg_alarm_THW_high == 1)
        {
            bflg_usb_THW_start = 1;
        }
        else
        {
            bflg_usb_THW_start = 0;
        }*/
    }
    //---------------------------------------------------------------------------开关门 8
    else if(bflg_prot2_door1_pin != bflg_usb_DOR_temp)  
    {
        bflg_usb_DOR_temp = bflg_prot2_door1_pin;
        bflg_sent_usb_DOR = 1;

        if(bflg_prot2_door1_pin == 1)
        {
            bflg_usb_DOR_open = 1;
        }
        else
        {
            bflg_usb_DOR_open = 0;
        }
    }
    //---------------------------------------------------------------------------上电断电 9
    else if(bflg_alarm_power_off != bflg_usb_POW_temp)  
    {
        bflg_usb_POW_temp = bflg_alarm_power_off;
        bflg_sent_usb_POW = 1;

        if(bflg_alarm_power_off == 0)
        {
            bflg_usb_POW_on = 1;     //上电
        }
        else
        {
            bflg_usb_POW_on = 0;
        }
    }
    //---------------------------------------------------------------------------电池报警 10
    else if(bflg_alarm_bat_discnnt != bflg_usb_BAT_discnnt_temp)
    {
        bflg_usb_BAT_discnnt_temp = bflg_alarm_bat_discnnt;//未连接
        if(bflg_alarm_bat_discnnt == 1)
        {
            bflg_sent_usb_BAT = 1;
        }
        
    }
    else if(bflg_alarm_bat_fed != bflg_usb_BAT_fed_temp)
    {
        bflg_usb_BAT_fed_temp = bflg_alarm_bat_fed;       //插反
        if(bflg_alarm_bat_fed == 1)
        {
            bflg_sent_usb_BAT = 1;
        }
    }
    else if(bflg_alarm_bat_low != bflg_usb_BAT_low_temp)
    {
        bflg_usb_BAT_low_temp = bflg_alarm_bat_low;       //电量低
        if(bflg_alarm_bat_low == 1)
        {
            bflg_sent_usb_BAT = 1;
        }
    }
    //---------------------------------------------------------------------------传感器故障 12
    else if(bflg_THW_fault != bflg_usb_THW_fault_temp)//环温
    {
        bflg_usb_THW_fault_temp = bflg_THW_fault;
        if(bflg_THW_fault == 1)
        {
            bflg_sent_usb_FAT_THW = 1;
            //bflg_sent_usb_FAT = 1;
        }
    }
    else if(bflg_TE_fault != bflg_usb_TE_fault_temp) //热交换器
    {
        bflg_usb_TE_fault_temp = bflg_TE_fault;
        if(bflg_TE_fault == 1)
        {
            bflg_sent_usb_FAT_TE = 1;
            //bflg_sent_usb_FAT = 1;
        }
    }
    else if(bflg_TC_fault != bflg_usb_TC_fault_temp) //冷凝器
    {
        bflg_usb_TC_fault_temp = bflg_TC_fault;
        if(bflg_TC_fault == 1)
        {
            bflg_sent_usb_FAT_TC = 1;
            //bflg_sent_usb_FAT = 1;
        }
    }
    else if(bflg_TA_fault != bflg_usb_TA_fault_temp) //主传感
    {
        bflg_usb_TA_fault_temp = bflg_TA_fault;
        if(bflg_TA_fault == 1)
        {
            bflg_sent_usb_FAT_TA = 1;
            //bflg_sent_usb_FAT = 1;
        }
    }
    //---------------------------------------------------------------------------冷凝器脏报警16
    else if(bflg_alarm_cond_dirty != bflg_usb_TC_temp)           
    {
        bflg_usb_TC_temp = bflg_alarm_cond_dirty;
        if(bflg_alarm_cond_dirty == 1)
        {
            bflg_sent_usb_TC = 1;
        }
    }
    //---------------------------------------------------------------------------断电报警
    else if(bflg_alarm_power_off != bflg_alarm_power_off_temp)
    {
        bflg_alarm_power_off_temp = bflg_alarm_power_off;
        if(bflg_alarm_power_off == 1)
        {
            bflg_sent_usb_POW_OFF = 1;
        }
    }
    //---------------------------------------------------------------------------开门报警
    else if(bflg_alarm_open_door != bflg_alarm_open_door_temp)
    {
        bflg_alarm_open_door_temp = bflg_alarm_open_door;
        if(bflg_alarm_open_door == 1)
        {
            bflg_sent_usb_DOOR_OPEN = 1;
        }
    }
}
/**********************************************************************************************************************
函数功能:  初始上电时，主板延时一段时间再发送usb事件，等待usb板的时间同步完成，
           否则记录这些事件的时间 会错误；
           
函数位置:  1s延时函数中 ------------------------------------------ok
**********************************************************************************************************************/
void com_usb_delay_time(void)  
{
    if(bflg_usb_delaytime == 1)
    {
        gss_usb_delaytime++;
        if(gss_usb_delaytime >= 25)
        {
            gss_usb_delaytime = 0;
            bflg_usb_delaytime = 0;
        }
    }
}

//------------------------------------------------------------------------------
#endif
