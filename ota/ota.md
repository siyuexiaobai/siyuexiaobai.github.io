# OTA学习笔记 #

## 前言

此笔记为OTA的学习笔记，学习参考bilibili的up主尚硅谷的[零基础OTA-BootLoader教程， ota远程升级，BootLoader程序设计bilibili](https://www.bilibili.com/video/BV11C65BDEJx/?vd_source=d4f13896e48b8d93ca6850bd2ae23031)视频，不赘述理论知识，只记录本人学习OTA的过程，及遇到的难题解决方式

## 准备

### 硬件

stm32f103开发板带spi-flash

st-link烧录器/j-link烧录器

串口转ttl调试器

### 软件

keil

STM32CubeMX

VsCode

STM32 ST-LINK Utility

sscom

## 项目规划

需要创建的项目

1.B区代码项目：[BootLoader](./BootLoader/)——用于程序跳转，固件升级

2.A区代码项目：[Application](./Application/)——正常运行的程序

## 分区说明

App——运行正常程序的分区

Boot——运行BootLoader的分区

download——存放下载固件的分区

factory——恢复出厂设置的分区（无）

AB分区指的是Application分区与BootLoad分区，而不是运行正常程序的分区分成A、B区做回滚

## OTA升级

### stm32f103zet6 + eeprom + spi-flash

使用eeprom存储是否需要升级的标志位，A区代码检测到需要升级请求之后会将固件下载到spi-flash，然后进行固件校验，当校验通过后将eeprom存储的升级标志位设置成1，然后进行重启，重启后首先执行b区代码，先检测eeprom的升级标注位，为1则将spi-flash中的固件写入到0x08004000地址，然后跳转到0x08004000执行新固件

### 创建项目

1.创建初始化项目，使用串口1进行日志打印

![配置RCC与SYS](./images/1.1配置RCC_SYS.png)

![1.2配置串口](images/1.2配置串口.png)

![1.3配置时钟树](./images/1.3配置时钟树)

项目初期要使用串口接收并保存固件，波特率不能设置太快，先选中9600bps如果稳定后再考虑115200bps。

2.重写fputc函数实现日志打印

![1.4引入标准io头文件](./images/1.4引入标准io头文件)

重写fputc函数要用到`FILE`结构体所以需要引入标准io头文件，因为main.h是hal库生成的驱动层代码都会引入的所以将操作写在main.h头文件中

![1.5从写fputc头文件](ota.assets/1.5从写fputc头文件.png)

![1.6勾选微库](./images/1.6勾选微库)

如果要使用串口打印必须勾选`Use MicroLIB`

![1.7测试.png](./images/1.7测试.png)
