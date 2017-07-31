/**************************************************************************************************************************
overview:
        1、热电偶板的温度采集
        2、gss_adg_Temp[10] 中的值就是采集到的那10路热电偶的值；----未放大

        3、采集方式：热电偶采集一路的值，是采集该路的Pt(铂)和Cu(铜)的值相减得到的；

***************************************************************************************************************************/

#ifndef _ADG_C_
#define _ADG_C_

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
#include "h_adg.h"
#include "h_main.h"
#include "h_ad.h"

//------------------------------------------------------------------------------
//函数声明
void adg_init(void);     //ad模块初始化程序，在初始化程序中调用

void adg_data_delaytime(void);//ad模块数据处理延时程序，在100ms定时程序中调用

int16_t get_adg_temp(uint16_t lus_ad);  //根据ad值查表得到温度值1

int16_t get_adg_temp2(uint16_t lus_ad); //根据ad值查表得到温度值2

int16_t get_adg_temp_end(int16_t lss_ad);

void adg_iic_int(void);       //ad模块iic模拟中断程序，在定时中断调用

uint8_t TEST(uint8_t luc_data, uint8_t luc_bit);

uint8_t SET(uint8_t luc_bit);

uint8_t CLR(uint8_t luc_bit);

//------------------------------------------------------------------------------
//变量定义
//------------------------------------------------------------------------------
//输入输出口定义
#define   SCLK_OUT_PIN        P4.3
#define   CLK_OUT_PIN         P4.4
#define   CS_OUT_PIN          P4.5
#define   DIN_OUT_PIN         P4.6
#define   DOUT_OUT_PIN        P4.7

#define   EN_OUT_PIN          P12.0
#define   WR_OUT_PIN          P14.0
#define   CSB_OUT_PIN         P14.1
#define   CSA_OUT_PIN         P14.2
#define   A0_OUT_PIN          P10.2
#define   A1_OUT_PIN          P0.1
#define   A2_OUT_PIN          P0.0
#define   A3_OUT_PIN          P14.5
//------------------------------------------------------------------------------
//命令字定义
#define   WRITE_MODE_ORDER    0x08  //写模式寄存器
#define   WRITE_MODE_REG1     0x20 
#define   WRITE_MODE_REG2     0x0A

#define   WRITE_CONFIG_ORDER  0x10  //写配置寄存器-通道1
#define   WRITE_CONFIG_REG1   0x4F  
#define   WRITE_CONFIG_REG2   0x90

#define   WRITE_CONFIG2_ORDER 0x10  //写配置寄存器-通道2
#define   WRITE_CONFIG2_REG1  0x10
#define   WRITE_CONFIG2_REG2  0x91

#define   READ_STATE_ORDER    0x40  //下一个操作为读
#define   READ_DATA_ORDER     0x58  //读数据寄存器

#define   H_LEVEL             1
#define   L_LEVEL             0

//------------------------------------------------------------------------------
//标志定义
flag_type flg_adg;
//------------------------------------------------------------------------------
uint8_t   guc_write_state;    //写状态阶段
uint8_t   guc_read_state;     //读状态阶段

uint8_t   guc_adg_bit;

uint8_t   guc_state_buffer;   //状态寄存器
word_type gus_data_buffer;    //数据寄存器

int16_t   gss_adg_data_delaytimer; //ad模块数据处理延时计时器

uint16_t  gus_adg_buffer[10];      //ad模块寄存器
uint32_t  gul_adg_sum_buffer[10];  //ad模块累加和寄存器
uint16_t  gus_adg_aver_buffer[10]; //ad模块平均值寄存器
uint8_t   guc_adg_cnt;
uint8_t   guc_adg_index;

uint16_t  gus_adg2_buffer;         //ad模块2寄存器
uint32_t  gul_adg2_sum_buffer;     //ad模块2累加和寄存器
uint16_t  gus_adg2_aver_buffer;    //ad模块2平均值寄存器

uint8_t   guc_write_config_cnt;    //写配置寄存器次数

