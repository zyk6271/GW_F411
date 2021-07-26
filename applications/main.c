/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     RT-Thread    first version
 */

#include <rtthread.h>
#include "flashwork.h"
#include "radio.h"
#include "device.h"
#include "key.h"
#include "wifi-uart.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    flash_Init();
    LoadDevice2Memory();
    Key_Reponse_Init();
    button_Init();
    Radio_Task_Init();
    WiFi_Init();
    while (1)
    {
        rt_thread_mdelay(1000);
    }
}
