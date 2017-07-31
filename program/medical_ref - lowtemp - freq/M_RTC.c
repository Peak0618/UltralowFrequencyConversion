/************************************************************************************************************************************************
文档概述:
          1、对超级时钟PT7C4302的一系列读写操作；
          2、通讯方式:3-wire Interface ，3线SPI
          3、模拟3线SPI,通过定时器控制节拍发送、接收、速度
*************************************************************************************************************************************************/


//包含的头文件
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "h_type_define.h"
#include "r_cg_timer.h"
#include "h_key_operation.h"
#include "m_secret.h"
#include "M_RTC.h"



//-----------------------------------------------------------------------------------------------------------------------------------------
//函数声明
void RTC_init(void);                                        
void RTC_send_data(uint8_t address,uint8_t data);         
void RTC_receive_data(uint8_t address,uint8_t *data);
void RTC_write_read_deal(void);
void RTC_read_interval(void);

uint8_t RTC_TEST(uint8_t luc_data, uint8_t luc_bit);
uint8_t RTC_SET(uint8_t luc_bit);               
uint8_t RTC_CLR(uint8_t luc_bit);
uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp);    //时间转换数据处理函数
uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp);     //数据转换时间处理函数
void RTC_write_read_enable(void);


//-----------------------------------------------------------------------------------------------------------------------------------------
//定义变量
flag_type flg_RTC,flg_RTC_1;             //标志结构

uint8_t guc_RTC_bit_index;     //bit位索引

uint8_t guc_RTC_r_year;        //接收到的年
uint8_t guc_RTC_r_month;       //接收到的月
uint8_t guc_RTC_r_day;         //接收到的日
uint8_t guc_RTC_r_hour;        //接收到的时
uint8_t guc_RTC_r_minute;      //接收到的分
uint8_t guc_RTC_r_second;      //接收到的秒

uint8_t guc_RTC_w_year;        //发送的年
uint8_t guc_RTC_w_month;       //发送的月
uint8_t guc_RTC_w_day;         //发送的日
uint8_t guc_RTC_w_hour;        //发送的时
uint8_t guc_RTC_w_minute;      //发送的分
uint8_t guc_RTC_w_second;      //发送的秒

uint8_t guc_RTC_ADDR;          //总的发送地址
uint8_t guc_RTC_WDATA;         //总的发送的数据
uint8_t guc_RTC_RDATA;         //总的接收的数据
uint8_t guc_RTC_count_w;       //连续写入年月日时分秒的各个标记
uint8_t guc_RTC_count_r;

int8_t gss_RTC_read_interval;   //RTC读取周期



//-----------------------------------------------------------------------------------------------------------------------------------------
//宏定义
#define   RTC_SCK             P6.2       //时钟管脚_开漏
#define   RTC_IO              P6.3       //数据口  _开漏
#define   RTC_RST             P6.4       //片选信号 高电平有效

#define   RTC_OUT_MODE        PM6.3 = 0  //数据引脚配置为输出
#define   RTC_IN_MODE         PM6.3 = 1  //数据引脚配置为输入





//-----------------------------------------------------------------------------------------------------------------------------------------
//具体函数
/******************************************************************************************************************************************
函数作用: PT7C4302时钟芯片初始化，空闲时都为低电平

函数位置: 程序初始化(1次) ------------------------ok
*******************************************************************************************************************************************/
void RTC_init(void)
{
    RTC_SCK = 0;
    RTC_IO  = 0;
    RTC_RST = 0;
    RTC_OUT_MODE;  
}