//------------------------------------------------------------------------------
#define   TEMP_TAB_LENGTH     400 //300  //热电偶温度表-----原来300
#define   TEMP2_TAB_LENGTH    140  //热电偶温度表
//------------------------------------------------------------------------------
//通道1的热电偶温度---改为对应的电压
const int16_t temp_for_adg[] =
{
    -5603
,   -5587
,   -5571
,   -5555
,   -5539
,   -5523
,   -5506
,   -5489
,   -5473
,   -5456
,   -5439
,   -5421
,   -5404
,   -5387
,   -5369
,   -5351
,   -5334
,   -5316
,   -5297
,   -5279
,   -5261
,   -5242
,   -5224
,   -5205
,   -5186
,   -5167
,   -5148
,   -5128
,   -5109
,   -5089
,   -5070
,   -5050
,   -5030
,   -5010
,   -4989
,   -4969
,   -4949
,   -4928
,   -4907
,   -4886
,   -4865
,   -4844
,   -4823
,   -4802
,   -4780
,   -4759
,   -4737
,   -4715
,   -4693
,   -4671
,   -4648
,   -4626
,   -4604
,   -4581
,   -4558
,   -4535
,   -4512
,   -4489
,   -4466
,   -4443
,   -4419
,   -4395
,   -4372
,   -4348
,   -4324
,   -4300
,   -4275
,   -4251
,   -4226
,   -4202
,   -4177
,   -4152
,   -4127
,   -4102
,   -4077
,   -4052
,   -4026
,   -4000
,   -3975
,   -3949
,   -3923
,   -3897
,   -3871
,   -3844
,   -3818
,   -3791
,   -3765
,   -3738
,   -3711
,   -3684
,   -3657
,   -3629
,   -3602
,   -3574
,   -3547
,   -3519
,   -3491
,   -3463
,   -3435
,   -3407
,   -3379
,   -3350
,   -3322
,   -3293
,   -3264
,   -3235
,   -3206
,   -3177
,   -3148
,   -3118
,   -3089
,   -3059
,   -3030
,   -3000
,   -2970
,   -2940
,   -2910
,   -2879
,   -2849
,   -2818
,   -2788
,   -2757
,   -2726
,   -2695
,   -2664
,   -2633
,   -2602
,   -2571
,   -2539
,   -2507
,   -2476
,   -2444
,   -2412
,   -2380
,   -2348
,   -2316
,   -2283
,   -2251
,   -2218
,   -2186
,   -2153
,   -2120
,   -2087
,   -2054
,   -2021
,   -1987
,   -1954
,   -1920
,   -1887
,   -1853
,   -1819
,   -1785
,   -1751
,   -1717
,   -1683
,   -1648
,   -1614
,   -1579
,   -1545
,   -1510
,   -1475
,   -1440
,   -1405
,   -1370
,   -1335
,   -1299
,   -1264
,   -1228
,   -1192
,   -1157
,   -1121
,   -1085
,   -1049
,   -1013
,   -976
,   -940
,   -904
,   -867
,   -830
,   -794
,   -757
,   -720
,   -683
,   -646
,   -608
,   -571
,   -534
,   -496
,   -459
,   -421
,   -383
,   -345
,   -307
,   -269
,   -231
,   -193
,   -154
,   -116
,   -77
,   -39
,   0
,   39
,   78
,   117
,   156
,   195
,   234
,   273
,   312
,   352
,   391
,   431
,   470
,   510
,   549
,   589
,   629
,   669
,   709
,   749
,   790
,   830
,   870
,   911
,   951
,   992
,   1033
,   1074
,   1114
,   1155
,   1196
,   1238
,   1279
,   1320
,   1362
,   1403
,   1445
,   1486
,   1528
,   1570
,   1612
,   1654
,   1696
,   1738
,   1780
,   1823
,   1865
,   1908
,   1950
,   1993
,   2036
,   2079
,   2122
,   2165
,   2208
,   2251
,   2294
,   2338
,   2381
,   2425
,   2468
,   2512
,   2556
,   2600
,   2643
,   2687
,   2732
,   2776
,   2820
,   2864
,   2909
,   2953
,   2998
,   3043
,   3087
,   3132
,   3177
,   3222
,   3267
,   3312
,   3358
,   3403
,   3448
,   3494
,   3539
,   3585
,   3631
,   3677
,   3722
,   3768
,   3814
,   3860
,   3907
,   3953
,   3999
,   4046
,   4092
,   4138
,   4185
,   4232
,   4279
,   4325
,   4372
,   4419
,   4466
,   4513
,   4561
,   4608
,   4655
,   4702
,   4750
,   4798
,   4845
,   4893
,   4941
,   4988
,   5036
,   5084
,   5132
,   5180
,   5228
,   5277
,   5325
,   5373
,   5422
,   5470
,   5519
,   5567
,   5616
,   5665
,   5714
,   5763
,   5812
,   5861
,   5910
,   5959
,   6008
,   6057
,   6107
,   6156
,   6206
,   6255
,   6305
,   6355
,   6404
,   6454
,   6504
,   6554
,   6604
,   6654
,   6704
,   6754
,   6805
,   6855
,   6905
,   6956
,   7006
,   7057
,   7107
,   7158
,   7209
,   7260
,   7310
,   7361
,   7412
,   7463
,   7515
,   7566
,   7617
,   7668
,   7720
,   7771
,   7823
,   7874
,   7926
,   7977
,   8029
,   8081
,   8133
,   8185
,   8237
,   8289
,   8341
,   8393
,   8445
,   8497
,   8550
,   8602
,   8654
,   8708
,   8759
,   8812
,   8865
,   8917
,   8970
,   9023
,   9076
,   9129
,   9182
,   9235
,   9288

};
//------------------------------------------------------------------------------
//通道1的AD值
const int16_t adg_to_temp[] =
{
    12711 
,   12768 
,   12825 
,   12883 
,   12940 
,   12999 
,   13060 
,   13119 
,   13178 
,   13239 
,   13302 
,   13365 
,   13426 
,   13489 
,   13553 
,   13616 
,   13679 
,   13745 
,   13811 
,   13876 
,   13942 
,   14008 
,   14075 
,   14143 
,   14211 
,   14279 
,   14349 
,   14419 
,   14489 
,   14559 
,   14629 
,   14700 
,   14772 
,   14845 
,   14919 
,   14991 
,   15064 
,   15139 
,   15215 
,   15290 
,   15365 
,   15441 
,   15516 
,   15593 
,   15670 
,   15747 
,   15826 
,   15905 
,   15984 
,   16064 
,   16145 
,   16224 
,   16304 
,   16387 
,   16469 
,   16552 
,   16634 
,   16717 
,   16799 
,   16883 
,   16969 
,   17054 
,   17138 
,   17224 
,   17310 
,   17398 
,   17486 
,   17574 
,   17661 
,   17749 
,   17839 
,   17928 
,   18018 
,   18108 
,   18197 
,   18289 
,   18382 
,   18473 
,   18565 
,   18658 
,   18751 
,   18844 
,   18939 
,   19034 
,   19129 
,   19224 
,   19319 
,   19416 
,   19513 
,   19610 
,   19708 
,   19807 
,   19905 
,   20004 
,   20103 
,   20203 
,   20303 
,   20404 
,   20504 
,   20605 
,   20707 
,   20809 
,   20911 
,   21015 
,   21119 
,   21223 
,   21327 
,   21431 
,   21537 
,   21642 
,   21748 
,   21854 
,   21960 
,   22067 
,   22175 
,   22282 
,   22392 
,   22501 
,   22610 
,   22720 
,   22829 
,   22940 
,   23051 
,   23162 
,   23273 
,   23385 
,   23496 
,   23609 
,   23723 
,   23836 
,   23949 
,   24064 
,   24179 
,   24293 
,   24408 
,   24525 
,   24641 
,   24758 
,   24874 
,   24991 
,   25109 
,   25227 
,   25346 
,   25464 
,   25584 
,   25704 
,   25824 
,   25944 
,   26064 
,   26186 
,   26308 
,   26430 
,   26552 
,   26674 
,   26797 
,   26921 
,   27045 
,   27168 
,   27292 
,   27418 
,   27543 
,   27669 
,   27794 
,   27919 
,   28047 
,   28174 
,   28301 
,   28430 
,   28558 
,   28685 
,   28814 
,   28943 
,   29072 
,   29203 
,   29334 
,   29463 
,   29594 
,   29726 
,   29857 
,   29988 
,   30121 
,   30253 
,   30386 
,   30520 
,   30655 
,   30787 
,   30922 
,   31056 
,   31191 
,   31327 
,   31463 
,   31599 
,   31736 
,   31872 
,   32008 
,   32146 
,   32284 
,   32422 
,   32560 
,   32698 
,   32838 
,   32978 
,   33118 
,   33257 
,   33397 
,   33537 
,   33677 
,   33817 
,   33958 
,   34100 
,   34241 
,   34383 
,   34525 
,   34666 
,   34808 
,   34951 
,   35095 
,   35238 
,   35381 
,   35527 
,   35672 
,   35815 
,   35960 
,   36106 
,   36251 
,   36398 
,   36545 
,   36690 
,   36835 
,   36982 
,   37131 
,   37280 
,   37427 
,   37575 
,   37724 
,   37873 
,   38022 
,   38170 
,   38321 
,   38472 
,   38622 
,   38773 
,   38923 
,   39074 
,   39226 
,   39379 
,   39531 
,   39683 
,   39836 
,   39990 
,   40144 
,   40298 
,   40452 
,   40606 
,   40760 
,   40915 
,   41071 
,   41227 
,   41382 
,   41538 
,   41694 
,   41852 
,   42010 
,   42166 
,   42322 
,   42481 
,   42641 
,   42798 
,   42956 
,   43116 
,   43275 
,   43435 
,   43596 
,   43756 
,   43915 
,   44076 
,   44238 
,   44399 
,   44560 
,   44724 
,   44887 
,   45048 
,   45211 
,   45374 
,   45537 
,   45702 
,   45867 
,   46030 
,   46193 
,   46358 
,   46523 
,   46690 
,   46857 
,   47021 
,   47188 
,   47355 
,   47520 
,   47686 
,   47855 
,   48023 
,   48190 
,   48357 
,   48525 
,   48694 
,   48862 
,   49033 
,   49203 
,   49371 
,   49540 
,   49710 
,   49882 
,   50052 
,   50223 
,   50395 
,   50565 
,   50735 
,   50907 
,   51080 
,   51252 
,   51424 
,   51598 
,   51771 
,   51943 
,   52117 
,   52291 
,   52465 
,   52639 
,   52813 
,   52988 
,   53164 
,   53340 
,   53515 
,   53691 
,   53867 
,   54042 
,   54218 
,   54394 
,   54571 
,   54749 
,   54926 
,   55104 
,   55281 
,   55460 
,   55638 
,   55815 
,   55994 
,   56174 
,   56353 
,   56532 
,   56711 
,   56891 
,   57072 
,   57253 
,   57432 
,   57613 
,   57794 
,   57975 
,   58156 
,   58337 
,   58520 
,   58703 
,   58884 
,   59065 
,   59248 
,   59431 
,   59615 
,   59800 
,   59983 
,   60165 
,   60350 
,   60535 
,   60719 
,   60904 
,   61089 
,   61273 
,   61458 
,   61644 
,   61831 
,   62017 
,   62203 
,   62390 
,   62576 
,   62763 
,   62949 
,   63135 
,   63324 
,   63512 
,   63698 
,   63888 
,   64077 
,   64263 
,   64453 
,   64641 
,   64829 
,   65019 
,   65209 
,   65399 

,   65535
,   65535
,   65535
,   65535

//,   65589 //因后面这些超int16的范围所以上面用65535
//,   65779 
//,   65969 
//,   66159 

};
//------------------------------------------------------------------------------
//通道2精密电阻的温度---改为对应热电偶的压差
const int16_t temp2_for_adg[] =
{
    -1475
,   -1440
,   -1405
,   -1370
,   -1335
,   -1299
,   -1264
,   -1228
,   -1192
,   -1157
,   -1121
,   -1085
,   -1049
,   -1013
,   -976
,   -940
,   -904
,   -867
,   -830
,   -794
,   -757
,   -720
,   -683
,   -646
,   -608
,   -571
,   -534
,   -496
,   -459
,   -421
,   -383
,   -345
,   -307
,   -269
,   -231
,   -193
,   -154
,   -116
,   -77
,   -39
,   0
,   39
,   78
,   117
,   156
,   195
,   234
,   273
,   312
,   352
,   391
,   431
,   470
,   510
,   549
,   589
,   629
,   669
,   709
,   749
,   790
,   830
,   870
,   911
,   951
,   992
,   1033
,   1074
,   1114
,   1155
,   1196
,   1238
,   1279
,   1320
,   1362
,   1403
,   1445
,   1486
,   1528
,   1570
,   1612
,   1654
,   1696
,   1738
,   1780
,   1823
,   1865
,   1908
,   1950
,   1993
,   2036
,   2079
,   2122
,   2165
,   2208
,   2251
,   2294
,   2338
,   2381
,   2425
,   2468
,   2512
,   2556
,   2600
,   2643
,   2687
,   2732
,   2776
,   2820
,   2864
,   2909
,   2953
,   2998
,   3043
,   3087
,   3132
,   3177
,   3222
,   3267
,   3312
,   3358
,   3403
,   3448
,   3494
,   3539
,   3585
,   3631
,   3677
,   3722
,   3768
,   3814
,   3860
,   3907
,   3953
,   3999
,   4046
,   4092
,   4138
,   4185
,   4232
,   4279

};
//------------------------------------------------------------------------------
//通道2精密电阻的AD值
const int16_t adg_to_temp2[] =
{
    31528
,   30022
,   28590
,   27228
,   25932
,   24700
,   23530
,   22415
,   21355
,   20348
,   19388
,   18481
,   17615
,   16793
,   16015
,   15270
,   14562
,   13893
,   13259
,   12653
,   12076
,   11531
,   11011
,   10516
,   10048
,   9601
,   9177
,   8772
,   8385
,   8020
,   7672
,   7340
,   7025
,   6725
,   6439
,   6166
,   5907
,   5660
,   5425
,   5201
,   4987
,   4783
,   4589
,   4403
,   4227
,   4058
,   3897
,   3743
,   3597
,   3457
,   3323
,   3195
,   3073
,   2956
,   2845
,   2738
,   2637
,   2539
,   2446
,   2357
,   2271
,   2190
,   2112
,   2037
,   1965
,   1897
,   1831
,   1768
,   1707
,   1649
,   1594
,   1540
,   1489
,   1440
,   1393
,   1348
,   1304
,   1263
,   1223
,   1184
,   1147
,   1112
,   1078
,   1045
,   1013
,   983
,   954
,   926
,   899
,   873
,   848
,   824
,   801
,   778
,   757
,   736
,   716
,   697
,   678
,   661
,   643
,   627
,   611
,   595
,   580
,   566
,   552
,   539
,   526
,   513
,   501
,   489
,   478
,   467
,   456
,   446
,   436
,   427
,   418
,   409
,   400
,   392
,   384
,   376
,   368
,   361
,   354
,   347
,   340
,   334
,   327
,   321
,   315
,   310
,   304
,   299
,   294
,   289
,   284
,   279
,   274

};

