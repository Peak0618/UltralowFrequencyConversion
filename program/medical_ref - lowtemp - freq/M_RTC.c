/************************************************************************************************************************************************
�ĵ�����:
          1���Գ���ʱ��PT7C4302��һϵ�ж�д������
          2��ͨѶ��ʽ:3-wire Interface ��3��SPI
          3��ģ��3��SPI,ͨ����ʱ�����ƽ��ķ��͡����ա��ٶ�
*************************************************************************************************************************************************/


//������ͷ�ļ�
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "h_type_define.h"
#include "r_cg_timer.h"
#include "h_key_operation.h"
#include "m_secret.h"
#include "M_RTC.h"



//-----------------------------------------------------------------------------------------------------------------------------------------
//��������
void RTC_init(void);                                        
void RTC_send_data(uint8_t address,uint8_t data);         
void RTC_receive_data(uint8_t address,uint8_t *data);
void RTC_write_read_deal(void);
void RTC_read_interval(void);

uint8_t RTC_TEST(uint8_t luc_data, uint8_t luc_bit);
uint8_t RTC_SET(uint8_t luc_bit);               
uint8_t RTC_CLR(uint8_t luc_bit);
uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp);    //ʱ��ת�����ݴ�����
uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp);     //����ת��ʱ�䴦����
void RTC_write_read_enable(void);


//-----------------------------------------------------------------------------------------------------------------------------------------
//�������
flag_type flg_RTC,flg_RTC_1;             //��־�ṹ

uint8_t guc_RTC_bit_index;     //bitλ����

uint8_t guc_RTC_r_year;        //���յ�����
uint8_t guc_RTC_r_month;       //���յ�����
uint8_t guc_RTC_r_day;         //���յ�����
uint8_t guc_RTC_r_hour;        //���յ���ʱ
uint8_t guc_RTC_r_minute;      //���յ��ķ�
uint8_t guc_RTC_r_second;      //���յ�����

uint8_t guc_RTC_w_year;        //���͵���
uint8_t guc_RTC_w_month;       //���͵���
uint8_t guc_RTC_w_day;         //���͵���
uint8_t guc_RTC_w_hour;        //���͵�ʱ
uint8_t guc_RTC_w_minute;      //���͵ķ�
uint8_t guc_RTC_w_second;      //���͵���

uint8_t guc_RTC_ADDR;          //�ܵķ��͵�ַ
uint8_t guc_RTC_WDATA;         //�ܵķ��͵�����
uint8_t guc_RTC_RDATA;         //�ܵĽ��յ�����
uint8_t guc_RTC_count_w;       //����д��������ʱ����ĸ������
uint8_t guc_RTC_count_r;

int8_t gss_RTC_read_interval;   //RTC��ȡ����



//-----------------------------------------------------------------------------------------------------------------------------------------
//�궨��
#define   RTC_SCK             P6.2       //ʱ�ӹܽ�_��©
#define   RTC_IO              P6.3       //���ݿ�  _��©
#define   RTC_RST             P6.4       //Ƭѡ�ź� �ߵ�ƽ��Ч

#define   RTC_OUT_MODE        PM6.3 = 0  //������������Ϊ���
#define   RTC_IN_MODE         PM6.3 = 1  //������������Ϊ����





//-----------------------------------------------------------------------------------------------------------------------------------------
//���庯��
/******************************************************************************************************************************************
��������: PT7C4302ʱ��оƬ��ʼ��������ʱ��Ϊ�͵�ƽ

����λ��: �����ʼ��(1��) ------------------------ok
*******************************************************************************************************************************************/
void RTC_init(void)
{
    RTC_SCK = 0;
    RTC_IO  = 0;
    RTC_RST = 0;
    RTC_OUT_MODE;  
}


