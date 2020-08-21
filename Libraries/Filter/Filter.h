

#define FILTER_BUF_SIZE	16

#define MEDIAN_FILTER	1

//Фильтр
typedef struct
	{
	//Массив значений фильтра
	  signed long buf[FILTER_BUF_SIZE];
	//Среднее значение
	  signed long average_data;
	//Предпоследнее значение
	  signed long last_data;
	//Порог чувствительности фильтра (чем меньше значение, тем болше быстродействие, чем больше, тем выше фильтрация)
	unsigned long  delta;
	//Указатель на значение последнего байта в буфере
	unsigned char buf_ptr;
	} Tfilter;


signed long Filter (signed long filter_inpit, Tfilter *filter);