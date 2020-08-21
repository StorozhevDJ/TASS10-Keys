

#ifndef ZigBeeAPI_h
#define ZigBeeAPI_h


#define DEBUG_MODE		0	//Add in watch debug_rx_buf[]


#define STATUS_MODEM		0x8A	//Статус модема
#define AT_CMD				0x08	//Передача АТ команды
#define ANSWER_AT_CMD		0x88	//Ответ на АТ команду
#define REMOTE_AT_CMD		0x17	//Фрейм запроса удаленной АТ команды
#define ANSWER_REMOTE_AT_CMD	0x97	//Фрейм ответа на удаленную команду
#define QUERY_TRANSMIT		0x10	//Фрейм запроса передачи
#define CMD_LINE_TRANSMIT	0x11	//Командный фрейм прямой передачи
#define TRANSMIT_STATUS		0x8B	//Фрейм статуса передачи
#define RECEIVE_PACKET		0x90	//Фрейм приема пакета
#define INDICATOR_LIN_RECEIVE	0x91	//Фрейм индикатора прямого приема
#define IDENTIFICATION		0x95	//Фрейм идентификации узла







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
	Estatus_modem status_modem;		//Статус модема
	
	unsigned char frame_r_id;		//Идентификатор принятого фрейма
	unsigned char frame_t_id;		//Идентификатор переданного фрейма
	
	unsigned char r_name_at_zigbee[2];	//Название принятой АТ команды
	unsigned char r_data_at_zigbee[16];//Данные принятой АТ команды
	unsigned char len_at_zigbee;		//Данные принятой АТ команды

	unsigned  int r_net_addr_zigbee;	//Принятый сетевой адрес
	unsigned char r_64_addr_zigbee[8];	//Принятый 64х битный адрес
	unsigned char transmit_status;	//Статус передачи
	//unsigned char data_len_zigbee;	//Длинна принимаемых данных
	unsigned char r_data_zigbee[72];	//Принятые от другого устройства данные
	unsigned char receive_status;		//Статус приема

	unsigned char data_len_t_zigbee;	//Длинна передаваемых данных
	unsigned char t_data_zigbee[72];	//Передаваемые данные
	unsigned char t_name_at_zigbee[2];	//Название передаваемой АТ команды
	
	

	//Function
	//Конструктор
	CZigBeeAPI(){Init();};
	//Обработчик прерывания
	void USART_IRQHandler(void);



	/*	Посылаем АТ команды с получение ответа
	cmd - AT command
	t - timeout
	ret_buf - returning data
	return - lenght returning data*/
	unsigned char ATcmd(const char * cmd, unsigned int t, unsigned char * ret_buf);
	
	/*		Передача АТ команды
	cmd - АТ команда
	data - данные для записи АТ команды*/
	void Send_AT_CMD (char const * cmd);	//чтение
	void Send_AT_CMD (char const *cmd, unsigned int data);//Запись
	void Send_AT_CMD (char const *cmd, char *str);//Запись строки
	
	/*		Передача удаленной АТ команды
	adr  - 64 битный адрес устройства назначения
	read - если 0 то запись данных, иначе чтение
	cmd1 - певый байт АТ команды
	cmd2 - второй байт АТ команды
	data - данные для записи АТ команды (если read!=0)*/
	void Send_Remote_AT_CMD(unsigned char * adr, unsigned char read, unsigned char cmd1, unsigned char cmd2, unsigned int data);

	//Передача данных
	/*
	adr - 64 битный адрес устройства назначения
	len - длинна передаваемых из буфера t_data_zigbee, данных*/
	//Передача на конкретный адрес
	void Send_query_transmit_addr	(unsigned char * adr, unsigned int len);
	//Передача своего буфера unsigned char на конкретный адрес (не проверено)
	void Send_query_transmit_addr	(unsigned char * adr,  unsigned int len, unsigned char * buf);
	//Передача любых данных на конкретный адрес (не проверено)
	bool Send_query_transmit_addr	(unsigned char * adr,  unsigned int len, ...);
	//Передача любых данных на конкретный адрес с подтверждением
	bool Send_query_transmit_addr(unsigned char * adr, unsigned int timeout,   unsigned int len, ...);
	//Передача на адрес с которого пришел последний пакет (ответная передача) (не проверено)
	void Send_query_transmit		(unsigned int len);
	
	/*		Отправка данных координатору (без подтверждения приема)
	len - размер передаваемых данных
	... - передаваемые данные
	return - true - данные отправлены, false - ошибка	*/
	bool Send_DataToCoordinator(unsigned char len, ...);
	
	/*		Отправка данных координатору (с подтверждением приема)
	len - размер передаваемых данных
	t   - timeout
	... - передаваемые данные
	return - true - данные доставлены, false - ошибка	*/
	bool Send_DataToCoordinator(unsigned char len, unsigned int t, ...);

	//Прямая передача (не реализована)
	void Send_cmd_line_transmit(unsigned char * adr, unsigned int len);

	//Управление спящим режимом
	void SetSleep(bool state);
	bool GetSleep(void);
	
	void GoBypassMode(void);
	//Exit from transperent mode
	void Escapes(void);
	//Exit AT command mode
	void Exit_AT_cmd_mode(void);
	//Send AT command in AT Command mode
	void Send_AT_cmd_mode(char * at_cmd);
	//Таймер (Необходимо вызывать данную функцию с периодом 1мс)
	void Timer_1ms(void);
	//Каллбек функция вызываемая по приему данных
	void CallBackRecievData(void (*callback)(unsigned char * data, unsigned char len)) {call_back_data_packet=callback;};
	void CallBackRecievAll(void (*callback)(unsigned char receivestatus, unsigned char * data, unsigned char len)) {call_back_other_packet=callback;};

private:
	//Variable
	unsigned char transmit_buf[115];	//Бинарные данные для передачи
	unsigned char transmit_buf_ptr;
	
	unsigned int  timeout_zigbee;
	unsigned short timeout_zigbee_rx;
	//unsigned char receive_buf[115];
	//unsigned char receive_buf_ptr;

	unsigned char data_len_zigbee;	//Длинна принимаемых данных
	unsigned char identifier_api;		//Идентификатор API фрейма
	unsigned char controll_summa;		//Контрольная сумма

	unsigned char step_receive_zigbee;	//Шаг приема пакета, если =0 то прием не идет
	unsigned char data_step_zigbee;	//Шаг приема фрейма API

	volatile unsigned char step_transmit_zigbee;//Шаг передачи пакета
	unsigned char api_step_t_zigbee;	//Шаг передачи фрейма API

	unsigned char controll_summa_t;	//Контрольная сумма на передачу
	unsigned char t_buf_zigbee[86];	//Буфер данных для передачи
	
	void (*call_back_data_packet)(unsigned char * data, unsigned char len);
	void (*call_back_other_packet)(unsigned char receivestatus, unsigned char * data, unsigned char len);
	

	//Инициализация ZigBee
	void Init(void);

	void Transmit_Interrupt(void);
	
	//Запуск передачи
	bool Start_transmit (void);
	//Передача данных через DMA
	void Com_DMASend(uint8_t *source, uint16_t size);
	//Ожидание окончания передачи по DMA
	bool Wait_end_DMASend();
	//Прием данных
	void Receive(unsigned char byte);
	//Передаем байт программно
	void SendChar(char data);
};

extern CZigBeeAPI ZigBeeAPI;

#endif