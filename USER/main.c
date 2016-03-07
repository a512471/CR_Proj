#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "usart.h"

#include "walk-contral.h"

//ALIENTEK Mini STM32�����巶������35
//UCOSIIʵ��1-�������  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  	 



/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 	
//LED0����
//�����������ȼ�
#define WALK_TASK_PRIO       			5 
//���������ջ��С
#define WALK_STK_SIZE  		    		64
//�����ջ	
OS_STK WALK_TASK_STK[WALK_STK_SIZE];
//������
void walk_task(void *pdata);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			7 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);


//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO       			6 
//���������ջ��С
#define LED1_STK_SIZE  					64
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);



 int main(void)
 {	
     #ifdef debug
        u32 a = 1;
        u32 *p0 = &a;
     #endif
	delay_init();	    	 //��ʱ������ʼ��	
    NVIC_Configuration();	 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ� 
    uart_init(115200);	 //��ʼ��Ϊ115200 ע����Ҫ������ʱ
    delay_ms(100);
    WalK_Init(); //��ʼ�����߶���
    delay_ms(100);
     #ifdef debug
        printf("Main into\n");
        printf("main = %x\n",p0); 
     #endif 
     
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
 }

	  
//��ʼ����
void start_task(void *pdata)
{
    #ifdef debug
        u32 a = 1;
        u32 *p0 = &a;
    #endif
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  

   	OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);						   

    
 	//OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	//OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
    
     #ifdef debug
        printf("start_task = %x\n",p0);
        printf("start_task into\n");
     #endif    
    
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}


//����:�������С������
//����:
//���:
void walk_task(void *pdata)
{
    #ifdef debug
        printf("walk_task into.\n");
    #endif
    while(1)
    {
        CR_Walk_Contral(CR_Go);
        delay_ms(1000);
        CR_Walk_Contral(CR_Back);
        delay_ms(1000);
    } 
}

//����:������������
//����:
//���:
void BLECtrl_task(void)
{
    
}  

//LED0����
void led0_task(void *pdata)
{	
     #ifdef debug
        u32 a = 1;
        u32 *p0 = &a;
        printf("led1_task = %x\n",p0);
        printf("led0_task into\n");
     #endif        
 	
	while(1)
	{
		LED0=0;
		delay_ms(10);
		LED0=1;
		delay_ms(10);
	};
}

//LED1����
void led1_task(void *pdata)
{
     #ifdef debug
        u32 a = 1;
        u32 *p0 = &a;
        printf("led2_task = %x\n",p0);
        printf("led1_task into\n");
     #endif 
    
	while(1)
	{
		LED1=0;
		delay_ms(300);
		LED1=1;
		delay_ms(300);
	};
}





