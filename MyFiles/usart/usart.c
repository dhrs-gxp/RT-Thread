#include "usart.h"	
#include "rtthread.h"
#include "string.h"

#define UART2_RX_BUF_LEN 100
#define UART2_TX_BUF_LEN 100
char Uart2_Rx_Buf[UART2_RX_BUF_LEN] = "";
char Uart2_Tx_Buf[UART2_TX_BUF_LEN] = "";
/**********************************����1*************************************/
//bound:������
void UART1_Config(uint32_t bound){
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
}

static int uart1_init(void) 
{
	UART1_Config(115200);
	return 0;
}
INIT_BOARD_EXPORT(uart1_init);

void rt_hw_console_output(const char *str)
{
    /* �����ٽ�� */
    rt_enter_critical();

    /* ֱ���ַ������� */
    while (*str!='\0')
    {
        /* ���� */
        if (*str=='\n')
        {
            USART_SendData(USART1, '\r');
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        }

        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }

    /* �˳��ٽ�� */
   rt_exit_critical();
}

char rt_hw_console_getchar(void)
{
    int ch = -1;
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        ch = (int)USART_ReceiveData(USART1);
    }
    else
    {
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART1,USART_FLAG_TC);
        }
       rt_thread_mdelay(10);
    }
    return ch;  
}


/**********************************����2*************************************/
//bound:������
void UART2_Config(uint32_t bound){
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_DeInit(USART2);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;	//GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//����
	GPIO_Init(GPIOA,&GPIO_InitStructure);					//��ʼ��PA2��PA3

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);										//��ʼ������2

	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;		//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	USART_Cmd(USART2, ENABLE);						//ʹ�ܴ���2 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	//��������ж�
}
 
static int uart2_init(void) 
{
	UART2_Config(115200);
	return 0;
}
INIT_BOARD_EXPORT(uart2_init);

void DMA_UART2_TX_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	//����DMAʱ�� 
    DMA_DeInit(DMA1_Stream6);
    while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}		//�ȴ�stream�����ã���DMAy_SxCR.EN��Ϊ0
    //����Stream
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//��8��channel��ѡ��һ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;       //�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart2_Tx_Buf;			//�洢��0��ַ��˫����ģʽ��Ҫʹ��M1AR
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;            		//�洢��������ģʽ RX TX��ͬ������赽��ַ���ߵ�ַ������
    DMA_InitStructure.DMA_BufferSize = UART2_TX_BUF_LEN;                	//���ݴ�������������������Ϊ��λ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //�����ַ���ֲ���
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //�洢����ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //��������λ��:8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //�洢������λ��:8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         	//��ͨģʽ(��ѭ��ģʽ��Ӧ)
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //�е����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //��ֹFIFOģʽ         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             //���δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //���δ���
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Stream6, DISABLE);					//����DMA���� 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	//����DMA��ʽ����
}

static int DMA_UART2_TX_Init(void) 
{
	DMA_UART2_TX_Config();
	return 0;
}
INIT_BOARD_EXPORT(DMA_UART2_TX_Init);

void DMA_USART2_Send(char *data, uint16_t size)
{
	memcpy(Uart2_Tx_Buf, data, size);					//�������ݵ�DMA���ͻ�����
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);	//ȷ��DMA���Ա�����
	DMA_SetCurrDataCounter(DMA1_Stream6, size);			//�������ݴ��䳤��
	DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
	DMA_Cmd(DMA1_Stream6, ENABLE);						//��DMA����������ʼ����
}

void DMA_UART2_RX_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	//����DMAʱ�� 
    DMA_DeInit(DMA1_Stream5);
    while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}		//�ȴ�stream�����ã���DMAy_SxCR.EN��Ϊ0
    //����Stream
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//��8��channel��ѡ��һ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;       //�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart2_Rx_Buf;			//�洢��0��ַ��˫����ģʽ��Ҫʹ��M1AR
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;            		//�洢��������ģʽ RX TX��ͬ������赽��ַ���ߵ�ַ������
    DMA_InitStructure.DMA_BufferSize = UART2_RX_BUF_LEN;                	//���ݴ�������������������Ϊ��λ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //�����ַ���ֲ���
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //�洢����ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //��������λ��:8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //�洢������λ��:8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         	//��ͨģʽ(��ѭ��ģʽ��Ӧ)
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //�е����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //��ֹFIFOģʽ         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             //���δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //���δ���
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
    /* 6. ����DMA�ж����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream5_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Stream5, ENABLE);					//����DMA���� 
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);	//�������ж�
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	//����DMA��ʽ����
}

static int DMA_UART2_RX_Init(void) 
{
	DMA_UART2_RX_Config();
	return 0;
}
INIT_BOARD_EXPORT(DMA_UART2_RX_Init);

void DMA1_Stream5_IRQHandler(void)
{
//�����־
	if(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) != RESET)//�ȴ�DMA2_Steam7������� 
	{
		DMA_Cmd(DMA1_Stream5, DISABLE); //�ر�DMA,��ֹ�������������   
		DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);//���DMA1_Steam5������ɱ�־  
//		DMA_SetCurrDataCounter(DMA1_Stream5, RX_BUF_LEN);
		DMA_Cmd(DMA1_Stream5, ENABLE);     //��DMA, 
	}
}


extern rt_sem_t sem_uart2;
void USART2_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //�����ж�
	{
		USART_ReceiveData(USART2);
		rt_sem_release(sem_uart2);
	} 
} 


void USART_SendString(USART_TypeDef* USARTx, char *DataString){
	USART_ClearFlag(USARTx,USART_FLAG_TC);									//�����ַ�ǰ��ձ�־λ������ȱʧ�ַ����ĵ�һ���ַ���
	while(*DataString != '\0')												//�ַ���������
	{
		USART_SendData(USARTx,*DataString);									//ÿ�η����ַ�����һ���ַ�
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == 0);				//�ȴ����ݷ��ͳɹ�
		USART_ClearFlag(USARTx,USART_FLAG_TC);								//�����ַ�����ձ�־λ
		DataString++;
	}
}


