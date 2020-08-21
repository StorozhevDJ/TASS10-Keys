/*******************************************************************************
| Library			 : Протокол работы с ZigBee модулем через API			|
| File Name          : ZigBee_API.c									|
| Date               : 15/08/2014										|
| Description        : 												|
| History version	 :												|
| * 06/05/2013 start												|
| * 15/08/2014	Разделение библиотеки на две части: низкоуровневый драйвер и	|
|			ZigBee стек											|
| * 18/08/2014	Добавление калбек функций по приему данных и др. ответов модуля	|
*******************************************************************************/

#include <stdarg.h>
#include <string.h>

#include "../../main.hpp"
#include "ZigBee_API.hpp"
#include "ZigBee_API_Driver.hpp"
//#include "Communicate.h"


CZigBeeAPI ZigBeeAPI;






/*void Receive_ZigBee(unsigned char byte)
{
if (receive_buf_ptr<115)
	{
	receive_buf[receive_buf_ptr++]=byte;
	if (receive_buf[0]!=0x7e) receive_buf_ptr=0;	//waiting Start Delimiter
	}
}*/


/*******************************************************************************
*					Передача по прерыванию						    *
*******************************************************************************/
void CZigBeeAPI::Transmit_Interrupt(void)
{
/*if (transmit_buf_ptr<115)
	{
	USART1->DR=transmit_buf[transmit_buf_ptr++];
	}
return 0;*/

#if ZIGBEEAPI_TX_INTERRUPT
if (step_transmit_zigbee==1)	//Перадача длинны данных MSB
	{
	step_transmit_zigbee++;
	UDR_ZIGBEE=0;
	return;
	}
if (step_transmit_zigbee==2)	//Перадача длинны данных LSB
	{
	step_transmit_zigbee++;
	UDR_ZIGBEE=data_len_t_zigbee;
	return;
	}
if (step_transmit_zigbee==3)	//Данные фрейма
	{
	controll_summa_t-=t_buf_zigbee[api_step_t_zigbee];
	UDR_ZIGBEE=t_buf_zigbee[api_step_t_zigbee];
	if (api_step_t_zigbee++>=data_len_t_zigbee-1) step_transmit_zigbee++;
	return;
	}
if (step_transmit_zigbee>=4)	//Контрольная сумма
	{
	step_transmit_zigbee=0;
	UDR_ZIGBEE=controll_summa_t;
	return;
	}
#endif
}



/*******************************************************************************
*		Подготовака данных и запуск передачи через DMA или программно	    *
*******************************************************************************/
bool CZigBeeAPI::Start_transmit (void)
{
if (timeout_zigbee==0) timeout_zigbee=100;
receive_status=0;	//Сбрасываем статус приема
//Если идет передача
if (step_transmit_zigbee)
	{
	#ifdef ZIGBEEAPI_DMA
	if (!Wait_end_DMASend()) return false;
	#elif ZIGBEEAPI_TX_INTERRUPT
	while (step_transmit_zigbee) if (timeout==0) return false;
	#endif
	}
step_transmit_zigbee=1;	//Выставляем флаг "идет передача"

controll_summa_t=0xff;

transmit_buf[0]=0x7e;
transmit_buf[1]=0x00;
transmit_buf[2]=data_len_t_zigbee;
for (char i=0; i<data_len_t_zigbee; i++)
	{
	controll_summa_t-=t_buf_zigbee[i];
	transmit_buf[3+i]=t_buf_zigbee[i];
	}
transmit_buf[3+data_len_t_zigbee]=controll_summa_t;


#ifdef ZIGBEEAPI_DMA
/*char flag=DMA_GetFlagStatus(DMA1_FLAG_TC4);
flag++;
while(DMA_GetFlagStatus(DMA1_FLAG_TC4));*/

//Запуск передачи буфера в USART через DMA
/*DMA_DeInit(DMA1_Channel4);

DMA_InitTypeDef DMA_InitStructure;
DMA_StructInit(&DMA_InitStructure);
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UDR_ZIGBEE);
DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&transmit_buf[0];
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
DMA_InitStructure.DMA_BufferSize = 4+data_len_t_zigbee;
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_Init(DMA1_Channel4, &DMA_InitStructure);

USART_DMACmd(ZIGBEE_USART, USART_DMAReq_Tx, ENABLE);
DMA_Cmd(DMA1_Channel4, ENABLE);*/

Com_DMASend(&transmit_buf[0], 4+data_len_t_zigbee);

#else
//Программная передача
if (transmit_buf_ptr<115)
	{
	for (char i=0; i<data_len_t_zigbee+4; i++) SendChar(transmit_buf[i]);
	}
else return false;
#endif
ZigBeeAPI.transmit_status=0;
return true;
}



