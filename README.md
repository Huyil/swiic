# swiic
I2C-Software-Driver for ARM

### 仓库简介

这个仓库包含了一个简单的 I2C 软件驱动，用于控制 I2C 设备的通信。
这个驱动适用于[华大(HC32)](https://www.xhsc.com.cn/)嵌入式系统。
通过修改初始化函数以及IO定义也可用于其他MCU。

### 功能特点

- 基于软件的 I2C 通信
- 支持启动、停止、写入和读取操作
- 灵活易用的接口函数
- 适用于多种嵌入式平台

## 用例

### 安装和配置

1. 克隆本仓库到您的本地目录：

   ```shell
   https://github.com/Huyil/swiic.git
   ```

2. 在您的项目中包含 swiic.c swiic.h文件。

### 使用示例

#### 初始化 I2C 总线

// 引入头文件
#include "swiic.h"

// 定义 I2C 总线引脚
```c

//创建I2C结构体
IIC_InitTypeDef IIC_ONE;
//创建I2C GPIO结构体
IIC_GPIO IIC_SDA = {SYSCTRL_PERICLK_PA, GPIOA, GPIO_PIN_02};
IIC_GPIO IIC_SCL = {
 SYSCTRL_PERICLK_PA, //GPIO时钟
 GPIOA,              //GPIO PORT
 GPIO_PIN_01         //GPIO Pin
};

void IIC_ONE_Init(void){
  //补充IIC_ONE的SDA SCL及从机地址
  IIC_ONE.SCL = IIC_SCL;
  IIC_ONE.SDA = IIC_SDA;
  IIC_ONE.addr = 0x70; //从机地址0111 000X
  //支持I2C复用,只需切换从机地址ADDR

  //初始化GPIO
  IIC_Init(&IIC_ONE);
}
```

#### 向设备指定内存地址写入数据

```c
uint8_t date[] = {0x12, 0x34, 0x56}; //准备需要写入的数据数组
uint8_t dataLen = 3; //数据长度
uint16_t MenADDR     //需要开始写入的从机内存地址
IIC_Mem_Write(&IIC_ONE, MenADDR, IIC_MEMADD_SIZE_8BIT, dataLen, date);
```

#### 从设备指定内存地址读取数据

```c
uint8_t readData[10]; //用于保存读取数据的数组
uint8_t dataLen = 10; //数据长度
uint16_t MenADDR      //需要开始读取的从机内存地址
IIC_Mem_Read(&IIC_ONE, MenADDR, IIC_MEMADD_SIZE_8BIT, dataLen, readData);
```

### 贡献
此 Readme.me 由 ChatGPT 提供支持。
如果您发现 Bug 或者有任何改进意见，欢迎提交 Issue 或 Pull Request。

## 许可证

遵循 MIT 许可。
