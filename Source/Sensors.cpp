
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "Filter\Filter.h"
#include "AD779x\drv_ad7794_98_99.h"

#include "InOut.hpp"

#include "Sensors.hpp"



Tfilter tenzo_filter;
Tfilter presure_filter;



CSensors Sensors;



void vTask_Sensor(void *pvParameters)
{

Sensors.Sensor();
}



void CSensors::Sensor(void)
{
Init_AD7794_95_98_99();

//��������� �������������� ���
Write_Reg_AD7794_95_98_99(Config_Reg, Chan_1p_1m|Gain_1|Bipolar);

tenzo_filter.delta=500;
presure_filter.delta=1000;

SetTenzoNull();
SetPresureNull();
SetEncoderNull();

this->sensors.laser_en=true;		//��������� ��������� ������ �� ������
this->sensors.presure_en=true;	//������ �������� �������
this->sensors.pull_en=true;		//������ ��������� �����

sensors.pull=GetPullValue();					//�������� �������� ������ ��������� �����
sensors.presure=GetPresureValue();				//�������� �������� �������
sensors.laser_height=(GetLaser()/LASER_CAL)-laser_null;	//������ �� ������
sensors.encoder_height=GetEncoder()/ENCODER_CAL;	//������ �� ��������
sensors.u_akb=GetUAKB()/UAKB_CAL;				//���������� �� ������������

while(1)
{
if (this->sensors.laser_en) this->sensors.laser_height=GetLaser()-laser_null;
if (this->sensors.presure_en) this->sensors.presure=GetPresureValue();
if (this->sensors.pull_en) this->sensors.pull=GetPullValue();

this->sensors.u_akb=GetUAKB()/UAKB_CAL;
this->sensors.encoder_height=GetEncoder()/ENCODER_CAL;

//���� �������� ������ � �������, ��� ������������� ������� 400��,
//��������� ������ ��������� ��������
if (0) SetEncoder(400*ENCODER_CAL);
//���� ������ � �������� ������ �����������, ������ �� �� �����������
if (this->sensors.encoder_height<ENCODER_MIN) SetEncoder(ENCODER_MIN*ENCODER_CAL);
//���� �������� FED ��������
if (FED()==1) SetEncoder(ENCODER_MIN*ENCODER_CAL);
}
}



void CSensors::SetTenzoNull(void)
{
unsigned long tmp;
tenzo_null=0;
for (char i=0; i<FILTER_BUF_SIZE; i++)
	{
	Get_Continuous_Conversion_ADC(Chan_1p_1m, Gain_1, &tmp, 1);
	tmp=tmp*TENZO_CAL;
	Filter (tmp, &tenzo_filter);
	}
tenzo_null=tmp;
}



void CSensors::SetPresureNull(void)
{
unsigned long tmp;
tenzo_null=0;
for (char i=0; i<FILTER_BUF_SIZE; i++)
	{
	Get_Continuous_Conversion_ADC(Chan_2p_2m, Gain_1, &tmp, 1);
	tmp=tmp*PRESURE_CAL;
	Filter (tmp, &presure_filter);
	}
presure_null=tmp;
}



signed long CSensors::GetPullValue(void)
{
unsigned long tmp;
for (char i=0; i<16; i++)
	{
	Get_Continuous_Conversion_ADC(Chan_1p_1m, Gain_1, &tmp, 1);
	tmp=(tmp*TENZO_CAL)-tenzo_null;
	Filter (tmp, &tenzo_filter);
	}
return tmp;
}



signed long CSensors::GetPresureValue(void)
{
unsigned long tmp;
for (char i=0; i<16; i++)
	{
	Get_Continuous_Conversion_ADC(Chan_2p_2m, Gain_1, &tmp, 1);
	tmp=(tmp*PRESURE_CAL)-tenzo_null;
	Filter (tmp, &tenzo_filter);
	}
return tmp;
}
