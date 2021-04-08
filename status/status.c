/* Based on FreeRTOS ParTest.c */
#include "FreeRTOS.h"
#include "task.h"
#include "status.h"

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

#define partstALL_BITS_OUTPUT			( ( unsigned portCHAR ) 0xff )
#define partstALL_OUTPUTS_OFF			( ( unsigned portCHAR ) 0xff )
#define partstMAX_OUTPUT_LED			( ( unsigned portCHAR ) 7 )

static volatile unsigned portCHAR ucCurrentOutputValue = partstALL_OUTPUTS_OFF; /*lint !e956 File scope parameters okay here. */

/*-----------------------------------------------------------*/

void statusInitialize( void )
{
  ucCurrentOutputValue = partstALL_OUTPUTS_OFF;

  /* Set port B direction to outputs.  Start with all output off. */
  DDRB = partstALL_BITS_OUTPUT;
  PORTB = ucCurrentOutputValue;
}
/*-----------------------------------------------------------*/

void statusSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue ) {
  unsigned portCHAR ucBit = ( unsigned portCHAR ) 1;

  if( uxLED <= partstMAX_OUTPUT_LED ) {
	ucBit <<= uxLED;
  }	

  vTaskSuspendAll();
  {
	if( xValue == pdTRUE ) {
	  ucBit ^= ( unsigned portCHAR ) 0xff;
	  ucCurrentOutputValue &= ucBit;
	} else {
	  ucCurrentOutputValue |= ucBit;
	}
	PORTB = ucCurrentOutputValue;
  }
  xTaskResumeAll();
}
/*-----------------------------------------------------------*/

void statusToggleLED( unsigned portBASE_TYPE uxLED ) {
  unsigned portCHAR ucBit;

  if( uxLED <= partstMAX_OUTPUT_LED ) {
	ucBit = ( ( unsigned portCHAR ) 1 ) << uxLED;

	vTaskSuspendAll();
	{
	  if( ucCurrentOutputValue & ucBit ) {
		ucCurrentOutputValue &= ~ucBit;
	  } else {
		ucCurrentOutputValue |= ucBit;
	  }

	  PORTB = ucCurrentOutputValue;
	}
	xTaskResumeAll();			
  }
}


