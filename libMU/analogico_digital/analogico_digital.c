/**
 * @addtogroup	libMU_AD
 * Library for easy access to the analog/digital converters
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */

#include <inc/hw_types.h>		// New data types
#include <inc/hw_memmap.h>		// Peripheral memory map
#include <driverlib/sysctl.h>	// System control functions
#include <driverlib/gpio.h>		// General Purpose IO functions
#include <driverlib/adc.h>		// Analog/Digital Converter functions
#include <libMU/analogico_digital.h>

/**
 * @param	channel		Selección de entrada ( ADC_CANAL_0, ADC_CANAL_1, ADC_CANAL_2, ... )
 * @return				Converted value
 * @note
 * - Un valor 0 corresponde a 0 voltios en el pin correspondiente
 * - Un valor 1024 corresponde a 3.0 voltions en el pin correspondiente
 * - Esta funcion tarda entre 0 y 10us en devolver el resultado
 */
unsigned long libMU_AD_GetChannelValue(int channel)
{
	static unsigned long ADC_Values[8];	// Max 8 values in FIFO
//	static unsigned long ADC_conversions = 0;

	if( ADCIntStatus( ADC0_BASE, 0, false ) ) {
		// Clear the fifo overflow
		ADCSequenceDataGet		( ADC0_BASE, 0, ADC_Values );
	    ADCSequenceOverflowClear( ADC0_BASE, 0 );
	    // Clear the interrupt status flag for the sample sequencer 0
		ADCIntClear				( ADC0_BASE, 0 );
		// Wait for the end of the next sequence conversion 
		// (5 values in sequence, max. 2us at 500ksps per value)
	    while( !ADCIntStatus( ADC0_BASE, 0, false ) ) {}
		// Get values from FIFO
		ADCSequenceDataGet		( ADC0_BASE, 0, ADC_Values );
		ADCIntClear				( ADC0_BASE, 0 );
//		ADC_conversions++;
	}
	// Return selected channel
	return ADC_Values[ channel ];
}	

/**
 * Inicializa el HW de la conversion analogico digital
 */
void 	libMU_AD_Initialize(void)
{
	///////////////////////////////////////////////////////////////////
	// Initialize ADC converter
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_ADC0 );	// Enable ADC0 peripheral
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOE );// Enable port for ADC pins & assign pins
    SysCtlADCSpeedSet		( SYSCTL_ADCSPEED_500KSPS );	// Set ADC speed to 500000 samples/s
    GPIOPinTypeADC			( GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 );
    // Always enable sample sequence 1. Sequence 1 will get 4 samples 
    ADCSequenceConfigure	( ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0 );

    // Sequence 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing 4 conversions
    // using sequence 1 we will configure steps 0-3.  For more information on the ADC 
    // sequences and steps see the datasheet.
    ADCSequenceStepConfigure( ADC0_BASE, 0, 0, ADC_CTL_CH0 );	// Step 0: CH0
    ADCSequenceStepConfigure( ADC0_BASE, 0, 1, ADC_CTL_CH1 );	// Step 1: CH1
    ADCSequenceStepConfigure( ADC0_BASE, 0, 2, ADC_CTL_CH2 );	// Step 2: CH2
    ADCSequenceStepConfigure( ADC0_BASE, 0, 3, ADC_CTL_CH3 );	// Step 3: CH3
    ADCSequenceStepConfigure( ADC0_BASE, 0, 4,	// Step 4: TS, interrupt signal & end sequence.
    						  ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END );
    ADCSequenceEnable		( ADC0_BASE, 0 );	// Enable sample sequence 1
    ADCIntClear				( ADC0_BASE, 0 );	// Clear ADC SS0 Int
}

/**
 * @}
 */
