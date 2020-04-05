#include <SoftwareSerial.h>
int rx = 10;
int tx = 11;

SoftwareSerial mySerial(rx, tx); // RX, TX

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

int a;
int b= 110;
int c= 123;
int d= 90;

char uno[4];
uno[0] = a;
uno[1] = b;
uno[2] = c;
uno[3] = d;



  for(char c : uno){
    Serial.print(c);
  }
  





  delay(50);
  
 /* while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
  int i = 100;
  while(i--){
  Serial.print("Cosa ");Serial.println(i);
  }
  while(i<100) {
    Serial.print("Cosa ");Serial.println(++i);
  }
  Serial.println("Cosarf ");
  Serial.println("Cosa44r4rf4fr ");
  Serial.println("Cosa4rf4v4rrf44rf4r ");
  Serial.println("Cosa4r4rf3frvrf4frr4refvr ");
 // if (Serial.available()) {
 //   mySerial.write(Serial.read());
 // }
 */
}
