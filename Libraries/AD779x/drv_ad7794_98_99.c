//==============================================================================
//
//==============================================================================


#include "drv_ad7794_98_99.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

//#include "main.h"





//==============================================================================
//
//==============================================================================
unsigned char     Num_Chan_AD7794_95_98_99 = 0;
unsigned char     bbbbbb;
unsigned char     a1, a2, a3, a4; 
unsigned long int adc_uli;
  signed long int adc_sli;



//==============================================================================
// аппаратный SPI
//==============================================================================
unsigned char SPI(unsigned char Data)
{
while(SPI_I2S_GetFlagStatus(SPI_REG, SPI_I2S_FLAG_BSY) == SET); //Передатчик занят?
SPI_REG->DR = Data;
return SPI_REG->DR;
}



//==============================================================================
//
//==============================================================================
unsigned char ret_, tmp_;
unsigned char Get_Status_AD7794_95_98_99(void)
{ clr_CS_AD7794_95_98_99();  
  //ret = SPI(WR_ENA + RD + (Status_Reg << 3) + CONT_READ_DIS); // пишем адрес статус регистра
  tmp_ = WR_ENA + RD + (Status_Reg << 3) + CONT_READ_DIS; // пишем адрес статус регистра
  unsigned char ret_ = SPI(tmp_); // читаем Status_Reg
//  __delay_cycles(1000);
  ret_ = SPI(0x00); // читаем Status_Reg    
  set_CS_AD7794_95_98_99();    
  return ret_;
}



//==============================================================================
//
//==============================================================================
unsigned long int Read_Reg_AD7794_95_98_99(unsigned char Reg)
{ union tmpp { unsigned char _uchar[3]; unsigned long int _ulint; } tmp;   
  tmp._ulint = 0;
  
  clr_CS_AD7794_95_98_99();  

  SPI(WR_ENA + RD + (Reg << 3) + CONT_READ_DIS); // пишем адрес статус регистра
  while(pin_MISO_SPI); // ждем готовности

  switch(Reg)
  { 
    //###########################################################
    case Status_Reg : // read-only 8 бит
    case ID_Reg :     // read-only 8 бит 
    case IO_Reg :     // read_write 8 бит
                                tmp._uchar[0] = SPI(0x00); // читаем  биты 7...0
    break;

    //###########################################################    
    case Mode_Reg :       // read_write 16 бит
    case Config_Reg :     // read_write 16 бит    
    #ifdef _AD7795        // || _AD7798
    case Data_Reg :       // read-only 16 бита для AD7795_98
    case Offset_Reg :     // read_write  16 бита для AD7795_98
    case Full_scale_Reg : // read_write 16 бит для AD7795_98
    #endif  
                                tmp._uchar[1] = SPI(0x00); // читаем биты 15...8
                                tmp._uchar[0] = SPI(0x00); // читаем биты 7...0
    break;

    //###########################################################    
    #ifdef _AD7799        // || _AD7794
    case Data_Reg :       // read-only 24 бита для AD7794_99 
    case Offset_Reg :     // read_write 24 бита для AD7794_99 
    case Full_Scale_Reg : // read_write 24 бита для AD7794_99     
                                tmp._uchar[2] = SPI(0x00); // читаем биты 23...16
                                tmp._uchar[1] = SPI(0x00); // читаем биты 15...8
                                tmp._uchar[0] = SPI(0x00); // читаем биты 7...0                                  
    break;
    #endif  

  }    

  set_CS_AD7794_95_98_99();
  return tmp._ulint;
}



