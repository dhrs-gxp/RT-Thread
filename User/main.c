#include "stm32f4xx.h"
#include "rtthread.h"

int main(void)
{
	while(1)
	{
		GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);
		rt_kprintf("succeed!");
		rt_thread_mdelay(1000);
		GPIO_ResetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);
		rt_thread_mdelay(1000);
	}

	return 0;
}

