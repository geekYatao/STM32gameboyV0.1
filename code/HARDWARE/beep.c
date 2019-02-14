#include "beep.h"
#include "stm32f10x.h"
#include "sys.h"
#include "IO.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"

u8 BGM = 0;
u8 BGM_LENGTH[6] = {0,5,4,36,12,33};
u8 BGM_change_flg = 0;
u8 BGM_volum; 

u16 CL[7]={262,294,330,349,392,440,494};
u16 CM[7]={523,587,659,698,784,880,988};
u16 CH[7]={1047,1175,1319,1397,1568,1760,1976};
u16 DL[7]={294,330,370,392,440,494,554};
u16 DM[7]={587,659,740,784,880,988,1109};
u16 DH[7]={1175,1319,1480,1568,1760,1976,2217};
u16 EL[7]={330,370,415,440,494,554,622};
u16 EM[7]={659,740,831,880,988,1109,1245};
u16 EH[7]={1319,1480,1661,1760,1976,1,1};//除数不能为0，用1代替
u16 FL[7]={349,392,440,466,523,587,659};
u16 FM[7]={698,784,880,932,1047,1175,1319};
u16 FH[7]={1397,1568,1760,1865,1,1,1};

#define TWO_TIGER_LENGTH  36
typedef struct
{
	short mName; //音名
	short mTime; //时值，全音符，二分音符，四分音符
}tNote;

/*
const tNote AllBGM[]=
{
	//两只老虎  36  BEGIN_BGM
	{CM1,TT/4},{CM2,TT/4},{CM3,TT/4},{CM1,TT/4},
	{CM1,TT/4},{CM2,TT/4},{CM3,TT/4},{CM1,TT/4},
	{CM3,TT/4},{CM4,TT/4},{CM5,TT/4},{0,TT/4},
	{CM3,TT/4},{CM4,TT/4},{CM5,TT/4},{0,TT/4},
	{CM5,TT/8},{CM6,TT/8},{CM5,TT/8},{CM4,TT/8},{CM3,TT/4},{CM1,TT/4},
	{CM5,TT/8},{CM6,TT/8},{CM5,TT/8},{CM4,TT/8},{CM3,TT/4},{CM1,TT/4},
	{CM1,TT/4},{CL5,TT/4},{CM1,TT/4},{0,TT/4},
	{CM1,TT/4},{CL5,TT/4},{CM1,TT/4},{0,TT/4},
};
*/
const tNote AllBGM[]=
{
	//击中   5    BAD_BGM
	{CM1,TT1/4},{CL4,TT1/4},{CL3,TT1/8},{CL1,TT1/8},{0,TT1/8},
	// 生成  4   GOOD_BGM
	{CH1,TT1/4},{CH2,TT1/4},{CH3,TT1/4},{0,TT1/8},
	//两只老虎  36  BEGIN_BGM
	{CM1,TT/4},{CM2,TT/4},{CM3,TT/4},{CM1,TT/4},
	{CM1,TT/4},{CM2,TT/4},{CM3,TT/4},{CM1,TT/4},
	{CM3,TT/4},{CM4,TT/4},{CM5,TT/4},{0,TT/4},
	{CM3,TT/4},{CM4,TT/4},{CM5,TT/4},{0,TT/4},
	{CM5,TT/8},{CM6,TT/8},{CM5,TT/8},{CM4,TT/8},{CM3,TT/4},{CM1,TT/4},
	{CM5,TT/8},{CM6,TT/8},{CM5,TT/8},{CM4,TT/8},{CM3,TT/4},{CM1,TT/4},
	{CM1,TT/4},{CL5,TT/4},{CM1,TT/4},{0,TT/4},
	{CM1,TT/4},{CL5,TT/4},{CM1,TT/4},{0,TT/4},
	//小猪佩奇  12   LIFE_BGM
	{CH5,TTS/4},{CH3,TTS/8},{CH1,TTS/8},{CH2,TTS/4},{CM5,TTS/4},
	{CM5,TTS/8},{CM7,TTS/8},{CH2,TTS/8},{CH4,TTS/8},{CH3,TTS/4},{CH1,TTS/4},{0,TT/4},
	//坦克大战 33   LEVEL_BGM
	{CM1,TT1/4},{CM2,TT1/4},{0,TT1/4},{CM3,TTS/4},
	{CM1,TT1/4},{CM2,TT1/4},{0,TT1/4},{CM3,TTS/4},
	{CM3,TT1/4},{CM4,TT1/4},{0,TT1/4},{CM5,TTS/4},
	{CM3,TT1/4},{CM4,TT1/4},{0,TT1/4},{CM5,TTS/4},
	{CM4,TT1/4},{CM5,TT1/4},{0,TT1/4},{CM6,TTS/4},
	{CM4,TT1/4},{CM5,TT1/4},{0,TT1/4},{CM6,TTS/4},
	{CM5,TT1/4},{CM7,TT1/4},{0,TT1/4},{CH1,TTS/4},
	{CM5,TT1/4},{CM7,TT1/4},{0,TT1/4},{CH1,TTS/4},{0,TTS/4},
};



	
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 		//LED1=!LED1;  
		}
}

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3   
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性，高低没有区别
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR4上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}

