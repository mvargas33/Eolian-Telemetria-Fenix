#include <mcp_can.h>
#include <SPI.h>
#include <SoftwareSerial.h>
int rx = 10;
int tx = 11;

SoftwareSerial mySerial(rx, tx); // RX, TX

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
////////////////////////////////////////////////////////////////////
/// Tx
unsigned char CCP_A2D_BATCH_READ1[1] = {0x1B};

int del = 100;
int i   = 0; 

/// Rx
unsigned char flagRecv = 0;
unsigned char len = 0;
char str[20];

unsigned char buff[7];

/* Excel
const int serialBufferSize = 32;      // buffer size for input
char  serialBuffer[serialBufferSize]; // buffer for input
const int serialMaxArgs = 4;          // max CSV message args
char* serialArgs[serialMaxArgs];      // args pointers
int   idxM1 = 0;                        // index
int   idxM2 = 0;
int   idxBMS  = 0;
int   outputTiming = 1000;            // packet sending timing in ms      important: this dermines the output timing
float input1;                         // received value
float inputArray[6];                  // received values
*/


///////////////////////////////////////////////////////////////////

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
  mySerial.begin(9600);
  
START_INIT:

  if (CAN_OK == CAN.begin(CAN_125KBPS))                  // init can bus : baudrate = 500k
  {
    mySerial.println("CAN BUS Shield esta ready papi!");
  }
  else
  {
    mySerial.println("CAN BUS Shield init fail");
    mySerial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  }
  attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
  // rx buffer clearing
  while (mySerial.available() > 0) {
    byte c = mySerial.read();
  }
}


void MCP2515_ISR()
{
  flagRecv = 1;
}

void loop(){
  
  ////////////////////////////////////////////// MPPT1 2.0 ////////////////////////////////////////////////////////////

    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x711, 0, 0, 0);
    unsigned long canId1 = CAN.getCanId();
    if (canId1 == 0x771){
      
    flagRecv = 0; //borrar flag
    CAN.readMsgBuf(&len, buff);

    int MPPT_TEMP  = buff[6];
    int  Uin  = ((bitRead(buff[0],1)<<1|bitRead(buff[0],1))<<8)|buff[1];
    int  Iin  = ((bitRead(buff[2],1)<<1|bitRead(buff[2],1))<<8)|buff[3];
    int Uout  = ((bitRead(buff[4],1)<<1|bitRead(buff[4],1))<<8)|buff[5];
    int BVLR = (bitRead(buff[0],7));
    int OVT  = (bitRead(buff[0],6));
    int NOC  = (bitRead(buff[0],5));
    int UNDV = (bitRead(buff[0],4));
    
    
    
    mySerial.print("MPPT1_BVLR,");mySerial.print(BVLR);mySerial.print("\n");
    mySerial.print("MPPT1_OVT,");mySerial.print(OVT);mySerial.print("\n");
    mySerial.print("MPPT1_NOC,");mySerial.print(NOC);mySerial.print("\n");
    mySerial.print("MPPT1_UNDV,");mySerial.print(UNDV);mySerial.print("\n");
    mySerial.print("MPPT1_TEMP,");mySerial.print(MPPT_TEMP);mySerial.print("\n");
    mySerial.print("MPPT1_UIN,");mySerial.print(Uin);mySerial.print("\n");
    mySerial.print("MPPT1_IIN,");mySerial.print(Iin);mySerial.print("\n");
    mySerial.print("MPPT1_UOUT");mySerial.print(Uout);mySerial.print("\n");
    }

    ////////////////////////////////////////////// MPPT2 2.0 ////////////////////////////////////////////////////////////

    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x712, 0, 0, 0);
    unsigned long canId2 = CAN.getCanId();
    if (canId2 == 0x772){
      
    flagRecv = 0; //borrar flag
    CAN.readMsgBuf(&len, buff);

    int MPPT_TEMP  = buff[6];
    int  Uin  = ((bitRead(buff[0],1)<<1|bitRead(buff[0],1))<<8)|buff[1];
    int  Iin  = ((bitRead(buff[2],1)<<1|bitRead(buff[2],1))<<8)|buff[3];
    int Uout  = ((bitRead(buff[4],1)<<1|bitRead(buff[4],1))<<8)|buff[5];
    int BVLR = (bitRead(buff[0],7));
    int OVT  = (bitRead(buff[0],6));
    int NOC  = (bitRead(buff[0],5));
    int UNDV = (bitRead(buff[0],4));
    
    
    mySerial.print("MPPT2_BVLR,");mySerial.print(BVLR);mySerial.print("\n");
    mySerial.print("MPPT2_OVT,");mySerial.print(OVT);mySerial.print("\n");
    mySerial.print("MPPT2_NOC,");mySerial.print(NOC);mySerial.print("\n");
    mySerial.print("MPPT2_UNDV,");mySerial.print(UNDV);mySerial.print("\n");
    mySerial.print("MPPT2_TEMP,");mySerial.print(MPPT_TEMP);mySerial.print("\n");
    mySerial.print("MPPT2_UIN,");mySerial.print(Uin);mySerial.print("\n");
    mySerial.print("MPPT2_IIN,");mySerial.print(Iin);mySerial.print("\n");
    mySerial.print("MPPT2_UOUT");mySerial.print(Uout);mySerial.print("\n");
    }

    /////// Fin Loop ///////
    } 



