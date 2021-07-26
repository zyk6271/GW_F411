#include <agile_led.h>
#include <stdlib.h>
#include "led.h"
#include "pin_config.h"
#include <agile_led.h>

static agile_led_t *led1 = RT_NULL;
static agile_led_t *led2 = RT_NULL;
static agile_led_t *led3 = RT_NULL;
static agile_led_t *led4 = RT_NULL;
static agile_led_t *beep = RT_NULL;

#define DBG_TAG "LED"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void Led_Init(void)
{
    if(led1 == RT_NULL)
    {
        led1 = agile_led_create(LED1_PIN, PIN_HIGH, "200,200", -1);
        LOG_D("LED_1 Init Success\r\n");
    }
    if(led2 == RT_NULL)
    {
        led2 = agile_led_create(LED2_PIN, PIN_HIGH, "200,200", -1);
        LOG_D("LED_2 Init Success\r\n");
    }
    if(led3 == RT_NULL)
    {
        led3 = agile_led_create(LED3_PIN, PIN_HIGH, "200,200", -1);
        LOG_D("LED_3 Init Success\r\n");
    }
    if(led4 == RT_NULL)
    {
        led4 = agile_led_create(LED4_PIN, PIN_HIGH, "200,200", -1);
        LOG_D("LED_4 Init Success\r\n");
    }
    if(beep == RT_NULL)
    {
        beep = agile_led_create(BUZZER_PIN, PIN_HIGH, "200,200", -1);
        LOG_D("beep Init Success\r\n");
    }
}
void beep_start(uint8_t led_id,int mode)
{
    switch (mode)
    {
    case 0://短叫一声
        if(led_id)//绿灯
        {
            agile_led_set_light_mode(beep, "200,200", 1);
            agile_led_start(beep);
            agile_led_set_light_mode(led1, "200,200", 1);
            agile_led_start(led1);
        }
        else//红灯
        {
            agile_led_set_light_mode(beep, "200,15000", -1);
            agile_led_start(beep);
            agile_led_set_light_mode(led3, "200,15000", -1);
            agile_led_start(led3);
        }
        break;
    }
}
