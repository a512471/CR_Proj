#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "usart.h"

#include "walk-contral.h"
#include "pwm.h"
#include "ble-contral.h"
#include "trouble-deal.h"
//ALIENTEK Mini STM32开发板范例代码35
//UCOSII实验1-任务调度  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  	 


extern u8 USART2_RX_BUF[USART2_REC_LEN];
/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			20 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 	
//CR行走任务
//设置任务优先级
#define WALK_TASK_PRIO       			10 
//设置任务堆栈大小
#define WALK_STK_SIZE  		    		64
//任务堆栈	
OS_STK WALK_TASK_STK[WALK_STK_SIZE];
//任务函数
void walk_task(void *pdata);

//BLECtrl蓝牙控制任务
//设置任务优先级
#define BLECTRL_TASK_PRIO       			12 
//设置任务堆栈大小
#define BLECTRL_STK_SIZE  		    		64
//任务堆栈	
OS_STK BLECTRL_TASK_STK[BLECTRL_STK_SIZE];
//任务函数
void BLECtrl_task(void *pdata);

//TroubleDeal壁障处理任务
//设置任务优先级
#define TROUBLEDEAL_TASK_PRIO       			11
//设置任务堆栈大小
#define TROUBLEDEAL_STK_SIZE  		    		64
//任务堆栈	
OS_STK TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE];
//任务函数
void TroubleDeal_task(void *pdata);

//LED任务
//设置任务优先级
#define LED0_TASK_PRIO       			19
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

/////////////////////////////////////////////////////////
OS_EVENT * sem_walk_contral;		//walk_contral信号量指针
OS_EVENT * msg_ble_data;			//蓝牙数据邮箱事件块指针
OS_EVENT * msg_ble_contral;			//蓝牙控制指令邮箱事件块指针
/////////////////////////////////////////////////////////////

 int main(void)
 {	
//     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//     #endif
	delay_init();	    	 //延时函数初始化	
    NVIC_Configuration();	 
	LED_Init();		  	//初始化与LED连接的硬件接口 
    uart_init(9600);	 //初始化为115200 注：需要加入延时
    delay_ms(100);
    WalK_Init(); //初始化行走动作
    BLE_init();
    delay_ms(100);
     
     #ifdef debug
        printf("Main into.\r\n");
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
    sem_walk_contral = OSSemCreate(0);		//创建信号量
    msg_ble_data  = OSMboxCreate((void*)0);	//创建消息邮箱
    msg_ble_contral  = OSMboxCreate((void*)0);	//创建消息邮箱
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
    OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);
   	OSTaskCreate(BLECtrl_task,(void *)0,(OS_STK*)&BLECTRL_TASK_STK[BLECTRL_STK_SIZE-1],BLECTRL_TASK_PRIO);						   
   	OSTaskCreate(TroubleDeal_task,(void *)0,(OS_STK*)&TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE-1],TROUBLEDEAL_TASK_PRIO);						      	
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
    
     #ifdef debug
//        printf("start_task = %x\n",p0);
        printf("start_task into\r\n");
     #endif    
    
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}


//功能:负责控制小车行走,获取消息邮箱中的控制命令，并进行相应控制，
//      通过信号量挂起等待指定pwm脉冲输出完成，此任务优先级应最高，第一时间获得相应并关闭pwm，之后回到原来的优先级
//      没有控制指令时直接挂起等待控制指令
//输入:
//输出:
void walk_task(void *pdata)
{
    u8 err;
    OS_CPU_SR cpu_sr=0;
    u8 *ble_contral_cmd;
    u16 RoundTimes_EachContral = 0;//每次控制时进入timer中断的次数
    #ifdef debug
        printf("walk_task into.\r\n");
    #endif
    while(1)
    {
        #ifdef debug
            printf("walk_wait_ble_msg into.\r\n");
        #endif
        
        ble_contral_cmd = (u8 *)OSMboxPend(msg_ble_contral,0,&err); //等待蓝牙控制信号
        
        #ifdef debug
            printf("walk_wait_ble_msg out.\r\n");
        #endif
        
        cpu_sr=0;
        OS_ENTER_CRITICAL();
        CR_Walk_Contral(*ble_contral_cmd);
        OS_EXIT_CRITICAL();  
        if(*ble_contral_cmd == CR_Stop)
        {
            RoundTimes_EachContral = 0;
            //调用路径记忆功能
            continue;
        }
        #ifdef debug
            printf("walk_sem into.\r\n");
        #endif
        OSSemPend(sem_walk_contral,0,&err);        //挂起并等待PWM完成的信号量
        #ifdef debug
            printf("walk_sem out.\r\n");
        #endif
        
        RoundTimes_EachContral++;
        //delay_ms(2000);
    } 
}

//功能:蓝牙接收任务
//优先级排第三低，用于接收串口蓝牙的数据，处理后发送控制消息给walk_contral任务
//输入:
//输出:
void BLECtrl_task(void *pdata)
{
    u8 err;	
    u8 *ble_data;//蓝牙发送过来的数据
    u8 *ble_contral_cmd;//对接收到的数据提取出来的指令
    #ifdef debug
        printf("BLECtrl into.\r\n");
    #endif    

    while(1)
    {
        #ifdef debug
            printf("msg into.\r\n");
        #endif 
        
        ble_data = (u8 *)OSMboxPend(msg_ble_data,0,&err); 
      
        
        if(Deal_BLE_Dat(ble_data,ble_contral_cmd) == ERROR)
        {
            USART2_RX_STA = 0;
            continue;
        }
        
        #ifdef debug
            printf("the reveive is %s.\r\n",ble_data);
        #endif 
        
        OSMboxPost(msg_ble_contral, ble_contral_cmd);
        
        #ifdef debug
            printf("msg out.\r\n");
        #endif            
        
        USART2_RX_STA = 0;//使能下一次蓝牙接收
        
  
    }
}  


//功能:避障的检测与处理任务
//此任务优先级应该比walk_contral低一级，用于检测到有障碍物时给walk_contral发送信号量及时改变路径
//输入:
//输出:
void TroubleDeal_task(void *pdata)
{
    #ifdef debug
        printf("TroubleDeal into.\r\n");
    #endif
    while(1)
    {
        delay_ms(1000);
    }
}

//LED0任务
//优先级最低，状态指示和调试
void led0_task(void *pdata)
{	
     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//        printf("led1_task = %x\n",p0);
        printf("led0_task into.\r\n");
     #endif        
 	
	while(1)
	{
		LED0=0;
		delay_ms(1000);
		LED0=1;
		delay_ms(1000);
	};
}



