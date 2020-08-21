
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "Sensors.hpp"
#include "PWM.hpp"

#include "Measure.hpp"


CPWM PWM;

void vTask_Main(void *pvParameters)
{
CMeasure Measure;
Measure.Main();
}



void CMeasure::Main (void)
{

while (1)
{
switch (measure_type)
	{
	case f: F(); break;
	case time: Time(); break;
	case angle: Angle(); break;
	case vertical: Vertical(); break;
	case no_measure: break;
	default: break;
	}
vTaskDelete( NULL );
}
}



void CMeasure::F(void)
{

}



void CMeasure::Time(void)
{

}



void CMeasure::Angle(void)
{
//¬ начале измерени€ полоз должен находитьс€ в нижнем положении
if (Sensors.sensors.encoder_height >400);		//≈сли он подн€т, то шлем команду "опустите токоприемник"
//else		//иначе шлем команду "поднимите"
/*

...

*/
//начинаем подъем
for (char i=0; i<=100; i++)	//–азгон€емс€ от 0 до 100 за 1 сек)))
	{
	PWM.SetMotorSpeed(i);
	vTaskDelay(10);
	}
while (Sensors.sensors.laser_height<400);	//∆дем пока не поднимитс€ до нужного значени€
if (Sensors.sensors.laser_height>3000) data.vertical=false;		//¬ышли за пределы допуска вертикальности
else data.vertical=true;
}



//ѕроверка вертикальности подъема полоза с помощью лазера (делаетс€ только на балконе)
void CMeasure::Vertical(void)
{
//¬ начале измерени€ полоз должен находитьс€ в нижнем положении
if (Sensors.sensors.laser_height>400);		//≈сли он подн€т, то шлем команду "опустите токоприемник"
//else		//иначе шлем команду "поднимите"
while (Sensors.sensors.laser_height<1900);	//∆дем пока не поднимитс€ до нужного значени€
if (Sensors.sensors.laser_height>3000) data.vertical=false;		//¬ышли за пределы допуска вертикальности
else data.vertical=true;
}
