 #include "adc.h"
 #include "delay.h"
 #include "usart.h"

   
__IO uint16_t ADC_ConvertedValue;		//采样值变量
float ADC_ConvertedValueLocal; 	//烟雾传感器数值

void MQ_2_AverageValue(float value);

//初始化ADC																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); //使能MNA1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA0 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit(DMA1_Channel1); //复位dma1
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t)ADC1_DR_Address;	//源地址  ADC地址   PeripheralBaseAddr 外部基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; //内存地址(要传输的变量的指针)  MemoryBaseAddr内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //传输方向设置  单向
	DMA_InitStructure.DMA_BufferSize = 1;	//传输缓存区大小 1 传输次数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //定义地址在传送后是否会递增 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存到内存后地址递增
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //内存数据单位    半字节  16位 
	DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_HalfWord ; //外设数据单位 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; //DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ; //优先级：高  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   //禁止内存到内存的传输 
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1的1通道  
	
	DMA_Cmd(DMA1_Channel1,ENABLE);  //使能DMA
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE); //传输结束中断
	
	
	ADC_DeInit(ADC1);  //复位ADC1 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5); //配置ADC1通道0为239个采样周期  
	
	ADC_DMACmd(ADC1,ENABLE);  //ADC DMA使能
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
    
	ADC_ResetCalibration(ADC1);	//使能复位校准   
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 


float ADC_RealValueLocal = 0;
float MQ_2Concentration =300; //MQ-2浓度 ppm单位 初始300


//============================================
//函数名称:DMA1_Channel1_IRQHandler
//功能描述:DMA中断 对AD采集值平均滤波
//输入:无
//输出:无
//============================================
void DMA1_Channel0_IRQHandler(void)		//DMA中断
{
	
	if(DMA_GetITStatus(DMA1_IT_TC1)!=0)
	{
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue*3.3/4096; // 值转换
		MQ_2_AverageValue(ADC_ConvertedValueLocal);
		printf("报警");
//		if(MQ_2Concentration>=2700)
//		{
//			
//		}
	}
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		DMA_ClearFlag(DMA1_FLAG_TC1);
}


void MQ_2_AverageValue(float value)		//均值滤波
{
	static float temp_val= 0;
	static u8 TIMES = 0;
	
	TIMES++;
	temp_val+=value;
	if(TIMES >= 20)
	{
		ADC_RealValueLocal = temp_val/TIMES;
		if(ADC_RealValueLocal<=0.15)
		{
			ADC_RealValueLocal=0.15;
		}
		MQ_2Concentration = (ADC_RealValueLocal-0.15)*100*20+300;		//ppm单位
		TIMES = 0;	
		temp_val=0;
	}
}




















