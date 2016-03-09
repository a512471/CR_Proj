#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "usart.h"

#include "walk-contral.h"
#include "pwm.h"
#include "ble-contral.h"
#include "trouble-deal.h"
//ALIENTEK Mini STM32�����巶������35
//UCOSIIʵ��1-�������  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  	 


extern u8 USART2_RX_BUF[USART2_REC_LEN];
/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			20 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 	
//CR��������
//�����������ȼ�
#define WALK_TASK_PRIO       			10 
//���������ջ��С
#define WALK_STK_SIZE  		    		64
//�����ջ	
OS_STK WALK_TASK_STK[WALK_STK_SIZE];
//������
void walk_task(void *pdata);

//BLECtrl������������
//�����������ȼ�
#define BLECTRL_TASK_PRIO       			12 
//���������ջ��С
#define BLECTRL_STK_SIZE  		    		64
//�����ջ	
OS_STK BLECTRL_TASK_STK[BLECTRL_STK_SIZE];
//������
void BLECtrl_task(void *pdata);

//TroubleDeal���ϴ�������
//�����������ȼ�
#define TROUBLEDEAL_TASK_PRIO       			11
//���������ջ��С
#define TROUBLEDEAL_STK_SIZE  		    		64
//�����ջ	
OS_STK TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE];
//������
void TroubleDeal_task(void *pdata);

//LED����
//�����������ȼ�
#define LED0_TASK_PRIO       			19
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

/////////////////////////////////////////////////////////
OS_EVENT * sem_walk_contral;		//walk_contral�ź���ָ��
OS_EVENT * msg_ble_data;			//�������������¼���ָ��
OS_EVENT * msg_ble_contral;			//��������ָ�������¼���ָ��
/////////////////////////////////////////////////////////////

 int main(void)
 {	
//     #ifdef debug
//        u32 a = 1;
//        u32 *p0 = &a;
//     #endif
	delay_init();	    	 //��ʱ������ʼ��	
    NVIC_Configuration();	 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ� 
    uart_init(9600);	 //��ʼ��Ϊ115200 ע����Ҫ������ʱ
    delay_ms(100);
    WalK_Init(); //��ʼ�����߶���
    BLE_init();
    delay_ms(100);
     
     #ifdef debug
        printf("Main into.\r\n");
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
    sem_walk_contral = OSSemCreate(0);		//�����ź���
    msg_ble_data  = OSMboxCreate((void*)0);	//������Ϣ����
    msg_ble_contral  = OSMboxCreate((void*)0);	//������Ϣ����
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
    OSTaskCreate(walk_task,(void *)0,(OS_STK*)&WALK_TASK_STK[WALK_STK_SIZE-1],WALK_TASK_PRIO);
   	OSTaskCreate(BLECtrl_task,(void *)0,(OS_STK*)&BLECTRL_TASK_STK[BLECTRL_STK_SIZE-1],BLECTRL_TASK_PRIO);						   
   	OSTaskCreate(TroubleDeal_task,(void *)0,(OS_STK*)&TROUBLEDEAL_TASK_STK[TROUBLEDEAL_STK_SIZE-1],TROUBLEDEAL_TASK_PRIO);						      	
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
    
     #ifdef debug
//        printf("start_task = %x\n",p0);
        printf("start_task into\r\n");
     #endif    
    
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}


//����:�������С������,��ȡ��Ϣ�����еĿ��������������Ӧ���ƣ�
//      ͨ���ź�������ȴ�ָ��pwm���������ɣ����������ȼ�Ӧ��ߣ���һʱ������Ӧ���ر�pwm��֮��ص�ԭ�������ȼ�
//      û�п���ָ��ʱֱ�ӹ���ȴ�����ָ��
//����:
//���:
void walk_task(void *pdata)
{
    u8 err;
    OS_CPU_SR cpu_sr=0;
    u8 *ble_contral_cmd;
    u16 RoundTimes_EachContral = 0;//ÿ�ο���ʱ����timer�жϵĴ���
    #ifdef debug
        printf("walk_task into.\r\n");
    #endif
    while(1)
    {
        #ifdef debug
            printf("walk_wait_ble_msg into.\r\n");
        #endif
        
        ble_contral_cmd = (u8 *)OSMboxPend(msg_ble_contral,0,&err); //�ȴ����������ź�
        
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
            //����·�����书��
            continue;
        }
        #ifdef debug
            printf("walk_sem into.\r\n");
        #endif
        OSSemPend(sem_walk_contral,0,&err);        //���𲢵ȴ�PWM��ɵ��ź���
        #ifdef debug
            printf("walk_sem out.\r\n");
        #endif
        
        RoundTimes_EachContral++;
        //delay_ms(2000);
    } 
}

//����:������������
//���ȼ��ŵ����ͣ����ڽ��մ������������ݣ�������Ϳ�����Ϣ��walk_contral����
//����:
//���:
void BLECtrl_task(void *pdata)
{
    u8 err;	
    u8 *ble_data;//�������͹���������
    u8 *ble_contral_cmd;//�Խ��յ���������ȡ������ָ��
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
        
        USART2_RX_STA = 0;//ʹ����һ����������
        
  
    }
}  


//����:���ϵļ���봦������
//���������ȼ�Ӧ�ñ�walk_contral��һ�������ڼ�⵽���ϰ���ʱ��walk_contral�����ź�����ʱ�ı�·��
//����:
//���:
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

//LED0����
//���ȼ���ͣ�״ָ̬ʾ�͵���
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



