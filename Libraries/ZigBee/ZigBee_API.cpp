/*******************************************************************************
| Library			 : �������� ������ � ZigBee ������� ����� API			|
| File Name          : ZigBee_API.c									|
| Date               : 15/08/2014										|
| Description        : 												|
| History version	 :												|
| * 06/05/2013 start												|
| * 15/08/2014	���������� ���������� �� ��� �����: �������������� ������� �	|
|			ZigBee ����											|
| * 18/08/2014	���������� ������ ������� �� ������ ������ � ��. ������� ������	|
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
*					�������� �� ����������						    *
*******************************************************************************/
void CZigBeeAPI::Transmit_Interrupt(void)
{
/*if (transmit_buf_ptr<115)
	{
	USART1->DR=transmit_buf[transmit_buf_ptr++];
	}
return 0;*/

#if ZIGBEEAPI_TX_INTERRUPT
if (step_transmit_zigbee==1)	//�������� ������ ������ MSB
	{
	step_transmit_zigbee++;
	UDR_ZIGBEE=0;
	return;
	}
if (step_transmit_zigbee==2)	//�������� ������ ������ LSB
	{
	step_transmit_zigbee++;
	UDR_ZIGBEE=data_len_t_zigbee;
	return;
	}
if (step_transmit_zigbee==3)	//������ ������
	{
	controll_summa_t-=t_buf_zigbee[api_step_t_zigbee];
	UDR_ZIGBEE=t_buf_zigbee[api_step_t_zigbee];
	if (api_step_t_zigbee++>=data_len_t_zigbee-1) step_transmit_zigbee++;
	return;
	}
if (step_transmit_zigbee>=4)	//����������� �����
	{
	step_transmit_zigbee=0;
	UDR_ZIGBEE=controll_summa_t;
	return;
	}
#endif
}



