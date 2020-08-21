

#include "InOut.hpp"


CInOut InOut;


CInOut::CInOut()
{
GPIOInit();
EncoderInit();
ADCInit();
}



//------------------------------------------------------------------------------
void CInOut::GPIOInit(void)
{
//Разрешаем тактирование шины портов
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

GPIO_InitTypeDef gpio_init;

//Входы/выходы IO
//Key UP | Key Down
gpio_init.GPIO_Pin  = PIN_IO1 | PIN_IO2;
gpio_init.GPIO_Mode = GPIO_Mode_IN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_IO1_2, &gpio_init);
//Encoder A | B
gpio_init.GPIO_Pin  = PIN_IO3 | PIN_IO4;
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_25MHz;
GPIO_Init(PORT_IO3_4, &gpio_init);
GPIO_PinAFConfig(PORT_IO3_4, GPIO_PinSource6, GPIO_AF_TIM8);
GPIO_PinAFConfig(PORT_IO3_4, GPIO_PinSource7, GPIO_AF_TIM8);
//FED
gpio_init.GPIO_Pin  = PIN_IO5 | PIN_IO6;
gpio_init.GPIO_Mode = GPIO_Mode_IN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_IO5_6, &gpio_init);
//Gerkon
gpio_init.GPIO_Pin  = PIN_IO7;
gpio_init.GPIO_Mode = GPIO_Mode_IN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_IO7, &gpio_init);
//N/C
gpio_init.GPIO_Pin  = PIN_IO8;
gpio_init.GPIO_Mode = GPIO_Mode_AN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_IO8, &gpio_init);

//Входы AIN
//Laser O1D100
gpio_init.GPIO_Pin  = PIN_AIN1;
gpio_init.GPIO_Mode = GPIO_Mode_AN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_AIN, &gpio_init);

//Выходы OUT
//LED "Связь" | "Низкий заряд"
gpio_init.GPIO_Pin  = PIN_OUT1 | PIN_OUT2;
gpio_init.GPIO_Mode = GPIO_Mode_AN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(PORT_OUT, &gpio_init);
}



//------------------------------------------------------------------------------
void CInOut::EncoderInit(void)
{
/* Enable GPIO clock */
//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

// PC.6,PC.7 for encoder
/*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_Init(GPIOC,&GPIO_InitStructure);

GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);*/

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
}



//------------------------------------------------------------------------------
bool CInOut::KeyUp(void)
{
return GPIO_ReadInputDataBit(PORT_IO1_2, PIN_IO1)?false:true;
}



//------------------------------------------------------------------------------
bool CInOut::KeyDown(void)
{
return GPIO_ReadInputDataBit(PORT_IO1_2, PIN_IO2)?false:true;
}



//------------------------------------------------------------------------------
bool CInOut::Gerkon(void)
{
return GPIO_ReadInputDataBit(PORT_IO7, PIN_IO7)?false:true;
}



//------------------------------------------------------------------------------
void CInOut::LedLink(bool state)
{
if (state) GPIO_SetBits(PORT_OUT, PIN_OUT1);
else GPIO_ResetBits(PORT_OUT, PIN_OUT1);
}



//------------------------------------------------------------------------------
void CInOut::LedBattLL(bool state)
{
if (state) GPIO_SetBits(PORT_OUT, PIN_OUT2);
else GPIO_ResetBits(PORT_OUT, PIN_OUT2);
}



//------------------------------------------------------------------------------
unsigned char CInOut::FED(void)
{
if (GPIO_ReadInputDataBit(PORT_IO5_6, PIN_IO5)) return 1;
if (GPIO_ReadInputDataBit(PORT_IO5_6, PIN_IO6)) return 2;
return 0;
}



//------------------------------------------------------------------------------
uint32_t CInOut::GetEncoder(void)
{
return TIM_GetCounter(TIM8);
}



//------------------------------------------------------------------------------
void CInOut::SetEncoder(uint32_t count)
{
TIM_SetCounter(TIM8, count);
}



//------------------------------------------------------------------------------
void CInOut::ADCInit(void)
{
ADC_InitTypeDef ADC_InitStructure;
ADC_StructInit(&ADC_InitStructure);

ADC_CommonInitTypeDef adc_init;
ADC_CommonStructInit(&adc_init);
// разрешаем тактирование AЦП1
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
// сбрасываем настройки АЦП
ADC_DeInit();

// АЦП1 и АЦП2 работают независимо
adc_init.ADC_Mode = ADC_Mode_Independent;
adc_init.ADC_Prescaler = ADC_Prescaler_Div2;
// выключаем scan conversion
ADC_InitStructure.ADC_ScanConvMode = DISABLE;
// Не делать длительные преобразования
ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

// Начинать преобразование програмно, а не по срабатыванию триггера
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
// 12 битное преобразование. результат в 12 младших разрядах результата
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

// инициализация
ADC_CommonInit(&adc_init);

ADC_Init(ADC1, &ADC_InitStructure);
// Включаем АЦП
ADC_Cmd(ADC1, ENABLE);
}



//------------------------------------------------------------------------------
u16 CInOut::ReadADC1(u8 channel)
{
ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_56Cycles);
// начинаем работу
ADC_SoftwareStartConv(ADC1);
// ждём пока преобразуется напряжение в код
while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
// возвращаем результат
return ADC_GetConversionValue(ADC1);
}
