#include "TAD_LEDS.h"

unsigned char timer_leds;

void init_LEDS(void)
{
    TRISEbits.TRISE0 = 0;   //2  
    TRISEbits.TRISE1 = 0;   //0 
    TRISEbits.TRISE2 = 0;   //1 
    TRISAbits.TRISA3 = 0;   //3
    TRISAbits.TRISA4 = 0;   //4
    TRISAbits.TRISA5 = 0;   //5
    resetLEDs();
    TI_NewTimer(&timer_leds);
}

void resetLEDs(void)
{
    LED0 = 0;
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    LED5 = 0;
}

void setLEDs(void)
{
    LED0 = 1;
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
}

//The configuration is 0 - 10, and tics are every 0.5ms. 50Hz.
void motor_LEDS(){
    unsigned char currentTics = TI_GetTics(timer_leds);
    
    if (!roomHasUser()){
        resetLEDs();
    }
    else {
    
        if (currentTics >= 40){
            if (roomHasUser())
                setLEDs();
            TI_ResetTics(timer_leds);
            currentTics = 0;
        }
    
        
    if (currentTics >= (getLedConfigFromCurrentUser(0, 0)) ) {
        LED0 = 0;
    }
    if (currentTics >= (getLedConfigFromCurrentUser(1, 0)) ){
        LED1 = 0;
    }
    if (currentTics >= (getLedConfigFromCurrentUser(2, 0)) ){
        LED2 = 0;
    }
    if (currentTics >= (getLedConfigFromCurrentUser(3, 0)) ){
        LED3 = 0;
    }
    if (currentTics >= (getLedConfigFromCurrentUser(4, 0)) ){
        LED4 = 0;
    }
    if (currentTics >= (getLedConfigFromCurrentUser(5, 0) ) ){
        LED5 = 0;
    }
    
    }
}
