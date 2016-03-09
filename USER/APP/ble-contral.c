#include "ble-contral.h"
#include "string.h"

#define BLE_DATA_HEAD1  0x3A
#define BLE_DATA_HEAD2  0x2E
#define EN_USART2_RX    1

extern OS_EVENT *msg_ble_data;			//蓝牙邮箱事件块指针

static void uart2_init(u32 bound);
static ErrorStatus check_crc(const uint8_t *src_data, const uint8_t len, const uint8_t *crc_data);

typedef struct
{
    uint8_t *Head;
    uint8_t len;
    uint8_t *data;
    uint8_t *crc;
} Ble_data_t;
//功能:蓝牙数据处理
//输入:
//输出:
ErrorStatus Deal_BLE_Dat(const uint8_t *input, uint8_t *output)
{
    ErrorStatus status;
    Ble_data_t *Ble_data;
    char *ptr;
    Ble_data->Head[0] = BLE_DATA_HEAD1;
    Ble_data->Head[1] = BLE_DATA_HEAD2;
    Ble_data->Head[2] = '\0';
    //数据格式：头(0x3A 0x2E) + 数据长度(十六进制从此处之后算起到CRC) + 数据(控制标志) + CRC 
    //获取数据缓冲区中的数据段
    ptr = strstr((const char*)input,(char *)(Ble_data->Head));
    if(ptr == NULL)
    {
        status = ERROR;
        return (status);
    }
    Ble_data->len = *(ptr+2);
    OS_MemCopy(Ble_data->data,(uint8_t *)(ptr+3),Ble_data->len-2);
    OS_MemCopy(Ble_data->crc,(uint8_t *)(ptr+Ble_data->len+1),2);
    //检验数据的完整性
    if(check_crc(Ble_data->data,Ble_data->len,Ble_data->crc) == ERROR)
    {
        status = ERROR;
        return (status);
    }
    //提取数据
    OS_MemCopy(output, Ble_data->data, Ble_data->len-2);
    status = SUCCESS;
    return (status);
}


//功能:蓝牙的初始化
//输入:
//输出:
void BLE_init()
{
    uart2_init(9600);
    //串口初始化
}

//功能:CRC校验
//输入:
//输出:
static ErrorStatus check_crc(const uint8_t *src_data, const uint8_t len, const uint8_t *crc_data)
{
    ErrorStatus status;
    uint8_t  i, j;
    uint16_t uncrcReg = 0xffff;
    uint16_t uncur;
    for (i = 0; i < len; i++)
    {
        uncur = src_data[i] << 8;
        for (j = 0; j < 8; j++)
        {
            if ((int16_t)(uncrcReg ^ uncur) < 0)
            {
                 uncrcReg = (uncrcReg << 1) ^ 0x1021;
            }
            else
            {
                  uncrcReg <<= 1;
            }               
            uncur <<= 1;           
        }
    }
    if(uncrcReg == ((uint16_t)crc_data[0] << 8 || (uint16_t)crc_data[1]))
        status = SUCCESS;
    else
        status = ERROR;
    return (status);
}

//功能:串口2初始化
//输入:
//输出:
static void uart2_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，GPIOA时钟
    
 	
    USART_DeInit(USART2);  //复位串口2
  
	//USART2_TX--PA.2  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化
   
    //USART1_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 

}

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA = 0;       //接收状态标记

#if EN_USART2_RX   //如果使能了接收
void USART2_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{      
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{         
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else 
                {
                    USART2_RX_STA|=0x8000;	//接收完成了
                  
                    OSMboxPost(msg_ble_data, USART2_RX_BUF);
                }
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
     } 
    

#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	

#ifdef debug
void puts_debug(const u8* str)
{      
    int i;
    for(i = 0; i < strlen((const char*)str)+1; i++)
        USART_SendData(USART2,str[i]);   
}
#endif


