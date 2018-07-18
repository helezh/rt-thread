/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-10-08     Bernard      add board.h to this bsp
 * 2018-02-20		 Helezh				 port for lm3s811
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include <rthw.h>
#include <rtthread.h>

#include "hw_uarts.h"
#include <lm3s811.h>                    // Device header

extern uint32_t SystemCoreClock;
uint32_t SysTick_Config(uint32_t);
void system_core_clk_cfg(void);

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 8

#define SRAM_SIZE         8
#define SRAM_END          (0x20000000 + SRAM_SIZE * 1024)

#endif
