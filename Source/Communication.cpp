
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "InOut.hpp"
#include "Measure.hpp"

#include "ZigBee\ZigBee_API.hpp"

#include "Communication.hpp"


CCom Com;



unsigned char mas_adr[][8] = 
{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF }, // all
  { 0x00, 0x13, 0xA2, 0x00, 0x40, 0x96, 0x00, 0x82 }, // коробка 0 axel 0x00 0x13 0xA2 0x00 0x40 0x96 0x00 0x82 (акселерометр)
  { 0x00, 0x13, 0xA2, 0x00, 0x40, 0x96, 0x05, 0x6A }, // коробка 1      0x00 0x13 0xA2 0x00 0x40 0x96 0x05 0x6A (низ коробка штанга)
  { 0x00, 0x13, 0xA2, 0x00, 0x40, 0x96, 0x05, 0x02 }, // коробка 2      0x00 0x13 0xA2 0x00 0x40 0x96 0x05 0x02 (верх коробка штанга)
  { 0x00, 0x13, 0xA2, 0x00, 0x40, 0x96, 0x05, 0x1D }  // PDU
};



void vTask_Communicate(void *pvParameters)
{
ZigBeeAPI.CallBackRecievData(ReceiveZigBeeData);
ZigBeeAPI.CallBackRecievAll(ReceiveZigBeeOther);
vTaskDelay(1000);
ZigBeeAPI.GoBypassMode();
ZigBeeAPI.Escapes();			//Переход в режим команд
ZigBeeAPI.Send_AT_cmd_mode("SH");
vTaskDelay(1000);

//Перевод модуля в режим API
//ZigBeeAPI.Send_AT_cmd_mode("RE");	//Restore Defaults
//vTaskDelay(1000);
//ZigBeeAPI.Send_AT_cmd_mode("WR");
//vTaskDelay(1000);
ZigBeeAPI.Send_AT_cmd_mode("AP1");	//Перевод в режим API
vTaskDelay(1000);//Delay_ms(1000);
ZigBeeAPI.Send_AT_CMD("CE0");		//Похоже на управление типом устройства
vTaskDelay(1000);
ZigBeeAPI.Send_AT_CMD("NI", "KEYS1_TAcC");//Задаем текстовый идентификатор модуля
vTaskDelay(50);
ZigBeeAPI.Send_AT_CMD("ID", 1651);	//ID сети
vTaskDelay(50);//Delay_ms(100);
ZigBeeAPI.Send_AT_CMD("WR");		//Запись настроек
vTaskDelay(350);
/*unsigned char a[21];
for (char i=0; i<100; i++) ZigBeeAPI.ATcmd("NI", 100, &a[0]);//Проверяем как сохранились данные
ZigBeeAPI.ATcmd("ID", 100, &a[0]);*/


for (char i=0; i<5; i++)
	{
	if (ZigBeeAPI.Send_query_transmit_addr(&mas_adr[4][0], 5000, 1, 15)) break;
	vTaskDelay(200);
	}


while (1)
{
//Запускаем задачу измерения параметров токоприемника
xTaskCreate( vTask_Main,  "Measure", 512+configMINIMAL_STACK_SIZE, NULL, 2, ( xTaskHandle * ) NULL);
vTaskDelay(200);
}
}



void ReceiveZigBeeOther(unsigned char receivestatus, unsigned char * data, unsigned char len)
{
InOut.LedLink(true);
}

void ReceiveZigBeeData(unsigned char * data, unsigned char len)
{
InOut.LedLink(true);
}

