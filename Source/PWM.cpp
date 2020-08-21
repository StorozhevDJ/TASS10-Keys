

#include "PWM.hpp"


const uint32_t sin[240] = {  0,   3,   6,   9,  13,  16,  19,  22,  25,  28,  31,  34,  38,  41,  44,  47,  50,  53,  56,  59,  62,  65,  68,  71,  74,  77,  80,  83,  86 , 89,  92,  95,  98, 100, 103, 106, 109, 112, 115, 117, 120, 123, 125, 128, 131, 133, 136, 139, 141, 144, 146, 149, 151, 153, 156, 158, 161, 163, 165, 167, 170, 172, 174, 176, 178, 180, 182, 185, 187, 188, 190, 192, 194, 196, 198, 200, 201, 203, 205, 206, 208, 209, 211, 212, 214, 215, 217, 218, 219, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 233, 234, 235, 235, 236, 237, 237, 238, 238, 238, 239, 239, 239, 239, 240, 240, 240, 240, 240, 240, 240, 240, 240, 239, 239, 239, 239, 238, 238, 238, 237, 237, 236, 235, 235, 234, 233, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 219, 218, 217, 215, 214, 212, 211, 209, 208, 206, 205, 203, 201, 200, 198, 196, 194, 192, 190, 188, 187, 185, 182, 180, 178, 176, 174, 172, 170, 167, 165, 163, 161, 158, 156, 153, 151, 149, 146, 144, 141, 139, 136, 133, 131, 128, 125, 123, 120, 117, 115, 112, 109, 106, 103, 100,  98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65,  62,  59,  56,  53,  50,  47,  44,  41,  38,  34,  31,  28,  25,  22,  19,  16,  13,   9,   6,   3};
const uint32_t cos[240] = {240, 240, 240, 240, 240, 239, 239, 239, 239, 238, 238, 238, 237, 237, 236, 235, 235, 234, 233, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 219, 218, 217, 215, 214, 212, 211, 209, 208, 206, 205, 203, 201, 200, 198, 196, 194, 192, 190, 188, 187, 185, 182, 180, 178, 176, 174, 172, 170, 167, 165, 163, 161, 158, 156, 153, 151, 149, 146, 144, 141, 139, 136, 133, 131, 128, 125, 123, 120, 117, 115, 112, 109, 106, 103, 100,  98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65,  62,  59,  56,  53,  50,  47,  44,  41,  38,  34,  31,  28,  25,  22,  19,  16,  13,   9,   6,   3,   0,   3,   6,   9,  13,  16,  19,  22,  25,  28,  31,  34,  38,  41,  44,  47,  50,  53,  56,  59,  62,  65,  68,  71,  74,  77,  80,  83,  86,  89,  92,  95,  98, 100, 103, 106, 109, 112, 115, 117, 120, 123, 125, 128, 131, 133, 136, 139, 141, 144, 146, 149, 151, 153, 156, 158, 161, 163, 165, 167, 170, 172, 174, 176, 178, 180, 182, 185, 187, 188, 190, 192, 194, 196, 198, 200, 201, 203, 205, 206, 208, 209, 211, 212, 214, 215, 217, 218, 219, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 233, 234, 235, 235, 236, 237, 237, 238, 238, 238, 239, 239, 239, 239, 240, 240, 240, 240};


CPWM::CPWM()
{
//Разрешаем тактирование шины портов -----------------------------------------//
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//Настраиваем порты
GPIO_InitTypeDef gpio_init;
//Настраеваем выход ~SD
gpio_init.GPIO_Pin  = GPIO_Pin_13;
gpio_init.GPIO_Mode = GPIO_Mode_OUT;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_25MHz;
GPIO_Init(GPIOC,&gpio_init);

SetOutEnable(false);

//Настраеваем выходы PWM
gpio_init.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;
gpio_init.GPIO_Speed= GPIO_Speed_100MHz;
GPIO_Init(GPIOA,&gpio_init);

//Настраеваем входы измерения тока
gpio_init.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
gpio_init.GPIO_Mode = GPIO_Mode_AN;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOC, &gpio_init);

GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2); 
GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2); 
GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2);

// Lock GPIO Pin
//GPIO_PinLockConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
//GPIO_PinLockConfig(GPIOC, GPIO_Pin_13);	//~SD Pin


// Enable DMA clock ----------------------------------------------------------//
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

DMA_InitTypeDef DMA_InitStructure;

DMA_DeInit(DMA1_Stream5);	//CH1
DMA_InitStructure.DMA_Channel = DMA_Channel_3;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM2->CCR1;
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &sin[0];
DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
DMA_InitStructure.DMA_BufferSize = 240;
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
DMA_InitStructure.DMA_Priority = DMA_Priority_High;
DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
DMA_Init(DMA1_Stream5, &DMA_InitStructure);

