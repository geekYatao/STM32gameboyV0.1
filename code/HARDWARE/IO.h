#ifndef __IO_H
#define __IO_H    


#define SLED1 PCout(0)
#define SLED2 PCout(1)
#define SLED3 PCout(2)
#define SLED4 PCout(3)
#define LED1 PCout(4)
#define LED2 PCout(5)
#define SLED5 PBout(12)
#define SLED6 PBout(13)
#define SLED7 PBout(14)
#define SLED8 PBout(15)
#define SKEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define SKEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define SKEY3 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)
#define SKEY4 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define SKEY5 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)
#define SKEY6 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define SKEY7 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define SKEY8 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)

#define PAUSE GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define BEEP PBout(1)

#define KEY1_PRES    1    //KEY1按下
#define KEY2_PRES    2    //KEY2按下
#define KEY3_PRES    3    //KEY3按下
#define KEY4_PRES    4    //KEY4按下
#define KEY5_PRES    5    //KEY5按下
#define KEY6_PRES    6    //KEY6按下
#define KEY7_PRES    7    //KEY7按下
#define KEY8_PRES    8    //KEY8按下
#define PAUSE_PRES   9

#define LED_ON  0
#define LED_OFF 1
#define DOWN  0 //按键按下
#define FREE  1
void AllLED_OFF(void);
void AllLED_ON(void);
void LED_Init(void);
void KEY_Init(void);
unsigned char KEY_Scan(unsigned char);
#endif
