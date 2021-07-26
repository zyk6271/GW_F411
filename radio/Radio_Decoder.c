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
#include "Radio_Decoder.h"
#include "Radio_Encoder.h"
#include "Flashwork.h"
#include "led.h"
#include "key.h"
#include "pin_config.h"
#include "wifi-api.h"

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

typedef struct
{
    int type;
    long Target_ID;
    long From_ID;
    long Device_ID;
    int Counter;
    int Command ;
    int Data;
    int Rssi;
}Message;

uint8_t Learn_Flag=1;

extern uint32_t Self_Id;
uint32_t Main_ID = 0;

uint8_t Check_Valid(uint32_t From_id)
{
    return Flash_Get_Key_Valid(From_id);
}
void Device_Learn(Message buf)
{
    switch(buf.Data)
    {
    case 1:
        if(Check_Valid(buf.From_ID))//如果数据库不存在该值
        {
            RadioEnqueue(0,buf.From_ID,buf.Counter,3,2);
            Add_Main(buf.From_ID);
            LOG_D("Send ACK\r\n");
        }
        else//存在该值
        {
            RadioEnqueue(0,buf.From_ID,buf.Counter,3,2);
            LOG_D("Include This Device，Send Ack\r\n");
        }
        break;
    case 2:
        if(Check_Valid(buf.From_ID))//如果数据库不存在该值
        {
            LOG_D("Ack Not Include This Device\r\n");
        }
        else//存在该值
        {
            LOG_D("Learn Success\r\n");
            Device_Add(buf.From_ID);
        }
        break;
    }
}
void NormalSolve(uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]==0x0A&&rx_buffer[rx_len-2]==0x0D)
     {
         sscanf((const char *)&rx_buffer[1],"{%ld,%ld,%d,%d,%d}",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Counter,&Rx_message.Command,&Rx_message.Data);
         if(Rx_message.Target_ID==Self_Id)
         {
             LOG_D("NormalSolve verify ok\r\n");
             switch(Rx_message.Command)
             {
             case 3://学习
                 Device_Learn(Rx_message);
                 break;
             case 9://学习
                 //Device_Learn(Rx_message);
                 break;
             }
         }
     }
}
void GatewaySyncSolve(uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]=='A')
    {
        LOG_D("GatewaySyncSolve verify ok\r\n");
        sscanf((const char *)&rx_buffer[2],"{%d,%ld,%ld,%ld,%d,%d}",&Rx_message.type,&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Device_ID,&Rx_message.Command,&Rx_message.Data);
        if(Rx_message.Target_ID == Self_Id && Check_Valid(Rx_message.From_ID) == RT_EOK)
        {
            switch(Rx_message.type)
            {
            case 1:
                Slave_Heart(Rx_message.Device_ID,Rx_message.Command);//心跳
                break;
            case 3:
                Device_Add(Rx_message.Device_ID);//增加终端
                break;
            case 4:
                Door_Add_WiFi(Rx_message.Device_ID);//增加门控
                break;
            case 5://删除全部
                break;
            }
        }
    }
}
void GatewayWarningSolve(uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]=='B')
    {
        LOG_D("GatewayWarningSolve verify ok\r\n");
        sscanf((const char *)&rx_buffer[2],"{%ld,%ld,%ld,%d,%d,%d}",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Device_ID,&Rx_message.Rssi,&Rx_message.Command,&Rx_message.Data);
        if(Rx_message.Target_ID == Self_Id && Check_Valid(Rx_message.From_ID) == RT_EOK)
        {
            LOG_D("WariningUpload Device ID is %ld,type is %d,value is %d\r\n",Rx_message.Device_ID,Rx_message.Command,Rx_message.Data);
            switch(Rx_message.Command)
            {
            case 1:
                WariningUpload(Rx_message.From_ID,1,Rx_message.Data);//主控水警
                break;
            case 2:
                WariningUpload(Rx_message.From_ID,0,Rx_message.Data);//主控阀门
                break;
            case 3:
                WariningUpload(Rx_message.From_ID,2,Rx_message.Data);//主控测水线掉落
                break;
            case 4:
                Slave_Heart(Rx_message.Device_ID,Rx_message.Rssi);
                WariningUpload(Rx_message.Device_ID,0,Rx_message.Data);//终端掉线
                break;
            case 5:
                Slave_Heart(Rx_message.Device_ID,Rx_message.Rssi);
                WariningUpload(Rx_message.Device_ID,1,Rx_message.Data);//终端水警
                break;
            case 6:
                Slave_Heart(Rx_message.Device_ID,Rx_message.Rssi);
                WariningUpload(Rx_message.Device_ID,2,Rx_message.Data);//终端低电量
                break;
            case 7:
                Warning_WiFi(Rx_message.From_ID,Rx_message.Data);//报警状态
                break;
            case 8://NTC报警
                break;
            case 9://网关离线
                break;
            }
        }
    }
    else
    {
        LOG_D("GatewayControlSolve verify Fail\r\n");
    }
}
void GatewayControlSolve(uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]=='C')
    {
        LOG_D("GatewayControlSolve verify ok\r\n");
        sscanf((const char *)&rx_buffer[2],"{%ld,%ld,%ld,%d,%d,%d}",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Device_ID,&Rx_message.Rssi,&Rx_message.Command,&Rx_message.Data);
        if(Rx_message.Target_ID == Self_Id && Check_Valid(Rx_message.From_ID) == RT_EOK)
        {
            switch(Rx_message.Command)
            {
            case 1:
                MotoUpload(Rx_message.From_ID,Rx_message.Data);//主控开关阀
                break;
            case 2:
                RemoteUpload(Rx_message.From_ID,Rx_message.Data);//终端开关阀
                //RemoteUpload(Rx_message.Device_ID,Rx_message.Data);//终端开关阀
                Slave_Heart(Rx_message.Device_ID,Rx_message.Rssi);//设备RSSI更新
                break;
            case 3:
                Delay_WiFi(Rx_message.Device_ID,Rx_message.Data);//延时开关
                Slave_Heart(Rx_message.Device_ID,Rx_message.Rssi);//设备RSSI更新
                break;
            case 4:
                Heart_Report(Rx_message.From_ID);
                break;
            case 5:
                MotoUpload(Rx_message.From_ID,Rx_message.Data);
                Ack_Report(Rx_message.From_ID);
                break;
            }
        }
    }
    else
    {
        LOG_D("GatewayControlSolve verify Fail\r\n");
    }
}
void Rx_Done_Callback(uint8_t *rx_buffer,uint8_t rx_len,int8_t rssi)
{
    switch(rx_buffer[1])
    {
    case '{':NormalSolve(rx_buffer,rx_len);
        break;
    case 'A':GatewaySyncSolve(rx_buffer,rx_len);
        break;
    case 'B':GatewayWarningSolve(rx_buffer,rx_len);
        break;
    case 'C':GatewayControlSolve(rx_buffer,rx_len);
        break;
    }
}

