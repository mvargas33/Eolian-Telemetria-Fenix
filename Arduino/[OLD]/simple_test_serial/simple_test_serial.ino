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

int buff[5];
int as=0;
void setup() {
  
  Serial.begin(115200);

  
  buff[0]=123;
  buff[1]=4506;
  buff[2]=44;
  buff[3]=21;
  buff[4]=295;
  
}

void loop() {
  Serial.print(char(0x61));
  for(int i=0;i<5;i++){
  buff[i]=buff[i]+1;
  }
  
  for(int i=0;i<5;i++){
  Printdata(buff[i]);
  }
  if(as=5000){
    as=0;
  }
  //Serial.println("");
  delay(75);
  as+=1;
}

