#include "TAD_MENU.h"


//Using const we store it in flash table.
const unsigned char menuText[] = "---------------\r\nMain Menu\r\n"
"---------------\r\nChoose an option:\r\n\n\t1. Who is in the room?\r\n"
"\t2. Show settings\r\n\t3. Change system time\r\n\nOption: ";

unsigned char systemTimer = 0;

unsigned char Time[4];  //LSB is minutes, MSB is Hours

unsigned char stateMenu = 100;    //Stores the current state of the motor

unsigned char aux;

void init_SysTime(){
    TI_NewTimer(&systemTimer);
    Time[0] = '0';
    Time[1] = '0';
    Time[2] = '0';
    Time[3] = '0';
    aux = 0;
}


/* The Menu module is in charge of the input/output to the computer. What it needs to do:
 * Polling of the 'Esc' key.
 * If 'Esc' is received or '#' is pressed in the KB for less than 3 seconds (use setter method to set state):
 *    Send the menu to the computer
 *    Polling of the chosen option (1, 2, or 3).
 *    Execute the chosen option.
 */
void Menu_Motor(void)
{
    unsigned char chosenOption = 9;     //Set to invalid value when function is called (just in case).
    static unsigned char timePos = 4;
	
    switch(stateMenu) {
        case 0: //Polling of 'Esc'
            if (SIO_RXAvail() == TRUE){         //Check if we can read
                if (SIO_GetChar() == 0x1B){     //Check if the input is 'Esc' (0x1B Ascii value)
                    stateMenu++;                  //We must enter Menu functionality
                } 
            }
            break;
            
        case 1:                         //Send menu to computer. If we finished printing the menu, state=2
            if (SIO_PutString(menuText) == 1){
                stateMenu++;
            }
            break;
            
        case 2:
            //Polling of chosen option and calling the function to execute it
            if (SIO_RXAvail() == TRUE){
                chosenOption = SIO_GetChar();
                
                if (chosenOption == 0x31){  //ascii for 1
                    //chosenOption = 5; 
                    SIO_PutChar('1');
                    stateMenu++;
                    break;
                }
                else if (chosenOption == 0x32){
                    //chosenOption = 6;  
                    SIO_PutChar('2');
                    stateMenu=4;
                    break;
                }
                else if (chosenOption == 0x33){
                    //chosenOption = 7;
                    SIO_PutChar('3');
                    stateMenu=5;
                    break;
                }
                else {
                    stateMenu = 7;
                }
            }

            break;
            
        case 3:         //1. WHO IS IN THE ROOM
            if (roomHasUser() ){
                printUserId(getCurrentUser());
                stateMenu = 10;
                chosenOption = 9;
            } else {
                if (SIO_PutString("\r\nNobody in the room\r\n\n") == 1){
                    stateMenu = 0;
                    chosenOption = 9;
                }
            }
            break;
            
        case 4:                     //2. SHOW SETTINGS
            setStateUsers(40);
            stateMenu = 10;
            chosenOption = 9;
            break;
            
        case 5:         //3. CHANGE SYSTEM TIME
            if (SIO_PutString(" ") == 1)
                stateMenu++;
            break;
        case 6:
            if (SIO_RXAvail() == TRUE){
                chosenOption = SIO_GetChar();
                SIO_PutChar(chosenOption);
                
                if (chosenOption != 0x3A) {
                    timePos--;
                    Time[timePos] = chosenOption;
                }
                
                if (timePos == 0) {
                    timePos = 4;
                    stateMenu++;
                }
				
				chosenOption = 9;
                
            }
            
            break;
        case 7:
            if (SIO_PutString("\r\n\n") == 1)
                stateMenu = 0;
            break;
        case 10:        //waiting state for other TADs to stop using SIO
            break;
        case 100:
            //if(SIO_PutString("Pau i Elia\n\r"))
            stateMenu = 1;
            break;
    }
}

void setMenuState(unsigned char state)
{
    stateMenu = state;
}

void setState_One(void)
{
    stateMenu = 1;
}

void updateSysTime()
{
    
    //30k ticks = 60k ms = 60s = 1 minute
    if (TI_GetTics(systemTimer) >= 30000){      
        TI_ResetTics(systemTimer);  //Start counting the next minute
        
        aux++;
        
        if (aux == 4){
            aux = 0;
            
            Time[0]++;

            if (Time[0] == '9'+1){
                Time[0] = '0';
                Time[1]++;
            }
            if (Time[1] == '6'){
                Time[1] = '0';
                Time[2]++;
            }
            if (Time[2] == '9'+1){
                Time[2] = '0';
                Time[3]++;
            }
            if (Time[3] == '2' && Time[2] == '4' ){
                Time[3] = '0';
                Time[2] = '0';
            }
        }
        
        
        
        
        
        //If minutes reach 60, reset to 0. If hours reach 24, reset to 0. 
    }
    
}

unsigned char getMinut_0(){         //minut -
    return Time[0];
}
unsigned char getMinut_1(){         //minut +
    return Time[1];
}
unsigned char getHora_0(){          //hora -
    return Time[2];
}
unsigned char getHora_1(){          //hora +
    return Time[3];
}