DMA_DeInit(DMA1_Stream6);	//CH2
DMA_InitStructure.DMA_Channel = DMA_Channel_3;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM2->CCR2;
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &sin[0];
DMA_Init(DMA1_Stream6, &DMA_InitStructure);

DMA_DeInit(DMA1_Stream1);	//CH3
DMA_InitStructure.DMA_Channel = DMA_Channel_3;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM2->CCR3;
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &cos[0];
DMA_Init(DMA1_Stream1, &DMA_InitStructure);

DMA_DeInit(DMA1_Stream7);	//CH4
DMA_InitStructure.DMA_Channel = DMA_Channel_3;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM2->CCR4;
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &cos[0];
DMA_Init(DMA1_Stream7, &DMA_InitStructure);

DMA_Cmd(DMA1_Stream5, ENABLE);	//CH1
DMA_Cmd(DMA1_Stream6, ENABLE);	//CH2
DMA_Cmd(DMA1_Stream1, ENABLE);	//CH3
DMA_Cmd(DMA1_Stream7, ENABLE);	//CH4


// TIM1 Peripheral Configuration ---------------------------------------------//
TIM_TimeBaseInitTypeDef TIM2_TimeBaseStructure;
TIM_OCInitTypeDef TIM2_OCInitStructure;
TIM_BDTRInitTypeDef TIM2_BDTRInitStructure;

// Enable TIM2 clock
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

// TIM2 Registers reset
TIM_DeInit(TIM2);

// Time Base configuration
TIM_TimeBaseStructInit(&TIM2_TimeBaseStructure);
/*!< Specifies the prescaler value used to divide the TIM clock.
This parameter can be a number between 0x0000 and 0xFFFF */
TIM2_TimeBaseStructure.TIM_Prescaler = 7-1;
TIM2_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
/*!< Specifies the period value to be loaded into the active
Auto-Reload Register at the next update event.
This parameter must be a number between 0x0000 and 0xFFFF.*/ 
TIM2_TimeBaseStructure.TIM_Period = PWM_PERIOD;
TIM2_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//TIM2_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
TIM_TimeBaseInit(TIM2, &TIM2_TimeBaseStructure);

// Channel 1, 2, 3 and 4 Configuration in PWM mode
TIM_OCStructInit(&TIM2_OCInitStructure);
TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM2_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
TIM2_OCInitStructure.TIM_Pulse = 2500; //dummy value
TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM2_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
TIM2_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
TIM2_OCInitStructure.TIM_OCNIdleState = LOW_SIDE_POLARITY;
TIM_OC1Init(TIM2, &TIM2_OCInitStructure);
TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2); 

TIM2_OCInitStructure.TIM_Pulse = 2500; //dummy value
TIM_OC2Init(TIM2, &TIM2_OCInitStructure);
TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM2_OCInitStructure.TIM_Pulse = 250; //dummy value
TIM_OC3Init(TIM2, &TIM2_OCInitStructure);
TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
TIM2_OCInitStructure.TIM_Pulse = 250; //dummy value
TIM_OC4Init(TIM2, &TIM2_OCInitStructure);
TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

// Automatic Output enable, Break, dead time and lock configuration
TIM2_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
TIM2_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
TIM2_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
TIM2_BDTRInitStructure.TIM_DeadTime = DEADTIME;
TIM2_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
TIM2_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
TIM2_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
TIM_BDTRConfig(TIM2, &TIM2_BDTRInitStructure);

TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

TIM_ClearITPendingBit(TIM2, TIM_IT_Break);
TIM_ITConfig(TIM2, TIM_IT_Break, ENABLE);

/* Enable TIM2 DMA */
//TIM_DMACmd(TIM2, TIM_DMA_CC1|TIM_DMA_CC2|TIM_DMA_CC3|TIM_DMA_CC4, ENABLE);

// TIM1 counter enable
TIM_Cmd(TIM2, ENABLE);
}



CPWM::~CPWM()
{
SetOutEnable(false);
}



void CPWM::SetOutEnable(bool state)
{
if (state) GPIO_SetBits(GPIOC, GPIO_Pin_13);
else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}



bool CPWM::GetOutEnable(void)
{
return GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)?true:false;
}


/*
void CPWM::SetOutValue(char chan, int val)
{

}



void CPWM::DMA_Mode(bool mode)
{

}*/



void CPWM::SetMotorSpeed(signed char speed)
{
if (speed==0) SetOutEnable(false);
else SetOutEnable(true);

uint32_t compa, compb;
compa=(PWM_PERIOD/2)+((PWM_PERIOD/2)*speed)/100;
compb=(PWM_PERIOD/2)-((PWM_PERIOD/2)*speed)/100;
if (compa==0) compa=1;
else if (compa==PWM_PERIOD) compa--;
if (compb==0) compb=1;
else if (compb==PWM_PERIOD) compb--;

TIM_SetCompare1(TIM2, compa);
TIM_SetCompare2(TIM2, compb);
}

