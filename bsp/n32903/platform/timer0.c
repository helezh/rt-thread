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
 * 2018-07-10     helezh       n329xx
 */

#include "timer0.h"
#include "w55fa93_reg.h"
#include "rtthread.h"
#include "wblib.h"
#include <stdlib.h>
#include <stdio.h>

void hw_timer0_init(void)
{
	*(volatile unsigned int*)REG_APBCLK|= TMR0_CKE;

	*(volatile unsigned int*)REG_TICR0= SystemCoreClock/(20*RT_TICK_PER_SECOND);
	*(volatile unsigned int*)REG_TCSR0= 0x6a000000;//EN_TM0 EN_IE PERIODIC_MODE
}
