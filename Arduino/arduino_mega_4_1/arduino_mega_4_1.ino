                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /*  EOLIAN FENIX - 2017 - UNIVERSIDAD DE CHILE
 *  PROGRAMA MONITOR PRINCIPAL
 *  PERMITE LEER DATOS DE CAN BUS
 *  LECTURA EN TIEMPO REAL DE:
 *  BMS - KELLYs - MPPT
 *  ENVIO DE LECTURAS A:
 *  XBEE - TABLET - SD
 */

//IMPORTANTE: delay_4 CONTROLA EL TAMANO DE LOS BLOQUES QUE RECIBE QT, 0 ES ÓPTIMO

//Librerías
#include <mcp_can.h>
#include <SPI.h>
#include "SdFat.h"    //SD
#include <stdlib.h>   //Math
#include <EasyTransfer.h> // Lectura MPPTs
SdFat sd;             //SD
SdFile myFile;        //SD

//Receive/Send ID de ambos Kelly
#define recvIdKelly1 0xC8//DER
#define sendIdKelly1 0xD2
#define recvIdKelly2 0xC9//IZ
#define sendIdKelly2 0xD3
//Tiempos de REQUEST/LECTURA KELLYS | DELAY TEMPERATURAS BMS
#define timi 40 //256
#define maxTimi 1000

/*///////////// LECTURA TEMPERATURAS ANÁLOGAS //////////////*/
const int numReadings1 = 10;
const int numReadings2 = 10;
int readings1[numReadings1];// the readings from the analog input
int readings2[numReadings2];
int readIndex1 = 0;              // the index of the current reading
int readIndex2 = 0;
int total1 = 0;                  // the running total
int total2 = 0;
int average1 = 0;                // the average
int average2 = 0;
unsigned long lastTempTime = 0;
unsigned long currentTime = 0;
int inputPin_Left = A0;         
int inputPin_Right = A1;

/*//////////////////// VARIABLES CAN ////////////////////*/
//IDs de request para kellys
unsigned char CCP_A2D_BATCH_READ1[1] = {0x1B};
unsigned char CCP_A2D_BATCH_READ2[1] = {0x1A};  // {Ia,Ib,Ic,Va,Vb,Vc}
unsigned char CPP_MONITOR1[1] = {0x33};         // {PWM,EnableMotorRotation,EngTemp,KellyTemp,highSideFETMOSTemp,lowSideFETMOSTemp}
unsigned char CPP_MONITOR2[1] = {0x37};         // {MSB RPM, LSB RPM, someValue, MSB ERROR CODE, LSB ERROR CODE}
unsigned char COM_SW_ACC[2] = {0x42, 0};
unsigned char COM_SW_BRK[2] = {0x43, 0};
unsigned char COM_SW_REV[2] = {0x44, 0};
unsigned char engData[2] = {B10000001,B10000001}; //Variable aux de envío/respuesta Kelly
bool motores = true; // Bool para el request
bool revMode = true; // Bool para envío completo de datos de motores (Datos adicionales)

//Variables para lectura CAN
const int SPI_CS_PIN = 9; //Pin de señal del CAN Shield hacia el Arduino |D10 para v0.9b y v1.0 | D9 para v1.1+
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned long canId;
unsigned char buff[8];
long lastKelly1Time = 0; //delay request
long lastKelly2Time = 0; //delay request
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin of CAN
void MCP2515_ISR(){
  flagRecv = 1;
}
/*//////////////////// VARIABLES SD ////////////////////*/
unsigned long TiempoGlobal; //Tiempo global SD
const int chipSelect = 53;
const char* archivo = "Prueba_ruta.txt";  //Nombre del archivo a guardar
int delay_sd = 0;     //Delay de alta influencia
// MISO 50 | MOSI 51 | SCK 52 | CS 53 // MISO 12 | MOSI 11 | SCK 13 | CS 10

/*//////////////////// VECTORES PRINCIPALES ////////////////////*/
int velocidad; //Velocidad
int mppt_in; //Potencia de salida hacia el banco int BMS[0];
unsigned int MAX_VOLT=0; //ex-BMS[5]
unsigned int MIN_VOLT=0; //ex-BMS[6]
int KELLY_DER[13];
int KELLY_IZQ[13];
int BMS[14];
int BMS_TEMP[60];
unsigned int BMS_VOLT[30];
unsigned int MPPT1[8];
unsigned int MPPT2[8];
unsigned int MPPT3[8];
unsigned int MPPT4[8];
double potencia_mppt1;
double potencia_mppt2;
double potencia_mppt3;
double potencia_mppt4;

// Variables adicionales
int aux_data=0;  // Concadenar envío de datos
int delay_4=0;     // 3ms(Arduino directo) 10ms(Xbee) de delay cada 10 valores - DELAY PRINCIPAL DE ENVÍO - AJUSTAR PARA LECTURA ÓPTIMA DE QT
bool serial_exp=false;  // Visualización primitiva en Serial0
bool check_mppt=false;   //Check receive de ET para MPPTs
bool xbee=true;      // Enviar o no por wifi, función Send_Xbee_Data()
bool tablet=true;    // Enviar o no por Serial0 la infromación a la tablet, función Send_tablet_Data()
/*//////////////////// VARIABLES TABLET ////////////////////*/
int aux_tablet = 0; // Concadenar envío de datos
int tiempoEnvioTablet = 10;  //30|10ms - DELAY DE ENVÍO PRINCIPAL - AJUSTAR PARA LECTURA EN TABLET
int delay_speed = 10;    //30|10ms Delay entre dato y velocidad tablet
long last_tablet_send;

