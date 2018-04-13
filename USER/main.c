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
	MQ_2();	//烟雾模块初始化
	TIM3_Int_Init(1999,7199);	//频率10KHZ，溢出时间200ms——温湿度模块
	while(DHT11_Init()){			//DHT11初始化
	}
	
	while(1)
	{
		
		
	}
	
}
