int rpm1;
int rpm2;
int datos[15];
int aux_tablet = 0;
int tiempoEnvio = 100;  //100ms
int delay_rpm = 100;    //Delay entre dato y rpm
long last_tablet_send;

int valor_variable = 0;

void Send_rpm(){
  if (aux_tablet%2 == 0){
    Serial.print("a,");Serial.print(valor_variable);Serial.print("\n");
    valor_variable+=1;
  }else{
    Serial.print("b,");Serial.print(valor_variable);Serial.print("\n");
    valor_variable+=1;
  }
}

void Send_tablet_data(){
  if((millis() - last_tablet_send) > tiempoEnvio){
    if(aux_tablet == 0){
      Serial.print("c,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 1;
      last_tablet_send = millis();
    }else if(aux_tablet == 1){
      Serial.print("d,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 2;
      last_tablet_send = millis();
    }else if(aux_tablet == 2){
      Serial.print("e,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 3;
      last_tablet_send = millis();
    }else if(aux_tablet == 3){
      Serial.print("f,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 4;
      last_tablet_send = millis();
    }else if(aux_tablet == 4){
      Serial.print("g,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 5;
      last_tablet_send = millis();
    }else if(aux_tablet == 5){
      Serial.print("h,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 6;
      last_tablet_send = millis();
    }else if(aux_tablet == 6){
      Serial.print("i,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 7;
      last_tablet_send = millis();
    }else if(aux_tablet == 7){
      Serial.print("j,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 8;
      last_tablet_send = millis();
    }else if(aux_tablet == 8){
      Serial.print("k,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 9;
      last_tablet_send = millis();
    }else if(aux_tablet == 9){
      Serial.print("l,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 10;
      last_tablet_send = millis();
    }else if(aux_tablet == 10){
      Serial.print("m,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 11;
      last_tablet_send = millis();
    }else if(aux_tablet == 11){
      Serial.print("n,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 12;
      last_tablet_send = millis();
    }else if(aux_tablet == 12){
      Serial.print("o,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 13;
      last_tablet_send = millis();
    }else if(aux_tablet == 13){
      Serial.print("p,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 14;
      last_tablet_send = millis();
    }else if(aux_tablet == 14){
      Serial.print("q,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 15;
      last_tablet_send = millis();
    }else if(aux_tablet == 15){
      Serial.print("r,");Serial.print(valor_variable);Serial.print("\n");
      delay(delay_rpm);
      Send_rpm();
      aux_tablet = 0;
      last_tablet_send = millis();
    }
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  if(valor_variable == 1000){
    valor_variable = 0;
  }
  
  Send_tablet_data();

}
