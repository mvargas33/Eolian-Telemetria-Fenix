void setup() {
 Serial.begin(115200);
}

void loop() {
  Serial.print(char(0x47));
          Serial.print("0045");
          Serial.print("0025");
          Serial.print("0056");
          Serial.print("0012");
  
          Serial.print("0011");
          Serial.print("0012");
          Serial.print("0025");
          Serial.print("0023");
          Serial.print("0052");
          Serial.print("0080");
          delay(5);

}
