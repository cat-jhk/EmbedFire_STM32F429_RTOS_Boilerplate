#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "stream_buffer.h"
#include "semphr.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"   


//创建队列句柄
QueueHandle_t Queue_Data_Handle;
#define queue_data_length 10
 
//宏定义 send_task
BaseType_t retA;		
TaskHandle_t Pt_send_Task_TaskHandle;
void queue_send_Task(void *p);
#define Sned_Task_Name 		  "queue_send"
#define Sned_Task_StackD  		128
#define Sned_Task_Priority		1
 
//宏定义 rece_task
BaseType_t retB;
TaskHandle_t Pt_rece_Task_TaskHandle;
void queue_receive_Task(void *p);
#define Rece_Task_Name 		  "queue_rece"
#define Rece_Task_StackD  		128
#define Rece_Task_Priority		1
 
//宏定义 LED_task
TaskHandle_t 		retC;
StackType_t 	xLEDTask_Stack[256];
StaticTask_t 	xLEDTask_TCB;
void LED_Task(void *p);
#define LED_Task_Name 		  "LED"
#define LED_Task_StackD  		256
#define LED_Task_Priority		2

/* 空闲任务静态内存 */
StackType_t 	xIdleTaskStack[100];
StaticTask_t 	xIdleTaskTCB;

/* 定时器任务静态内存 */
StaticTask_t xTimerTaskTCB;
StackType_t  xTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

/* 静态创建必须实现的空闲任务内存函数 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint16_t *pulIdleTaskStackSize ) 
{
    *ppxIdleTaskTCBBuffer   = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = xIdleTaskStack;
    *pulIdleTaskStackSize   = 100;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint16_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer   = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = xTimerTaskStack;
    *pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
}

void queue_send_Task(void *p)
{	
	uint32_t send_value = 0;
	while(1)
	{    
		xQueueSend(Queue_Data_Handle, &send_value, portMAX_DELAY);
		{	//如果BUFF满，则不会执行如下程序
			send_value++;
			if(send_value > 100)
			{
				send_value = 0;
			}
			vTaskDelay(100);
		}
	}
}
 
void queue_receive_Task(void *p)
{
	uint32_t rece_value = 0;
	while(1)
	{
		xQueueReceive(Queue_Data_Handle, &rece_value, portMAX_DELAY);
		{
			//如果BUFF空，则不会执行如下程序
			printf("rece_value = %d\r\n", rece_value);
			
		}		
	}
}
 
void LED_Task(void *p)
{
	while (1)
	{
		LED1( ON );			 // 亮 
		vTaskDelay(1000);
		LED1( OFF );		  // 灭

		LED2( ON );			// 亮 
		vTaskDelay(1000);
		LED2( OFF );		  // 灭

		LED3( ON );			 // 亮 
		vTaskDelay(1000);
		LED3( OFF );		  // 灭	

		/*轮流显示 红绿蓝黄紫青白 颜色*/
		LED_RED;
		vTaskDelay(1000);
		
		LED_GREEN;
		vTaskDelay(1000);
		
		LED_BLUE;
		vTaskDelay(1000);
		
		LED_YELLOW;
		vTaskDelay(1000);
		
		LED_PURPLE;
		vTaskDelay(1000);
				
		LED_CYAN;
		vTaskDelay(1000);
		
		LED_WHITE;
		vTaskDelay(1000);
		
		LED_RGBOFF;
		vTaskDelay(1000);
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Debug_USART_Config();
	LED_GPIO_Config();
	
	/*创建队列 注意队列长度 和 数据字节大小*/
	Queue_Data_Handle = xQueueCreate(queue_data_length,  //队列长度
																		sizeof(uint32_t));  //单个数据大小四字节
									
	/*创建发送任务*/
	retA = xTaskCreate(	(TaskFunction_t)    queue_send_Task,
											(const char *)			Sned_Task_Name,
											(uint16_t)					Sned_Task_StackD,
											(void *)						NULL,
											(UBaseType_t)				Sned_Task_Priority,
											(TaskHandle_t *)		&Pt_send_Task_TaskHandle);
											
	/*创建接收任务*/
	retB = xTaskCreate(	(TaskFunction_t)    queue_receive_Task,
											(const char *)			Rece_Task_Name,
											(uint16_t)					Rece_Task_StackD,
											(void *)						NULL,
											(UBaseType_t)				Rece_Task_Priority,
											(TaskHandle_t *)		&Pt_rece_Task_TaskHandle);
											
	/*创建LED任务*/									
	retC = xTaskCreateStatic(	(TaskFunction_t)    LED_Task,
														(const char *)			LED_Task_Name,
														(uint16_t)					LED_Task_StackD,
														(void *)						NULL,
														(UBaseType_t)				LED_Task_Priority,
														(StackType_t *)		  xLEDTask_Stack,
														(StaticTask_t *)		&xLEDTask_TCB);
											
	/*开始调度*/
	vTaskStartScheduler();
	/*不会执行到这里*/
	while (1) 
	{		
		;
	}
}
