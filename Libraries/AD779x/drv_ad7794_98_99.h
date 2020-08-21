


#ifndef _drv_ad7794_98_99
#define _drv_ad7794_98_99

//#define _AD7794
//#define _AD7795
//#define _AD7798
#define _AD7799


#define Delay_ms(x)		vTaskDelay(x);

//==============================================================================
//
//==============================================================================
#define set_CS_AD7794_95_98_99()   { Delay_ms(2); GPIO_SetBits(GPIOD, GPIO_Pin_7);   Delay_ms(5); }
#define clr_CS_AD7794_95_98_99()   { Delay_ms(5); GPIO_ResetBits(GPIOD, GPIO_Pin_7); Delay_ms(2); }
#define pin_MISO_SPI               GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)

#define SPI_REG               SPI1

//==============================================================================
// AD7794_95_98_99 -> установка режима работы MD, управление ключем PSW, настройка фильтра FS
//==============================================================================
// биты Status_Reg
#define WR_DIS                  (1 << 7) // AD7794_95_98_99
#define WR_ENA                  (0 << 7) // AD7794_95_98_99

#define RD                      (1 << 6) // AD7794_95_98_99
#define WR                      (0 << 6) // AD7794_95_98_99

#define CONT_READ_DIS           (0 << 2) // AD7794_95_98_99
#define CONT_READ_ENA           (1 << 2) // AD7794_95_98_99

//==============================================================================
//
//==============================================================================
// регистры
#define Commun_Reg      0 // during a write operation 8 бит дл€ AD7794_95_98_99
#define Status_Reg      0 // during a read operation 8 бит дл€ AD7794_95_98_99
#define Mode_Reg        1 // read_write 16 бит дл€ AD7794_95_98_99
#define Config_Reg      2 // read_write 16 бит дл€ AD7794_95_98_99
#define Data_Reg        3 // read-only 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 
#define ID_Reg          4 // read-only 8 бит дл€ AD7794_95_98_99 
#define IO_Reg          5 // read_write 8 бит дл€ AD7794_95_98_99
#define Offset_Reg      6 // read_write 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 
#define Full_Scale_Reg  7 // read_write 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 



//***************************************************************
// биты Mode_Reg, режима работы MD
//***************************************************************

#define Contin_Convers_Mode (0 << 13) // AD7794_95_98_99
#define Single_Convers_Mode (1 << 13) // AD7794_95_98_99
#define Idle_Mode (2 << 13) // AD7794_95_98_99
#define Power_Down_Mode (3 << 13) // AD7794_95_98_99
#define Inter_Zero_Scale_Calibr (4 << 13) // AD7794_95_98_99
#define Inter_Full_Scale_Calibr (5 << 13) // AD7794_95_98_99
#define Syst_Zero_Scale_Calibr (6 << 13) // AD7794_95_98_99
#define Syst_Full_Scale_Calibr (7 << 13) // AD7794_95_98_99

// управление ключем PSW
#define PSW_Off (0 << 12) // AD7794_95_98_99
#define PSW_On  (1 << 12) // AD7794_95_98_99

#ifdef _AD7794 // || _AD7795
	#define Int_64_kHz_Clk_Not_Clk         (0 << 6) // AD7794_95
	#define Int_64_kHz_Clk_Out_Clk         (1 << 6) // AD7794_95
	#define Ext_64_kHz_Clk                 (2 << 6) // AD7794_95
	#define Ext_64_kHz_Clk_Mul_2           (3 << 6) // AD7794_95

	#define Chop_Ena (1 << 4) // AD7794_95
	#define Chop_Dis (0 << 4) // AD7794_95
#endif

