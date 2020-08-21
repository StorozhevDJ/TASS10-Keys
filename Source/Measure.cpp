
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
//� ������ ��������� ����� ������ ���������� � ������ ���������
if (Sensors.sensors.encoder_height >400);		//���� �� ������, �� ���� ������� "�������� ������������"
//else		//����� ���� ������� "���������"
/*

...

*/
//�������� ������
for (char i=0; i<=100; i++)	//����������� �� 0 �� 100 �� 1 ���)))
	{
	PWM.SetMotorSpeed(i);
	vTaskDelay(10);
	}
while (Sensors.sensors.laser_height<400);	//���� ���� �� ���������� �� ������� ��������
if (Sensors.sensors.laser_height>3000) data.vertical=false;		//����� �� ������� ������� ��������������
else data.vertical=true;
}



//�������� �������������� ������� ������ � ������� ������ (�������� ������ �� �������)
void CMeasure::Vertical(void)
{
//� ������ ��������� ����� ������ ���������� � ������ ���������
if (Sensors.sensors.laser_height>400);		//���� �� ������, �� ���� ������� "�������� ������������"
//else		//����� ���� ������� "���������"
while (Sensors.sensors.laser_height<1900);	//���� ���� �� ���������� �� ������� ��������
if (Sensors.sensors.laser_height>3000) data.vertical=false;		//����� �� ������� ������� ��������������
else data.vertical=true;
}
