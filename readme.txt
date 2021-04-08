This is a FreeRTOS port to the ATMEGA644, composed by Mathias Dalheimer 
<md@gonium.net>. It integrates Peter Fleurys UART library with FreeRTOS v. 5.0.3.

The code provides a minimal starting point for microcontroller projects. The
directory has been stripped from all unneccessary files and ports. The template
initializes the UART and starts two tasks which write "foo" and "bar" to the UART, 
respectively. In addition, a third tasks toggles the 7th bit of PORTB if the other 
tasks are functioning properly. In my setup, it is connected to an LED to provide a 
visual feedback without serial lines attached.

Have fun,
	 -Mathias
