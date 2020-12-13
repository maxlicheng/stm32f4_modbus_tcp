## 项目简介
- 基于 **正点原子STM32F4探索者开发板** 和 **LabVIEW开发的上位机软件**，两者进行Modbus协议通信的开源项目;
- 项目采用TCP/IP通信方式，移植freeModbus协议到STM32，利用LabVIEW Modbus库;
- 编写配套上位机读取开发板按键、AD值，并可通过上位机设置开发板LED、DAC状态。

## 功能简介
- 线 圈     ：led控制
- 离散输入  ：按键读取
- 保持寄存器：ADC读取
- 输入寄存器：DAC设置

## 上位机
- 1.上位机由LabVIEW 2013编写，请使用高于2013的版本打开，并将[NI Modbus库](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases/download/v0.1.0/nimodbus121.zip)解压到指定路径，详见[Releases](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases);
- 2.[ModbusPoll](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases/download/v0.1.0/ModbusPoll.rar)调试工具见[Releases](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases)；

## 下位机
- 1.下位机采用正点原子探索者stm32f4开发板实现；

## 注意事项
- 1.开发板与运行上位机的PC需要连入同一局域网；
- 2.编译代码前需要修改静态IP参数，需与您的局域网网段一致；
- 3.开发板需要插入LCD显示屏；
- 4.PC需要安装高于LabVIEW2013的开发环境，并将[NI Modbus库](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases/download/v0.1.0/nimodbus121.zip)解压到对应文件夹；
- 5.[NI Modbus库](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases/download/v0.1.0/nimodbus121.zip)请从[Github Releases]((https://github.com/maxlicheng/stm32f4_modbus_tcp/releases))页面下载，视频[《LabVIEW上位机Modbus程序讲解》](https://www.bilibili.com/video/av60889214/?p=3)5分钟处有详细说明NI Modbus库的下载及使用方法；
- 6.运行LabVIEW上位机前，填入开发板的静态IP；
- 7.不熟悉LabVIEW的同学可以使用ModbusPoll调试工具进行学习。

## 效果图
![Image text](https://raw.githubusercontent.com/maxlicheng/stm32f4_modbus_tcp_prj/master/views/LabVIEW.jpg)

## 视频教程
- 视频1：[《STM32与LabVIEW上下位机Modbus TCP/IP通信功能演示》](https://www.bilibili.com/video/av60889214/)
- 视频2：[《Modbus简单介绍及STM32源码分析》](https://www.bilibili.com/video/av60889214/?p=2)
- 视频3：[《LabVIEW上位机Modbus程序讲解》](https://www.bilibili.com/video/av60889214/?p=3)
- 视频4：[《ModbusPoll调试工具演示》](https://www.bilibili.com/video/av60889214/?p=4)

## 结束语
- LabVIEW上位机务必安装Release页面的[NI Modbus库](https://github.com/maxlicheng/stm32f4_modbus_tcp/releases/download/v0.1.0/nimodbus121.zip)，否则运行上位机会提示找不到NI Mobdus库。视频[《LabVIEW上位机Modbus程序讲解》](https://www.bilibili.com/video/av60889214/?p=3)5分钟处有详细说明NI Modbus库的下载及安装；

