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
#define pinRX 6
#define pinTX 7

SoftwareSerial mySerial(pinRX, pinTX); // RX, TX
unsigned char dataReceived[14];
unsigned char buff[8];
unsigned char inChar;
unsigned char id;
int charsRead=0;
int index=0;


 void setup() {
mySerial.begin(9600);
Serial.begin(9600);

}

void loop() {

while(mySerial.available() || (charsRead < 14)){  //Revisar si es necesario un timeout
    //Serial.println("MySerial_Available");
    
    if(index < 14){
      Serial.print(index);Serial.println(" < 14");
      inChar = mySerial.read();
      dataReceived[index] = inChar;
      index++;
      Serial.print("inChar: ");Serial.println(inChar);
    }else{
      //inChar = mySerial.read();
      //inData[index] = inChar;
      //index++;
      Serial.println("DATERECEIVED_COMPLETO");
    if((dataReceived[0] == 255) && (dataReceived[1] == 255) && (dataReceived[4] == 255) && (dataReceived[13] == 255)){
      Serial.println("LLEGO EN ORDEN");
      if(dataReceived[2] == 1){  //REVISAR que igualdad se cumpla!!!
        id = dataReceived[1];
        
        for(int j = 5;j<13;j++){
          buff[j-5] = dataReceived[j];
          }

        char a0 = buff[0];
        char a1 = buff[1];
        char a2 = buff[2];
        char a3 = buff[3];
        char a4 = buff[4];
        char a5 = buff[5];
        char a6 = buff[6];
        char a7 = buff[7];

        Serial.println("a0");
        Serial.println("a1");
        Serial.println("a2");
        Serial.println("a3");
        Serial.println("a4");
        Serial.println("a5");
        Serial.println("a6");
        Serial.println("a7");
      }
      
    }else{
        for(int j = 1; j<13;j++){
          dataReceived[j-1] = dataReceived[j];
          }
          index-=1;
      }
    }
    charsRead++;
    
    if (charsRead>13){
      charsRead=0;
      index=0;
    }
  }


}
