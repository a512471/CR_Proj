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
        return (status = ERROR);
    Ble_data->len = *(ptr+2);
    OS_MemCopy(Ble_data->data,(uint8_t *)(ptr+3),Ble_data->len-2);
    OS_MemCopy(Ble_data->crc,(uint8_t *)(ptr+Ble_data->len+1),2);
    //�������ݵ�������
    if(check_crc(Ble_data->data,Ble_data->len,Ble_data->crc) == ERROR)
        return (status = ERROR);
    //��ȡ����
    OS_MemCopy(output, Ble_data->data, Ble_data->len-2);
    return (status = SUCCESS);
}


//����:�����ĳ�ʼ��
//����:
//���:
void BLE_init()
{
    //���ڳ�ʼ��
}

void uart2_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	
    USART_DeInit(USART1);  //��λ����1
  
	//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

//����:CRCУ��
//����:
//���:
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


