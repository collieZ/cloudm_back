/*
	串口二： wifi模块      	串口三： 蓝牙模块	温湿度模块： 
	TX：PA2					TX： PB10 			DATA： PA1
	RX：PA3 				RX： PB11 

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


#define Temp_alarm 50 //温度上限（单位： C）
#define Humi_alarm 70 //湿度上限（单位： %）
#define alarm_num 6 //报警次数


//WIFI连接状态
//wifi_sta 0: 断开
//         1: 已连接
u8 wifi_sta=0;


//接收数据缓冲区
uint8_t read_buff[50]={0};


//发送数据缓冲区
//u8 send_buff[50]={0x55,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x55};
u8 send_buff[50]={66,3,3,3,3,3,3,3,3,66};
//u8 send_buff[50]={'s','3','3','3','3','3','3','3','3','e'};

//透传数据接收标志位
//1：接收到数据  0：无
u8 read_sta=0;

//用户区当前设备状态结构体
extern dataPoint_t currentDataPoint;


//协议初始化
void Gizwits_Init(void)
{
	TIM3_Int_Init(9,7199);//1MS 系统定时
	uart_init2(9600);//WIFI 初始化 波特率必须为 9600
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();
}

//数据采集
void userHandle(void)
{
	static u8 t=0;
	static u8 temp,hum;
	static u8 temp_num=0;
	static u8 humi_num=0;
	if(wifi_sta)
	{
		if(t==20)//每 2S 读取一次
		{
			t=0;
			DHT11_Read_Data(&temp,&hum);//读取 DHT11 传感器
			currentDataPoint.valueTemperature = temp ;//温度数据
			currentDataPoint.valueHumidity = hum;//湿度数据
			
			if(temp>=Temp_alarm)//温度>报警值  异常
			{
				temp_num++;
				if(temp_num>=alarm_num)		// 超过报警次数，上传报警信息
				{
					temp_num=0;
					currentDataPoint.valuetemp_alarm=1;//温度达上限报警
					printf("温度异常！！\r\n");
					
				}else	// 没超过报警次数，偶然异常，不报警
				{
				
					if(currentDataPoint.valuetemp_alarm==0)
						printf("温度偶然异常，请注意\r\n");
				}
			}else
			{
				
				currentDataPoint.valuetemp_alarm=0;		// 正常温度
			}
			
			if(hum>=Humi_alarm)//湿度>报警值
			{
				humi_num++;
				if(humi_num>=alarm_num)//报警达到一定次数
				{
					
					humi_num=0;
					currentDataPoint.valuehum_alarm=1;//湿度达上限报警
					printf("湿度超过上限值，异常报警！！\r\n");
				}else
				{
				
					if(currentDataPoint.valuehum_alarm==0)
						printf("湿度偶然异常，请注意!!\r\n");
				}
			}else 
			{
				
				currentDataPoint.valuehum_alarm=0;	// 正常湿度
			}
		}
		if(t%2) LED0=!LED0;   // 状态指示灯
			t++;		
	}else
	{
		
		if(temp_num!=0||humi_num!=0)	// 在wifi断开时，清空存取的值
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
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	KEY_Init(); //按键初始化
 	LED_Init();			     // LED端口初始化
	Gizwits_Init();			// 协议初始化
	while(DHT11_Init()) // DHT11 初始化
	{
		if(dht11_times == 30) 
		{
			printf("--dht11初始化失败--\r\n");
			dht11_times = 0;
		}
		delay_ms(50);
		dht11_times++;
	}
	printf("--dht11初始化成功--\r\n");
	printf("KEY1:AirLink 连接模式\t KEY_UP:复位\r\n\r\n");
   	while(1)
	{
		userHandle();//用户采集
		
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
			wifi_sta=0;//标志 wifi 已断开
		}
		
		if(key==KEY0_PRES)//发送数据
		{
		      if(wifi_sta)
			  { 
				  if(read_sta==0)
				  {
					  memcpy((uint8_t*)&currentDataPoint.valuevaluedata,(uint8_t*)send_buff,
												   sizeof(currentDataPoint.valuevaluedata));
					   //将需发送的数据存到数据点数组上
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
 