// настройка фильтра FS
#define fADC_Rsd     0    // AD7794_95_98_99 Reserved
#define fADC_500     1    // AD7794_95_98_99 fADC (Hz) -> 500  / tSETTLE (ms) -> 4
#define fADC_250     2    // AD7794_95_98_99 fADC (Hz) -> 250  / tSETTLE (ms) -> 8
#define fADC_125     3    // AD7794_95_98_99 fADC (Hz) -> 125  / tSETTLE (ms) -> 16
#define fADC_62_5    4    // AD7794_95_98_99 fADC (Hz) -> 62.5 / tSETTLE (ms) -> 32
#define fADC_50      5    // AD7794_95_98_99 fADC (Hz) -> 50   / tSETTLE (ms) -> 40
#define fADC_39_2    6    // AD7794_95_98_99 fADC (Hz) -> 39.2 / tSETTLE (ms) -> 48
#define fADC_33_3    7    // AD7794_95_98_99 fADC (Hz) -> 33.3 / tSETTLE (ms) -> 60
#define fADC_19_6    8    // AD7794_95_98_99 fADC (Hz) -> 19.6 / tSETTLE (ms) -> 101 / Rejection @ 50 Hz/60 Hz -> 90 dB (60 Hz only)
#define fADC_16_7_80 9    // AD7794_95_98_99 fADC (Hz) -> 16.7 / tSETTLE (ms) -> 120 / Rejection @ 50 Hz/60 Hz -> 80 dB (50 Hz only)
#define fADC_16_7_65 10   // AD7794_95_98_99 fADC (Hz) -> 16.7 / tSETTLE (ms) -> 120 / Rejection @ 50 Hz/60 Hz -> 65 dB
#define fADC_12_5    11   // AD7794_95_98_99 fADC (Hz) -> 12.5 / tSETTLE (ms) -> 160 / Rejection @ 50 Hz/60 Hz -> 66 dB
#define fADC_10      12   // AD7794_95_98_99 fADC (Hz) -> 10   / tSETTLE (ms) -> 200 / Rejection @ 50 Hz/60 Hz -> 69 dB
#define fADC_8_33    13   // AD7794_95_98_99 fADC (Hz) -> 8.33 / tSETTLE (ms) -> 240 / Rejection @ 50 Hz/60 Hz -> 70 dB
#define fADC_6_25    14   // AD7794_95_98_99 fADC (Hz) -> 6.25 / tSETTLE (ms) -> 320 / Rejection @ 50 Hz/60 Hz -> 72 dB
#define fADC_4_17    15   // AD7794_95_98_99 fADC (Hz) -> 4.17 / tSETTLE (ms) -> 480 / Rejection @ 50 Hz/60 Hz -> 74 dB



//==============================================================================
// биты IO_Reg
//==============================================================================
#ifdef _AD7794 // || _AD7795
	#define Current_Sourc_Con_11_22  (0 << 2) // AD7794_95
	#define Current_Sourc_Con12_21   (1 << 2) // AD7794_95
	#define Both_Current_Sourc_Con_1 (2 << 2) // AD7794_95
	#define Both_Current_Sourc_Con_2 (3 << 2) // AD7794_95

	#define Excitation_Cur_Dis      (0) // AD7794_95
	#define Excitation_Cur_10_ua    (1) // AD7794_95
	#define Excitation_Cur_200_ua   (2) // AD7794_95
	#define Excitation_Cur_1_ma     (3) // AD7794_95
#endif

#define P2_P1_Dig_Ena (1 << 6) // AD7794_95_98_99
#define P2_P1_Dig_Dis (0 << 6) // AD7794_95_98_99

#define P2_Log_1 (1 << 5) // AD7794_95_98_99
#define P2_Log_0 (0 << 5) // AD7794_95_98_99

#define P1_Log_1 (1 << 4) // AD7794_95_98_99
#define P1_Log_0 (0 << 4) // AD7794_95_98_99



//==============================================================================
// биты Config_Reg
//==============================================================================
#ifdef _AD7794 // || _AD7795
	#define Bias_Volt_Gen_Dis  (0 << 14) // AD7794_95
	#define Bias_Volt_Gen_AIN1 (1 << 14) // AD7794_95
	#define Bias_Volt_Gen_AIN2 (2 << 14) // AD7794_95
	#define Bias_Volt_Gen_AIN3 (3 << 14) // AD7794_95

	#define Ext_Ref_REFIN1     (0 << 4) // AD7794_95
	#define Ext_Ref_REFIN2     (1 << 4) // AD7794_95
	#define Int_Ref_1_17V      (2 << 4) // AD7794_95
	#define Ext_Rsd            (3 << 4) // AD7794_95