//精密电阻和热电偶总的电压合并后对应的温度值
const int16_t end_temp_for_adg[] =
{
    -200
,   -199
,   -198
,   -197
,   -196
,   -195
,   -194
,   -193
,   -192
,   -191
,   -190
,   -189
,   -188
,   -187
,   -186
,   -185
,   -184
,   -183
,   -182
,   -181
,   -180
,   -179
,   -178
,   -177
,   -176
,   -175
,   -174
,   -173
,   -172
,   -171
,   -170
,   -169
,   -168
,   -167
,   -166
,   -165
,   -164
,   -163
,   -162
,   -161
,   -160
,   -159
,   -158
,   -157
,   -156
,   -155
,   -154
,   -153
,   -152
,   -151
,   -150
,   -149
,   -148
,   -147
,   -146
,   -145
,   -144
,   -143
,   -142
,   -141
,   -140
,   -139
,   -138
,   -137
,   -136
,   -135
,   -134
,   -133
,   -132
,   -131
,   -130
,   -129
,   -128
,   -127
,   -126
,   -125
,   -124
,   -123
,   -122
,   -121
,   -120
,   -119
,   -118
,   -117
,   -116
,   -115
,   -114
,   -113
,   -112
,   -111
,   -110
,   -109
,   -108
,   -107
,   -106
,   -105
,   -104
,   -103
,   -102
,   -101
,   -100
,   -99
,   -98
,   -97
,   -96
,   -95
,   -94
,   -93
,   -92
,   -91
,   -90
,   -89
,   -88
,   -87
,   -86
,   -85
,   -84
,   -83
,   -82
,   -81
,   -80
,   -79
,   -78
,   -77
,   -76
,   -75
,   -74
,   -73
,   -72
,   -71
,   -70
,   -69
,   -68
,   -67
,   -66
,   -65
,   -64
,   -63
,   -62
,   -61
,   -60
,   -59
,   -58
,   -57
,   -56
,   -55
,   -54
,   -53
,   -52
,   -51
,   -50
,   -49
,   -48
,   -47
,   -46
,   -45
,   -44
,   -43
,   -42
,   -41
,   -40
,   -39
,   -38
,   -37
,   -36
,   -35
,   -34
,   -33
,   -32
,   -31
,   -30
,   -29
,   -28
,   -27
,   -26
,   -25
,   -24
,   -23
,   -22
,   -21
,   -20
,   -19
,   -18
,   -17
,   -16
,   -15
,   -14
,   -13
,   -12
,   -11
,   -10
,   -9
,   -8
,   -7
,   -6
,   -5
,   -4
,   -3
,   -2
,   -1
,   0
,   1
,   2
,   3
,   4
,   5
,   6
,   7
,   8
,   9
,   10
,   11
,   12
,   13
,   14
,   15
,   16
,   17
,   18
,   19
,   20
,   21
,   22
,   23
,   24
,   25
,   26
,   27
,   28
,   29
,   30
,   31
,   32
,   33
,   34
,   35
,   36
,   37
,   38
,   39
,   40
,   41
,   42
,   43
,   44
,   45
,   46
,   47
,   48
,   49
,   50
,   51
,   52
,   53
,   54
,   55
,   56
,   57
,   58
,   59
,   60
,   61
,   62
,   63
,   64
,   65
,   66
,   67
,   68
,   69
,   70
,   71
,   72
,   73
,   74
,   75
,   76
,   77
,   78
,   79
,   80
,   81
,   82
,   83
,   84
,   85
,   86
,   87
,   88
,   89
,   90
,   91
,   92
,   93
,   94
,   95
,   96
,   97
,   98
,   99
,   100
,   101
,   102
,   103
,   104
,   105
,   106
,   107
,   108
,   109
,   110
,   111
,   112
,   113
,   114
,   115
,   116
,   117
,   118
,   119
,   120
,   121
,   122
,   123
,   124
,   125
,   126
,   127
,   128
,   129
,   130
,   131
,   132
,   133
,   134
,   135
,   136
,   137
,   138
,   139
,   140
,   141
,   142
,   143
,   144
,   145
,   146
,   147
,   148
,   149
,   150
,   151
,   152
,   153
,   154
,   155
,   156
,   157
,   158
,   159
,   160
,   161
,   162
,   163
,   164
,   165
,   166
,   167
,   168
,   169
,   170
,   171
,   172
,   173
,   174
,   175
,   176
,   177
,   178
,   179
,   180
,   181
,   182
,   183
,   184
,   185
,   186
,   187
,   188
,   189
,   190
,   191
,   192
,   193
,   194
,   195
,   196
,   197
,   198
,   199
,   200
};

