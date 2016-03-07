#include "pwm.h"
#include "led.h"
#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//PWM  驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define UPDATE_TIME  20 //20次更新事件后进入中断
//功能:Time1 channal1 的 gpio初始化 PA8->PE9 PA9->PE11
//输入:
//输出:
void TIM1_CH1_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);
   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);  
}

//设置IO口为浮空输入，省电
void TIM1_CH1_GPIO_Sleep(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能                                     	

   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //复用浮空输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	// GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM1_CH1_GPIO_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式   
    TIM_TimeBaseStructure.TIM_RepetitionCounter = UPDATE_TIME; //计数UPDATE_TIME次进入中断
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //PWM设置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_ClearFlag(TIM1, TIM_FLAG_Update); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2预装载使能 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器	
//    TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );//使能或者失能指定的TIM中断    
//	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}



void TIM1_PWM_Begin(u32 val)
{
    
    TIM1_CH1_GPIO_Init();
    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能
    TIM_SetCompare1(TIM1,val);  
    TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );//使能或者失能指定的TIM中断    
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1    
}

void TIM1_PWM_Stop(void)
{
    TIM1_CH1_GPIO_Sleep();
    TIM_Cmd(TIM1, DISABLE);  //使能TIM1
    TIM_CtrlPWMOutputs(TIM1,DISABLE);	//MOE 主输出使能       
} 

//time1中断程序
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
