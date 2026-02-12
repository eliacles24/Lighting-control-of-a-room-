#include "TAD_USERS.h"

//---------------------------------
//Data structures declaration START
//---------------------------------

typedef unsigned char UserId[10];

typedef UserId UserIDS[N_USERS];

unsigned char stringID[] = "00-00-00-00-00\r\n";
unsigned char ledString[] = "L0: 0 - L1: 0 - L2: 0 - L3: 0 - L4: 0 - L5: 0\0";

unsigned char stateUsers;
unsigned char matchedUser;
unsigned char goBackTo;

// The User struct stores the index of the user's card ID in userIDS, and their current light configuration
typedef struct {	
	unsigned char Lights[6];
} User;

//---------------------------------
//Data structures declaration END
//---------------------------------



//------------------------------------------------------
//Constant values and global variables declaration START
//------------------------------------------------------

//Flash table containing the ID of every card
UserIDS userIDS = {
	{'A', 'F', '6', '5', 'B', 'A', '1', 'C', '6', 'C'},	//User 0 card codde
	{'A', 'F', '5', '3', 'C', '5', '1', 'F', '2', '6'},	 //User 1 card taur
	{'B', 'D', 'B', '7', 'D', '9', '2', '1', 'F', '2'}, //User 2 card tv
    {'6', '3', 'E', 'C', 'F', 'C', '1', 'A', '6', '9'}, //User 3 pin rainbow
    {'6', '4', '6', 'B', '4', '3', 'A', '3', 'E', 'F'}, //User 4 pin flower
    {'E', 'E', '9', '8', '4', '3', '6', 'E', '5', '4'}, //Tarjeta UAB invalida
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'},
    {'B', 'C', 'B', '7', 'C', '9', '2', '1', 'B', '5'}
};


unsigned char currentUserRoom;          //When no user is in the room, we set current user to N_USERS, which is the first index out of bounds of userIDS
User users[N_USERS];					//This is the array that stores all users 

//------------------------------------------------------
//Constant values and global variables declaration END
//------------------------------------------------------


//local functions declaration

void init_Users() 
{
    
	for (unsigned char i=0; i < N_USERS; i++){
		
		for (unsigned char j = 0; j < N_LEDS; j++){	//Set LEDS to all on by default
			users[i].Lights[j] = 0;	
		}
	}
    
    currentUserRoom = N_USERS;
    matchedUser = N_USERS;
    stateUsers = 0;
    goBackTo = 0;
}

char findUser(unsigned char ID[])
{
    static unsigned char stateFind = 0;
    static unsigned char nUser = 0;
    static unsigned char nId = 0;
    
    switch(stateFind){
        case 0:
            nUser = 0;
            nId = 0;
            stateFind++;
            break;
        case 1:
            if (ID[nId] == userIDS[nUser][nId]) {
                nId++;
                
                if (nId == 10) {
                    stateFind = 0;
                    return nUser;
                }
                
            }
            else
                stateFind++;
        break;
        case 2:
            nUser++;
            nId = 0;
            
            if (nUser >= N_USERS) {
                stateFind = 0;
                return N_USERS;
            }
            else
                stateFind--;
            break;
    }

    return (N_USERS +1);
}


//This function receives a Light to change and the value to update the light, and updates it for the current user
void setLightPreference(unsigned char value) {
    static unsigned char functionality = 0;            //0 registering led, 1 registering intensity
    static unsigned char led;
    
    if (roomHasUser()) {
        switch (functionality) {
            case 0:                             //what led
                if (value >= 0 && value <= 5) {
                    led = value;
                    functionality = 1;
                }
                break;
            case 1:                             //set new intensity of the LED
                users[getCurrentUser()].Lights[led] = value;
                functionality = 0;
                break;
        }
        
    }
    else {
        functionality = 0;
    }
    
}

