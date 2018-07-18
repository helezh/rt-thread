/***************************************************************************
 *                                                                         *
 * Copyright (c) 2009 Nuvoton Technology. All rights reserved.             *
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     WBLIB.h
 *
 * VERSION
 *     1.1
 *
 * DESCRIPTION
 *     The header file of NUC900 system library.
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2008-06-26  Ver 1.0 draft by Min-Nan Cheng
 *     2009-02-26  Ver 1.1 Changed for NUC900 MCU
 *
 * REMARK
 *     None
 **************************************************************************/
#ifndef _WBLIB_H
#define _WBLIB_H

#include "W55FA93_reg.h"
#include "string.h"

#define MHz	1000000
#define EXTERNAL_CRYSTAL_CLOCK  12*MHz

#define UINT32 unsigned int 
#define UINT8 unsigned char 
	
/* Define constants for use external IO in service parameters.  */
#define EXT0			0
#define EXT1			1
#define EXT2			2


#define SIZE_256K		4
#define SIZE_512K		5
#define SIZE_1M			6
#define SIZE_2M			7
#define SIZE_4M			8
#define SIZE_8M			9
#define SIZE_16M		10
#define SIZE_32M		11

#define BUS_DISABLE		12
#define BUS_BIT_8		13
#define BUS_BIT_16		14
#define BUS_BIT_32		15

extern UINT32 SystemCoreClock;

/* Define the vector numbers associated with each interrupt */
typedef enum int_source_e
{
	IRQ_WDT = 1, 
	IRQ_EXTINT0 = 2, 
	IRQ_EXTINT1 = 3, 
	IRQ_EXTINT2 = 4, 
	IRQ_EXTINT3 = 5, 
	IRQ_SPU = 6, 
	IRQ_I2S = 7, 
	IRQ_VPOST = 8, 
	IRQ_VIDEOIN = 9, 
	IRQ_GPU = 10, 
	IRQ_BLT = 11, 
	IRQ_FSC = 12, 
	IRQ_HUART = 13, 
	IRQ_TMR0 = 14, 
	IRQ_TMR1 = 15, 
	IRQ_UDC = 16, 
	IRQ_SIC = 17, 
	IRQ_UHC = 18, 
	IRQ_EDMA = 19, 
	IRQ_SPIMS0 = 20, 
	IRQ_SPIMS1 = 21, 
	IRQ_ADC = 22, 
	IRQ_RTC = 23, 
	IRQ_UART = 24, 
	IRQ_PWM = 25, 
	IRQ_JPG = 26,  
	//IRQ_PWM2 = 27, 
	IRQ_KPI = 28, 
	IRQ_DES	= 29,
	IRQ_I2C = 30, 
	IRQ_PWR = 31	
} INT_SOURCE_E;

typedef enum
{
	WE_GPIO =0x01,
	WE_RTC =0x02,
	WE_SDH =0x04,
	WE_UART =0x08,
	WE_UDC =0x10,
	WE_UHC =0x20,
	WE_ADC =0x40,
	WE_KPI =0x80
}WAKEUP_SOURCE_E;

typedef struct datetime_t
{
	UINT32	year;
	UINT32	mon;
	UINT32	day;
	UINT32	hour;
	UINT32	min;
	UINT32	sec;
} DateTime_T;

/* Define constants for use timer in service parameters.  */
#define TIMER0            0
#define TIMER1            1


#define ONE_SHOT_MODE     0
#define PERIODIC_MODE     1
#define TOGGLE_MODE       2
#define UNINTERRUPT_MODE  3

#define ONE_HALF_SECS     0
#define FIVE_SECS         1
#define TEN_SECS          2
#define TWENTY_SECS       3

/* Define constants for use UART in service parameters.  */
#define WB_UART_0		0
#define WB_UART_1		1

#define WB_DATA_BITS_5    0x00
#define WB_DATA_BITS_6    0x01
#define WB_DATA_BITS_7    0x02
#define WB_DATA_BITS_8    0x03

#define WB_STOP_BITS_1    0x00
#define WB_STOP_BITS_2    0x04

#if 0 //VA91
#define WB_PARITY_NONE    0x00
#define WB_PARITY_ODD     0x00
#define WB_PARITY_EVEN    0x10
#else
#define WB_PARITY_NONE   0x00
#define WB_PARITY_ODD     0x08
#define WB_PARITY_EVEN    0x18
#endif
//#define WB_DTR_Low        0x01
//#define WB_RTS_Low        0x02
//#define WB_MODEM_En       0x08

