/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#ifndef RADIO_RADIO_DECODER_H_
#define RADIO_RADIO_DECODER_H_

#include "rtthread.h"

void Rx_Done_Callback(uint8_t *rx_buffer,uint8_t rx_len,int8_t rssi);
void Set_Learn_Flag(uint8_t value);
uint8_t Get_Learn_Flag(void);

#endif /* RADIO_RADIO_DECODER_H_ */
