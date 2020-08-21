

#include "Power.hpp"


CPower::CPower()
{
GPIOInit();
On();
}



CPower::~CPower()
{
Off();
}



void CPower::GPIOInit(void)
{
//Разрешаем тактирование шины портов
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

GPIO_InitTypeDef gpio_init;

//Key On/Off
gpio_init.GPIO_Pin  = PIN_KEY_ON_OFF;
gpio_init.GPIO_Mode = GPIO_Mode_IN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_KEY_ON_OFF, &gpio_init);
//Power_On
gpio_init.GPIO_Pin  = PIN_POWER_ON;
gpio_init.GPIO_Mode = GPIO_Mode_OUT;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_POWER_ON, &gpio_init);
//12V Shutdown
gpio_init.GPIO_Pin  = PIN_12V_SHDN;
gpio_init.GPIO_Mode = GPIO_Mode_OUT;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_12V_SHDN, &gpio_init);
//Voltage Battery
gpio_init.GPIO_Pin  = PIN_UBAT;
gpio_init.GPIO_Mode = GPIO_Mode_AN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_UBAT, &gpio_init);
}



inline void CPower::On(void)
{
GPIO_ResetBits(PORT_POWER_ON, PIN_POWER_ON);
}



inline void CPower::Off(void)
{
GPIO_SetBits(PORT_POWER_ON, PIN_POWER_ON);
}



bool CPower::KeyOnOff(void)
{
return GPIO_ReadInputDataBit(PORT_KEY_ON_OFF, PIN_KEY_ON_OFF)?false:true;
}
