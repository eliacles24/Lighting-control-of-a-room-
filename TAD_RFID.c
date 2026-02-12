/*
 * File:   TRFID.c
 * Author: jnavarro & ester.vidana
 *
 * 
 * Inspired by: https://simplesoftmx.blogspot.com/2014/11/libreria-para-usar-lector-rfid-rc522.html
 */

#include "TAD_RFID.h"

unsigned char stateRFID = 0;
unsigned char timerRF;

//-------------- Private functions: --------------
void InitPortDirections(){
DIR_MFRC522_SO  = 1; 
DIR_MFRC522_SI  = 0; 
DIR_MFRC522_SCK = 0; 
DIR_MFRC522_CS  = 0; 
DIR_MFRC522_RST = 0;
}

void delay_us(char howMany){
#define NUM_US 16  //Number of instructions in 1 uS  
    char x;
    for (x=0; x<howMany*NUM_US;x ++) Nop();
}

unsigned char MFRC522_Rd(unsigned char Address) 
{
   unsigned int i, ucAddr;
   unsigned int ucResult = 0;
   MFRC522_SCK = 0;
   MFRC522_CS = 0;
   ucAddr = ( (Address<<1)&0x7E)|0x80;

   for (i = 8; i > 0; i--)
   {
      MFRC522_SI= ((ucAddr&0x80) == 0x80);
      MFRC522_SCK = 1;
      //delay_us(5);

      ucAddr <<= 1;
      MFRC522_SCK = 0;
      //delay_us(5);

   }

   for (i = 8; i > 0; i--)
   {
      MFRC522_SCK = 1;
      //delay_us(5);
      ucResult <<= 1;   
      ucResult|= MFRC522_SO;
      MFRC522_SCK = 0;
      //delay_us(5);
   }
  
   MFRC522_CS = 1;
   MFRC522_SCK = 1;
   return ucResult;
}


void MFRC522_Wr(unsigned char Address, unsigned char value)
{
  
   unsigned char i, ucAddr;
   MFRC522_SCK = 0;
   MFRC522_CS = 0;
   ucAddr = ( (Address<<1)&0x7E);
   for (i = 8; i > 0; i--)
   {
      MFRC522_SI= ( (ucAddr&0x80) == 0x80);
      MFRC522_SCK = 1;
      ucAddr <<= 1;
      //delay_us(5);
      MFRC522_SCK = 0;
      //delay_us(5);
   }

   for (i = 8; i > 0; i--)
   {
      MFRC522_SI = ( (value&0x80) == 0x80);
      MFRC522_SCK = 1;
      value <<= 1;
      //delay_us(5);
      MFRC522_SCK = 0;
      //delay_us(5);

   }

   MFRC522_CS = 1;
   MFRC522_SCK = 1;
}

void MFRC522_Reset()
{ 
   MFRC522_RST = 1;
   delay_us (1);
   MFRC522_RST = 0;
   delay_us (1);
   MFRC522_RST = 1;
   delay_us (1);
   MFRC522_Wr( COMMANDREG, PCD_RESETPHASE );
   delay_us (1);
}

