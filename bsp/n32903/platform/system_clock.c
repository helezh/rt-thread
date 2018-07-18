/*
 * File      : clock.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
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
 * 2018-7-10     helezh       first version
 */

#include <rtthread.h>
#include <rthw.h>
#include <stdio.h>
#include "wblib.h"

uint32_t SystemCoreClock=240*MHz;

// @brief System Clock Configuration
void rt_hw_clock_init(void)
{
	//set upll1 Fout=Fin*((FB_DV+2)/IN_DV+2)*(1/No)	1M<Fin/Nr<15M;100M<=Fout<500<=500M
	uint32_t Fdiv = (SystemCoreClock*2/12000000)-2;
	*(volatile UINT32*)REG_APLLCON=0x0000|Fdiv;	//
	printf("SET SYSCLK@%dMHz.\n",(*(volatile UINT32*)REG_APLLCON+2)*12/2);//delay for pll

	//set CPU_N
	*(volatile UINT32*)REG_CLKDIV4 &= ~0x00000001;//cpu_n=0	
	
	//seclect freq
	*(volatile UINT32*)REG_CLKDIV0 &= ~0x00000f00; //system_n=0
	*(volatile UINT32*)REG_CLKDIV0 &= ~0x00000007; //system_s=0
	*(volatile UINT32*)REG_CLKDIV0 &= 0x10;			   //system_sf=apll
	printf("ENABLE CPU@%dMHz.\n",(*(volatile UINT32*)REG_APLLCON+2)*12/2);//delay for switch to pll
}

