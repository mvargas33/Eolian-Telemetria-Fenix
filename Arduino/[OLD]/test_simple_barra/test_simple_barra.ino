char outData[6];
char salto = 10;
char coma = 44;

int valor1= (char)65;


int valor2= (char)91;


void SendOutData(){
  for(unsigned char charToSend:outData){
    Serial.write(charToSend);
    //Serial.println(charToSend);
  }
}
  
void setup() {
  Serial.begin(9600);
  outData[1] = coma;
  //outData[3] = coma;
  outData[5] = salto;

  
}

void loop(){

  outData[0] = 65;  // 'A'id
  for (int i=0;i<10;i++){
  outData[2] = valor1+i;  // Dato1 int
  //outData[4] = valor2+i;
  delay(1000);
  SendOutData();
  }
//  outData[5] = valor3;  // Dato2 int
//  outData[6] = valor4;

}
