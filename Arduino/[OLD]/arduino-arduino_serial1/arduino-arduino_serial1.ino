/*#include <SoftwareSerial.h>

SoftwareSerial myserial(4,5); //Rx,Tx


void setup() {
  myserial.begin(9600);

}

void loop() {
 myserial.println('DOTA2');
 delay(50);
 

}

*/

/*#include <SoftwareSerial.h>

SoftwareSerial myserial = SoftwareSerial(4,5); //Rx,Tx

*/
void setup() {
  
 // pinMode(4, INPUT);
 // pinMode(5, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  if (Serial.available()){
  Serial.println(Serial.read());
 }
}

