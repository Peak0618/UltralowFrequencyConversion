#ifndef _M_SECRET_H_
#define _M_SECRET_H_






extern void secret_key_main_loop(void);  
extern void secret_key_led_disp1_deal(void);     
extern void secret_key_read_key_deal(void);        
extern void secret_key_decode_deal(void);   

extern flag_type flg_secret;
        #define   bflg_secret_key_1                      flg_secret.bits.bit0  //ÃÜÔ¿
        #define   bflg_secret_key_2                      flg_secret.bits.bit1
        #define   bflg_secret_key_3                      flg_secret.bits.bit2
        #define   bflg_secret_key_ok                     flg_secret.bits.bit3


















#endif
