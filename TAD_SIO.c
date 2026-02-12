#include "TAD_SIO.h"

#define TRUE 1
#define FALSE 0

void initEusart (){
    
    RCSTAbits.SPEN = 1; 
    TRISCbits.RC6 = 1; 
    TRISCbits.RC7 = 1; 
    TXSTAbits.SYNC = 0; 
    TXSTAbits.TXEN = 1; 
    TXSTAbits.BRGH = 0; 
    BAUDCONbits.BRG16 = 0; 
    RCSTAbits.CREN = 1; 
    SPBRG = 64; 
        
}

unsigned char SIO_RXAvail(void)
{
    return PIR1bits.RCIF;
}

unsigned char SIO_GetChar(void)
{
    //We read the character received
    return RCREG;
}

unsigned char SIO_TXAvail(void)
{
    return TXSTAbits.TRMT;
}

void SIO_PutChar(unsigned char value)
{
    TXREG = value;
}

char printString(unsigned char *s) {
    
    if (*s){
        if (SIO_TXAvail() ){
            SIO_PutChar(*s++);
        }
        return 0;   //Function hasn't finished yet
    }

    return 1;       //Function has finished printing
}


unsigned char SIO_PutString(char *s) {
    static char *a = NULL;
    
    if (a == NULL) a = s;   //Copy pointer to first character of input string in first call
    
    if (SIO_TXAvail() ){
        SIO_PutChar(*a++);  //If char was printed successfully, advance the pointer to next char
    }
    
    if (!(*a) || a == NULL || a == '\0'){
        a = NULL;
        return 1;           //Function has finished printing
    } 
    
    return 0;               //Function hasn't finished yet

}