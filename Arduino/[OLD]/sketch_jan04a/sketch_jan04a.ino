#include <SD.h>
#include <SPI.h>                 // SD

File myFile;
const int chipSelect = 53;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  // make sure that the default chip select pin 53 is set to
  // output, even if you don't use it:
  pinMode(53, OUTPUT);//MEGA Uno is pin 10
 // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    lcd.println("Card failed");
    // don't do anything more:
    return;
  }
}
void loop() {
//get time and gather numeric values;
 
  myFile = SD.open(filename, FILE_WRITE);//<<<<<<<<<<<<< OPEN
  myFile.print(hour);
  myFile.print(":");
  myFile.print(minute);
  myFile.print(":");
  myFile.print(second);
  myFile.print(",");

  myFile.print(InTemp);
  myFile.print(",");
  myFile.print(OutTemp);
  myFile.print(",");
  myFile.print(DrainTemp);
  myFile.print(",");
  myFile.print(diff);
  myFile.println();
  myFile.close();//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>CLOSE     

  delay(1000);
}  // loop ends here