/*******************************************************************************
*		����������� ������ � ������ �������� ����� DMA ��� ����������	    *
*******************************************************************************/
bool CZigBeeAPI::Start_transmit (void)
{
if (timeout_zigbee==0) timeout_zigbee=100;
receive_status=0;	//���������� ������ ������
//���� ���� ��������
if (step_transmit_zigbee)
	{
	#ifdef ZIGBEEAPI_DMA
	if (!Wait_end_DMASend()) return false;
	#elif ZIGBEEAPI_TX_INTERRUPT
	while (step_transmit_zigbee) if (timeout==0) return false;
	#endif
	}
step_transmit_zigbee=1;	//���������� ���� "���� ��������"

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

//������ �������� ������ � USART ����� DMA
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
//����������� ��������
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
*					����� ������ �� ����������					    *
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

//������ ������
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
	//������ ������ ������� ����
if (step_receive_zigbee==1)
	{
	step_receive_zigbee++;
	return;
	}
	//������ ������ ������� ����
if (step_receive_zigbee==2)
	{
	data_len_zigbee=data;
	if (data_len_zigbee>100) data_len_zigbee=100;
	step_receive_zigbee++;
	return;
	}
	//������������� api ������
if (step_receive_zigbee==3)
	{
	identifier_api=data;
	controll_summa-=data;
	step_receive_zigbee++;
	return;
	}
	//������ api ������
if (step_receive_zigbee==4)
	{
	
		//����� ������� ������
	if (identifier_api==STATUS_MODEM)
		{
		status_modem=(Estatus_modem)data;
		step_receive_zigbee++;
		}else
		
		//����� ������ �� �� �������
	if (identifier_api==ANSWER_AT_CMD)
		{
		if (data_step_zigbee==0)		//������������� ������
			{
			frame_r_id=data;
			len_at_zigbee=data_len_zigbee-5;
			}
		else if (data_step_zigbee==1)	//������ ���� �� �������
			{
			r_name_at_zigbee[0]=data;
			}
		else if (data_step_zigbee==2)	//������ ���� �� �������
			{
			r_name_at_zigbee[1]=data;
			}
		else if (data_step_zigbee==3);//������	
		else if (data_step_zigbee>=4)	//��������
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
		
		//����� ������ �� ��������� �� �������
	if (identifier_api==ANSWER_REMOTE_AT_CMD)
		{
		if (data_step_zigbee==0) frame_r_id=data;		//������������� ������
		else if (data_step_zigbee==10) r_name_at_zigbee[data_step_zigbee-1]=data;//������ ���� �� �������
		else if (data_step_zigbee==11) r_name_at_zigbee[data_step_zigbee-1]=data;//������ ���� �� �������
		else if (data_step_zigbee==12);	//������
		else if (data_step_zigbee>=13)	//��������
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>8))
				step_receive_zigbee++;
			else r_data_at_zigbee[data_step_zigbee-13]=data;
			}
		}else
		
		//������ ��������
	if (identifier_api==TRANSMIT_STATUS)
		{
		if (data_step_zigbee==0) frame_r_id=data;		//������������� ������
		if (data_step_zigbee==1) r_net_addr_zigbee=data;	//������� ����� H
		if (data_step_zigbee==2) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//������� ����� L
		if (data_step_zigbee==3);					//���������� ������� ��������
		if (data_step_zigbee==4) transmit_status=data;	//������ ��������
		if (data_step_zigbee>=5) step_receive_zigbee++;	//������ ����������� ���� � ��������

		data_step_zigbee++;
		}else
		
		//����� ������
	if (identifier_api==RECEIVE_PACKET)
		{
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;	//64� ������ �����
		if (data_step_zigbee==8) r_net_addr_zigbee=data;					//������� ����� H
		if (data_step_zigbee==9)	r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//������� ����� L
		if (data_step_zigbee==10);	//����� (1-�����������, 2-�����������������)
		else if (data_step_zigbee>=11)//����� ������
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>83))
				step_receive_zigbee++;
			else r_data_zigbee[data_step_zigbee-11]=data;
			}
		data_step_zigbee++;
		}else
		
		//��������� ������� ������
	if (identifier_api==INDICATOR_LIN_RECEIVE)
		{
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;	//64� ������ �����
		if (data_step_zigbee==8) r_net_addr_zigbee=data;					//������� ����� H
		if (data_step_zigbee==9) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;//������� ����� L
		if (data_step_zigbee==10);	//�������� ��������
		if (data_step_zigbee==11);	//�������� ����������
		if (data_step_zigbee==12);	//������������� ��������
		if (data_step_zigbee==13);	//������������� ��������2
		if (data_step_zigbee==14);	//������������� �������
		if (data_step_zigbee==15);	//������������� �������2
		if (data_step_zigbee==16);	//����� (1-�����������, 2-�����������������)
		else if (data_step_zigbee>=17)//����� ������ ������
			{
			if ((data_step_zigbee>=data_len_zigbee)||(data_step_zigbee>(72+17)))
				step_receive_zigbee++;
			else r_data_zigbee[data_step_zigbee-13]=data;
			}
		data_step_zigbee++;
		}else
		
		//����� ������������� ����
	if (identifier_api==IDENTIFICATION)
		{
		//64� ������ �����
		if (data_step_zigbee<=7) r_64_addr_zigbee[data_step_zigbee]=data;
		//������� ����� H
		else if (data_step_zigbee==8) r_net_addr_zigbee=data;
		//������� ����� L
		else if (data_step_zigbee==9) r_net_addr_zigbee=(r_net_addr_zigbee<<8)|data;
		//����� ������
		else if (data_step_zigbee==10);
		//��������� �� ����� ������
		else if (data_step_zigbee==data_len_zigbee) step_receive_zigbee++;
		//Step incriment
		data_step_zigbee++;
		}
	
	//���� �������� ����������� �����, �� ����� ����� ������
	else step_receive_zigbee=0;
	//���� ����� ������
	if (data_step_zigbee>=data_len_zigbee-1) step_receive_zigbee++;
	controll_summa-=data;
	return;
	}
	//����������� �����
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
*						������								    *
*	���������� �������� ������ ������� � �������� 1��					    *
*******************************************************************************/
void CZigBeeAPI::Timer_1ms(void)
{
if (timeout_zigbee) timeout_zigbee--;
if (timeout_zigbee_rx) timeout_zigbee_rx--;
};



