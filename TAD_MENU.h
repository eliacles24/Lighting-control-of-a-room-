#ifndef TAD_MENU
#define	TAD_MENU

#include <xc.h>
#include "pic18f4321.h"

#include "TAD_TIMER.h"
#include "TAD_SIO.h"
#include "TAD_LEDS.h"
#include "TAD_USERS.h"

#define TRUE 1
#define FALSE 0

#define FUNCIO1 
#define FUNCIO2 
#define FUNCIO3 

/**************************************************
 *          PRIVATE FUNCTIONS
 *************************************************/

/**************************************************
 *          PUBLIC FUNCTIONS
 *************************************************/
void init_SysTime(void);
void Menu_Motor(void);
void setMenuState(unsigned char state);
void setState_One(void);
void updateSysTime(void);

unsigned char getMinut_0(void);
unsigned char getMinut_1(void);
unsigned char getHora_0(void);
unsigned char getHora_1(void);

#endif