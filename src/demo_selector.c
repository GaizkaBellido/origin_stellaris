/**
 * Selección del demo
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>

/**
 * Function prototypes
 */
extern	int	main_demo_1(void);	// Buttons & LED (+ Display)
extern	int	main_demo_2(void);	// Analog-Digital Converter and Temperature measurements
extern	int	main_demo_3(void);	// PWM demo
extern	int	main_demo_4(void);	// Serial communications
extern	int	main_demo_5(void);	// Serial communications XON/XOFF protocol demo
extern	int	main_demo_6(void);	// LCD screen graphical demo
extern	int	main_demo_7(void);	// FreeRTOS demo
extern	int	main_demo_8(void);	// Internet client and server demo
extern	int	main_demo_9(void);	// Internet client, server and PLC demo
extern	int	main_demo_10(void);	// Music demo

int main(void)
{
	static int res;
//	res = main_demo_1();	// Buttons & LED (+ Display)
//	res = main_demo_2();	// Analog-Digital Converter and Temperature measurements
//	res = main_demo_3();	// PWM demo
//	res = main_demo_4();	// Serial communications
//	res = main_demo_5();	// Serial communications XON/XOFF protocol demo
//	res = main_demo_6();	// LCD screen graphical demo
//	res = main_demo_7();	// FreeRTOS demo
	res = main_demo_8();	// Internet client and server demo
//	res = main_demo_9();	// Internet client, server and PLC demo
//	res = main_demo_10();	// Music demo
	return res;
}
