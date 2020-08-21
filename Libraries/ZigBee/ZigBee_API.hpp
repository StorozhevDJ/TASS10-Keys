

#ifndef ZigBeeAPI_h
#define ZigBeeAPI_h


#define DEBUG_MODE		0	//Add in watch debug_rx_buf[]


#define STATUS_MODEM		0x8A	//������ ������
#define AT_CMD				0x08	//�������� �� �������
#define ANSWER_AT_CMD		0x88	//����� �� �� �������
#define REMOTE_AT_CMD		0x17	//����� ������� ��������� �� �������
#define ANSWER_REMOTE_AT_CMD	0x97	//����� ������ �� ��������� �������
#define QUERY_TRANSMIT		0x10	//����� ������� ��������
#define CMD_LINE_TRANSMIT	0x11	//��������� ����� ������ ��������
#define TRANSMIT_STATUS		0x8B	//����� ������� ��������
#define RECEIVE_PACKET		0x90	//����� ������ ������
#define INDICATOR_LIN_RECEIVE	0x91	//����� ���������� ������� ������
#define IDENTIFICATION		0x95	//����� ������������� ����







typedef enum 
		{
		reboot_module=0,
		reboot_wd,
		connected,
		disconnected,
		resync,
		resatart_coord,
		start_coord
		} Estatus_modem;



class CZigBeeAPI
{
public:
	//Variable
	Estatus_modem status_modem;		//������ ������
	
	unsigned char frame_r_id;		//������������� ��������� ������
	unsigned char frame_t_id;		//������������� ����������� ������
	
	unsigned char r_name_at_zigbee[2];	//�������� �������� �� �������
	unsigned char r_data_at_zigbee[16];//������ �������� �� �������
	unsigned char len_at_zigbee;		//������ �������� �� �������

	unsigned  int r_net_addr_zigbee;	//�������� ������� �����
	unsigned char r_64_addr_zigbee[8];	//�������� 64� ������ �����
	unsigned char transmit_status;	//������ ��������
	//unsigned char data_len_zigbee;	//������ ����������� ������
	unsigned char r_data_zigbee[72];	//�������� �� ������� ���������� ������
	unsigned char receive_status;		//������ ������

	unsigned char data_len_t_zigbee;	//������ ������������ ������
	unsigned char t_data_zigbee[72];	//������������ ������
	unsigned char t_name_at_zigbee[2];	//�������� ������������ �� �������
	
	

	//Function
	//�����������
	CZigBeeAPI(){Init();};
	//���������� ����������
	void USART_IRQHandler(void);



	/*	�������� �� ������� � ��������� ������
	cmd - AT command
	t - timeout
	ret_buf - returning data
	return - lenght returning data*/
	unsigned char ATcmd(const char * cmd, unsigned int t, unsigned char * ret_buf);
	
	/*		�������� �� �������
	cmd - �� �������
	data - ������ ��� ������ �� �������*/
	void Send_AT_CMD (char const * cmd);	//������
	void Send_AT_CMD (char const *cmd, unsigned int data);//������
	void Send_AT_CMD (char const *cmd, char *str);//������ ������
	
	/*		�������� ��������� �� �������
	adr  - 64 ������ ����� ���������� ����������
	read - ���� 0 �� ������ ������, ����� ������
	cmd1 - ����� ���� �� �������
	cmd2 - ������ ���� �� �������
	data - ������ ��� ������ �� ������� (���� read!=0)*/
	void Send_Remote_AT_CMD(unsigned char * adr, unsigned char read, unsigned char cmd1, unsigned char cmd2, unsigned int data);

	//�������� ������
	/*
	adr - 64 ������ ����� ���������� ����������
	len - ������ ������������ �� ������ t_data_zigbee, ������*/
	//�������� �� ���������� �����
	void Send_query_transmit_addr	(unsigned char * adr, unsigned int len);
	//�������� ������ ������ unsigned char �� ���������� ����� (�� ���������)
	void Send_query_transmit_addr	(unsigned char * adr,  unsigned int len, unsigned char * buf);
	//�������� ����� ������ �� ���������� ����� (�� ���������)
	bool Send_query_transmit_addr	(unsigned char * adr,  unsigned int len, ...);
	//�������� ����� ������ �� ���������� ����� � ��������������
	bool Send_query_transmit_addr(unsigned char * adr, unsigned int timeout,   unsigned int len, ...);
	//�������� �� ����� � �������� ������ ��������� ����� (�������� ��������) (�� ���������)
	void Send_query_transmit		(unsigned int len);
	
	/*		�������� ������ ������������ (��� ������������� ������)
	len - ������ ������������ ������
	... - ������������ ������
	return - true - ������ ����������, false - ������	*/
	bool Send_DataToCoordinator(unsigned char len, ...);
	
	/*		�������� ������ ������������ (� �������������� ������)
	len - ������ ������������ ������
	t   - timeout
	... - ������������ ������
	return - true - ������ ����������, false - ������	*/
	bool Send_DataToCoordinator(unsigned char len, unsigned int t, ...);

	//������ �������� (�� �����������)
	void Send_cmd_line_transmit(unsigned char * adr, unsigned int len);

	//���������� ������ �������
	void SetSleep(bool state);
	bool GetSleep(void);
	
	void GoBypassMode(void);
	//Exit from transperent mode
	void Escapes(void);
	//Exit AT command mode
	void Exit_AT_cmd_mode(void);
	//Send AT command in AT Command mode
	void Send_AT_cmd_mode(char * at_cmd);
	//������ (���������� �������� ������ ������� � �������� 1��)
	void Timer_1ms(void);
	//������� ������� ���������� �� ������ ������
	void CallBackRecievData(void (*callback)(unsigned char * data, unsigned char len)) {call_back_data_packet=callback;};
	void CallBackRecievAll(void (*callback)(unsigned char receivestatus, unsigned char * data, unsigned char len)) {call_back_other_packet=callback;};

private:
	//Variable
	unsigned char transmit_buf[115];	//�������� ������ ��� ��������
	unsigned char transmit_buf_ptr;
	
	unsigned int  timeout_zigbee;
	unsigned short timeout_zigbee_rx;
	//unsigned char receive_buf[115];
	//unsigned char receive_buf_ptr;

	unsigned char data_len_zigbee;	//������ ����������� ������
	unsigned char identifier_api;		//������������� API ������
	unsigned char controll_summa;		//����������� �����

	unsigned char step_receive_zigbee;	//��� ������ ������, ���� =0 �� ����� �� ����
	unsigned char data_step_zigbee;	//��� ������ ������ API

	volatile unsigned char step_transmit_zigbee;//��� �������� ������
	unsigned char api_step_t_zigbee;	//��� �������� ������ API

	unsigned char controll_summa_t;	//����������� ����� �� ��������
	unsigned char t_buf_zigbee[86];	//����� ������ ��� ��������
	
	void (*call_back_data_packet)(unsigned char * data, unsigned char len);
	void (*call_back_other_packet)(unsigned char receivestatus, unsigned char * data, unsigned char len);
	

	//������������� ZigBee
	void Init(void);

	void Transmit_Interrupt(void);
	
	//������ ��������
	bool Start_transmit (void);
	//�������� ������ ����� DMA
	void Com_DMASend(uint8_t *source, uint16_t size);
	//�������� ��������� �������� �� DMA
	bool Wait_end_DMASend();
	//����� ������
	void Receive(unsigned char byte);
	//�������� ���� ����������
	void SendChar(char data);
};

extern CZigBeeAPI ZigBeeAPI;

#endif