/*  EOLIAN FENIX - 2017 - UNIVERSIDAD DE CHILE
 *  PROGRAMA MONITOR MPPT.
 *  PERMITE LEER DATOS MPPT
  */
/*///////////////////// VARIABLES CAN /////////////////////*/
#include <mcp_can.h>
#include <SPI.h>
const int SPI_CS_PIN = 9;
unsigned char len = 0;
unsigned char buff[7];
unsigned long lastMpptTime = 0;
unsigned long actualTime = 0;
MCP_CAN CAN(SPI_CS_PIN);

/*///////////////////// EASY TRANSFER /////////////////////*/
#include <EasyTransfer.h>
EasyTransfer ET;
struct SEND_DATA_STRUCTURE{
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
SEND_DATA_S
TRUCTURE mydata;
unsigned long last_ET=0;
/*///////////////////// VARIABLES GLOBALES /////////////////////*/
bool serial_direct = false; // Visualización anterior
bool easy_transfer =true;  // Envío por EasyTransfer

/*///////////////////// SET UP /////////////////////*/
void setup() {
  // ET
  Serial.begin(115200);
  ET.begin(details(mydata), &Serial);

  // CAN BUS
  while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("NO SE HA PODIDO INICIAR CAN BUS");
        Serial.println("INICIANDO NUEVAMENTE");
        delay(500);
    }
    Serial.println("CAN BUS INICIADO");
    Serial.println("PROGRAMA MONITOR MPPT EOLIAN FENIX OK");
}

