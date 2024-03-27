#ifndef __OLED_H
#define __OLED_H
#include "stdint.h"
#include "bit_addr.h"

#define OLED_DEV_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define OLED_DAT_ADDRESS	0x40
#define OLED_CMD_ADDRESS	0x00

void OLED_IIC_Config(void);
uint8_t I2C_ReadOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t Reg_addr);
void I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t Reg_addr,uint8_t value);
void OLED_Config(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize);

#endif
