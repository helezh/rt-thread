/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-02-20		 Helezh				 port for lm3s811
 */
#include <rthw.h>
#include <rtthread.h>

// rtthread tick configuration
// 1. include header files
// 2. configure rtos tick and interrupt
// 3. add tick interrupt handler

// rtthread tick configuration
// 1. include some header file as need
#include "board.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define HEAP_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define HEAP_BEGIN    (&__bss_end)
#endif

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	// rtthread tick configuration
	// 2. Configure rtos tick and interrupt
		system_core_clk_cfg();
		SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)SRAM_END);
#endif
}

// rtthread tick configuration
// 3. add tick interrupt handler tick
void SysTick_Handler(void)
{
//	NVIC_ST_CTRL_R;
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

uint32_t SystemCoreClock=6000000;//50Hz,64kB,8KB

void system_core_clk_cfg(void)
{	//1.
	SYSCTL_RCC_R=SYSCTL_RCC_BYPASS;
	SYSCTL_RCC_R&=~SYSCTL_RCC_USESYSDIV;
	//2.
	SYSCTL_RCC_R|=SYSCTL_RCC_XTAL_6MHZ|SYSCTL_RCC_OSCSRC_MAIN;
	SYSCTL_RCC_R&=~(SYSCTL_RCC_OEN|SYSCTL_RCC_PWRDN);
	//3.
	SYSCTL_RCC_R|=SYSCTL_RCC_SYSDIV_4|SYSCTL_RCC_USESYSDIV;
	//4.
	while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS)==SYSCTL_RIS_PLLLRIS);
	//5.
	SYSCTL_RCC_R=SYSCTL_RCC_R&(~SYSCTL_RCC_BYPASS);
	SystemCoreClock=50000000;
}

uint32_t SysTick_Config(uint32_t ticks)
{
	if ((ticks - 1UL) > 0xFFFFFFUL)
  {
    return (1UL);                                                  
  }
  NVIC_ST_RELOAD_R  = (uint32_t)(ticks - 1UL); 				             
	NVIC_ST_CTRL_R |=NVIC_ST_CTRL_INTEN;
	NVIC_ST_CTRL_R |=NVIC_ST_CTRL_CLK_SRC;
	NVIC_ST_CTRL_R |=NVIC_ST_CTRL_ENABLE;
	NVIC_SYS_PRI3_R|=NVIC_SYS_PRI3_TICK_M;

	return (0UL);
}
__asm void wait(int a)
{
	mov r0,lr
  bx 	lr
}

void FaultISR(void)
{
	uint32_t _lr_=0;
	wait(_lr_);
}
