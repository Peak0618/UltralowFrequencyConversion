/***************************************************************************************************************************
overview:
        1���жϴ������Ƿ��й���(��·/��·)
        2����ʵ�ʼ����¶ȡ���ѹ�������ȣ�
****************************************************************************************************************************/

#ifndef _AD_C_
#define _AD_C_
//------------------------------------------------------------------------------
//�����ļ�����
#include "h_type_define.h"
#include "h_ad.h"
#include "h_ad_tab.h"
#include "h_rom_para2.h"
#include "h_key_operation.h"
#include "h_protect.h"
#include "h_comp2.h"
#include "h_bat_ctrl.h"
//------------------------------------------------------------------------------
//��������
void ad_val_deal(void);  //adֵ���������ADת�������е���

void ad_temp_calc(void); //ad�¶ȼ����������ѭ�������е���

int16_t get_temp1(uint16_t lus_ad);     //����adֵ���õ��¶�ֵ1

int16_t get_temp2(uint16_t lus_ad);     //����adֵ���õ��¶�ֵ2

int16_t get_temp3(uint16_t lus_ad);     //����adֵ���õ��¶�ֵ3

int16_t get_temp4(uint16_t lus_ad);     //����adֵ���õ��¶�ֵ4

//------------------------------------------------------------------------------
//��������
flag_type flg_adc;
//----------------------------------------------------------
#define   AD_SUM_INDEX   6    //��ƽ����λֵ
#define   AD_SUM_CNT     64   //��ƽ������

#define   AD_VAL_MIN     20    //AD����ֵ ԭ5
#define   AD_VAL_MAX     1000  //AD����ֵ

//----------------------------------------------------------
uint16_t  gus_ad_val;             //�� ADCRת������Ĵ����л�ȡ��ֵ
uint16_t  gus_ad_buffer[12];      //ֻ����˲ʱ��ѹʱ�ù�
uint32_t  gul_ad_sum_buffer[12];  //12��Ԫ�أ�û��Ԫ��(ͨ��)����64���ۼ�
uint16_t  gus_ad_aver_buffer[12];
uint8_t   guc_ad_index;
uint8_t   guc_ad_cnt;
//----------------------------------------------------------
uint8_t   guc_ad_fault_cnt[12];         //ad���ϴ���������
uint8_t   guc_ad_fault_release_cnt[12]; //ad�����ͷŴ���������

//----------------------------------------------------------
int16_t   gss_TA;   //��������           �Ŵ�10������������
int16_t   gss_THW;  //���´�����
int16_t   gss_TC;   //�������¶ȴ�����
int16_t   gss_TE;   //�Ƚ������¶ȴ�����

int16_t   gss_VAC;  //��Դ��ѹ
uint8_t   guc_12V;  //12V��ѹ

int16_t   gss_Tbat1;     //﮵���¶�1
int16_t   gss_Tbat2;     //﮵���¶�2

uint8_t   guc_bat_Vin;   //��س���ѹ
uint8_t   guc_bat_Vout;  //��طŵ��ѹ �Ŵ�10��
uint8_t   guc_bat_Vout_instant;  //��طŵ��ѹ˲ʱֵ


uint32_t  gul_bat_Vin;   //peak���������ʱʹ��
uint32_t  gul_bat_Vout;  //peak���������ʱʹ��
int16_t   gss_bat_I;     //��س�����

int16_t   gss_adg_Temp[10];
int16_t   gss_adg_Temp2;

uint8_t   guc_ad_calc_cnt;




