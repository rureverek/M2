/* Based on FreeRTOS Demo main (AVR-GCC Port). */
/* Modified by 
	Piotr Mikulowski, University of Southampton
	pm4u19@soton.ac.uk
*/

#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

#include "status.h"
#include "uart.h"

//PortIO settings
#include "lights.h"

/*
Il MATTO PINOUT
PORT	| PIN	|FUNCTION|
--------------------------
 A		| 0		| RED LED		|
 A		| 2		| GREEN L.
 A		| 3		| YELLOW L.
 D 		| 2		| BUTTON INPUT (Need pull-down)
*/

/* Priority definitions for most of the tasks in the demo application.  Some
tasks just use the idle priority. */
#define mainCHECK_TASK_PRIORITY			( tskIDLE_PRIORITY + 3 )

/* Baud rate used by the serial port tasks. */
#define UART_BAUD_RATE			( ( unsigned portLONG ) 9600 )

/* define CPU frequency in Hz here if not defined in Makefile */ //Corrected by Hector, this is in Hz not MHz
#ifndef F_CPU
#define F_CPU 12000000UL //16000000UL
#endif

/* LED that is toggled by the check task.  The check task periodically checks
that all the other tasks are operating without error.  If no errors are found
the LED is toggled.  If an error is found at any time the LED is never toggles
again. */
#define mainCHECK_TASK_LED				( 7 )

/* The period between executions of the check task. */
#define mainCHECK_PERIOD				( ( portTickType ) 3000 / portTICK_RATE_MS  )

/*
 * Function prototypes, see below.
 */
static void serialWriter( void *pvParameters );
static void vErrorChecks( void *pvParameters );
static void prvCheckOtherTasksAreStillRunning( void );
void vApplicationIdleHook( void );


//Prototypes for Cycle function.
static void Cycle();
static void vButton();

/*
0	-	Red
1	-	RedYellow
2	-	Green
3	-	Yellow
*/
short State = 0;

/*
 * status variables.
 */
static portBASE_TYPE serialWriterStatus = pdTRUE;
static xQueueHandle xRxedChars;
void Setup (void)
{
    //Sets Port A to Output. This controlls the lights.
    DDRA = 0xff;
    PORTA = 0x00;
	//DDRA = 0xff;
	State = 0;
	
    DDRD = 0x00; //Input and interupt
	PORTD = 0x00;
	
	EICRA |= _BV(ISC01) | _BV(ISC00); //Rising edge INT0
	EIMSK |= _BV(INT0); //Enable int0 interrupt
	sei(); //Enable Interrupts
	
}

ISR(INT0_vect){ //Catch interrupt 0
    signed portCHAR prt;;
	prt = PORTA;
    xQueueSendFromISR( xRxedChars, &prt, pdFALSE); //Post value of PORTA to queue
}

portSHORT main( void ) {
	Setup();
  /* Setup the LED's for output. */
  statusInitialize();
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
  sei();

  /* Create the tasks defined within this file. */
  xTaskCreate( vErrorChecks, ( signed portCHAR * ) "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );
  xTaskCreate( Cycle, ( signed portCHAR * ) "Move", configMINIMAL_STACK_SIZE, "Move", mainCHECK_TASK_PRIORITY, NULL);
  xTaskCreate( vButton, ( signed portCHAR * ) "Stop", configMINIMAL_STACK_SIZE, "Stop", mainCHECK_TASK_PRIORITY, NULL);
  uart_puts_P("\r\nFreeRTOS template initialized.");

  vTaskStartScheduler();
  return 0;
}

static void Cycle()
{
	for(;;) {
		switch(State)
		{
			case 0: //Red
			State = 1; //Green
			PORTA = Red;
			vTaskDelay(5000);
			break;
			case 1: //RedYellow
			State = 2;
			PORTA = RedYellow;
			vTaskDelay(1000);
			break;
			case 2: //Green
			State = 2;
			PORTA = Green;
			vTaskDelay(2000);
			break;
			case 3: //Yellow 
			State = 0;
			PORTA = Yellow;
			vTaskDelay(1000);
		}
		
	}
}


static void vButton()
{
	    // We must take care enabelling interupts as we dont know what else is going on.
	portENTER_CRITICAL();// Create queue and enable interupts. Dont want anything getting in the way
        {
		xRxedChars = xQueueCreate( 4, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
        //MCUCR = MCUCR | 0x03;
		//GICR = GICR | 0x40; // Enable interupt 0
		//EIMSK |= _BV(INT0); /* Enable *//*
        }
	portEXIT_CRITICAL();
	
	    for(;;){
        unsigned char store;

        if( xQueueReceive( xRxedChars, &store, 250 * 5 ) ){ // 5 second time out
			
			if(PORTA == Green){
			State = 3;
			vTaskDelay(100);
			}
			
        }
        else	{ //error or time out.
            vTaskDelay(100); // Delay for half the flash period then turn the LED on.
        }
    }

	
}


static void vErrorChecks( void *pvParameters ) {
  /* The parameters are not used. */
  ( void ) pvParameters;

  /* Cycle for ever, delaying then checking all the other tasks are still
     operating without error. */
  for( ;; ) {
    vTaskDelay( mainCHECK_PERIOD );
    prvCheckOtherTasksAreStillRunning();
  }
}

static void serialWriter( void *pvParameters ) {
  char* message = pvParameters;
  for (;;) {
	vTaskDelay((portTickType) 100/portTICK_RATE_MS);
	/* Notify a single error. */
	if (uart_puts_P("\r\n") != pdTRUE)
	  serialWriterStatus = pdFALSE;
	if (uart_puts(message) != pdTRUE)
	  serialWriterStatus = pdFALSE;
  }
}

static void prvCheckOtherTasksAreStillRunning( void ) {
  static portBASE_TYPE xErrorHasOccurred = pdFALSE;
  //if( xCheckSerialTasks() != pdTRUE ) {
  //  xErrorHasOccurred = pdTRUE;
  //}

  if( xErrorHasOccurred == pdFALSE ) {
    /* Toggle the LED if everything is okay so we know if an error occurs even if not
       using console IO. */
    statusToggleLED( mainCHECK_TASK_LED );
    uart_puts_P("\r\nAll tasks working.");
  }
}

void vApplicationIdleHook( void ) {
  vCoRoutineSchedule();
}

