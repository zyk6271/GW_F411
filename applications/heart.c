/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-27     Rick       the first version
 */
#include "rtthread.h"
#include "heart.h"
#include "flashwork.h"
#include "radio_encoder.h"
#include "wifi-api.h"

#define DBG_TAG "heart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t heart_t = RT_NULL;
extern Device_Info Global_Device;

void heart_callback(void *parameter)
{
    uint8_t num;
    LOG_D("Heart Check Init Success\r\n");
    while(1)
    {
        rt_thread_mdelay(60000*3);
        for(num=1;num<=Global_Device.Num;num++)
        {
            if(Global_Device.ID[num]!=0 && Global_Device.Bind_ID[num]==0)
            {
                if(Global_Device.HeartRecv[num] == 0)
                {
                    switch(Global_Device.HeartRetry[num])
                    {
                    case 0:
                        Global_Device.HeartRetry[num] = 1;
                        GatewayDataEnqueue(Global_Device.ID[num],0,0,3,0);
                        LOG_D("Rerty 1 fail\r\n");
                        break;
                    case 1:
                        Global_Device.HeartRetry[num] = 2;
                        GatewayDataEnqueue(Global_Device.ID[num],0,0,3,0);
                        LOG_D("Rerty 2 fail\r\n");
                        break;
                    case 2:
                        Global_Device.HeartRetry[num] = 0;
                        Flash_Set_Heart(Global_Device.ID[num],0);
                        LOG_D("Rerty 3 fail\r\n");
                        break;
                    }
                }
                else
                {
                    LOG_D("Get Heart\r\n");
                    Flash_Set_Heart(Global_Device.ID[num],1);
                    Global_Device.HeartRetry[num] = 0;
                    Global_Device.HeartRecv[num] = 0;
                    GatewayDataEnqueue(Global_Device.ID[num],0,0,3,0);
                }
            }
            rt_thread_mdelay(10000);
        }
    }
}
void Heart_Init(void)
{
    heart_t = rt_thread_create("heart", heart_callback, RT_NULL, 2048, 10, 10);
    if(heart_t == RT_NULL)
    {
        return;
    }
    rt_thread_startup(heart_t);
}
