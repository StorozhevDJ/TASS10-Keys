

#define FILTER_BUF_SIZE	16

#define MEDIAN_FILTER	1

//������
typedef struct
	{
	//������ �������� �������
	  signed long buf[FILTER_BUF_SIZE];
	//������� ��������
	  signed long average_data;
	//������������� ��������
	  signed long last_data;
	//����� ���������������� ������� (��� ������ ��������, ��� ����� ��������������, ��� ������, ��� ���� ����������)
	unsigned long  delta;
	//��������� �� �������� ���������� ����� � ������
	unsigned char buf_ptr;
	} Tfilter;


signed long Filter (signed long filter_inpit, Tfilter *filter);