/*///////////////////// LOOP /////////////////////*/
void loop() {
  actualTime = millis();

  if((actualTime - lastMpptTime) > 500){
    //Serial.println("Sending Request to MPPTs");
    CAN.sendMsgBuf(0x711, 0, 0, 0);
    CAN.sendMsgBuf(0x712, 0, 0, 0);
    CAN.sendMsgBuf(0x713, 0, 0, 0);
    CAN.sendMsgBuf(0x714, 0, 0, 0);
    CAN.sendMsgBuf(0x715, 0, 0, 0);
    CAN.sendMsgBuf(0x716, 0, 0, 0);
    lastMpptTime = millis();
  }
  
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, buff);
    unsigned int canId = CAN.getCanId();

    if (canId == 0x771){
      unsigned int Uin = (((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49;
      unsigned int Iin = (((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72;
      unsigned int Uout = (((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79;
      unsigned int uout_umax = bitRead(buff[0],7);
      unsigned int t_cooler = bitRead(buff[0],6);
      unsigned int bateria = bitRead(buff[0],5);
      unsigned int under_volt = bitRead(buff[0],4);
      unsigned int temp = buff[6];
      mydata.Uin_1 = Uin;
      mydata.Iin_1 = Iin;
      mydata.Uout_1 = Uout;
      mydata.uout_umax_1 = uout_umax;
      mydata.t_cooler_1 = t_cooler;
      mydata.bateria_1 = bateria;
      mydata.under_volt_1 = under_volt;
      mydata.temp_1 = temp;
    
    }else if (canId == 0x772){
      unsigned int Uin = (((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49;
      unsigned int Iin = (((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72;
      unsigned int Uout = (((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79;
      unsigned int uout_umax = bitRead(buff[0],7);
      unsigned int t_cooler = bitRead(buff[0],6);
      unsigned int bateria = bitRead(buff[0],5);
      unsigned int under_volt = bitRead(buff[0],4);
      unsigned int temp = buff[6];
      mydata.Uin_2 = Uin;
      mydata.Iin_2 = Iin;
      mydata.Uout_2 = Uout;
      mydata.uout_umax_2 = uout_umax;
      mydata.t_cooler_2 = t_cooler;
      mydata.bateria_2 = bateria;
      mydata.under_volt_2 = under_volt;
      mydata.temp_2 = temp;
      
    } else if (canId == 0x773){
      unsigned int Uin = (((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49;
      unsigned int Iin = (((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72;
      unsigned int Uout = (((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79;
      unsigned int uout_umax = bitRead(buff[0],7);
      unsigned int t_cooler = bitRead(buff[0],6);
      unsigned int bateria = bitRead(buff[0],5);
      unsigned int under_volt = bitRead(buff[0],4);
      unsigned int temp = buff[6];
      mydata.Uin_3 = Uin;
      mydata.Iin_3 = Iin;
      mydata.Uout_3 = Uout;
      mydata.uout_umax_3 = uout_umax;
      mydata.t_cooler_3 = t_cooler;
      mydata.bateria_3 = bateria;
      mydata.under_volt_3 = under_volt;
      mydata.temp_3 = temp;
      
    } else if (canId == 0x774){
      unsigned int Uin = (((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49;
      unsigned int Iin = (((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72;
      unsigned int Uout = (((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79;
      unsigned int uout_umax = bitRead(buff[0],7);
      unsigned int t_cooler = bitRead(buff[0],6);
      unsigned int bateria = bitRead(buff[0],5);
      unsigned int under_volt = bitRead(buff[0],4);
      unsigned int temp = buff[6];
      mydata.Uin_4 = Uin;
      mydata.Iin_4 = Iin;
      mydata.Uout_4 = Uout;
      mydata.uout_umax_4 = uout_umax;
      mydata.t_cooler_4 = t_cooler;
      mydata.bateria_4 = bateria;
      mydata.under_volt_4 = under_volt;
      mydata.temp_4 = temp;
     
    }else if (canId == 0x775){
      unsigned int Uin = (((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49;
      unsigned int Iin = (((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72;
      unsigned int Uout = (((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79;
      unsigned int uout_umax = bitRead(buff[0],7);
      unsigned int t_cooler = bitRead(buff[0],6);
      unsigned int bateria = bitRead(buff[0],5);
      unsigned int under_volt = bitRead(buff[0],4);
      unsigned int temp = buff[6];
      mydata.Uin_5 = Uin;
      mydata.Iin_5 = Iin;
      mydata.Uout_5 = Uout;
      mydata.uout_umax_5 = uout_umax;
      mydata.t_cooler_5 = t_cooler;
      mydata.bateria_5 = bateria;
      mydata.under_volt_5 = under_volt;
      mydata.temp_5 = temp;
    }
    
    if(serial_direct){
      Serial.print("ID:");Serial.print("\t");Serial.print(canId,HEX);Serial.print("\t");
      Serial.print("Uin:");Serial.print("\t");Serial.print((((bitRead(buff[0],1)<<1|bitRead(buff[0],0))<<8)|buff[1])*150.49);Serial.print("\t");
      Serial.print("Iin:");Serial.print("\t");Serial.print((((bitRead(buff[2],1)<<1|bitRead(buff[2],0))<<8)|buff[3])*8.72);Serial.print("\t");
      Serial.print("Uout:");Serial.print((((bitRead(buff[4],1)<<1|bitRead(buff[4],0))<<8)|buff[5])*208.79);Serial.print("\t");
      if(bitRead(buff[0],7) == 1){
        Serial.print("Uout = Umax");Serial.print("\t");
      } else {
        Serial.print("Uout < Umax");Serial.print("\t");
      }
      if(bitRead(buff[0],6) == 1){
        Serial.print("Tcooler >= 95°C");Serial.print("\t");
      } else {
        Serial.print("Tcooler < 95°C");Serial.print("\t");
      }
      if(bitRead(buff[0],5) == 1){
        Serial.print("Bateria NO conectada");Serial.print("\t");
      } else {
        Serial.print("Batería conectada");Serial.print("\t");
      }
      if(bitRead(buff[0],4) == 1){
        Serial.print("Uin <= 26V");Serial.println("\t");
      } else {
        Serial.print("Uin > 26V");Serial.println("\t");
      }
    }
    
  }// FIn Check Receive()

  if ((easy_transfer)&&(actualTime-last_ET>100)){
    ET.sendData();
    last_ET = actualTime;
  }
  Serial.flush();
}