//==============================================================================
//
//==============================================================================
void Write_Reg_AD7794_95_98_99(unsigned char Reg, unsigned long int Data)
{ union tmpp { unsigned char _uchar[3]; unsigned long int _ulint; } tmp;   
  tmp._ulint = Data;
  
  clr_CS_AD7794_95_98_99();
  
  SPI(WR_ENA + WR + (Reg << 3) + CONT_READ_DIS); // пишем адрес статус регистра
 
  switch(Reg)
  { 
    //###########################################################
    //case Status_Reg : // read-only 8 бит
    //case ID_Reg : // read-only 8 бит 
    case IO_Reg : // read_write 8 бит                                
    //case Data_Reg : // read-only 24 бита для AD7794_99 
                                SPI(tmp._uchar[0]); // читаем биты 7...0
    break;

    //###########################################################
    case Mode_Reg : // read_write 16 бит
    case Config_Reg : // read_write 16 бит    
    #ifdef _AD7795 // || _AD7798
    case Offset_Reg : // read_write 16 бит для AD7795_98 
    case Full_scale_Reg : // read_write 16 бит для AD7795_98
    #endif  
                                SPI(tmp._uchar[1]); // читаем биты 15...8
                                SPI(tmp._uchar[0]); // читаем биты 7...0
    break;

    //###########################################################
    #ifdef _AD7799 // || _AD7794          
    case Offset_Reg : // read_write  24 бита для AD7794_99 
    case Full_Scale_Reg : // read_write 24 бита для AD7794_99     
                                SPI(tmp._uchar[2]); // читаем биты 23...16
                                SPI(tmp._uchar[1]); // читаем биты 15...8
                                SPI(tmp._uchar[0]); // читаем биты 7...0
    break;
    #endif
    //###########################################################
  }      
  set_CS_AD7794_95_98_99();  
}



//==============================================================================
//
//==============================================================================
void Write_Mode_Reg_AD7794_95_98_99(int Data)
{ clr_CS_AD7794_95_98_99();
  SPI(WR_ENA + WR + (Mode_Reg << 3) + CONT_READ_DIS); // данные для Mode_Reg  
  SPI((unsigned char)(Data >> 8)); SPI((unsigned char)Data); // данные
  set_CS_AD7794_95_98_99();
}



void Write_Config_Reg_AD7794_95_98_99(unsigned int Data)
{ clr_CS_AD7794_95_98_99();
  SPI(WR_ENA + WR + Config_Reg + CONT_READ_DIS); // данные для Mode_Reg
  SPI((unsigned char)(Data >> 8)); SPI((unsigned char)Data); // данные
  set_CS_AD7794_95_98_99();
}



//==============================================================================
//
//==============================================================================
void Write_IO_Reg_AD7794_95_98_99(unsigned char Data)
{ clr_CS_AD7794_95_98_99();
  SPI(WR_ENA + WR + IO_Reg + CONT_READ_DIS); // данные для IO_Reg
  SPI(Data); // данные
  set_CS_AD7794_95_98_99();
}



//==============================================================================
//
//==============================================================================
void Reset_AD7794_95_98_99(void)
{ clr_CS_AD7794_95_98_99();  
  SPI(0xff);
  SPI(0xff);
  SPI(0xff);
  SPI(0xff);
  set_CS_AD7794_95_98_99();   
  Delay_ms(200);
  // конфигурируем
  clr_CS_AD7794_95_98_99();
  SPI(AD7799_ENA_WR + AD7799_WR + AD7799_ADR_MODE_REGISTER + AD7799_CONT_DIS_READ);
  //SPI(AD7799_Single_Convers_Mode + AD7799_PSW_Off); // данные
  SPI(AD7799_Contin_Convers_Mode + AD7799_PSW_On); // данные
  SPI(AD7799_fADC_16_7_80); // данные  
  set_CS_AD7794_95_98_99();
  
  Delay_ms(100); 
}



//==============================================================================
//
//==============================================================================
void Inter_Zero_Scale_Calibr_AD7794_95_98_99(unsigned char NumChan)
{ unsigned int tmp;
  
  clr_CS_AD7794_95_98_99();   
    // while(pin_MISO_SPI); // ждем готовности 
    bbbbbb = AD7799_ENA_WR + AD7799_WR + AD7799_ADR_CONFIGURATION_REGISTER + AD7799_CONT_DIS_READ; // данные для Config_Reg
    bbbbbb = SPI(bbbbbb); // адрес
    bbbbbb = AD7799_DIS_BURCUR + AD7799_UNIPOLAR + 0;//Gain;
    bbbbbb = SPI(bbbbbb); // данные   
    bbbbbb = AD7799_DIS_REF_DET + AD7799_DIS_BUF + NumChan;
    bbbbbb = SPI(bbbbbb); // данные   
  set_CS_AD7794_95_98_99();
  

  clr_CS_AD7794_95_98_99();
  SPI(WR_ENA + WR + (Mode_Reg << 3) + CONT_READ_DIS); // 0x08      
  //tmp = Inter_Zero_Scale_Calibr | PSW_Off | fADC_16_7_80;
  tmp = Inter_Zero_Scale_Calibr | PSW_On | fADC_16_7_80;
  SPI((unsigned char)(tmp >> 8));  SPI((unsigned char)tmp); 
  //while(pin_MISO_SPI); // ждем готовности
  set_CS_AD7794_95_98_99();


  unsigned char uc;
  do { uc = Get_Status_AD7794_95_98_99(); } while((uc & 0x80) != 0);
}  