/*******************************************************************************
*				�������� �� ������� �� ������						    *
*	cmd - �� �������.											    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd)
{
t_buf_zigbee[0]=AT_CMD;		//������������� API
t_buf_zigbee[1]=++frame_t_id;	//������������� ������
char i;
for (i=0; i<16; i++)
	{
	if (cmd[i]==0) break;
	t_buf_zigbee[2+i]=cmd[i];//������ 2 ����� - �������� �� �������
	}
data_len_t_zigbee=2+i;
if (cmd[3]) if (((cmd[0]=='N')&&(cmd[1]=='I'))||
    ((cmd[0]=='N')&&(cmd[1]=='D'))||
    ((cmd[0]=='D')&&(cmd[1]=='H')))
	{
	t_buf_zigbee[2+i]=0;
	data_len_t_zigbee=2+i+1;
	}

Start_transmit();			//�������� ����� �����
}



/*******************************************************************************
*				�������� �� ������� ��� ������					    *
*	cmd1 - ����� ���� �� �������									    *
*	cmd2 - ������ ���� �� �������									    *
*	data - ������ ��� ������ �� �������							    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd, unsigned int data)
{
t_buf_zigbee[0]=AT_CMD;		//������������� API
t_buf_zigbee[1]=++frame_t_id;	//������������� ������
t_buf_zigbee[2]=cmd[0];		//������ ���� �������� �� �������
t_buf_zigbee[3]=cmd[1];		//������ ����
t_buf_zigbee[4]=(data>>8);	//������� ���� ������
t_buf_zigbee[5]=data&0xff;	//������� ���� ������
data_len_t_zigbee=6;

//while (step_transmit_zigbee!=0);	//���� ��������� �������� �������� ������
Start_transmit();			//�������� ����� �����
}



/*******************************************************************************
*				�������� �� ������� ��� ������					    *
*	cmd - �� �������											    *
*	data - ������ ��� ������ �� �������							    *
*******************************************************************************/
void CZigBeeAPI::Send_AT_CMD (char const *cmd, char *str)
{
char str_len;
t_buf_zigbee[0]=AT_CMD;		//������������� API
t_buf_zigbee[1]=++frame_t_id;	//������������� ������
t_buf_zigbee[2]=cmd[0];		//������ ���� �������� �� �������
t_buf_zigbee[3]=cmd[1];		//������ ����
str_len=strlen(str);
for (char i=0; i<str_len; i++) t_buf_zigbee[4+i]=*str++;
data_len_t_zigbee=4+str_len;

//while (step_transmit_zigbee!=0);	//���� ��������� �������� �������� ������
Start_transmit();			//�������� ����� �����
}



/*******************************************************************************
*					�������� ��������� �� �������					    *
*	adr  - 64 ������ ����� ���������� ����������						    *
*	read - ���� 0 (false), �� ������ ������, ����� ������				    *
*	cmd1 - ����� ���� �� �������									    *
*	cmd2 - ������ ���� �� �������									    *
*	data - ������ ��� ������ �� ������� (���� read!=0)				    *
*******************************************************************************/
void CZigBeeAPI::Send_Remote_AT_CMD(unsigned char * adr, unsigned char read, unsigned char cmd1, unsigned char cmd2, unsigned int data)
{
t_buf_zigbee[0]=REMOTE_AT_CMD;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x02;			//����� �������
t_buf_zigbee[13]=cmd1;			//�������� �������
t_buf_zigbee[14]=cmd2;
if (read)
	{
	data_len_t_zigbee=15;
	}
else
	{
	t_buf_zigbee[15]=(data>>8);	//������� ���� ������
	t_buf_zigbee[16]=data&0xff;	//������� ���� ������
	data_len_t_zigbee=17;
	}
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				�������� ������ �� ���������� �����				    *
*	adr - 64 ������ ����� ���������� ����������						    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr, unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=t_data_zigbee[i];
	}
