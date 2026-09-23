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







