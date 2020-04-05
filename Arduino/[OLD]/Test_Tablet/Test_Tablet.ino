void setup() {
  Serial.begin(9600);

}

void loop() {
  for(int i = 0;i < 30;i++){
    Serial.print("ENGA,");Serial.print(1);Serial.print("\n");
    delay(100);
    Serial.print("PACK_SOC,");Serial.print(i);Serial.print("\n");
    delay(100);
    Serial.print("PACK_CURRENT,");Serial.print(i+3);Serial.print("\n");
    delay(100);
    Serial.print("PACK_INST_VTG,");Serial.print(i+15);Serial.print("\n");
    delay(100);
    Serial.print("MAXIM_PACK_VTG,");Serial.print(i+12);Serial.print("\n");
    delay(100);
    Serial.print("MINIM_PACK_TG,");Serial.print(i+26);Serial.print("\n");
    delay(100);
    Serial.print("HIGH_TEMP,");Serial.print(i+3);Serial.print("\n");
    delay(100);
    Serial.print("LOW_TEMP,");Serial.print(i+10);Serial.print("\n");
    delay(100);
    Serial.print("HIGH_TID,");Serial.print(i+8);Serial.print("\n");
    delay(100);
    Serial.print("LOW_TID,");Serial.print(i+30);Serial.print("\n");
    delay(100);
    Serial.print("AVG_TEMP,");Serial.print(i+30);Serial.print("\n");
    delay(100);
    
    
    }
}
