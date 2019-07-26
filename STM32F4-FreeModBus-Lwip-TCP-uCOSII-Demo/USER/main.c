/**********************************************************************************************************
** 文件名		:main.c
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-02-18
** 描述			:stm32f4 modbus 主程序
************************************************************************************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "timer.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"
#include "mb.h"
#include "lsens.h"
#include "adc.h"
#include "dac.h"
#include "beep.h"

//eMBPoll任务
#define eMBPoll_TASK_PRIO 		7				//设置任务优先级 
#define eMBPoll_STK_SIZE		128				//任务堆栈大小
OS_STK eMBPoll_TASK_STK[eMBPoll_STK_SIZE]; 		//任务堆栈
void eMBPoll_task(void *pdata);  				//任务函数

//KEY_Poll任务
#define KEY_POLL_TASK_PRIO       8   			//设置任务优先级 
#define KEY_POLL_STK_SIZE  		 64  			//设置任务堆栈大小
OS_STK  KEY_POLL_TASK_STK[KEY_POLL_STK_SIZE];   //任务堆栈	
void    key_poll_task(void *pdata);             //任务函数    

//LED_Poll任务
#define LED_POLL_TASK_PRIO       9   			//设置任务优先级 
#define LED_POLL_STK_SIZE  		 64  			//设置任务堆栈大小
OS_STK  LED_POLL_TASK_STK[LED_POLL_STK_SIZE];   //任务堆栈	
void    led_poll_task(void *pdata);             //任务函数


//在LCD上显示地址信息任务
#define DISPLAY_TASK_PRIO	10					//任务优先级
#define DISPLAY_STK_SIZE	128					//任务堆栈大小
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];		//任务堆栈
void display_task(void *pdata);					//任务函数

//START任务
#define START_TASK_PRIO		11					//任务优先级
#define START_STK_SIZE		128					//任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];			//任务堆栈
void start_task(void *pdata); 					//任务函数

//ADC_Poll任务
#define ADC_POLL_TASK_PRIO       12   			//设置任务优先级 
#define ADC_POLL_STK_SIZE  		 64  			//设置任务堆栈大小
OS_STK  ADC_POLL_TASK_STK[ADC_POLL_STK_SIZE];   //任务堆栈	
void    adc_poll_task(void *pdata);             //任务函数

//DAC_Poll任务
#define DAC_POLL_TASK_PRIO       13   			//设置任务优先级 
#define DAC_POLL_STK_SIZE  		 64  			//设置任务堆栈大小
OS_STK  DAC_POLL_TASK_STK[DAC_POLL_STK_SIZE];   //任务堆栈	
void    dac_poll_task(void *pdata);             //任务函数

//在LCD上显示地址信息
//mode:1 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	POINT_COLOR = RED; 		//红色字体
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
//		LCD_ShowString(30,230,210,16,16,"Port:8088!"); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);		//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
//		LCD_ShowString(30,230,210,16,16,"Port:8088!"); 
	}	
}

int main(void)
{
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	uart_init(115200);   	//串口波特率设置
//	usmart_dev.init(84);	//初始化USMART
	LED_Init(); 			//LED初始化
	KEY_Init();  			//按键初始化
	LCD_Init();  			//LCD初始化
	Adc_Init();
	Lsens_Init();
	Dac1_Init();
	BEEP_Init();
	FSMC_SRAM_Init(); 		//SRAM初始化
	
	mymem_init(SRAMIN);  	//初始化内部内存池
	mymem_init(SRAMEX);  	//初始化外部内存池
	mymem_init(SRAMCCM); 	//初始化CCM内存池
	
	POINT_COLOR = RED; 		//红色字体
	LCD_ShowString(30,10,200,20,16,"Explorer STM32F4");
	LCD_ShowString(30,30,200,20,16,"FreeModBus-1.5 Test");
	LCD_ShowString(30,50,200,20,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,70,200,20,16,"2018/2/22");
	POINT_COLOR = BLUE; //蓝色字体

	OSInit(); 					//UCOS初始化
	while(lwip_comm_init()) 	//lwip初始化
	{
		LCD_ShowString(30,90,200,20,16,"Lwip Init failed!"); 	//lwip初始化失败
		delay_ms(500);
		LCD_Fill(30,90,230,150,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,90,200,20,16,"Lwip Init Success!"); 		//lwip初始化成功
//	while(tcp_server_init()) 									//初始化tcp_server(创建tcp_server线程)
//	{
//		LCD_ShowString(30,150,200,20,16,"TCP Server failed!!"); //tcp服务器创建失败
//		delay_ms(500);
//		LCD_Fill(30,150,230,170,WHITE);
//		delay_ms(500);
//	}
//	LCD_ShowString(30,150,200,20,16,"TCP Server Success!"); 	//tcp服务器创建成功
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//创建DHCP任务
#endif
	OSTaskCreate(led_poll_task,(void *)0,(OS_STK*)&LED_POLL_TASK_STK[LED_POLL_STK_SIZE-1],LED_POLL_TASK_PRIO);	  	//创建LED任务
	OSTaskCreate(key_poll_task,(void *)0,(OS_STK*)&KEY_POLL_TASK_STK[KEY_POLL_STK_SIZE-1],KEY_POLL_TASK_PRIO); 		//创建KEY任务
	OSTaskCreate(eMBPoll_task, (void *)0,(OS_STK*)&eMBPoll_TASK_STK[eMBPoll_STK_SIZE-1],   eMBPoll_TASK_PRIO); 		//创建eMBPoll任务
	OSTaskCreate(adc_poll_task,(void *)0,(OS_STK*)&ADC_POLL_TASK_STK[ADC_POLL_STK_SIZE-1],ADC_POLL_TASK_PRIO);		//创建ADC任务
	OSTaskCreate(dac_poll_task,(void *)0,(OS_STK*)&DAC_POLL_TASK_STK[DAC_POLL_STK_SIZE-1],DAC_POLL_TASK_PRIO);		//创建ADC任务
	OSTaskCreate(display_task, (void *)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],   DISPLAY_TASK_PRIO); 		//显示任务
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  //开中断
}

//显示地址等信息的任务函数
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
		}
#else
		show_address(0); 						//显示静态地址
		OSTaskSuspend(OS_PRIO_SELF); 			//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//按键轮询任务 
//Slave mode:DiscreteInputs variables
//************************************************************************//
//**02功能码 
//**02 Read Discrete Inputs (1x)
//**起始地址 100
//************************************************************************//
void key_poll_task(void *pdata)
{
	POINT_COLOR = BLUE; //蓝色字体
	LCD_ShowString(30,270,200,20,16,"KEYPoll is Start!"); 		//lwip初始化成功
	while(1)
	{
		Key_Poll();
		OSTimeDlyHMSM(0,0,0,10);  //延时10ms
	}
}