//------------------------------------------------------------------------------
//程序内容//
//------------------------------------------------------------------------------
/*****************************************************************************************************************************************************
函数功能：热电偶通讯的初始化

函数位置：系统初始化中(1次)------------------------------------ok
*******************************************************************************************************************************************************/
void adg_init(void)      //ad模块初始化程序，在初始化程序中调用
{
    guc_write_state = 1;
    guc_adg_bit = 0;
    
    CLK_OUT_PIN = 0;
    CS_OUT_PIN = 1;
    SCLK_OUT_PIN = 1;
    DIN_OUT_PIN = 1;
    //----------------------------------
    EN_OUT_PIN = 0;
    WR_OUT_PIN = 0;
    CSB_OUT_PIN = 0;
    CSA_OUT_PIN = 0;
    A3_OUT_PIN = 0;
    A2_OUT_PIN = 0;
    A1_OUT_PIN = 0;
    A0_OUT_PIN = 0;
    //----------------------------------
    bflg_write_read_adg = 1;
    guc_read_state = 1;
    guc_adg_cnt = 9;
    guc_adg_index = 9;
}
/*************************************************************************************************************************************
函数功能：讨论 1、必须是间隔200ms采集一次；
               2、原来每一个通道采集8次，再换下一个通道，现在改为采集2次就更改通道，增加速度；

函数位置：100ms------------------------------------------------ok
*************************************************************************************************************************************/
void adg_data_delaytime(void) //ad模块数据处理延时程序，在100ms定时程序中调用
{
    int16_t lss_end_temp_voltage;
    
    gss_adg_data_delaytimer++;
    if (gss_adg_data_delaytimer >= 2)
    {
        gss_adg_data_delaytimer = 0;
        
        if (bflg_adg_data_ok == 0)
        {
            if (bflg_adg_channel == 0)  //如果是通道1
            {
                if (bflg_write_read_adg == 0)
                {
                    if (bflg_write_config_adg == 1)
                    {
                        guc_write_config_cnt++;
                        if (guc_write_config_cnt >= 5)
                        {
                            guc_write_config_cnt = 0;
                            bflg_write_config_adg = 0;
                        }
                    }
                    else
                    {
                        if (guc_write_state == 1)
                        {
                            bflg_write_read_adg = 1;
                            guc_read_state = 1;
                        }
                    }
                }
                else
                {
                    if (guc_read_state == 1)
                    {
                        bflg_write_read_adg = 0;
                        guc_write_state = 1;
                        
                        gus_adg_buffer[guc_adg_index] = gus_data_buffer.uword;
                        
                        if (bflg_test_mode == 0)
                        {
                            if (guc_adg_cnt > 0)
                            {
                                gul_adg_sum_buffer[guc_adg_index] += gus_adg_buffer[guc_adg_index];
                            }
                            
                            guc_adg_cnt++;
                            if (guc_adg_cnt >= 2)  //原9 --->2
                            {
                                guc_adg_cnt = 0;
                                bflg_write_config_adg = 1;
                                
                                gus_adg_aver_buffer[guc_adg_index] = (uint16_t)(gul_adg_sum_buffer[guc_adg_index] >> 0); //原3----->0
                                gul_adg_sum_buffer[guc_adg_index] = 0;
                                
                                //gss_adg_Temp[guc_adg_index] = gss_adg_Temp2 - get_adg_temp(gus_adg_aver_buffer[guc_adg_index]);

                                lss_end_temp_voltage = get_adg_temp(gus_adg_aver_buffer[guc_adg_index]);    //测试通道1电压            
                                lss_end_temp_voltage +=  gss_adg_Temp2;                                     //通道2电压
                                gss_adg_Temp[guc_adg_index] = get_adg_temp_end(lss_end_temp_voltage);

                                
                                //原来就注释
                                //gss_adg_Temp[guc_adg_index] = get_adg_temp(gus_adg_aver_buffer[guc_adg_index]);
                                
                                guc_adg_index++;                  
                                if (guc_adg_index >= 10)
                                {
                                    guc_adg_index = 0;
                                    
                                    bflg_adg_channel = 1;
                                }
                            }
                        }
                        else  //如果是测试模式
                        {
                            bflg_write_config_adg = 1;
                            gss_adg_Temp[guc_adg_index] = gss_adg_Temp2 - get_adg_temp(gus_adg_buffer[guc_adg_index]);
                            
                            guc_adg_index++;
                            if (guc_adg_index >= 10)
                            {
                                guc_adg_index = 0;
                                
                                bflg_adg_channel = 1;
                                    
                                bflg_test_ok = 1;  //测试完成标志
                            }
                        }
                        //--------------------------------------
                        if (guc_adg_index == 0)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 0;
                        }
                        else if (guc_adg_index == 1)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 1;
                        }
                        else if (guc_adg_index == 2)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 1;
                            A0_OUT_PIN = 0;
                        }
                        else if (guc_adg_index == 3)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 1;
                            A0_OUT_PIN = 1;
                        }
                        else if (guc_adg_index == 4)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 1;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 0;
                        }
                        else if (guc_adg_index == 5)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 1;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 1;
                        }
                        else if (guc_adg_index == 6)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 1;
                            A1_OUT_PIN = 1;
                            A0_OUT_PIN = 0;
                        }
                        else if (guc_adg_index == 7)
                        {
                            A3_OUT_PIN = 0;
                            A2_OUT_PIN = 1;
                            A1_OUT_PIN = 1;
                            A0_OUT_PIN = 1;
                        }
                        else if (guc_adg_index == 8)
                        {
                            A3_OUT_PIN = 1;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 0;
                        }
                        else if (guc_adg_index == 9)
                        {
                            A3_OUT_PIN = 1;
                            A2_OUT_PIN = 0;
                            A1_OUT_PIN = 0;
                            A0_OUT_PIN = 1;
                        }
                    }
                }
            }
            else    //如果是通道2
            {
                if (bflg_write_read_adg == 0)
                {
                    if (bflg_write_config_adg == 1)
                    {
                        guc_write_config_cnt++;
                        if (guc_write_config_cnt >= 5)
                        {
                            guc_write_config_cnt = 0;
                            bflg_write_config_adg = 0;
                        }
                    }
                    else
                    {
                        if (guc_write_state == 1)
                        {
                            bflg_write_read_adg = 1;
                            guc_read_state = 1;
                        }
                    }
                }
                else
                {
                    if (guc_read_state == 1)
                    {
                        bflg_write_read_adg = 0;
                        guc_write_state = 1;
                        
                        gus_adg2_buffer = gus_data_buffer.uword;
                        
                        if (guc_adg_cnt > 0)
                        {
                            gul_adg2_sum_buffer += gus_adg2_buffer;
                        }
                        
                        guc_adg_cnt++;
                        if (guc_adg_cnt >= 2)  //原9 ---->2
                        {
                            guc_adg_cnt = 0;
                            bflg_write_config_adg = 1;
                            
                            gus_adg2_aver_buffer = (uint16_t)(gul_adg2_sum_buffer >> 0); //原3 ---->0 参考点温度
                            gul_adg2_sum_buffer = 0;
                            
                            gss_adg_Temp2 = get_adg_temp2(gus_adg2_aver_buffer);
                            
                            bflg_adg_channel = 0;
                        }
                    }
                }
            }
            
            bflg_adg_data_ok = 1;
        }
    }
}
//------------------------------------------------------------------------------
int16_t get_adg_temp(uint16_t lus_ad)   //根据ad值查表得到温度值1
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP_TAB_LENGTH >> 1);
    
    if (lus_ad > adg_to_temp[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP_TAB_LENGTH)
            {
                lss_temp_index = TEMP_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad < adg_to_temp[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad > adg_to_temp[lss_temp_index]);
    }
    else if (lus_ad < adg_to_temp[lss_temp_index])
    {
        do
        {
            lss_temp_index--;
            if (lss_temp_index < 0)
            {
                lss_temp_index = 0;
                break;
            }
            //------------------------------------
            if (lus_ad > adg_to_temp[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad < adg_to_temp[lss_temp_index]);
    }
    //查表温度高1℃，在次减一个指针，温度降1℃
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp_for_adg[lss_temp_index];
}
//------------------------------------------------------------------------------
int16_t get_adg_temp2(uint16_t lus_ad)      //根据ad值查表得到温度值2
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP2_TAB_LENGTH >> 1);
    
    if (lus_ad > adg_to_temp2[lss_temp_index])
    {
        do
        {
            lss_temp_index--;
            if (lss_temp_index < 0)
            {
                lss_temp_index = 0;
                break;
            }
            //------------------------------------
            if (lus_ad < adg_to_temp2[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > adg_to_temp2[lss_temp_index]);
    }
    else if (lus_ad < adg_to_temp2[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP2_TAB_LENGTH)
            {
                lss_temp_index = TEMP2_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > adg_to_temp2[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < adg_to_temp2[lss_temp_index]);
    }
    //查表温度高1℃，在次减一个指针，温度降1℃
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp2_for_adg[lss_temp_index];
}
/***********************************************************************************************************************************
函数功能：合并精密电阻和热电偶的电压总值，求最终热电偶的温度

函数位置：adg_data_delaytime()  ----------------------------未调
************************************************************************************************************************************/
int16_t get_adg_temp_end(int16_t lus_ad) 
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP_TAB_LENGTH >> 1);
    
    if (lus_ad > temp_for_adg[lss_temp_index]) 
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP_TAB_LENGTH)
            {
                lss_temp_index = TEMP_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad < temp_for_adg[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad > temp_for_adg[lss_temp_index]);
    }
    else if (lus_ad < temp_for_adg[lss_temp_index])
    {
        do
        {
            lss_temp_index--;
            if (lss_temp_index < 0)
            {
                lss_temp_index = 0;
                break;
            }
            //------------------------------------
            if (lus_ad > temp_for_adg[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad < temp_for_adg[lss_temp_index]);
    }
    //查表温度高1℃，在次减一个指针，温度降1℃
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return end_temp_for_adg[lss_temp_index];
}

//------------------------------------------------------------------------------
void adg_iic_int(void)        //ad模块iic模拟中断程序，在定时中断调用
{
    if (bflg_adg_data_ok == 1)
    {
        CS_OUT_PIN = L_LEVEL;
        SCLK_OUT_PIN = ~SCLK_OUT_PIN;
        //--------------------------------------------------
        if (bflg_adg_reset == 1)   //如果是ad模块复位
        {
            DIN_OUT_PIN = H_LEVEL;
            
            if (SCLK_OUT_PIN == L_LEVEL)
            {
                guc_adg_bit++;
                //if (guc_adg_bit >= 32)
                if (guc_adg_bit >= 40)
                {
                    guc_adg_bit = 0;
                    CS_OUT_PIN = H_LEVEL;
                    bflg_adg_reset = 0;
                    bflg_adg_data_ok = 0;
                }
            }
        }
        else if (bflg_adg_channel == 0) //如果是通道1
        {
            if (bflg_write_read_adg == 0)    //如果是写ad模块标志
            {
                if (bflg_write_config_adg == 1)   //如果是写配置ad模块
                {
                    if (guc_write_state == 1)     //如果是写配置寄存器命令
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG_ORDER, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 2;   //写模式寄存器1
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 2)     //如果是写配置寄存器1
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG_REG1, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 3;   //写模式寄存器2
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 3)     //如果是写配置寄存器2
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG_REG2, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 1;   //写模式寄存器阶段1
                                CS_OUT_PIN = H_LEVEL;
                                
                                bflg_adg_data_ok = 0;
                            }
                        }
                    }
                }
                else    //如果是写模式ad模块
                {
                    if (guc_write_state == 1)   //如果是写模式寄存器命令
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_ORDER, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 2; //写模式寄存器1
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 2)   //如果是写模式寄存器1
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_REG1, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 3; //写模式寄存器2
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 3)   //如果是写模式寄存器2
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_REG2, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 1; //写模式寄存器阶段1
                                CS_OUT_PIN = H_LEVEL;
                                
                                bflg_adg_data_ok = 0;
                            }
                        }
                    }
                }
            }
            else   //如果是读ad模块标志
            {
                if (guc_read_state == 1)   //如果是读状态寄存器命令
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            DIN_OUT_PIN = TEST(READ_STATE_ORDER, (uint8_t) (7 - guc_adg_bit));
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 2;  //读状态寄存器
                            CS_OUT_PIN = H_LEVEL;
                            DIN_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 2)    //如果是读状态寄存器
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                guc_state_buffer |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                guc_state_buffer &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 3;  //读数据寄存器命令
                            CS_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 3)    //如果是读数据寄存器命令
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            DIN_OUT_PIN = TEST(READ_DATA_ORDER, (uint8_t) (7 - guc_adg_bit));
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 4;  //读数据寄存器1
                            CS_OUT_PIN = H_LEVEL;
                            DIN_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 4)    //如果是读数据寄存器1
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                gus_data_buffer.ubyte.high |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                gus_data_buffer.ubyte.high &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 5;  //读数据寄存器2
                        }
                    }
                }
                else if (guc_read_state == 5)    //如果是读数据寄存器2
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                gus_data_buffer.ubyte.low |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                gus_data_buffer.ubyte.low &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 1;  //读数据寄存器1
                            CS_OUT_PIN = H_LEVEL;
                            
                            bflg_adg_data_ok = 0;
                        }
                    }
                }
            }
        }
        else   //如果是通道2
        {
            if (bflg_write_read_adg == 0)    //如果是写ad模块标志
            {
                if (bflg_write_config_adg == 1)   //如果是写配置ad模块
                {
                    if (guc_write_state == 1)     //如果是写配置寄存器命令
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG2_ORDER, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 2;   //写模式寄存器1
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 2)     //如果是写配置寄存器1
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG2_REG1, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 3;   //写模式寄存器2
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 3)     //如果是写配置寄存器2
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_CONFIG2_REG2, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 1;   //写模式寄存器阶段1
                                CS_OUT_PIN = H_LEVEL;
                                
                                bflg_adg_data_ok = 0;
                            }
                        }
                    }
                }
                else    //如果是写模式ad模块
                {
                    if (guc_write_state == 1)   //如果是写模式寄存器命令
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_ORDER, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 2; //写模式寄存器1
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 2)   //如果是写模式寄存器1
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_REG1, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 3; //写模式寄存器2
                                CS_OUT_PIN = H_LEVEL;
                            }
                        }
                    }
                    else if (guc_write_state == 3)   //如果是写模式寄存器2
                    {
                        if (SCLK_OUT_PIN == L_LEVEL)
                        {
                            if (guc_adg_bit < 8)
                            {
                                DIN_OUT_PIN = TEST(WRITE_MODE_REG2, (uint8_t) (7 - guc_adg_bit));
                                guc_adg_bit++;
                            }
                        }
                        else
                        {
                            if (guc_adg_bit == 8)
                            {
                                guc_adg_bit = 0;
                                guc_write_state = 1; //写模式寄存器阶段1
                                CS_OUT_PIN = H_LEVEL;
                                
                                bflg_adg_data_ok = 0;
                            }
                        }
                    }
                }
            }
            else   //如果是读ad模块标志
            {
                if (guc_read_state == 1)   //如果是读状态寄存器命令
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            DIN_OUT_PIN = TEST(READ_STATE_ORDER, (uint8_t) (7 - guc_adg_bit));
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 2;  //读状态寄存器
                            CS_OUT_PIN = H_LEVEL;
                            DIN_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 2)    //如果是读状态寄存器
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                guc_state_buffer |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                guc_state_buffer &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 3;  //读数据寄存器命令
                            CS_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 3)    //如果是读数据寄存器命令
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            DIN_OUT_PIN = TEST(READ_DATA_ORDER, (uint8_t) (7 - guc_adg_bit));
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 4;  //读数据寄存器1
                            CS_OUT_PIN = H_LEVEL;
                            DIN_OUT_PIN = H_LEVEL;
                        }
                    }
                }
                else if (guc_read_state == 4)    //如果是读数据寄存器1
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                gus_data_buffer.ubyte.high |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                gus_data_buffer.ubyte.high &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 5;  //读数据寄存器2
                        }
                    }
                }
                else if (guc_read_state == 5)    //如果是读数据寄存器2
                {
                    if (SCLK_OUT_PIN == L_LEVEL)
                    {
                        if (guc_adg_bit < 8)
                        {
                            if (DOUT_OUT_PIN == H_LEVEL)
                            {
                                gus_data_buffer.ubyte.low |= SET((uint8_t) (7 - guc_adg_bit));
                            }
                            else
                            {
                                gus_data_buffer.ubyte.low &= CLR((uint8_t) (7 - guc_adg_bit));
                            }
                            guc_adg_bit++;
                        }
                    }
                    else
                    {
                        if (guc_adg_bit == 8)
                        {
                            guc_adg_bit = 0;
                            guc_read_state = 1;  //读数据寄存器1
                            CS_OUT_PIN = H_LEVEL;
                            
                            bflg_adg_data_ok = 0;
                        }
                    }
                }
            }
        }
    }
    else
    {
        CS_OUT_PIN = H_LEVEL; //选通不使能
        SCLK_OUT_PIN = H_LEVEL;
    }
}
//------------------------------------------------------------------------------
uint8_t TEST(uint8_t luc_data, uint8_t luc_bit)
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
uint8_t SET(uint8_t luc_bit)
{
    uint8_t luc_data;
    
    luc_data = 0x01;
    luc_data <<= luc_bit;
    
    return luc_data;
}
//------------------------------------------------------------------------------
uint8_t CLR(uint8_t luc_bit)
{
    uint8_t luc_data;
    
    luc_data = 0x01;
    luc_data <<= luc_bit;
    luc_data = (uint8_t) (~luc_data);
    
    return luc_data;
}
//------------------------------------------------------------------------------
#endif

