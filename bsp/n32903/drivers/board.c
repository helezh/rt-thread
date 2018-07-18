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
 * 2018-07-10     helezh     Port bsp from at91sam9260 to n329xx
 */

#include <rtthread.h>
#include <rthw.h>
#include <timer0.h>
#include "board.h"
#include <mmu.h>
#include "interrupt.h"

extern void rt_hw_interrupt_init(void);
extern void rt_hw_clock_init(void);
extern void rt_hw_uart_init(void);

static struct mem_desc hw_mem_desc[] =
{
    { 0x00000000, 0xFFFFFFFF, 0x00000000, RW_NCNB },	/* None cached for 4G memory */
//  visual start, visual end, phy start , props
    { 0x00000000, 0x000fffff, 0x00700000, RW_CB 	},  /* ISR Vector table SRAM */
    { 0x00700000, 0x007fffff, 0x00700000, RW_CB 	},  /* 1M cached SDRAM */ 
    { 0x00800000, 0x00efffff, 0x00000000, RW_CB 	},  /* 7M cached SDRAM */    
    { 0x00f00000, 0x00f01fff, 0xff000000, RW_NCNB },	/* 8K SRAM */
};

/**
 * This function will handle rtos timer
 */
static void rt_systick_handler(int vector, void *param)
{
	uint32_t ir = *(volatile unsigned int*)REG_TISR;
	if (ir & 1UL)
			rt_tick_increase();
	*(volatile unsigned int*)REG_TISR|=0x1;
}

/**
 * This function will init pit for system ticks
 */
static void rt_hw_timer_init()
{
    hw_timer0_init();
    /* install interrupt handler */
    rt_hw_interrupt_install(IRQ_TMR0, rt_systick_handler, RT_NULL, "SysTick");
    rt_hw_interrupt_umask(IRQ_TMR0);
}

#if defined(__CC_ARM)
extern int Image$$ER_ZI$$ZI$$Limit;
#define HEAP_BEGIN (&Image$$ER_ZI$$ZI$$Limit)
#elif(defined(__GNUC__))
extern unsigned char __bss_end__;
#define HEAP_BEGIN (&__bss_end__)
#elif(defined(__ICCARM__))
#pragma section = ".noinit"
#define HEAP_BEGIN (__section_end(".noinit"))
#endif

#define HEAP_END (0x00f01fff)//8.8M

/**
 * This function will init n329xx soc */
void rt_hw_board_init(void)
{ 
		/* initialize mmu */
    rt_hw_mmu_init(hw_mem_desc, sizeof(hw_mem_desc)/sizeof(hw_mem_desc[0]));

#ifdef RT_USING_HEAP
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif

    /* initialize hardware interrupt */
    rt_hw_interrupt_init();

    /* initialize the system clock */
    rt_hw_clock_init(); 

    /* initialize uart */
    rt_hw_uart_init();
	
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);

    /* initialize timer0 */
    rt_hw_timer_init();
}
