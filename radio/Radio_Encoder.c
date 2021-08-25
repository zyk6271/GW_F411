/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include "drv_spi.h"
#include <string.h>
#include "AX5043.h"
#include "Radio_Config.h"
#include "Radio.h"
#include "Radio_Encoder.h"
#include "led.h"
#include "flashwork.h"

#define DBG_TAG "RF_EN"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_thread_t Radio_QueueTask = RT_NULL;
rt_timer_t FreqRefresh = RT_NULL;

uint32_t Self_Id = 0;
uint32_t Self_Default_Id = 98989898;
uint32_t Self_Counter = 0;

typedef struct
{
    uint8_t NowNum;
    uint8_t TargetNum;
    uint8_t Type[30];
    uint32_t Taget_Id[30];
    uint32_t Device_Id[30];
    uint8_t Counter[30];
    uint8_t Command[30];
    uint8_t Data[30];
}Radio_Queue;

Radio_Queue Main_Queue={0};

void Tx_Done_Callback(uint8_t *rx_buffer,uint8_t rx_len)
{
    //LOG_D("Send ok\r\n");
}
void sendtest(void)
{
    RadioSend(99999999,1,3,1);
}
MSH_CMD_EXPORT(sendtest,sendtest);
void Start_Learn(void)
{
    if(Get_MainNums()==RT_EOK)
    {
        RadioSend(99999999,1,3,1);
    }
    else
    {
        learn_fail();
    }
}
void RadioSend(uint32_t Taget_Id,uint8_t counter,uint8_t Command,uint8_t Data)
{
    uint8_t check = 0;
    uint8_t buf[35];
    if(counter<255)counter++;
    else counter=0;

    sprintf((char *)(buf),"{%08ld,%08ld,%03d,%02d,%d}",\
                                            Taget_Id,\
                                            Self_Id,\
                                            counter,\
                                            Command,\
                                            Data);

    for(uint8_t i = 0 ; i < 28 ; i ++)
    {
        check += buf[i];
    }
    buf[28] = ((check>>4) < 10)?  (check>>4) + '0' : (check>>4) - 10 + 'A';
    buf[29] = ((check&0xf) < 10)?  (check&0xf) + '0' : (check&0xf) - 10 + 'A';
    buf[30] = '\r';
    buf[31] = '\n';
    Normal_send(buf,32);
}
void GatewayDataEnqueue(uint32_t target_id,uint32_t device_id,uint8_t rssi,uint8_t control,uint8_t value)
{
    uint8_t NumTemp = Main_Queue.TargetNum;
    if(NumTemp<30)
    {
        NumTemp ++;
        LOG_D("Queue Num Increase,Value is %d\r\n",NumTemp);
    }
    else
    {
        LOG_I("Queue is Full,Value is %d\r\n",NumTemp);
        return;
    }
    Main_Queue.Type[NumTemp] = 1;
    Main_Queue.Taget_Id[NumTemp] = target_id;
    Main_Queue.Device_Id[NumTemp] = device_id;
    Main_Queue.Counter[NumTemp] = rssi;
    Main_Queue.Command[NumTemp] = control;
    Main_Queue.Data[NumTemp] = value;
    Main_Queue.TargetNum++;
    LOG_D("GatewayDataEnqueue Success\r\n");
}
void GatewayDataSend(uint32_t target_id,uint32_t device_id,uint8_t rssi,uint8_t control,uint8_t value)
{
    uint8_t buf[50]={0};
    sprintf((char *)(&buf),"G{%08ld,%08ld,%08ld,%03d,%03d,%02d}G",\
                                            target_id,\
                                            Self_Id,\
                                            device_id,\
                                            rssi,\
                                            control,\
                                            value);
    Normal_send(buf,41);
}
void RadioEnqueue(uint32_t Taget_Id,uint8_t counter,uint8_t Command,uint8_t Data)
{
    uint8_t NumTemp = Main_Queue.TargetNum;
    if(NumTemp<30)
    {
        NumTemp ++;
        LOG_D("Queue Num Increase,Value is %d\r\n",NumTemp);
    }
    else
    {
        LOG_I("Queue is Full,Value is %d\r\n",NumTemp);
        return;
    }
    Main_Queue.Type[NumTemp] = 0;
    Main_Queue.Taget_Id[NumTemp] = Taget_Id;
    Main_Queue.Counter[NumTemp] = counter;
    Main_Queue.Command[NumTemp] = Command;
    Main_Queue.Data[NumTemp] = Data;
    Main_Queue.TargetNum++;
    LOG_D("RadioEnqueue Success\r\n");
}
void RadioDequeue(void *paramaeter)
{
    LOG_D("Queue Init Success\r\n");
    while(1)
    {
        if(Main_Queue.NowNum == Main_Queue.TargetNum)
        {
            Main_Queue.NowNum = 0;
            Main_Queue.TargetNum = 0;
        }
        else if(Main_Queue.TargetNum>0 && Main_Queue.TargetNum>Main_Queue.NowNum)
        {
            Main_Queue.NowNum++;
            switch(Main_Queue.Type[Main_Queue.NowNum])
            {
            case 0:
                RadioSend(Main_Queue.Taget_Id[Main_Queue.NowNum],Main_Queue.Counter[Main_Queue.NowNum],Main_Queue.Command[Main_Queue.NowNum],Main_Queue.Data[Main_Queue.NowNum]);
                LOG_D("Normal Send With Now Num %d,Target Num is %d,Target_Id %ld,counter %d,command %d,data %d\r\n",Main_Queue.NowNum,Main_Queue.TargetNum,Main_Queue.Taget_Id[Main_Queue.NowNum],Main_Queue.Counter[Main_Queue.NowNum],Main_Queue.Command[Main_Queue.NowNum],Main_Queue.Data[Main_Queue.NowNum]);
                rt_thread_mdelay(200);
                break;
            case 1:
                GatewayDataSend(Main_Queue.Taget_Id[Main_Queue.NowNum],Main_Queue.Device_Id[Main_Queue.NowNum],Main_Queue.Counter[Main_Queue.NowNum],Main_Queue.Command[Main_Queue.NowNum],Main_Queue.Data[Main_Queue.NowNum]);
                LOG_I("GatewaySend With Now Num %d,Target Num is %d,Type is %d,Target_Id %ld,Device_Id %ld,control %d,value %d\r\n",Main_Queue.NowNum,Main_Queue.TargetNum,Main_Queue.Type[Main_Queue.NowNum],Main_Queue.Taget_Id[Main_Queue.NowNum],Main_Queue.Device_Id[Main_Queue.NowNum],Main_Queue.Command[Main_Queue.NowNum],Main_Queue.Data[Main_Queue.NowNum]);
                rt_thread_mdelay(200);
                break;
            default:break;
            }
            //LOG_D("Dequeue Success\r\n");
        }
        rt_thread_mdelay(50);
    }
}
void RadioDequeueTaskInit(void)
{
//    int *p;
//    p=(int *)(0x0801FFFC);//这就是已知的地址，要强制类型转换
//    Self_Id = *p;//从Flash加载ID
    Self_Id = 0;
    if(Self_Id==0xFFFFFFFF || Self_Id==0)
    {
        Self_Id = Self_Default_Id;
    }
    Radio_QueueTask = rt_thread_create("Radio_QueueTask", RadioDequeue, RT_NULL, 1024, 10, 10);
    if(Radio_QueueTask)rt_thread_startup(Radio_QueueTask);
}
MSH_CMD_EXPORT(RadioDequeueTaskInit,RadioDequeueTaskInit);