/******************************************************************************************************************************************
��������: �򳬼�ʱ��PT7C4302д���ݣ�ͨѶ��ʽ��ģ��SPI

��дע��: 1��������д�룬���ߵ�ƽ�ȶ����͵�ƽʱ���޸�����---��д���ڵ͵�ƽ����

��ʱ: 
        1��guc_RTC_ADDR   = д����ĵ�ַ_�̶�
        2��guc_RTC_WDATA  = �Լ�Ҫд������       
        3��bflg_RTC_allow_write = 1
        4��R_TAU0_Channel4_Start(); 

����λ��: RTC_write_read_deal()�� ----------------ok
*******************************************************************************************************************************************/
void RTC_send_data(uint8_t address,uint8_t data)  //д�ĵ�ַ�����ݶ���һ���ֽ�
{
    if(guc_RTC_bit_index >= 8)     //���ڴ˴�Ϊ�˸����һλ��������ʱ��
    {    
        R_TAU0_Channel4_Stop(); 
        
        guc_RTC_bit_index = 0;
        bflg_RTC_allow_write = 0;
        
        bflg_RTC_addsend_ok = 0;    //�ر�
        RTC_RST = 0;  

        if(bflg_RTC_continuous_w_start == 1)
        {
            guc_RTC_count_w ++;         //����д��������...��
        }
    }
    else if(RTC_SCK == 0)
    {
        RTC_RST = 1;                    //Ƭѡʹ�ܸߵ�ƽ��Ч
        
        if(bflg_RTC_addsend_ok == 0)    //���������ַaddress
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
        else                            //����Ҫ���͵�����data             
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

            /*if(guc_RTC_bit_index >= 8)   //�˴������λ�ᴫ��ʧ��
            {    
                R_TAU0_Channel4_Stop(); 
                
                guc_RTC_bit_index = 0;
                bflg_RTC_allow_write = 0;
                
                bflg_RTC_addsend_ok = 0;    //�ر�
                RTC_RST = 0;  
            }*/
        }
    }
    RTC_SCK = ~RTC_SCK;    
}

/******************************************************************************************************************************************
��������: �򳬼�ʱ��PT7C4302�����ݣ�ͨѶ��ʽ��ģ��SPI

��дע��: ���½���������ݣ��͵�ƽ�ȶ����ɶ�����--��д���ڵ͵�ƽ����

��ʱ:
        1��bflg_RTC_allow_read = 1;
        2��guc_RTC_ADDR  = ������ĵ�ַ_�̶�
        3��R_TAU0_Channel4_Start(); 

����λ��: RTC_write_read_deal()��(��ϵͳ������ʱ��4���ж��� ) ----------------ok
*******************************************************************************************************************************************/
void RTC_receive_data(uint8_t address,uint8_t *data) //д�ĵ�ַ�������ݶ���һ���ֽ�
{    
    if(RTC_SCK == 0)
    {
        RTC_RST = 1;
        
        if(bflg_RTC_addsend_ok == 0)                         //д�����ַ
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
        else                                                    //������
        {   
            RTC_IN_MODE;                                        //�����л�Ϊ��������
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
                RTC_OUT_MODE;                                   //����ΪĬ�ϵ����ģʽ
                RTC_RST = 0;                                    //��Ƭѡ

                switch(address)                                 //���������ж϶�ȡ������һ��ֵ
                {
                    case CMD_R_Y:
                        //guc_RTC_r_year = *data;    
                        guc_RTC_r_year = func_4302_timer_deal(*data);   //��  0x17  ---->17
                        break;
                    case CMD_R_MO:
                        guc_RTC_r_month = func_4302_timer_deal(*data);  //��
                        break;    
                    case CMD_R_D:
                        guc_RTC_r_day = func_4302_timer_deal(*data);    //��
                        break;  
                    case CMD_R_H:
                        guc_RTC_r_hour = func_4302_timer_deal(*data);   //ʱ
                        break;
                    case CMD_R_M:                        
                        guc_RTC_r_minute = func_4302_timer_deal(*data); //��
                        break;

                    case CMD_R_S:
                        guc_RTC_r_second = func_4302_timer_deal(*data); //��
                        break;

                    default:
                        break;
                }

                if(bflg_RTC_continuous_r_start == 1)
                {
                    guc_RTC_count_r ++;                //������������ʱ������
                }
            }
        }
    }
    RTC_SCK = ~RTC_SCK;
}


