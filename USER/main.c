// 2018-4-19  定时器 串口一移植完毕  接下来修改main.c

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

/* 用户区当前设备状态结构体*/
dataPoint_t currentDataPoint;

/*WIFI连接状态
wifi_sta 0: 断开
         1: 已连接*/
extern u8 wifi_sta;

//协议初始化
void Gizwits_Init(void)
{	
	TIM7_Int_Init(9,7199);		// 1MS系统定时
    uart_init2(9600);	// WIFI初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));	// 设备状态结构体初始化
	gizwitsInit();		// 缓冲区初始化
}



//数据采集
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
	MQ_2();	//烟雾模块初始化
	TIM3_Int_Init(1999,7199);	//频率10KHZ，溢出时间200ms——温湿度模块
	
//	while(DHT11_Init()){			//DHT11初始化
//	}
	
	while(1)
	{
//		u3_printf("usart3 test");
//		printf("usart1 test");
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理
		
		key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1 按键
		{
			printf("WIFI 进入 AirLink 连接模式\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link 模式接入
		}
		if(key==WKUP_PRES)//KEY_UP 按键
		{
			printf("WIFI 复位，请重新配置连接\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI 复位
		}
		delay_ms(200);
		LED0=!LED0;
		
	}
	
}
