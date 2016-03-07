#include "ble-contral.h"
#include "string.h"

#define BLE_DATA_HEAD1  0x3A
#define BLE_DATA_HEAD2  0x2E

ErrorStatus check_crc(const uint8_t *src_data, const uint8_t len, const uint8_t *crc_data);
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
        return (status = ERROR);
    Ble_data->len = *(ptr+2);
    OS_MemCopy(Ble_data->data,(uint8_t *)(ptr+3),Ble_data->len-2);
    OS_MemCopy(Ble_data->crc,(uint8_t *)(ptr+Ble_data->len+1),2);
    //检验数据的完整性
    if(check_crc(Ble_data->data,Ble_data->len,Ble_data->crc) == ERROR)
        return (status = ERROR);
    //提取数据
    OS_MemCopy(output, Ble_data->data, Ble_data->len-2);
    return (status = SUCCESS);
}


//功能:蓝牙的初始化
//输入:
//输出:
void BLE_init()
{
    //串口初始化
}

void uart2_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	
    USART_DeInit(USART1);  //复位串口1
  
	//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}

//功能:CRC校验
//输入:
//输出:
ErrorStatus check_crc(const uint8_t *src_data, const uint8_t len, const uint8_t *crc_data)
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
        return (status = SUCCESS);
    else
        return (status = ERROR);
}


