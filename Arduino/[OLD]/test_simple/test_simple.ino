char outData[5];
char salto = 10;
char coma = 44;
char id = 65;
unsigned char buff[2];

void SendOutData(){
  for(unsigned char charToSend:outData){
    Serial.write(charToSend);
    //Serial.println(charToSend);
  }
}

void setup() {
  Serial.begin(9600);
  //outData[0] = fijo;
  outData[1] = coma;
  outData[3] = salto;
}

void loop() {
  outData[0]=id;
  for (int i=0; i<2; i++){
      buff[i] = i+50;
  }

  for (int m=0; m<2; m++){
    outData[m+2]=buff[m];
  }

SendOutData();
}
