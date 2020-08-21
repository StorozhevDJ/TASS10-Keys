/*
Обработка данных с сенсоров
*/

#ifndef SENSORS_h
#define SENSORS_h


#define TENZO_CAL	(0x100/10000)
#define PRESURE_CAL	(0x100/10000)
#define ENCODER_CAL	(1)
#define LASER_CAL	(1)
#define UAKB_CAL	(1)

#define ENCODER_MIN	(1)



//Значения измеряемых парамеров
typedef struct
	{
	//Значение усилия на трос
	  signed short pull;
	//Значение давления воздуха
	  signed short presure;
	//Показания дальномера
	unsigned short laser_height;
	//Высота по энкодеру
	unsigned short encoder_height;
	//Напряжение аккумулятора
	unsigned short u_akb;
	//Разрешение опроса измерительных каналов
	bool pull_en;
	bool presure_en;
	bool laser_en;
	} Tsensors;



void vTask_Sensor(void *pvParameters);

#include "InOut.hpp"
class CSensors: public virtual CInOut
	{
	public:
	Tsensors sensors;
	signed long tenzo_null;
	signed long presure_null;
	signed long laser_null;
	
	
	//void Sensor(void (*vTask_Sensor)(void *pvParameters));
	void Sensor(void);
	
	void SetTenzoNull(void);
	void SetPresureNull(void);
	void SetLaserNull(void);
	inline void SetEncoderNull(void){InOut.SetEncoder(0);};
	signed long GetPullValue(void);
	signed long GetPresureValue(void);
	};

extern CSensors Sensors;

#endif