/* Based on FreeRTOS ParTest.h */

#ifndef STATUS_STATUS_H
#define STATUS_STATUS_H 1

void statusInitialize( void );
void statusSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue );
void statusToggleLED( unsigned portBASE_TYPE uxLED );

#endif /* STATUS_STATUS_H */
