#include <LiquidCrystal.h>
const int RS=2, EN=3, D4=4, D5=5, D6=6, D7=7; // RS: VERDE, EN:NARANJO, D4: ROJO, D5: NEGRO, D6: AMARILLO, D7: AZUL
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

int vel=77;
float consumo=2.3;

void setup() {
  lcd.begin(16,2);
}

void loop() {
  mostrarVelocidad(vel,consumo);
  //lcd.setCursor(0, 0);
  //lcd.print("HOLA");
}

void mostrarVelocidad(int vel, float consumo){
  lcd.setCursor(0, 0);
  lcd.print("Velocidad: "+String(vel));
  lcd.setCursor(0, 1);
  lcd.print("Consumo  : "+String(consumo));
}


