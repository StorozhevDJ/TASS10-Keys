
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "ZigBee\ZigBee_API.hpp"

#include "PWM.hpp"
#include "Power.hpp"
#include "InOut.hpp"

#include "Sensors.hpp"
#include "Communication.hpp"
#include "Measure.hpp"




CPower Power;









// Дескриптор периодического таймера
xTimerHandle	xAutoReloadTimer;

/* Функция периодического таймера.
*	Является функцией обратного вызова.
*	В программе не должно быть ее явных вызовов.
*	В функцию автоматически передается дескриптор таймера в виде аргумента xTimer. */
void vAutoReloadTimerFunction(xTimerHandle xTimer)
//void AutoReloadTimer(void)
{
ZigBeeAPI.Timer_1ms();


/*if (delay_time) delay_time--;
if (time_battery_check) time_battery_check--;

if (++time_1ms>=1000)
	{
	time_1ms=0;
	unixtime++;
	if (programm_timeout) programm_timeout--;
	if (sleep_timeout) sleep_timeout--;
	if (timeout_LED_shootdown) {timeout_LED_shootdown--; LED_ON;} else LED_OFF;
	}
*/
}






unsigned char pwr_key_pressed;
/*
* Функция периодического таймера.
*/
xTimerHandle	xAutoReloadTimer_1s;
void vAutoReloadTimer_1s_Function(xTimerHandle xTimer)
{
//Обрабатываем нажатие кнопки "Вкл/Откл"
if (Power.KeyOnOff()) pwr_key_pressed++;
else pwr_key_pressed=0;
if (pwr_key_pressed>=3) Power.Off();	//Если нажатие удерживается 3 и более сек, то выключим себе питание
//Проверяем уровень заряда АКБ и в случае если он низкий включим светодиод
if (InOut.GetUAKB()<1150) InOut.LedBattLL(true);
else InOut.LedBattLL(false);
//Гасим светодиод связи
InOut.LedLink(false);
}











void main()
{

Power.KeyOnOff();


// Создать периодический таймер.
xAutoReloadTimer = xTimerCreate("AutoReloadTimer", 1/portTICK_RATE_MS, pdTRUE, 0, vAutoReloadTimerFunction);
xAutoReloadTimer_1s = xTimerCreate("AutoReloadTimer_1s", 1000/portTICK_RATE_MS, pdTRUE, 0, vAutoReloadTimer_1s_Function);
/* Вьполнить сброс периодического таймера ДО запуска планировщика.
*  Таким образом, он начнет отсчет времени одновременно с запуском планировщика */
xTimerReset(xAutoReloadTimer, 0);
xTimerReset(xAutoReloadTimer_1s, 0);

//xTaskCreate( vTask_Main,  "Measure", 512, NULL, 2, ( xTaskHandle * ) NULL);
xTaskCreate( vTask_Sensor,  "Sensor", 512, NULL, 2, ( xTaskHandle * ) NULL);
xTaskCreate( vTask_Communicate, "ZigBee", 2042+configMINIMAL_STACK_SIZE, NULL, 2, ( xTaskHandle * ) NULL);
vTaskStartScheduler();

while(1)
{

}
}