/*********************************************************************************************************************************
��������:  ��ÿһ���ɼ���ͨ������Ƿ��й���(��·/��·)

����λ��: ad_convert_deal() ��,�����������1ms��ʱ��-----------ok 
**********************************************************************************************************************************/
void ad_val_deal(void)   //adֵ���������ADת�������е���  peak 1ms�� 
{
    //------------------------------------------------------
    //adֵ�ۼ�
    gus_ad_buffer[guc_ad_index] = gus_ad_val;
    gul_ad_sum_buffer[guc_ad_index] += gus_ad_val;
    //------------------------------------------------------
    
    //adֵ��ƽ��
    if (guc_ad_cnt == 0)
    {
        gus_ad_aver_buffer[guc_ad_index] = (uint16_t) (gul_ad_sum_buffer[guc_ad_index] >> AD_SUM_INDEX);
        gul_ad_sum_buffer[guc_ad_index] = 0;
    }
    
    //------------------------------------------------------
    //ad�����ж�����Ͻ��
    if ((gus_ad_val < AD_VAL_MIN) || (gus_ad_val > AD_VAL_MAX))
    {
        guc_ad_fault_release_cnt[guc_ad_index] = 0;
        //--------------------------------------------------
        guc_ad_fault_cnt[guc_ad_index]++;
        if (guc_ad_fault_cnt[guc_ad_index] >= 100)
        {
            guc_ad_fault_cnt[guc_ad_index] = 0;
            
            switch (guc_ad_index)
            {
                case 0:
                    if ((bflg_THW_fault == 0) && (ram_para[num_comp2_THW_shield] == 0))
                    {
                        bflg_THW_fault = 1;            //���´������������
                        gus_trip_code1 |= THW_S_CODE;
                    }
                    break;
                case 1:
                    
                    break;
                case 2:
                    if ((bflg_TC_fault == 0) && (ram_para[num_comp2_TC_shield] == 0))
                    {
                        bflg_TC_fault = 1;             //����
                        gus_trip_code1 |= TC_S_CODE;
                    }
                    break;
                case 3:
                  /*  if ((bflg_TE_fault == 0) && (ram_para[num_comp2_TE_shield] == 0))
                    {
                        bflg_TE_fault = 1;            //�Ƚ�����
                        gus_trip_code1 |= 0x0040;
                    }
                    break;*/
                    
                    if(gss_factory_parm_value[DISP_FACTORY_SS] == 1)          //�Ƚ��������ι���
                    {                    
                        bflg_TE_fault = 0;
                    }
                    else
                    {
                        bflg_TE_fault = 1;
                        gus_trip_code1 |= TE_S_CODE;
                    }                 
                    break;
                case 4:
                    
                    break;
                case 5:
                    
                    break;
                case 6:
                    
                    break;
                case 7:
                    
                    break;
                case 8:
                    if ((bflg_TA_fault == 0) && (ram_para[num_comp2_PT100_shield] == 0))
                    {
                        bflg_TA_fault = 1;                //PT100
                        gus_trip_code1 |= TA_S_CODE;  
                    }
                    break;
                case 9:
                    if (bflg_Tbat2_fault == 0)            //����¶�2 CN23
                    {
                    	  bflg_Tbat2_fault = 1;
                    	  bflg_bat_2_type = LEAD_ACID_BAT; //Ǧ���޴�������·��adֵӦΪ1024(��·ʱҲĬ��Ǧ��)            
                    }      
                    break;
                case 10:
                    if (bflg_Tbat1_fault == 0)            //����¶�1 CN22
                    { 
                    	  bflg_Tbat1_fault = 1;
                    	  bflg_bat_1_type = LEAD_ACID_BAT;
                    }
                    break;
                case 11:
                    
                    break;
            }
        }
    }
    else
    {
        guc_ad_fault_cnt[guc_ad_index] = 0;  
        //--------------------------------------------------
        guc_ad_fault_release_cnt[guc_ad_index]++;
        if (guc_ad_fault_release_cnt[guc_ad_index] >= 100)
        {
            guc_ad_fault_release_cnt[guc_ad_index] = 0;
            
            switch (guc_ad_index)
            {
                case 0:
                    bflg_THW_fault = 0;         //����
                    gus_trip_code1 &= ~THW_S_CODE;
                    break;
                case 1:
                    
                    break;
                case 2:
                    bflg_TC_fault = 0;         //������
                    gus_trip_code1 &= ~TC_S_CODE;
                    break;
                case 3:
                    bflg_TE_fault = 0;         //�Ƚ�����
                    gus_trip_code1 &= ~TE_S_CODE;
                    break;
                case 4:
                    
                    break;
                case 5:
                    
                    break;
                case 6:
                    
                    break;
                case 7:
                    
                    break;
                case 8:
                    bflg_TA_fault = 0;       // PT100
                    gus_trip_code1 &= ~TA_S_CODE;
                    break;
                case 9:
                    bflg_Tbat2_fault = 0;   //����¶�2
                    bflg_bat_2_type = LI_BAT;
                    break;
                case 10:
                    bflg_Tbat1_fault = 0;   //����¶�1
                    bflg_bat_1_type = LI_BAT;
                    break;
                case 11:
                    
                    break;
            }
        }
    }
    //------------------------------------------------------
    //�����ۼ�
    guc_ad_index++;
    if (guc_ad_index >= 12)              //peak 12��ͨ�����ɼ���һ��               
    {
        guc_ad_index = 0;
        //------------------------------
        if (guc_ad_cnt == 0)
        {
            bflg_allow_ad_calc = 1;     //������ad�����־, ����1ʱ ad_temp_calc() ����ִ�У�����0��
        }
        //------------------------------
        guc_ad_cnt++;
        if (guc_ad_cnt >= AD_SUM_CNT)   //12·AD���ɼ���64�κ��������㣻
        {
            guc_ad_cnt = 0;
        }
    }
}

