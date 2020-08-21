

#ifndef MEASURE_h
#define MEASURE_h



typedef enum
	{
	no_measure=0,
	f,
	time,
	angle,
	vertical
	}Emeasure_type;



void vTask_Main(void *pvParameters);

class CMeasure
	{
	public:
	Emeasure_type measure_type;
	struct
		{
		bool vertical;
		bool height_2100;
		  signed short angle;
		unsigned short time_up;
		unsigned short time_down;
		  signed short f_up[15];
		  signed short f_down[15];
		  signed short presure_up[15];
		  signed short presure_down[15];
		} data;
	
	void Main (void);
	
	void F (void);
	void Time(void);
	void Angle (void);
	void Vertical (void);
	};

extern CMeasure Measure;

#endif