int vtg[12];
int aux=0;
int zero=0;

bool zeros(int vector[12], int desde){
  int largo =12;
  while(desde<12){
    if (vector[desde]!=0){
      return false;
    }else{
      desde+=1;     
    }
  }
  return true;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
/*
  //Serial.println(sizeof(vtg));
  if(zeros(vtg)){
    Serial.println("DETECTO ZEROS!");
    for (int i=0;i<12;i++){
        vtg[i]=aux+20;
        aux++;
    }
      
  }else{
      Serial.print("Vector: ");

      for(int j=0;j<11;j++){
        Serial.print(vtg[j]);
        Serial.print(",");
      }
      Serial.println(vtg[11]);
      
      memset(vtg, 0, sizeof(vtg));
      
      Serial.print("Vector0: ");

      for(int j=0;j<11;j++){
        Serial.print(vtg[j]);
        Serial.print(",");
      }
      Serial.println(vtg[11]);
      
      aux=0;
  }
    

*/
for(int i=0;i<10;i++){
Serial.print("A");Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(5000+i*1000);Serial.print(",");Serial.print(7000+i*1000);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(50+i);Serial.print(",");Serial.print(20+i);Serial.print(",");Serial.println(60+i);
Serial.print("c");Serial.print(",");Serial.print(500-i*7);Serial.print(",");Serial.println(2);
Serial.print("d");Serial.print(",");Serial.print(499-i*7);Serial.print(",");Serial.println(2);
Serial.print("V");Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(4000+i*1000);Serial.print("\n");
Serial.print("T");Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print(",");Serial.print(0+i);Serial.print(",");Serial.print(20+i*2);Serial.print("\n");
delay(400);

}

//delay(1000);
}

