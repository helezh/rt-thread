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

#include <rthw.h>
#include "w55fa93_reg.h"
#include "interrupt.h"
#include "wblib.h"

#define MAX_HANDLERS    (64/2)

extern rt_uint32_t rt_interrupt_nest;

/* exception and interrupt handler table */
struct rt_irq_desc irq_desc[MAX_HANDLERS];

rt_uint32_t rt_interrupt_from_thread;
rt_uint32_t rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrupt_flag;


/* --------------------------------------------------------------------
 *  Interrupt initialization
 * -------------------------------------------------------------------- */
#if defined(__CC_ARM)
__asm void setVec(void) 
{
		mov r3,#16
		ldr r0,=0x0
		ldr r1,=0x700000
lcp
		ldr r2,[r1]
		str r2,[r0]
		add r0,r0,#4
		add r1,r1,#4
		sub r3,r3,#1
		cmp r3,#0
		bne lcp
		bx lr
}
#elif(defined(__GNUC__))
__asm(
//				".globl setVec\n"
				"setVec:\n"
				"mov r3,#16\n"
				"ldr r0,=0x0\n"
				"ldr r1,=0x700000\n"
				"lcp:\n"
				"ldr r2,[r1]\n"
				"str r2,[r0]\n"
				"add r0,r0,#4\n"
				"add r1,r1,#4\n"
				"sub r3,r3,#1\n"
				"cmp r3,#0\n"
				"bne lcp\n"
				"bx lr\n"
			);  
#elif(defined(__ICCARM__))
#endif
/*//
 * The default interrupt priority levels (0 = lowest, 3 = highest).
 */

static rt_uint32_t default_irq_priority[MAX_HANDLERS] =
{
    FIQ_LEVEL_0, /*00 */
    IRQ_LEVEL_7, /*01 WDT_INT*/
    IRQ_LEVEL_7, /*02 GPIO_INT0*/
    IRQ_LEVEL_7, /*03 GPIO_INT1*/
	
    IRQ_LEVEL_7, /*04 GPIO_INT2*/
    IRQ_LEVEL_7, /*05 GPIO_INT3*/
    IRQ_LEVEL_7, /*06 SPU_INT*/
    IRQ_LEVEL_7, /*07 I2S_INT*/
	
    IRQ_LEVEL_7, /*08 VPOST_INT*/
    IRQ_LEVEL_7, /*09 CAP_INT*/
    IRQ_LEVEL_7, /*10 Reserved*/
    IRQ_LEVEL_7, /*11 BLT_INT*/
	
    IRQ_LEVEL_7, /*12 FSC_INT*/
    IRQ_LEVEL_7, /*13 HUART_INT*/
    IRQ_LEVEL_7, /*14 TMR0_INT*/
    IRQ_LEVEL_7, /*15 TMR1_INT*/

    IRQ_LEVEL_7, /*16 UDC_INT*/
    IRQ_LEVEL_7, /*17 SIC_INT*/
    IRQ_LEVEL_7, /*18 UHC_INT*/
    IRQ_LEVEL_7, /*19 EDMA_INT*/
	
    IRQ_LEVEL_7, /*20 SPIMS0_INT*/
    IRQ_LEVEL_7, /*21 SPIMS1_INT*/
    IRQ_LEVEL_7, /*22 ADC_INT*/
    IRQ_LEVEL_7, /*23 RTC_INT*/
	
    IRQ_LEVEL_5, /*24 UART_INT*/
    IRQ_LEVEL_7, /*25 PWM_INT*/
    IRQ_LEVEL_7, /*26 JPG_INT*/
    IRQ_LEVEL_7, /*27 Reserved*/
	
    IRQ_LEVEL_7, /*28 KPI*/
    IRQ_LEVEL_7, /*29 Reserved*/
    IRQ_LEVEL_7, /*30 I2C_INT*/
    IRQ_LEVEL_7, /*31 PWR_INT*/	
};

