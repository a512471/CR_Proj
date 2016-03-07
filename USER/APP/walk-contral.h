#ifndef __WALK_CONTRAL_H__
#define __WALK_CONTRAL_H__

#include "includes.h" 

//状态标志
#define CR_TurnLeft   0
#define CR_TurnRigh   1
#define CR_Go         2
#define CR_Back       3
#define CR_Stop       4

//以小车go方向为视角
//左轮电机
#define CR_LWalk_ENA   PAout(0) //电机脱机控制
#define CR_LWalk_DIR   PAout(1) //电机正方反转控制
#define CR_LWalk_PUL   PAout(8) //控制电机转动的PWM信号源
//右轮电机
#define CR_RWalk_ENA   PAout(2) //电机脱机控制
#define CR_RWalk_DIR   PAout(3) //电机正方反转控制
#define CR_RWalk_PUL   PAout(9) //控制电机转动的PWM信号源

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

//input CR_TurnLeft:向左转UPDATE_TIME次脉冲;
//CR_TurnRigh:向右转UPDATE_TIME次脉冲;
//CR_Go:直走UPDATE_TIME次脉冲;
//CR_Back:后退UPDATE_TIME次脉冲;
//CR_Stop:停止，用于GPIO口省电并停止电机运作
void CR_Walk_Contral(u8 cmd);
void WalK_Init(void);


#endif