/******************************************************************************************************************************************
函数作用: 向超级时钟PT7C4302写数据，通讯方式是模拟SPI

编写注意: 1、上升沿写入，即高电平稳定，低电平时可修改数据---读写都在低电平操作

打开时: 
        1、guc_RTC_ADDR   = 写命令的地址_固定
        2、guc_RTC_WDATA  = 自己要写的数据       
        3、bflg_RTC_allow_write = 1
        4、R_TAU0_Channel4_Start(); 

函数位置: RTC_write_read_deal()中 ----------------ok
*******************************************************************************************************************************************/
void RTC_send_data(uint8_t address,uint8_t data)  //写的地址、数据都是一个字节
{
    if(guc_RTC_bit_index >= 8)     //放在此处为了给最后一位数据留有时间
    {    
        R_TAU0_Channel4_Stop(); 
        
        guc_RTC_bit_index = 0;
        bflg_RTC_allow_write = 0;
        
        bflg_RTC_addsend_ok = 0;    //关闭
        RTC_RST = 0;  

        if(bflg_RTC_continuous_w_start == 1)
        {
            guc_RTC_count_w ++;         //连续写入年月日...用
        }
    }
    else if(RTC_SCK == 0)
    {
        RTC_RST = 1;                    //片选使能高电平有效
        
        if(bflg_RTC_addsend_ok == 0)    //发送命令地址address
        {            
            if (RTC_TEST(address, guc_RTC_bit_index) == 0)
            {
                RTC_IO = 0;
            }
            else
            {
                RTC_IO = 1;
            }
            
            guc_RTC_bit_index++;

            if(guc_RTC_bit_index >= 8)
            {    
                guc_RTC_bit_index = 0;
                bflg_RTC_addsend_ok = 1;
            }
        } 
        else                            //发送要发送的数据data             
        {
            if (RTC_TEST(data, guc_RTC_bit_index) == 0)
            {
                RTC_IO = 0;
            }
            else
            {
                RTC_IO = 1;
            }
            
            guc_RTC_bit_index++;

            /*if(guc_RTC_bit_index >= 8)   //此处最后哪位会传送失败
            {    
                R_TAU0_Channel4_Stop(); 
                
                guc_RTC_bit_index = 0;
                bflg_RTC_allow_write = 0;
                
                bflg_RTC_addsend_ok = 0;    //关闭
                RTC_RST = 0;  
            }*/
        }
    }
    RTC_SCK = ~RTC_SCK;    
}

/******************************************************************************************************************************************
函数作用: 向超级时钟PT7C4302读数据，通讯方式是模拟SPI

编写注意: 在下降延输出数据，低电平稳定，可读数据--读写都在低电平操作

打开时:
        1、bflg_RTC_allow_read = 1;
        2、guc_RTC_ADDR  = 读命令的地址_固定
        3、R_TAU0_Channel4_Start(); 

函数位置: RTC_write_read_deal()中(即系统函数定时器4的中断中 ) ----------------ok
*******************************************************************************************************************************************/
void RTC_receive_data(uint8_t address,uint8_t *data) //写的地址、读数据都是一个字节
{    
    if(RTC_SCK == 0)
    {
        RTC_RST = 1;
        
        if(bflg_RTC_addsend_ok == 0)                         //写命令地址
        {
            if (RTC_TEST(address, guc_RTC_bit_index) == 0)
            {
                RTC_IO = 0;
            }
            else
            {
                RTC_IO = 1;
            }
            
            guc_RTC_bit_index++;

            if(guc_RTC_bit_index >= 8)
            {    
                guc_RTC_bit_index = 0;
                bflg_RTC_addsend_ok = 1;
            }
        }
        else                                                    //读数据
        {   
            RTC_IN_MODE;                                        //引脚切换为输入引脚
            if(RTC_IO == 0)    
            {
                *data &= RTC_CLR(guc_RTC_bit_index);
            }
            else
            {
                *data |= RTC_SET(guc_RTC_bit_index);
            }                                                     
            guc_RTC_bit_index++;
            if(guc_RTC_bit_index >= 8)
            {            
                R_TAU0_Channel4_Stop(); 
                
                guc_RTC_bit_index = 0;
                bflg_RTC_addsend_ok = 0;
                bflg_RTC_allow_read = 0;
                RTC_OUT_MODE;                                   //设置为默认的输出模式
                RTC_RST = 0;                                    //关片选

                switch(address)                                 //根据命令判断读取的是哪一个值
                {
                    case CMD_R_Y:
                        //guc_RTC_r_year = *data;    
                        guc_RTC_r_year = func_4302_timer_deal(*data);   //年  0x17  ---->17
                        break;
                    case CMD_R_MO:
                        guc_RTC_r_month = func_4302_timer_deal(*data);  //月
                        break;    
                    case CMD_R_D:
                        guc_RTC_r_day = func_4302_timer_deal(*data);    //日
                        break;  
                    case CMD_R_H:
                        guc_RTC_r_hour = func_4302_timer_deal(*data);   //时
                        break;
                    case CMD_R_M:                        
                        guc_RTC_r_minute = func_4302_timer_deal(*data); //分
                        break;

                    case CMD_R_S:
                        guc_RTC_r_second = func_4302_timer_deal(*data); //秒
                        break;

                    default:
                        break;
                }

                if(bflg_RTC_continuous_r_start == 1)
                {
                    guc_RTC_count_r ++;                //连续读年月日时分秒用
                }
            }
        }
    }
    RTC_SCK = ~RTC_SCK;
}


