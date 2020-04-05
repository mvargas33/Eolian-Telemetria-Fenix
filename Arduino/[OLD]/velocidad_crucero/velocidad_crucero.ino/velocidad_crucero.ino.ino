/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/
#include <mcp_can.h>
#include <SPI.h>

#include <PID_v1.h>

#define PIN_INPUT 0
#define PIN_OUTPUT 3

//Tiempos de REQUEST/LECTURA KELLYS | DELAY TEMPERATURAS BMS
#define timi 100//256
#define maxTimi 1200

//Receive/Send ID de ambos Kelly
#define recvIdKelly1 0xC8//DER
#define sendIdKelly1 0xD2
#define recvIdKelly2 0xC9//IZ
#define sendIdKelly2 0xD3

//IDs de request para kellys
unsigned char CCP_A2D_BATCH_READ1[1] = {0x1B};
unsigned char CCP_A2D_BATCH_READ2[1] = {0x1A};  // {Ia,Ib,Ic,Va,Vb,Vc}
unsigned char CPP_MONITOR1[1] = {0x33};         // {PWM,EnableMotorRotation,EngTemp,KellyTemp,highSideFETMOSTemp,lowSideFETMOSTemp}
unsigned char CPP_MONITOR2[1] = {0x37};         // {MSB RPM, LSB RPM, someValue, MSB ERROR CODE, LSB ERROR CODE}
unsigned char COM_SW_ACC[2] = {0x42, 0};
unsigned char COM_SW_BRK[2] = {0x43, 0};
unsigned char COM_SW_REV[2] = {0x44, 0};

//Variable aux de envío/respuesta Kelly
unsigned char engData[2] = {B10000001,B10000001}; 

//Pin de señal del Cna Shield hacia el Arduino |D10 para v0.9b y v1.0 | D9 para v1.1+
const int SPI_CS_PIN = 9;

//Variables para lectura CAN
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned long canId;
unsigned char buff[8];

//Variables de delay
long lastKelly1Time = 0;
long lastKelly2Time = 0;

/*//////////////////// TRUE PARA ENVIAR ////////////////////*/
bool revMode = true;
bool motores = true;
bool serialEngine = true;
bool serialBms = true;
bool serial_vectores= false;  //Serial de vectores
bool serial_mppt = false;     //Serial trackers para probar EasyTransfer

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin of CAN
void MCP2515_ISR(){
  flagRecv = 1;
}

