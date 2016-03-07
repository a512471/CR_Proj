#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "usart.h"

#include "walk-contral.h"

//ALIENTEK Mini STM32开发板范例代码35
//UCOSII实验1-任务调度  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  	 



/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 	
//CR行走任务
//设置任务优先级
#define WALK_TASK_PRIO       			9 
//设置任务堆栈大小
#define WALK_STK_SIZE  		    		64
//任务堆栈	
OS_STK WALK_TASK_STK[WALK_STK_SIZE];
//任务函数
void walk_task(void *pdata);

//BLECtrl蓝牙控制任务
//设置任务优先级
#define BLECTRL_TASK_PRIO       			8 
//设置任务堆栈大小
#define BLECTRL_STK_SIZE  		    		64
//任务堆栈	
OS_STK BLECTRL_TASK_STK[BLECTRL_STK_SIZE];
//任务函数
void BLECtrl_task(void *pdata);

//TroubleDeal壁障处理任务
//设置任务优先级
#define TROUBLEDEAL_TASK_PRIO       			7 
//设置任务堆栈大小
#define TROUBLEDEAL_STK_SIZE  		    		64
//任务堆栈	
OS_STK TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE];
//任务函数
void TroubleDeal_task(void *pdata);

//LED任务
//设置任务优先级
#define LED0_TASK_PRIO       			11 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);



 int main(void)
 {	
//     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//     #endif
	delay_init();	    	 //延时函数初始化	
    NVIC_Configuration();	 
	LED_Init();		  	//初始化与LED连接的硬件接口 
    uart_init(115200);	 //初始化为115200 注：需要加入延时
    delay_ms(100);
    WalK_Init(); //初始化行走动作
    delay_ms(100);
     #ifdef debug
        printf("Main into\t\n");
//        printf("main = %x\n",p0); 
     #endif 
     
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
 }

	  
//开始任务
void start_task(void *pdata)
{
//    #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//    #endif
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  

   	OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);						   
   	OSTaskCreate(BLECtrl_task,(void *)0,(OS_STK*)&BLECTRL_TASK_STK[BLECTRL_STK_SIZE-1],BLECTRL_TASK_PRIO);						   
   	OSTaskCreate(TroubleDeal_task,(void *)0,(OS_STK*)&TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE-1],TROUBLEDEAL_TASK_PRIO);						      	OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);   
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
    
     #ifdef debug
//        printf("start_task = %x\n",p0);
        printf("start_task into\t\n");
     #endif    
    
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}


//功能:负责控制小车行走
//输入:
//输出:
void walk_task(void *pdata)
{
    #ifdef debug
        printf("walk_task into.\t\n");
    #endif
    while(1)
    {
        CR_Walk_Contral(CR_Go);
        delay_ms(1000);
        CR_Walk_Contral(CR_Stop);
        delay_ms(1000);
    } 
}

//功能:蓝牙接收任务
//输入:
//输出:
void BLECtrl_task(void *pdata)
{
    #ifdef debug
        printf("BLECtrl into.\t\n");
    #endif    
    while(1)
    {
        delay_ms(1000);
    }
}  


//功能:避障的检测与处理任务
//输入:
//输出:
void TroubleDeal_task(void *pdata)
{
    #ifdef debug
        printf("TroubleDeal into.\t\n");
    #endif
    while(1)
    {
        delay_ms(1000);
    }
}

//LED0任务
void led0_task(void *pdata)
{	
     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//        printf("led1_task = %x\n",p0);
        printf("led0_task into.\t\n");
     #endif        
 	
	while(1)
	{
		LED0=0;
		delay_ms(100);
		LED0=1;
		delay_ms(100);
	};
}



