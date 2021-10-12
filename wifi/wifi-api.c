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

Remote_Info Remote_Device={0};
extern Device_Info Global_Device;

uint8_t Sync_Counter = 1;
rt_timer_t Sync_Request_t = RT_NULL;

void WariningUpload(uint32_t from_id,uint32_t device_id,uint8_t type,uint8_t value)
{
    unsigned char *device_id_buf = rt_malloc(20);
    unsigned char *from_id_buf = rt_malloc(20);
    sprintf(device_id_buf,"%ld",device_id);
    sprintf(from_id_buf,"%ld",from_id);
    if(device_id>0)
    {
        switch(type)
        {
            case 0://掉线
               mcu_dp_bool_update(103,value,device_id_buf,my_strlen(device_id_buf)); //BOOL型数据上报;
               break;
            case 1://漏水
               mcu_dp_enum_update(1,value,device_id_buf,my_strlen(device_id_buf)); //BOOL型数据上报;
               break;
            case 2://电量
                mcu_dp_enum_update(102,value,device_id_buf,my_strlen(device_id_buf)); //BOOL型数据上报;
               break;
            case 3://掉落
                mcu_dp_bool_update(104,value,device_id_buf,my_strlen(device_id_buf)); //VALUE型数据上报;
               break;
        }
    }
    else
    {
        switch(type)
        {
            case 0://自检
                if(value == 0)
                {
                    mcu_dp_bool_update(DPID_DEVICE_CHECK,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
                else if(value == 1)
                {
                    mcu_dp_bool_update(113,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
                else if(value == 2)
                {
                    mcu_dp_bool_update(DPID_DEVICE_CHECK,1,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_TEMP,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_LINE_STATE,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
                else if(value == 3)
                {
                    mcu_dp_bool_update(113,1,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_TEMP,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_LINE_STATE,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
               break;
            case 1://漏水
                mcu_dp_bool_update(DPID_DEVICE_ALARM,value,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                if(value)
                {
                    mcu_dp_bool_update(DPID_DEVICE_CHECK,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(113,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_TEMP,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                    mcu_dp_bool_update(DPID_LINE_STATE,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
               break;
            case 2://掉落
                mcu_dp_bool_update(DPID_LINE_STATE,value,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
               break;
            case 3://NTC
                mcu_dp_bool_update(DPID_TEMP,value,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                if(value)
                {
                    mcu_dp_bool_update(DPID_LINE_STATE,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
                }
               break;
        }
    }
    rt_free(device_id_buf);
    rt_free(from_id_buf);
}
void CloseWarn_Main(uint32_t device_id)
{
    unsigned char *from_id_buf = rt_malloc(20);
    sprintf(from_id_buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DEVICE_ALARM,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    mcu_dp_bool_update(106,0,from_id_buf,my_strlen(from_id_buf)); //VALUE型数据上报;
    LOG_I("CloseWarn_Main ID is %ld\r\n",device_id);
    rt_free(from_id_buf);
}
void InitWarn_Main(uint32_t device_id)
{
    unsigned char *from_id_buf = rt_malloc(20);
    sprintf(from_id_buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DEVICE_ALARM,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_DEVICE_CHECK,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    mcu_dp_bool_update(113,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_TEMP,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_LINE_STATE,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    LOG_I("CloseWarn_Main ID is %ld\r\n",device_id);
    rt_free(from_id_buf);
}
void CloseWarn_Slave(uint32_t device_id)
{
    unsigned char *from_id_buf = rt_malloc(20);
    sprintf(from_id_buf,"%ld",device_id);
    mcu_dp_enum_update(1,0,from_id_buf,my_strlen(from_id_buf)); //BOOL型数据上报;
    rt_free(from_id_buf);
}
void Remote_Delete(uint32_t device_id)
{
    unsigned char *id_buf = rt_malloc(20);
    sprintf(id_buf,"%ld",GetBindID(device_id));
    GatewayDataEnqueue(GetBindID(device_id),device_id,0,6,0);
    LOG_I("Remote_Delete Success %s\r\n",id_buf);
    Del_Device(device_id);
    if(device_id>=30000000)
    {
        mcu_dp_value_update(108,0,id_buf,my_strlen(id_buf)); //BOOL型数据上报;
    }
    rt_free(id_buf);
}
void Slave_Heart(uint32_t device_id,uint8_t rssi)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    LOG_I("Slave_Heart Device ID is %ld,rssi is %d\r\n",device_id,rssi);
    mcu_dp_enum_update(101,rssi,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void MotoUpload(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("MotoUpload State is %d,device_id is %ld\r\n",state,device_id);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DEVICE_STATE,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void RemoteUpload(uint32_t from_id,uint32_t device_id,uint8_t state)
{
    char *FromBuf = rt_malloc(20);
    char *DeviceBuf = rt_malloc(20);
    LOG_I("RemoteUpload %ld from %ld is upload %d\r\n",device_id,from_id,state);
    sprintf(FromBuf,"%ld",from_id);
    sprintf(DeviceBuf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DEVICE_STATE,state,FromBuf,my_strlen(FromBuf)); //VALUE型数据上报;
    rt_free(FromBuf);
    rt_free(DeviceBuf);
}
void DoorUpload(uint32_t from_id,uint32_t device_id,uint8_t state)
{
    char *FromBuf = rt_malloc(20);
    char *DeviceBuf = rt_malloc(20);
    LOG_I("DoorUpload %ld from %ld is upload %d\r\n",device_id,from_id,state);
    sprintf(FromBuf,"%ld",from_id);
    sprintf(DeviceBuf,"%ld",device_id);
    mcu_dp_bool_update(DPID_DEVICE_STATE,state,FromBuf,my_strlen(FromBuf)); //VALUE型数据上报;
    mcu_dp_bool_update(104,state,DeviceBuf,my_strlen(DeviceBuf)); //VALUE型数据上报;
    rt_free(FromBuf);
    rt_free(DeviceBuf);
}
void Device_Add2Flash(uint32_t device_id,uint32_t from_id)
{
    if(Flash_Get_Key_Valid(device_id) == RT_ERROR)
    {
        if(device_id>=10000000 && device_id<20000000)
        {
            MainAdd_Flash(device_id);
        }
        if(device_id>=20000000 && device_id<30000000)
        {
            SlaveAdd_Flash(device_id,from_id);
        }
        else if(device_id>=30000000 && device_id<40000000)
        {
            DoorAdd_Flash(device_id,from_id);
        }
    }
}
void Device_Add2Wifi(uint32_t device_id,uint32_t from_id)
{
    if(Flash_Get_Key_Valid(device_id) == RT_EOK)
    {
        if(device_id>=10000000 && device_id<20000000)
        {
            Main_Add_WiFi(device_id);
        }
        if(device_id>=20000000 && device_id<30000000)
        {
            Slave_Add_WiFi(device_id,from_id);
        }
        else if(device_id>=30000000 && device_id<40000000)
        {
            Door_Add_WiFi(device_id,from_id);
        }
    }
}
void Device_Add2Flash_Wifi(uint32_t device_id,uint32_t from_id)
{
    if(Flash_Get_Key_Valid(device_id) == RT_ERROR)
    {
        if(device_id>=10000000 && device_id<20000000)
        {
            MainAdd_Flash(device_id);
            Main_Add_WiFi(device_id);
            Reset_Main_Value(device_id);
        }
        if(device_id>=20000000 && device_id<30000000)
        {
            SlaveAdd_Flash(device_id,from_id);
            Slave_Add_WiFi(device_id,from_id);
            Reset_Slave_Value(device_id,from_id);
        }
        else if(device_id>=30000000 && device_id<40000000)
        {
            DoorAdd_Flash(device_id,from_id);
            Door_Add_WiFi(device_id,from_id);
            Reset_Door_Value(device_id,from_id);
        }
    }
    else
    {
        if(device_id>=10000000 && device_id<20000000)
        {
            Main_Add_WiFi(device_id);
        }
        if(device_id>=20000000 && device_id<30000000)
        {
            Slave_Add_WiFi(device_id,from_id);
        }
        else if(device_id>=30000000 && device_id<40000000)
        {
            Door_Add_WiFi(device_id,from_id);
        }
    }
}
void DeviceCheck(uint32_t device_id,uint32_t from_id)
{
    Device_Add2Flash_Wifi(device_id,from_id);
    Flash_Set_Heart(device_id,1);
}
void Local_Delete(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    char *Mainbuf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    sprintf(Mainbuf,"%ld",GetBindID(device_id));
    local_subdev_del_cmd(Buf);
    if(device_id>=30000000)
    {
        mcu_dp_value_update(108,0,Mainbuf,my_strlen(Mainbuf)); //BOOL型数据上报;
    }
    rt_free(Buf);
    rt_free(Mainbuf);
}
void Main_Add_WiFi(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x01);
    gateway_subdevice_add("1.0",main_pid,0,Buf,10,0);
    mcu_dp_value_update(112,device_id,Buf,my_strlen(Buf)); //BOOL型数据上报;
    LOG_I("Main_Add_WiFi ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Reset_Main_Value(uint32_t device_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_value_update(112,device_id,Buf,my_strlen(Buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_DEVICE_ALARM,0,Buf,my_strlen(Buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_LINE_STATE,0,Buf,my_strlen(Buf)); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_TEMP,0,Buf,my_strlen(Buf)); //BOOL型数据上报;
    LOG_I("Reset_Main_Value ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Slave_Add_WiFi(uint32_t device_id,uint32_t from_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x01);
    gateway_subdevice_add("1.0",slave_pid,0,Buf,10,0);
    mcu_dp_value_update(107,from_id,Buf,my_strlen(Buf)); //BOOL型数据上报;
    LOG_I("Slave_Add_by WiFi ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Reset_Slave_Value(uint32_t device_id,uint32_t from_id)
{
    char *Buf = rt_malloc(20);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_value_update(107,from_id,Buf,my_strlen(Buf)); //BOOL型数据上报;
    mcu_dp_enum_update(1,0,Buf,my_strlen(Buf)); //BOOL型数据上报;
    mcu_dp_bool_update(104,0,Buf,my_strlen(Buf)); //VALUE型数据上报;
    LOG_I("Reset_Slave_Value ID is %d\r\n",device_id);
    rt_free(Buf);
}
void Door_Add_WiFi(uint32_t device_id,uint32_t from_id)
{
    char *Mainbuf = rt_malloc(20);
    char *Doorbuf = rt_malloc(20);
    sprintf(Mainbuf,"%ld",from_id);
    sprintf(Doorbuf,"%ld",device_id);
    local_add_subdev_limit(1,0,0x01);
    gateway_subdevice_add("1.0",door_pid,0,Doorbuf,10,0);
    mcu_dp_value_update(107,from_id,Doorbuf,my_strlen(Doorbuf)); //BOOL型数据上报;
    mcu_dp_value_update(108,device_id,Mainbuf,my_strlen(Mainbuf)); //BOOL型数据上报;
    LOG_I("Door_Add_WiFi ID is %d\r\n",device_id);
    rt_free(Mainbuf);
    rt_free(Doorbuf);
}
void Reset_Door_Value(uint32_t device_id,uint32_t from_id)
{
    char *Mainbuf = rt_malloc(20);
    char *Doorbuf = rt_malloc(20);
    sprintf(Mainbuf,"%ld",from_id);
    sprintf(Doorbuf,"%ld",device_id);
    mcu_dp_value_update(107,from_id,Doorbuf,my_strlen(Doorbuf)); //BOOL型数据上报;
    mcu_dp_value_update(108,device_id,Mainbuf,my_strlen(Mainbuf)); //BOOL型数据上报;
    LOG_I("Reset_Door_Value ID is %d\r\n",device_id);
    rt_free(Mainbuf);
    rt_free(Doorbuf);
}
void Remote_Delay_WiFi(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("Remote_Delay_WiFi %d from %ld is upload\r\n",state,device_id);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(106,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void Door_Delay_WiFi(uint32_t device_id,uint8_t state)
{
    char *Buf = rt_malloc(20);
    LOG_I("Door_Delay_WiFi %d from %ld is upload\r\n",state,device_id);
    sprintf(Buf,"%ld",device_id);
    mcu_dp_bool_update(105,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
    rt_free(Buf);
}
void Warning_WiFi(uint32_t device_id,uint8_t state)
{
//    LOG_I("Warning_WiFi value %d is upload\r\n",state);
//    char *Buf = rt_malloc(20);
//    sprintf(Buf,"%ld",device_id);
//    mcu_dp_bool_update(DPID_NORMAL,state,Buf,my_strlen(Buf)); //VALUE型数据上报;
//    rt_free(Buf);
    InitWarn_Main(device_id);
}
void Moto_CloseRemote(uint32_t device_id)
{
    LOG_I("Main %d Moto is Remote close\r\n",device_id);
    GatewayDataEnqueue(device_id,0,0,2,0);
}
MSH_CMD_EXPORT(Moto_CloseRemote,Moto_CloseRemote);
void Moto_OpenRemote(uint32_t device_id)
{
    LOG_I("Main %d Moto is Remote Open\r\n",device_id);
    GatewayDataEnqueue(device_id,0,0,2,1);
}
MSH_CMD_EXPORT(Moto_OpenRemote,Moto_OpenRemote);
void Delay_CloseRemote(uint32_t device_id)
{
    LOG_I("Main %d Delay is Remote close\r\n",device_id);
    GatewayDataEnqueue(device_id,0,0,1,0);
}
MSH_CMD_EXPORT(Delay_CloseRemote,Delay_CloseRemote);
void Delay_OpenRemote(uint32_t device_id)
{
    LOG_I("Main %d Delay is Remote Open\r\n",device_id);
    GatewayDataEnqueue(device_id,0,0,1,1);
}
MSH_CMD_EXPORT(Delay_OpenRemote,Delay_OpenRemote);
void Heart_Report(uint32_t device_id,int rssi)
{
    char *id = rt_malloc(20);
    sprintf(id,"%ld",device_id);
    if(rssi>85)
    {
        mcu_dp_enum_update(105,0,id,my_strlen(id));
    }
    else if(rssi<=84 && rssi>54)
    {
        mcu_dp_enum_update(105,1,id,my_strlen(id));
    }
    else {
        mcu_dp_enum_update(105,2,id,my_strlen(id));
    }
    LOG_I("Heart_Report %d is upload\r\n",device_id);
    rt_free(id);
}
void Ack_Report(uint32_t device_id)
{
    GatewayDataEnqueue(device_id,0,0,5,1);
    LOG_I("Ack_Report %d is upload\r\n",device_id);
}
void Heart_Change(uint32_t device_id,uint8_t heart)
{
    char *id = rt_malloc(20);
    sprintf(id,"%ld",device_id);
    if(heart>0)
    {
        heart_beat_report(id,0);
    }
    rt_free(id);
}
void Heart_Request(char *device_id)
{
    uint32_t id = 0;
    id = atol(device_id);
    if(Flash_Get_Heart(id))//非主控直接上报
    {
        heart_beat_report(device_id,0);
    }
}
void Sync_Request_Callback(void *parameter)
{
    if(Sync_Counter<=Global_Device.Num)
    {
        if(Global_Device.ID[Sync_Counter]>0 && Global_Device.ID[Sync_Counter]<20000000 && Global_Device.Bind_ID[Sync_Counter]==0)
        {
            GatewayDataEnqueue(Global_Device.ID[Sync_Counter],0,0,4,0);
            LOG_I("Sync_Request %s is download\r\n");
        }
        Sync_Counter++;
    }
    else
    {
        rt_timer_stop(Sync_Request_t);
        Remote_Sync();
    }
}
void Sync_Request(void)
{
    Sync_Counter = 1;
    if(Sync_Request_t==RT_NULL)
    {
        Sync_Request_t = rt_timer_create("Sync_Request", Sync_Request_Callback, RT_NULL, 8000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
    }
    rt_timer_start(Sync_Request_t);
}
void Remote_Device_Add(uint32_t device_id)
{
    Remote_Device.ID[++Remote_Device.Num]=device_id;
}
void Remote_Sync(void)
{
    LOG_I("Remote Num is %d,Global Num is %d\r\n",Remote_Device.Num,Global_Device.Num);
    uint8_t Add_Flag = 1;
    if(Global_Device.Num==0)
    {
        return;
    }
    for(uint8_t i=1;i<=Global_Device.Num;i++)
    {
        Add_Flag = 1;
        for(uint8_t num=1;num<=Remote_Device.Num;num++)
        {
            if(Global_Device.ID[i]==Remote_Device.ID[num])
            {
                Add_Flag = 0;
                break;
            }
        }
        if(Add_Flag)
        {
            Device_Add2Wifi(Global_Device.ID[i],Global_Device.Bind_ID[i]);
        }
    }
}