#endif

#define Bur_Cur_Ena (1 << 13) // AD7794_95_98_99
#define Bur_Cur_Dis (0 << 13) // AD7794_95_98_99

#define Unipolar (1 << 12) // AD7794_95_98_99
#define Bipolar  (0 << 12) // AD7794_95_98_99

#define Ref_Detect_Ena  (1 << 5) // AD7794_95_98_99
#define Ref_Detect_Dis  (0 << 5) // AD7794_95_98_99

#define Buf_Ena  (1 << 4) // AD7794_95_98_99
#define Buf_Dis  (0 << 4) // AD7794_95_98_99
   
#define Gain_1   (0 << 8) // AD7794_95_98_99 1 (in-amp not used) 2.5 V
#define Gain_2   (1 << 8) // AD7794_95_98_99 2 (in-amp not used) 1.25 V
#define Gain_4   (2 << 8) // AD7794_95_98_99 4 625 mV
#define Gain_8   (3 << 8) // AD7794_95_98_99 8 312.5 mV
#define Gain_16  (4 << 8) // AD7794_95_98_99 16 156.2 mV
#define Gain_32  (5 << 8) // AD7794_95_98_99 32 78.125 mV
#define Gain_64  (6 << 8) // AD7794_95_98_99 64 39.06 mV
#define Gain_128 (7 << 8) // AD7794_95_98_99 128 19.53 mV

#ifdef _AD7794 // || _AD7795
	#define Chan_1p_1m      0 // AD7794_95 AIN1(+) Ц AIN1(Ц) 
	#define Chan_2p_2m      1 // AD7794_95 AIN2(+) Ц AIN2(Ц) 
	#define Chan_3p_3m      2 // AD7794_95 AIN3(+) Ц AIN3(Ц) 
	#define Chan_4p_4m      3 // AD7794_95 AIN1(+) Ц AIN1(Ц) 
	#define Chan_5p_5m      4 // AD7794_95 AIN2(+) Ц AIN2(Ц) 
	#define Chan_6p_6m      5 // AD7794_95 AIN3(+) Ц AIN3(Ц) 
	#define Chan_Temp_Sens  6 // AD7794_95 Temp Sensor
	#define AVDD_monit      7 // AD7794_95 AVDD monitor Automatically selects gain = 1/6 and internal reference = 1.17 V
	#define Chan_1m_1m      8 // AD7794_95 AIN1(Ц) Ц AIN1(Ц) 
#endif

#ifdef _AD7799 // || _AD7798
	#define Chan_1p_1m      0 // AD7798_99 AIN1(+) Ц AIN1(Ц) 
	#define Chan_2p_2m      1 // AD7798_99 AIN2(+) Ц AIN2(Ц) 
	#define Chan_3p_3m      2 // AD7798_99 AIN3(+) Ц AIN3(Ц) 
	#define Chan_1m_1m      3 // AD7798_99 AIN1(Ц) Ц AIN1(Ц) 
	#define AVDD_monit      7 // AD7798_99 AVDD monitor Automatically selects gain = 1/6 and internal reference = 1.17 V
#endif



//==============================================================================
// биты COMMUNICATION REGISTER
//==============================================================================
#define AD7799_DIS_WR                       (1 << 7) // AD7794_95_98_99
#define AD7799_ENA_WR                       (0 << 7) // AD7794_95_98_99

#define AD7799_RD                           (1 << 6) // AD7794_95_98_99
#define AD7799_WR                           (0 << 6) // AD7794_95_98_99

