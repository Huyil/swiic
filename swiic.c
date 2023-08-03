#include "swiic.h"
/**
@brief 启动I2C总线通信，发送启动信号
@param IIC I2C初始化结构体指针
@return 无
*/
void IIC_Start(IIC_InitTypeDef* IIC)
{
    SDA_OUT();
    __SDA_1;             // 将SDA线置高，配置GPIO管脚输出高电平
    __SCL_1;
    IIC_Delay();         // 保证SDA线在高电平一段时间，在SDA=0之前延时
    __SDA_0;
    IIC_Delay();
    __SCL_0;
    IIC_Delay();
}

/**
@brief 停止I2C总线通信，发送停止信号
@param IIC I2C初始化结构体指针
@return 无
*/
void IIC_Stop(IIC_InitTypeDef* IIC)
{
    SDA_OUT();
    __SCL_0;
    __SDA_0;             // 将SDA线置低，配置GPIO管脚输出低电平
    IIC_Delay();
    __SCL_1;
    IIC_Delay();         // 保证SDA线在高电平一段时间，在SDA=0之前延时
    __SDA_1;
    IIC_Delay();
}

/**
@brief 等待应答信号，产生一个时钟并读取器件的ACK信号
@param IIC I2C初始化结构体指针
@return 返回0表示正确应答，返回1表示无应答
*/
uint8_t IIC_WaitAck(IIC_InitTypeDef* IIC)
{
    uint16_t timeout = 0;
    __SDA_1;             // CPU释放总线
    SDA_IN();            // 将SDA设置为输入
    IIC_Delay();
    __SCL_1;             // CPU驱动SCL=1，此时器件会返回ACK信号
    IIC_Delay();
    while (__SDA_READ)
    {
        timeout++;
        if (timeout > OUTTIME)
        {
            __SCL_0;
            IIC_Delay();
            SDA_OUT();
            return 1;
        }
    }
    __SCL_0;
    IIC_Delay();
    SDA_OUT();
    return 0;
}

/**
@brief 发送应答信号，产生一个时钟并将SDA线置低
@param IIC I2C初始化结构体指针
@return 无
*/
void IIC_Ack(IIC_InitTypeDef* IIC)
{
    __SDA_0;             // CPU驱动SDA=0
    IIC_Delay();
    __SCL_1;             // 产生一个高电平时钟
    IIC_Delay();
    __SCL_0;
    IIC_Delay();
    __SDA_1;             // CPU释放总线
    IIC_Delay();
}

/**
@brief 发送非应答信号，产生一个时钟并将SDA线置高
@param IIC I2C初始化结构体指针
@return 无
*/
void IIC_NAck(IIC_InitTypeDef* IIC)
{
    __SDA_1;             // CPU驱动SDA=1
    IIC_Delay();
    __SCL_1;             // 产生一个高电平时钟
    IIC_Delay();
    __SCL_0;
    IIC_Delay();
}

/**
 @brief CPU向I2C总线设备发送8bit数据
 @param IIC 句柄
 @param ucByte -[in] 等待发送的字节
 @return 无
*/
void IIC_SendByte(IIC_InitTypeDef* IIC,uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			__SDA_1;
		}
		else
		{
			__SDA_0;
		}
		IIC_Delay();
		__SCL_1;
		IIC_Delay();	
		__SCL_0;

		_ucByte <<= 1;	/* 左移一个bit */
	}
}

/**
 @brief CPU从I2C总线设备读取8bit数据
 @param IIC IIC_InitTypeDef句柄
 @return 读到的数据
*/
uint8_t IIC_ReadByte(IIC_InitTypeDef* IIC,uint8_t *read)
{
	uint8_t i;
	uint8_t value;
  SDA_IN();
	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		__SCL_1;
		IIC_Delay();
		if (__SDA_READ)
		{
			value++;

		}
		__SCL_0;
		IIC_Delay();
	}
	SDA_OUT();
	*read = value;
	return value;
}
/**
@brief 向I2C设备写入数据
@param IIC I2C初始化结构体指针
@param pData 要写入的数据指针
@return 返回0表示成功，返回1表示失败
*/
uint8_t IIC_Write(IIC_InitTypeDef* IIC, uint8_t *pData)
{
    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr);
    IIC->status |= IIC_WaitAck(IIC);

    IIC_SendByte(IIC, *pData);
    IIC->status |= IIC_WaitAck(IIC);

    IIC_Stop(IIC);
    if (IIC->status) 
        return (1);
    else
        return (0);
}

/**
@brief 从I2C设备读取数据
@param IIC I2C初始化结构体指针
@param pData 读取数据存储的指针
@return 返回0表示成功，返回1表示失败
*/
uint8_t IIC_Read(IIC_InitTypeDef* IIC, uint8_t *pData)
{
    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr | 0x01);
    IIC->status |= IIC_WaitAck(IIC);

    IIC_ReadByte(IIC, pData);
    IIC_NAck(IIC);

    IIC_Stop(IIC);
    if (IIC->status) 
        return (1);
    else
        return (0);
}

