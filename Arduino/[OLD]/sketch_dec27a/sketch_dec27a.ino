char outData[6];
char salto = 10;
char coma = 44;

int valor1= (char)90;
int valor2= (char)91;

int valor3 = 90;
int valor4 = 91;


void SendOutData(){
  for(unsigned char charToSend:outData){
    Serial.write(charToSend);
    //Serial.println(charToSend);
  }
}
  
void setup() {
  Serial.begin(9600);
  outData[1] = coma;
  outData[3] = coma;
  outData[5] = salto;

  
}

void loop(){
for(int i=0;i<10;i++){
Serial.print("A");Serial.print(",");Serial.println(valor3+i);

//Serial.print(",");Serial.println(valor4+i);
//  outData[5] = valor3;  // Dato2 int
//  outData[6] = valor4;
delay(800);
}
}