/*******************************************************************************
*					Прием данных по прерыванию					    *
*******************************************************************************/
#if DEBUG_MODE
char debug_rx_buf[100];
char debug_rx_ptr=0;
#endif
void CZigBeeAPI::Receive(unsigned char byte)
{
unsigned char data;
data=UDR_ZIGBEE;

#if DEBUG_MODE
debug_rx_buf[debug_rx_ptr++]=data;
if (debug_rx_ptr>=100) debug_rx_ptr=0;
#endif

if (timeout_zigbee_rx==0)
	{
	data_step_zigbee=0;
	step_receive_zigbee=0;
	controll_summa=0xff;
	}
timeout_zigbee_rx=500;

//Начало пакета
if (step_receive_zigbee==0)
	{
	if (data==0x7e)
		{
		receive_status=0;
		data_step_zigbee=0;
		step_receive_zigbee++;
		controll_summa=0xff;
		}
	return;
	}
	//Длинна данных старший байт
if (step_receive_zigbee==1)
	{
	step_receive_zigbee++;
	return;
	}
	//Длинна данных младший байт
if (step_receive_zigbee==2)
	{
	data_len_zigbee=data;
	if (data_len_zigbee>100) data_len_zigbee=100;
	step_receive_zigbee++;
	return;
	}
	//Идентификатор api фрейма
if (step_receive_zigbee==3)
	{
	identifier_api=data;
	controll_summa-=data;
	step_receive_zigbee++;
	return;
	}
	//Данные api фрейма
if (step_receive_zigbee==4)
	{
	
		//Фрейм статуса модема
	if (identifier_api==STATUS_MODEM)
		{
		status_modem=(Estatus_modem)data;
		step_receive_zigbee++;
		}else
		
		//Фрейм ответа на АТ команду
	if (identifier_api==ANSWER_AT_CMD)
		{
		if (data_step_zigbee==0)		//Идентификатор фрейма
			{
			frame_r_id=data;
			len_at_zigbee=data_len_zigbee-5;
			}
		else if (data_step_zigbee==1)	//Первый байт АТ команды
			{
			r_name_at_zigbee[0]=data;
			}
		else if (data_step_zigbee==2)	//Второй байт АТ команды
			{
			r_name_at_zigbee[1]=data;
			}
		else if (data_step_zigbee==3);//Статус	
		else if (data_step_zigbee>=4)	//Значение
			{
			if ((data_step_zigbee>=data_len_zigbee-1)||(data_step_zigbee>20))
				step_receive_zigbee++;
			else
				{
				r_data_at_zigbee[data_step_zigbee-4]=data;
				}
			}
		data_step_zigbee++;
		}else
		
		//Фрейм ответа на удаленную АТ команду
	if (identifier_api==ANSWER_REMOTE_AT_CMD)
		{
		if (data_step_zigbee==0) frame_r_id=data;		//Идентификатор фрейма
		else if (data_step_zigbee==10) r_name_at_zigbee[data_step_zigbee-1]=data;//Первый байт АТ команды
		else if (data_step_zigbee==11) r_name_at_zigbee[data_step_zigbee-1]=data;//Второй байт АТ команды
		else if (data_step_zigbee==12);	//Статус
		else if (data_step_zigbee>=13)	//Значение
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>8))
				step_receive_zigbee++;
			else r_data_at_zigbee[data_step_zigbee-13]=data;
			}
		}else
		
		//Статус передачи
	if (identifier_api==TRANSMIT_STATUS)
		{
		if (data_step_zigbee==0) frame_r_id=data;		//Идентификатор фрейма
		if (data_step_zigbee==1) r_net_addr_zigbee=data;	//Сетевой адрес H
		if (data_step_zigbee==2) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//Сетевой адрес L
		if (data_step_zigbee==3);					//Количество попыток передачи
		if (data_step_zigbee==4) transmit_status=data;	//Статус передачи
		if (data_step_zigbee>=5) step_receive_zigbee++;	//Статус определения узла и маршрута

		data_step_zigbee++;
		}else
		
		//Прием пакета
	if (identifier_api==RECEIVE_PACKET)
		{
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;	//64х битный адрес
		if (data_step_zigbee==8) r_net_addr_zigbee=data;					//Сетевой адрес H
		if (data_step_zigbee==9)	r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//Сетевой адрес L
		if (data_step_zigbee==10);	//Опции (1-подтвержден, 2-широковещательный)
		else if (data_step_zigbee>=11)//Прием пакета
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>83))
				step_receive_zigbee++;
			else r_data_zigbee[data_step_zigbee-11]=data;
			}
		data_step_zigbee++;
		}else
		
		//Индикатор прямого приема
	if (identifier_api==INDICATOR_LIN_RECEIVE)
		{
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;	//64х битный адрес
		if (data_step_zigbee==8) r_net_addr_zigbee=data;					//Сетевой адрес H
		if (data_step_zigbee==9) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//Сетевой адрес L
		if (data_step_zigbee==10);	//Терминал источник
		if (data_step_zigbee==11);	//Терминал назначения
		if (data_step_zigbee==12);	//Идентификатор кластера
		if (data_step_zigbee==13);	//Идентификатор кластера2
		if (data_step_zigbee==14);	//Идентификатор профиля
		if (data_step_zigbee==15);	//Идентификатор профиля2
		if (data_step_zigbee==16);	//Опции (1-подтвержден, 2-широковещательный)
		else if (data_step_zigbee>=17)//Прием пакета данных
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>(72+17)))
				step_receive_zigbee++;
			else r_data_zigbee[data_step_zigbee-13]=data;
			}
		data_step_zigbee++;
		}else
		
		//Фрейм идентификации узла
	if (identifier_api==IDENTIFICATION)
		{
		//64х битный адрес
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;
		//Сетевой адрес H
		else if (data_step_zigbee==8) r_net_addr_zigbee=data;
		//Сетевой адрес L
		else if (data_step_zigbee==9) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;
		//Опции приема
		else if (data_step_zigbee==10);
		//Принимаем до конца пакета
		else if (data_step_zigbee==data_len_zigbee) step_receive_zigbee++;
		//Step incriment
		data_step_zigbee++;
		}
	
	//Если получили неизвестный фрейм, то будем ждать нового
	else step_receive_zigbee=0;
	//Если конец пакета
	if (data_step_zigbee>=data_len_zigbee-1) step_receive_zigbee++;
	controll_summa-=data;
	return;
	}
	//Контрольная сумма
