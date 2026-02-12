#include <xc.h>
#include "pic18f4321.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG	
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

#include "TAD_KEYBOARD.h"
#include "TAD_LCD.h"
#include "TAD_LEDS.h"
#include "TAD_MENU.h"
#include "TAD_RFID.h"
#include "TAD_SIO.h"
#include "TAD_TIMER.h"
#include "TAD_USERS.h"

void __interrupt(high_priority) MyRSI(void){
    if (TMR0IF == 1){
        RSI_Timer0();
    }
} 

void main(void) {
    //Initialise ports    
    TI_Init();
    ADCON1 = 0x0F; 
    init_TAD_KEYBOARD();
    init_Users();
    initEusart();
    init_LEDS();
    initRFID();
    init_SysTime();
    LcInit(2, 16);
    TRISDbits.TRISD6 = 0;
   
    
    //Leds, LCD, RFID, KB, Menu
    //Can we update time in TMR0 RSI?
    while (1){
        keyboard_motor();        
        motorUsers();
        Menu_Motor();
        updateSysTime();
        motorRFID();
		motor_LEDS();
        motorLCD();
        //LED0= !LED0;
        LATDbits.LATD6 = !LATDbits.LATD6;
    }

}