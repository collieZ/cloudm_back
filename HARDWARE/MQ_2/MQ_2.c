#include "sys.h"
#include "MQ_2.h"

#define Mq_2 PFin(0)

void MQ_2(void){
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);	 //使能PF端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_Init(GPIOF, &GPIO_InitStructure);					
}

u8 checkMq_2(void){
	
	if(Mq_2 == 1){
	
		//正常
		return 1;
	} else{
		
		//报警
		return 0;
	}
}

