int BMS[12];
int aux=0;
void SendBMS(){
  Serial.print("A");
  Serial.print(",");

for(int i=0;i<11;i++){
  Serial.print(BMS[i]);
  Serial.print(",");
}
  Serial.print(BMS[11]);
  Serial.print("\n");
}

void setup() {
 Serial.begin(9600);
}

void loop() {
  if (aux<1000){
    for(int i=0;i<12;i++){
      BMS[i]= aux+i;
    }
  }
  
  
  SendBMS();
  Serial1.flush();
  aux+=1;
  delay(800);
}