data_len_t_zigbee=14+len;
while (step_transmit_zigbee!=0);
Start_transmit();
}




/*******************************************************************************
*				�������� ������ �� ���������� �����				    *
*	adr - 64 ������ ����� ���������� ����������						    *
*	buf - ������������ ������									    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr,  unsigned int len, unsigned char * buf)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=*buf++;
	}
data_len_t_zigbee=14+len;
//while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				�������� ������ �� ���������� �����				    *
*	adr - 64 ������ ����� ���������� ����������						    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*	... - ������������ ������									    *
*******************************************************************************/
bool CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr,  unsigned int len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
//������ (�� 72 ���� �� �����)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

//while (step_transmit_zigbee!=0);

va_end(list);
Start_transmit();
return true;
}



/*******************************************************************************
*		�������� ������ �� ���������� ����� � �������������			    *
*	adr - 64 ������ ����� ���������� ����������						    *
*	timeout - ����� �������� �������������							    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*	... - ������������ ������									    *
*******************************************************************************/
bool CZigBeeAPI::Send_query_transmit_addr(unsigned char * adr, unsigned int timeout, unsigned int len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
//������ (�� 72 ���� �� �����)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

//while (step_transmit_zigbee!=0);

va_end(list);

timeout_zigbee=timeout;//������ �������

//���������� ������ �� UART�
if (!Start_transmit()) return false;

//���� ������� �� �����, �� ���� ������������� ������
if (timeout==0) return true;

//���� ��������� ������������� ��������
while(timeout_zigbee)
	{
	if (receive_status==TRANSMIT_STATUS)	//������������� �������
		{
		if (transmit_status==0) return true;//������ ���������� �������
		else return false;				//������ �� ����������
		}
	}

return false;
}



/*******************************************************************************
*	�������� �� ����� � �������� ������ ��������� ����� (�������� ��������)   *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_query_transmit(unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=0; i<8; i++)	//64������ ����� ����������
	{
	t_buf_zigbee[i+2]=r_64_addr_zigbee[i];
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
for (unsigned char i=0; i<len; i++)
	{
	t_buf_zigbee[14+i]=t_data_zigbee[i];
	}
data_len_t_zigbee=14+len;
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*					�������� �� ��������� �����					    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*******************************************************************************/
void CZigBeeAPI::Send_cmd_line_transmit(unsigned char * adr, unsigned int len)
{
t_buf_zigbee[0]=QUERY_TRANSMIT;	//������������� API
t_buf_zigbee[1]=++frame_t_id;		//������������� ������
for (unsigned char i=2; i<8+2; i++)//64������ ����� ����������
	{
	t_buf_zigbee[i]=*adr++;
	}
t_buf_zigbee[10]=0xff;			//������� �����
t_buf_zigbee[11]=0xfe;
t_buf_zigbee[12]=0x00;			//������ ����������������� ��������
t_buf_zigbee[13]=0x00;			//�����
for (unsigned char i=0; i<=len; i++)
	{
	t_buf_zigbee[13+i]=t_data_zigbee[i];
	}
while (step_transmit_zigbee!=0);
Start_transmit();
}



/*******************************************************************************
*				�������� �� ������� � ��������� ������				    *
*	cmd - �������� ������������ �������							    *
*	t - ����� �������� ������, ��									    *
*	ret_buf - ����� ��� ��������� ������							    *
*	return - ������ ������ ����������� ������						    *
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
*			�������� ������ ������� ��� ������������� ���������		    *
*	len - ������ ������������ ������ �� ������ t_data_zigbee			    *
*	... - ������������ ������									    *
*	return - ������ �������� ������:								    *
*		false - �������� ������ �� ������������� (������)				    *
*		true - ������ ��������									    *
*******************************************************************************/
bool CZigBeeAPI::Send_DataToCoordinator(unsigned char len, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, array);
char *pp;
pp = va_arg(list, char *);

