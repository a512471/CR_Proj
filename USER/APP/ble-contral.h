#ifndef __BLE_CONTRAL_H__
#define __BLE_CONTRAL_H__

#include "includes.h" 

#define USART2_REC_LEN  200

extern uint16_t USART2_RX_STA;

void BLE_init(void);
ErrorStatus Deal_BLE_Dat(const uint8_t *input, uint8_t *output);

#ifdef debug
    void puts_debug(const uint8_t* str);
#endif

#endif








