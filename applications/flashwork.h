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

#define MaxSupport 50
#define MainSupport 3
typedef struct
{
    uint8_t Num;
    uint8_t MainNum;
    uint8_t Heart[MaxSupport];
    uint8_t HeartRetry[MaxSupport];
    uint8_t HeartRecv[MaxSupport];
    uint32_t ID[MaxSupport];
    uint32_t Bind_ID[MaxSupport];
}Device_Info;
typedef struct
{
    uint8_t Num;
    uint32_t ID[MaxSupport];
}Remote_Info;

int flash_Init(void);
uint8_t Del_MainBind(uint32_t Device_ID);
uint8_t Del_Device(uint32_t Device_ID);
void LoadDevice2Memory(void);
void DeleteAllDevice(void);
uint32_t Flash_Get_Key_Value(uint8_t type,uint32_t key);
uint8_t Flash_Get_Key_Valid(uint32_t Device_ID);
uint8_t MainAdd_Flash(uint32_t Device_ID);
void Flash_Heart_Change(uint32_t Device_ID,uint32_t value);
uint8_t SlaveAdd_Flash(uint32_t Device_ID,uint32_t Bind_ID);
uint8_t DoorAdd_Flash(uint32_t Device_ID,uint32_t Bind_ID);
uint32_t GetBindID(uint32_t Device_ID);
uint8_t Flash_Get_Heart(uint32_t Device_ID);
uint8_t Flash_Set_Heart(uint32_t Device_ID,uint8_t heart);
uint8_t Get_MainNums(void);

#endif /* APPLICATIONS_FLASHWORK_H_ */