#define AD7799_ADR_COMMUNICATION REGISTER   (0 << 3) // during a write operation 8 бит дл€ AD7794_95_98_99
#define AD7799_ADR_STATUS_REGISTER          (0 << 3) // during a read operation 8 бит дл€ AD7794_95_98_99
#define AD7799_ADR_MODE_REGISTER            (1 << 3) // read_write 16 бит дл€ AD7794_95_98_99
#define AD7799_ADR_CONFIGURATION_REGISTER   (2 << 3) // read_write 16 бит дл€ AD7794_95_98_99
#define AD7799_ADR_DATA_REGISTER            (3 << 3) // read-only 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 
#define AD7799_ADR_ID_REGISTER              (4 << 3) // read-only 8 бит дл€ AD7794_95_98_99 
#define AD7799_ADR_IO_REGISTER              (5 << 3) // read_write 8 бит дл€ AD7794_95_98_99
#define AD7799_ADR_OFFSET_REGISTER          (6 << 3) // read_write 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 
#define AD7799_ADR_FULL_SCALE_REGISTER      (7 << 3) // read_write 16 бит дл€ AD7795_98 и 24 бита дл€ AD7794_99 

#define AD7799_CONT_ENA_READ                (1 << 2) // AD7794_95_98_99
#define AD7799_CONT_DIS_READ                (0 << 2) // AD7794_95_98_99



//==============================================================================
//
//==============================================================================
// старший байт слова MODE REGISTER (ADR = 1)
#define AD7799_Contin_Convers_Mode     (0 << 5) // AD7794_95_98_99
#define AD7799_Single_Convers_Mode     (1 << 5) // AD7794_95_98_99
#define AD7799_Idle_Mode               (2 << 5) // AD7794_95_98_99
#define AD7799_Power_Down_Mode         (3 << 5) // AD7794_95_98_99
#define AD7799_Inter_Zero_Scale_Calibr (4 << 5) // AD7794_95_98_99
#define AD7799_Inter_Full_Scale_Calibr (5 << 5) // AD7794_95_98_99
#define AD7799_Syst_Zero_Scale_Calibr  (6 << 5) // AD7794_95_98_99
#define AD7799_Syst_Full_Scale_Calibr  (7 << 5) // AD7794_95_98_99

#define AD7799_PSW_Off                 (0 << 4) // AD7794_95_98_99
#define AD7799_PSW_On                  (1 << 4) // AD7794_95_98_99

// младший байт слова MODE REGISTER (ADR = 1)
#define AD7799_fADC_Rsd                0    // AD7794_95_98_99 Reserved
#define AD7799_fADC_500                1    // AD7794_95_98_99 fADC (Hz) -> 500  / tSETTLE (ms) -> 4
#define AD7799_fADC_250                2    // AD7794_95_98_99 fADC (Hz) -> 250  / tSETTLE (ms) -> 8
#define AD7799_fADC_125                3    // AD7794_95_98_99 fADC (Hz) -> 125  / tSETTLE (ms) -> 16
#define AD7799_fADC_62_5               4    // AD7794_95_98_99 fADC (Hz) -> 62.5 / tSETTLE (ms) -> 32
#define AD7799_fADC_50                 5    // AD7794_95_98_99 fADC (Hz) -> 50   / tSETTLE (ms) -> 40
#define AD7799_fADC_39_2               6    // AD7794_95_98_99 fADC (Hz) -> 39.2 / tSETTLE (ms) -> 48
#define AD7799_fADC_33_3               7    // AD7794_95_98_99 fADC (Hz) -> 33.3 / tSETTLE (ms) -> 60
#define AD7799_fADC_19_6               8    // AD7794_95_98_99 fADC (Hz) -> 19.6 / tSETTLE (ms) -> 101 / Rejection @ 50 Hz/60 Hz -> 90 dB (60 Hz only)
#define AD7799_fADC_16_7_80            9    // AD7794_95_98_99 fADC (Hz) -> 16.7 / tSETTLE (ms) -> 120 / Rejection @ 50 Hz/60 Hz -> 80 dB (50 Hz only)
#define AD7799_fADC_16_7_65           10   // AD7794_95_98_99 fADC (Hz) -> 16.7 / tSETTLE (ms) -> 120 / Rejection @ 50 Hz/60 Hz -> 65 dB
#define AD7799_fADC_12_5              11   // AD7794_95_98_99 fADC (Hz) -> 12.5 / tSETTLE (ms) -> 160 / Rejection @ 50 Hz/60 Hz ->  66 dB
#define AD7799_fADC_10                12   // AD7794_95_98_99 fADC (Hz) -> 10   / tSETTLE (ms) -> 200 / Rejection @ 50 Hz/60 Hz ->  69 dB
#define AD7799_fADC_8_33              13   // AD7794_95_98_99 fADC (Hz) -> 8.33 / tSETTLE (ms) -> 240 / Rejection @ 50 Hz/60 Hz ->  70 dB
#define AD7799_fADC_6_25              14   // AD7794_95_98_99 fADC (Hz) -> 6.25 / tSETTLE (ms) -> 320 / Rejection @ 50 Hz/60 Hz ->  72 dB
#define AD7799_fADC_4_17              15   // AD7794_95_98_99 fADC (Hz) -> 4.17 / tSETTLE (ms) -> 480 / Rejection @ 50 Hz/60 Hz ->  74 dB

