#ifndef __USART_H
#define __USART_H
#include "stm32f4xx.h"
#include "stdint.h"

void UART1_Config(uint32_t bound);
void rt_hw_console_output(const char *str);
char rt_hw_console_getchar(void);

void UART2_Config(uint32_t bound);
void USART_SendString(USART_TypeDef* USARTx, char *DataString);
void DMA_UART2_RX_Config(void);
void DMA_USART2_Send(char *data, uint16_t size);
#endif