void UpdateSpeed(int RPM1, int RPM2){
  velocidad_final = ((RPM1+RPM2)/2)*0.101410611;
}

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup(){
  START_INIT:

  if (CAN_OK == CAN.begin(CAN_1000KBPS)){
    Serial.println("Falla de inicio CAN BUS Shield BMS-KELLY ... Reiniciando");
    delay(100);
    goto START_INIT;
  }
  
  attachInterrupt(0, MCP2515_ISR , FALLING); // start interrupt
  // rx buffer clearing
  while (Serial1.available() > 0) {
    byte c = Serial1.read();
  }
  
  //initialize the variables we're linked to
  Input = analogRead(PIN_INPUT);
  Setpoint = 100;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  /*//////////////////// BLOQUE DE REQUEST KELLYS ////////////////////*/

  if(motores){
    if(millis() - lastKelly1Time > maxTimi){
      engData[0] = B10000001;
      lastKelly1Time = millis();
    }
    if(millis() - lastKelly2Time > maxTimi){
      engData[1] = B10000001; 
      lastKelly2Time = millis();
    }
    //Serial1.print("kelly1 ");Serial1.print(millis() - lastKelly1Time);Serial1.print("  kelly2 ");Serial1.print(millis() - lastKelly2Time);Serial1.print("  engineData ");Serial1.println(engineData);
    
    if(((millis() - lastKelly1Time) > timi) && bitRead(engData[0],7)){
      if((B1111111&engData[0]) == 1){
        CAN.sendMsgBuf(recvIdKelly1, 0, 1, CCP_A2D_BATCH_READ2);
      } else if ((B1111111&engData[0]) == 2){
        CAN.sendMsgBuf(recvIdKelly1, 0, 1, CPP_MONITOR2);
      } else if ((B1111111&engData[0]) == 3){
        CAN.sendMsgBuf(recvIdKelly1, 0, 1, CPP_MONITOR1);
      } else if (((B1111111&engData[0]) == 4) && (revMode == true)){
        CAN.sendMsgBuf(recvIdKelly1, 0, 2, COM_SW_ACC);
      } else if (((B1111111&engData[0]) == 5) && (revMode == true)){
        CAN.sendMsgBuf(recvIdKelly1, 0, 2, COM_SW_REV);
      }
      bitWrite(engData[0],7,0);
      lastKelly1Time = millis();
    }
  
    if(((millis() - lastKelly2Time) > timi) && bitRead(engData[1],7)){
      if((B1111111&engData[1]) == 1){
        CAN.sendMsgBuf(recvIdKelly2, 0, 1, CCP_A2D_BATCH_READ2);
      } else if ((B1111111&engData[1]) == 2){
        CAN.sendMsgBuf(recvIdKelly2, 0, 1, CPP_MONITOR2);
      } else if ((B1111111&engData[1]) == 3){
        CAN.sendMsgBuf(recvIdKelly2, 0, 1, CPP_MONITOR1);
      } else if (((B1111111&engData[1]) == 4) && (revMode == true)){
        CAN.sendMsgBuf(recvIdKelly2, 0, 2, COM_SW_ACC);
      } else if (((B1111111&engData[1]) == 5) && (revMode == true)){
        CAN.sendMsgBuf(recvIdKelly2, 0, 2, COM_SW_REV);
      }
      bitWrite(engData[1],7,0);
      lastKelly2Time = millis();
    }
  }

/*//////////////////// FIN REQUEST KELLYS ////////////////////*/

  if(CAN.checkReceive()){
    CAN.readMsgBuf(&len, buff);
    unsigned long canId = CAN.getCanId();
    
/*//////////////////// LECTURA CAN: KELLYS ////////////////////*/
  //Kelly Der//
    if((canId == sendIdKelly1) && !bitRead(engData[0],7)){

      if((B1111111&engData[0]) == 1){

        KELLY_DER[0]= (buff[0],DEC);     // IA DER
        KELLY_DER[1]= (buff[1],DEC);     // IB DER
        KELLY_DER[2]= (buff[2],DEC);     // IC DER
        KELLY_DER[3]= (buff[3]/1.84);    // VA DER
        KELLY_DER[4]= (buff[4]/1.84);    // VB DER
        KELLY_DER[5]= (buff[5]/1.84);    // VC DER
        engData[0] += 1;
        if(nuevo){
          Serial.print(char(0x51));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[0]);
          Printdata(KELLY_DER[1]);
          Printdata(KELLY_DER[2]);
          Printdata(KELLY_DER[3]);
          Printdata(KELLY_DER[4]);
          Printdata(KELLY_DER[5]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("I1_A,");Serial.print(buff[0],DEC);Serial.print(" | ");
          Serial.print("I1_B,");Serial.print(buff[1],DEC);Serial.print(" | ");
          Serial.print("I1_C,");Serial.print(buff[2],DEC);Serial.print(" | ");
          Serial.print("V1_A,");Serial.print(buff[3]/1.84);Serial.print(" | ");
          Serial.print("V1_B,");Serial.print(buff[4]/1.84);Serial.print(" | ");
          Serial.print("V1_C,");Serial.print(buff[5]/1.84);Serial.print("\n");
        }
      }else if((B1111111&engData[0]) == 2){  // Si el 1er digito de engineData es '2' se procede a leer RPM.

        KELLY_DER[6]= ((buff[0])<<8|buff[1]);   // RPM DER
        KELLY_DER[7]= ((buff[3])<<8|buff[4]);   // ERROR CODE DER
        UpdateSpeed(KELLY_DER[6],KELLY_IZ[6]);  // Update de velocidad_final
        if(nuevo){
          Serial.print(char(0x52));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[6]);
          Printdata(KELLY_DER[7]);
          Printdata(KELLY_DER[8]);
          Printdata(KELLY_DER[9]);
          Printdata(KELLY_DER[10]);
          Printdata(KELLY_DER[11]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG1_RPM,");Serial.print((buff[0])<<8|buff[1]);Serial.print(" | ");
          Serial.print("ENG1_ERR_CODE,");Serial.print((buff[3])<<8|buff[4]);Serial.print("\n");
        }
        engData[0] += 1;
        
      }else if((B1111111&engData[0]) == 3){                        // Si el 1er digito de engineData es '3' se procede a leer la temperatura.

        KELLY_DER[8]= buff[0];     //PWM DER
        KELLY_DER[9]= buff[1];     //EMR DER
        KELLY_DER[10]= buff[2];    //MOTOR TEMP DER
        KELLY_DER[11]= buff[3];    //KELLY TEMP DER
        if(nuevo){
          Serial.print(char(0x52));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[6]);
          Printdata(KELLY_DER[7]);
          Printdata(KELLY_DER[8]);
          Printdata(KELLY_DER[9]);
          Printdata(KELLY_DER[10]);
          Printdata(KELLY_DER[11]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG1_PWM,");Serial.print(buff[0]);Serial.print(" | ");
          Serial.print("ENG1_EMR,");Serial.print(buff[1]);Serial.print(" | ");
          Serial.print("ENG1_TEMP,");Serial.print(buff[2]);Serial.print(" | ");      // Temperatura motor: Celcius
          Serial.print("Kelly1_Temp,");Serial.print(buff[3]);Serial.print("\n");
        }
        if(revMode){ 
          engData[0] += 1;
        }else{
          engData[0] = 1;
        }
        
      }else if((B1111111&engData[0]) == 4){ 

        KELLY_DER[12]=buff[0];   //THROTLE SWITCH STATUS DER
        if(nuevo){
          Serial.print(char(0x53));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[12]);
          Printdata(KELLY_DER[13]);
          Printdata(KELLY_IZ[0]);
          Printdata(KELLY_IZ[1]);
          Printdata(KELLY_IZ[2]);
          Printdata(KELLY_IZ[3]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG1_Current_Throttle_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Throttle Status
        }
        engData[0] += 1;
        
      }else if((B1111111&engData[0]) == 5){     

        KELLY_DER[13]=buff[0];   //REVERSE SWITCH STATUS DER
        if(nuevo){
          Serial.print(char(0x53));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[12]);
          Printdata(KELLY_DER[13]);
          Printdata(KELLY_IZ[0]);
          Printdata(KELLY_IZ[1]);
          Printdata(KELLY_IZ[2]);
          Printdata(KELLY_IZ[3]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG1_Current_Reserve_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Reverse Status
        }
        engData[0] = 1;
        
      }
      bitWrite(engData[0],7,1);

    //Kelly IZ//
    }else if((canId == sendIdKelly2) && !bitRead(engData[1],7)){

      if((B1111111&engData[1]) == 1){         

        KELLY_IZ[0]= (buff[0],DEC);   //IA IZ
        KELLY_IZ[1]= (buff[1],DEC);   //IB IZ
        KELLY_IZ[2]= (buff[2],DEC);   //IC IZ
        KELLY_IZ[3]= (buff[3]/1.84);  //VA IZ
        KELLY_IZ[4]= (buff[4]/1.84);  //VB IZ
        KELLY_IZ[5]= (buff[5]/1.84);  //VC IZ
        if(nuevo){
          Serial.print(char(0x53));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_DER[12]);
          Printdata(KELLY_DER[13]);
          Printdata(KELLY_IZ[0]);
          Printdata(KELLY_IZ[1]);
          Printdata(KELLY_IZ[2]);
          Printdata(KELLY_IZ[3]);
          delay(delay_4);
          Serial.print(char(0x54));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[4]);
          Printdata(KELLY_IZ[5]);
          Printdata(KELLY_IZ[6]);
          Printdata(KELLY_IZ[7]);
          Printdata(KELLY_IZ[8]);
          Printdata(KELLY_IZ[9]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("I2_A,");Serial.print(buff[0],DEC);Serial.print(" | ");
          Serial.print("I2_B,");Serial.print(buff[1],DEC);Serial.print(" | ");
          Serial.print("I2_C,");Serial.print(buff[2],DEC);Serial.print(" | ");
          Serial.print("V2_A,");Serial.print(buff[3]/1.84);Serial.print(" | ");
          Serial.print("V2_B,");Serial.print(buff[4]/1.84);Serial.print(" | ");
          Serial.print("V2_C,");Serial.print(buff[5]/1.84);Serial.print("\n");
        }
        engData[1] += 1;
        
      }else if((B1111111&engData[1]) == 2){  // 2do digito de engineData es 2, lee RPM.

        KELLY_IZ[6]= ((buff[0])<<8|buff[1]); //RPM IZ
        KELLY_IZ[7]= ((buff[3])<<8|buff[4]); //ERROR CODE IZ
        UpdateSpeed(KELLY_DER[6],KELLY_IZ[6]);  // Update de velocidad_final
        if(nuevo){
          Serial.print(char(0x54));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[4]);
          Printdata(KELLY_IZ[5]);
          Printdata(KELLY_IZ[6]);
          Printdata(KELLY_IZ[7]);
          Printdata(KELLY_IZ[8]);
          Printdata(KELLY_IZ[9]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG2_RPM,");Serial.print((buff[0])<<8|buff[1]);Serial.print(" | ");
          Serial.print("ENG2_ERR_CODE,");Serial.print((buff[3])<<8|buff[4]);Serial.print("\n");
        }
        engData[1] += 1;
        
      }else if((B1111111&engData[1]) == 3){                        // 2do digito de engineData es 3, lee temperatura.
        
        KELLY_IZ[8]= buff[0];   //PWM IZ
        KELLY_IZ[9]= buff[1];   //EMR IZ
        KELLY_IZ[10]= buff[2];   //MOTOR TEMP IZ
        KELLY_IZ[11]= buff[3];   //KELLY TEMP IZ
        if(nuevo){
          Serial.print(char(0x54));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[4]);
          Printdata(KELLY_IZ[5]);
          Printdata(KELLY_IZ[6]);
          Printdata(KELLY_IZ[7]);
          Printdata(KELLY_IZ[8]);
          Printdata(KELLY_IZ[9]);
          delay(delay_4);
          Serial.print(char(0x55));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[10]);
          Printdata(KELLY_IZ[11]);
          Printdata(KELLY_IZ[12]);
          Printdata(KELLY_IZ[13]);
          Printdata(MPPT1[0]);
          Printdata(MPPT1[1]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG2_PWM,");Serial.print(buff[0]);Serial.print(" | ");
          Serial.print("ENG2_EMR,");Serial.print(buff[1]);Serial.print(" | ");
          Serial.print("ENG2_TEMP,");Serial.print(buff[2]);Serial.print(" | ");      // Temperatura motor: Celcius
          Serial.print("Kelly2_Temp,");Serial.print(buff[3]);Serial.print("\n");
        }
        if(revMode) { 
          engData[1] += 1;
        } else {
          engData[1] = 1;
        }
        
      }else if((B1111111&engData[1]) == 4){  
  
        KELLY_IZ[12]=buff[0];   //THROTLE SWITCH STATUS IZ
        if(nuevo){
          Serial.print(char(0x55));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[10]);
          Printdata(KELLY_IZ[11]);
          Printdata(KELLY_IZ[12]);
          Printdata(KELLY_IZ[13]);
          Printdata(MPPT1[0]);
          Printdata(MPPT1[1]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG2_Current_Throttle_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Throttle Status
        }
        engData[1] += 1;
        
      }else if((B1111111&engData[1]) == 5){     

        KELLY_IZ[13]=buff[0]; //REVERSE SWITCH STATUS IZ
        if(nuevo){
          Serial.print(char(0x55));
          Printdata(velocidad_final);
          Printdata(BMS[0]);
          Printdata(BMS[1]);
          Printdata(BMS[2]);
  
          Printdata(KELLY_IZ[10]);
          Printdata(KELLY_IZ[11]);
          Printdata(KELLY_IZ[12]);
          Printdata(KELLY_IZ[13]);
          Printdata(MPPT1[0]);
          Printdata(MPPT1[1]);
          delay(delay_4);
        }
        if(serial_exp){
          Serial.print("ENG2_Current_Reserve_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Reverse Status
        }
        engData[1] = 1;
        
      }
      bitWrite(engData[1],7,1);
    }
  }

/*//////////////////// FIN KELLYS ////////////////////*/

    
  Input = analogRead(PIN_INPUT);
  myPID.Compute();
  analogWrite(PIN_OUTPUT, Output);
}


