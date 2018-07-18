/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-07-10     helezh     Port to n329xx
 */
 
#include "w55fa93_reg.h"
#include "wblib.h"
#include "rtthread.h"

void led(void *p)
{
	*(volatile unsigned int*)REG_GPIOA_OMD	|= (1u<<1);
	
	while(1)
	{
		*(volatile unsigned int*)REG_GPIOA_DOUT	^= (1u<<1);//led
		rt_thread_delay(50);
	}
}

void print1(void *p)
{
	rt_uint32_t i=0;
	
	while(1)
	{
		rt_kprintf("pa:%08x\n",*(volatile UINT32*)(0xff001f00));
		*(volatile UINT32*)(0xff001f00)=0x12345678;
		rt_kprintf("va:%08x\n",*(volatile UINT32*)(0x00f01f00));
		*(volatile UINT32*)(0x00f01f00)=0x43218765;
		rt_kprintf("print_thread1,i=%d\n",i++);
		rt_thread_delay(100);
	}
}

void print2(void *p)
{
	rt_uint32_t i=0;

	while(1)
	{
		rt_kprintf("print_thread2,i=%d\n",i++);
		rt_thread_delay(1000);
	}
}

#ifdef RT_USING_USER_MAIN
int main()
{
    rt_thread_t tid;
    tid = rt_thread_create("led", led, RT_NULL, 0xff,
                                   (RT_THREAD_PRIORITY_MAX / 8 * 2), 20);
    if(tid != RT_NULL)
        rt_thread_startup(tid);
		
    tid = rt_thread_create("prt1", print1, RT_NULL, 0xff,
                                   (RT_THREAD_PRIORITY_MAX / 8 * 2), 20);
//    if(tid != RT_NULL)
//        rt_thread_startup(tid);
		
    tid = rt_thread_create("prt2", print2, RT_NULL, 0xff,
                                   (RT_THREAD_PRIORITY_MAX / 8 * 2), 20);
//    if(tid != RT_NULL)
//        rt_thread_startup(tid);
}
#endif