/***************************************************************************************************************************
��������: �Բ�ͬͨ����ADֵ�����м���õ���Ӧ���¶�ֵ����ѹֵ������ֵ�ȣ�

��������: bflg_allow_ad_calc == 1 ʱ��

����λ��: ��ѭ����     -------------------------------------------ok
****************************************************************************************************************************/
void ad_temp_calc(void)  //ad�¶ȼ����������ѭ�������е���
{
    int32_t lsl_tmp;
    //------------------------------------------------------------------------------------------------------
    //gss_THW = get_temp1(gus_ad_aver_buffer[0]) + ram_para[num_comp2_THW_ref];  //ԭ����
    gss_THW = get_temp1(gus_ad_aver_buffer[0]) + gss_factory_parm_value[DISP_FACTORY_CA];                 //��ӻ���У׼
    gss_TC = get_temp1(gus_ad_aver_buffer[2]);
    
    gss_TE = get_temp2(gus_ad_aver_buffer[3]);
    //debug
    //gss_TA = get_temp3(gus_ad_aver_buffer[8]) + ram_para[num_comp2_PT100_ref];
    //gss_TA = get_temp3(gus_ad_aver_buffer[8]) + (gss_para[PT100_DELT_TEMP] * 10); //ԭ����
    lsl_tmp = gss_TA;
    gss_TA = get_temp3(gus_ad_aver_buffer[8]) + (gss_factory_parm_value[DISP_FACTORY_Ct] * 10);              //���У׼  ��temp3�зŴ��
    gss_TA = (( ((int16_t)lsl_tmp)*3 + gss_TA)>> 2); 
    
    gss_Tbat2 = get_temp4(gus_ad_aver_buffer[9]);
    gss_Tbat1 = get_temp4(gus_ad_aver_buffer[10]);
    //------------------------------------------------------------------------------------------------------
    //��Դ��ѹ���㣨��λ��1V�� peak--220v ---��ʱ���ѹ�������36������Ϊ��ѹ���������ȣ���δ֪��
    lsl_tmp = gus_ad_aver_buffer[4];
    //lsl_tmp *= 185;
    lsl_tmp *= 180;
    lsl_tmp >>= 9;
    
    //gss_VAC = ((int16_t) (lsl_tmp)) + ram_para[num_comp2_VAC_ref];    //ԭ����
    gss_VAC = ((int16_t) (lsl_tmp)) + gss_factory_parm_value[DISP_FACTORY_CU];                   //��ӵ�ѹУ׼
    
    //------------------------------------------------------------------------------------------------------
    //12V���㣨��λ��0.1V�� 
    lsl_tmp = gus_ad_aver_buffer[5];      //���ݵ����ѹ��5v��Ӧ1024����(10λad)
    //lsl_tmp *= 150;
    lsl_tmp *= 150;                       //peak �Ŵ�10�������Ժ���Ƚϵ���ҲҪ�Ŵ�10��
    lsl_tmp >>= 10;
    
    guc_12V = (uint8_t) (lsl_tmp);
    
    //------------------------------------------------------------------------------------------------------
    gss_disp_see_buffer[0] = gss_VAC;  //�����ʾsee����ʱʹ��
    gss_disp_see_buffer[1] = gss_THW;
    
    //------------------------------------------------------------------------------------------------------
    //��س���ѹ���㣨��λ��0.1V��
    lsl_tmp = gus_ad_aver_buffer[11];
    //lsl_tmp *= 224;
    lsl_tmp *= 224;                   //�Ŵ�10��
    gul_bat_Vin = lsl_tmp;            //gul_bat_Vinֻ�ڼ������ʱ��,û��>>10�������ǷŴ���1024��
    lsl_tmp >>= 10;
    
    guc_bat_Vin = (uint8_t) (((guc_bat_Vin * 3) + ((uint8_t) (lsl_tmp))) >> 2); // peak ��ľ�˲�: �ϴ���ֵ����3�����ڵ�ֵ��������4�� 
    
    //------------------------------------------------------------------------------------------------------
    //��طŵ��ѹ���㣨��λ��0.1V��
    lsl_tmp = gus_ad_aver_buffer[7];
    //lsl_tmp *= 224;
    lsl_tmp *= 224;                   //�Ŵ�10��
    gul_bat_Vout = lsl_tmp;           //gul_bat_Voutֻ�ڼ������ʱ��,û��>>10�������ǷŴ���1024��
    lsl_tmp >>= 10;
    
    guc_bat_Vout = (uint8_t) (((guc_bat_Vout * 3) + ((uint8_t) (lsl_tmp))) >> 2);//��ľ�˲�
    
    //------------------------------------------------------------------------------------------------------
    lsl_tmp = gus_ad_buffer[7];                  //ʹ��˲ʱΪ���ж��ٶȿ�
    //lsl_tmp *= 224;
    lsl_tmp *= 224;                              //�Ŵ�10��
    //gul_bat_Vout = lsl_tmp;                    //peakɾ������ƽ����                    
    lsl_tmp >>= 10;
    guc_bat_Vout_instant = (uint8_t) (lsl_tmp);  //peak �жϵ���Ƿ�����  
    
    //------------------------------------------------------------------------------------------------------
    //��س��������㣨��λ��1mA��
    lsl_tmp = gul_bat_Vin;
    lsl_tmp -= gul_bat_Vout;
    lsl_tmp *= 10000;                  //1.82-->182 �Ŵ�100�� �����0.1v-->1mA�Ŵ�100�����Դ˴�Ϊ10000
    //lsl_tmp /= 182;
    lsl_tmp /= 180;
    lsl_tmp >>= 10;                    //gul_bat_Vin������û�г���1024��Ϊ�˷Ŵ��׼��㣬�����ڴ˴�����
    
    gss_bat_I = ((gss_bat_I * 3) + ((int16_t) (lsl_tmp))) >> 2;   //��ľ�˲�  δ�Ŵ󣬵�λ��mA
    
    //------------------------------------------------------------------------------------------------------
    guc_ad_calc_cnt++;              //peak �տ�ʼΪ���ȶ���ɼ��������ж�
    if (guc_ad_calc_cnt >= 5)
    {
    	  bflg_ad_calc_ok = 1;
    }
}

