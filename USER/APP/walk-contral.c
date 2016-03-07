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


//功能:初始化CR的行走驱动
//输入:
//输出:
void WalK_Init(void)
{
    TIM1_PWM_Init(899,1599);//time1 channal0-PA8 channal1-PA9 50Hz
    WalkCtrl_GPIO_Init();
}

//功能:小车行走动作
//输入:行走方向
//输出:
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
//功能:CR行走电机状态初始化 待前行停止状态  
//输入:
//输出:
static void WalkCtrl_GPIO_Init(void)
{
    CR_Walk_Stop();
    CR_LWalk_Go();
    CR_RWalk_Go();
}
//功能:ENA拉低，停止 
//输入:
//输出:
static void CR_Walk_Stop(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_ENA_CLK|CR_RWalk_ENA_CLK, ENABLE);	 //使能PA端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为10MHz
    #ifdef CR_WALK_ENA_GPIP_EQU
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN|CR_RWalk_ENA_PIN;				 //端口配置
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);					  
    #else
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN;				 //端口配置
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = CR_RWalk_ENA_PIN;				 //端口配置     
        GPIO_Init(CR_RWalk_ENA_GPIOx, &GPIO_InitStructure);
    #endif
    CR_LWalk_ENA = CR_RWalk_ENA = 0;
}

//功能:ENA拉高，启动
//输入:
//输出:
static void CR_Walk_Start(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_ENA_CLK|CR_RWalk_ENA_CLK, ENABLE);	 //使能PA端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
    #ifdef CR_WALK_ENA_GPIP_EQU
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN|CR_RWalk_ENA_PIN;				 //端口配置
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);					 
    #else
        GPIO_InitStructure.GPIO_Pin = CR_LWalk_ENA_PIN;				 //端口配置
        GPIO_Init(CR_LWalk_ENA_GPIOx, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = CR_RWalk_ENA_PIN;				 //端口配置     
        GPIO_Init(CR_RWalk_ENA_GPIOx, &GPIO_InitStructure);
    #endif
}


//功能:左轮DIR拉高，顺时针转
//输入:
//输出:
static void CR_LWalk_Go(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_DIR_CLK, ENABLE);	 //使能DIR端口时钟
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输入
    GPIO_InitStructure.GPIO_Pin = CR_LWalk_DIR_PIN;
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);	      
}

//功能:右轮DIR拉高，顺时针转
//输入:
//输出:
static void CR_RWalk_Go(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_RWalk_DIR_CLK, ENABLE);	 //使能DIR端口时钟
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输入
    GPIO_InitStructure.GPIO_Pin = CR_RWalk_DIR_PIN;				 //端口配置 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);		
}

//功能:左轮DIR拉低，逆时针转
//输入:
//输出:
static void CR_LWalk_Back(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_LWalk_DIR_CLK, ENABLE);	 //使能PA端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pin = CR_LWalk_DIR_PIN;				 //端口配置 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);					 
    CR_LWalk_DIR = 0;
}

//功能:右轮DIR拉低，逆时针转
//输入:
//输出:
static void CR_RWalk_Back(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(CR_RWalk_DIR_CLK, ENABLE);	 //使能PA端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pin = CR_RWalk_DIR_PIN;				 //端口配置 
    GPIO_Init(CR_LWalk_DIR_GPIOx, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.1
    CR_RWalk_DIR = 0;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


//功能:停止
//输入:
//输出:
static void Stop(void)
{
    TIM1_PWM_Stop();
    CR_Walk_Stop();
    //GPIO进入省电模式
    //关闭对电机的供电
}

//功能:启动一次运转,输出指定个数的PWM脉冲,注意控制结束后要调用Stop();
//输入:
//输出:
static void Start(void)
{
    //打开GPIO
    //打开对电机的供电 
    CR_Walk_Start();
    TIM1_PWM_Begin(450);//占空比：450/(899+1)
}

//功能:左转 左右轮顺时针
//输入:
//输出:
static void TurnLeft(void)
{
    CR_RWalk_Go();
    CR_LWalk_Go();
    Start();  
}


//功能:右转 左右轮逆时针
//输入:
//输出:
static void TurnRigh(void)
{
    CR_RWalk_Back();
    CR_LWalk_Back();
    Start();  
}


//功能:前进 左轮逆时针 右轮顺时针
//输入:
//输出:
static void Go(void)
{
    CR_RWalk_Go();
    CR_LWalk_Back();
    Start();    
}


//功能:后退 左轮顺时针 右轮逆时针
//输入:
//输出:
static void Back(void)
{
    CR_RWalk_Back();
    CR_LWalk_Go();
    Start();
}