if (step_receive_zigbee>=5)
	{
	if (controll_summa==data)
		{
		receive_status=identifier_api;
		if (receive_status==RECEIVE_PACKET) call_back_data_packet(&r_data_zigbee[0], data_step_zigbee-1);
		else call_back_other_packet(receive_status, r_data_zigbee, data_step_zigbee-1);
		}
	this->step_receive_zigbee=0;
	return;
	}
}



/*******************************************************************************
*						Таймер								    *
*	Необходимо вызывать данную функцию с периодом 1мс					    *
*******************************************************************************/
void CZigBeeAPI::Timer_1ms(void)
{
if (timeout_zigbee) timeout_zigbee--;
if (timeout_zigbee_rx) timeout_zigbee_rx--;
};



/*******************************************************************************
*				Передача АТ команды на чтение						    *
*	cmd - АТ команды.											    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd)
{
t_buf_zigbee[0]=AT_CMD;		//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;	//Идентификатор фрейма
char i;
for (i=0; i<16; i++)
	{
	if (cmd[i]==0) break;
	t_buf_zigbee[2+i]=cmd[i];//Первые 2 байта - название АТ команды
	}
data_len_t_zigbee=2+i;
if (cmd[3]) if (((cmd[0]=='N')&&(cmd[1]=='I'))||
    ((cmd[0]=='N')&&(cmd[1]=='D'))||
    ((cmd[0]=='D')&&(cmd[1]=='H')))
	{
	t_buf_zigbee[2+i]=0;
	data_len_t_zigbee=2+i+1;
	}

Start_transmit();			//Передаем новый пакет
}



/*******************************************************************************
*				Передача АТ команды для записи					    *
*	cmd1 - певый байт АТ команды									    *
*	cmd2 - второй байт АТ команды									    *
*	data - данные для записи АТ команды							    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd, unsigned int data)
{
t_buf_zigbee[0]=AT_CMD;		//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;	//Идентификатор фрейма
t_buf_zigbee[2]=cmd[0];		//Первый байт названия АТ команды
t_buf_zigbee[3]=cmd[1];		//Второй байт
t_buf_zigbee[4]=(data>>8);	//Старший байт данных
t_buf_zigbee[5]=data&0xff;	//Младший байт данных
data_len_t_zigbee=6;

//while (step_transmit_zigbee!=0);	//Ждем окончания передачи прошлого пакета
Start_transmit();			//Передаем новый пакет
}



/*******************************************************************************
*				Передача АТ команды для записи					    *
*	cmd - АТ команды											    *
*	data - данные для записи АТ команды							    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd, char *str)
{
char str_len;
t_buf_zigbee[0]=AT_CMD;		//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;	//Идентификатор фрейма
t_buf_zigbee[2]=cmd[0];		//Первый байт названия АТ команды
t_buf_zigbee[3]=cmd[1];		//Второй байт
str_len=strlen(str);
for (char i=0; i<str_len; i++) t_buf_zigbee[4+i]=*str++;
data_len_t_zigbee=4+str_len;

//while (step_transmit_zigbee!=0);	//Ждем окончания передачи прошлого пакета
Start_transmit();			//Передаем новый пакет
}



/*******************************************************************************
*					Передача удаленной АТ команды					    *
*	adr  - 64 битный адрес устройства назначения						    *
*	read - если 0 (false), то запись данных, иначе чтение				    *
*	cmd1 - певый байт АТ команды									    *
*	cmd2 - второй байт АТ команды									    *
*	data - данные для записи АТ команды (если read!=0)				    *
*******************************************************************************/
void CZigBeeAPI::Send_Remote_AT_CMD(unsigned char * adr, unsigned char read, unsigned char cmd1, unsigned char cmd2, unsigned int data)
{
t_buf_zigbee[0]=REMOTE_AT_CMD;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x02;			//Опции команды
t_buf_zigbee[13]=cmd1;			//Название команды
t_buf_zigbee[14]=cmd2;
if (read)
	{
	data_len_t_zigbee=15;
	}
else
	{
	t_buf_zigbee[15]=(data>>8);	//Старший байт данных
	t_buf_zigbee[16]=data&0xff;	//Младший байт данных
	data_len_t_zigbee=17;
	}
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				Передача данных на конкретный адрес				    *
*	adr - 64 битный адрес устройства назначения						    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr, unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=t_data_zigbee[i];
	}
