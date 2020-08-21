
#ifndef ZigBeeAPI_Driver_h
#define ZigBeeAPI_Driver_h

#define BAUD_RATE		9600L
#define ZIGBEE_USART	USART1
#define UDR_ZIGBEE		USART1->DR

#define ZIGBEEAPI_DMA	1

#define ZIGBEE_SLEEP_RQ	GPIO_Pin_6
#define ZIGBEE_SLEEP_RQ_PORT	GPIOD
//#define ZIGBEE_SLEEP	GPIO_Pin_1
//#define ZIGBEE_RESET	GPIO_Pin_9

#endif	//ZigBeeAPI_Driver_h