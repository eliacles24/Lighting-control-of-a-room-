#ifndef TAD_USERS
#define	TAD_USERS

#include <xc.h> 
#include "pic18f4321.h"

#include "TAD_RFID.h"
#include "TAD_SIO.h"
#include "TAD_KEYBOARD.h"

#define N_USERS 15
#define N_LEDS 6

/**************************************************
 *          PRIVATE FUNCTIONS
 *************************************************/

char findUser(unsigned char ID[]);

unsigned char roomHasUser(void);
//Post: Checks that there is a user in the room


/**************************************************
 *          PUBLIC FUNCTIONS
 *************************************************/
 
void init_Users(void);
//Post: Initializes the users id, and sets the light prefernces to 100%. Precodition for the rest of the functions

void setLightPreference(unsigned char value);
//Pre: It must be checked that there is a user in the room (roomHasUser() returns TRUE)
//Post: Will change the setting of one of the lights in the current user

unsigned char getCurrentUser(void);
//Post: Returns the id of the user in the room, or N_USERS if the room is empty

unsigned char printUserId(unsigned char user);

void motorUsers();

unsigned char getLedConfigFromCurrentUser(unsigned char led, unsigned char call);

//void resetUserSettings();
//Post: All the desired led settings for the users will be deleted, and if there's a user in the room they will leave to turn off the lights

//unsigned char printLightInfo();

unsigned char getCurrentUserLastLetter(void);

unsigned char receivedID(unsigned char ID[]);

void setStateUsers(unsigned char state);

#endif

