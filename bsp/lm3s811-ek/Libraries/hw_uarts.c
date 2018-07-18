
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
 
 #include "board.h"

int hw_fputc(int ch, struct hw_uart* uart)
{		
	UART0_DR_R=ch;
//	while((UART0_FR_R&UART_FR_BUSY) == UART_FR_BUSY);
	while(UART0_FR_R & UART_FR_TXFF);
		
	return ch;
}

int hw_fgetc(int ch, struct hw_uart* uart)
{
		return ch;
}

void hw_close_uarts_isr(struct hw_uart* uart)
{
	// Disable the UART Interrupt //
	NVIC_EN0_R &=~NVIC_EN0_INT5;
}

void hw_open_uarts_isr(struct hw_uart* uart)
{
	// Enable the UART Interrupt //
	NVIC_EN0_R |=NVIC_EN0_INT5;
}

rt_err_t hw_uart_init(struct hw_uart* uart,uint32_t bandrate)
{		
	//PA1/TX
	//PA0/RX
	SYSCTL_RCGC1_R|=SYSCTL_RCGC1_UART0;
	SYSCTL_RCGC2_R|=SYSCTL_RCGC2_GPIOA;
	
	rt_hw_interrupt_disable();
	
	GPIO_PORTA_AFSEL_R|=0x3;
	UART0_IBRD_R=((((SystemCoreClock * 8) / bandrate) + 1) / 2)/64;
	UART0_FBRD_R=((((SystemCoreClock * 8) / bandrate) + 1) / 2)%64;	
	UART0_LCRH_R|=UART_LCRH_WLEN_8;
	UART0_CTL_R |=UART_CTL_UARTEN;
	UART0_IM_R |= UART_IM_RXIM|UART_IM_RTIM;

	return RT_EOK;
}
void hw_uarts_irq_handler(struct hw_uart* uart)
{
    // enter interrupt //
    rt_interrupt_enter();
    
		if((UART0_MIS_R & UART_MIS_RTMIS )==UART_MIS_RTMIS )
    {
				UART0_ICR_R |= UART_ICR_RTIC;	
				
				rt_ringbuffer_putchar_force(&(uart->rx_rb), (rt_uint8_t)UART0_DR_R);
        // invoke callback //
        if(uart->parent.rx_indicate != RT_NULL)
        {
            uart->parent.rx_indicate(&uart->parent, rt_ringbuffer_data_len(&uart->rx_rb));
        }    
    }
    
    // leave interrupt //
    rt_interrupt_leave();
}
