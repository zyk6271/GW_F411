/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-17     Rick       the first version
 */
#include "wifi.h"
#include "wifi-api.h"
#include "flashwork.h"
#include "stdio.h"
#include "wifi-service.h"
#include "radio_encoder.h"

#define DBG_TAG "WIFI-API"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

char *door_pid = {"emnzq3qxwfplx7db"};
char *slave_pid = {"lnbkva6cip8dw7vy"};
char *main_pid = {"q3xnn9yqt55ifaxm"};

extern Device_Info Global_Device;

void WariningUpload(uint32_t device_id,uint8_t type,uint8_t value)
{
    unsigned char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    if(device_id>0)
    {
        switch(type)
        {
            case 0://掉线
               mcu_dp_bool_update(103,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
            case 1://漏水
               mcu_dp_enum_update(1,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
            case 2://电量
                mcu_dp_enum_update(102,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
        }
    }
    else
    {
        switch(type)
        {
            case 0://自检
                mcu_dp_bool_update(DPID_DEVICE_CHECK,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
            case 1://漏水
                mcu_dp_bool_update(DPID_DEVICE_ALARM,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
            case 2://掉落
                mcu_dp_bool_update(DPID_LINE_STATE,value,Buf,my_strlen(Buf)); //BOOL型数据上报;
               break;
        }
    }
   rt_free(Buf);
}
void Slave_Heart(uint32_t device_id,uint8_t rssi)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    LOG_I("Slave_Heart Device ID is %ld,rssi is %d\r\n",device_id,rssi);
    mcu_dp_enum_update(101,rssi,Buf,my_strlen(Buf)); //VALUE型数据上报;
}
void MotoUpload(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("MotoUpload State is %d\r\n",state);
    sprintf(Buf,"%d",0);
    mcu_dp_bool_update(DPID_DEVICE_STATE,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void RemoteUpload(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("RemoteUpload %ld is upload %d\r\n",device_id,state);
    sprintf(Buf,"%d",device_id);
    mcu_dp_bool_update(DPID_CONTROL_STATE,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void Device_Add(uint32_t device_id)
{
    if(device_id>=10000000 && device_id<20000000)
    {
        Main_Add_WiFi(device_id);
    }
    else if(device_id>=20000000 && device_id<30000000)
    {
        Slave_Add_WiFi(device_id);
    }
    else if(device_id>=30000000 && device_id<40000000)
    {
        Door_Add_WiFi(device_id);
    }
}
void Main_Add_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x3C);
    gateway_subdevice_add("1.0",main_pid,0,Buf,10,0);
    LOG_I("Main_Add_WiFi ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Slave_Add_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x3C);
    gateway_subdevice_add("1.0",slave_pid,0,Buf,10,0);
    LOG_I("Slave_Add_by WiFi ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Door_Add_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x3C);
    gateway_subdevice_add("1.0",door_pid,0,Buf,10,0);
    LOG_I("Door_Add_by WiFi ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Device_Delete_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    LOG_I("Device_Del_by WiFi ID is %d\r\n",device_id);
    local_subdev_del_cmd(Buf);
    rt_free(Buf);
}
void Delay_WiFi(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("Delay_WiFi %d from %ld is upload\r\n",state,device_id);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DELAY_STATE,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void Delay_Close_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    LOG_I("Delay_Close_WiFi is upload\r\n");
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DELAY_STATE,1,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void Warning_WiFi(uint32_t device_id,uint8_t state)
{
    LOG_I("Warning_WiFi value %d is upload\r\n",state);
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_NORMAL,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
}
void Moto_CloseRemote(uint32_t device_id)
{
    LOG_I("Main %d Moto is Remote close\r\n",device_id);
    RadioEnqueue(0,device_id,1,9,0);
}
MSH_CMD_EXPORT(Moto_CloseRemote,Moto_CloseRemote);
void Moto_OpenRemote(uint32_t device_id)
{
    LOG_I("Main %d Moto is Remote Open\r\n",device_id);
    RadioEnqueue(0,device_id,1,9,1);
}
MSH_CMD_EXPORT(Moto_OpenRemote,Moto_OpenRemote);
void Delay_CloseRemote(uint32_t device_id)
{
    LOG_I("Main %d Delay is Remote close\r\n",device_id);
    RadioEnqueue(0,device_id,1,8,1);
}
MSH_CMD_EXPORT(Delay_CloseRemote,Delay_CloseRemote);
void Delay_OpenRemote(uint32_t device_id)
{
    LOG_I("Main %d Delay is Remote Open\r\n",device_id);
    RadioEnqueue(0,device_id,1,8,0);
}
MSH_CMD_EXPORT(Delay_OpenRemote,Delay_OpenRemote);
void Heart_Report(uint32_t device_id)
{
    char *id = rt_malloc(20);
    sprintf(id,"%ld",device_id);
    heart_beat_report(id,0);
    LOG_I("Heart_Report %d is upload\r\n",device_id);
}
void Ack_Report(uint32_t device_id)
{
    RadioEnqueue(0,device_id,1,12,0);
    LOG_I("Ack_Report %d is upload\r\n",device_id);
}
void Heart_Request(char *device_id)
{
    uint32_t id = 0;
    id = atol(device_id);
    if(id>=20000000)//非主控直接上报
    {
        heart_beat_report(device_id,0);
    }
    else
    {
        heart_beat_report(device_id,0);
        //RadioEnqueue(0,id,1,10,0);
        //LOG_I("Heart_Request %s is download\r\n",device_id);
    }
}
rt_timer_t Sync_Request_t = RT_NULL;
uint8_t Sync_Counter = 1;
void Sync_Request_Callback(void *parameter)
{
    if(Sync_Counter<=Global_Device.Num)
    {
        RadioEnqueue(0,Global_Device.ID[Sync_Counter],1,11,0);
        LOG_I("Sync_Request %s is download\r\n");
        Sync_Counter++;
    }
    else
    {
        rt_timer_stop(Sync_Request_t);
    }
}
void Sync_Request(void)
{
    Sync_Counter = 1;
    if(Sync_Request_t==RT_NULL)
    {
        Sync_Request_t = rt_timer_create("Sync_Request", Sync_Request_Callback, RT_NULL, 3000, RT_TIMER_FLAG_PERIODIC||RT_TIMER_FLAG_SOFT_TIMER);
    }
    rt_timer_start(Sync_Request_t);
}
void Heart_Request_test(void)
{
    extern uint32_t Main_ID;
    char *id = rt_malloc(20);
    sprintf(id,"%ld",Main_ID);
    Heart_Request(id);
}
MSH_CMD_EXPORT(Heart_Request_test,Heart_Request_test);
