#include "ble-contral.h"
#include "string.h"

#define BLE_DATA_HEAD1  0x3A
#define BLE_DATA_HEAD2  0x2E
#define EN_USART2_RX    1

extern OS_EVENT *msg_ble_data;			//���������¼���ָ��

static void uart2_init(u32 bound);
static ErrorStatus check_crc(const uint8_t *src_data, const uint8_t len, const uint8_t *crc_data);

typedef struct
{
    uint8_t *Head;
    uint8_t len;
    uint8_t *data;
    uint8_t *crc;
} Ble_data_t;
//����:�������ݴ���
//����:
//���:
ErrorStatus Deal_BLE_Dat(const uint8_t *input, uint8_t *output)
{
    ErrorStatus status;
    Ble_data_t *Ble_data;
    char *ptr;
    Ble_data->Head[0] = BLE_DATA_HEAD1;
    Ble_data->Head[1] = BLE_DATA_HEAD2;
    Ble_data->Head[2] = '\0';
    //���ݸ�ʽ��ͷ(0x3A 0x2E) + ���ݳ���(ʮ�����ƴӴ˴�֮������CRC) + ����(���Ʊ�־) + CRC 
    //��ȡ���ݻ������е����ݶ�
    ptr = strstr((const char*)input,(char *)(Ble_data->Head));
    if(ptr == NULL)
    {
        status = ERROR;
        return (status);
    }
    Ble_data->len = *(ptr+2);
    OS_MemCopy(Ble_data->data,(uint8_t *)(ptr+3),Ble_data->len-2);
    OS_MemCopy(Ble_data->crc,(uint8_t *)(ptr+Ble_data->len+1),2);
    //�������ݵ�������
    if(check_crc(Ble_data->data,Ble_data->len,Ble_data->crc) == ERROR)
    {
        status = ERROR;
        return (status);
    }
    //��ȡ����
    OS_MemCopy(output, Ble_data->data, Ble_data->len-2);
    status = SUCCESS;
    return (status);
}


//����:�����ĳ�ʼ��
//����:
//���:
void BLE_init()
{
    uart2_init(9600);
    //���ڳ�ʼ��
}

//����:CRCУ��
//����:
//���:
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

//����:����2��ʼ��
//����:
//���:
static void uart2_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��
    
 	
    USART_DeInit(USART2);  //��λ����2
  
	//USART2_TX--PA.2  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��
   
    //USART1_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA = 0;       //����״̬���

#if EN_USART2_RX   //���ʹ���˽���
void USART2_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{      
			if(USART2_RX_STA&0x4000)//���յ���0x0d
			{         
				if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else 
                {
                    USART2_RX_STA|=0x8000;	//���������
                  
                    OSMboxPost(msg_ble_data, USART2_RX_BUF);
                }
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
     } 
    

#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
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


