/**
 * @addtogroup libMU_Serial
 * Library for easy access to the serial port
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <string.h>
#include <inc/hw_types.h>		// New data types
#include <inc/hw_memmap.h>		// Peripheral memory map
#include <driverlib/sysctl.h>	// System control functions
#include <driverlib/gpio.h>		// General Purpose IO functions
#include <driverlib/uart.h>		// UART functions
#include <libMU/serie.h>

/**
 * Inicializa el HW de la línea serie
 * @param baudrate	Velocidad de comunicaciones
 * @note			The CPU frequency must be set before calling this function
 */
void 	libMU_Serial_Initialize(unsigned int baudrate)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART 0 for console I/O at given baudrate.
    if( baudrate == 0 ) baudrate = 115200;	// default baudrate
    SysCtlPeripheralEnable( SYSCTL_PERIPH_UART0 );
    UARTConfigSetExpClk( UART0_BASE, SysCtlClockGet(), baudrate, 
    	UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );
    UARTEnable( UART0_BASE );
}

/**
 * Envia un numero de caracteres a la línea serie
 * @param text		Texto enviado a la línea serie
 * @param len		Número de caracteres a enviar a la línea serie
 * @return			Número de caracteres enviados
 * @note			A '\r' character is sent before every '\n' character
 */
int	libMU_Serial_SendChars(const char* text, int len)
{
    unsigned int i;

    // Check for valid UART base address, and valid arguments.
    if( text == NULL || len <= 0 ) return 0;

    // Send the characters
    for(i = 0; i < len; i++)
    {
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \r\n in the output.
        if(text[i] == '\n')
        {
            libMU_Serial_SendChar('\r');
        }
        // Send the character to the UART output.
        libMU_Serial_SendChar( text[i] );
    }
    // Return the number of characters written.
    return(i);
}

/**
 * Envia un texto a la línea serie
 * @param text		Texto enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			A '\r' character is sent before every '\n' character
 */
int	libMU_Serial_SendString(const char* text)
{
	if( text == NULL ) return 0;
    return libMU_Serial_SendChars( text, strlen(text) );
}

/**
 * Lee un texto de la línea serie
 * @param donde		Dirección donde se guarda texto leído desde la línea serie
 * @param tamano	Cantidad máxima de caracteres que se pueden leer
 * @return			Número de caracteres leídos
 */
int	libMU_Serial_GetString(char* donde, int tamano)
{
    unsigned long ulCount = 0;
    char cChar;
    static char bLastWasCR = 0;

    // Check for valid UART base address, and valid arguments.
    if( donde == NULL || tamano <= 0 ) return 0;

    // Adjust the length back by 1 to leave space for the trailing null terminator.
    tamano--;

    // Process characters until a newline is received.
    for(;;)
    {
        // Read the next character from the console.
        libMU_Serial_GetChar( &cChar );
        // See if the backspace key was pressed.
        if(cChar == '\b')
        {
            // If there are any characters already in the buffer, then delete
            // the last.
            if(ulCount)
            {
                // Rub out the previous character.
                libMU_Serial_SendChars( "\b \b", 3 );
                // Decrement the number of characters in the buffer.
                ulCount--;
            }
            // Skip ahead to read the next character.
            continue;
        }
        // If this character is LF and last was CR, then just gobble up the
        // character because the EOL processing was taken care of with the CR.
        if((cChar == '\n') && bLastWasCR)
        {
            bLastWasCR = 0;
            continue;
        }
        // See if a newline or escape character was received.
        if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
        {
            // If the character is a CR, then it may be followed by a LF which
            // should be paired with the CR.  So remember that a CR was
            // received.
            if(cChar == '\r')
            {
                bLastWasCR = 1;
            }
            // Stop processing the input and end the line.
            break;
        }
        // Process the received character as long as we are not at the end of
        // the buffer.  If the end of the buffer has been reached then all
        // additional characters are ignored until a newline is received.
        if( ulCount < tamano )
        {
            // Store the character in the caller supplied buffer.
        	donde[ulCount] = cChar;
            // Increment the count of characters received.
            ulCount++;
            // Reflect the character back to the user.
            libMU_Serial_SendChar( cChar );
        }
    }
    // Add a null termination to the string.
    donde[ulCount] = 0;
    // Send a CRLF pair to the terminal to end the line.
    libMU_Serial_SendChars( "\n", 1 );
    // Return the count of chars in the buffer, not counting the trailing 0.
    return(ulCount);
}

/**
 * Envía un caracter a la línea serie
 * @param c			Caracter enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			Esta función bloquea la ejecución hasta que se pueda enviar el carácter
 */
int	libMU_Serial_SendChar(const char c)
{
	UARTCharPut( UART0_BASE, c );
	return 1;
}

/**
 * Envía un caracter a la línea serie sin bloquear la ejecución del programa
 * @param	c		Caracter enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_SendChar_NB(const char c)
{
	if( UARTCharPutNonBlocking( UART0_BASE, c ) )
		return 1;
	else
		return 0;
}

/**
 * Lee un carácter de la línea serie
 * @param donde		Dirección donde se guarda el carácter leído desde la línea serie
 * @return			Número de caracteres leídos
 * @note			Esta función bloquea la ejecución hasta que se reciba un carácter
 */
int	libMU_Serial_GetChar(char* donde)
{
	*donde = UARTCharGet( UART0_BASE );
	return 1;
}

/**
 * Lee un carácter de la línea serie sin bloquear la ejecución del programa
 * @param donde		Dirección donde se guarda el carácter leído desde la línea serie
 * @return			Número de caracteres leídos
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_GetChar_NB(char* donde)
{
	long c = UARTCharGetNonBlocking( UART0_BASE );
	if( c != -1 ) {
		*donde = (char)c;
		return 1;
	}
	return 0;
}

/**
 * @}
 */
