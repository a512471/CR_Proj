#ifndef __WALK_CONTRAL_H__
#define __WALK_CONTRAL_H__

#include "includes.h" 

//״̬��־
#define CR_TurnLeft   0
#define CR_TurnRigh   1
#define CR_Go         2
#define CR_Back       3
#define CR_Stop       4

//��С��go����Ϊ�ӽ�
//���ֵ��
#define CR_LWalk_ENA   PAout(0) //����ѻ�����
#define CR_LWalk_DIR   PAout(1) //���������ת����
#define CR_LWalk_PUL   PAout(8) //���Ƶ��ת����PWM�ź�Դ
//���ֵ��
#define CR_RWalk_ENA   PAout(2) //����ѻ�����
#define CR_RWalk_DIR   PAout(3) //���������ת����
#define CR_RWalk_PUL   PAout(9) //���Ƶ��ת����PWM�ź�Դ

#define CR_LWalk_ENA_PIN   GPIO_Pin_0
#define CR_RWalk_ENA_PIN   GPIO_Pin_2 
#define CR_LWalk_DIR_PIN   GPIO_Pin_1 
#define CR_RWalk_DIR_PIN   GPIO_Pin_3 
#define CR_LWalk_PUL_PIN   GPIO_Pin_9 
#define CR_RWalk_PUL_PIN   GPIO_Pin_11 

#define CR_LWalk_ENA_GPIOx   GPIOA
#define CR_RWalk_ENA_GPIOx   GPIOA 
#define CR_LWalk_DIR_GPIOx   GPIOA
#define CR_RWalk_DIR_GPIOx   GPIOA 
#define CR_LWalk_PUL_GPIOx   GPIOE
#define CR_RWalk_PUL_GPIOx   GPIOE

#define CR_LWalk_ENA_CLK     RCC_APB2Periph_GPIOA
#define CR_RWalk_ENA_CLK     RCC_APB2Periph_GPIOA 
#define CR_LWalk_DIR_CLK     RCC_APB2Periph_GPIOA
#define CR_RWalk_DIR_CLK     RCC_APB2Periph_GPIOA 
#define CR_LWalk_PUL_CLK     RCC_APB2Periph_GPIOA
#define CR_RWalk_PUL_CLK     RCC_APB2Periph_GPIOA

#define CR_WALK_ENA_GPIP_EQU
#define CR_WALK_DIR_GPIP_EQU
#define CR_WALK_PUL_GPIP_EQU

//input CR_TurnLeft:����תUPDATE_TIME������;
//CR_TurnRigh:����תUPDATE_TIME������;
//CR_Go:ֱ��UPDATE_TIME������;
//CR_Back:����UPDATE_TIME������;
//CR_Stop:ֹͣ������GPIO��ʡ�粢ֹͣ�������
void CR_Walk_Contral(u8 cmd);
void WalK_Init(void);


#endif


