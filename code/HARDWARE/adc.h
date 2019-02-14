#ifndef __ADC_H
#define __ADC_H    
#include "sys.h"
void ADC_Config(void);
#define ADC1_DR_Address    ((u32)0x4001244C)//ADC规则数据寄存器，ADC_DR,含规则通道的转换结果，只用16位


extern volatile u16 ADC_ConvertedValue[6];
extern const u16 tempRes_buf[51] ;
#endif
