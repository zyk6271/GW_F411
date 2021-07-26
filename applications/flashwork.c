/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <spi_flash.h>
#include <drv_spi.h>
#include <string.h>
#include <stdlib.h>
#include "pin_config.h"
#include "flashwork.h"
#include "fal.h"
#include "easyflash.h"

#define DBG_TAG "FLASH"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_spi_flash_device_t fm25q16;
uint8_t read_value_temp[64]={0};
Device_Info Global_Device={0};

int flash_Init(void)
{
    rt_err_t status;
    extern rt_spi_flash_device_t rt_sfud_flash_probe(const char *spi_flash_dev_name, const char *spi_dev_name);
    rt_hw_spi_device_attach("spi1", "spi10", GPIOA, GPIO_PIN_4);
    fm25q16 = rt_sfud_flash_probe("norflash0", "spi10");
    if (RT_NULL == fm25q16)
    {
        LOG_E("sfud fail\r\n");
        return -RT_ERROR;
    };
    status = fal_init();
    if (status == 0)
    {
        LOG_E("fal_init fail\r\n");
        return -RT_ERROR;
    };
    status = easyflash_init();
    if (status != EF_NO_ERR)
    {
        LOG_E("easyflash_init fail\r\n");
        return -RT_ERROR;
    };
    LOG_I("Storage Init Success\r\n");
    return RT_EOK;
}
uint32_t Flash_Get_Learn_Nums(void)
{
    uint8_t read_len = 0;
    uint32_t read_value = 0;
    char *keybuf="Learn_Nums";
    memset(read_value_temp,0,64);
    read_len = ef_get_env_blob(keybuf, read_value_temp, 64, NULL);
    if(read_len>0)
    {
        read_value = atol(read_value_temp);
    }
    else
    {
        read_value = 0;
    }
    LOG_D("Reading Key %s value %ld \r\n", keybuf, read_value);//输出
    return read_value;
}
void Flash_LearnNums_Change(uint32_t value)
{
    const char *keybuf="Learn_Nums";
    char *Temp_ValueBuf = rt_malloc(10);
    sprintf(Temp_ValueBuf, "%ld", value);
    ef_set_env(keybuf, Temp_ValueBuf);
    rt_free(Temp_ValueBuf);
    LOG_D("Writing %ld to key %s\r\n", value,keybuf);
}
void Flash_Key_Change(uint32_t key,uint32_t value)
{
    char *Temp_KeyBuf = rt_malloc(20);
    sprintf(Temp_KeyBuf, "%ld", key);
    char *Temp_ValueBuf = rt_malloc(20);//申请临时buffer空间
    sprintf(Temp_ValueBuf, "%ld", value);
    ef_set_env(Temp_KeyBuf, Temp_ValueBuf);
    rt_free(Temp_KeyBuf);
    rt_free(Temp_ValueBuf);
    LOG_D("Writing %ld to key %ld \r\n", value,key);
}
uint8_t Add_Main(uint32_t Device_ID)
{
    uint32_t Num=0;
    Num = Flash_Get_Learn_Nums();
    if(Num>10)return RT_ERROR;
    Num++;
    Flash_LearnNums_Change(Num);
    Global_Device.Num = Num;
    Global_Device.ID[Num] = Device_ID;
    Flash_Key_Change(Num,Device_ID);
    return RT_EOK;
}
void Delete_Main(void)
{
    Flash_LearnNums_Change(0);
}
uint8_t Flash_Get_Key_Valid(uint32_t Device_ID)//查询内存中的ID
{
    uint16_t num = Global_Device.Num;
    if(!num)return RT_ERROR;
    while(num)
    {
        if(Global_Device.ID[num]==Device_ID)return RT_EOK;
        num--;
    }
    return RT_ERROR;
}
uint32_t Flash_Get_Key_Value(uint32_t key)
{
    uint8_t read_len = 0;
    uint32_t read_value = 0;
    char *keybuf = rt_malloc(20);
    sprintf(keybuf, "%ld", key);//将传入的数字转换成数组
    memset(read_value_temp,0,64);
    read_len = ef_get_env_blob(keybuf, read_value_temp, 64, NULL);
    if(read_len>0)
    {
        read_value = atol(read_value_temp);
    }
    else
    {
        read_value = 0;
    }
    rt_free(keybuf);//释放临时buffer对应内存空间
    LOG_D("Reading Key %s value %ld \r\n", keybuf, read_value);//输出
    return read_value;
}
void LoadDevice2Memory(void)
{
    memset(&Global_Device,0,sizeof(Global_Device));
    Global_Device.Num = Flash_Get_Learn_Nums();
    LOG_D("Nums is %d",Global_Device.Num);
    for(uint8_t i=1;i<=Global_Device.Num;i++)
    {
        Global_Device.ID[i] = Flash_Get_Key_Value(i);
        LOG_D("GOT ID is %ld\r\n",Global_Device.ID[i]);
    }
}