data_len_t_zigbee=14+len;
while (step_transmit_zigbee!=0);
Start_transmit();
}




/*******************************************************************************
*				Передача данных на конкретный адрес				    *
*	adr - 64 битный адрес устройства назначения						    *
*	buf - передаваемые данные									    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr,  unsigned int len, unsigned char * buf)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=*buf++;
	}
data_len_t_zigbee=14+len;
//while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				Передача данных на конкретный адрес				    *
*	adr - 64 битный адрес устройства назначения						    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*	... - передаваемые данные									    *
*******************************************************************************/
bool CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr,  unsigned int len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
//Данные (до 72 байт на пакет)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

//while (step_transmit_zigbee!=0);

va_end(list);
Start_transmit();
return true;
}



/*******************************************************************************
*		Передача данных на конкретный адрес с подтвержением			    *
*	adr - 64 битный адрес устройства назначения						    *
*	timeout - время ожидания подтверждения							    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*	... - передаваемые данные									    *
*******************************************************************************/
bool CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr, unsigned int timeout, unsigned int len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
//Данные (до 72 байт на пакет)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

//while (step_transmit_zigbee!=0);

va_end(list);

timeout_zigbee=timeout;//Задаем таймаут

//Отправляем данные по UARTу
if (!Start_transmit()) return false;

