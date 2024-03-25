#include "usart.h"	
#include "rtthread.h"
#include "string.h"

#define UART2_RX_BUF_LEN 100
#define UART2_TX_BUF_LEN 100
char Uart2_Rx_Buf[UART2_RX_BUF_LEN] = "";
char Uart2_Tx_Buf[UART2_TX_BUF_LEN] = "";
/**********************************串口1*************************************/
//bound:波特率
void UART1_Config(uint32_t bound){
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
}

static int uart1_init(void) 
{
	UART1_Config(115200);
	return 0;
}
INIT_BOARD_EXPORT(uart1_init);

void rt_hw_console_output(const char *str)
{
    /* 进入临界段 */
    rt_enter_critical();

    /* 直到字符串结束 */
    while (*str!='\0')
    {
        /* 换行 */
        if (*str=='\n')
        {
            USART_SendData(USART1, '\r');
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        }

        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }

    /* 退出临界段 */
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


/**********************************串口2*************************************/
//bound:波特率
void UART2_Config(uint32_t bound){
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_DeInit(USART2);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2时钟
 
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;	//GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);					//初始化PA2，PA3

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART2, &USART_InitStructure);										//初始化串口2

	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;			//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化NVIC寄存器

	USART_Cmd(USART2, ENABLE);						//使能串口2 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	//开启相关中断
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
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	//开启DMA时钟 
    DMA_DeInit(DMA1_Stream6);
    while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}		//等待stream可配置，即DMAy_SxCR.EN变为0
    //配置Stream
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//从8个channel中选择一个
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;       //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart2_Tx_Buf;			//存储器0地址，双缓存模式还要使用M1AR
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;            		//存储器到外设模式 RX TX不同这里，外设到地址或者地址到外设
    DMA_InitStructure.DMA_BufferSize = UART2_TX_BUF_LEN;                	//数据传输量，以外设数据项为单位 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //外设地址保持不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //存储器地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据位宽:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据位宽:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         	//普通模式(与循环模式对应)
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //禁止FIFO模式         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             //单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //单次传输
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Stream6, DISABLE);					//开启DMA传输 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	//采用DMA方式接收
}

static int DMA_UART2_TX_Init(void) 
{
	DMA_UART2_TX_Config();
	return 0;
}
INIT_BOARD_EXPORT(DMA_UART2_TX_Init);

void DMA_USART2_Send(char *data, uint16_t size)
{
	memcpy(Uart2_Tx_Buf, data, size);					//复制数据到DMA发送缓存区
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);	//确保DMA可以被设置
	DMA_SetCurrDataCounter(DMA1_Stream6, size);			//设置数据传输长度
	DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
	DMA_Cmd(DMA1_Stream6, ENABLE);						//打开DMA数据流，开始发送
}

void DMA_UART2_RX_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	//开启DMA时钟 
    DMA_DeInit(DMA1_Stream5);
    while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}		//等待stream可配置，即DMAy_SxCR.EN变为0
    //配置Stream
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//从8个channel中选择一个
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;       //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart2_Rx_Buf;			//存储器0地址，双缓存模式还要使用M1AR
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;            		//存储器到外设模式 RX TX不同这里，外设到地址或者地址到外设
    DMA_InitStructure.DMA_BufferSize = UART2_RX_BUF_LEN;                	//数据传输量，以外设数据项为单位 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //外设地址保持不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //存储器地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据位宽:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据位宽:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         	//普通模式(与循环模式对应)
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //禁止FIFO模式         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             //单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //单次传输
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
    /* 6. 配置DMA中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream5_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Stream5, ENABLE);					//开启DMA传输 
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);	//采用满中断
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	//采用DMA方式接收
}

static int DMA_UART2_RX_Init(void) 
{
	DMA_UART2_RX_Config();
	return 0;
}
INIT_BOARD_EXPORT(DMA_UART2_RX_Init);

void DMA1_Stream5_IRQHandler(void)
{
//清除标志
	if(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) != RESET)//等待DMA2_Steam7传输完成 
	{
		DMA_Cmd(DMA1_Stream5, DISABLE); //关闭DMA,防止处理其间有数据   
		DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);//清除DMA1_Steam5传输完成标志  
//		DMA_SetCurrDataCounter(DMA1_Stream5, RX_BUF_LEN);
		DMA_Cmd(DMA1_Stream5, ENABLE);     //打开DMA, 
	}
}


extern rt_sem_t sem_uart2;
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //接收中断
	{
		USART_ReceiveData(USART2);
		rt_sem_release(sem_uart2);
	} 
} 


void USART_SendString(USART_TypeDef* USARTx, char *DataString){
	USART_ClearFlag(USARTx,USART_FLAG_TC);									//发送字符前清空标志位（否则缺失字符串的第一个字符）
	while(*DataString != '\0')												//字符串结束符
	{
		USART_SendData(USARTx,*DataString);									//每次发送字符串的一个字符
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == 0);				//等待数据发送成功
		USART_ClearFlag(USARTx,USART_FLAG_TC);								//发送字符后清空标志位
		DataString++;
	}
}


