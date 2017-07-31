/***********************************************************************************
����:
0����ַ255�����Ƿ���дE2�ı�־

1�� ��ʼ���Ƚ� EEPROM_init[]�� EEPROM_init2[]���������д��E2��ֻд��һάԪ��[0],
    ֻ���°���ʱ��Ҫд�룻
    
    EEPROM_init[] д���ַ: 0
    EEPROM_init2[]д���ַ: 0+256   

    �������趨��EEPROM_init[] �����256�����ݣ�
    
2�� Ȼ���ʼ��ram_para[]��ram_para2[] ��E2�ж�ȡ���ݣ���ַҲ�ǰ������Ӧ�ĵ�ַ��ȡ��
    ��:
    ram_para[]  ֻ��ȡ EEPROM_init[] �е�valֵ��
    ram_para2[] ֻ��ȡ EEPROM_init2[]�е�valֵ��

3�����ڶ���:
   ram_para[]  �е����ݿ����޸ģ�����ͨ�����������ɶ���ֵ�����޸ı��棻
   ram_para2[] �е�����û���޸ģ��ڰ��������ϵ����µ���ʱ����ֵ���������ƣ�

   EEPROM_init[]  ���� ��min��max��editrare
                  min max�ڳ�ʼ��д��E2ʱ��val���޶���Χ��
                  editrare �ڰ�������ʱʹ�õĲ���ֵ��
   EEPROM_init2[] ��min��max
                  min max�ڳ�ʼ��д��E2ʱ��val���޶���Χ��
************************************************************************************/


#ifndef _ROM_PARA2_C_
#define _ROM_PARA2_C_
//------------------------------------------------------------------------------
//�����ļ�����
#include "h_type_define.h"
#include "h_rom_para2.h"

//------------------------------------------------------------------------------------
//��������
int16_t ram_para2[MAX_PARA_NUMBER2 + 1];   //��E2�ж�ȡ ,�� EEPROM_init2[] ��min��max
//------------------------------------------------------------------------------------
int16_t ram_para[MAX_PARA_NUMBER + 1];     //��E2�ж�ȡ ,�� EEPROM_init[]  ��min��max��editrare

                                          //min��maxֻ�ǳ�ʼ��дE2ʱ��val�����ƣ��������е�
                                          //�����Сֵ�ķ�Χ���������е�valֵ��


