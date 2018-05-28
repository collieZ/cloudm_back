/*
	���ڶ��� wifiģ��      	�������� ����ģ��	��ʪ��ģ�飺 
	TX��PA2					TX�� PB10 			DATA�� PA1
	RX��PA3 				RX�� PB11 

*/

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "gizwits_product.h"
#include "usart3.h"
#include "dht11.h"


#define Temp_alarm 50 //�¶����ޣ���λ�� C��
#define Humi_alarm 70 //ʪ�����ޣ���λ�� %��
#define alarm_num 6 //��������


//WIFI����״̬
//wifi_sta 0: �Ͽ�
//         1: ������
u8 wifi_sta=0;


//�������ݻ�����
uint8_t read_buff[50]={0};


//�������ݻ�����
//u8 send_buff[50]={0x55,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x55};
u8 send_buff[50]={66,3,3,3,3,3,3,3,3,66};
//u8 send_buff[50]={'s','3','3','3','3','3','3','3','3','e'};

//͸�����ݽ��ձ�־λ
//1�����յ�����  0����
u8 read_sta=0;

//�û�����ǰ�豸״̬�ṹ��
extern dataPoint_t currentDataPoint;


//Э���ʼ��
void Gizwits_Init(void)
{
	TIM3_Int_Init(9,7199);//1MS ϵͳ��ʱ
	uart_init2(9600);//WIFI ��ʼ�� �����ʱ���Ϊ 9600
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();
}

//���ݲɼ�
void userHandle(void)
{
	static u8 t=0;
	static u8 temp,hum;
	static u8 temp_num=0;
	static u8 humi_num=0;
	if(wifi_sta)
	{
		if(t==20)//ÿ 2S ��ȡһ��
		{
			t=0;
			DHT11_Read_Data(&temp,&hum);//��ȡ DHT11 ������
			currentDataPoint.valueTemperature = temp ;//�¶�����
			currentDataPoint.valueHumidity = hum;//ʪ������
			
			if(temp>=Temp_alarm)//�¶�>����ֵ  �쳣
			{
				temp_num++;
				if(temp_num>=alarm_num)		// ���������������ϴ�������Ϣ
				{
					temp_num=0;
					currentDataPoint.valuetemp_alarm=1;//�¶ȴ����ޱ���
					printf("�¶��쳣����\r\n");
					
				}else	// û��������������żȻ�쳣��������
				{
				
					if(currentDataPoint.valuetemp_alarm==0)
						printf("�¶�żȻ�쳣����ע��\r\n");
				}
			}else
			{
				
				currentDataPoint.valuetemp_alarm=0;		// �����¶�
			}
			
			if(hum>=Humi_alarm)//ʪ��>����ֵ
			{
				humi_num++;
				if(humi_num>=alarm_num)//�����ﵽһ������
				{
					
					humi_num=0;
					currentDataPoint.valuehum_alarm=1;//ʪ�ȴ����ޱ���
					printf("ʪ�ȳ�������ֵ���쳣��������\r\n");
				}else
				{
				
					if(currentDataPoint.valuehum_alarm==0)
						printf("ʪ��żȻ�쳣����ע��!!\r\n");
				}
			}else 
			{
				
				currentDataPoint.valuehum_alarm=0;	// ����ʪ��
			}
		}
		if(t%2) LED0=!LED0;   // ״ָ̬ʾ��
			t++;		
	}else
	{
		
		if(temp_num!=0||humi_num!=0)	// ��wifi�Ͽ�ʱ����մ�ȡ��ֵ
		{
			temp_num=0;humi_num=0;
		}
	}
}

 int main(void)
 {		
	int key;
	u8 i;
	int dht11_times = 0;
//	u8 buff[20]={0};
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	KEY_Init(); //������ʼ��
 	LED_Init();			     // LED�˿ڳ�ʼ��
	Gizwits_Init();			// Э���ʼ��
	while(DHT11_Init()) // DHT11 ��ʼ��
	{
		if(dht11_times == 30) 
		{
			printf("--dht11��ʼ��ʧ��--\r\n");
			dht11_times = 0;
		}
		delay_ms(50);
		dht11_times++;
	}
	printf("--dht11��ʼ���ɹ�--\r\n");
	printf("KEY1:AirLink ����ģʽ\t KEY_UP:��λ\r\n\r\n");
   	while(1)
	{
		userHandle();//�û��ɼ�
		
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
			wifi_sta=0;//��־ wifi �ѶϿ�
		}
		
		if(key==KEY0_PRES)//��������
		{
		      if(wifi_sta)
			  { 
				  if(read_sta==0)
				  {
					  memcpy((uint8_t*)&currentDataPoint.valuevaluedata,(uint8_t*)send_buff,
												   sizeof(currentDataPoint.valuevaluedata));
					   //���跢�͵����ݴ浽���ݵ�������
					  printf("send data:");
					  for(i=0;i<sizeof(currentDataPoint.valuevaluedata);i++)
					  printf("%-4x",send_buff[i]);
					  printf("\r\n"); 
				  }
				  
			  }
				
		}
		LED0=!LED0;
		delay_ms(200);		   
	}	 
}	 
 
