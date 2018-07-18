/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2015, RT-Thread Development Team
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
 * 2018-07-10     helezh       first version
 */

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include "interrupt.h"
#include "board.h"
#include <stdio.h>

static struct rt_serial_device serial;

typedef struct
{
	char *dev_name;
	volatile UINT32* UART_REG_BAUD;
	volatile UINT32* UART_REG_LCR;
	volatile UINT32* UART_REG_THR;
	volatile UINT32* UART_REG_ISR;
	volatile UINT32* UART_REG_RBR;
	volatile UINT32* UART_REG_IER;
	volatile UINT32* UART_REG_FCR;
	volatile UINT32* UART_REG_TOR;
	int irq;
} hw_uart_t;
hw_uart_t uart_n;

static int hw_uart_fputc(struct rt_serial_device *serial, char c)
{
    hw_uart_t *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (hw_uart_t *)serial->parent.user_data;
	
		*uart->UART_REG_THR = c;
		while(((*uart->UART_REG_ISR)&THRE_IF)!=THRE_IF){}
			
    return c;
}
static int hw_uart_fgetc(struct rt_serial_device *serial)
{
	hw_uart_t *uart;

	RT_ASSERT(serial != RT_NULL);
	uart = (hw_uart_t *)serial->parent.user_data;
	
	if( *uart->UART_REG_ISR & 0x01 )
		return   *uart->UART_REG_RBR & 0xff;
	
	return -1;
}

/**
 * This function will handle serial
 */
void hw_uart_handler(int vector, void *param)
{
    rt_uint32_t status;
    hw_uart_t *uart;
    rt_device_t dev = (rt_device_t)param;
    uart = (hw_uart_t *)dev->user_data;
	
    status = *uart->UART_REG_ISR;

    if(!(status & 0x01))
        return;
    rt_hw_serial_isr((struct rt_serial_device *)dev, RT_SERIAL_EVENT_RX_IND);
}

/**
* UART device in RT-Thread
*/
static rt_err_t hw_uart_configure(struct rt_serial_device *serial,
                                    struct serial_configure *uart_config)
{
	hw_uart_t *uart;

	RT_ASSERT(serial != RT_NULL);
	RT_ASSERT(uart_config != RT_NULL);
	uart = (hw_uart_t *)serial->parent.user_data;
	
	*(volatile unsigned int*)(REG_CLKDIV3) &=(~0xFF00);
	*(volatile unsigned int*)(REG_GPAFUN) |=(MF_GPA11 | MF_GPA10);
	*(volatile unsigned int*)(REG_APBCLK) |=UART1_CKE;
			
//	*uart->UART_REG_FCR =TX_RST|RX_RST;
	*uart->UART_REG_BAUD=(0x30<<24)|((EXTERNAL_CRYSTAL_CLOCK/ uart_config->baud_rate)-2);//
	*uart->UART_REG_LCR =0x3;
	*uart->UART_REG_FCR =(LEVEL_1_BYTE << 4);
	*uart->UART_REG_TOR =0x7F;	
	*uart->UART_REG_IER=RDA_IEN;//en uart_irq
	
    return RT_EOK;
}

static rt_err_t hw_uart_control(struct rt_serial_device *serial,
                                  int cmd, void *arg)
{
    hw_uart_t* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (hw_uart_t *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        rt_hw_interrupt_mask(uart->irq);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        rt_hw_interrupt_umask(uart->irq);
        break;
    }

    return RT_EOK;
}
void init_uart_reg(void)
{
	uint32_t uart_offset=0x0100;

#if defined(RT_USING_UART0)
	uart_offset=0x000;
	uart_n.dev_name="uart0",
#endif
	
#if defined(RT_USING_UART1)
	uart_offset=0x100;
	uart_n.dev_name="uart1",	
#endif
	
  uart_n.irq=IRQ_UART;
	uart_n.UART_REG_LCR=(UINT32*)(REG_UART_LCR+uart_offset);
	uart_n.UART_REG_THR=(UINT32*)(REG_UART_THR+uart_offset);
	uart_n.UART_REG_ISR=(UINT32*)(REG_UART_ISR+uart_offset);
	uart_n.UART_REG_RBR=(UINT32*)(REG_UART_RBR+uart_offset);
	uart_n.UART_REG_IER=(UINT32*)(REG_UART_IER+uart_offset);
	uart_n.UART_REG_FCR=(UINT32*)(REG_UART_FCR+uart_offset);
	uart_n.UART_REG_TOR=(UINT32*)(REG_UART_TOR+uart_offset);	
	uart_n.UART_REG_BAUD=(UINT32*)(REG_UART_BAUD+uart_offset);
}
static const struct rt_uart_ops hw_usart_ops =
{
    .configure = hw_uart_configure,
    .control = hw_uart_control,
    .putc = hw_uart_fputc,
    .getc = hw_uart_fgetc,
		.dma_transmit = RT_NULL
};
void hw_serial_config_set_default(struct rt_serial_device* serial)
{
    serial->ops = &hw_usart_ops;

    serial->config.baud_rate = BAUD_RATE_115200;
    serial->config.bit_order = BIT_ORDER_LSB;
    serial->config.data_bits = DATA_BITS_8;
    serial->config.parity    = PARITY_NONE;
    serial->config.stop_bits = STOP_BITS_1;
    serial->config.invert    = NRZ_NORMAL;
    serial->config.bufsz     = RT_SERIAL_RB_BUFSZ;
		init_uart_reg();
}
/**
 * This function will handle init uart
 */
int rt_hw_uart_init(void)
{
	#define DRV_REG_OPS (RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM)	
	
	hw_serial_config_set_default(&serial);
	
	/* register uart device */
	rt_hw_serial_register(&serial, uart_n.dev_name, DRV_REG_OPS, &uart_n);
	rt_hw_interrupt_install(uart_n.irq, hw_uart_handler,(void *)&(serial.parent), uart_n.dev_name);
	rt_hw_interrupt_umask(uart_n.irq);

	return 0;
}
INIT_BOARD_EXPORT(rt_hw_uart_init);

//for printf 
#pragma import(__use_no_semihosting)         
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;
int fputc(int ch, FILE *f)
{      
//	hw_uart_fputc(0,ch);    
	*(volatile UINT32*)(REG_UART_THR+0x100) = ch;
	while((*(volatile unsigned int*)(REG_UART_ISR+0x100)&THRE_IF)!=THRE_IF){}
	return ch;
}