#define LEVEL_1_BYTE      	0x0	/* Normal/High speed UART */
#define LEVEL_4_BYTES    	0x1
#define LEVEL_8_BYTES     	0x2
#define LEVEL_14_BYTES    	0x3
#define LEVEL_30_BYTES    	0x4	/* High speed UART only */
#define LEVEL_46_BYTES    	0x5
#define LEVEL_62_BYTES    	0x6 

/* Define constants for use AIC in service parameters.  */
#define WB_SWI                     0
#define WB_D_ABORT                 1
#define WB_I_ABORT                 2
#define WB_UNDEFINE                3

/* The parameters for sysSetInterruptPriorityLevel() and 
   sysInstallISR() use */
#define FIQ_LEVEL_0                0
#define IRQ_LEVEL_1                1
#define IRQ_LEVEL_2                2
#define IRQ_LEVEL_3                3
#define IRQ_LEVEL_4                4
#define IRQ_LEVEL_5                5
#define IRQ_LEVEL_6                6
#define IRQ_LEVEL_7                7

/* The parameters for sysSetGlobalInterrupt() use */
#define ENABLE_ALL_INTERRUPTS      0
#define DISABLE_ALL_INTERRUPTS     1

/* The parameters for sysSetInterruptType() use */
#define LOW_LEVEL_SENSITIVE        0x00
#define HIGH_LEVEL_SENSITIVE       0x01
#define NEGATIVE_EDGE_TRIGGER   0x02
#define POSITIVE_EDGE_TRIGGER    0x03

/* The parameters for sysSetLocalInterrupt() use */
#define ENABLE_IRQ                 0x7F
#define ENABLE_FIQ                 0xBF
#define ENABLE_FIQ_IRQ          0x3F
#define DISABLE_IRQ                0x80
#define DISABLE_FIQ                0x40
#define DISABLE_FIQ_IRQ         0xC0

/* Define Cache type  */
#define CACHE_WRITE_BACK		0
#define CACHE_WRITE_THROUGH	1
#define CACHE_DISABLE			-1

#define MMU_DIRECT_MAPPING	0
#define MMU_INVERSE_MAPPING	1


/* Define Error Code */
#define E_ERR_CLK			(SYS_BA+0x01)

/* Define system clock come from */
typedef enum 
{
	eSYS_EXT 	= 0,
	eSYS_X32K 	= 1,
	eSYS_APLL  	= 2,
	eSYS_UPLL  	= 3
}E_SYS_SRC_CLK;

/* Define constants for use Cache in service parameters.  */
#define CACHE_4M		2
#define CACHE_8M		3
#define CACHE_16M		4
#define CACHE_32M		5
#define I_CACHE		6
#define D_CACHE		7
#define I_D_CACHE		8


/* Define UART initialization data structure */
typedef struct UART_INIT_STRUCT
{
	UINT32		uart_no;
    	UINT32		uiFreq;
    	UINT32		uiBaudrate;
    	UINT8		uiDataBits;
    	UINT8		uiStopBits;
    	UINT8		uiParity;
    	UINT8		uiRxTriggerLevel;
}WB_UART_T;

//extern UINT32 UART_BA;

/* UART return value */
#define WB_INVALID_PARITY       	-1
#define WB_INVALID_DATA_BITS    	-2
#define WB_INVALID_STOP_BITS    	-3
#define WB_INVALID_BAUD         	-4

/* Define the constant values of PM */
#define WB_PM_IDLE		1
#define WB_PM_PD			2
#define WB_PM_MIDLE	    	5

/* Define Wake up source */ 
#define	WAKEUP_GPIO 	0
#define	WAKEUP_RTC 		1
#define	WAKEUP_SDH  	2
#define	WAKEUP_UART  	3
#define	WAKEUP_UDC  	4
#define	WAKEUP_UHC  	5
#define	WAKEUP_ADC  	6
#define	WAKEUP_KPI  	7

#define WB_PM_PD_IRQ_Fail			-1
#define WB_PM_Type_Fail			    	-2
#define WB_PM_INVALID_IRQ_NUM		-3
#define WB_PM_CACHE_OFF			-4

#endif  /* _WBLIB_H */

