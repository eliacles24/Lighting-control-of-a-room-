#ifndef TAD_KEYBOARD
#define	TAD_KEYBOARD

#include <xc.h>
#include "pic18f4321.h"

#include "TAD_TIMER.h"
#include "TAD_LEDS.h"
#include "TAD_USERS.h"
#include "TAD_MENU.h"

#define Row0 LATDbits.LATD0
#define Row1 LATDbits.LATD1
#define Row2 LATDbits.LATD2
#define Row3 LATDbits.LATD3

#define Column0 PORTAbits.RA0
#define Column1 PORTAbits.RA1
#define Column2 PORTAbits.RA2

/**************************************************
 *          PRIVATE FUNCTIONS
 *************************************************/
void sweepRows(void);
unsigned char getKey(unsigned char);

/**************************************************
 *          PUBLIC FUNCTIONS
 *************************************************/

void init_TAD_KEYBOARD(void);
void keyboard_motor(void);
//void setStateKB(unsigned char state);

#endif