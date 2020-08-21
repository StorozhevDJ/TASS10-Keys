

#include <stdlib.h>

#include "Filter.h"


//------------------------------------------------------------------------------
signed long Filter (signed long filter_inpit, Tfilter *filter)
{
//���� �������� ����� ������������ �� ���� ������� � ������� ������� ��������� ������� ������ ������
if (abs(filter->average_data-filter_inpit)<=filter->delta)
	{
	//���������� �������� ������ � �������
	//�������������� ������
	if (filter->buf_ptr>=FILTER_BUF_SIZE) filter->buf_ptr=0;
	//� ������� � ���� ����� ��������
	filter->buf[filter->buf_ptr++]=filter_inpit;
	//��������� ����� ������� ��������
	signed long long temp=0;
	for (char i=0; i<FILTER_BUF_SIZE-2; i++)
		{
#ifdef MEDIAN_FILTER
		//����� ������ ������� �������� �� ���� �������� 
		if      (((filter->buf[i]  < filter->buf[i+1])&& (filter->buf[i]  > filter->buf[i+2]))||
			    ((filter->buf[i]  > filter->buf[i+1])&& (filter->buf[i]  < filter->buf[i+2]))) temp+=filter->buf[i];
		else if (((filter->buf[i+1]< filter->buf[i])  && (filter->buf[i+1]> filter->buf[i+2]))||
			    ((filter->buf[i+1]> filter->buf[i])  && (filter->buf[i+1]< filter->buf[i+2]))) temp+=filter->buf[i+1];
		else temp+=filter->buf[i+2];
#else
		temp+=filter->buf[i];
#endif
		}
	//������� ������� ��������
	filter->average_data=temp/(FILTER_BUF_SIZE-2);
	}
//���� �������� ����� ����������� ��������� ������� � ������� �������� ��������� ������ ������
else if (abs(filter->last_data-filter_inpit)<filter->delta)
	{
	filter->buf[0]=filter->last_data;
	//������� ��� ������ � ������� �� ����� ������
	for (char i=1; i<FILTER_BUF_SIZE; i++)
		{
		filter->buf[i]=filter_inpit;
		}
	filter->buf_ptr=1;
	//�������� ����� ������� ��������
	filter->average_data=(filter_inpit+filter->last_data)/2;
	}
filter->last_data=filter_inpit;

return filter->average_data;
}

