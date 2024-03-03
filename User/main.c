#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"

int main(void)
{
	uart1_init(115200);
	delay_init(168);
	
	while(1)
	{
		printf("Succeed!\r\n");
		delay_ms(1000);
	}
	
}