//蜂鸣器发出声音 
//usFreq即发声频率,volume_level是音量等级,1最高，到9几乎就听不到声音了。
void buzzerSound(unsigned short usFraq,unsigned char volume_level)   //usFraq是发声频率，即真实世界一个音调的频率。
{
	unsigned long Autoreload;
	if((usFraq<=122)||(usFraq>20000))
	{
		buzzerQuiet();
	}
	else
	{
		Autoreload=(8000000/usFraq)-1;  //频率变为自动重装值
		TIM_SetAutoreload(TIM3,Autoreload);
		TIM_SetCompare4(TIM3,Autoreload>>volume_level);   //音量  
	}
}
//蜂鸣器停止发声
void buzzerQuiet(void)
{
	TIM_SetCompare4(TIM3,0);
}


void TIM5_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	//定时器TIM5初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIMx中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIMx中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM5, ENABLE);  //使能TIMx					 
}
/*

//定时器5中断服务程序
void TIM5_IRQHandler(void)   
{
	static u16 i = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM5更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update );  //清除TIMx更新中断标志 
			if(i<TWO_TIGER_LENGTH)
			{
				buzzerSound(AllBGM[i].mName,BGM_volum);
				TIM_SetAutoreload(TIM5,AllBGM[i].mTime*10-1);
				TIM_SetCounter(TIM5,0);
				i++;
			}
			else
				i = 0;
		}
}
*/
//定时器5中断服务程序
void TIM5_IRQHandler(void)   //TIM3中断
{
	static u16 i = 0;
	static u8 old_BGM = 0;
	static u16 END = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update );  //清除TIMx更新中断标志 
			if(old_BGM != BGM)//改变了BGM
			{
				u16 temp;
				i = 0;
				for(temp = 0;temp<BGM;temp++)//计算音乐的开头
				{
					i += BGM_LENGTH[temp];
				}
				END = i+BGM_LENGTH[temp];//计算音乐的开头
				old_BGM = BGM;
				BGM_change_flg = 0;//此处为1可以设为单曲循环。
			}
			if(i<END)
			{
				buzzerSound(AllBGM[i].mName,BGM_volum);
				TIM_SetAutoreload(TIM5,AllBGM[i].mTime*10-1);
				TIM_SetCounter(TIM5,0);
				i++;
			}
			else
			{
				buzzerSound(0,BGM_volum);//停止。
				if(BGM_change_flg)
				{
					BGM_change_flg = 0;
					old_BGM = 0;
				}
			}
		}
}
/*
void musicPlay(int length,unsigned char volume_level)
{
	u8 i=0;
	while(i<length)
	{
		buzzerSound(AllBGM[i].mName,volume_level);
		delay_ms(AllBGM[i].mTime);
		i++;
	}
}
*/