void setCurrentUser(unsigned char user)
{
    if (currentUserRoom == N_USERS)
        currentUserRoom = user;
    else
        if (currentUserRoom == user)
            currentUserRoom = N_USERS;
        else
            currentUserRoom = user;
}

unsigned char getCurrentUser(void)
{
    return currentUserRoom;
}

unsigned char roomHasUser(void)
{
    return (currentUserRoom != N_USERS);
}


unsigned char printUserId(unsigned char user)
{
    matchedUser = user;
    stateUsers = 19;
    goBackTo = 23;
    
}

unsigned char getLedConfigFromCurrentUser(unsigned char led, unsigned char call){
    if (call == 0)
        return (users[currentUserRoom].Lights[led] * 4);
    if (call == 1 && users[currentUserRoom].Lights[led] == 10)
        return 'A' + (users[currentUserRoom].Lights[led] -10);
    else
        return '0' + users[currentUserRoom].Lights[led];
}

//void resetUserSettings() 
//{
//    static unsigned char nUser = 0;
//    
//    users[nUser].Lights[0] = 0;
//    users[nUser].Lights[1] = 0;
//    users[nUser].Lights[2] = 0;
//    users[nUser].Lights[3] = 0;
//    users[nUser].Lights[4] = 0;
//    users[nUser].Lights[5] = 0;
//    
//    nUser++;
//    
//    if (nUser == N_USERS) {
//        setCurrentUser(N_USERS);
//        setStateKB(0);
//        nUser = 0;
//    }
//    
//}

unsigned char receivedID(unsigned char ID[])
{
    matchedUser = findUser(ID);

    if (matchedUser <= N_USERS)
        return 1;
    else
        return 0;
}

