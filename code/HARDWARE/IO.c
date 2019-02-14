#include "stm32f10x.h"
#include "sys.h"
#include "IO.h"
#include "delay.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	AllLED_ON();
	LED1 = LED_ON;
	LED2 = LED_OFF;
}
void KEY_Init(void)  //PB0-PB3
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	
	/*	
	//PB3与PB4默认用作调试口，如果用作普通的IO，需要加上以下两句 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	  
}
unsigned char KEY_Scan(unsigned char mode)
{    
    static u8 key_up=1;//按键按松开标志
    if(mode)key_up=1;  //支持连按          
    if(key_up&&(SKEY1==DOWN||SKEY2==DOWN||SKEY3==DOWN||SKEY4==DOWN||SKEY5==DOWN||SKEY6==DOWN||SKEY7==DOWN||SKEY8==DOWN||PAUSE==DOWN))
    {
      delay_ms(10);//去抖动
      key_up=0;
			if(PAUSE==DOWN)return PAUSE_PRES;
      else if(SKEY1==DOWN)return KEY1_PRES;
      else if(SKEY2==DOWN)return KEY2_PRES;
      else if(SKEY3==DOWN)return KEY3_PRES;
      else if(SKEY4==DOWN)return KEY4_PRES;
			else if(SKEY5==DOWN)return KEY5_PRES;
			else if(SKEY6==DOWN)return KEY6_PRES;
			else if(SKEY7==DOWN)return KEY7_PRES;
			else if(SKEY8==DOWN)return KEY8_PRES;
    }else if(SKEY1==FREE && SKEY2==FREE && SKEY3==FREE && SKEY4==FREE && SKEY5==FREE && SKEY6==FREE && SKEY7==FREE && SKEY8==FREE && PAUSE==FREE)key_up=1;         
    return 0;// 无按键按下
}

void AllLED_ON(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
}
void AllLED_OFF(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
}