void MFRC522_AntennaOn()
{                                              
    unsigned char stt;
    stt= MFRC522_Rd( TXCONTROLREG ) ;
    while (!(MFRC522_Set_BitCoop( TXCONTROLREG, 0x03 ))){}
}
void MFRC522_AntennaOff()
{
    while (!MFRC522_Clear_BitCoop( TXCONTROLREG, 0x03 )){}
}
void MFRC522_Init()     
{                 
    MFRC522_CS = 1; 
    MFRC522_RST = 1;
   
     MFRC522_Reset();       
     MFRC522_Wr( TMODEREG, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
     MFRC522_Wr( TPRESCALERREG, 0x3E ); //TModeReg[3..0] + TPrescalerReg
     MFRC522_Wr( TRELOADREGL, 30 );
     MFRC522_Wr( TRELOADREGH, 0 ); 
     MFRC522_Wr( TXAUTOREG, 0x40 );    //100%ASK
     MFRC522_Wr( MODEREG, 0x3D );      // CRC valor inicial de 0x6363
       
  
    MFRC522_AntennaOff() ;            
    MFRC522_AntennaOn();
     
    TI_NewTimer(&timerRF);
     
}

char MFRC522_SelectTag( char *serNum )
{
  char i;
  char _status;
  char size;
  unsigned recvBits;
  char buffer[9];
 
  
  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;
 
  for ( i=2; i < 7; i++ ) buffer[(int)i] = *serNum++;
  
  while (!(MFRC522_CRCCoop( buffer, 7, &buffer[7])));
 
  do {
      _status = MFRC522_ToCardCoop( PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits );
  } while (_status == -20);
  
  if ( (_status == MI_OK) && (recvBits == 0x18) ) size = buffer[0];
  else size = 0;
  return size;
}

unsigned char MFRC522_Clear_BitCoop( char addr, char mask )
{   static unsigned char  tmp = 0x0;
    static unsigned char sClear = 0;
    
    switch (sClear)
    {
        case 0:
            tmp = MFRC522_Rd(addr);
            sClear = 1;
            break;
        case 1:
            MFRC522_Wr( addr,  tmp&~mask);
            sClear = 0;
            return 1;
            break;
    }
    
    return 0;
    
}

unsigned char MFRC522_Set_BitCoop ( char addr, char mask )
{
    static unsigned char  tmp = 0x0;
    static unsigned char sSet = 0;
    
    switch(sSet)
    {
        case 0:
            tmp = MFRC522_Rd( addr ) ;
            sSet = 1;
            break;
        case 1:
            MFRC522_Wr( addr, tmp|mask );
            sSet = 0;
            return 1;
            break;
    }
     
    return 0;
}

unsigned char MFRC522_CRCCoop( char *dataIn, char length, char *dataOut )
{
    
    static char i;
    static char n;
    static unsigned char stateCRC = 0;
    
    switch (stateCRC)
    {
        case 0:
            if (MFRC522_Clear_BitCoop( DIVIRQREG, 0x04 ))
                stateCRC++;
            break;
        case 1:
            if (MFRC522_Set_BitCoop( FIFOLEVELREG, 0x80 )) {
                stateCRC++;
                i = 0;
            }
            break;
        case 2:
            MFRC522_Wr( FIFODATAREG, *dataIn++);
            i++;
            
            if (i >= length)
                stateCRC++;
            break;
        case 3:
            MFRC522_Wr( COMMANDREG, PCD_CALCCRC );
            stateCRC++;
            break;
        case 4:
            i = 0xFF;
            stateCRC++;
            break;
        case 5:
            n = MFRC522_Rd( DIVIRQREG );
            i--;
            if (!( i && !(n & 0x04) ))        //CRCIrq = 1
                stateCRC++;
            break;
        case 6:
            dataOut[0] = MFRC522_Rd( CRCRESULTREGL);
            stateCRC++;
            break;    
        case 7:
            dataOut[1] = MFRC522_Rd( CRCRESULTREGM);
            stateCRC = 0;
            return 1;
    }
    
    return 0;
}

char MFRC522_ToCardCoop( char command, char *sendData, char sendLen, char *backData, unsigned *backLen)
{
    
  static unsigned char statusCard = 0;
  
  static char _status = MI_ERR;
  static char irqEn = 0x77;
  static char waitIRq = 0x30;               
  static char lastBits;
  static char n;
  static unsigned i;
  
  static char serNumCheck = 0;
  
  static unsigned char  tmp = 0x0;
  
  switch (statusCard)
  {
      case 0:                       //DETECT which function is comming from
          if (sendLen == 1) {
              statusCard = IsCard_in;
          }
          else if (sendLen == 2)
              statusCard = ReadCard_in;
          else if (sendLen == 4)
              statusCard = Halt_in;
          else
              statusCard = toCardCoop;
          break;
          
      /***********************************************************************
       **************************** IS CARD **********************************
       ***********************************************************************/          
      case IsCard_in:
          MFRC522_Wr( BITFRAMINGREG, 0x07 );
          statusCard++;
          break;
      case 2:
          sendData[0] = PICC_REQIDL;
          backData[0] = PICC_REQIDL;
          statusCard = toCardCoop;
          break;
          
      /***********************************************************************
       ************************ READ CARD SERIAL *****************************
       ***********************************************************************/ 
          
      case ReadCard_in:                //FOR RESEST
          serNumCheck = 0;
          statusCard++;
          break;
      case 4:
            MFRC522_Wr( BITFRAMINGREG, 0x00 );                //TxLastBists = BitFramingReg[2..0]
            statusCard++;
          break;
      case 5:
            sendData[0] = PICC_ANTICOLL;
            sendData[1] = 0x20;
            
            backData[0] = PICC_ANTICOLL;
            backData[1] = 0x20;
            
            if (MFRC522_Clear_BitCoop( STATUS2REG, 0x08 ))
                statusCard = toCardCoop;
          break;
      /***********************************************************************
       ****************************** HALT ***********************************
       ***********************************************************************/ 
      case Halt_in:
          
        sendData[0] = PICC_HALT;
        sendData[1] = 0;
        
        backData[0] = PICC_HALT;
        backData[1] = 0;
        
        statusCard++;
        break;
        
      case 51:
          if (MFRC522_CRCCoop( sendData, 2, &sendData[2]))
            statusCard++;
          break;
      case 52:
          if (MFRC522_Clear_BitCoop( STATUS2REG, 0x80 ))
                statusCard = toCardCoop;
          break;
          
      /***********************************************************************
       *********************** TO CARD COOPERATIVE ***************************
       ***********************************************************************/
      case toCardCoop:
          MFRC522_Wr( COMMIENREG, irqEn | 0x80 );  //Interrupt request
          statusCard++;
          break;
      case 7:
          if (MFRC522_Clear_BitCoop( COMMIRQREG, 0x80 ))   //Clear all interrupt request bit
            statusCard++;
          break;
      case 8:
          if (MFRC522_Set_BitCoop( FIFOLEVELREG, 0x80 ))   //FlushBuffer=1, FIFO Initialization
            statusCard++;
          break;
      case 9:
          MFRC522_Wr( COMMANDREG, PCD_IDLE );      //NO action; Cancel the current command???
          i = 0;
          statusCard++;
          break;
      case 10:
          
            MFRC522_Wr( FIFODATAREG, sendData[i] );
            i++;
          
          if (i >= sendLen)
                statusCard++;
          break;    
      case 11:
          MFRC522_Wr( COMMANDREG, command );
          if (command == PCD_TRANSCEIVE )
            statusCard++;
          else
              statusCard = 13;
          break;    
      case 12:
          if (MFRC522_Set_BitCoop( BITFRAMINGREG, 0x80 ))
            statusCard++;
          break;
      case 13:
          i = 0xFFFF;
          statusCard++;
          break;
      case 14:
          n = MFRC522_Rd( COMMIRQREG );
          i--;
          
          if (!( i && !(n & 0x01) && !( n & waitIRq ) ))
            statusCard++;
          break;
      case 15:
          if (MFRC522_Clear_BitCoop( BITFRAMINGREG, 0x80 ))
              statusCard = 100;
          break;
      case 100:
          
          if (i!= 0 && !( MFRC522_Rd( ERRORREG ) & 0x1B ))
              if ( n & irqEn & 0x01 )
                statusCard = 17;
              else
                  statusCard = 16;
          else if (i != 0)
              statusCard = 23;
          else
              statusCard = 24;
          
          break;
      case 16:
          _status = MI_OK;
          
          if ( command == PCD_TRANSCEIVE )
                statusCard = 18;
          else
              statusCard = 24;
          break;
      case 17:
          _status = MI_NOTAGERR;
          
          if (command == PCD_TRANSCEIVE )
                statusCard++;
          else
              statusCard = 24;
          
          break;
      case 18:
          n = MFRC522_Rd( FIFOLEVELREG );
          statusCard++;
          break;
      case 19:
          lastBits = MFRC522_Rd( CONTROLREG ) & 0x07;
          statusCard++;
          break;    
      case 20:
          if (lastBits) *backLen = (n-1) * 8 + lastBits;
          else *backLen = n * 8;
                        
          if (n == 0) n = 1;
          else if (n > 16) n = 16;
          
          i = 0;
          statusCard++;
          break;
      case 21:
          backData[i] = MFRC522_Rd( FIFODATAREG );
          i++;
          
          if (i >= n)
              statusCard++;
          break;
      case 22:
          backData[i] = 0;
          statusCard = 24;
          break;    
      case 23:
          _status = MI_ERR;
          statusCard++;
          break;
          
      case 24:

            if (sendLen == 1) {
                statusCard = IsCard_out;
            } else if (sendLen == 2)
                statusCard = ReadCard_out;
            else if (sendLen == 4)
                statusCard = Halt_out;
            else
                statusCard = 30;
            
          break;
          
      /***********************************************************************
       ***************************** IS CARD ***********************************
       ***********************************************************************/            
      case IsCard_out:
          
            if ( (_status != MI_OK) || (*backLen != 0x10) )
                _status = MI_ERR;
            if (_status == MI_OK)
                stateRFID = 2;
            
          statusCard = 0;
          break;
          
      /***********************************************************************
       ************************ READ CARD SERIAL *****************************
       ***********************************************************************/
      case ReadCard_out:
          i = 0;
          if (_status == MI_OK)
              statusCard++;
          else 
              statusCard = 28;
            
          break;
      case 27:
          serNumCheck ^= backData[(int)i];
          i++;
          
          if (i >= 4)
              statusCard++;

          break;
      case 28:
          
          if ( serNumCheck != backData[4] ) 
              _status = MI_ERR;
          
          backData[5] = 0;
  
            if (_status == MI_OK) {
                stateRFID = 3;
            } else
                stateRFID = 100;
          
          statusCard = 0;
          break;
      /***********************************************************************
       ****************************** HALT ***********************************
       ***********************************************************************/          
      case Halt_out:
          if (MFRC522_Clear_BitCoop( STATUS2REG, 0x08 )) {
            stateRFID = 101;
            statusCard = 0;
          }
          break;
  
//DEFAULT
      case 30:
          statusCard = 0;
          return _status;
          break;
  }
  
  return -20;
}


//-------------- Public functions: --------------
void initRFID(){
    InitPortDirections();
    MFRC522_Init(); 
}


void motorRFID()
{
  static unsigned char UID[16];
  static unsigned char ID[10];
  
  static char TagType;
  static unsigned backBits;
  static unsigned unLen;
  static char buff[4];
  
  static unsigned char nId = 0;
  static unsigned char variable = 0;
  
  switch(stateRFID)
  {
      /***********************************************************************
       **************************** IS CARD **********************************
       ***********************************************************************/
      case 0:
          nId = 0;
          MFRC522_ToCardCoop(PCD_TRANSCEIVE, &TagType, 1, &TagType, &backBits);
          break;

      /***********************************************************************
       ************************ READ CARD SERIAL *****************************
       ***********************************************************************/          
      case 2:
          MFRC522_ToCardCoop(PCD_TRANSCEIVE, (char*) UID, 2, (char*) UID, &unLen);
          break;
          break;
      case 3:
          if (nId > 4) {
              stateRFID = 7;
              nId = 0;
          }
          else {
              variable = UID[nId];
              if (variable > 16)
                stateRFID++;
              else
                stateRFID = 6;
          }
          break;
      case 4:
          if (variable > 16)
              variable -= 16;
          else 
              stateRFID++;
          break;
      case 5:
          
          if (((UID[nId] - variable) / 16) >= 10)
              ID[nId*2] = 'A' + ((UID[nId] - variable) / 16) - 10;
          else
            ID[nId*2] = '0' + ((UID[nId] - variable) / 16);
          
          if (variable >= 10)
              ID[(nId*2)+1] = 'A' + (variable - 10);
          else
            ID[(nId*2)+1] = '0' + variable;
          
          nId++;
          stateRFID = 3;
          break;
      case 6:
          ID[nId*2] = '0';
          
          if (UID[nId] >= 10)
              ID[(nId*2)+1] = 'A' + (UID[nId] -10);
          else
            ID[(nId*2)+1] = '0' + UID[nId];
          
          nId++;
          stateRFID = 3;
          break;
      case 7:
          nId = 0;
          if (receivedID(ID) == 1) {
            stateRFID++;
            setStateUsers(1);
            TI_ResetTics(timerRF);
          }
          break;
      case 8:       //waiting for user to finish
          if (TI_GetTics(timerRF) >= 2500)
              stateRFID = 100;
          break;
      /***********************************************************************
       ****************************** HALT ***********************************
       ***********************************************************************/          
      case 100:
        MFRC522_ToCardCoop(PCD_TRANSCEIVE, buff, 4, buff, &unLen );
        break;
      case 101:
          stateRFID = 0;
          break;
  }

}

void setStatusRFID(unsigned char stat)
{
    stateRFID = stat;
}