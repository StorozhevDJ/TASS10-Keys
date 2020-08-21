
#include "ZigBee_API.hpp"
#include "ZigBee_API_Driver.hpp"


void CZigBeeAPI::Init(void)
{
//Разрешаем тактирование шины портов
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

GPIO_InitTypeDef gpio_init;
//Настраеваем вход RX
gpio_init.GPIO_Pin  = GPIO_Pin_7;
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOB,&gpio_init);

//Настраеваем выход TX
gpio_init.GPIO_Pin  = GPIO_Pin_6;
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOB,&gpio_init);

//Настраеваем выход ~ZB_SLEEP
#ifdef ZIGBEE_SLEEP_RQ
gpio_init.GPIO_Pin  = ZIGBEE_SLEEP_RQ;
gpio_init.GPIO_Mode = GPIO_Mode_OUT;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOD,&gpio_init);

SetSleep(false);
#endif

//Настраеваем вход ~SLEEP
#ifdef ZIGBEE_SLEEP
gpio_init.GPIO_Pin  = ZIGBEE_SLEEP;
gpio_init.GPIO_Mode = GPIO_Mode_IN;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOC,&gpio_init);
#endif

#ifdef ZIGBEE_RESET
GPIO_WriteBit(GPIOB, ZIGBEE_RESET, Bit_SET);
#endif

//Включаем тактирование шины USART
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

//Инициализируем USART
USART_InitTypeDef usart_init_struct;
USART_StructInit(&usart_init_struct);

usart_init_struct.USART_BaudRate=BAUD_RATE;
usart_init_struct.USART_WordLength=USART_WordLength_8b;
usart_init_struct.USART_StopBits=USART_StopBits_1;
usart_init_struct.USART_Parity=USART_Parity_No;
usart_init_struct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
usart_init_struct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;

USART_Init(ZIGBEE_USART, &usart_init_struct);

NVIC_InitTypeDef NVIC_InitStructure;

NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitStructure);

//Разрешаем прерывания по приему и передаче
USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_RXNE);
USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_TXE);
USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_TC);
//USART_ITConfig(ZIGBEE_USART, USART_IT_TXE, ENABLE);
USART_ITConfig(ZIGBEE_USART, USART_IT_RXNE, ENABLE);

//Включаем USART1
USART_Cmd(ZIGBEE_USART, ENABLE);

//Включаем тактирование шины DMA
#ifdef ZIGBEEAPI_DMA
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

//Инициализируем DMA
DMA_InitTypeDef   DMA_InitStructure;
DMA_InitStructure.DMA_Channel = DMA_Channel_4;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UDR_ZIGBEE);
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)t_data_zigbee;
DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
DMA_InitStructure.DMA_BufferSize = 72;
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
DMA_Init(DMA2_Stream7, &DMA_InitStructure);
//Включаем DMA
DMA_Cmd(DMA2_Stream7, ENABLE);
#endif
}



//------------------------------------------------------------------------------
//			Прерывание по приему/передаче данных по USART
//------------------------------------------------------------------------------
void CZigBeeAPI::USART_IRQHandler(void)
{
//LED_ON;
//Если прерывания по приему
while (ZIGBEE_USART->SR & USART_SR_RXNE)
	{
	USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_RXNE);
	//USART2->SR &= ~USART_SR_RXNE;
	//Передаем принятый байт в функцию
	Receive(ZIGBEE_USART->DR);
	}
// Если прерывание по завершению передачи.
if ((ZIGBEE_USART->SR & USART_SR_TXE)||(ZIGBEE_USART->SR & USART_SR_TC))
	{
	//Transmit_Interrupt();
	//USART1->SR &= ~USART_SR_TXE;
	USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_TXE);
	USART_ClearITPendingBit(ZIGBEE_USART, USART_IT_TC);
	}
}



void CZigBeeAPI::Com_DMASend(unsigned char *source, uint16_t size)
{
DMA_InitTypeDef dma;

DMA_DeInit(DMA2_Stream7);

dma.DMA_BufferSize = size;
dma.DMA_Memory0BaseAddr = (uint32_t)source;
dma.DMA_Channel = DMA_Channel_4;
dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
dma.DMA_Mode = DMA_Mode_Normal;
dma.DMA_PeripheralBaseAddr = (uint32_t)&(UDR_ZIGBEE);
dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
dma.DMA_Priority = DMA_Priority_Medium;
DMA_Init(DMA2_Stream7, &dma);


/*dma_nvic.NVIC_IRQChannel = DMA2_Stream7_IRQn;
dma_nvic.NVIC_IRQChannelCmd = ENABLE;
dma_nvic.NVIC_IRQChannelPreemptionPriority = 1;
dma_nvic.NVIC_IRQChannelSubPriority = 0;
NVIC_Init(&dma_nvic);*/
USART_DMACmd(ZIGBEE_USART, USART_DMAReq_Tx, ENABLE);
DMA_Cmd(DMA2_Stream7, ENABLE);

return;
}



/*void DMA2_Stream7_IRQHandler(void)
{
if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
	{
     USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
     DMA_Cmd(DMA2_Stream7, DISABLE);
	step_transmit_zigbee=0;
	}
}*/



bool CZigBeeAPI::Wait_end_DMASend()
{
while (DMA_GetFlagStatus (DMA2_Stream7, DMA_FLAG_TCIF7) == RESET)//Дождемся её окончания
	{
	if (timeout_zigbee==0) return false;
	}
DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
return true;
}



/*******************************************************************************
*					Выставление режима Sleep
*	state = true - включение спящего режима
*	state = false - вЫключение спящего режима
*******************************************************************************/
void CZigBeeAPI::SetSleep(bool state)
{
if (state) GPIO_SetBits(ZIGBEE_SLEEP_RQ_PORT, ZIGBEE_SLEEP_RQ);
else GPIO_ResetBits(ZIGBEE_SLEEP_RQ_PORT, ZIGBEE_SLEEP_RQ);
}



/*******************************************************************************
*					Запрос режима Sleep
*	true - включен спящий режим
*	false - вЫключен спящий режим
*******************************************************************************/
#ifdef ZIGBEE_SLEEP
bool CZigBeeAPI::GetSleep(void)
{
return GPIO_ReadInputDataBit(GPIOB, ZIGBEE_SLEEP)?true:false;
}
#endif



// Send one byte
void CZigBeeAPI::SendChar(char data)
{
while (USART_GetFlagStatus(ZIGBEE_USART, USART_FLAG_TXE) == RESET);
USART_SendData(ZIGBEE_USART, data);
}