/******************************************************************************************************************************************
函数作用: 读写函数在定时器中的调用

函数位置: 系统函数定时器4的中断中 ----------------ok
*******************************************************************************************************************************************/
void RTC_write_read_deal(void)
{
    if(bflg_RTC_allow_write == 1)
    {
        RTC_send_data(guc_RTC_ADDR, guc_RTC_WDATA);
    }

    else if(bflg_RTC_allow_read == 1)
    {
        RTC_receive_data(guc_RTC_ADDR, &guc_RTC_RDATA);

    } 
}

/******************************************************************************************************************************************
函数作用: 将年、月、日、时、分、秒连续写入的函数

使用时: 1、bflg_RTC_continuous_w_start = 1;
        2、guc_RTC_w_year  = 赋值;
           guc_RTC_w_month = 赋值;
           。
           。
           。

函数位置: -------------------------------只是调试时用过-------未调
*******************************************************************************************************************************************/
void RTC_continuous_w(void)
{
    if(bflg_RTC_continuous_w_start == 1)
    {
        if(guc_RTC_count_w == 0)             //写年
        {
            if(bflg_RTC_year_w == 0)
            {
                bflg_RTC_year_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_Y;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_year);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 1)       //写月
        {
            if(bflg_RTC_month_w == 0)
            {
                bflg_RTC_month_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_MO;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_month);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 2)       //写日
        {
            if(bflg_RTC_day_w == 0)
            {
                bflg_RTC_day_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_D;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_day);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 3)       //写时
        {
            if(bflg_RTC_hour_w == 0)
            {
                bflg_RTC_hour_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_H;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_hour);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 4)       //写分
        {
            if(bflg_RTC_minute_w == 0)
            {
                bflg_RTC_minute_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_M;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_minute);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 5)       //写秒
        {
            if(bflg_RTC_second_w == 0)
            {
                bflg_RTC_second_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_S;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_second);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w >= 6)         //结束连续写
        {
            bflg_RTC_continuous_w_start = 0;
            guc_RTC_count_w = 0;

            bflg_RTC_year_w = 0;
            bflg_RTC_month_w = 0;
            bflg_RTC_day_w = 0;
            bflg_RTC_hour_w = 0;
            bflg_RTC_minute_w = 0;
            bflg_RTC_second_w = 0;
        }
    }
}


/******************************************************************************************************************************************
函数作用: 将年、月、日、时、分、秒连续读出的函数

使用时: 1、bflg_RTC_continuous_r_start = 1;
        

函数位置: 密钥的主循环中---------------------------------------ok
          正式程序主循环---------------------------------------ok
*******************************************************************************************************************************************/
void RTC_continuous_r(void)
{
    if(bflg_RTC_continuous_r_start == 1)
    {
        if(guc_RTC_count_r == 0)           //读年
        {
            if(bflg_RTC_year_r == 0)
            {
                bflg_RTC_year_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_Y;
            }
        }
        else if(guc_RTC_count_r == 1)       //读月
        {
            if(bflg_RTC_month_r == 0)
            {
                bflg_RTC_month_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_MO;
            }
        }
        else if(guc_RTC_count_r == 2)       //读日
        {
            if(bflg_RTC_day_r == 0)
            {
                bflg_RTC_day_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_D;
            }
        }
        else if(guc_RTC_count_r == 3)       //读时
        {
            if(bflg_RTC_hour_r == 0)
            {
                bflg_RTC_hour_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_H;
            }
        }
        else if(guc_RTC_count_r == 4)       //读分
        {
            if(bflg_RTC_minute_r == 0)
            {
                bflg_RTC_minute_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_M;
            }
        }
        else if(guc_RTC_count_r == 5)       //读秒
        {
            if(bflg_RTC_second_r == 0)
            {
                bflg_RTC_second_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_S;
            }
        }
        else if(guc_RTC_count_r >= 6)      //结束连续读
        {
            bflg_RTC_continuous_r_start = 0;
            guc_RTC_count_r = 0;
    
            bflg_RTC_year_r = 0;
            bflg_RTC_month_r = 0;
            bflg_RTC_day_r = 0;
            bflg_RTC_hour_r = 0;
            bflg_RTC_minute_r = 0;
            bflg_RTC_second_r = 0;
    
            bflg_RTC_continuous_r_ok = 1;
        }
    }
}