/******************************************************************************************************************************************
��������: ��д�����ڶ�ʱ���еĵ���

����λ��: ϵͳ������ʱ��4���ж��� ----------------ok
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
��������: ���ꡢ�¡��ա�ʱ���֡�������д��ĺ���

ʹ��ʱ: 1��bflg_RTC_continuous_w_start = 1;
        2��guc_RTC_w_year  = ��ֵ;
           guc_RTC_w_month = ��ֵ;
           ��
           ��
           ��

����λ��: -------------------------------ֻ�ǵ���ʱ�ù�-------δ��
*******************************************************************************************************************************************/
void RTC_continuous_w(void)
{
    if(bflg_RTC_continuous_w_start == 1)
    {
        if(guc_RTC_count_w == 0)             //д��
        {
            if(bflg_RTC_year_w == 0)
            {
                bflg_RTC_year_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_Y;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_year);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 1)       //д��
        {
            if(bflg_RTC_month_w == 0)
            {
                bflg_RTC_month_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_MO;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_month);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 2)       //д��
        {
            if(bflg_RTC_day_w == 0)
            {
                bflg_RTC_day_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_D;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_day);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 3)       //дʱ
        {
            if(bflg_RTC_hour_w == 0)
            {
                bflg_RTC_hour_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_H;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_hour);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 4)       //д��
        {
            if(bflg_RTC_minute_w == 0)
            {
                bflg_RTC_minute_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_M;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_minute);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w == 5)       //д��
        {
            if(bflg_RTC_second_w == 0)
            {
                bflg_RTC_second_w = 1;
                
                guc_RTC_ADDR   =  CMD_W_S;
                guc_RTC_WDATA = func_timer_4302_deal(guc_RTC_w_second);  
                bflg_RTC_enable_write = 1;
            }
        }
        else if(guc_RTC_count_w >= 6)         //��������д
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
��������: ���ꡢ�¡��ա�ʱ���֡������������ĺ���

ʹ��ʱ: 1��bflg_RTC_continuous_r_start = 1;
        

����λ��: ��Կ����ѭ����---------------------------------------ok
          ��ʽ������ѭ��---------------------------------------ok
*******************************************************************************************************************************************/
void RTC_continuous_r(void)
{
    if(bflg_RTC_continuous_r_start == 1)
    {
        if(guc_RTC_count_r == 0)           //����
        {
            if(bflg_RTC_year_r == 0)
            {
                bflg_RTC_year_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_Y;
            }
        }
        else if(guc_RTC_count_r == 1)       //����
        {
            if(bflg_RTC_month_r == 0)
            {
                bflg_RTC_month_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_MO;
            }
        }
        else if(guc_RTC_count_r == 2)       //����
        {
            if(bflg_RTC_day_r == 0)
            {
                bflg_RTC_day_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_D;
            }
        }
        else if(guc_RTC_count_r == 3)       //��ʱ
        {
            if(bflg_RTC_hour_r == 0)
            {
                bflg_RTC_hour_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_H;
            }
        }
        else if(guc_RTC_count_r == 4)       //����
        {
            if(bflg_RTC_minute_r == 0)
            {
                bflg_RTC_minute_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_M;
            }
        }
        else if(guc_RTC_count_r == 5)       //����
        {
            if(bflg_RTC_second_r == 0)
            {
                bflg_RTC_second_r = 1;
    
                bflg_RTC_enable_read = 1;     
                guc_RTC_ADDR  = CMD_R_S;
            }
        }
        else if(guc_RTC_count_r >= 6)      //����������
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
��������: ��ȡRTC��ʱ������ÿ����̶�ʱ���ȡʱ��

����λ��: 100ms��ʱ��-----------------------------ok
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






//��ֲ�����ĺ���
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
uint8_t func_timer_4302_deal(uint8_t luc_timer_tmp) //ʱ��ת�����ݴ�����
{                                                             //peak ��Ҫд���ʱ��ת��Ϊ����
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
uint8_t func_4302_timer_deal(uint8_t luc_4302_tmp)  //����ת��ʱ�䴦����
{                                                             //peak����ȡ������ת��Ϊʱ��_BCD��
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

//��ʱд������
/*****************************************************************************************************************************************
�������ܣ�ʱ�Ӷ���дʹ�ܿ��ƣ�

����λ�ã�1ms��ʱ��--------------------------ok
******************************************************************************************************************************************/
void RTC_write_read_enable(void)
{
    if(bflg_RTC_allow_write == 0) //û��дʱ�����ܿ����Ķ�ʱ���ж�
    {
        if(bflg_RTC_enable_read == 1) //��������ĵط�д = 1��
        {
            bflg_RTC_enable_read = 0;
            bflg_RTC_allow_read = 1;
            R_TAU0_Channel4_Start();
        }
    }
   //-----------------------------------------------------------------------
    if(bflg_RTC_allow_read == 0) //û�ж�ʱ�����ܿ�д�Ķ�ʱ���ж�
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