/**
@brief 向I2C设备的内存地址写入数据
@param IIC I2C初始化结构体指针
@param MemAddr 内存地址
@param MemAddSize 地址大小
@param Size 数据长度
@param pData 要写入的数据指针
@return 返回0表示成功，返回1表示失败
*/
uint8_t IIC_Mem_Write(IIC_InitTypeDef* IIC, uint16_t MemAddr, uint8_t MemAddSize, uint16_t Size, uint8_t *pData)
{
    uint16_t i;
    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr);
    IIC->status = IIC_WaitAck(IIC);

    if (MemAddSize == IIC_MEMADD_SIZE_16BIT)
    {
        IIC_SendByte(IIC, (uint8_t)(MemAddr >> 8));
        IIC->status |= IIC_WaitAck(IIC);
    }
    IIC_SendByte(IIC, (uint8_t)(MemAddr & 0xFF));
    IIC->status |= IIC_WaitAck(IIC);

    for (i = 0; i < Size; i++) {
        IIC_SendByte(IIC, *(pData + i));
        IIC->status |= IIC_WaitAck(IIC);
    }

    IIC_Stop(IIC);
    if (IIC->status) 
        return (1);
    else
        return (0);
}

/**
@brief 从I2C设备读取数据到指定地址
@param IIC I2C初始化结构体指针
@param pData 读取数据存储的指针
@param Len 数据长度
@return 返回0表示成功，返回1表示失败
*/
uint8_t IIC_Read_Data(IIC_InitTypeDef* IIC, uint8_t *pData, uint16_t Len)
{
    uint16_t i;
    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr | 0x01);
    IIC->status |= IIC_WaitAck(IIC);

    for (i = 0; i < Len; i++) {
        IIC_ReadByte(IIC, pData + i);
        if (i == Len - 1) {
            IIC_NAck(IIC);
        }
        else {
            IIC_Ack(IIC);
        }
    }

    IIC_Stop(IIC);
    if (IIC->status) 
        return (1);
    else
        return (0);
}

/**
@brief 从I2C设备的内存地址读取数据
@param IIC I2C初始化结构体指针
@param MemAddr 内存地址
@param MemAddSize 地址大小
@param Size 数据长度
@param pData 读取数据存储的指针
@return 返回0表示成功，返回1表示失败
*/
uint8_t IIC_Mem_Read(IIC_InitTypeDef* IIC, uint16_t MemAddr, uint8_t MemAddSize, uint16_t Size, uint8_t *pData)
{
    uint16_t i;
    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr);
    IIC->status = IIC_WaitAck(IIC);

    if (MemAddSize == IIC_MEMADD_SIZE_16BIT)
    {
        IIC_SendByte(IIC, (uint8_t)(MemAddr >> 8));
        IIC->status |= IIC_WaitAck(IIC);
    }
    IIC_SendByte(IIC, (uint8_t)(MemAddr & 0xFF));
    IIC->status |= IIC_WaitAck(IIC);

    IIC_Start(IIC);
    IIC_SendByte(IIC, IIC->addr | 0X01);
    IIC->status = IIC_WaitAck(IIC);

    for (i = 0; i < Size; i++) {
        IIC_ReadByte(IIC, pData + i);
        if (i == Size - 1) {
            IIC_NAck(IIC);
        }
        else {
            IIC_Ack(IIC);
        }
    }
    IIC_Stop(IIC);

    if (IIC->status) 
        return (1);
    else
        return (0);
}

/**
@brief 初始化I2C接口
@param IIC I2C初始化结构体指针
@return 无
*/
void IIC_Init(IIC_InitTypeDef* IIC)
{
    SYSCTRL_PeriphClkEnable(IIC->SCL.AHB);
    SYSCTRL_PeriphClkEnable(IIC->SDA.AHB);

    stc_gpio_init_t stcGpioInit;
    ///< SDA初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_OD;
    stcGpioInit.u32Pin = IIC->SDA.Pin;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIO_Init(IIC->SDA.Port, &stcGpioInit);

    ///< SCL初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_OD;
    stcGpioInit.u32Pin = IIC->SCL.Pin;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIO_Init(IIC->SCL.Port, &stcGpioInit);

    IIC_Stop(IIC);
}


//支持I2C复用,只需切换从机地址ADDR
IIC_InitTypeDef IIC_ONE;

void IIC_ONE_Init(void){
	IIC_GPIO IIC_SCL = {SYSCTRL_PERICLK_PA,GPIOA,GPIO_PIN_01};
	IIC_GPIO IIC_SDA = {SYSCTRL_PERICLK_PA,GPIOA,GPIO_PIN_02};

	IIC_ONE.SCL = IIC_SCL;
	IIC_ONE.SDA = IIC_SDA;
	//IIC_IIC.addr = 0xAA;
	
	IIC_Init(&IIC_ONE);
}
