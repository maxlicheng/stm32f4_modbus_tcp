# 说明
 基于stm32f4的modbus tcp上下位机通信项目

## 功能介绍
- 移植freemodbus到stm32，实现上下位机modbus通信
- 1.线圈：led控制
- 2.离散量：按键值读取
- 3.保持寄存器：ADC
- 4.输入寄存器：DAC
- 5.保留，待补充

## 上位机
- 1.上位机由LabVIEW 2013编写，请使用高于2013的版本打开，并将ni modbus 库解压到指定路径，详见release;
- 2.ModbusPoll调试工具见release；

## 下位机
- 1.下位机采用正点原子探索者stm32f4开发板实现；
- 2.保留，待补充

## 效果图
![Image text](https://raw.githubusercontent.com/maxlicheng/stm32f4_modbus_tcp_prj/master/views/LabVIEW.jpg)

## 视频教程
- 1.[《STM32与LabVIEW上下位机Modbus TCP/IP通信功能演示》](https://www.bilibili.com/video/av60889214/)
- 2.[《Modbus简单介绍及STM32源码分析》](https://www.bilibili.com/video/av60889214/?p=2)
- 3.[《LabVIEW上位机Modbus程序讲解》](https://www.bilibili.com/video/av60889214/?p=3)
- 4.[《ModbusPoll调试工具演示》](https://www.bilibili.com/video/av60889214/?p=4)

## 其他
更详细的使用方式，请移步个人博客：www.maxlicheng.com

