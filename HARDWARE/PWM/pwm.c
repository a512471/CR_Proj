#include "pwm.h"
#include "led.h"
#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//PWM  ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define UPDATE_TIME  20 //20�θ����¼�������ж�
//����:Time1 channal1 �� gpio��ʼ�� PA8->PE9 PA9->PE11
//����:
//���:
void TIM1_CH1_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);  
}

//����IO��Ϊ�������룬ʡ��
void TIM1_CH1_GPIO_Sleep(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIO����ʱ��ʹ��                                     	

   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //���ø������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	// GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM1_CH1_GPIO_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ   
    TIM_TimeBaseStructure.TIM_RepetitionCounter = UPDATE_TIME; //����UPDATE_TIME�ν����ж�
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //PWM����
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

    TIM_ClearFlag(TIM1, TIM_FLAG_Update); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2Ԥװ��ʹ�� 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���	
//    TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );//ʹ�ܻ���ʧ��ָ����TIM�ж�    
//	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
}



void TIM1_PWM_Begin(u32 val)
{
    
    TIM1_CH1_GPIO_Init();
    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��
    TIM_SetCompare1(TIM1,val);  
    TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );//ʹ�ܻ���ʧ��ָ����TIM�ж�    
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1    
}

void TIM1_PWM_Stop(void)
{
    TIM1_CH1_GPIO_Sleep();
    TIM_Cmd(TIM1, DISABLE);  //ʹ��TIM1
    TIM_CtrlPWMOutputs(TIM1,DISABLE);	//MOE �����ʹ��       
} 

//time1�жϳ���
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  
    {
        TIM_ClearFlag(TIM1, TIM_FLAG_Update);
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );   
        TIM1_PWM_Stop();
        LED1 = !LED1;
    }
}