/*//////////////////// RECEIVE VOLT DATA ////////////////////*/
//create object
EasyTransfer ET_VOLT; 
struct RECEIVE_DATA_STRUCTURE_VOLT{
  unsigned int cellID;
  unsigned int voltage;
};
//give a name to the group of data
RECEIVE_DATA_STRUCTURE_VOLT volt_data;  
/*//////////////////// RECEIVE MPPT DATA ////////////////////*/
//create object
EasyTransfer ET_MPPT; 
struct RECEIVE_DATA_STRUCTURE_MPPT{
  double Uin_1;
  double Iin_1;
  double Uout_1;
  unsigned int uout_umax_1;
  unsigned int t_cooler_1;
  unsigned int bateria_1;
  unsigned int under_volt_1;
  unsigned int temp_1;
  double Uin_2;
  double Iin_2;
  double Uout_2;
  unsigned int uout_umax_2;
  unsigned int t_cooler_2;
  unsigned int bateria_2;
  unsigned int under_volt_2;
  unsigned int temp_2;
  double Uin_3;
  double Iin_3;
  double Uout_3;
  unsigned int uout_umax_3;
  unsigned int t_cooler_3;
  unsigned int bateria_3;
  unsigned int under_volt_3;
  unsigned int temp_3;
  double Uin_4;
  double Iin_4;
  double Uout_4;
  unsigned int uout_umax_4;
  unsigned int t_cooler_4;
  unsigned int bateria_4;
  unsigned int under_volt_4;
  unsigned int temp_4;
  double Uin_5;
  double Iin_5;
  double Uout_5;
  unsigned int uout_umax_5;
  unsigned int t_cooler_5;
  unsigned int bateria_5;
  unsigned int under_volt_5;
  unsigned int temp_5;
};
//give a name to the group of data
RECEIVE_DATA_STRUCTURE_MPPT mppt_data;
///////////////////////////////// FIN VARIABLES ////////////////////////////////////////////