//Если таймаут не задан, не ждем подтверждения приема
if (timeout==0) return true;

//Ждем получения подтверждения доставки
while(timeout_zigbee)
	{
	if (receive_status==TRANSMIT_STATUS)	//Подтверждение принято
		{
		if (transmit_status==0) return true;//Данные доставлены успешно
		else return false;				//Данные НЕ доставлены
		}
	}

return false;
}



/*******************************************************************************
*	Передача на адрес с которого пришел последний пакет (ответная передача)   *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit(unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=0; i<8; i++)	//64битный адрес назначения
	{
	t_buf_zigbee[i+2]=r_64_addr_zigbee[i];
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=t_data_zigbee[i];
	}
data_len_t_zigbee=14+len;
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*					Передача на указанный адрес					    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_cmd_line_transmit(unsigned char * adr, unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//Идентификатор API
t_buf_zigbee[1]=++frame_t_id;		//Идентификатор фрейма
for (unsigned char i=2; i<8+2; i++)//64битный адрес назначения
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//Сетевой адрес
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//Радиус широковещательной передачи
t_buf_zigbee[13]=0x00;			//Опции
for (unsigned char i=0; i<=len; i++)
	{
	t_buf_zigbee[13+i]=t_data_zigbee[i];
	}
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				Посылаем АТ команды с получение ответа				    *
*	cmd - название передаваемой команды							    *
*	t - время ожидания ответа, мс									    *
*	ret_buf - буфер для получения ответа							    *
*	return - длинна данных полученного ответа						    *
*******************************************************************************/
unsigned char CZigBeeAPI::ATcmd(const char * cmd, unsigned int t, unsigned char * ret_buf)
{
Send_AT_CMD(cmd);	//Send AT cmd
timeout_zigbee=t;
//Waiting answer an AT cmd
while (receive_status!=ANSWER_AT_CMD) if (timeout_zigbee==0) return 0;
//Compare sended and received cmd
if ((r_name_at_zigbee[0]!=cmd[0])||(r_name_at_zigbee[1]!=cmd[1])) return 0;
for (char i=0; i<=len_at_zigbee; i++) ret_buf[i]=r_data_at_zigbee[i];
return len_at_zigbee;
}



/*******************************************************************************
*			Передача данных мастеру без подтверждения получения		    *
*	len - длинна передаваемых данных из буфера t_data_zigbee			    *
*	... - передаваемые данные									    *
*	return - статус передачи данных:								    *
*		false - передача данных не осуществленна (ошибка)				    *
*		true - данные переданы									    *
*******************************************************************************/
bool CZigBeeAPI::Send_DataToCoordinator(unsigned char len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

//Идентификатор API
t_buf_zigbee[0]=QUERY_TRANSMIT;
//Идентификатор фрейма
t_buf_zigbee[1]=++frame_t_id;
//64битный адрес назначения
t_buf_zigbee[2]=0x00;
t_buf_zigbee[3]=0x00;
t_buf_zigbee[4]=0x00;
t_buf_zigbee[5]=0x00;
t_buf_zigbee[6]=0x00;
t_buf_zigbee[7]=0x00;
t_buf_zigbee[8]=0xff;
t_buf_zigbee[9]=0xff;
//Сетевой адрес мастера
t_buf_zigbee[10]=0x00;			
t_buf_zigbee[11]=0x00;
//Радиус широковещательной передачи
/*Устанавливает количество "скачков" для широковещательной передачи. Если равно 0,
будет установлено максимально возможное значение (10).*/
t_buf_zigbee[12]=0x00;
//Опции
/*0x08- мультивещательная передача.  (Если не установлены - моновещательная.)
Все другие биты должны быть равны 0.*/
t_buf_zigbee[13]=0x00;
//Данные (до 72 байт на пакет)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

va_end(list);

//Отправляем данные по UARTу
Start_transmit();
return true;
}



/*******************************************************************************
*		Передача данных мастеру с подтверждением приема				    *
* len - длинна передаваемых данных									    *
* t   - таймают передачи и ожидания подтверждения (мс)					    *
* ... - передаваемые данные										    *
* return - отчет о доставке (true - доставлено успешно)				    *
*******************************************************************************/
bool CZigBeeAPI::Send_DataToCoordinator(unsigned char len, unsigned int t, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, char *);
char *pp;
pp = va_arg(list, char *);