//==============================================================================
//
//==============================================================================
void Inter_Full_Scale_Calibr_AD7794_95_98_99(unsigned char NumChan)
{ unsigned int tmp;
  clr_CS_AD7794_95_98_99();   
    //while(pin_MISO_SPI); // ждем готовности 
    bbbbbb = AD7799_ENA_WR + AD7799_WR + AD7799_ADR_CONFIGURATION_REGISTER + AD7799_CONT_DIS_READ; // данные для Config_Reg
    bbbbbb = SPI(bbbbbb); // адрес
    bbbbbb = AD7799_DIS_BURCUR + AD7799_UNIPOLAR + 0;//Gain;
    bbbbbb = SPI(bbbbbb); // данные   
    bbbbbb = AD7799_DIS_REF_DET + AD7799_DIS_BUF + NumChan;
    bbbbbb = SPI(bbbbbb); // данные   
  set_CS_AD7794_95_98_99();

  clr_CS_AD7794_95_98_99(); 
  SPI(WR_ENA + WR + (Mode_Reg << 3) + CONT_READ_DIS); // 0x08        
  //tmp = Inter_Full_Scale_Calibr | PSW_Off | fADC_16_7_80;    
  tmp = Inter_Full_Scale_Calibr | PSW_On | fADC_16_7_80;
  SPI((unsigned char)(tmp >> 8)); SPI((unsigned char)tmp); 
//  while(pin_MISO_SPI); // ждем готовности
  set_CS_AD7794_95_98_99(); 
  unsigned char uc;
  do { uc = Get_Status_AD7794_95_98_99(); } while((uc & 0x80) != 0);

}  



//==============================================================================
// функции для обмена с AD7799
//==============================================================================
void Conf_AD7794_95_98_99(unsigned char NumChan)
{ Inter_Zero_Scale_Calibr_AD7794_95_98_99(NumChan); 
  Inter_Full_Scale_Calibr_AD7794_95_98_99(NumChan);
}



//==============================================================================
//
//==============================================================================
void _Set_NumChan_Gain_AD7794_95_98_99(unsigned char NumChan, unsigned char Gain)
{ clr_CS_AD7794_95_98_99();   
  //while(pin_MISO_SPI); // ждем готовности 
  bbbbbb = AD7799_ENA_WR + AD7799_WR + AD7799_ADR_CONFIGURATION_REGISTER + AD7799_CONT_DIS_READ; // данные для Config_Reg
  bbbbbb = SPI(bbbbbb); // адрес
  bbbbbb = AD7799_DIS_BURCUR + AD7799_UNIPOLAR + Gain;
  bbbbbb = SPI(bbbbbb); // данные   
  bbbbbb = AD7799_DIS_REF_DET + AD7799_DIS_BUF + NumChan;
  bbbbbb = SPI(bbbbbb); // данные 
    
  set_CS_AD7794_95_98_99();
}  



//==============================================================================
//
//==============================================================================
void  ___Set_NumChan_Gain_AD7794_95_98_99(unsigned char NumChan, unsigned char Gain)
{ clr_CS_AD7794_95_98_99();   
  // while(pin_MISO_SPI); // ждем готовности 
  bbbbbb = AD7799_ENA_WR + AD7799_WR + AD7799_ADR_CONFIGURATION_REGISTER + AD7799_CONT_DIS_READ; // данные для Config_Reg
  bbbbbb = SPI(bbbbbb); // адрес
  bbbbbb = AD7799_DIS_BURCUR + AD7799_BIPOLAR + Gain;
  bbbbbb = SPI(bbbbbb); // данные   
  bbbbbb = AD7799_DIS_REF_DET + AD7799_DIS_BUF + NumChan;
  bbbbbb = SPI(bbbbbb); // данные   
  set_CS_AD7794_95_98_99();
}  



