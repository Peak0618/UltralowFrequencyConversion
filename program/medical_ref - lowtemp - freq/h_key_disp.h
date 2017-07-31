#ifndef _H_KEY_DISP_H_
#define _H_KEY_DISP_H_


//declaration
extern void led_disp1_deal(void); 
extern void led_disp2_deal(void); 
extern void led_disp3_deal(void); 
extern void view_error_code_delaytime (void);
extern void lamp_disp_deal(void);
extern void led_power_off_disp_delaytime(void);



extern flag_type flg_key_disp;      
        #define   bflg_power_off_dis        flg_key_disp.bits.bit0  //断电时数码管显示


























#endif