rt_isr_handler_t rt_hw_interrupt_handle(rt_uint32_t vector, void *param)
{
    rt_kprintf("UN-handled interrupt %d occurred!!!\n", vector);
    return RT_NULL;
}
/**
 * This function will initialize hardware interrupt
 */
void rt_hw_interrupt_init(void)
{
    register rt_uint32_t idx;
	
//		setVec();

    for(idx = 0; idx < (MAX_HANDLERS); idx++)
    {		
			*(volatile unsigned int*)(REG_AIC_SCR1+(idx/4)*4)= 
			(*(volatile unsigned int*)(REG_AIC_SCR1+(idx/4)*4)
			& ( 0xFFFFFFF8<<((idx%4)*8) | ( (1<<((idx%4)*8))-1 ) ))
			| default_irq_priority[idx]<<((idx%4)*8);
		}
		
    /* init exceptions table */
    for(idx=0; idx < MAX_HANDLERS; idx++)
    {
        irq_desc[idx].handler = (rt_isr_handler_t)rt_hw_interrupt_handle;
        irq_desc[idx].param = RT_NULL;
#ifdef RT_USING_INTERRUPT_INFO
        rt_snprintf(irq_desc[idx].name, RT_NAME_MAX - 1, "default");
        irq_desc[idx].counter = 0;
#endif
    }

    /* init interrupt nest, and context in thread sp */
    rt_interrupt_nest = 0;
    rt_interrupt_from_thread = 0;
    rt_interrupt_to_thread = 0;
    rt_thread_switch_interrupt_flag = 0;
}


/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_mask(int irq)
{
	*(volatile unsigned int*)(REG_AIC_MDCR)= (1 << irq);
}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_umask(int irq)
{
	*(volatile unsigned int*)(REG_AIC_MECR)=(1 << irq);
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param handler the interrupt service routine to be installed
 * @param param the interrupt service function parameter
 * @param name the interrupt name
 * @return old handler
 */
rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler,void *param, char *name)
{
    rt_isr_handler_t old_handler = RT_NULL;

    if(vector < MAX_HANDLERS)
    {
        old_handler = irq_desc[vector].handler;
        if (handler != RT_NULL)
        {
            irq_desc[vector].handler = (rt_isr_handler_t)handler;
            irq_desc[vector].param = param;
#ifdef RT_USING_INTERRUPT_INFO
            rt_snprintf(irq_desc[vector].name, RT_NAME_MAX - 1, "%s", name);
            irq_desc[vector].counter = 0;
#endif
        }
    }

    return old_handler;
}

rt_uint32_t rt_hw_interrupt_get_active(rt_uint32_t fiq_irq)
{
    //volatile rt_uint32_t irqstat;
    rt_uint32_t id;
    if (fiq_irq == INT_FIQ)
        return 0;

    //IRQ
		id = *(volatile unsigned int*)REG_AIC_IPER>>2;

    return id;
}

void rt_hw_interrupt_ack(rt_uint32_t fiq_irq, rt_uint32_t id)
{
    /* new FIQ generation */
    if (fiq_irq == INT_FIQ)
        return;

    /* new IRQ generation */
    // EIOCR must be write any value after interrupt,
    // or else can't response next interrupt
    *(volatile unsigned int*)(REG_AIC_EOSCR)= 0x0;
}

#ifdef RT_USING_FINSH
void list_irq(void)
{
    int irq;
    rt_kprintf("number\tcount\tname\n");
    for (irq = 0; irq < MAX_HANDLERS; irq++)
    {
        if (rt_strncmp(irq_desc[irq].name, "default", sizeof("default")))
        {
            rt_kprintf("%02ld: %10ld  %s\n",irq, irq_desc[irq].counter, irq_desc[irq].name);
        }
    }
}

#include <finsh.h>
FINSH_FUNCTION_EXPORT(list_irq, list system irq);
FINSH_FUNCTION_EXPORT_ALIAS(list_irq, __cmd_lsirq, list system irq.);
void extern list_device(void);
FINSH_FUNCTION_EXPORT_ALIAS(list_device, __cmd_lsdev, list system device.);
#endif