unsigned int cnt_wait_ready_adc;
//==============================================================================
//
//==============================================================================
unsigned char Get_Single_Conver_AD7794_95_98_99(void)
{ cnt_wait_ready_adc = 15000;

  clr_CS_AD7794_95_98_99(); 

  SPI(AD7799_ENA_WR + 			//Пишем в COMMUNICATION REGISTER
	 AD7799_WR + 				//Разрешаем запись в регистр
	 AD7799_ADR_MODE_REGISTER + 	// read_write 16 бит Mode register  
	 AD7799_CONT_DIS_READ);		//
  //SPI(AD7799_Single_Convers_Mode + AD7799_PSW_Off); // данные
  SPI(AD7799_Single_Convers_Mode + AD7799_PSW_On); // данные
  //старший байт слова MODE REGISTER
//  SPI(AD7799_Contin_Convers_Mode + //Режим непрерывного преобразования
//	 AD7799_PSW_On); 			//Включаем питание моста
  SPI(AD7799_fADC_16_7_80);		//fADC(Hz)->16.7 / tSETTLE(ms)->120 / Rejection @50Hz->80dB

  SPI(0x58); Delay_ms(200);

  // ждем готовности
  while(pin_MISO_SPI) { if(cnt_wait_ready_adc-- == 0) { Reset_AD7794_95_98_99(); return 0; } }
  
  a1 = 0;
  a2 = SPI(0x00); // читаем биты 23...16
  a3 = SPI(0x00); // читаем биты 15...8
  a4 = SPI(0x00); // читаем биты 7...0
  
  set_CS_AD7794_95_98_99();

  adc_uli = ((unsigned long int)a1 << 24) + ((unsigned long int)a2 << 16) + ((unsigned long int)a3 << 8) + ((unsigned long int)a4 << 0);
  //adc_sli = (signed long int) adc_uli;
  return 1;
}



//==============================================================================
//
//==============================================================================
void Get_Continuous_Conversion_ADC(unsigned char NumChan, unsigned int Gain, unsigned long int * ptrBuf, unsigned char Col)
{ union tmpp { unsigned char _uchar[4]; unsigned long int _ulint; } tmp; unsigned int aaaa; 
  clr_CS_AD7794_95_98_99();
  
  SPI(WR_ENA + WR + (Config_Reg << 3) + CONT_READ_ENA); // данные для Config_Reg        
  aaaa = Bur_Cur_Dis + Bipolar + Ref_Detect_Dis + Buf_Dis + Gain + NumChan;
  SPI((unsigned char)(aaaa >> 8));  SPI((unsigned char)aaaa); 
  while(pin_MISO_SPI); // ждем готовности

  SPI(WR_ENA + WR + (Mode_Reg << 3) + CONT_READ_DIS); // данные для Mode_Reg    
  aaaa = Contin_Convers_Mode + PSW_On + fADC_500;    
  SPI((unsigned char)(aaaa >> 8)); SPI((unsigned char)aaaa); // данные
  while(pin_MISO_SPI); // ждем готовности  
   
  while(Col)
  { SPI(WR_ENA + RD + (Data_Reg << 3) + CONT_READ_DIS); // читаем регистр Data 0x58    
    //while(!pin_MISO_SPI); // ждем готовности
    tmp._uchar[3] = SPI(0x00); // читаем биты 23...16
    tmp._uchar[2] = SPI(0x00); // читаем биты 15...8
    tmp._uchar[1] = SPI(0x00); // читаем биты 7...0                                     
    * ptrBuf = tmp._ulint; Col--;
  }
  set_CS_AD7794_95_98_99();
}



