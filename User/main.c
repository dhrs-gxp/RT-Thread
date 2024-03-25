#include "stm32f4xx.h"
#include "rtthread.h"
#include "bit_addr.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

#define THREAD1_PRIORITY         1
#define THREAD2_PRIORITY         2
#define THREAD3_PRIORITY         3
#define THREAD4_PRIORITY         0

#define THREAD1_STACK_SIZE       512
#define THREAD2_STACK_SIZE       512
#define THREAD3_STACK_SIZE       512
#define THREAD4_STACK_SIZE       512

#define THREAD1_TIMESLICE        5
#define THREAD2_TIMESLICE        5
#define THREAD3_TIMESLICE        5
#define THREAD4_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;
static rt_thread_t tid4 = RT_NULL;

rt_sem_t sem_key1 = RT_NULL;
rt_sem_t sem_key2 = RT_NULL;
rt_sem_t sem_uart2 = RT_NULL;

static void thread1_entry(void *paramenter) {
	static int val = 0;
	while(1) {
		rt_sem_take(sem_key1, RT_WAITING_FOREVER);
		PFout(9) = val;
		val = ~val;
	}
}

static void thread2_entry(void *paramenter) {
	static int val = 0;
	while(1) {
		rt_sem_take(sem_key2, RT_WAITING_FOREVER);
		PFout(10) = val;
		val = ~val;
	}
}

static void thread3_entry(void *paramenter) {
	while(1) {
		if(PBin(8) == 0) {
			rt_thread_mdelay(20);
			if(PBin(8) == 0) {
				rt_sem_release(sem_key1);
				while(PBin(8) == 0) rt_thread_mdelay(200);
			}
		}
		else if(PBin(9) == 0) {
			rt_thread_mdelay(20);
			if(PBin(9) == 0) {
				rt_sem_release(sem_key2);
				while(PBin(9) == 0) rt_thread_mdelay(200);
			}
		}
		else rt_thread_mdelay(50);
	}
}

static void thread4_entry(void *paramenter) {
	char str[10] = "";
	while(1) {
		rt_sem_take(sem_uart2, RT_WAITING_FOREVER);
		sprintf(str, "%d\r\n", DMA_GetCurrDataCounter(DMA1_Stream5));
		DMA_USART2_Send(str, strlen(str));
	}
}

int main(void)
{ 
	sem_key1 = rt_sem_create("sem_key1", 0, RT_IPC_FLAG_PRIO);
	sem_key2 = rt_sem_create("sem_key2", 0, RT_IPC_FLAG_PRIO);
	sem_uart2 = rt_sem_create("sem_uart2", 0, RT_IPC_FLAG_PRIO);
	
	tid1 = rt_thread_create("LED1",
							thread1_entry, RT_NULL,
							THREAD1_STACK_SIZE,
							THREAD1_PRIORITY,
							THREAD1_TIMESLICE
							);
	if(tid1 != RT_NULL) rt_thread_startup(tid1);
	else return -1;
	
	tid2 = rt_thread_create("LED2",
							thread2_entry, RT_NULL,
							THREAD2_STACK_SIZE,
							THREAD2_PRIORITY,
							THREAD2_TIMESLICE
							);
	if(tid2 != RT_NULL) rt_thread_startup(tid2);
	else return -1;
	
	tid3 = rt_thread_create("KEY",
							thread3_entry, RT_NULL,
							THREAD3_STACK_SIZE,
							THREAD3_PRIORITY,
							THREAD3_TIMESLICE
							);
	if(tid3 != RT_NULL) rt_thread_startup(tid3);
	else return -1;
	
	tid4 = rt_thread_create("UART2",
							thread4_entry, RT_NULL,
							THREAD4_STACK_SIZE,
							THREAD4_PRIORITY,
							THREAD4_TIMESLICE
							);
	if(tid4 != RT_NULL) rt_thread_startup(tid4);
	else return -1;
	
	return 0;
}

