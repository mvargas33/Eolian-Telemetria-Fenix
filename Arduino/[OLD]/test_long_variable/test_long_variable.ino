int VARIABLE[20];

void setup()
{
Serial.begin(9600);

for(int i=0;i<20;i++){
  VARIABLE[i] = i+10;  
}

}

void loop() {
Serial.println(sizeof(VARIABLE));
for(int i=0;i<20;i++){
  Serial.print(VARIABLE[i]);Serial.println(",");
}

}
