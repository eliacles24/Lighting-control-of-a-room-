#ifndef TAD_LEDS
#define	TAD_LEDS

#include <xc.h>
#include "pic18f4321.h"

#include "TAD_TIMER.h"
#include "TAD_USERS.h"

#define LED0 LATEbits.LATE1
#define LED1 LATEbits.LATE2
#define LED2 LATEbits.LATE0

#define LED3 LATAbits.LATA3
#define LED4 LATAbits.LATA4
#define LED5 LATAbits.LATA5

/**************************************************
 *          PRIVATE FUNCTIONS
 *************************************************/
void resetLEDs(void);
void setLEDs(void);


/**************************************************
 *          PUBLIC FUNCTIONS
 *************************************************/
void init_LEDS(void);
void motor_LEDS(void);

#endif	