//Идентификатор API
t_buf_zigbee[0]=QUERY_TRANSMIT;
//Идентификатор фрейма
t_buf_zigbee[1]=++frame_t_id;
//64битный адрес назначения
t_buf_zigbee[2]=0x00;
t_buf_zigbee[3]=0x00;
t_buf_zigbee[4]=0x00;
t_buf_zigbee[5]=0x00;
t_buf_zigbee[6]=0x00;
t_buf_zigbee[7]=0x00;
t_buf_zigbee[8]=0xff;
t_buf_zigbee[9]=0xff;
//Сетевой адрес мастера
t_buf_zigbee[10]=0x00;			
t_buf_zigbee[11]=0x00;
//Радиус широковещательной передачи
/*Устанавливает количество "скачков" для широковещательной передачи.
Если равно 0, будет установлено максимально возможное значение (10).*/
t_buf_zigbee[12]=0x00;
//Опции
/*0x08- мультивещательная передача.  (Если не установлены - моновещательная.)
Все другие биты должны быть равны 0.*/
t_buf_zigbee[13]=0x00;
//Данные (до 72 байт на пакет)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

va_end(list);

timeout_zigbee=t;//Задаем таймаут
frame_r_id=0;

//Отправляем данные по UARTу
if (!Start_transmit()) return false;

//Если таймаут не задан, не ждем подтверждения приема
if (t==0) return true;

//Ждем получения подтверждения доставки
while(timeout_zigbee)
	{
	if ((receive_status==TRANSMIT_STATUS)&&	//Подтверждение принято
	    (frame_t_id==frame_r_id))			//Принят ответ на нужный запрос
		{
		if (transmit_status==0) return true;//Данные доставлены успешно
		else return false;				//Данные НЕ доставлены
		}
	}
return false;
}



/*void InitUsartCom(uint32_t baudrate)
{
GPIO_InitTypeDef gpio;

RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

gpio.GPIO_Mode = GPIO_Mode_AF;
gpio.GPIO_OType = GPIO_OType_PP;
gpio.GPIO_Pin = GPIO_Pin_2;
gpio.GPIO_PuPd = GPIO_PuPd_UP;
gpio.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &gpio);

gpio.GPIO_Pin = GPIO_Pin_3;
GPIO_Init(GPIOA, &gpio);

GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

USART_InitTypeDef com;

com.USART_BaudRate = baudrate;
com.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
com.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
com.USART_Parity = USART_Parity_No;
com.USART_StopBits = USART_StopBits_1;
com.USART_WordLength = USART_WordLength_8b;
USART_Init(USART2, &com);
USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
USART_ClearFlag(USART2, USART_FLAG_TC);

NVIC_InitTypeDef nvic;

NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
nvic.NVIC_IRQChannel = USART2_IRQn;
nvic.NVIC_IRQChannelCmd = ENABLE;
nvic.NVIC_IRQChannelPreemptionPriority = 0;
nvic.NVIC_IRQChannelSubPriority = 0;
NVIC_Init(&nvic);

USART_Cmd(USART2, ENABLE);

return;
}*/






//Exit from transperent mode (go to in AT command mode)
void CZigBeeAPI::Escapes(void)
{
timeout_zigbee=1000;
while(timeout_zigbee);//while(1) SendChar(0xaa);
SendChar('+');
SendChar('+');
SendChar('+');
timeout_zigbee=1500;
while(timeout_zigbee);
}



//Exit from AT command mode (returned to transparent mode)
void CZigBeeAPI::Exit_AT_cmd_mode(void)
{
SendChar('A');
SendChar('T');
SendChar('C');
SendChar('N');
SendChar(0x0D);
}



//Send AT command in AT Command mode
void CZigBeeAPI::Send_AT_cmd_mode(char * at_cmd)
{
SendChar('A');
SendChar('T');
while (*at_cmd) SendChar(*at_cmd++);
SendChar(0x0D);
}



// Go to Bypass mode
void CZigBeeAPI::GoBypassMode(void)
{
SendChar('B');
timeout_zigbee=100;
while(timeout_zigbee);
SendChar('B');
}