//==============================================================================
// Запрашиваем следующие значение при включенном режиме непрерывного измереня
//==============================================================================
signed long Get_Next_Continuous_Conversion_ADC(void)
{
union
	{
	unsigned char _uchar[4];
	unsigned long int _ulint;
	} tmp;

clr_CS_AD7794_95_98_99();

while(pin_MISO_SPI); // ждем готовности  

SPI(WR_ENA + RD + (Data_Reg << 3) + CONT_READ_DIS); // читаем регистр Data 0x58    
while(!pin_MISO_SPI); // ждем готовности
tmp._uchar[3] = 0;
tmp._uchar[2] = SPI(0x00); // читаем биты 23...16
tmp._uchar[1] = SPI(0x00); // читаем биты 15...8
tmp._uchar[0] = SPI(0x00); // читаем биты 7...0                                     

set_CS_AD7794_95_98_99();
return tmp._ulint;
}



//==============================================================================
// 
//==============================================================================
void Set_P0_AD7794_95_98_99()
{ unsigned char tmp;
  tmp = Read_Reg_AD7794_95_98_99(5);
  tmp |= 0x40 + 0x10;
  Write_Reg_AD7794_95_98_99(5, tmp);
}
void Clr_P0_AD7794_95_98_99()
{ unsigned char tmp;
  tmp = Read_Reg_AD7794_95_98_99(5);
  tmp |= 0x40; tmp &= ~0x10;
  Write_Reg_AD7794_95_98_99(5, tmp);
}
void Set_P1_AD7794_95_98_99()
{ unsigned char tmp;
  tmp = Read_Reg_AD7794_95_98_99(5);
  tmp |= 0x40 + 0x20;
  Write_Reg_AD7794_95_98_99(5, tmp);
}
void Clr_P1_AD7794_95_98_99()
{ unsigned char tmp;
  tmp = Read_Reg_AD7794_95_98_99(5);
  tmp |= 0x40; tmp &= ~0x20;
  Write_Reg_AD7794_95_98_99(5, tmp);
}



//==============================================================================
// Инициализация интерфейса работы с АЦП
//==============================================================================
void Init_AD7794_95_98_99(void)
{
// Тактирование порта А
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

//GPIO_InitTypeDef GPIO_Init_struct;
/*
    // PB12 - (вход ADC_CS)    
  GPIO_Init_struct.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_Init_struct);  
  
  // PB13 - (выход ADC_CLK)  
  GPIO_Init_struct.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_Init_struct);   
  
  // PB14 - (вход ADC_DOUT) MISO   
  GPIO_Init_struct.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_Init_struct);  
  
  // PB15 - (выход ADC_DIN) MOSI  
  GPIO_Init_struct.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_Init_struct);     
    */
  
GPIO_InitTypeDef gpio_init;

//Настраеваем выход CS
gpio_init.GPIO_Pin  = GPIO_Pin_7;			//~ADC_CS
gpio_init.GPIO_Mode = GPIO_Mode_OUT;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOD,&gpio_init);

//Настраеваем выходы SPI ADC
gpio_init.GPIO_Pin=(GPIO_Pin_5|			//MOSI
				GPIO_Pin_3);			//SCK
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_PP;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOB,&gpio_init);

//Настраеваем вход SPI ADC
gpio_init.GPIO_Pin=GPIO_Pin_4;			//MISO
gpio_init.GPIO_Mode = GPIO_Mode_AF;
gpio_init.GPIO_OType= GPIO_OType_OD;
gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
gpio_init.GPIO_Speed= GPIO_Speed_2MHz;
GPIO_Init(GPIOB,&gpio_init);

GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

//Настройка SPI интерфейса
// Тактирование модуля SPI1
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

SPI_InitTypeDef SPI_InitStructure;

//Заполняем структуру с параметрами SPI модуля
SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полный дуплекс
SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // передаем по 8 бит
SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // Полярность и
SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // фаза тактового сигнала
SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // Управлять состоянием сигнала NSS программно
SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // Предделитель SCK
SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Первым отправляется старший бит
SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // Режим - мастер
SPI_Init(SPI1, &SPI_InitStructure); //Настраиваем SPI1

// Поскольку сигнал NSS контролируется программно, установим его в единицу
// Если сбросить его в ноль, то наш SPI модуль подумает, что
// у нас мультимастерная топология и его лишили полномочий мастера.
SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set);

SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1....

Reset_AD7794_95_98_99();
}

