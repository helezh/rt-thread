
 // File      : usart.c
 // This file is part of RT-Thread RTOS
 // COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 //
 // The license and distribution terms for this file may be
 // found in the file LICENSE in this distribution or at
 // http://www.rt-thread.org/license/LICENSE
 //
 // Change Logs:
 // Date           Author       Notes
 // 2018-02-20		 Helezh				port for lm3s811

#include "rtthread.h"
#include "board.h"

void thread_a_entry(void *p)
{
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOB;
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOD;
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOE;
	GPIO_PORTB_DIR_R=0x3;
	GPIO_PORTD_DIR_R=0x3;
	GPIO_PORTE_DIR_R=0x3;
	
	GPIO_PORTB_DATA_R&=~(0x1<<0);//b0
	GPIO_PORTB_DATA_R&=~(0x1<<1);//b1
	GPIO_PORTD_DATA_R&=~(0x1<<0);//d0
	GPIO_PORTD_DATA_R&=~(0x1<<1);//d1
	GPIO_PORTE_DATA_R&=~(0x1<<0);//e0
	GPIO_PORTE_DATA_R&=~(0x1<<1);//e1
	
	while(1)
	{
		GPIO_PORTB_DATA_R^=0x1<<1;	
		GPIO_PORTD_DATA_R^=0x1<<0;		
		rt_thread_delay(50);	
		
		GPIO_PORTD_DATA_R^=0x1<<1;	
		GPIO_PORTE_DATA_R^=0x1<<0;		
		rt_thread_delay(50);	
		
		GPIO_PORTB_DATA_R^=0x1<<0;
		GPIO_PORTE_DATA_R^=0x1<<1;		
		rt_thread_delay(50);	
	}
}

void thread_b_entry(void *p)
{
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOC;
	GPIO_PORTC_DIR_R=0x20;//c5
	while(1)
	{
		GPIO_PORTC_DATA_R^=0x1<<5;
		while((GPIO_PORTC_DATA_R&0x10)==0x10)//c4
			rt_thread_delay(5);
		rt_thread_delay(5);	
	}
}
ALIGN(8)
rt_uint8_t  a_stack[0x80];
struct rt_thread	thread_a;

rt_uint8_t  b_stack[0xff];
struct rt_thread	thread_b;

void rt_usr_thread_init(void)
{
	rt_err_t err=RT_ERROR;
	err=rt_thread_init(&thread_a,"a",thread_a_entry,RT_NULL,a_stack,sizeof(a_stack), RT_THREAD_PRIORITY_MAX / 3,10);
	if(err!=RT_ERROR)
		rt_thread_startup(&thread_a);
	
	err=RT_ERROR;
	err=rt_thread_init(&thread_b,"b",thread_b_entry,RT_NULL,b_stack,sizeof(b_stack), RT_THREAD_PRIORITY_MAX / 3,10);
	if(err!=RT_ERROR)
		rt_thread_startup(&thread_b);
}
int main()
{
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOC;
	GPIO_PORTC_DIR_R=0x20;
	if((GPIO_PORTC_DATA_R&0x10)==0x10)
	{
		while(1);//·ÀÖ¹jtageËø
	}
	rt_usr_thread_init();
	return 0;
}
