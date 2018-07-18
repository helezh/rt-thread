
#ifndef HW_UARTS_H
#define HW_UARTS_H

#define UART_RX_BUFSZ 8

#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

/* hw uart driver */
struct hw_uart
{
	struct rt_device parent;	
	struct rt_ringbuffer rx_rb;		
	rt_uint8_t rx_buffer[UART_RX_BUFSZ];
	char *name;
};

int hw_fgetc(int ch,struct hw_uart *p);
int hw_fputc(int ch,struct hw_uart *p);
void hw_open_uarts_isr(struct hw_uart *p);
void hw_close_uarts_isr(struct hw_uart *p);
void hw_uarts_irq_handler(struct hw_uart* uart);
rt_err_t hw_uart_init(struct hw_uart *p,uint32_t bandrate);

#endif
