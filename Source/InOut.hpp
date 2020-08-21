

#ifndef INOUT_h
#define INOUT_h


#define PORT_IO1_2	GPIOA
#define PIN_IO1	GPIO_Pin_5	//Key UP
#define PIN_IO2	GPIO_Pin_4	//Key Down

#define PORT_IO3_4	GPIOC
#define PIN_IO3	GPIO_Pin_6	//Encoder A
#define PIN_IO4	GPIO_Pin_7	//Encoder B

#define PORT_IO5_6	GPIOB
#define PIN_IO5	GPIO_Pin_8	//FED 1
#define PIN_IO6	GPIO_Pin_9	//FED 2

#define PORT_IO7	GPIOE
#define PIN_IO7	GPIO_Pin_1	//Gerkon

#define PORT_IO8	GPIOE
#define PIN_IO8	GPIO_Pin_0	//N/C

#define PORT_OUT	GPIOB
#define PIN_OUT1	GPIO_Pin_1	//LED "Связь"
#define PIN_OUT2	GPIO_Pin_0	//LED "Низкий заряд"

#define PORT_AIN	GPIOA
#define PIN_AIN1	GPIO_Pin_7	//Laser
#define PIN_AIN2	GPIO_Pin_6



class CInOut
	{
	public:
	CInOut();
	~CInOut(){};
	
	bool KeyUp(void);
	bool KeyDown(void);
	bool Gerkon(void);
	void LedLink(bool state);
	void LedBattLL(bool state);
	unsigned char FED(void);
	uint32_t GetEncoder(void);
	void SetEncoder(uint32_t count);
	inline u16 GetLaser(void){return ReadADC1(7);};
	inline u16 GetUAKB(void){return ReadADC1(14);};
	
	u16 ReadADC1(u8 channel);
	
	private:
	void GPIOInit(void);
	void EncoderInit(void);
	void ADCInit(void);
	};

extern CInOut InOut;

#endif