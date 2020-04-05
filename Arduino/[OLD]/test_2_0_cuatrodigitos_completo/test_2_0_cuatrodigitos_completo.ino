/*//////////////////// VECTORES PRINCIPALES ////////////////////*/
int BMS[15];              // 14 y 15 Max/Min volt ID
unsigned int BMS_VOLT[30];
int BMS_TEMP[60];
int KELLY_DER[14];
int KELLY_IZ[14];
int MPPT1[8];
int MPPT2[8];
int MPPT3[8];
int MPPT4[8];

int velocidad_final=0;
int incremento=1;
int delay_4 = 5;         // 5ms de delay cada 4 valores

/*//////////////////// PRINT DE 4 DIGITOS INT ////////////////////*/
void Printdata(int entero){
  if (entero == 0){
    Serial.print("0000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial.print("000");Serial.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial.print("00");Serial.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial.print("0");Serial.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial.print(entero);
  }  
}

/*//////////////////// PRINT DE 4 DIGITOS UNSIGNED INT ////////////////////*/
void PrintdataUnsigned(int entero){
  if (entero == 0){
    Serial.print("0000");
  }
  else if ((0<entero)&&(entero<10)){
    Serial.print("000");Serial.print(entero);
  }
  else if ((10<=entero)&&(entero<100)){
    Serial.print("00");Serial.print(entero);
  }
  else if ((100<=entero)&&(entero<1000)){
    Serial.print("0");Serial.print(entero);
  }
  else if ((1000<=entero)&&(entero<10000)){
    Serial.print(entero);
  }
}
/*//////////////////// SEND ALL DATA ////////////////////*/

//Envía por Serial0 todos los datos
void Send_FullData(){

  ///////// BMS  /////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);
  Printdata(BMS[0]);
  Printdata(BMS[1]);
  Printdata(BMS[2]);
  delay(delay_4);
  Serial.print(char(0x21));
  Printdata(velocidad_final);
  Printdata(BMS[3]);
  Printdata(BMS[4]);
  Printdata(BMS[5]);
  delay(delay_4);
  Serial.print(char(0x22));
  Printdata(velocidad_final);
  Printdata(BMS[6]);
  Printdata(BMS[7]);
  Printdata(BMS[8]);
  delay(delay_4);
  Serial.print(char(0x23));
  Printdata(velocidad_final);
  Printdata(BMS[9]);
  Printdata(BMS[10]);
  Printdata(BMS[11]);
  delay(delay_4);
  Serial.print(char(0x24));
  Printdata(velocidad_final);
  Printdata(BMS[12]);
  Printdata(BMS[13]);
  Printdata(BMS[14]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////

  /////// BMS_VOLT ///////
  Serial.print(char(0x25));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[0]);
  PrintdataUnsigned(BMS_VOLT[1]);
  PrintdataUnsigned(BMS_VOLT[2]);
  delay(delay_4);
  Serial.print(char(0x26));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[3]);
  PrintdataUnsigned(BMS_VOLT[4]);
  PrintdataUnsigned(BMS_VOLT[5]);
  delay(delay_4);
  Serial.print(char(0x27));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[6]);
  PrintdataUnsigned(BMS_VOLT[7]);
  PrintdataUnsigned(BMS_VOLT[8]);
  delay(delay_4);
  Serial.print(char(0x28));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[9]);
  PrintdataUnsigned(BMS_VOLT[10]);
  PrintdataUnsigned(BMS_VOLT[11]);
  delay(delay_4);
  Serial.print(char(0x29));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[12]);
  PrintdataUnsigned(BMS_VOLT[13]);
  PrintdataUnsigned(BMS_VOLT[14]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  Serial.print(char(0x30));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[15]);
  PrintdataUnsigned(BMS_VOLT[16]);
  PrintdataUnsigned(BMS_VOLT[17]);
  delay(delay_4);
  Serial.print(char(0x31));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[18]);
  PrintdataUnsigned(BMS_VOLT[19]);
  PrintdataUnsigned(BMS_VOLT[20]);
  delay(delay_4);
  Serial.print(char(0x32));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[21]);
  PrintdataUnsigned(BMS_VOLT[22]);
  PrintdataUnsigned(BMS_VOLT[23]);
  delay(delay_4);
  Serial.print(char(0x33));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[24]);
  PrintdataUnsigned(BMS_VOLT[25]);
  PrintdataUnsigned(BMS_VOLT[26]);
  delay(delay_4);
  Serial.print(char(0x34));
  Printdata(velocidad_final);
  PrintdataUnsigned(BMS_VOLT[27]);
  PrintdataUnsigned(BMS_VOLT[28]);
  PrintdataUnsigned(BMS_VOLT[29]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  /////// BMS_TEMP ///////
  Serial.print(char(0x35));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[0]);
  Printdata(BMS_TEMP[1]);
  Printdata(BMS_TEMP[2]);
  delay(delay_4);
  Serial.print(char(0x36));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[3]);
  Printdata(BMS_TEMP[4]);
  Printdata(BMS_TEMP[5]);
  delay(delay_4);
  Serial.print(char(0x37));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[6]);
  Printdata(BMS_TEMP[7]);
  Printdata(BMS_TEMP[8]);
  delay(delay_4);
  Serial.print(char(0x38));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[9]);
  Printdata(BMS_TEMP[10]);
  Printdata(BMS_TEMP[11]);
  delay(delay_4);
  Serial.print(char(0x39));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[12]);
  Printdata(BMS_TEMP[13]);
  Printdata(BMS_TEMP[14]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  Serial.print(char(0x40));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[15]);
  Printdata(BMS_TEMP[16]);
  Printdata(BMS_TEMP[17]);
  delay(delay_4);
  Serial.print(char(0x41));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[18]);
  Printdata(BMS_TEMP[19]);
  Printdata(BMS_TEMP[20]);
  delay(delay_4);
  Serial.print(char(0x42));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[21]);
  Printdata(BMS_TEMP[22]);
  Printdata(BMS_TEMP[23]);
  delay(delay_4);
  Serial.print(char(0x43));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[24]);
  Printdata(BMS_TEMP[25]);
  Printdata(BMS_TEMP[26]);
  delay(delay_4);
  Serial.print(char(0x44));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[27]);
  Printdata(BMS_TEMP[28]);
  Printdata(BMS_TEMP[29]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  Serial.print(char(0x45));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[30]);
  Printdata(BMS_TEMP[31]);
  Printdata(BMS_TEMP[32]);
  delay(delay_4);
  Serial.print(char(0x46));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[33]);
  Printdata(BMS_TEMP[34]);
  Printdata(BMS_TEMP[35]);
  delay(delay_4);
  Serial.print(char(0x47));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[36]);
  Printdata(BMS_TEMP[37]);
  Printdata(BMS_TEMP[38]);
  delay(delay_4);
  Serial.print(char(0x48));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[39]);
  Printdata(BMS_TEMP[40]);
  Printdata(BMS_TEMP[41]);
  delay(delay_4);
  Serial.print(char(0x49));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[42]);
  Printdata(BMS_TEMP[43]);
  Printdata(BMS_TEMP[44]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  Serial.print(char(0x50));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[45]);
  Printdata(BMS_TEMP[46]);
  Printdata(BMS_TEMP[47]);
  delay(delay_4);
  Serial.print(char(0x51));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[48]);
  Printdata(BMS_TEMP[49]);
  Printdata(BMS_TEMP[50]);
  delay(delay_4);
  Serial.print(char(0x52));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[51]);
  Printdata(BMS_TEMP[52]);
  Printdata(BMS_TEMP[53]);
  delay(delay_4);
  Serial.print(char(0x53));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[54]);
  Printdata(BMS_TEMP[55]);
  Printdata(BMS_TEMP[56]);
  delay(delay_4);
  Serial.print(char(0x54));
  Printdata(velocidad_final);
  Printdata(BMS_TEMP[57]);
  Printdata(BMS_TEMP[58]);
  Printdata(BMS_TEMP[59]);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////

  /////// KELLY_DER ///////
  Serial.print(char(0x55));
  Printdata(velocidad_final);
  Printdata(KELLY_DER[0]);
  Printdata(KELLY_DER[1]);
  Printdata(KELLY_DER[2]);
  delay(delay_4);
  Serial.print(char(0x56));
  Printdata(velocidad_final);
  Printdata(KELLY_DER[3]);
  Printdata(KELLY_DER[4]);
  Printdata(KELLY_DER[5]);
  delay(delay_4);
  Serial.print(char(0x57));
  Printdata(velocidad_final);
  Printdata(KELLY_DER[6]);
  Printdata(KELLY_DER[7]);
  Printdata(KELLY_DER[8]);
  delay(delay_4);
  Serial.print(char(0x58));
  Printdata(velocidad_final);
  Printdata(KELLY_DER[9]);
  Printdata(KELLY_DER[10]);
  Printdata(KELLY_DER[11]);
  delay(delay_4);
  Serial.print(char(0x59));
  Printdata(velocidad_final);
  Printdata(KELLY_DER[12]);
  Printdata(KELLY_DER[13]);
  Printdata(0);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  /////// KELLY_IZ ///////
  Serial.print(char(0x60));
  Printdata(velocidad_final);
  Printdata(KELLY_IZ[0]);
  Printdata(KELLY_IZ[1]);
  Printdata(KELLY_IZ[2]);
  delay(delay_4);
  Serial.print(char(0x61));
  Printdata(velocidad_final);
  Printdata(KELLY_IZ[3]);
  Printdata(KELLY_IZ[4]);
  Printdata(KELLY_IZ[5]);
  delay(delay_4);
  Serial.print(char(0x62));
  Printdata(velocidad_final);
  Printdata(KELLY_IZ[6]);
  Printdata(KELLY_IZ[7]);
  Printdata(KELLY_IZ[8]);
  delay(delay_4);
  Serial.print(char(0x63));
  Printdata(velocidad_final);
  Printdata(KELLY_IZ[9]);
  Printdata(KELLY_IZ[10]);
  Printdata(KELLY_IZ[11]);
  delay(delay_4);
  Serial.print(char(0x64));
  Printdata(velocidad_final);
  Printdata(KELLY_IZ[12]);
  Printdata(KELLY_IZ[13]);
  Printdata(0);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  ///////// MPPT1 /////////
  Serial.print(char(0x65));
  Printdata(velocidad_final);
  Printdata(MPPT1[0]);
  Printdata(MPPT1[1]);
  Printdata(MPPT1[2]);
  delay(delay_4);
  Serial.print(char(0x66));
  Printdata(velocidad_final);
  Printdata(MPPT1[3]);
  Printdata(MPPT1[4]);
  Printdata(MPPT1[5]);
  delay(delay_4);
  Serial.print(char(0x67));
  Printdata(velocidad_final);
  Printdata(MPPT1[6]);
  Printdata(MPPT1[7]);
  Printdata(0);
  delay(delay_4);

  ///////// MPPT2 /////////
  Serial.print(char(0x68));
  Printdata(velocidad_final);
  Printdata(MPPT2[0]);
  Printdata(MPPT2[1]);
  Printdata(MPPT2[2]);
  delay(delay_4);
  Serial.print(char(0x69));
  Printdata(velocidad_final);
  Printdata(MPPT2[3]);
  Printdata(MPPT2[4]);
  Printdata(MPPT2[5]);
  delay(delay_4);
  Serial.print(char(0x70));
  Printdata(velocidad_final);
  Printdata(MPPT2[6]);
  Printdata(MPPT2[7]);
  Printdata(0);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
  
  ///////// MPPT3 /////////
  Serial.print(char(0x71));
  Printdata(velocidad_final);
  Printdata(MPPT3[0]);
  Printdata(MPPT3[1]);
  Printdata(MPPT3[2]);
  delay(delay_4);
  Serial.print(char(0x72));
  Printdata(velocidad_final);
  Printdata(MPPT3[3]);
  Printdata(MPPT3[4]);
  Printdata(MPPT3[5]);
  delay(delay_4);
  Serial.print(char(0x73));
  Printdata(velocidad_final);
  Printdata(MPPT3[6]);
  Printdata(MPPT3[7]);
  Printdata(0);
  delay(delay_4);

  ///////// MPPT4 /////////
  Serial.print(char(0x74));
  Printdata(velocidad_final);
  Printdata(MPPT4[0]);
  Printdata(MPPT4[1]);
  Printdata(MPPT4[2]);
  delay(delay_4);
  Serial.print(char(0x75));
  Printdata(velocidad_final);
  Printdata(MPPT4[3]);
  Printdata(MPPT4[4]);
  Printdata(MPPT4[5]);
  delay(delay_4);
  Serial.print(char(0x76));
  Printdata(velocidad_final);
  Printdata(MPPT4[6]);
  Printdata(MPPT4[7]);
  Printdata(0);
  delay(delay_4);

  ///////// EXTRA ////////
  Serial.print(char(0x20));
  Printdata(velocidad_final);// Velocidad
  Printdata(BMS[0]);         // SOC
  Printdata(BMS[1]);         // Corriente Total
  Printdata(BMS[2]);         // Voltaje Total   
  delay(delay_4);
  ///////// EXTRA ////////
}