//------------------------------------------------------------------------------------
//������ʼ��ֵ
const int16_t EEPROM_init2[MAX_PARA_NUMBER2 + 1][5]=
{
  //----------------------------------------------------------------------------
  //                      ����ֵ      
  //  val    min    max   editrate   para_disprate
  {   200,     0, 10000,      1,      not_dot},   //Pr-00   num_key_locked_delaytime      20��=200*100ms(�ж�)
  {    50,     0, 10000,      1,      not_dot},   //Pr-01   num_key_long_delaytime        60����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {  -860, -1000,  1000,     10,      dot_0d1},   //Pr-02   num_comp2_TS_min
  {  -100, -1000,  1000,     10,      dot_0d1},   //Pr-03   num_comp2_TS_max
  
  {  -990, -1000,  1000,     10,      dot_0d1},   //Pr-04   num_comp2_ALH_min
  {  -100, -1000,  1000,     10,      dot_0d1},   //Pr-05   num_comp2_ALH_max
  
  {  -990, -1000,  1000,     10,      dot_0d1},   //Pr-06   num_comp2_ALL_min
  {  -100, -1000,  1000,     10,      dot_0d1},   //Pr-07   num_comp2_ALL_max
  //----------------------------------------------------------------------------
  {     2, -1000,  1000,      1,      dot_0d1},   //Pr-08   num_comp2_run_Tdelt
  {    -4, -1000,  1000,      1,      dot_0d1},   //Pr-09   num_comp2_stop_Tdelt
  {    10, -1000,  1000,      1,      dot_0d1},   //Pr-10   num_comp2_mode_T
  
  {    30,     0, 10000,      1,      not_dot},   //Pr-11   num_comp2_run_T_delaytime          30����
  {    30,     0, 10000,      1,      not_dot},   //Pr-12   num_comp2_stop_T_delaytime         30����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   600,     0, 10000,      1,      not_dot},   //Pr-13   num_comp2_run_delaytime  10����
  {   480,     0, 10000,      1,      not_dot},   //Pr-14   num_comp2_stop_delaytime 8����

  {    30,     0, 10000,      1,      not_dot},   //Pr-15   num_comp2_TA_fault_run_delaytime   30����
  {   600,     0, 10000,      1,      not_dot},   //Pr-16   num_comp2_TA_fault_stop_delaytime  10����
  
  {   600,     0, 10000,      1,      not_dot},   //Pr-17   num_comp2_first_run_space_delaytime  10����
  {    60,     0, 10000,      1,      not_dot},   //Pr-18   num_comp2_run_space_delaytime   1����
  
  {   840,     0, 10000,      1,      not_dot},   //Pr-19   num_comp2_continue_first_run_delaytime  16Сʱ
  {   300,     0, 10000,      1,      not_dot},   //Pr-20   num_comp2_continue_run_delaytime   5Сʱ
  {   600,     0, 10000,      1,      not_dot},   //Pr-21   num_comp2_force_stop_delaytime     10����
  
  {   -30,  -100,   100,      1,      not_dot},   //Pr-22   num_comp21_run_T    -30��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {    20,  -100,   100,      1,      not_dot},   //Pr-23   num_outfan2_on_T              20��
  {    12,  -100,   100,      1,      not_dot},   //Pr-24   num_outfan2_off_T             12��
  {   120,     0, 10000,      1,      not_dot},   //Pr-25   num_outfan2_off_delaytime     2����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   960,     0, 10000,      1,      not_dot},   //Pr-26   num_heat20_space_delaytime         16Сʱ
  {     0, -1000,  1000,      1,      dot_0d1},   //Pr-27   num_heat20_T                       0��
  {    60,     0, 10000,      1,      not_dot},   //Pr-28   num_heat20_wait_delaytime          1Сʱ
  {   180,     0, 10000,      1,      not_dot},   //Pr-29   num_heat20_comp2_stop_delaytime    3����
  {   360,     0, 10000,      1,      not_dot},   //Pr-30   num_heat20_run_delaytime           6����
  {   600,     0, 10000,      1,      not_dot},   //Pr-31   num_heat20_force_run_delaytime     10����
  {   180,     0, 10000,      1,      not_dot},   //Pr-32   num_heat20_stop_delaytime          3����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {  -350, -1000,  1000,      1,      dot_0d1},   //Pr-33   num_heat21_T                       -35��
  {     5,     0, 10000,      1,      not_dot},   //Pr-34   num_heat21_run_delaytime           5����
  {     5,     0, 10000,      1,      not_dot},   //Pr-35   num_heat21_stop_delaytime          5����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   -20, -1000,  1000,      1,      dot_0d1},   //Pr-36   num_prot2_TA_high_Tdelt
  {   60,     0, 10000,      1,      not_dot},    //Pr-37   num_prot2_TA_high_delaytime   1����
  {    20, -1000,  1000,      1,      dot_0d1},   //Pr-38   num_prot2_TA_low_Tdelt
  {   60,     0, 10000,      1,      not_dot},    //Pr-39   num_prot2_TA_low_delaytime    1����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {    -1,  -100,   100,      1,      not_dot},   //Pr-40   num_prot2_THW_Tdelt
  {    30,     0, 10000,      1,      not_dot},   //Pr-41   num_prot2_THW_high_delaytime  1����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {    90,     0,  1000,      1,      not_dot},   //Pr-42   num_power_off_12V   9.0V
  {   110,     0,  1000,      1,      not_dot},   //Pr-43   num_power_on_12V    11.0V
  {    80,     0,  1000,      1,      not_dot},   //Pr-44   num_power_fautl_VAC1     80V
  {    90,     0,  1000,      1,      not_dot},   //Pr-45   num_power_fautl_VAC2     90V
  {    20,     0,  1000,      1,      not_dot},   //Pr-46   num_port2_bat_discnnt_V1      2.0V
  {    50,     0,  1000,      1,      not_dot},   //Pr-47   num_port2_bat_discnnt_V2      5.0V
  {   100,     0,  1000,      1,      not_dot},   //Pr-48   num_prot2_bat_discnnt_I       50mA
  
  {   105,     0,  1000,      1,      not_dot},   //Pr-49   num_bat_low_V1      80V
  {   110,     0,  1000,      1,      not_dot},   //Pr-50   num_bat_low_V2      80V
  {    90,     0,  1000,      1,      not_dot},   //Pr-51   num_bat_low_V3      80V
  {    95,     0,  1000,      1,      not_dot},   //Pr-52   num_bat_low_V4      80V
  {    24,     0, 10000,      1,      not_dot},   //Pr-53   num_prot2_bat_low_delaytime   24Сʱ
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   120,     0, 10000,      1,      not_dot},   //Pr-54   num_prot2_TC_first_delaytime  3Сʱ
  {    13,  -100,   100,      1,      not_dot},   //Pr-55   num_prot2_cond_dirty_TC1
  {    13,  -100,   100,      1,      not_dot},   //Pr-56   num_prot2_cond_dirty_TC2
  {   300,     0, 10000,      1,      not_dot},   //Pr-57   num_prot2_cond_dirty_delaytime     5����
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   184,     0,  1000,      1,      not_dot},   //Pr-58   num_prot2_volt_V1   184V
  {   253,     0,  1000,      1,      not_dot},   //Pr-59   num_prot2_volt_V2   253V
  {   194,     0,  1000,      1,      not_dot},   //Pr-60   num_prot2_volt_V3   194V
  {   243,     0,  1000,      1,      not_dot},   //Pr-61   num_prot2_volt_V4   243V
  {    93,     0,  1000,      1,      not_dot},   //Pr-62   num_prot2_volt_V5   93V
  {   136,     0,  1000,      1,      not_dot},   //Pr-63   num_prot2_volt_V6   136V
  {    98,     0,  1000,      1,      not_dot},   //Pr-64   num_prot2_volt_V7   98V
  {   131,     0,  1000,      1,      not_dot},   //Pr-65   num_prot2_volt_V8   131V
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   184,     0, 10000,      1,      not_dot},   //Pr-66   num_comp2_volt_low_220V1      184V
  {   253,     0, 10000,      1,      not_dot},   //Pr-67   num_comp2_volt_low_220V2      253V-->194
  {    93,     0, 10000,      1,      not_dot},   //Pr-68   num_comp2_volt_low_115V1       93V
  {   136,     0, 10000,      1,      not_dot},   //Pr-69   num_comp2_volt_low_115V2      136V-->98
  {    10,     0, 10000,      1,      not_dot},   //Pr-70   num_comp2_volt_low_delaytime  10��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {     7,     0, 10000,      1,      not_dot},   //Pr-71   num_comp2_volt_delaytime      7����
  {    30,     0, 10000,      1,      not_dot},   //Pr-72   num_comp21_volt_delaytime     30��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   242,     0, 10000,      1,      not_dot},   //Pr-73   num_volt_down_220V1      242V
  {   230,     0, 10000,      1,      not_dot},   //Pr-74   num_volt_down_220V2      230V
  {   205,     0, 10000,      1,      not_dot},   //Pr-75   num_volt_up_220V1        205V
  {   218,     0, 10000,      1,      not_dot},   //Pr-76   num_volt_up_220V2        218V
  {   122,     0, 10000,      1,      not_dot},   //Pr-77   num_volt_down_115V1      122V
  {   115,     0, 10000,      1,      not_dot},   //Pr-78   num_volt_down_115V2      115V
  {   125,     0, 10000,      1,      not_dot},   //Pr-79   num_volt_down_115V3      125V
  {   120,     0, 10000,      1,      not_dot},   //Pr-80   num_volt_down_115V4      120V
  {    98,     0, 10000,      1,      not_dot},   //Pr-81   num_volt_up_115V1         98V
  {   105,     0, 10000,      1,      not_dot},   //Pr-82   num_volt_up_115V2        105V
  {    10,     0, 10000,      1,      not_dot},   //Pr-83   num_volt_up_down_delaytime    10��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   135,     0,  1000,      1,      not_dot},   //Pr-84   num_bat1_charge_V1  13.5V
  {   143,     0,  1000,      1,      not_dot},   //Pr-85   num_bat1_charge_V2  14.3V
  {   124,     0,  1000,      1,      not_dot},   //Pr-86   num_bat1_charge_V3  12.4V
  {   610,     0,  1000,      1,      not_dot},   //Pr-87   num_bat1_charge_I1  610mA
  {   590,     0,  1000,      1,      not_dot},   //Pr-88   num_bat1_charge_I2  590mA
  {   270,     0,  1000,      1,      not_dot},   //Pr-89   num_bat1_charge_I3  270mA
  {   250,     0,  1000,      1,      not_dot},   //Pr-90   num_bat1_charge_I4  250mA
  {   139,     0,  1000,      1,      not_dot},   //Pr-91   num_bat1_charge_V4  13.9V
  {   137,     0,  1000,      1,      not_dot},   //Pr-92   num_bat1_charge_V5  13.7V
  {    36,     0,  1000,      1,      not_dot},   //Pr-93   num_bat1_charge_continue_delaytime 36Сʱ
  {    30,     0,    40,      1,      not_dot},   //Pr-94   num_bat1_charge_sapce_delaytime    30��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   120,     0,  1000,      1,      not_dot},   //Pr-95   num_bat2_charge_V1  12.0V
  {   124,     0,  1000,      1,      not_dot},   //Pr-96   num_bat2_charge_V2  12.4V
  {   112,     0,  1000,      1,      not_dot},   //Pr-97   num_bat2_charge_V3  11.2V
  {   710,     0,  1000,      1,      not_dot},   //Pr-98   num_bat2_charge_I1  710mA
  {   690,     0,  1000,      1,      not_dot},   //Pr-99   num_bat2_charge_I2  690mA
  {   330,     0,  1000,      1,      not_dot},   //Pr-100  num_bat2_charge_I3  330mA
  {   310,     0,  1000,      1,      not_dot},   //Pr-101  num_bat2_charge_I4  310mA
  {   129,     0,  1000,      1,      not_dot},   //Pr-102  num_bat2_charge_V4  12.9V
  {   127,     0,  1000,      1,      not_dot},   //Pr-103  num_bat2_charge_V5  12.7V
  {     7,     0,  1000,      1,      not_dot},   //Pr-104  num_bat2_charge_continue_delaytime 7Сʱ
  {    30,     0,    40,      1,      not_dot},   //Pr-105  num_bat2_charge_sapce_delaytime    30��
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {   105,     0,  1000,      1,      not_dot},   //Pr-106  num_bat1_discharge_off_V 10.5V
  {    90,     0,  1000,      1,      not_dot},   //Pr-107  num_bat2_discharge_off_V 9.0V
  {    65,  -100,   100,      1,      not_dot},   //Pr-108  num_bat2_discharge_off_T
  {    55,  -100,   100,      1,      not_dot},   //Pr-109  num_bat2_discharge_on_T
  
  //  val    min    max   editrate   para_disprate                                       peak add  �������˿����
  {  -350, -1000,  1000,      1,      dot_0d1},   //Pr-110   num_heat22_T                       -35��
  {     5,     0, 10000,      1,      not_dot},   //Pr-111   num_heat22_run_delaytime           5����
  {     5,     0, 10000,      1,      not_dot},   //Pr-112   num_heat22_stop_delaytime          5����
  //----------------------------------------------------------------------------

  //  val    min    max   editrate   para_disprate                                      peak add  �������
  {   35, -1000,  1000,      1,      not_dot},   //Pr-113   num_outfan2_T                    35��
  {   60,     0, 10000,      1,      not_dot},   //Pr-114   num_outfan2_delaytime            60����
  {   3500,   0, 10000,      1,      not_dot},   //Pr-115   num_outfan2_rad                  3500rad/min ת��
  //----------------------------------------------------------------------------

  //peak add ���ݰ����¹�������ӵĿɵ�����
  //  val    min    max   editrate   para_disprate                                      
  {   1,      0,   1000,      1,      not_dot},   //Pr-116   num_door_alarm_delaytime_min    ���ű�����ʱ��Сֵ  ��Χ  1-30min         
  {   30,     0,   1000,      1,      not_dot},   //Pr-117   num_door_alarm_delaytime_max

  {   1,      0,   1000,      1,      not_dot},   //Pr-118   num_usb_read_delaytime_min      usbȡ��������Сֵ         1-99min      
  {   99,     0,   1000,      1,      not_dot},   //Pr-119   num_usb_read_delaytime_max

  {   0,      0,   10000,     1,      not_dot},   //Pr-120   num_IC_password_min             IC��ע������              000-999   
  {  999,     0,   10000,     1,      not_dot},   //Pr-121   num_IC_password_max

  {  -10, -1000,   1000,      1,      not_dot},   //Pr-122   num_pt100_calibration_min       pt100���¶�У׼           -10 ~ 10  
  {   10,     0,   1000,      1,      not_dot},   //Pr-123   num_pt100_calibration_max

  {   -9, -1000,   1000,      1,      not_dot},   //Pr-124   num_voltage_calibration_min     ��ѹУ׼                  -9~9  
  {    9,     0,   1000,      1,      not_dot},   //Pr-125   num_voltage_calibration_max

  {   -10,-1000,   1000,      1,      not_dot},   //Pr-126   num_THW_calibration_min         ����У׼                  -10 ~10 
  {   10,     0,   1000,      1,      not_dot},   //Pr-127   num_THW_calibration_max

  {   30,     0,   1000,      1,      not_dot},   //Pr-128   num_buzz_alarm_delaytime_min    ����������ʱ              30��60��120      
  {   120,    0,   1000,      1,      not_dot},   //Pr-129   num_buzz_alarm_delaytime_max 

  {   30,     0,   1000,      1,      not_dot},   //Pr-130   num_THW_hight_alarm_min         ���¹��߱���              30~50   
  {   50,     0,   1000,      1,      not_dot},   //Pr-131   num_THW_hight_alarm_max

  {   1,      0,   1000,      1,      not_dot},   //Pr-132   num_comp_startup_delaytime_min  ����ѹ��������ʱ          1~15     
  {   15,     0,   1000,      1,      not_dot},   //Pr-133   num_comp_startup_delaytime_max

  {   0,      0,   1000,      1,      not_dot},   //Pr-134   num_password_val_min            ��������                     0-99 
  {  99,      0,   1000,      1,      not_dot},   //Pr-135   num_password_val_max            ��Ӧ��������� 0 �ţ�֮ǰû����

  {   0,      0,   1000,      1,      not_dot},   //Pr-136   num_TE_fault_shield_min         �Ƚ�������������                
  {   1,      0,   1000,      1,      not_dot},   //Pr-137   num_TE_fault_shield_max       

  {   0,      0,   1000,      1,      not_dot},   //Pr-138   num_password_val_min            ��������                
  {  99,      0,   1000,      1,      not_dot},   //Pr-139   num_password_val_max  

  {   0,      0,   1000,      1,      not_dot},   //Pr-140   num_usb_P6_min                  USB��������ģʽ                
  {  12,      0,   1000,      1,      not_dot},   //Pr-141   num_usb_P6_max  
  
                                                  
  //----------------------------------------------------------------------------
};
//------------------------------------------------------------------------------
const int16_t EEPROM_init[MAX_PARA_NUMBER + 1][5]=
{
  //  val    min    max   editrate   para_disprate
  {    16, -1000,  1000,      1,      dot_0d1},   //Pr-00   num_password_val
  //----------------------------------------------------------------------------
  //  val    min    max   editrate   para_disprate
  {  -800, -1000,  1000,     10,      dot_0d1},   //Pr-01   num_comp2_TS
  {  -700, -1000,  1000,     10,      dot_0d1},   //Pr-02   num_comp2_ALH
  {  -900, -1000,  1000,     10,      dot_0d1},   //Pr-03   num_comp2_ALL
  {     1,     0,     1,      1,      dot_0d1},   //Pr-04   num_comp2_mode               ������/����
  
  {    60,     0, 10000,      1,      not_dot},   //Pr-05   num_comp2_power_on_delaytime  60����
  {    30,     0, 10000,      1,      not_dot},   //Pr-06   num_alarm_buzz_off_delaytime  30����
  {    10,     0,    10,      1,      not_dot},   //Pr-07   num_alarm_buzz_level          10
  {     0, -1000,  1000,     10,      dot_0d1},   //Pr-08   num_comp2_PT100_ref
  {     0, -1000,  1000,     10,      dot_0d1},   //Pr-09   num_comp2_PT100_ref_backup
  {     0,     0,     1,      1,      dot_0d1},   //Pr-10   num_comp2_backup
  {  -860, -1000,  1000,     10,      dot_0d1},   //Pr-11   num_comp2_backup_TS
  //  val    min    max   editrate   para_disprate
  //----------------------------------------------------------------------------
  {   180,     0, 10000,      1,      not_dot},   //Pr-12   num_prot2_TA_first_delaytime  3Сʱ
  {     5,     0, 10000,      1,      not_dot},   //Pr-13   num_prot2_door_delaytime 5����
  
  {     0, -1000,  1000,     10,      dot_0d1},   //Pr-14   num_comp2_VAC_ref
  {     0,  -100,   100,      1,      not_dot},   //Pr-15   num_comp2_THW_ref
  
  {    35,  -100,   100,      1,      not_dot},   //Pr-16   num_prot2_THW_T
  {     0,  -100,   100,      1,      not_dot},   //Pr-17   num_comp2_prot_T
  
  {     0,     0,     1,      1,      not_dot},   //Pr-18   num_comp2_PT100_shield      //peak ��������
  {     0,     0,     1,      1,      not_dot},   //Pr-19   num_comp2_THW_shield
  {     0,     0,     1,      1,      not_dot},   //Pr-20   num_comp2_TC_shield
  {     0,     0,     1,      1,      not_dot},   //Pr-21   num_comp2_TE_shield
  {     0,     0,     1,      1,      not_dot},   //Pr-22   num_comp2_PT100_backup_shield

  //----------------------------------------------------------------------------
  
  //peak add ���ݰ����¹�������ӵĿɵ�����
  //  val    min    max   editrate   para_disprate
  {     5,     0,  1000,      1,      not_dot},   //Pr-23   num_door_alarm_delaytime
  {     6,     0,  1000,      1,      not_dot},   //Pr-24   num_usb_read_delaytime
  {     8,     0,  1000,      1,      not_dot},   //Pr-25   num_IC_password
  {     0, -1000,  1000,      1,      not_dot},   //Pr-26   num_pt100_calibration
  {     0, -1000,  1000,      1,      not_dot},   //Pr-27   num_voltage_calibration
  {     0, -1000,  1000,      1,      not_dot},   //Pr-28   num_THW_calibration
  {    30,     0,  1000,      1,      not_dot},   //Pr-29   num_buzz_alarm_delaytime
  {    35,     0,  1000,      1,      not_dot},   //Pr-30   num_THW_hight_alarm
  {     1,     0,  1000,      1,      not_dot},   //Pr-31   num_comp_startup_delaytime
  {     0,     0,     1,      1,      not_dot},   //Pr-32   num_TE_fault_shield        ---�����������δ���������Сֵ
  {    12,     0,   100,      1,      not_dot},   //Pr-33   num_usb_P6         
  {     0,     0,   100,      1,      not_dot},   //Pr-34   num_secret_key          
  {     0,     0,   100,      1,      not_dot},   //Pr-35   num_usb_language          
  {     1,     0,   100,      1,      not_dot},   //Pr-36   num_tank_mouth_heat          

  {     5, -1000,  1000,      1,      not_dot},   //Pr-37   num_debug_P20          
  {     0, -1000,  1000,      1,      not_dot},   //Pr-38   num_debug_P21 
  {     6,     0,    99,      1,      not_dot},   //Pr-39   num_password_PS1         
};
//------------------------------------------------------------------------------
#endif
