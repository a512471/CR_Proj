#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "usart.h"

#include "walk-contral.h"
#include "pwm.h"
#include "ble-contral.h"
#include "trouble-deal.h"
#include <stdlib.h>
//ALIENTEK Mini STM32�����巶������35
//UCOSIIʵ��1-�������  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  	 


extern uint8_t USART2_RX_BUF[USART2_REC_LEN];
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
#define STATDISTANCE_TASK_PRIO       			9 
//���������ջ��С
#define STATDISTANCE_STK_SIZE  		    		64
//�����ջ	
OS_STK STATDISTANCE_TASK_STK[STATDISTANCE_STK_SIZE];
//������
void StatDistance_task(void *pdata);

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
OS_EVENT    *sem_per_PWM;		//per_PWM�ź���ָ��,����ÿ��PWM����timer�ж���ͳ��������������ͬ��
OS_EVENT    *msg_ble_data;			//�������������¼���ָ�룬BLE�յ������ݴ��ݸ�BLE_contral�������Ϣ
OS_EVENT    *msg_ble_contral;			//��������ָ�������¼���ָ�룬ble_contral���񴫵ݸ�walk_contral�������Ϣ
OS_EVENT    *msg_trouble_contral;       //���Ͽ���ָ�������¼���ָ�룬trouble_contral���񴫵ݸ�walk_contral�������Ϣ
OS_FLAG_GRP *flags_walk_contral;	        //walk_contral�ź�����,BLE_contral�����trouble_deal���񴫵ݸ�walk_contral��ͬ���ź�
OS_FLAG_GRP *flags_troubles;	        //trouble_deal�ź��������������жϴ��ݸ�trouble_deal�����ͬ���ź�
/////////////////////////////////////////////////////////////
#define     From_Ble_Contral        0x01    //����BLE_Contral�ı�־
#define     From_Trouble_Contral    0x02    //����Trouble_Contral�ı�־
#define     All_Contral_Num         From_Ble_Contral|From_Trouble_Contral
///////////////////////////////////////////////////////////////
void BCA(const uint8_t *input,uint8_t *output);
///////////////////////////////////////////////////////////////

 int main(void)
 {	
//     #ifdef debug
//        uint32_t a = 1;
//        uint32_t *p0 = &a;
//     #endif
	delay_init();	    	 //��ʱ������ʼ��	
    NVIC_Configuration();	 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ� 
    uart_init(9600);	 //��ʼ��Ϊ115200 ע����Ҫ������ʱ
    delay_ms(100);
    WalK_Init(); //��ʼ�����߶���
    BLE_init();
//    Trouble_Check_init();
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
//        uint32_t a = 1;
//        uint32_t *p0 = &a;
//    #endif
    uint8_t err;
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
    sem_per_PWM = OSSemCreate(0);		//�����ź���
    msg_ble_data  = OSMboxCreate((void*)0);	//������Ϣ����
    msg_ble_contral  = OSMboxCreate((void*)0);	//������Ϣ����
    msg_trouble_contral  = OSMboxCreate((void*)0);	//������Ϣ����
    flags_troubles = OSFlagCreate(0,&err); 	//�����ź�����	
    flags_walk_contral = OSFlagCreate(0,&err); 	//�����ź�����	
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
    OSTaskCreate(StatDistance_task,(void *)0,(OS_STK*)&STATDISTANCE_TASK_STK[STATDISTANCE_STK_SIZE-1],STATDISTANCE_TASK_PRIO);						   
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


//����:��������PWM�ĸ����ļ��㣬��walk_task����Ŀ���
//����:
//���:
void StatDistance_task(void *pdata)
{
    uint8_t err;
    uint16_t RoundTimes_EachContral = 0;//ÿ�ο���ʱ����timer�жϵĴ���
    #ifdef debug
        printf("StatDistance_task into.\r\n");
    #endif
    while(1)
    {
        #ifdef debug
            printf("walk_sem into.\r\n");
        #endif
        OSSemPend(sem_per_PWM,0,&err);        //���𲢵ȴ�PWM��ɵ��ź���
        #ifdef debug
            printf("walk_sem out.\r\n");
        #endif
        
        RoundTimes_EachContral++;
        delay_ms(2000);
    }
}

//����:�������С������,��ȡ��Ϣ�����еĿ��������������Ӧ���ƣ�
//      ͨ���ź�������ȴ�ָ��pwm���������ɣ����������ȼ�Ӧ��ߣ���һʱ������Ӧ���ر�pwm��֮��ص�ԭ�������ȼ�
//      û�п���ָ��ʱֱ�ӹ���ȴ�����ָ��
//����:
//���:
void walk_task(void *pdata)
{
    uint8_t err;
    OS_CPU_SR cpu_sr=0;
    uint32_t ble_contral_cmd;
    uint32_t trouble_contral_cmd;
    uint16_t flags;
    uint8_t  ascii_cmd[10];
    #ifdef debug
        printf("walk_task into.\r\n");
    #endif
    while(1)
    {
        //�ȴ��ź�����
        flags = OSFlagPend(flags_walk_contral,All_Contral_Num,OS_FLAG_WAIT_SET_OR,0,&err);//�ȴ��ź���,��8λ��

        //�ж��ź�����
        switch(flags)
        {
            case From_Ble_Contral://�����������ƴ���
                #ifdef debug
                    printf("walk_wait_ble_msg into.\r\n");
                #endif                
                ble_contral_cmd = (uint32_t)OSMboxPend(msg_ble_contral,0,&err); //�ȴ����������ź� 
                #ifdef debug
                    printf("walk_wait_ble_msg out.\r\n");
                    printf("the ble_contral_cmd msg is %d.\r\n",ble_contral_cmd);
                    BCA((uint8_t *)&ble_contral_cmd,ascii_cmd);
                #endif            
                cpu_sr=0;
                OS_ENTER_CRITICAL();
                CR_Walk_Contral(ble_contral_cmd);
                OS_EXIT_CRITICAL();              
                break;
            case From_Trouble_Contral: //������ϴ���
                trouble_contral_cmd = (uint32_t)OSMboxPend(msg_trouble_contral,0,&err); //�ȴ����Ͽ����ź� 
                //������Ӧ����ı���
                break;
            default:
                break;
        }
       //��ͳ��·�߹���
        //delay_ms(2000);
    } 
}

//����:������������
//���ȼ��ŵ����ͣ����ڽ��մ������������ݣ�������Ϳ�����Ϣ��walk_contral����
//����:
//���:
void BLECtrl_task(void *pdata)
{
    uint8_t err;	
    uint8_t *ble_data;//�������͹���������
    uint32_t ble_contral_cmd = 0;//�Խ��յ���������ȡ������ָ��
    #ifdef debug
        printf("BLECtrl into.\r\n");
    #endif    

    while(1)
    {
        #ifdef debug
            printf("ble_contral_msg into.\r\n");
        #endif 
        
        ble_data = (uint8_t *)OSMboxPend(msg_ble_data,0,&err); 
        
        #ifdef debug
            printf("ble_contral_msg out.\r\n");
        #endif      
        if(Deal_BLE_Dat(ble_data,(uint8_t *)&ble_contral_cmd) == ERROR)
        {
            USART2_RX_STA = 0;
            continue;
        }
        
        #ifdef debug
            printf("deal ble_data success.\r\n");
        #endif 
        
        OSFlagPost(flags_walk_contral,From_Ble_Contral,OS_FLAG_SET,&err);//�����ź�����walk_contral����
        OSMboxPost(msg_ble_contral, (void *)ble_contral_cmd);
        
        USART2_RX_STA = 0;//ʹ����һ����������
  
    }
}  


//����:16����תASCII 
//����:
//���:
void BCA(const uint8_t *input,uint8_t *output)
{
    uint8_t i,j,tmp = 0;
    for(i = 0; i < strlen((const char *)input); i++)
    {
        j = i << 2;
        //ת����4λ
        if( (input[i] & 0x0f) <= 9 )
            output[j + 1] = (input[i] & 0x0f) + 0x30;
        else 
            output[j + 1] = (input[i] & 0x0f)+ 0x37;
        #ifdef debug
            printf("the lowbit is %c.\r\n",output[j + 1]);
        #endif 
        //ת����4λ
        tmp = input[i] >> 4;
        if( tmp <= 9 )
            output[j] = tmp + 0x30;
        else
            output[j] = tmp + 0x37;
        #ifdef debug
            printf("the highbit is %c.\r\n",output[j]);
        #endif         
    }
    output[i << 2] = '\0';
}


//����:���ϵļ���봦������
//���������ȼ�Ӧ�ñ�walk_contral��һ�������ڼ�⵽���ϰ���ʱ��walk_contral�����ź�����ʱ�ı�·��
//����:
//���:
void TroubleDeal_task(void *pdata)
{
    uint8_t err;
    uint16_t flags;
    uint32_t trouble_contral_cmd;
    #ifdef debug
        printf("TroubleDeal_task into.\r\n");
    #endif
    while(1)
    {
//        flags = OSFlagPend(flags_troubles,All_Trouble_Checkers,OS_FLAG_WAIT_SET_OR,0,&err);//�ȴ��ź���,��8λ��
//        OSFlagPost(flags_troubles,All_Trouble_Checkers,OS_FLAG_CLR,&err);//ȫ���ź�������
//        
//        OSFlagPost(flags_walk_contral,From_Trouble_Contral,OS_FLAG_SET,&err);//�����ź�������֪ͨwalk_contral����
//        OSMboxPost(msg_trouble_contral, (void *)trouble_contral_cmd);//���ݱ��ϸ�����ı�־
        delay_ms(1000);
    }
}

//LED0����
//���ȼ���ͣ�״ָ̬ʾ�͵���
void led0_task(void *pdata)
{	
     #ifdef debug
//        uint32_t a = 1;
//        uint32_t *p0 = &a;
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



