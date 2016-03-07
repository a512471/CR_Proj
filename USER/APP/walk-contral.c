#include "walk-contral.h"
#include "pwm.h"


static void CR_Walk_Stop(void);
static void CR_Walk_Start(void);
static void CR_LWalk_Go(void);
static void CR_RWalk_Go(void);
static void CR_LWalk_Back(void);
static void CR_RWalk_Back(void);
static void Start(void);
static void Stop(void);
static void TurnLeft(void);
static void TurnRigh(void);
static void Go(void);
static void Back(void);
static void WalkCtrl_GPIO_Init(void);


//����:��ʼ��CR����������
//����:
//���:
void WalK_Init(void)
{
    TIM1_PWM_Init(899,1599);//time1 channal0-PA8 channal1-PA9 50Hz
    WalkCtrl_GPIO_Init();
}

//����:С�����߶���
//����:���߷���
//���:
void CR_Walk_Contral(u8 cmd)
{
    switch(cmd)
    {
        case CR_Back:
            Back();
            break;
        case CR_Go:
            Go();
            break;
        case CR_TurnLeft:
            TurnLeft();
            break;
        case CR_TurnRigh:
            TurnRigh();
            break;
        case CR_Stop:
            Stop();
        default:
            break;
    }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
//����:CR���ߵ��״̬��ʼ�� ��ǰ��ֹͣ״̬  
//����:
//���:
static void WalkCtrl_GPIO_Init(void)
{
    CR_Walk_Stop();
    CR_LWalk_Go();
    CR_RWalk_Go();
}
//����:ENA���ͣ�ֹͣ 
//����:
//���:
static void CR_Walk_Stop(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_ENA_CLK|CR_RWalk_ENA_CLK, ENABLE);	 //ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ10MHz
    #ifdef CR_WALK_ENA_GPIP_EQU
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN|CR_RWalk_ENA_PIN;				 //�˿�����
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);					  
    #else
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN;				 //�˿�����
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = CR_RWalk_ENA_PIN;				 //�˿�����     
        GPIO_Init(CR_RWalk_ENA_GPIOx, &GPIO_InitStructure);
    #endif
    CR_LWalk_ENA = CR_RWalk_ENA = 0;
}

//����:ENA���ߣ�����
//����:
//���:
static void CR_Walk_Start(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_ENA_CLK|CR_RWalk_ENA_CLK, ENABLE);	 //ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //�������
    #ifdef CR_WALK_ENA_GPIP_EQU
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN|CR_RWalk_ENA_PIN;				 //�˿�����
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);					 
    #else
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN;				 //�˿�����
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = CR_RWalk_ENA_PIN;				 //�˿�����     
        GPIO_Init(CR_RWalk_ENA_GPIOx, &GPIO_InitStructure);
    #endif
}


//����:����DIR���ߣ�˳ʱ��ת
//����:
//���:
static void CR_LWalk_Go(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_DIR_CLK, ENABLE);	 //ʹ��DIR�˿�ʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //��������
    GPIO_InitStructure.GPIO_Pin = CR_LWalk_DIR_PIN;
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);	      
}

//����:����DIR���ߣ�˳ʱ��ת
//����:
//���:
static void CR_RWalk_Go(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_RWalk_DIR_CLK, ENABLE);	 //ʹ��DIR�˿�ʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //��������
    GPIO_InitStructure.GPIO_Pin = CR_RWalk_DIR_PIN;				 //�˿����� 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);		
}

//����:����DIR���ͣ���ʱ��ת
//����:
//���:
static void CR_LWalk_Back(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_DIR_CLK, ENABLE);	 //ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pin = CR_LWalk_DIR_PIN;				 //�˿����� 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);					 
    CR_LWalk_DIR = 0;
}

//����:����DIR���ͣ���ʱ��ת
//����:
//���:
static void CR_RWalk_Back(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_RWalk_DIR_CLK, ENABLE);	 //ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pin = CR_RWalk_DIR_PIN;				 //�˿����� 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.1
    CR_RWalk_DIR = 0;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


//����:ֹͣ
//����:
//���:
static void Stop(void)
{
    TIM1_PWM_Stop();
    CR_Walk_Stop();
    //GPIO����ʡ��ģʽ
    //�رնԵ���Ĺ���
}

//����:����һ����ת,���ָ��������PWM����,ע����ƽ�����Ҫ����Stop();
//����:
//���:
static void Start(void)
{
    //��GPIO
    //�򿪶Ե���Ĺ��� 
    CR_Walk_Start();
    TIM1_PWM_Begin(450);//ռ�ձȣ�450/(899+1)
}

//����:��ת ������˳ʱ��
//����:
//���:
static void TurnLeft(void)
{
    CR_RWalk_Go();
    CR_LWalk_Go();
    Start();  
}


//����:��ת ��������ʱ��
//����:
//���:
static void TurnRigh(void)
{
    CR_RWalk_Back();
    CR_LWalk_Back();
    Start();  
}


//����:ǰ�� ������ʱ�� ����˳ʱ��
//����:
//���:
static void Go(void)
{
    CR_RWalk_Go();
    CR_LWalk_Back();
    Start();    
}


//����:���� ����˳ʱ�� ������ʱ��
//����:
//���:
static void Back(void)
{
    CR_RWalk_Back();
    CR_LWalk_Go();
    Start();
}














