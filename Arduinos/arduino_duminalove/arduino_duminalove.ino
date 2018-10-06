/*  EOLIAN FENIX - 2016 - UNIVERSIDAD DE CHILE
 *  PROGRAMA MONITOR MPPT.
 *  PERMITE LEER DATOS MPPT EN CASOS DE:
 *  - FALLA PROGRAMA PRINCIPAL.
 *  - SE REQUIERE SOLO DATOS DE MPPTs.
 */
 
///////////////////////////////////////////////////////////////////////////////////
#include <EasyTransfer.h>
//create object
EasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  unsigned int cellID;
  unsigned int voltage;
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;

///////////////////////////////////////////////////////////////////////////////////
#include <mcp_can.h>
#include <SPI.h>

//Variables CAN
const int SPI_CS_PIN = 9;
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned long canId;
unsigned char buff[8];

//Registro ultimo envío
long lastVoltTime = 0;

///////////////////////////////////////////////////////////////////

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void MCP2515_ISR(){
  flagRecv = 1;
}

void setup()
{
  Serial.begin(115200);
  ET.begin(details(mydata), &Serial);
  
START_INIT:
  if (CAN_OK == CAN.begin(CAN_1000KBPS))       //Velocidad de MPPTS
  {
    Serial.println("CAN BUS-VOLT Shield RECONOCIDO!");
  }else{
    Serial.println("CAN BUS-VOLT Shield FALLA INICIO");
    Serial.println("REINICIANDO CAN BUS-VOLT Shield ");
    delay(1000);
    goto START_INIT;
  }
  attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
  // rx buffer clearing
  while (Serial.available() > 0) {
    byte c = Serial.read();
  }
}

void loop(){
  
  if(CAN.checkReceive()){
    CAN.readMsgBuf(&len, buff);
    unsigned long canId = CAN.getCanId();

    if (canId == 0x036){
      mydata.cellID = buff[0];
      //Serial.print(buff[0]);
      //Serial.print(" = ");
      //Serial.println((buff[1]<<8)|buff[2]);
      mydata.voltage = (buff[1]<<8)|buff[2];
    }
  }

  if((millis() - lastVoltTime) > 20){
      ET.sendData();
      //Serial.println("Envío!!");
      lastVoltTime=millis();
    }

} //Fin Loop//



