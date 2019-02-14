#include "stm32f10x.h"
#include "IO.h"
#include "sys.h"
#include "delay.h"
#include "OLED.h"
#include "IIC.h"
#include "timer.h"
#include "stdlib.h"
#include "beep.h"
#include "time.h"

//函数声明
u8 random_num(void);
void CreatMouse(u8 mouse);
void add_life(void);
void level_up(void);
void FirstScreen(void);
void show_opt(void);
void add_score(void);
void sub_life(void);
void set_default(void);
void set_BGM(u8 volum,u8 bgm_temp);


//默认参数
#define LIFE_NUM  3          // 默认几条命
#define LEVEL_UP_CNT  10    //击中x个地鼠难度增加

//不同难度对应的时间，（数值+1）/10 = 时间ms
u16 level1_time_arr[15] = {19999,14999,11999,9999,7999,5999,4999,3999,2999,1999,1799,1599,1399,1199,999};

//全局变量定义
u8 life = LIFE_NUM;    //生命
u32 score = 0;          //得分记录
u8 level = 1;           //当前难度，数字越大难度越高
u8 next_flg = 1;    //生成下一个地鼠标记
u8 timeout_flg =0;    //在规定时间内没有打中地鼠标记
u8 mouse = 0;          //生成的地鼠位置
u8 key = 0;             //按键记录
u32 time_us = 0;         //随机生成地鼠，用到的随机数种子
u8 add_life_cnt = 1;       //用于增加生命计数，每到100的整数倍以后加命
u8 level_cnt = 0;         //击中的地鼠数量，用于升级计数

int main(void)
{
	set_BGM(6,BEGIN_BGM);  //上电音乐与BGM 音量 1最大，10听不到
	TIM5_Int_Init(9,7199);   //上电先播放背景音乐
	TIM3_PWM_Init(0xfffe,8); //蜂鸣器频率定时器初始化
	LED_Init();
	KEY_Init();
	delay_init();
	initIIC();
	initOLED();
	set_default();
	TIM4_Seed_Init();
	FirstScreen();      //显示完屏幕内容以后，再开启打地鼠计时用的定时器
	show_opt();
	
	TIM2_Int_Init(level1_time_arr[level_cnt],7199);
	
	while(1)
	{
		if(life)//还有命
		{
			if(next_flg)  //需要生成下一个地鼠
			{
				next_flg = 0;
				mouse = random_num();       //产生随机数		
				CreatMouse(mouse);           //随机生成地鼠
				TIM_SetCounter(TIM2, 0);//定时器清零
				timeout_flg = 0;
			}
			if(timeout_flg)  //超时，减命，生成下一个地鼠
			{
				timeout_flg = 0;
				next_flg = 1;
				sub_life();
			}
			key = KEY_Scan(0);
			if(key)       //如果按下按键
			{
				next_flg = 1;   //不论打的对不对，都要生产下一个地鼠
				TIM_SetCounter(TIM2, 0);//不论打中的地鼠对不对，定时器都清零
				if(key == mouse)//正确打中地鼠   加分，生成下一个地鼠
				{
					add_score();
					if(score/100 == add_life_cnt)    //一定积分以后加命
					{
						add_life();
					}
					else if(++level_cnt > LEVEL_UP_CNT) //升级与加命不同时处理
					{
						level_up();
					}		
				}	
				else //打错   减命
				{
					sub_life();
				}
				
			}
		}
		else//没命了
		{
			AllLED_OFF();   //关掉所有的灯
			BGM = BEGIN_BGM;
			if(PAUSE_PRES == KEY_Scan(0))//重生
			{
				BGM = LIFE_BGM;
				set_default();
				show_opt();
				TIM_SetAutoreload(TIM2,level1_time_arr[level]);
				TIM_SetCounter(TIM2, 0);//定时器清零
			}		
		}	

	}
}


		
		
//按照参数，点亮某个LED
void CreatMouse(u8 mouse)
{
	AllLED_OFF();  //先把灯全都关掉
	delay_ms(50);  //然后稍微延时，避免地鼠刷新到同一个位置时，看不出来
	switch(mouse)
	{
		case 1: SLED1 = 0;break;
		case 2: SLED2 = 0;break;
		case 3: SLED3 = 0;break;
		case 4: SLED4 = 0;break;
		case 5: SLED5 = 0;break;
		case 6: SLED6 = 0;break;
		case 7: SLED7 = 0;break;
		case 8: SLED8 = 0;break;
		default: break;
	}
}

//使用定时器的计数值生成随机数
u8 random_num(void)
{
	u8 i = 0;    //从随机数到地鼠用的临时变量
	srand(time_us);
	i = rand()%8 +1;
	return i;
}

//加命并显示
void add_life(void)
{
	BGM = LIFE_BGM;
	BGM_change_flg = 1;//修改BGM
	add_life_cnt++;
	life++;
	showNumber(56,2,life,DEC,8,FONT_16_EN);
}
//难度提升并显示
void level_up(void)
{
	BGM = LEVEL_BGM;
	BGM_change_flg = 1;//修改BGM
	level_cnt = 0;
	level++;
	if(level>14)//默认14关
		level = 14;
	TIM_SetAutoreload(TIM2,level1_time_arr[level]);
	showNumber(56,4,level,DEC,8,FONT_16_EN);
}
//按照level加分并显示
void add_score(void)
{
	BGM = GOOD_BGM;
	BGM_change_flg = 1;//修改BGM
	score += level;
	showNumber(56,6,score,DEC,8,FONT_16_EN);
}
//减命并显示
void sub_life(void)
{
	BGM = BAD_BGM;
	BGM_change_flg = 1;//修改BGM
	life--;
	showNumber(56,2,life,DEC,8,FONT_16_EN);			
}
//设置固定显示的内容
void FirstScreen(void)
{
	//显示大LOGO
	formatScreen(0x00);
	showImage(0,0,128,8,Y_LOGO_ENUM);
	delay_ms(1000);
	
	//显示汉字
	formatScreen(0x00);
//	showString(0,0,"yoodao",FONT_16_EN);
	showCNString(0,0,"小极客打地鼠掌机",FONT_16_CN);
	
	//显示生命、难度与分数
	showString(0,2,"life:",FONT_16_EN);
	showString(0,4,"level:",FONT_16_EN);
	showString(0,6,"score:",FONT_16_EN);
}
//设置屏幕上的参数
void show_opt(void)
{
	showNumber(56,2,life,DEC,8,FONT_16_EN);
	showNumber(56,4,level,DEC,8,FONT_16_EN);
	showNumber(56,6,score,DEC,8,FONT_16_EN);
}

//设置默认参数
void set_default(void)
{
	life = LIFE_NUM;
	mouse = 0;
	score = 0;
	level = 1;
	next_flg = 1;
	timeout_flg = 0;
	level_cnt = 0;         
	add_life_cnt = 1;     
}
//设置音量与背景
void set_BGM(u8 volum,u8 bgm_temp)
{
	BGM_volum = volum;
	BGM = bgm_temp;
}

