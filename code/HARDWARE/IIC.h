#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"

/**************************以下宏定义用于移植******************************/
#define IIC_SPEED 1											 //IIC通信延时时间，单位us

#define IIC_SDA_RCC RCC_APB2Periph_GPIOA                    //SDA时钟
#define IIC_SCL_RCC RCC_APB2Periph_GPIOA                     //SCL时钟

#define IIC_SDA_PORT GPIOA                                  //SDA引脚所在GPIO组
#define IIC_SCL_PORT GPIOA                                  //SCL引脚所在GPIO组

#define IIC_SDA_PIN GPIO_Pin_6                             //SDA引脚
#define IIC_SCL_PIN GPIO_Pin_5                              //SCL引脚

#define IIC_SDA_IN() setSDA_IN()                             //设置SDA为输入 
#define IIC_SDA_OUT() setSDA_OUT()                           //设置SDA为输出

#define IIC_SDA_H() GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)   //拉高SDA
#define IIC_SDA_L() GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN) //拉低SDA

#define IIC_SCL_H() GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)   //拉高SCL
#define IIC_SCL_L() GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN) //拉低SCL

/********************************~END~***********************************/

//IIC返回的两种状态
typedef enum
{
    NACK = 0x00,    //无响应
    ACK  = 0x01     //有响应
}IIC_ACK;

void setSDA_OUT(void);
void setSDA_IN(void);

void initIIC(void);
void startIIC(void);
void stopIIC(void);
void sendIICByte(u8 byte);
u8  receiveIICByte(void);
void sendIICAck(void);
void sendIICNAck(void);
IIC_ACK waitAck(void);

#endif
