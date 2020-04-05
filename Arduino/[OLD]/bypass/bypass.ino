/*  PROTOCOLO DE ENVÍO: 12 bytes
 *  Bytes
 *  ╔  ╗
 *  ║00║ || 255 - FIJO COMO PROTOCOLO ||
 *  ╠  ╣
 *  ║01║ || 255 - FIJO COMO PROTOCOLO ||
 *  ╠  ╣
 *  ║02║ Reservados para ID de BMS. Valores:2     ,0 para MPPT, 1 para MOTORES, 2 para BMS
 *  ╠  ╣
 *  ║03║ Reservados para ID de BMS. Valores:0
 *  ╠  ╣
 *  ║04║ || 255 - FIJO COMO PROTOCOLO ||
 *  ╠  ╣
 *  ║05║ CAN BUFF[0]
 *  ╠  ╣
 *  ║06║ CAN BUFF[1]
 *  ╠  ╣
 *  ║07║ CAN BUFF[2]
 *  ╠  ╣
 *  ║08║ CAN BUFF[3]
 *  ╠  ╣
 *  ║09║ CAN BUFF[4]
 *  ╠  ╣
 *  ║10║ CAN BUFF[5]
 *  ╠  ╣
 *  ║11║ CAN BUFF[6]
 *  ╠  ╣
 *  ║12║ CAN BUFF[7]
 *  ╠  ╣
 *  ║13║ || 255 - FIJO COMO PROTOCOLO ||
 *  ╚  ╝
 */
 
#include <SoftwareSerial.h>
#define pinRX 7
#define pinTX 6

SoftwareSerial mySerial(pinRX, pinTX); // RX, TX
unsigned char dataToSend[14];

unsigned char buff[8];

void SendMsg(){
  for(unsigned char charToSend:dataToSend){
    mySerial.write(charToSend);
    Serial.write(charToSend);
  }
}

void setup() {

Serial.begin(9600);
mySerial.begin(9600);
dataToSend[0]   = 255; //Header
dataToSend[1]   = 255; //Header
dataToSend[4]   = 255; //Middle
dataToSend[13]  = 255; //END

//Cambiando valores
for (int i=0; i<8; i++){
  buff[i]=i+99;
}
}

void loop(){

//Send
//Ids
  dataToSend[2] = 1;
  dataToSend[3] = 1;
  
  for(int j=0; j<8; j++){
    dataToSend[j+5] = buff[j];
  }
  SendMsg();

delay(1000);
}