/******************************************************************************************************************************************
函数作用: 读取RTC的时间间隔，每间隔固定时间读取时间

函数位置: 100ms定时器-----------------------------ok
*******************************************************************************************************************************************/
void RTC_read_interval(void)
{
    gss_RTC_read_interval++;
    if(gss_RTC_read_interval >= 10)   // 1s
    {
        gss_RTC_read_interval = 0;
        bflg_RTC_continuous_r_start = 1;
    }
}






//移植过来的函数
//------------------------------------------------------------------------------
uint8_t RTC_TEST(uint8_t luc_data, uint8_t luc_bit)
{
    luc_data >>= luc_bit;
    luc_data &= 0x01;
    
    if (luc_data == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    
}
//------------------------------------------------------------------------------
uint8_t RTC_SET(uint8_t luc_bit)
{
    uint8_t luc_data;
    
    luc_data = 0x01;
    luc_data <<= luc_bit;
    
    return luc_data;
}
//------------------------------------------------------------------------------
uint8_t RTC_CLR(uint8_t luc_bit)
{
    uint8_t luc_data;
    
    luc_data = 0x01;
    luc_data <<= luc_bit;
    luc_data = (uint8_t) (~luc_data);
    
    return luc_data;
}

//------------------------------------------------------------------------------
uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp) //时间转换数据处理函数
{                                                             //peak 将要写入的时间转换为数据
	  uint8_t luc_tmp;
	  
	  luc_tmp = 0;
	  if (luc_timer_tmp >= 40)
	  {
	  	  luc_tmp |= 0x40;
	  	  luc_timer_tmp -= 40;
	  }
	  if (luc_timer_tmp >= 20)
	  {
	  	  luc_tmp |= 0x20;
	  	  luc_timer_tmp -= 20;
	  }
	  if (luc_timer_tmp >= 10)
	  {
	  	  luc_tmp |= 0x10;
	  	  luc_timer_tmp -= 10;
	  }
	  luc_tmp |= luc_timer_tmp;
	  
	  return luc_tmp;
}
//------------------------------------------------------------------------------
uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp)  //数据转换时间处理函数
{                                                             //peak将读取的数据转换为时间_BCD码
	  uint8_t luc_tmp;                                        //0x17--->17
	  
	  luc_tmp = 0;
	  if ((luc_4302_tmp & 0x40) != 0)
	  {
	  	  luc_tmp += 40;
	  }
	  if ((luc_4302_tmp & 0x20) != 0)
	  {
	  	  luc_tmp += 20;
	  }
	  if ((luc_4302_tmp & 0x10) != 0)
	  {
	  	  luc_tmp += 10;
	  }
	  luc_4302_tmp &= 0x0F;
	  luc_tmp += luc_4302_tmp;
	  
	  return luc_tmp;
}

//临时写着玩玩
/*****************************************************************************************************************************************
函数功能：时钟读、写使能控制，

函数位置：1ms定时器--------------------------ok
******************************************************************************************************************************************/
void RTC_write_read_enable(void)
{
    if(bflg_RTC_allow_write == 0) //没有写时，才能开读的定时器中断
    {
        if(bflg_RTC_enable_read == 1) //在允许读的地方写 = 1；
        {
            bflg_RTC_enable_read = 0;
            bflg_RTC_allow_read = 1;
            R_TAU0_Channel4_Start();
        }
    }
   //-----------------------------------------------------------------------
    if(bflg_RTC_allow_read == 0) //没有读时，才能开写的定时器中断
    {
        if(bflg_RTC_enable_write == 1)  
        {
            bflg_RTC_enable_write = 0;
            bflg_RTC_allow_write = 1;
            R_TAU0_Channel4_Start();            
        }
    }
}

/****************************************END OF THE FILE****************************************/