//==============================================================================
//
//==============================================================================
// старший байт слова CONFIGURATION REGISTER (ADR = 2)
#define AD7799_ENA_BURCUR       (1 << 5)
#define AD7799_DIS_BURCUR       (0 << 5)
#define AD7799_UNIPOLAR         (1 << 4)
#define AD7799_BIPOLAR          (0 << 4)
#define AD7799_GAIN_128         0x07
#define AD7799_GAIN_64          0x06  
#define AD7799_GAIN_32          0x05
#define AD7799_GAIN_16          0x04
#define AD7799_GAIN_8           0x03
#define AD7799_GAIN_4           0x02
#define AD7799_GAIN_2           0x01
#define AD7799_GAIN_1           0x00
// младший байт слова CONFIGURATION REGISTER (ADR = 2)
#define AD7799_DIS_REF_DET      (0 << 5)
#define AD7799_ENA_REF_DET      (1 << 5)
#define AD7799_DIS_BUF          (0 << 4)
#define AD7799_ENA_BUF          (1 << 4)
#define AD7799_CHAN_AVDD_1_17V  0x07
#define AD7799_CHAN_1N_1N       0x03
#define AD7799_CHAN_3P_3N       0x02
#define AD7799_CHAN_2P_2N       0x01
#define AD7799_CHAN_1P_1N       0x00




extern float temper_1,   temper_2,   temper_3;
extern signed long int adc_sli;
extern unsigned long int adc_uli;

unsigned char Get_Status_AD7794_95_98_99(void);
unsigned long int Read_Reg_AD7794_95_98_99(unsigned char Reg);
void Write_Reg_AD7794_95_98_99(unsigned char Reg, unsigned long int Data);
void Write_Mode_Reg_AD7794_95_98_99(int Data);
void Write_Config_Reg_AD7794_95_98_99(unsigned int Data);
void Write_IO_Reg_AD7794_95_98_99(unsigned char Data);
void Inter_Zero_Scale_Calibr_AD7794_95_98_99(unsigned char NumChan);
void Inter_Full_Scale_Calibr_AD7794_95_98_99(unsigned char NumChan);
void Conf_AD7794_95_98_99(unsigned char NumChan);
unsigned char Get_Single_Conver_AD7794_95_98_99(void);
void Get_Continuous_Conversion_ADC(unsigned char NumChan, unsigned int Gain, unsigned long int * ptrBuf, unsigned char Col);
signed long Get_Next_Continuous_Conversion_ADC(void);
void  ___Set_NumChan_Gain_AD7794_95_98_99(unsigned char NumChan, unsigned char Gain);
void _Set_NumChan_Gain_AD7794_95_98_99(unsigned char NumChan, unsigned char Gain);
void Init_AD7794_95_98_99(void);



#endif