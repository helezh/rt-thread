
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
 // 2016-08-30     Aubr.Cool    the first version
 // 2018-02-20		 Helezh				port for lm3s811
 
#include "board.h"

#ifdef RT_USING_UART
#ifdef RT_USING_UART0

struct hw_uart uart_device
={
	.name="uart0",
};

void UART0_IRQHandler(void)
{
    hw_uarts_irq_handler(&uart_device);
}

#endif

static rt_err_t rt_serial_init (rt_device_t dev)
{
	struct hw_uart* uart;
	RT_ASSERT(dev != RT_NULL);
	uart = (struct hw_uart *)dev;
    
	hw_uart_init(uart,115200);

  return RT_EOK;
}

static rt_err_t rt_serial_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct hw_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct hw_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
			hw_open_uarts_isr(uart);
    }

    return RT_EOK;
}

static rt_err_t rt_serial_close(rt_device_t dev)
{
    struct hw_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct hw_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
			hw_close_uarts_isr(uart);
    }

    return RT_EOK;
}

static rt_size_t rt_serial_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t length;
    struct hw_uart* uart;
    uart = (struct hw_uart *)dev;
	  RT_ASSERT(uart != RT_NULL);
	
    // interrupt receive //
    rt_base_t level;

    RT_ASSERT(uart != RT_NULL);

    // disable interrupt //
    level = rt_hw_interrupt_disable();
    length = rt_ringbuffer_get(&(uart->rx_rb), buffer, size);
    // enable interrupt //
    rt_hw_interrupt_enable(level);

    return length;
}

static rt_size_t rt_serial_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    char *ptr = (char*) buffer;
    struct hw_uart* uart;
    uart = (struct hw_uart *)dev;
		RT_ASSERT(uart != RT_NULL);
	
    if (dev->open_flag & RT_DEVICE_FLAG_STREAM)
    {
        // stream mode //
        while (size)
        {
            if (*ptr == '\n')
            {										
							hw_fputc('\r',0);
            }						
						hw_fputc(*ptr,0);						
            ptr ++;
            size --;
        }
    }
    else
    {
        while (size)
        {			
						hw_fputc(*ptr,0);	
            ptr++;
            size--;
        }
    }

    return (rt_size_t) ptr - (rt_size_t) buffer;
}

int rt_serial_install(void)
{  	
		struct hw_uart* uart;

		// get uart device //
		uart = &uart_device;

		// device initialization //
		uart->parent.type = RT_Device_Class_Char;
		
		rt_ringbuffer_init(&(uart->rx_rb), uart->rx_buffer, sizeof(uart->rx_buffer));

		// device interface //
		uart->parent.init 	    = rt_serial_init;
		uart->parent.open 	    = rt_serial_open;
		uart->parent.close      = rt_serial_close;
		uart->parent.read 	    = rt_serial_read;
		uart->parent.write      = rt_serial_write;
		uart->parent.control    = RT_NULL;
		uart->parent.user_data  = RT_NULL;

		rt_device_register(&uart->parent,uart->name, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);

    return 0;
}
INIT_BOARD_EXPORT(rt_serial_install);//
#endif //RT_USING_UART
