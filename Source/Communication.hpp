/*

*/


#ifndef COMMUNICATION_h
#define COMMUNICATION_h


#define ADR_ALL		0
#define ADR_AXEL		1
#define ADR_KOROBKA1	2
#define ADR_KOROBKA2	3
#define ADR_PDU		4



void vTask_Communicate(void *pvParameters);
void ReceiveZigBeeOther(unsigned char receivestatus, unsigned char * data, unsigned char len);
void ReceiveZigBeeData(unsigned char * data, unsigned char len);

class CCom
	{
	public:
	
	

	//private:
	
	};

extern CCom Com;

#endif