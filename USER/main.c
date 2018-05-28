// 2018-4-19  ��ʱ�� ����һ��ֲ���  �������޸�main.c

#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "dht11.h"
#include "timer.h"
#include "adc.h"
#include "MQ_2.h"
#include "usart3.h"
#include "key.h"
#include "gizwits_product.h" 
#include "led.h"

/* �û�����ǰ�豸״̬�ṹ��*/
dataPoint_t currentDataPoint;

/*WIFI����״̬
wifi_sta 0: �Ͽ�
         1: ������*/
extern u8 wifi_sta;

//Э���ʼ��
void Gizwits_Init(void)
{	
	TIM7_Int_Init(9,7199);		// 1MSϵͳ��ʱ
    uart_init2(9600);	// WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));	// �豸״̬�ṹ���ʼ��
	gizwitsInit();		// ��������ʼ��
}



//���ݲɼ�
void userHandle(void)
{
	/*
	
	*/
}

int main(void)
{
	int key;
	
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	usart3_init(9600);
	MQ_2();	//����ģ���ʼ��
	TIM3_Int_Init(1999,7199);	//Ƶ��10KHZ�����ʱ��200ms������ʪ��ģ��
	
//	while(DHT11_Init()){			//DHT11��ʼ��
//	}
	
	while(1)
	{
//		u3_printf("usart3 test");
//		printf("usart1 test");
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦��
		
		key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1 ����
		{
			printf("WIFI ���� AirLink ����ģʽ\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link ģʽ����
		}
		if(key==WKUP_PRES)//KEY_UP ����
		{
			printf("WIFI ��λ����������������\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI ��λ
		}
		delay_ms(200);
		LED0=!LED0;
		
	}
	
}