//led轮询任务 
//************************************************************************//
//**01 05 15功能码
//**01 Read Coils（0x）  
//**05 Write Single Coil
//**15 Write Multiple Coils
//**起始地址 000
//************************************************************************//
void led_poll_task(void *pdata)
{
	POINT_COLOR = BLUE; //蓝色字体
	LCD_ShowString(30,250,200,20,16,"LEDPoll is Start!"); 		//lwip初始化成功
    while(1)
    {
        LED_Poll();
		BEEP_Poll();
        OSTimeDlyHMSM(0,0,0,10);  //延时10ms
    }
}

//eMBPoll任务
//************************************************************************//
//**03 06 16功能码 
//**03 Read Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 400
//************************************************************************//
void eMBPoll_task(void *pdata)
{
	eMBErrorCode    xStatus;
	eMBTCPInit(0);
	eMBEnable();
	POINT_COLOR = BLUE; //蓝色字体
	LCD_ShowString(30,210,200,20,16,"eMBPoll is Start!"); 		//
	while(1)
	{
		do
		{
			xStatus = eMBPoll(  );
			OSTimeDlyHMSM(0,0,0,10);  	//延时10ms
		}while( xStatus == MB_ENOERR );
		( void )eMBDisable(  );
		( void )eMBClose(  );
		OSTimeDlyHMSM(0,0,0,10);  		//延时10ms
	}
}

//Slave mode:InputRegister variables
//************************************************************************//
//**04功能码 (3x)
//**04 Read Input Registers
//**起始地址 300
//************************************************************************//
void adc_poll_task(void *pdata)
{
	POINT_COLOR = BLUE; //蓝色字体
	LCD_ShowString(30,230,200,20,16,"ADCPoll is Start!"); 		//
    while(1)
    {
        ADC_Poll();
        OSTimeDlyHMSM(0,0,0,10);  //延时10ms
    }
}

//Slave mode:HoldingRegister variables
//************************************************************************//
//**03 06 16功能码 
//**03 Read  Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 40000
//************************************************************************//
void dac_poll_task(void *pdata)
{
	POINT_COLOR = BLUE; //蓝色字体
	LCD_ShowString(30,290,200,20,16,"DACPoll is Start!"); 		//
	while(1)
	{
		DAC_Poll();
		OSTimeDlyHMSM(0,0,0,10);  //延时10ms
	}
}









