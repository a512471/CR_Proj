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
 	
//CR��������
//�����������ȼ�
#define WALK_TASK_PRIO       			9 
//���������ջ��С
#define WALK_STK_SIZE  		    		64
//�����ջ	
OS_STK WALK_TASK_STK[WALK_STK_SIZE];
//������
void walk_task(void *pdata);

//BLECtrl������������
//�����������ȼ�
#define BLECTRL_TASK_PRIO       			8 
//���������ջ��С
#define BLECTRL_STK_SIZE  		    		64
//�����ջ	
OS_STK BLECTRL_TASK_STK[BLECTRL_STK_SIZE];
//������
void BLECtrl_task(void *pdata);

//TroubleDeal���ϴ�������
//�����������ȼ�
#define TROUBLEDEAL_TASK_PRIO       			7 
//���������ջ��С
#define TROUBLEDEAL_STK_SIZE  		    		64
//�����ջ	
OS_STK TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE];
//������
void TroubleDeal_task(void *pdata);

//LED����
//�����������ȼ�
#define LED0_TASK_PRIO       			11 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);



 int main(void)
 {	
//     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//     #endif
	delay_init();	    	 //��ʱ������ʼ��	
    NVIC_Configuration();	 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ� 
    uart_init(115200);	 //��ʼ��Ϊ115200 ע����Ҫ������ʱ
    delay_ms(100);
    WalK_Init(); //��ʼ�����߶���
    delay_ms(100);
     #ifdef debug
        printf("Main into\t\n");
//        printf("main = %x\n",p0); 
     #endif 
     
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
 }

	  
//��ʼ����
void start_task(void *pdata)
{
//    #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//    #endif
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  

   	OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);						   
   	OSTaskCreate(BLECtrl_task,(void *)0,(OS_STK*)&BLECTRL_TASK_STK[BLECTRL_STK_SIZE-1],BLECTRL_TASK_PRIO);						   
   	OSTaskCreate(TroubleDeal_task,(void *)0,(OS_STK*)&TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE-1],TROUBLEDEAL_TASK_PRIO);						      	OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);   
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
    
     #ifdef debug
//        printf("start_task = %x\n",p0);
        printf("start_task into\t\n");
     #endif    
    
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}


//����:�������С������
//����:
//���:
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

//����:������������
//����:
//���:
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


//����:���ϵļ���봦������
//����:
//���:
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

//LED0����
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



