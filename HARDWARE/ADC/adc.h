#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define ADC1_DR_Address 	((u32)0x40012400+0x4c)     //ADC1 DR¼Ä´æÆ÷»ùµØÖ·	

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
 
#endif 
