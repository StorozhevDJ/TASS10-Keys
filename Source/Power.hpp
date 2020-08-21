

#ifndef POWER_h
#define POWER_h


#define PORT_POWER_ON	GPIOE
#define PIN_POWER_ON	GPIO_Pin_5	//Power_On

#define PORT_KEY_ON_OFF	GPIOE
#define PIN_KEY_ON_OFF	GPIO_Pin_6	//Key On/Off

#define PORT_UBAT		GPIOC
#define PIN_UBAT		GPIO_Pin_4	//Battery power voltage

#define PORT_12V_SHDN	GPIOA
#define PIN_12V_SHDN	GPIO_Pin_15	//12V step Up converter enable




class CPower
	{
	public:
	CPower();
	~CPower();
	
	inline void On(void);
	void Off(void);
	bool KeyOnOff(void);
	
	private:
	void GPIOInit(void);
	};

#endif