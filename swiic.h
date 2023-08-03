#ifndef __SWIIC_H_
#define __SWIIC_H_

#include "gpio.h"  	//GPIO 初始化
#include "ddl.h"		//延迟函数

#define OUTTIME 500   //ACK超时时间
#define IIC_Delay() 	delay10us(1)     //设定电平保持时间，经过测量，在主频72M时3us IIC时钟速度为100K

#define __SDA_1 REG_WRITE(IIC->SDA.Port->BSRR, IIC->SDA.Pin)    //SDA置高
#define __SDA_0 REG_WRITE(IIC->SDA.Port->BRR, IIC->SDA.Pin)  		//SDA置低

#define __SCL_1 REG_WRITE(IIC->SCL.Port->BSRR, IIC->SCL.Pin)  	//SCL置高
#define __SCL_0 REG_WRITE(IIC->SCL.Port->BRR, IIC->SCL.Pin)   	//SCL置低

#define	__SDA_READ REG_READBITS(IIC->SDA.Port->IN, IIC->SDA.Pin)    //读取SDA

#define 	SDA_IN()  REG_SETBITS(IIC->SDA.Port->DIR, IIC->SDA.Pin) 	//更改SDA模式为输入
#define 	SDA_OUT() REG_CLEARBITS(IIC->SDA.Port->DIR, IIC->SDA.Pin) //更改SDA模式为输出

#define 	IIC_MEMADD_SIZE_8BIT    			8       //寄存器地址为8位地址  
#define		IIC_MEMADD_SIZE_16BIT   			16      //寄存器地址为16位地址

typedef struct{
	uint32_t AHB;
	GPIO_TypeDef *Port;
	uint16_t Pin;
}IIC_GPIO;

typedef struct{
	IIC_GPIO SCL;
	IIC_GPIO SDA;
	uint8_t status;
	uint8_t addr;
}IIC_InitTypeDef;

extern IIC_GPIO IIC_SCL;
extern IIC_GPIO IIC_SDA;
extern IIC_InitTypeDef IIC_ONE;

void IIC_Start(IIC_InitTypeDef* IIC);
void IIC_Stop(IIC_InitTypeDef* IIC);
uint8_t IIC_WaitAck(IIC_InitTypeDef* IIC);
void IIC_Ack(IIC_InitTypeDef* IIC);
void IIC_NAck(IIC_InitTypeDef* IIC);
void IIC_SendByte(IIC_InitTypeDef* IIC,uint8_t _ucByte);
uint8_t IIC_ReadByte(IIC_InitTypeDef* IIC,uint8_t *read);

void IIC_Init(IIC_InitTypeDef* IIC);

uint8_t IIC_Read_Data(IIC_InitTypeDef* IIC,uint8_t *pData,uint16_t Len);
uint8_t IIC_Read(IIC_InitTypeDef* IIC,uint8_t *pData);
uint8_t IIC_Write(IIC_InitTypeDef* IIC,uint8_t *pData);
uint8_t IIC_Mem_Read(IIC_InitTypeDef* IIC,uint16_t MemAddr,uint8_t MemAddSize,uint16_t Size,uint8_t *pData);
uint8_t IIC_Mem_Write(IIC_InitTypeDef* IIC,uint16_t MemAddr,uint8_t MemAddSize,uint16_t Size,uint8_t *pData);

//示例复用IIC-Pin
#define    	IIC_SDA_AHB    SYSCTRL_PERICLK_PA
#define    	IIC_SDA_Port   GPIOA
#define    	IIC_SDA_Pin    GPIO_PIN_01
	
#define    	IIC_SCL_AHB    SYSCTRL_PERICLK_PA
#define    	IIC_SCL_Port   GPIOA
#define    	IIC_SCL_Pin    GPIO_PIN_02
void IIC_ONE_Init(void);
//示例复用IIC-Pin

#endif //__SWIIC_H_