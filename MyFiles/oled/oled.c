#include "stm32f4xx.h"
#include "oled.h"
#include "codetab.h"
#include "rtthread.h"

uint32_t ulTimeOut_Time = 0;

void OLED_IIC_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    RCC_ClocksTypeDef   rcc_clocks;

    /* GPIO Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    /* Reset I2Cx IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    /* Release reset signal of I2Cx IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    /*I2C1 configuration*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); //注意，此处不能合并写成GPIO_PinSource6|GPIO_PinSource7
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

    //PB6: I2C1_SCL  PB7: I2C1_SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C Struct Initialize */
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    /* I2C Initialize */
    I2C_Cmd(I2C1, ENABLE);
	I2C_ClearFlag(I2C1, I2C_FLAG_BUSY);
    /*超时设置*/
    RCC_GetClocksFreq(&rcc_clocks);
    ulTimeOut_Time = (rcc_clocks.SYSCLK_Frequency /10000); 
}

static int OLED_IIC_init(void) 
{
	OLED_IIC_Config();
	return 0;
}
INIT_BOARD_EXPORT(OLED_IIC_init);

unsigned char I2C_Err=0;

uint8_t I2C_ReadOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t Reg_addr)
{  
    uint8_t readout;
    u32 tmr;

    tmr = ulTimeOut_Time;
    while((--tmr)&&I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
    if(tmr==0) I2C_Err = 1;

    I2C_GenerateSTART(I2Cx, ENABLE);
    //发送I2C的START信号，接口自动从从设备编程主设备
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
    if(tmr==0) I2C_Err = 1;

    I2C_Send7bitAddress(I2Cx,I2C_Addr,I2C_Direction_Transmitter);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
    if(tmr==0) I2C_Err = 1;

    I2C_SendData(I2Cx, Reg_addr);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

    I2C_GenerateSTART(I2Cx, ENABLE);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
    if(tmr==0) I2C_Err = 1;

    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
    if(tmr==0) I2C_Err = 1; 

    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))));  /* EV7 */
    if(tmr==0) I2C_Err = 1;

    readout = I2C_ReceiveData(I2Cx);

    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return readout;
}


void I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t Reg_addr,uint8_t value)
{
    u32 tmr;

    tmr = ulTimeOut_Time;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
    while((--tmr)&&I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
    if(tmr==0) I2C_Err = 1;

    I2C_GenerateSTART(I2Cx, ENABLE);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))); 
    if(tmr==0) I2C_Err = 1;

    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
    if(tmr==0) I2C_Err = 1;

    I2C_SendData(I2Cx, Reg_addr);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

    I2C_SendData(I2Cx, value);
    tmr = ulTimeOut_Time;
    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

    I2C_GenerateSTOP(I2Cx, ENABLE);
    //I2C_AcknowledgeConfig(I2Cx, DISABLE);
}


void OLED_Config(void)
{
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xAE);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x20);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x10);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xb0);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xc8);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x00);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x10);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x40);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x81);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xff);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xa1);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xa6);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xa8);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x3F);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xa4);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xd3);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x00);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xd5);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xf0);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xd9);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x22);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xda);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x12);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xdb);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x20);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x8d);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x14);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xaf);
	
	for(int m = 0; m < 8; m++)
	{
		I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xb0+m);
		I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x00);
		I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0x10);
		for(int n = 0; n < 128; n++)
		{
			I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_DAT_ADDRESS, 0x00);
		}
	}
}

static int OLED_init(void) 
{
	OLED_Config();
	return 0;
}
INIT_DEVICE_EXPORT(OLED_init);


void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, 0xb0+y);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, ((x&0xf0)>>4)|0x10);
	I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_CMD_ADDRESS, (x&0x0f)|0x01);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_DAT_ADDRESS, F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_DAT_ADDRESS, F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					I2C_WriteOneByte(I2C1, OLED_DEV_ADDRESS, OLED_DAT_ADDRESS, F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}




