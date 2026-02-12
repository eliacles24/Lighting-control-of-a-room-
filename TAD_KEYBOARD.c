#include "TAD_KEYBOARD.h"

void sweepRows(void);
void debounceKey(void);
unsigned char getKey(unsigned char key);

unsigned char stateKB;
unsigned char pressedKey;  //Arbitrary value that doesn't represent any key 
unsigned int timePressed;
unsigned char currentRow;
unsigned char nKeys;

unsigned char debounceTimer;
unsigned char keyTimer;


void init_TAD_KEYBOARD(void) 
{
    ADCON1 = 0x0F;

    //set direction of rows and columns
    TRISDbits.TRISD0 = 0;    
    TRISDbits.TRISD1 = 0;    
    TRISDbits.TRISD2 = 0;    
    TRISDbits.TRISD3 = 0;    
    TRISAbits.TRISA1 = 1;    
    TRISAbits.TRISA2 = 1;    
    TRISAbits.TRISA0 = 1;    
    
    
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    
    LATAbits.LATA3 = 0;
    LATAbits.LATA4 = 0;
    
    //Rows are 0 by default, so we set them
    Row0 = 0;
    Row1 = 0;
    Row2 = 0;
    Row3 = 0;
	
	//Timers initialization
	TI_NewTimer(&debounceTimer);
	TI_NewTimer(&keyTimer);
    
    //init variables
    pressedKey =20;
    currentRow =0;
    stateKB = 0;
    timePressed = 0;
    currentRow = 0;
    nKeys = 0;
}


/* state 0: sweeping rows. Go to state 1 if we detect a key press
 * state 1: We have detected a key press. Debouncing routine (16ms). If the key isn't the same, go back to state 0 (sweeping) and detect the key being pressed now.
 * state 2: First checks if the key pressed has been changed to another (if so go to state 0 and detect the new key). Else, check if no key is currently pressed
 * state 3: Debouncing for letting go a key
 * state 4: We have the key pressed and how long it was pressed for. Now we call the adequate functions. Then we go to state 0.
*/
void keyboard_motor(void) 
{   
    
    switch (stateKB) {
        case 0:
            sweepRows();
			pressedKey = 20; //Reset key pressed
			timePressed = 0; //Reset time pressed
			
            //pressedKey = getKey(currentRow);
            //if (pressedKey != 20){
			if (Column0 || Column1 || Column2) {
                stateKB++;
				pressedKey = getKey(currentRow);
				TI_ResetTics (debounceTimer);
			}
            break;
			
		case 1:
            if (TI_GetTics (debounceTimer) > 64) {			//Wait debouncing time, 64 tics = 32ms
                
				if (pressedKey == getKey(currentRow) ) {	//Check key pressed hasn't changed
                    stateKB++;
					TI_ResetTics(keyTimer);
				} else { stateKB = 0; }						//If key pressed changed go to state 0 (detect it and debounce from 0)
			}	
			break;
			
		case 2:		
            
			if (!(Column0 || Column1 || Column2)) {			//Go check exit rebounds 
                stateKB++;
                TI_ResetTics(debounceTimer);
			}
			
			break;
			
        case 3:     //Debouncing exit rebound
            if (TI_GetTics (debounceTimer) > 64) {
                if ((getKey(currentRow) != pressedKey) && (!(Column0 || Column1 || Column2)) ) { //If key is no longer pressed save total time pressed and exit
                    timePressed = TI_GetTics(keyTimer);
                    stateKB++;
                } else {
                    stateKB = 2;    //If the key is still pressed go back to waiting for release
                }
            }
            break;
            
		case 4:
			if (pressedKey <= 10) { //send info to TAD_USERS
				setLightPreference(pressedKey);
			}
			if (pressedKey == 11 && timePressed < 6000) {   //6000 tics * 0.5ms = 3s
				//Activate the menu functionality
                setMenuState(1);
			}
			if (pressedKey == 11 && timePressed >= 6000) {
                setStateUsers(50);
			}
            stateKB = 0;
			break;
	}
}


void sweepRows(void) 
{
    currentRow++;
    
    if(currentRow == 4) currentRow = 0;

    switch(currentRow){
        case 0:
            Row3 = 0;
            Row0 = 1;
            break;
        case 1:
            Row0 = 0;
            Row1 = 1;

            break;
        case 2:
            Row1 = 0;
            Row2 = 1;
            break;
        case 3:
            Row2 = 0;
            Row3 = 1;
            break;
    }
    

}


unsigned char getKey(unsigned char row)
{
    switch (row){
        case 0:
            if(Column0 == 1){
                return 1;
            }
            if(Column1 == 1){
                return 2;
            }
            if(Column2 == 1){
                return 3;
            }
       
            break;
        case 1:
            if(Column0 == 1){    
                return 4;
            }
            if(Column1 == 1){
                return 5;
            }
            if(Column2 == 1){
                return 6;
            }
            break;
        case 2:
            if(Column0 == 1){
                return 7;
            }
            if(Column1 == 1){
                return 8;
            }
            if(Column2 == 1){
                return 9;
            }
            
        case 3:
            if(Column0 == 1){
                return 10;  //10 represents the * key
            }
            if(Column1 == 1){
                return 0;
            }
            if(Column2 == 1){
                return 11;  //11 represents the # key
            } 
            
    }
     
    return 20; //If 20 is returned, no key is pressed anymore (shouldnt happen)  
}

//void setStateKB(unsigned char state)
//{
//    stateKB = state;
//}