/***************************************************************************************************************************
��������: ���ݻ�ȡ��ADֵ���ͱ���е�ADֵ�Ƚϣ���ȡ��ADֵ�ڱ���е�����ֵ������
          ����ֵ���Ե�����в��ҵ���Ӧ����ֵ���¶�ֵ��

����λ��: ad_temp_calc() �У�

��ȡ����: ��ȡ���¡��������¶�ֵ(����������ͬһ��������)
****************************************************************************************************************************/
int16_t get_temp1(uint16_t lus_ad)      //����adֵ���õ��¶�ֵ1
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP1_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp1[lss_temp_index])
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
            if (lus_ad < ad_to_temp1[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp1[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp1[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP1_TAB_LENGTH)
            {
                lss_temp_index = TEMP1_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > ad_to_temp1[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp1[lss_temp_index]);
    }
    //����¶ȸ�1�棬�ڴμ�һ��ָ�룬�¶Ƚ�1��
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp1_for_ad[lss_temp_index];
}
//------------------------------------------------------------------------------
/*******************************************************************************
��������: 

����λ��: ad_temp_calc() �У�

��ȡ����: ��ȡ�Ƚ��������¶�ֵ 
********************************************************************************/
int16_t get_temp2(uint16_t lus_ad)      //����adֵ���õ��¶�ֵ2
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP2_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp2[lss_temp_index])
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
            if (lus_ad < ad_to_temp2[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp2[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp2[lss_temp_index])
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
            if (lus_ad > ad_to_temp2[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp2[lss_temp_index]);
    }
    //����¶ȸ�1�棬�ڴμ�һ��ָ�룬�¶Ƚ�1��
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp2_for_ad[lss_temp_index];
}
//------------------------------------------------------------------------------
/*******************************************************************************
��������: 

����λ��: ad_temp_calc() �У�

��ȡ����: ��ȡPT100���¶�ֵ 
********************************************************************************/
int16_t get_temp3(uint16_t lus_ad)      //����adֵ���õ��¶�ֵ3
{
    int16_t lss_delt;
    
    int16_t lss_temp_index;
    lss_temp_index = (TEMP3_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp3[lss_temp_index])
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
            if (lus_ad < ad_to_temp3[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp3[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp3[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP3_TAB_LENGTH)
            {
                lss_temp_index = TEMP3_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > ad_to_temp3[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp3[lss_temp_index]);
    }
    //����¶ȸ�1�棬�ڴμ�һ��ָ�룬�¶Ƚ�1��
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    lss_delt = lus_ad;
    lss_delt -= ad_to_temp3[lss_temp_index];
    lss_delt *= 10;    //��Ϊpt100������Ŵ���10�������Դ˴���С�����ֵ�ֵҲҪ�Ŵ�10��
    lss_delt >>= 2;    //�μ�"ѪҺ"
    
    //return temp3_for_ad[lss_temp_index];
    return (temp3_for_ad[lss_temp_index] * 10 - lss_delt);
}
//------------------------------------------------------------------------------
/*******************************************************************************
��������: 

����λ��: ad_temp_calc() �У�

��ȡ����: ��ȡ��ص��¶�ֵ 
********************************************************************************/
int16_t get_temp4(uint16_t lus_ad)      //����adֵ���õ��¶�ֵ4
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP4_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp4[lss_temp_index])
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
            if (lus_ad < ad_to_temp4[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp4[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp4[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP4_TAB_LENGTH)
            {
                lss_temp_index = TEMP4_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > ad_to_temp4[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp4[lss_temp_index]);
    }
    //����¶ȸ�1�棬�ڴμ�һ��ָ�룬�¶Ƚ�1��
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp4_for_ad[lss_temp_index];
}
//------------------------------------------------------------------------------
#endif
