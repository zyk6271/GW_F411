/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     Rick       the first version
 */
#ifndef APPLICATIONS_FLASHWORK_H_
#define APPLICATIONS_FLASHWORK_H_

#include "rtthread.h"

typedef struct
{
    uint32_t Num;
    uint32_t ID[20];
}Device_Info;

int flash_Init(void);
uint8_t Add_Main(uint32_t Device_ID);
void Delete_Main(void);
void LoadDevice2Memory(void);
uint32_t Flash_Get_Key_Value(uint32_t key);
uint8_t Flash_Get_Key_Valid(uint32_t Device_ID);

#endif /* APPLICATIONS_FLASHWORK_H_ */
