#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
 /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
*/

 // Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
 // mySerial.println("Hello, world?");
}

void loop() { // run over and over

  //Serial.println("Goodnight moon!");
 // mySerial.println("Hello, world?");
  //delay(50);
  
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
 // if (Serial.available()) {
 //   mySerial.write(Serial.read());
 // }
}
