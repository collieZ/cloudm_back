#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "dht11.h"
#include "timer.h"
#include "adc.h"
#include "MQ_2.h"
#include "usart3.h"




int main(void)
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	usart3_init(9600);
	MQ_2();	//����ģ���ʼ��
	TIM3_Int_Init(1999,7199);	//Ƶ��10KHZ�����ʱ��200ms������ʪ��ģ��
	while(DHT11_Init()){			//DHT11��ʼ��
	}
	
	while(1)
	{
		
		
	}
	
}
