float i;

void setup() {
 Serial.begin(9600);
}

void loop() {

  for(i=3.1415;i<101;i++){
     
  Serial.print( "Holaaa, ");Serial.println(i);
  delay(1000);
 
  }
}
