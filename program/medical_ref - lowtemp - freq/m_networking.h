#ifndef _M_NETWORKING_H_
#define _M_NETWORKING_H_

extern void net_com_interval(void);
extern void net_com_tx_int(void);
extern void net_com_deal(void);
extern void net_led_disp1_deal_slave_1(void);
extern void net_led_disp2_deal_slave_1(void);
extern void net_led_disp3_deal_slave_1(void);
extern void net_led_disp1_deal_slave_2(void);
extern void net_led_disp2_deal_slave_2(void);
extern void net_led_disp3_deal_slave_2(void);
extern void net_led_disp1_deal_slave_3(void);
extern void net_led_disp2_deal_slave_3(void);
extern void net_led_disp3_deal_slave_3(void);


extern flag_type flg_net_com;
        #define bflg_net_tx_allow     flg_net_com.bits.bit0
        #define bflg_net_com_tx_busy  flg_net_com.bits.bit1
        #define bflg_net_com_tx_ok    flg_net_com.bits.bit2


















#endif