//������������� API
t_buf_zigbee[0]=QUERY_TRANSMIT;
//������������� ������
t_buf_zigbee[1]=++frame_t_id;
//64������ ����� ����������
t_buf_zigbee[2]=0x00;
t_buf_zigbee[3]=0x00;
t_buf_zigbee[4]=0x00;
t_buf_zigbee[5]=0x00;
t_buf_zigbee[6]=0x00;
t_buf_zigbee[7]=0x00;
t_buf_zigbee[8]=0xff;
t_buf_zigbee[9]=0xff;
//������� ����� �������
t_buf_zigbee[10]=0x00;			
t_buf_zigbee[11]=0x00;
//������ ����������������� ��������
/*������������� ���������� "�������" ��� ����������������� ��������. ���� ����� 0,
����� ����������� ����������� ��������� �������� (10).*/
t_buf_zigbee[12]=0x00;
//�����
/*0x08- ����������������� ��������.  (���� �� ����������� - ���������������.)
��� ������ ���� ������ ���� ����� 0.*/
t_buf_zigbee[13]=0x00;
//������ (�� 72 ���� �� �����)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

va_end(list);

//���������� ������ �� UART�
Start_transmit();
return true;
}



/*******************************************************************************
*		�������� ������ ������� � �������������� ������				    *
* len - ������ ������������ ������									    *
* t   - ������� �������� � �������� ������������� (��)					    *
* ... - ������������ ������										    *
* return - ����� � �������� (true - ���������� �������)				    *
*******************************************************************************/
bool CZigBeeAPI::Send_DataToCoordinator(unsigned char len, unsigned int t, ...)
{
if (status_modem==disconnected) return false;
if (len>72) return false;

va_list list;
va_start(list, char *);
char *pp;
pp = va_arg(list, char *);

//������������� API
t_buf_zigbee[0]=QUERY_TRANSMIT;
//������������� ������
t_buf_zigbee[1]=++frame_t_id;
//64������ ����� ����������
t_buf_zigbee[2]=0x00;
t_buf_zigbee[3]=0x00;
t_buf_zigbee[4]=0x00;
t_buf_zigbee[5]=0x00;
t_buf_zigbee[6]=0x00;
t_buf_zigbee[7]=0x00;
t_buf_zigbee[8]=0xff;
t_buf_zigbee[9]=0xff;
//������� ����� �������
t_buf_zigbee[10]=0x00;			
t_buf_zigbee[11]=0x00;
//������ ����������������� ��������
/*������������� ���������� "�������" ��� ����������������� ��������.
���� ����� 0, ����� ����������� ����������� ��������� �������� (10).*/
t_buf_zigbee[12]=0x00;
//�����
/*0x08- ����������������� ��������.  (���� �� ����������� - ���������������.)
��� ������ ���� ������ ���� ����� 0.*/
t_buf_zigbee[13]=0x00;
//������ (�� 72 ���� �� �����)
for (unsigned char i=0; i<len; i++) t_buf_zigbee[14+i]=pp[i]; 
data_len_t_zigbee=14+len;

va_end(list);

timeout_zigbee=t;//������ �������
frame_r_id=0;

//���������� ������ �� UART�
if (!Start_transmit()) return false;

//���� ������� �� �����, �� ���� ������������� ������
if (t==0) return true;

//���� ��������� ������������� ��������
while(timeout_zigbee)
	{
	if ((receive_status==TRANSMIT_STATUS)&&	//������������� �������
	    (frame_t_id==frame_r_id))			//������ ����� �� ������ ������
		{
		if (transmit_status==0) return true;//������ ���������� �������
		else return false;				//������ �� ����������
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