void motorUsers()
{
    static unsigned char count0;
    static unsigned char count1;
    static unsigned char count2;
    static unsigned char prov;
    
    switch (stateUsers)
    {
        case 0:
            count0 = 0;
            count1 = 0;
            count2 = 0;
            goBackTo = 0;
            matchedUser = N_USERS;
            break;
            
        /***********************************************************************
         *************************** RECIEVE ID ********************************
         **********************************************************************
         *  i = counter to avoid the RFID to read while it's still processing
         *  goBackTo = marker to where the function print user needs to go back to
         */
       
        case 1:
            count0 = 0;
            stateUsers++;
            break;
        case 2:
            if (matchedUser != N_USERS)
                stateUsers++;
            else
                stateUsers = 9;
            break;
        case 3:
            setCurrentUser(matchedUser);
            stateUsers++;
            break;
        case 4:
            
            if (SIO_PutString("\r\nCard detected!\r\n\tUID: ") == 1) {
                stateUsers++;
            }
            break;
        case 5:                     //print UID
            goBackTo = 6;
            stateUsers = 20;
            break;
        case 6:
            if (SIO_PutString("\r\t") == 1) {
                stateUsers++;
            }
        case 7:                     //print LEDS
            goBackTo = 8;
            stateUsers = 29;
            break;
        case 8:
            if (SIO_PutString("\r\n\n") == 1) {
                stateUsers = 10;
                count0 = 0;
            }
            break;
        case 9:
                if (SIO_PutString("\n\rInvalid\n\r") == 1)
                    stateUsers++;
            break;
        case 10:
                stateUsers = 0;
            break;
        /***********************************************************************
         *********************** PRINT USER ID *********************************
         ***********************************************************************
         * count0 = characters put of the id
         * count1 = position in the string to print
         * count2 = counter to skip ' - '
         * goBackTo = marker to where the function print user needs to go back to
         ************************************************************************/
        case 19:
            if (SIO_PutString("\r\n\n") == 1)
                stateUsers++;
            break;
        case 20:        //state for needed resets
            count0 = 0;
            count1 = 0;
            count2 = 0;
            stateUsers++;
            break;
        case 21:
            if (count2 < 2) {
                stringID[count1] = userIDS[matchedUser][count0];
                count0++;
                count2++;
            }
            else {
                count2 = 0;
            }
            
            count1++;
            
            if (count0 >= 10 || count1 >= 44)
                stateUsers++;
            break;
        case 22:
            if (SIO_PutString(stringID) == 1) {
                stateUsers = goBackTo;
                count0 = 0;
                setMenuState(0);
            }
            break;
        case 23:
            if (SIO_PutString("\r\n") == 1)
                stateUsers = 0;
            break;
            
        /***********************************************************************
         *********************** PRINT LEDS CONFIG *****************************
         ***********************************************************************
         * count0 = counter of leds printed
         * count1 = counter of the position in the string
         * goBackTo = marker to what state called it and needs to return to
         ************************************************************************/
        case 29:            //for printing current user's info
            
            
            if (currentUserRoom != N_USERS) matchedUser = currentUserRoom;
//            if (matchedUser == 1){
//                LED2=1;
//                LED3=0;
//                LED4=0;
//            } else {
//                LED2=0;
//                LED4=0;
//                if (matchedUser == 2){
//                    LED3=1;
//                } else {
//                    LED2=0;
//                    LED3=0;
//                    if (matchedUser == 0){
//                        LED4=1;
//                    }
//                }
//            }
            
            
            
            
            stateUsers++;
            break;
        case 30:            //Set values
            count0 = 0;
            stateUsers++;
            break;
        case 31:
            count1 = 4;
            stateUsers++;
            break;
        case 32:
            
            if (users[matchedUser].Lights[count0] == 10)
                ledString[count1] = 'A';
            else
                ledString[count1] = '0' + users[matchedUser].Lights[count0];
        
            count1 += 8;
            count0++;
            
            if (count0 == 6)
                stateUsers++;
        break;
        case 33:
            if (goBackTo == 45) stateUsers++;
            else{
                if(SIO_PutString("|")) stateUsers++;
            }
        case 34:
            if (SIO_PutString(ledString) == 1) {
                stateUsers = goBackTo;
            }
            break;
        
        /***********************************************************************
         ********************** PRINT USERS SETTINGS ***************************
         ***********************************************************************
         * i = counter of users explored
         * count1 = counter of the position in the string
         * goBackTo = marker to what state called it and needs to return to
         ************************************************************************/
        case 40:
            goBackTo = 0;
            stateUsers++;
            break;
        case 41:
            matchedUser = 0;
            stateUsers++;
            break;
        case 42:
            if (SIO_PutString("\r\n\n") == 1) {
                if (matchedUser >= N_USERS) {
                    stateUsers = 0;
                    setMenuState(0);
                }
                else
                    stateUsers++;
            }
            break;
        case 43:            //print ID
            goBackTo = 44;
            stateUsers = 20;
            break;
        case 44:            //print LEDS
            goBackTo = 45;
            stateUsers = 30;
            break;
        case 45:
            matchedUser++;
            stateUsers = 42;
            break;
            
        /***********************************************************************
         ********************** RESET USERS SETTINGS ***************************
         ***********************************************************************
         * i = counter of users explored
         * count1 = counter of the position in the string
         * goBackTo = marker to what state called it and needs to return to
         ************************************************************************/
        case 50:
            count0 = 0;
            stateUsers++;
            break;
        case 51:
            users[count0].Lights[0] = 0;
            users[count0].Lights[1] = 0;
            users[count0].Lights[2] = 0;
            users[count0].Lights[3] = 0;
            users[count0].Lights[4] = 0;
            users[count0].Lights[5] = 0;
    
            count0++;
            
            if (count0 == N_USERS)
                stateUsers++;
            break;
        case 52:
            setCurrentUser(N_USERS);
            stateUsers = 0;
            break;
    }
}

unsigned char getCurrentUserLastLetter(void){
    return userIDS[currentUserRoom][9];
}

void setStateUsers(unsigned char state)
{
    stateUsers = state;
}