void setup() {

  for(int i=0;i<15;i++){
    BMS[i]=i+10;
  }
  for(unsigned int i=0;i<30;i++){
    BMS_VOLT[i]=i+2000;
  }
  for(int i=0;i<60;i++){
    BMS_TEMP[i]=i;
  }
  for(int i=0;i<14;i++){
    KELLY_DER[i]=i+100;
  }
  for(int i=0;i<14;i++){
    KELLY_IZ[i]=i+200;
  }
  for(int i=0;i<8;i++){
    MPPT1[i]=i+500;
  }
  for(int i=0;i<8;i++){
    MPPT2[i]=i+600;
  }
  for(int i=0;i<8;i++){
    MPPT3[i]=i+700;
  }
  for(int i=0;i<8;i++){
    MPPT4[i]=i+800;
  }

  Serial.begin(115200);     //Tablet

}

void loop() {
  
  for(int i=0;i<15;i++){
    BMS[i]=i+10+incremento;
  }
  for(unsigned int i=0;i<30;i++){
    BMS_VOLT[i]=i+4000+incremento;
  }
  for(int i=0;i<60;i++){
    BMS_TEMP[i]=i+incremento;
  }
  for(int i=0;i<14;i++){
    KELLY_DER[i]=i+100+incremento;
  }
  for(int i=0;i<14;i++){
    KELLY_IZ[i]=i+200+incremento;
  }
  for(int i=0;i<8;i++){
    MPPT1[i]=i+500+incremento;
  }
  for(int i=0;i<8;i++){
    MPPT2[i]=i+600+incremento;
  }
  for(int i=0;i<8;i++){
    MPPT3[i]=i+700+incremento;
  }
  for(int i=0;i<8;i++){
    MPPT4[i]=i+800+incremento;
  }

  if(velocidad_final==150){
    velocidad_final=1;
  }else{
    velocidad_final+=1;
  }
  
  if(incremento==57){
    incremento=0;
  }else{
    incremento+=1;
  }

  //Envío de vectores
  Send_FullData();
  //Limpieza de Puertos Seriales
  Serial.flush();

}