/*//////////////////// PRINT DE 4 DIGITOS INT ////////////////////*/
//Print de 1 dígitos
void Printdata_1(int entero){
  if (entero == 0){
    Serial1.print("0");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print(entero/10);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print(entero/100);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print(entero/1000);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero/10000);
  }
}//Print de 2 dígitos
void Printdata_2(int entero){
  if (entero == 0){
    Serial1.print("00");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print(entero/10);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print(entero/100);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero/1000);
  }
}
//Print de 3 dígitos
void Printdata_3(int entero){
  if (entero == 0){
    Serial1.print("000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("00");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print(entero/10);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero/100);
  }
}
//Print de 4 dígitos
void Printdata_4(int entero){
  if (entero == 0){
    Serial1.print("0000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("000");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print("00");Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print(entero);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero/10);
  }
}
//Print de 5 dígitos
void Printdata_5(int entero){
  if (entero == 0){
    Serial1.print("00000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("0000");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print("000");Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print("00");Serial1.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero);
  }
}
/*//////////////////// PRINT DE 5 DIGITOS UNSIGNED INT ////////////////////*/
void PrintdataUnsigned_1(unsigned int entero){
  if (entero == 0){
    Serial1.print("0");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print(entero);
  }
}
void PrintdataUnsigned_2(unsigned int entero){
  if (entero == 0){
    Serial1.print("00");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print(entero/10);
  }
}

// Voltajes de 5 digitos
void PrintdataUnsignedVolt(unsigned int entero){
  if (entero == 0){
    Serial1.print("00000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("0000");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print("000");Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print("00");Serial1.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print(entero);
  }
}
// Voltajes de 6 digitos
void PrintdataUnsignedVolt_6(unsigned int entero){
  if (entero == 0){
    Serial1.print("000000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial1.print("00000");Serial1.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial1.print("0000");Serial1.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial1.print("000");Serial1.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial1.print("00");Serial1.print(entero);
  }
  else if ((10000<=entero)&&(entero<100000)){
    Serial1.print("0");Serial1.print(entero);
  }
  else if ((100000<=entero)&&(entero<1000000)){
    Serial1.print(entero);
  }
}
/*//////////////////// CONCADENACIÓN DE PRINT ////////////////////*/
void Send_Xbee_all(){
  if(aux_data==0){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    //Kelly DER
    Serial1.print(char(0x41));
    Printdata_2(KELLY_DER[0]);  // IA
    Printdata_2(KELLY_DER[1]);  // IB
    Printdata_2(KELLY_DER[2]);  // IC
    delay(delay_4);
    Serial1.print(char(0x42));
    Printdata_2(KELLY_DER[3]);  // VA
    Printdata_2(KELLY_DER[4]);  // VB
    Printdata_2(KELLY_DER[5]);  // VC
    delay(delay_4);
    Serial1.print(char(0x43));
    Printdata_4(KELLY_DER[6]);  // RPM
    Printdata_2(KELLY_DER[8]);  // PWM
    delay(delay_4);
    Serial1.print(char(0x44));
    Printdata_4(KELLY_DER[7]);  // ERROR CODE
    Printdata_2(KELLY_DER[11]); // TEMP KELLY
    delay(delay_4);
    Serial1.print(char(0x45));
    Printdata_1(KELLY_DER[9]);  // EMR
    Printdata_3(KELLY_DER[10]); // TEMP MOTOR
    Printdata_1(KELLY_DER[12]); // THROTTLE
    Printdata_1(KELLY_DER[13]); // REVERSE
    delay(delay_4);
    //Kelly IZ
    Serial1.print(char(0x46));
    Printdata_2(KELLY_IZQ[0]);  // IA
    Printdata_2(KELLY_IZQ[1]);  // IB
    Printdata_2(KELLY_IZQ[2]);  // IC
    delay(delay_4);
    Serial1.print(char(0x47));
    Printdata_2(KELLY_IZQ[3]);  // VA
    Printdata_2(KELLY_IZQ[4]);  // VB
    Printdata_2(KELLY_IZQ[5]);  // VC
    delay(delay_4);
    Serial1.print(char(0x48));
    Printdata_4(KELLY_IZQ[6]);  // RPM
    Printdata_2(KELLY_IZQ[8]);  // PWM
    delay(delay_4);
    Serial1.print(char(0x49));
    Printdata_4(KELLY_IZQ[7]);  // ERROR CODE
    Printdata_2(KELLY_IZQ[11]); // TEMP KELLY
    delay(delay_4);
    Serial1.print(char(0x4a));
    Printdata_1(KELLY_IZQ[9]);  // EMR
    Printdata_3(KELLY_IZQ[10]); // TEMP MOTOR
    Printdata_1(KELLY_IZQ[12]); // THROTTLE
    Printdata_1(KELLY_IZQ[13]); // REVERSE
    delay(delay_4);
    aux_data=1;
  }else if(aux_data==1){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    //BMS
    Serial1.print(char(0x4b));
    Printdata_3(BMS[1]);        // CURRENT
    Printdata_3(BMS[2]);        // VOLTAGE
    delay(delay_4);
    Serial1.print(char(0x4c));
    Printdata_3(BMS[0]);        // SOC
    Printdata_3(BMS[3]);        // OPEN VOLTAGE
    delay(delay_4);
    Serial1.print(char(0x4d));
    Printdata_2(BMS[4]);        // ABSOLUTE CURRENT
    Printdata_2(BMS[7]);        // MAX TEMP
    Printdata_2(BMS[8]);        // MAX TEMP ID
    delay(delay_4);
    Serial1.print(char(0x4e));
    PrintdataUnsignedVolt_6(MAX_VOLT);        // MAX VOLTAGE
    delay(delay_4);
    Serial1.print(char(0x4f));
    PrintdataUnsignedVolt_6(MIN_VOLT);        // MIN VOLTAGE
    delay(delay_4);
    Serial1.print(char(0x50));
    Printdata_2(BMS[9]);        // MIN TEMP
    Printdata_2(BMS[10]);       // MIN TEMP ID
    Printdata_2(BMS[11]);       // AVERAGE TEMP
    delay(delay_4);
    Serial1.print(char(0x51));
    Printdata_2(BMS[12]);       // INTERNAL TEMP
    Printdata_2(BMS[13]);       // MAX VOLTAGE ID
    Printdata_2(BMS[14]);       // MIN VOLTAGE ID
    delay(delay_4);
    //BMS TEMP
    Serial1.print(char(0x52));
    Printdata_2(BMS_TEMP[0]);
    Printdata_2(BMS_TEMP[1]);
    Printdata_2(BMS_TEMP[2]);
    delay(delay_4);
    Serial1.print(char(0x53));
    Printdata_2(BMS_TEMP[3]);
    Printdata_2(BMS_TEMP[4]);
    Printdata_2(BMS_TEMP[5]);
    delay(delay_4);
    Serial1.print(char(0x54));
    Printdata_2(BMS_TEMP[6]);
    Printdata_2(BMS_TEMP[7]);
    Printdata_2(BMS_TEMP[8]);
    delay(delay_4);
    aux_data=2;
  }else if(aux_data==2){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x55));
    Printdata_2(BMS_TEMP[9]);
    Printdata_2(BMS_TEMP[10]);
    Printdata_2(BMS_TEMP[11]);
    delay(delay_4);
    Serial1.print(char(0x56));
    Printdata_2(BMS_TEMP[12]);
    Printdata_2(BMS_TEMP[13]);
    Printdata_2(BMS_TEMP[14]);
    delay(delay_4);
    Serial1.print(char(0x57));
    Printdata_2(BMS_TEMP[15]);
    Printdata_2(BMS_TEMP[16]);
    Printdata_2(BMS_TEMP[17]);
    delay(delay_4);
    Serial1.print(char(0x58));
    Printdata_2(BMS_TEMP[18]);
    Printdata_2(BMS_TEMP[19]);
    Printdata_2(BMS_TEMP[20]);
    delay(delay_4);
    Serial1.print(char(0x59));
    Printdata_2(BMS_TEMP[21]);
    Printdata_2(BMS_TEMP[22]);
    Printdata_2(BMS_TEMP[23]);
    delay(delay_4);
    Serial1.print(char(0x5a));
    Printdata_2(BMS_TEMP[24]);
    Printdata_2(BMS_TEMP[25]);
    Printdata_2(BMS_TEMP[26]);
    delay(delay_4);
    Serial1.print(char(0x61));
    Printdata_2(BMS_TEMP[27]);
    Printdata_2(BMS_TEMP[28]);
    Printdata_2(BMS_TEMP[29]);
    delay(delay_4);
    Serial1.print(char(0x62));
    Printdata_2(BMS_TEMP[30]);
    Printdata_2(BMS_TEMP[31]);
    Printdata_2(BMS_TEMP[32]);
    delay(delay_4);
    Serial1.print(char(0x63));
    Printdata_2(BMS_TEMP[33]);
    Printdata_2(BMS_TEMP[34]);
    Printdata_2(BMS_TEMP[35]);
    delay(delay_4);
    Serial1.print(char(0x64));
    Printdata_2(BMS_TEMP[36]);
    Printdata_2(BMS_TEMP[37]);
    Printdata_2(BMS_TEMP[38]);
    delay(delay_4);
    aux_data=3;
  }else if(aux_data==3){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x65));
    Printdata_2(BMS_TEMP[39]);
    Printdata_2(BMS_TEMP[40]);
    Printdata_2(BMS_TEMP[41]);
    delay(delay_4);
    Serial1.print(char(0x66));
    Printdata_2(BMS_TEMP[42]);
    Printdata_2(BMS_TEMP[43]);
    Printdata_2(BMS_TEMP[44]);
    delay(delay_4);
    Serial1.print(char(0x67));
    Printdata_2(BMS_TEMP[45]);
    Printdata_2(BMS_TEMP[46]);
    Printdata_2(BMS_TEMP[47]);
    delay(delay_4);
    Serial1.print(char(0x68));
    Printdata_2(BMS_TEMP[48]);
    Printdata_2(BMS_TEMP[49]);
    Printdata_2(BMS_TEMP[50]);
    delay(delay_4);
    Serial1.print(char(0x69));
    Printdata_2(BMS_TEMP[51]);
    Printdata_2(BMS_TEMP[52]);
    Printdata_2(BMS_TEMP[53]);
    delay(delay_4);
    Serial1.print(char(0x6a));
    Printdata_2(BMS_TEMP[54]);
    Printdata_2(BMS_TEMP[55]);
    Printdata_2(BMS_TEMP[56]);
    delay(delay_4);
    Serial1.print(char(0x6b));
    Printdata_2(BMS_TEMP[57]);
    Printdata_2(BMS_TEMP[58]);
    Printdata_2(BMS_TEMP[59]);
    delay(delay_4);
    //BMS VOLT
    Serial1.print(char(0x40)); //@A
    Serial1.print(char(0x41));
    PrintdataUnsignedVolt(BMS_VOLT[0]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@B
    Serial1.print(char(0x42));
    PrintdataUnsignedVolt(BMS_VOLT[1]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@C
    Serial1.print(char(0x43));
    PrintdataUnsignedVolt(BMS_VOLT[2]);
    delay(delay_4);
    aux_data=4;
  }else if(aux_data==4){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x40)); //@D
    Serial1.print(char(0x44));
    PrintdataUnsignedVolt(BMS_VOLT[3]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@E
    Serial1.print(char(0x45));
    PrintdataUnsignedVolt(BMS_VOLT[4]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@F
    Serial1.print(char(0x46));
    PrintdataUnsignedVolt(BMS_VOLT[5]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@G
    Serial1.print(char(0x47));
    PrintdataUnsignedVolt(BMS_VOLT[6]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@H
    Serial1.print(char(0x48));
    PrintdataUnsignedVolt(BMS_VOLT[7]);
    delay(delay_4);
    delay(delay_4);
    Serial1.print(char(0x40)); //@I
    Serial1.print(char(0x49));
    PrintdataUnsignedVolt(BMS_VOLT[8]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@J //10
    Serial1.print(char(0x4a));
    PrintdataUnsignedVolt(BMS_VOLT[9]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@K 
    Serial1.print(char(0x4b));
    PrintdataUnsignedVolt(BMS_VOLT[10]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@L
    Serial1.print(char(0x4c));
    PrintdataUnsignedVolt(BMS_VOLT[11]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@M
    Serial1.print(char(0x4d));
    PrintdataUnsignedVolt(BMS_VOLT[12]);
    delay(delay_4);
    aux_data=5;
  }else if(aux_data==5){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x40)); //@N
    Serial1.print(char(0x4e));
    PrintdataUnsignedVolt(BMS_VOLT[13]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@O
    Serial1.print(char(0x4f));
    PrintdataUnsignedVolt(BMS_VOLT[14]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@P
    Serial1.print(char(0x50));
    PrintdataUnsignedVolt(BMS_VOLT[15]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@Q
    Serial1.print(char(0x51));
    PrintdataUnsignedVolt(BMS_VOLT[16]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@R
    Serial1.print(char(0x52));
    PrintdataUnsignedVolt(BMS_VOLT[17]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@S
    Serial1.print(char(0x53));
    PrintdataUnsignedVolt(BMS_VOLT[18]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@T  //20
    Serial1.print(char(0x54));
    PrintdataUnsignedVolt(BMS_VOLT[19]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@U
    Serial1.print(char(0x55));
    PrintdataUnsignedVolt(BMS_VOLT[20]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@V
    Serial1.print(char(0x56));
    PrintdataUnsignedVolt(BMS_VOLT[21]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@W
    Serial1.print(char(0x57));
    PrintdataUnsignedVolt(BMS_VOLT[22]);
    delay(delay_4);
    aux_data=6;
  }else if(aux_data==6){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x40)); //@X
    Serial1.print(char(0x58));
    PrintdataUnsignedVolt(BMS_VOLT[23]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@Y
    Serial1.print(char(0x59));
    PrintdataUnsignedVolt(BMS_VOLT[24]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@Z
    Serial1.print(char(0x5a));
    PrintdataUnsignedVolt(BMS_VOLT[25]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@a
    Serial1.print(char(0x61));
    PrintdataUnsignedVolt(BMS_VOLT[26]);
    delay(delay_4);
    Serial1.print(char(0x40)); //@b  //28
    Serial1.print(char(0x62));
    PrintdataUnsignedVolt(BMS_VOLT[27]);
    delay(delay_4);
    Serial1.print(char(0x6d)); //m
    PrintdataUnsignedVolt_6(MPPT1[0]);  // Uin
    delay(delay_4);
    Serial1.print(char(0x6e)); //n
    PrintdataUnsignedVolt_6(MPPT1[1]);  // Iin
    delay(delay_4);
    Serial1.print(char(0x6f)); //o  
    PrintdataUnsignedVolt_6(MPPT1[2]);  // Uout
    delay(delay_4);
    Serial1.print(char(0x70)); //p
    PrintdataUnsigned_1(MPPT1[3]);      // BVLR  = 
    PrintdataUnsigned_1(MPPT1[4]);      // OVT   = Over voltage
    PrintdataUnsigned_1(MPPT1[5]);      // NOC   = 
    PrintdataUnsigned_1(MPPT1[6]);      // UNDV  = Under voltage
    PrintdataUnsigned_2(MPPT1[7]);      // TEMP
    delay(delay_4);
    Serial1.print(char(0x71));  //q
    PrintdataUnsignedVolt_6(MPPT2[0]);  // Uin
    delay(delay_4);
    aux_data=7;
  }else if(aux_data==7){
    //Speed-Current
    Serial1.print(char(0x6c));
    Printdata_3(velocidad);     // SPEED
    Printdata_3(BMS[1]);        // CURRENT
    delay(delay_4);
    Serial1.print(char(0x72));  //r
    PrintdataUnsignedVolt_6(MPPT2[1]);  // Iin
    delay(delay_4);
    Serial1.print(char(0x73));  //s
    PrintdataUnsignedVolt_6(MPPT2[2]);  // Uout
    delay(delay_4);
    Serial1.print(char(0x74));  //t
    PrintdataUnsigned_1(MPPT2[3]);      // BVLR  = 
    PrintdataUnsigned_1(MPPT2[4]);      // OVT   = Over voltage
    PrintdataUnsigned_1(MPPT2[5]);      // NOC   = 
    PrintdataUnsigned_1(MPPT2[6]);      // UNDV  = Under voltage
    PrintdataUnsigned_2(MPPT2[7]);      // TEMP
    delay(delay_4);
    Serial1.print(char(0x75));  //u
    PrintdataUnsignedVolt_6(MPPT3[0]);  // Uin
    delay(delay_4);
    Serial1.print(char(0x76));  //v
    PrintdataUnsignedVolt_6(MPPT3[1]);  // Iin
    delay(delay_4);
    Serial1.print(char(0x77));  //w
    PrintdataUnsignedVolt_6(MPPT3[2]);  // Uout
    delay(delay_4);
    Serial1.print(char(0x78));  //x
    PrintdataUnsigned_1(MPPT3[3]);      // BVLR  = 
    PrintdataUnsigned_1(MPPT3[4]);      // OVT   = Over voltage
    PrintdataUnsigned_1(MPPT3[5]);      // NOC   = 
    PrintdataUnsigned_1(MPPT3[6]);      // UNDV  = Under voltage
    PrintdataUnsigned_2(MPPT3[7]);      // TEMP
    delay(delay_4);
    Serial1.print(char(0x79));  //y
    PrintdataUnsignedVolt_6(MPPT4[0]);  // Uin
    delay(delay_4);
    Serial1.print(char(0x7a));  //z
    PrintdataUnsignedVolt_6(MPPT4[1]);  // Iin
    delay(delay_4);
    Serial1.print(char(0x7b));  //{
    PrintdataUnsignedVolt_6(MPPT4[2]);  // Uout
    delay(delay_4);
    Serial1.print(char(0x7d));  //}
    PrintdataUnsigned_1(MPPT4[3]);      // BVLR  = 
    PrintdataUnsigned_1(MPPT4[4]);      // OVT   = Over voltage
    PrintdataUnsigned_1(MPPT4[5]);      // NOC   = 
    PrintdataUnsigned_1(MPPT4[6]);      // UNDV  = Under voltage
    PrintdataUnsigned_2(MPPT4[7]);      // TEMP
    delay(delay_4);
    aux_data=0;
  }
  
}
/*//////////////////// Función para la escritura en SD ////////////////////*/
/*void EscribirSD(){
  //Tiempo
  myFile.print(TiempoGlobal/1000);  //En segundos
  myFile.print(",");
/*//////////////////// BMS WRITE ////////////////////*/
/*  myFile.print("BMS,");
//BMS A
for(int i=0;i<15;i++){  
  myFile.print(BMS[i]);
  myFile.print(",");
  delay(delay_sd);
}
//BMS VOLT
for(unsigned int i=0;i<30;i++){
  myFile.print(BMS_VOLT[i]);
  myFile.print(",");
  delay(delay_sd);
}
//BMS TEMP
for(unsigned int i=0;i<60;i++){
  myFile.print(BMS_TEMP[i]);
  myFile.print(",");
  delay(delay_sd);
}
/*//////////////////// KELLY WRITE ////////////////////*/
 /* myFile.print("KELLY,");
//KELLY DERECHO
for(int i=0;i<14;i++){
  myFile.print(KELLY_DER[i]);
  myFile.print(",");
  delay(delay_sd);
}
//KELLY IZQUIERDO
for(int i=0;i<14;i++){
  myFile.print(KELLY_IZ[i]);
  myFile.print(",");
  delay(delay_sd);
}
/*//////////////////// MPPT WRITE ////////////////////*/
 /* myFile.print("MPPT,");
//MPPT1
for(int i=0;i<8;i++){
  myFile.print(MPPT1[i]);
  myFile.print(",");
  delay(delay_sd);
}
//MPPT2
for(int i=0;i<8;i++){
  myFile.print(MPPT2[i]);
  myFile.print(",");
  delay(delay_sd);
}
//MPPT3
for(int i=0;i<8;i++){
  myFile.print(MPPT3[i]);
  myFile.print(",");
  delay(delay_sd);
}
//MPPT4
for(int i=0;i<8;i++){
  myFile.print(MPPT4[i]);
  myFile.print(",");
  delay(delay_sd);
}
//Fin de la línea
  myFile.println("END");
  //myFile.print("\n");
  delay(delay_sd);
}

/*//////////////////// UPDATE mppt_in FOR TABLET ////////////////////*/
void UpdateMPPTin(){
  /*Serial.print("Potencia MPPT1: ");Serial.println(potencia_mppt1);
  Serial.print("Potencia MPPT2: ");Serial.println(potencia_mppt2);
  Serial.print("Potencia MPPT3: ");Serial.println(potencia_mppt3);
  Serial.print("Potencia MPPT4: ");Serial.println(potencia_mppt4);
  */
  mppt_in = (potencia_mppt1+potencia_mppt2+potencia_mppt3+potencia_mppt4); // en Whatts
}
/*///////////////////////// UPDATE VELOCIDAD //////////////////////////*/
void UpdateSpeed(int RPM1, int RPM2){
  velocidad = float(((float(RPM1)+float(RPM2))/2))*float(0.101410611);
  //Serial.print("velocidad = ");Serial.println(velocidad);
}
/*//////////////////// MAIN FUNCTIONS TO SEND DATA TO TABLET ////////////////////*/

void Send_speed(){
    Serial.print("a,");Serial.print(velocidad);Serial.print("\n"); // Velocidad final
}
void Send_tablet_data(){
  if((millis() - last_tablet_send) > tiempoEnvioTablet){
    if(aux_tablet == 0){
      Serial.print("b,");Serial.print(BMS[0]);Serial.print("\n");  // PACK SOC all_data.bms0/2
      delay(delay_speed);
      Send_speed();
      aux_tablet = 1;
      last_tablet_send = millis();
    }else if(aux_tablet == 1){
      Serial.print("c,");Serial.print(BMS[1]/10);Serial.print("\n"); // PACK CURRENT all_data.bms1
      delay(delay_speed);
      Send_speed();
      aux_tablet = 2;
      last_tablet_send = millis();
    }else if(aux_tablet == 2){
      Serial.print("d,");Serial.print(BMS[2]);Serial.print("\n"); // PACK INST VOLT all_data.bms2
      delay(delay_speed);
      Send_speed();
      aux_tablet = 3;
      last_tablet_send = millis();
    }else if(aux_tablet == 3){
      UpdateMPPTin(); //Update mppt_in value
      Serial.print("e,");Serial.print(0);Serial.print("\n"); // MPPT IN IN WHATTS mppt_in
      delay(delay_speed);
      Send_speed();
      aux_tablet = 4;
      last_tablet_send = millis();
    }else if(aux_tablet == 4){
      Serial.print("f,");Serial.print(MIN_VOLT);Serial.print("\n"); // MIN VOLT MIN_VOLT
      delay(delay_speed);
      Send_speed();
      aux_tablet = 5;
      last_tablet_send = millis();
    }else if(aux_tablet == 5){
      Serial.print("g,");Serial.print(MAX_VOLT);Serial.print("\n"); // MAX VOLT MAX_VOLT
      delay(delay_speed);
      Send_speed();
      aux_tablet = 6;
      last_tablet_send = millis();
    }else if(aux_tablet == 6){
      Serial.print("h,");Serial.print(KELLY_DER[10]);Serial.print("\n"); //MOTOR DER TEMP KELLY_DER[10]
      delay(delay_speed);
      Send_speed();
      aux_tablet = 7;
      last_tablet_send = millis();
    }else if(aux_tablet == 7){
      Serial.print("i,");Serial.print(KELLY_IZQ[10]);Serial.print("\n"); //MOTOR IZQ TEMP KELLY_IZQ[10]
      delay(delay_speed);
      Send_speed();
      aux_tablet = 8;
      last_tablet_send = millis();
    }else if(aux_tablet == 8){
      Serial.print("j,");Serial.print(BMS[9]);Serial.print("\n"); // MIN TEMP BMS[9]
      delay(delay_speed);
      Send_speed();
      aux_tablet = 9;
      last_tablet_send = millis();
    }else if(aux_tablet == 9){
      Serial.print("k,");Serial.print(BMS[7 ]);Serial.print("\n"); //MAX TEMP BMS[7]
      delay(delay_speed);
      Send_speed();
      aux_tablet = 0;
      last_tablet_send = millis();
    }
  }
}

/*//////////////////// SETUP ////////////////////*/

void setup()
{
  Serial.begin(115200);                //Tablet 115200
  Serial1.begin(9600);                 //Xbee 9600
  Serial2.begin(115200);               //EasyTransfer Volts 115200
  Serial3.begin(115200);               //EasyTransfer MPPTs 115200
  ET_VOLT.begin(details(volt_data), &Serial2);
  ET_MPPT.begin(details(mppt_data), &Serial3);
/*
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
    Serial1.println("ERROR INICIO SD");
    //sd.initErrorHalt();
  }
  */
START_INIT:

  if (CAN_OK == CAN.begin(CAN_1000KBPS)){
    Serial.println("CAN BUS Shield BMS-KKELLY iniciado!");
  }else{
    Serial.println("Falla de inicio CAN BUS Shield BMS-KELLY ... Reiniciando");
    delay(100);
    goto START_INIT;
  }
  attachInterrupt(0, MCP2515_ISR , FALLING); // start interrupt
  // rx buffer clearing
  while (Serial1.available() > 0) {
    byte c = Serial1.read();
  }
  //TEMPERATURAS ANÁLOGAS
  // initialize all the readings to 0:
  for (int thisReading1 = 0; thisReading1 < numReadings1; thisReading1++) {
    readings1[thisReading1] = 0;
  }
  for (int thisReading2 = 0; thisReading2 < numReadings2; thisReading2++) {
    readings2[thisReading2] = 0;
  }
}//END SETUP

/*///////////////////////// LOOP /////////////////////////*/

void loop(){
  /*///////////// LECTURA TEMPERATURAS ANÁLOGAS //////////////*/
currentTime = millis();
  if ((currentTime - lastTempTime) > 500) {
    // subtract the last reading:
    total1 = total1 - readings1[readIndex1];
    // read from the sensor:
    readings1[readIndex1] = ((analogRead(inputPin_Left)*(1.915/405)) - 1.375)/22.5*1000;    // FORMULA DANILO
    // ANTIGUA FORMULA: readings1[readIndex1] = ((analogRead(inputPin_Left) * (5.0 / 1024)) - 1.375) / 0.0225;
    
    // add the reading to the total:
    total1 = total1 + readings1[readIndex1];
    // advance to the next position in the array:
    readIndex1 = readIndex1 + 1;
    // if we're at the end of the array...
    if (readIndex1 >= numReadings1) {
      // ...wrap around to the beginning:
      readIndex1 = 0;
    }
    // calculate the average:
    average1 = total1 / numReadings1;
    // send it to the computer as ASCII digits
    total2 = total2 - readings2[readIndex2];
    // read from the sensor:
    readings2[readIndex2] = ((analogRead(inputPin_Right)*(1.915/405))-1.375)/22.5*1000;   // FORMULA DANILO
    // ANTIGUA FORMULA: readings1[readIndex1] = ((analogRead(inputPin_Right) * (5.0 / 1024)) - 1.375) / 0.0225;
    // add the reading to the total:
    total2 = total2 + readings2[readIndex2];
    // advance to the next position in the array:
    readIndex2 = readIndex2 + 1;
    // if we're at the end of the array...
    if (readIndex2 >= numReadings2) {
      // ...wrap around to the beginning:
      readIndex2 = 0;
    }
    // calculate the average:
    average2 = total2 / numReadings2;
    // send it to the computer as ASCII digits

    KELLY_DER[10] = average1;
    KELLY_IZQ[10]  = average2;
  }

/*////////////////////// RECEIVE MPPTS //////////////////////*/
  
  if(ET_MPPT.receiveData()){
    //Serial.println("ET MPPT RECEIVE!");
    if(check_mppt){
      //Serial.println("///////MPPTS//////");
      Serial.print("MPPT 1_uin = ");Serial.println((mppt_data.Uin_1));
      Serial.print("MPPT 1_iin = ");Serial.println((mppt_data.Iin_1));
      Serial.print("MPPT 1_uout = ");Serial.println((mppt_data.Uout_1));
      Serial.print("MPPT 1_uout_umax = ");Serial.println((mppt_data.uout_umax_1));  // BVLR
      Serial.print("MPPT 1_t_cooler = ");Serial.println((mppt_data.t_cooler_1));    // OVT
      Serial.print("MPPT 1_bateria = ");Serial.println((mppt_data.bateria_1));      // NOC
      Serial.print("MPPT 1_under_volt = ");Serial.println((mppt_data.under_volt_1));// UNDV
      Serial.print("MPPT 1_temp = ");Serial.println((mppt_data.temp_1));            // TEMP
      
      Serial.print("MPPT 2_uin = ");Serial.println((mppt_data.Uin_2));
      Serial.print("MPPT 2_iin = ");Serial.println((mppt_data.Iin_2));
      Serial.print("MPPT 2_uout = ");Serial.println((mppt_data.Uout_2));
      Serial.print("MPPT 2_uout_umax = ");Serial.println((mppt_data.uout_umax_2));
      Serial.print("MPPT 2_t_cooler = ");Serial.println((mppt_data.t_cooler_2));
      Serial.print("MPPT 2_bateria = ");Serial.println((mppt_data.bateria_2));
      Serial.print("MPPT 2_under_volt = ");Serial.println((mppt_data.under_volt_2));
      Serial.print("MPPT 2_temp = ");Serial.println((mppt_data.temp_2));

      Serial.print("MPPT 3_uin = ");Serial.println((mppt_data.Uin_3));
      Serial.print("MPPT 3_iin = ");Serial.println((mppt_data.Iin_3));
      Serial.print("MPPT 3_uout = ");Serial.println((mppt_data.Uout_3));
      Serial.print("MPPT 3_uout_umax = ");Serial.println((mppt_data.uout_umax_3));
      Serial.print("MPPT 3_t_cooler = ");Serial.println((mppt_data.t_cooler_3));
      Serial.print("MPPT 3_bateria = ");Serial.println((mppt_data.bateria_3));
      Serial.print("MPPT 3_under_volt = ");Serial.println((mppt_data.under_volt_3));
      Serial.print("MPPT 3_temp = ");Serial.println((mppt_data.temp_3));

      Serial.print("MPPT 4_uin = ");Serial.println((mppt_data.Uin_4));
      Serial.print("MPPT 4_iin = ");Serial.println((mppt_data.Iin_4));
      Serial.print("MPPT 4_uout = ");Serial.println((mppt_data.Uout_4));
      Serial.print("MPPT 4_uout_umax = ");Serial.println((mppt_data.uout_umax_4));
      Serial.print("MPPT 4_t_cooler = ");Serial.println((mppt_data.t_cooler_4));
      Serial.print("MPPT 4_bateria = ");Serial.println((mppt_data.bateria_4));
      Serial.print("MPPT 4_under_volt = ");Serial.println((mppt_data.under_volt_4));
      Serial.print("MPPT 4_temp = ");Serial.println((mppt_data.temp_4));
    }
    MPPT1[0]= mppt_data.Uin_1;
    MPPT1[1]= mppt_data.Iin_1;
    MPPT1[2]= mppt_data.Uout_1;
    MPPT1[3]= mppt_data.uout_umax_1;  // BLVR
    MPPT1[4]= mppt_data.t_cooler_1;   // OVT
    MPPT1[5]= mppt_data.bateria_1;    // NOC
    MPPT1[6]= mppt_data.under_volt_1; // UNDV
    MPPT1[7]= mppt_data.temp_1;       // TEMP
    potencia_mppt1= (mppt_data.Uin_1*mppt_data.Iin_1)/1000000; //Potencia en W
    
    MPPT2[0]= mppt_data.Uin_2;
    MPPT2[1]= mppt_data.Iin_2;
    MPPT2[2]= mppt_data.Uout_2;
    MPPT2[3]= mppt_data.uout_umax_2;
    MPPT2[4]= mppt_data.t_cooler_2;
    MPPT2[5]= mppt_data.bateria_2;
    MPPT2[6]= mppt_data.under_volt_2;
    MPPT2[7]= mppt_data.temp_2;
    potencia_mppt2= (mppt_data.Uin_2*mppt_data.Iin_2)/1000000; //Potencia en W
    
    MPPT3[0]= mppt_data.Uin_3;
    MPPT3[1]= mppt_data.Iin_3;
    MPPT3[2]= mppt_data.Uout_3;
    MPPT3[3]= mppt_data.uout_umax_3;
    MPPT3[4]= mppt_data.t_cooler_3;
    MPPT3[5]= mppt_data.bateria_3;
    MPPT3[6]= mppt_data.under_volt_3;
    MPPT3[7]= mppt_data.temp_3;
    potencia_mppt3= (mppt_data.Uin_3*mppt_data.Iin_3)/1000000; //Potencia en W
    
    MPPT4[0]= mppt_data.Uin_4;
    MPPT4[1]= mppt_data.Iin_4;
    MPPT4[2]= mppt_data.Uout_4;
    MPPT4[3]= mppt_data.uout_umax_4;
    MPPT4[4]= mppt_data.t_cooler_4;
    MPPT4[5]= mppt_data.bateria_4;
    MPPT4[6]= mppt_data.under_volt_4;
    MPPT4[7]= mppt_data.temp_4;
    potencia_mppt4= (mppt_data.Uin_4*mppt_data.Iin_4)/1000000; //Potencia en W
  }
  //Serial.print("MPPT2 UIN 6 dígitos:");PrintdataUnsignedVolt_6(MPPT2[0]);Serial.print(" MPPT_data_uin_2:");Serial.println(mppt_data.Uin_2);
  //Serial.print("MPPT2 IIN 6 dígitos:");PrintdataUnsignedVolt_6(MPPT2[1]);Serial.print(" MPPT_data_iin_2:");Serial.println(mppt_data.Iin_2);
  //Serial.print("MPPT2 UOUT 6 dígitos:");PrintdataUnsignedVolt_6(MPPT2[2]);Serial.print(" MPPT_data_uout_2:");Serial.println(mppt_data.Uout_2);

  
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
        UpdateSpeed(KELLY_DER[6],KELLY_IZQ[6]);  // Update de velocidad_final
        if(serial_exp){
          Serial.print("ENG1_RPM,");Serial.print((buff[0])<<8|buff[1]);Serial.print(" | ");
          Serial.print("ENG1_ERR_CODE,");Serial.print((buff[3])<<8|buff[4]);Serial.print("\n");
        }
        engData[0] += 1;
        
      }else if((B1111111&engData[0]) == 3){                        // Si el 1er digito de engineData es '3' se procede a leer la temperatura.
        KELLY_DER[8]= buff[0];     //PWM DER
        KELLY_DER[9]= buff[1];     //EMR DER
        //KELLY_DER[10]= buff[2];    //MOTOR TEMP DER
        KELLY_DER[11]= buff[3];    //KELLY TEMP DER
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
        if(serial_exp){
          Serial.print("ENG1_Current_Throttle_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Throttle Status
        }
        engData[0] += 1;
        
      }else if((B1111111&engData[0]) == 5){     
        KELLY_DER[13]=buff[0];   //REVERSE SWITCH STATUS DER
        if(serial_exp){
          Serial.print("ENG1_Current_Reserve_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Reverse Status
        }
        engData[0] = 1;
      }
      bitWrite(engData[0],7,1);

    //Kelly IZ//
    }else if((canId == sendIdKelly2) && !bitRead(engData[1],7)){

      if((B1111111&engData[1]) == 1){         
        KELLY_IZQ[0]= (buff[0],DEC);   //IA IZ
        KELLY_IZQ[1]= (buff[1],DEC);   //IB IZ
        KELLY_IZQ[2]= (buff[2],DEC);   //IC IZ
        KELLY_IZQ[3]= (buff[3]/1.84);  //VA IZ
        KELLY_IZQ[4]= (buff[4]/1.84);  //VB IZ
        KELLY_IZQ[5]= (buff[5]/1.84);  //VC IZ
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
        KELLY_IZQ[6]= ((buff[0])<<8|buff[1]); //RPM IZ
        KELLY_IZQ[7]= ((buff[3])<<8|buff[4]); //ERROR CODE IZ
        UpdateSpeed(KELLY_DER[6],KELLY_IZQ[6]);  // Update de velocidad_final
        if(serial_exp){
          Serial.print("ENG2_RPM,");Serial.print((buff[0])<<8|buff[1]);Serial.print(" | ");
          Serial.print("ENG2_ERR_CODE,");Serial.print((buff[3])<<8|buff[4]);Serial.print("\n");
        }
        engData[1] += 1;
        
      }else if((B1111111&engData[1]) == 3){                        // 2do digito de engineData es 3, lee temperatura.
        KELLY_IZQ[8]= buff[0];   //PWM IZ
        KELLY_IZQ[9]= buff[1];   //EMR IZ
        //KELLY_IZQ[10]= buff[2];   //MOTOR TEMP IZ
        KELLY_IZQ[11]= buff[3];   //KELLY TEMP IZ
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
        KELLY_IZQ[12]=buff[0];   //THROTLE SWITCH STATUS IZ
        if(serial_exp){
          Serial.print("ENG2_Current_Throttle_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Throttle Status
        }
        engData[1] += 1;
        
      }else if((B1111111&engData[1]) == 5){     
        KELLY_IZQ[13]=buff[0]; //REVERSE SWITCH STATUS IZ
        if(serial_exp){
          Serial.print("ENG2_Current_Reserve_Switch_Status,");Serial.print(buff[0]);Serial.print("\n");      // Reverse Status
        }
        engData[1] = 1;
      }
      bitWrite(engData[1],7,1);
    
/*//////////////////// FIN KELLYS ////////////////////*/
/*///////////////// LECTURA CAN: BMS //////////////////*/
    }else if(canId == 0x100){
      int packSOC         = buff[0]/2;
      double packCurrent     = (buff[1]<<8)|buff[2];
      int packInstVolt    = (buff[3]<<8)|buff[4];
      int packOpenVolt    = (buff[5]<<8)|buff[6];
      BMS[0]= packSOC;
      BMS[1]= int(abs(packCurrent));
      BMS[2]= packInstVolt;
      BMS[3]= packOpenVolt;
      if(serial_exp){
        Serial.print("PACK_SOC,");Serial.print(buff[0]/2);Serial.print("\n");        
        Serial.print("PACK_CURRENT,");Serial.print((buff[1]<<8)|buff[2]);Serial.print("\n");
        Serial.print("PACK_INST_VTG,");Serial.print((buff[3]<<8)|buff[4]);Serial.print("\n");
        Serial.print("PACK_OPEN_VTG,");Serial.print((buff[5]<<8)|buff[6]);Serial.print("\n");
      }
    }else if( canId == 0x101){
      int packAbsCurrent  = (buff[0]<<8)|buff[1];
      int maximumPackVolt = (buff[2]<<8)|buff[3];
      int minimumPackVolt = (buff[4]<<8)|buff[5];
      BMS[4]= packAbsCurrent;
      MAX_VOLT= maximumPackVolt;
      MIN_VOLT= minimumPackVolt;
      if(serial_exp){
        Serial.print("PACK_ABSCURRENT,");Serial.print(packAbsCurrent);Serial.print("\n");
        Serial.print("MAXIM_PACK_VTG,");Serial.print(maximumPackVolt);Serial.print("\n");
        Serial.print("MINIM_PACK_VTG,");Serial.print(minimumPackVolt);Serial.print("\n");
      }
  }else if( canId == 0x102){
      int highTemperature   = buff[0];
      int highThermistorID  = buff[1];
      int lowTemperature    = buff[2];
      int lowThermistorID   = buff[3];
      int avgTemp           = buff[4];
      int internalTemp      = buff[5];
      int max_volt_id       = buff[6];
      int min_volt_id       = buff[7];
      BMS[7]= highTemperature;
      BMS[8]= highThermistorID;
      BMS[9]= lowTemperature;
      BMS[10]= lowThermistorID;
      BMS[11]= avgTemp;
      BMS[12]= internalTemp;
      BMS[13]= min_volt_id;
      BMS[14]= min_volt_id;
      if(serial_exp){
        Serial.print("HIGH_TEMP,");Serial.print(highTemperature);Serial.print("\n");
        Serial.print("LOW_TEMP,");Serial.print(lowTemperature);Serial.print("\n");
        Serial.print("HIGH_TID,");Serial.print(highThermistorID);Serial.print("\n");
        Serial.print("LOW_TID,");Serial.print(lowThermistorID);Serial.print("\n");
        Serial.print("AVG_TEMP,");Serial.print(avgTemp);Serial.print("\n");
        Serial.print("INT_TEMP,");Serial.print(internalTemp);Serial.print("\n");
        Serial.print("MAX_VOLT_ID,");Serial.print(max_volt_id);Serial.print("\n");
        Serial.print("MIN_VOLT_ID,");Serial.print(min_volt_id);Serial.print("\n");
      }
  }else if(canId == 0x081){ // Lectura de temperaturas
      int thermistorID = buff[0];
      int temperature = buff[1];
      if((thermistorID >= 0)&&(thermistorID < 60)){
        BMS_TEMP[thermistorID] = temperature;
      }
      if(serial_exp){
          Serial.print("Temperatura Nro ");Serial.print(thermistorID);Serial.print(": ");Serial.print(temperature);Serial.print("\n");
      }
  }else if(canId == 0x082){ // Lectura de temperaturas
      int thermistorID = buff[1];
      int temperature = buff[2];
      if((thermistorID >= 0)&&(thermistorID < 60)){
        BMS_TEMP[thermistorID] = temperature;
      }
      if(serial_exp){
          Serial.print("Temperatura Nro ");Serial.print(thermistorID);Serial.print(": ");Serial.print(temperature);Serial.print("\n");
        }
      }
  }//Fin CAN.checkReceive()

/*//////////////////// RECEIVE VOLTAGES ////////////////////*/
  //check and see if a data packet has come in. 
  if(ET_VOLT.receiveData()){
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out.
    //Serial.println("RECIVO VOLTAJES");
    //Serial.print(volt_data.cellID-37);Serial.print(" = ");Serial.println(volt_data.voltage);
    if((volt_data.cellID>=36)&&(volt_data.cellID<=64)){ // Modulos del 37 al 64 por segundo conector
      BMS_VOLT[volt_data.cellID-36]=volt_data.voltage;
      //for(int i=0;i<30;i++){
        // Serial.print(i);Serial.print(" = ");Serial.println(BMS_VOLT[i]);
      //}
    }
      if(serial_exp){
        Serial.print("Voltaje modulo ");Serial.print(volt_data.cellID);Serial.print(":  ");Serial.print(volt_data.voltage);Serial.print("\n");
      }
  }
  //delay(100);


/*//////////////////// FIN LECTURA DATOS ////////////////////*/
  
if (xbee){
  Send_Xbee_all();
}
if (tablet){
  Send_tablet_data();
}

/*//////////////////////// SD ////////////////////////*/
/*  
  //Actualizar TiempoGlobal
  TiempoGlobal = millis();

  //Escribir en archivo de SD
  if (!myFile.open(archivo, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("ERROR EN ESCRITURA DE SD AL ABRIR .txt");
    Serial.println("ERROR EN ESCRITURA DE SD AL ABRIR .txt");
    //sd.errorHalt("opening archivo.txt for write failed");
  }else{
    EscribirSD();
    myFile.close();
  }
*/////////////////////// FIN SD //////////////////////*/

//Limpieza de Puertos Seriales
  Serial.flush();
  Serial1.flush();
  Serial2.flush();
  Serial3.flush();

//Fin Loop
}

