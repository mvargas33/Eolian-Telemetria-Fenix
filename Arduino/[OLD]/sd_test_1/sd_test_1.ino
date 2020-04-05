const int chipSelect = 53;
const char* archivo = "prueba_nombre_ahorasi.txt";
//int a=sizeof(archivo);

#include <SPI.h>
#include "SdFat.h"
SdFat sd;
SdFile myFile;

int aux=0;

void setup() {
  Serial.begin(9600);
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }

}

void loop() {
  if (!myFile.open(archivo, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening archivo.txt for write failed");
  }else{
    Serial.print("Writing to test.txt...");
    myFile.print("Max,");myFile.print(aux);myFile.print(",Vale,");myFile.println(aux);
    myFile.close();
    Serial.println("done.");
  }
  //Read

  if (!myFile.open(archivo, O_READ)) {
    sd.errorHalt("opening archivo for read failed");
  }else{
    Serial.println("prueba: ");
    int data;
    while ((data = myFile.read()) >= 0) {
      Serial.write(data);
    }
    myFile.close();
  }
  aux+=1;
  delay(1000);
}
