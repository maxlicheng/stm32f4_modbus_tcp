/**********************************************************************************************************
** 文件名		:user_mb_app.h
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-02-18
** 描述			:modbus线圈及寄存器配置
************************************************************************************************************/
#ifndef     USER_APP
#define     USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
//#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
//************************************************************************//
//**02功能码 
//**02 Read Discrete Inputs (1x)
//************************************************************************//
#define S_DISCRETE_INPUT_START        10000     //开关寄存器起始地址
#define S_DISCRETE_INPUT_NDISCRETES   16        //开关寄存器数量

//************************************************************************//
//**01 05 15功能码
//**01 Read Coils（0x）  
//**05 Write Single Coil
//**15 Write Multiple Coils
//************************************************************************//
#define S_COIL_START                  0     //线圈起始地址 
#define S_COIL_NCOILS                 64    //线圈数量

//************************************************************************//
//**04功能码 (3x)
//**04 Read Input Registers
//************************************************************************//
#define S_REG_INPUT_START             30000     //输入寄存器起始地址
#define S_REG_INPUT_NREGS             100       //输入寄存器数量

//************************************************************************//
//**03 06 16功能码 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//************************************************************************//
#define S_REG_HOLDING_START           40000     //保持寄存器起始地址
#define S_REG_HOLDING_NREGS           100       //保持寄存器数量

/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0
#define          S_HD_CPU_USAGE_MAJOR             1
#define          S_HD_CPU_USAGE_MINOR             2
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0

///* -----------------------Master Defines -------------------------------------*/
//#define M_DISCRETE_INPUT_START        0
//#define M_DISCRETE_INPUT_NDISCRETES   16
//#define M_COIL_START                  0
//#define M_COIL_NCOILS                 64
//#define M_REG_INPUT_START             0
//#define M_REG_INPUT_NREGS             100
//#define M_REG_HOLDING_START           0
//#define M_REG_HOLDING_NREGS           100
///* master mode: holding register's all address */
//#define          M_HD_RESERVE                     0
///* master mode: input register's all address */
//#define          M_IN_RESERVE                     0
///* master mode: coil's all address */
//#define          M_CO_RESERVE                     0
///* master mode: discrete's all address */
//#define          M_DI_RESERVE                     0

#endif
