#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QtGlobal>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDateTime>
#include <QElapsedTimer>

//Display inicial, valores iniciales, detecta Arduino (no modificar), define parámetros Arduino//

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    qDebug() << "Iniciando interfaz" << endl;
    timer.start();
    //Iniciar arrays con zeros
    memset(BMS, 0, sizeof(BMS));
    memset(BMS_VOLT, 0, sizeof(BMS_VOLT));
    memset(BMS_TEMP, 0, sizeof(BMS_TEMP));
    memset(KELLY_DER, 0, sizeof(KELLY_DER));
    memset(KELLY_IZ, 0, sizeof(KELLY_IZ));
    memset(MPPT1, 0, sizeof(MPPT1));
    memset(MPPT2, 0, sizeof(MPPT2));
    memset(MPPT3, 0, sizeof(MPPT3));
    memset(MPPT4, 0, sizeof(MPPT4));
    last_speed = 0;

    arduino = new QSerialPort(this);

    Dialog::GetProductVendorID();
    Dialog::SetupArduino();
    Dialog::SetupPlots();
    Dialog::SetupSpeedometer();
    Dialog::SetupProgressbars();
}

Dialog::~Dialog()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    delete ui;
}

void Dialog::readSerial_N(){
    serialData = arduino->readAll(); //Array de Bytes entrantes
    //qDebug() << "Bytes: " << serialData << endl;    // Teste de recivida
    if(timer.elapsed()-lastRead>1){
        qDebug() << "Bytes: " << serialData << endl;    // Teste de recivida
        indice=0;                                       // Comienza a leer desde el primer byte
        largo=serialData.length();                      // Hasta el último byte=largo-1
        Dialog::Recorrido(indice,largo,serialData);     // Recorre el vector de bytes y muestra gráficamente
        lastRead=timer.elapsed();                       // Registro última lectura
    }
    //serialData.clear(); //Se limpia el vector para volver a leer
}

// Definiciones:
// Bloque dieal: "A(40)B(40)C(40)D(40)"      , con (40)=40 bytes de números
// Bloque real:  "A(40)B(12)CDE(2)X(40)(4)"
// Cadena: A(40)

/*////////////////// Formato de 7 bytes NEW ///////////////////*/
void Dialog::Recorrido(int index, int len, QByteArray bytes){
    // Si el índice está fuera de rango
    if(index>=len){
        serialData.clear();
        index=0;
        len=0;
    }else{
        //qDebug() << "All bytes: " << bytes << endl;
        //qDebug() << bytes[index] << bytes[index+1] << bytes[index+2] << bytes[index+3] << bytes[index+4] << bytes[index+5] << bytes[index+6] << "Byte 1 es letra: " << Es_letra(bytes,index) << " Byte 2-7 no letras: " << (Es_numero(bytes,index+1)) << (Es_numero(bytes,index+2)) << (Es_numero(bytes,index+3)) << (Es_numero(bytes,index+4)) << (Es_numero(bytes,index+5)) << (Es_numero(bytes,index+6)) << endl;
        // CASO l(6)A(6) ... J(6) | Lectura de 77 bytes
       if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+7]==char(0x41))&&(bytes[index+14]==char(0x42))&&(bytes[index+21]==char(0x43))&&(bytes[index+28]==char(0x44))&&(bytes[index+35]==char(0x45))&&(bytes[index+42]==char(0x46))&&(bytes[index+49]==char(0x47))&&(bytes[index+56]==char(0x48))&&(bytes[index+63]==char(0x49))&&(bytes[index+70]==char(0x4a))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_A(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

       // CASO l(6)K(6) ... T(6) | Lectura de 77 bytes
       }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+7]==char(0x4b))&&(bytes[index+14]==char(0x4c))&&(bytes[index+21]==char(0x4d))&&(bytes[index+28]==char(0x4e))&&(bytes[index+35]==char(0x4f))&&(bytes[index+42]==char(0x50))&&(bytes[index+49]==char(0x51))&&(bytes[index+56]==char(0x52))&&(bytes[index+63]==char(0x53))&&(bytes[index+70]==char(0x54))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_K(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)U(6) ... d(6) | Lectura de 77 bytes
       }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+7]==char(0x55))&&(bytes[index+14]==char(0x56))&&(bytes[index+21]==char(0x57))&&(bytes[index+28]==char(0x58))&&(bytes[index+35]==char(0x59))&&(bytes[index+42]==char(0x5a))&&(bytes[index+49]==char(0x61))&&(bytes[index+56]==char(0x62))&&(bytes[index+63]==char(0x63))&&(bytes[index+70]==char(0x64))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_U(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)e(6) ... @C(5) | Lectura de 77 bytes
        }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+7]==char(0x65))&&(bytes[index+14]==char(0x66))&&(bytes[index+21]==char(0x67))&&(bytes[index+28]==char(0x68))&&(bytes[index+35]==char(0x69))&&(bytes[index+42]==char(0x6a))&&(bytes[index+49]==char(0x6b))&&(bytes[index+57]==char(0x41))&&(bytes[index+64]==char(0x42))&&(bytes[index+71]==char(0x43))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_e(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)@D(5) ... @M(5) | Lectura de 77 bytes
        }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+8]==char(0x44))&&(bytes[index+15]==char(0x45))&&(bytes[index+22]==char(0x46))&&(bytes[index+29]==char(0x47))&&(bytes[index+36]==char(0x48))&&(bytes[index+43]==char(0x49))&&(bytes[index+50]==char(0x4a))&&(bytes[index+57]==char(0x4b))&&(bytes[index+64]==char(0x4c))&&(bytes[index+71]==char(0x4d))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_D(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)@N(5) ... @W(5) | Lectura de 77 bytes
        }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+8]==char(0x4e))&&(bytes[index+15]==char(0x4f))&&(bytes[index+22]==char(0x50))&&(bytes[index+29]==char(0x51))&&(bytes[index+36]==char(0x52))&&(bytes[index+43]==char(0x53))&&(bytes[index+50]==char(0x54))&&(bytes[index+57]==char(0x55))&&(bytes[index+64]==char(0x56))&&(bytes[index+71]==char(0x57))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_N(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)@X(5) ... q(6) | Lectura de 77 bytes
        }else if((index+76<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+8]==char(0x58))&&(bytes[index+15]==char(0x59))&&(bytes[index+22]==char(0x5a))&&(bytes[index+29]==char(0x61))&&(bytes[index+36]==char(0x62))&&(bytes[index+42]==char(0x6d))&&(bytes[index+49]==char(0x6e))&&(bytes[index+56]==char(0x6f))&&(bytes[index+63]==char(0x70))&&(bytes[index+70]==char(0x71))&&(Es_numero(bytes,index+76))){
            for(int i=0;i<77;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_X(serialSelected);
            serialSelected.clear();
            index+=77;
            Dialog::Recorrido(index,len,bytes);

        // CASO l(6)r(6) ... }(6) | Lectura de 84 bytes
        }else if((index+83<=len)&&(bytes[index]==char(0x6c))&&(bytes[index+7]==char(0x72))&&(bytes[index+14]==char(0x73))&&(bytes[index+21]==char(0x74))&&(bytes[index+28]==char(0x75))&&(bytes[index+35]==char(0x76))&&(bytes[index+42]==char(0x77))&&(bytes[index+49]==char(0x78))&&(bytes[index+56]==char(0x79))&&(bytes[index+63]==char(0x7a))&&(bytes[index+70]==char(0x7b))&&(bytes[index+77]==char(0x7d))&&(Es_numero(bytes,index+83))){
            for(int i=0;i<84;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bytes[index+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_r(serialSelected);
            serialSelected.clear();
            index+=84;
            Dialog::Recorrido(index,len,bytes);



        // CASO Normal: A(6)
       }else if((Es_letra(bytes,index))&&(Es_numero(bytes,index+1))&&(Es_numero(bytes,index+2))&&(Es_numero(bytes,index+3))&&(Es_numero(bytes,index+4))&&(Es_numero(bytes,index+5))&&(Es_numero(bytes,index+6))){
           for(int i=0;i<7;i++){                                      //Seleccionamos primeros 7 bytes aparte
               serialSelected[i]=bytes[index+i];
           }
           qDebug() << "serialSelected: " << serialSelected << endl;
           Dialog::Update_display_7(serialSelected);                     //Hacemos display
           serialSelected.clear();
           index+=7;
           Dialog::Recorrido(index,len,bytes);
       // CASO Voltajes: @A(5)
       }else if ((bytes[index]==char(0x40))&&(Es_letra(bytes,index+1))&&(Es_numero(bytes,index+2))&&(Es_numero(bytes,index+3))&&(Es_numero(bytes,index+4))&&(Es_numero(bytes,index+5))&&(Es_numero(bytes,index+6))){
           for(int i=0;i<7;i++){                                      //Seleccionamos primeros 7 bytes aparte
               serialSelected[i]=bytes[index+i];
           }
           qDebug() << "serialSelected: " << serialSelected << endl;
           Dialog::Update_display_7_volt(serialSelected);                     //Hacemos display
           serialSelected.clear();                                     //Limpiamos vector de bytes
           index+=7;
           Dialog::Recorrido(index,len,bytes);
       // CASO: Indexado no calza con los perfiles
       }else{
           index+=1;
           Dialog::Recorrido(index,len,bytes);
       }
    }
}

bool Dialog::Es_letra(QByteArray bits,int index){
    bool estado_inicial = false;
    if ((bits[index]==char(0x41))||(bits[index]==char(0x42))||(bits[index]==char(0x43))||(bits[index]==char(0x44))||(bits[index]==char(0x45))||(bits[index]==char(0x46))||(bits[index]==char(0x47))||(bits[index]==char(0x48))||(bits[index]==char(0x49))||(bits[index]==char(0x4a))||(bits[index]==char(0x4b))||(bits[index]==char(0x4c))||(bits[index]==char(0x4d))||(bits[index]==char(0x4e))||(bits[index]==char(0x4f))||(bits[index]==char(0x50))||(bits[index]==char(0x51))||(bits[index]==char(0x52))||(bits[index]==char(0x53))||(bits[index]==char(0x54))||(bits[index]==char(0x55))||(bits[index]==char(0x56))||(bits[index]==char(0x57))||(bits[index]==char(0x58))||(bits[index]==char(0x59))||(bits[index]==char(0x5a))||(bits[index]==char(0x61))||(bits[index]==char(0x62))||(bits[index]==char(0x63))||(bits[index]==char(0x64))||(bits[index]==char(0x65))||(bits[index]==char(0x66))||(bits[index]==char(0x67))||(bits[index]==char(0x68))||(bits[index]==char(0x69))||(bits[index]==char(0x6a))||(bits[index]==char(0x6b))||(bits[index]==char(0x6c))||(bits[index]==char(0x6d))||(bits[index]==char(0x6e))||(bits[index]==char(0x6f))||(bits[index]==char(0x70))||(bits[index]==char(0x71))||(bits[index]==char(0x72))||(bits[index]==char(0x73))||(bits[index]==char(0x74))||(bits[index]==char(0x75))||(bits[index]==char(0x76))||(bits[index]==char(0x77))||(bits[index]==char(0x78))||(bits[index]==char(0x79))||(bits[index]==char(0x7a))||(bits[index]==char(0x7b))||(bits[index]==char(0x7c))||(bits[index]==char(0x7d))||(bits[index]==char(0x7e))){
        estado_inicial = true;
    }
    //qDebug() << "bit: " << bits[index] << " Es letra: " << estado_inicial << endl;
    return estado_inicial;
}
bool Dialog::Es_numero(QByteArray bits, int index){
    bool estado_inicial = false;
    if ((bits[index]==char(0x30))||(bits[index]==char(0x31))||(bits[index]==char(0x32))||(bits[index]==char(0x33))||(bits[index]==char(0x34))||(bits[index]==char(0x35))||(bits[index]==char(0x36))||(bits[index]==char(0x37))||(bits[index]==char(0x38))||(bits[index]==char(0x39))){
        estado_inicial = true;
    }
    return estado_inicial;
}
void Dialog::Update_A(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];

    //Kelly Der
    // A 41
    Auxiliar[2]=(Auxiliar[2].append(bytes[8]).append(bytes[9]));      // [0] Ia
    Auxiliar[3]=(Auxiliar[3].append(bytes[10]).append(bytes[11]));    // [1] Ib
    Auxiliar[4]=(Auxiliar[4].append(bytes[12]).append(bytes[13]));    // [2] Ic
    DatosDouble[2]=Auxiliar[2].toDouble();
    DatosDouble[3]=Auxiliar[3].toDouble();
    DatosDouble[4]=Auxiliar[4].toDouble();
    // B 42
    Auxiliar[5]=(Auxiliar[5].append(bytes[15]).append(bytes[16]));  // [3] Va
    Auxiliar[6]=(Auxiliar[6].append(bytes[17]).append(bytes[18]));  // [4] Vb
    Auxiliar[7]=(Auxiliar[7].append(bytes[19]).append(bytes[20]));  // [5] Vc
    DatosDouble[5]=Auxiliar[5].toDouble();
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    KELLY_DER[3] = DatosDouble[5];
    KELLY_DER[4] = DatosDouble[6];
    KELLY_DER[5] = DatosDouble[7];
    // C 43
    Auxiliar[8]=(Auxiliar[8].append(bytes[22]).append(bytes[23]).append(bytes[24]).append(bytes[25]));  // [6] = RPM
    Auxiliar[9]=(Auxiliar[9].append(bytes[26]).append(bytes[27]));                                      // [8] = PWM
    DatosDouble[8]=Auxiliar[8].toDouble();
    DatosDouble[9]=Auxiliar[9].toDouble();
    KELLY_DER[6] = DatosDouble[8];
    KELLY_DER[8] = DatosDouble[9];
    // D
    Auxiliar[10]=(Auxiliar[10].append(bytes[29]).append(bytes[30]).append(bytes[31]).append(bytes[32]));    // [7] = ERROR CODE
    Auxiliar[11]=(Auxiliar[11].append(bytes[33]).append(bytes[34]));                                        // [11] = KELLY TEMP
    DatosDouble[10]=Auxiliar[10].toDouble();
    DatosDouble[11]=Auxiliar[11].toDouble();
    KELLY_DER[7] = DatosDouble[10];
    KELLY_DER[11] = DatosDouble[11];
    // E
    Auxiliar[12]=(Auxiliar[12].append(bytes[36]));                                      // [9] = EMR
    Auxiliar[13]=(Auxiliar[13].append(bytes[37]).append(bytes[38]).append(bytes[39]));  // [10] = MOTOR TEMP
    Auxiliar[14]=(Auxiliar[14].append(bytes[40]));                                      // [12] = Throttle Switch Status DERECHO
    Auxiliar[15]=(Auxiliar[15].append(bytes[41]));                                      // [13] = Reverse Switch Status DERECHO
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    DatosDouble[14]=Auxiliar[14].toDouble();
    DatosDouble[15]=Auxiliar[15].toDouble();
    KELLY_DER[9] = DatosDouble[12];
    KELLY_DER[10] = DatosDouble[13];
    KELLY_DER[12] = DatosDouble[14];
    KELLY_DER[13] = DatosDouble[15];

    //Kelly Izq
    // F
    Auxiliar[16]=(Auxiliar[16].append(bytes[43]).append(bytes[44]));  // [0] Ia
    Auxiliar[17]=(Auxiliar[17].append(bytes[45]).append(bytes[46]));  // [1] Ib
    Auxiliar[18]=(Auxiliar[18].append(bytes[47]).append(bytes[48]));  // [2] Ic
    DatosDouble[16]=Auxiliar[16].toDouble();
    DatosDouble[17]=Auxiliar[17].toDouble();
    DatosDouble[18]=Auxiliar[18].toDouble();
    KELLY_IZ[0] = DatosDouble[16];
    KELLY_IZ[1] = DatosDouble[17];
    KELLY_IZ[2] = DatosDouble[18];
    // G
    Auxiliar[19]=(Auxiliar[19].append(bytes[50]).append(bytes[51]));  // [3] Va
    Auxiliar[20]=(Auxiliar[20].append(bytes[52]).append(bytes[53]));  // [4] Vb
    Auxiliar[21]=(Auxiliar[21].append(bytes[54]).append(bytes[55]));  // [5] Vc
    DatosDouble[19]=Auxiliar[19].toDouble();
    DatosDouble[20]=Auxiliar[20].toDouble();
    DatosDouble[21]=Auxiliar[21].toDouble();
    KELLY_IZ[3] = DatosDouble[19];
    KELLY_IZ[4] = DatosDouble[20];
    KELLY_IZ[5] = DatosDouble[21];
    // H
    Auxiliar[22]=(Auxiliar[22].append(bytes[57]).append(bytes[58]).append(bytes[59]).append(bytes[60]));    // [6] = RPM
    Auxiliar[23]=(Auxiliar[23].append(bytes[61]).append(bytes[62]));                                        // [8] = PWM
    DatosDouble[22]=Auxiliar[22].toDouble();
    DatosDouble[23]=Auxiliar[23].toDouble();
    KELLY_IZ[6] = DatosDouble[22];
    KELLY_IZ[8] = DatosDouble[23];
    // I
    Auxiliar[24]=(Auxiliar[24].append(bytes[64]).append(bytes[65]).append(bytes[66]).append(bytes[67]));  // [7] = ERROR CODE
    Auxiliar[25]=(Auxiliar[25].append(bytes[68]).append(bytes[69]));                                    // [11] = KELLY TEMP
    DatosDouble[24]=Auxiliar[24].toDouble();
    DatosDouble[25]=Auxiliar[25].toDouble();
    KELLY_IZ[7] = DatosDouble[24];
    KELLY_IZ[11] = DatosDouble[25];
    // J
    Auxiliar[26]=(Auxiliar[26].append(bytes[71]));                                      // [9] = EMR
    Auxiliar[27]=(Auxiliar[27].append(bytes[72]).append(bytes[73]).append(bytes[74]));  // [10] = MOTOR TEMP
    Auxiliar[28]=(Auxiliar[28].append(bytes[75]));                                      // [12] = Throttle Switch Status DERECHO
    Auxiliar[29]=(Auxiliar[29].append(bytes[76]));                                      // [13] = Reverse Switch Status DERECHO
    DatosDouble[26]=Auxiliar[26].toDouble();
    DatosDouble[27]=Auxiliar[27].toDouble();
    DatosDouble[28]=Auxiliar[28].toDouble();
    DatosDouble[29]=Auxiliar[29].toDouble();
    KELLY_IZ[9] = DatosDouble[26];
    KELLY_IZ[10] = DatosDouble[27];
    KELLY_IZ[12] = DatosDouble[28];
    KELLY_IZ[13] = DatosDouble[29];

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_K(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // K 4b
    Auxiliar[2]=(Auxiliar[2].append(bytes[8]).append(bytes[9]).append(bytes[10]));      // [1] = Pack Current
    Auxiliar[3]=(Auxiliar[3].append(bytes[11]).append(bytes[12]).append(bytes[13]));    // [2] = Pack Inst Volt
    DatosDouble[2]=Auxiliar[2].toDouble();
    DatosDouble[3]=Auxiliar[3].toDouble();
    BMS[1] = DatosDouble[2];
    BMS[2] = DatosDouble[3];
    // L 4c
    Auxiliar[4]=(Auxiliar[4].append(bytes[15]).append(bytes[16]).append(bytes[17]));   // [0] = SOC
    Auxiliar[5]=(Auxiliar[5].append(bytes[18]).append(bytes[19]).append(bytes[20]));   // [3] = Pack Open Volt
    DatosDouble[4]=Auxiliar[4].toDouble();
    DatosDouble[5]=Auxiliar[5].toDouble();
    BMS[0] = DatosDouble[4];
    BMS[3] = DatosDouble[5];
    // M 4d
    Auxiliar[6]=(Auxiliar[6].append(bytes[22]).append(bytes[23]));    // [4] = Pack Abs Current
    Auxiliar[7]=(Auxiliar[7].append(bytes[24]).append(bytes[25]));    // [7] = High Temp
    Auxiliar[8]=(Auxiliar[8].append(bytes[26]).append(bytes[27]));    // [8] = High Temp Thermistor ID
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    DatosDouble[8]=Auxiliar[8].toDouble();
    BMS[4] = DatosDouble[6];
    BMS[7] = DatosDouble[7];
    BMS[8] = DatosDouble[8];
    // N 4e
    Auxiliar[9]=(Auxiliar[9].append(bytes[29]).append(bytes[30]).append(bytes[31]).append(bytes[32]).append(bytes[33]).append(bytes[34]));    // [5] = Maximum Pack Volt
    DatosDouble[9]=Auxiliar[9].toDouble();
    BMS[5] = DatosDouble[9];
    // O 4f
    Auxiliar[10]=(Auxiliar[10].append(bytes[36]).append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]).append(bytes[41]));  // [6] = Minimum Pack Volt
    DatosDouble[10]=Auxiliar[10].toDouble();
    BMS[6] = DatosDouble[10];
    // P 50
    Auxiliar[11]=(Auxiliar[11].append(bytes[43]).append(bytes[44]));  // [9] = Low Temp
    Auxiliar[12]=(Auxiliar[12].append(bytes[45]).append(bytes[46]));  // [10] = Low Temp Thermistor ID
    Auxiliar[13]=(Auxiliar[13].append(bytes[47]).append(bytes[48]));  // [11] = Average Temp
    DatosDouble[11]=Auxiliar[11].toDouble();
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    BMS[9] = DatosDouble[11];
    BMS[10] = DatosDouble[12];
    BMS[11] = DatosDouble[13];
    // Q 51
    Auxiliar[14]=(Auxiliar[14].append(bytes[50]).append(bytes[51]));  // [12] = Internal Temp
    Auxiliar[15]=(Auxiliar[15].append(bytes[52]).append(bytes[53]));  // [13] = Max Volt ID
    Auxiliar[16]=(Auxiliar[16].append(bytes[54]).append(bytes[55]));  // [14] = Min Volt ID
    DatosDouble[14]=Auxiliar[14].toDouble();
    DatosDouble[15]=Auxiliar[15].toDouble();
    DatosDouble[16]=Auxiliar[16].toDouble();
    BMS[12] = DatosDouble[14];
    BMS[13] = DatosDouble[15];
    BMS[14] = DatosDouble[16];
    // R 52
    Auxiliar[17]=(Auxiliar[17].append(bytes[57]).append(bytes[58]));    // [60 Temperaturas]
    Auxiliar[18]=(Auxiliar[18].append(bytes[59]).append(bytes[60]));    // [60 Temperaturas]
    Auxiliar[19]=(Auxiliar[19].append(bytes[61]).append(bytes[62]));    // [60 Temperaturas]
    DatosDouble[17]=Auxiliar[17].toDouble();
    DatosDouble[18]=Auxiliar[18].toDouble();
    DatosDouble[19]=Auxiliar[19].toDouble();
    BMS_TEMP[0] = DatosDouble[17];
    BMS_TEMP[1] = DatosDouble[18];
    BMS_TEMP[2] = DatosDouble[19];
    // S 53
    Auxiliar[20]=(Auxiliar[20].append(bytes[64]).append(bytes[65]));  // [60 Temperaturas]
    Auxiliar[21]=(Auxiliar[21].append(bytes[66]).append(bytes[67]));  // [60 Temperaturas]
    Auxiliar[22]=(Auxiliar[22].append(bytes[68]).append(bytes[69]));  // [60 Temperaturas]
    DatosDouble[20]=Auxiliar[20].toDouble();
    DatosDouble[21]=Auxiliar[21].toDouble();
    DatosDouble[22]=Auxiliar[22].toDouble();
    BMS_TEMP[3] = DatosDouble[20];
    BMS_TEMP[4] = DatosDouble[21];
    BMS_TEMP[5] = DatosDouble[22];
    // T 54
    Auxiliar[23]=(Auxiliar[23].append(bytes[71]).append(bytes[72]));  // [60 Temperaturas]
    Auxiliar[24]=(Auxiliar[24].append(bytes[73]).append(bytes[74]));  // [60 Temperaturas]
    Auxiliar[25]=(Auxiliar[25].append(bytes[75]).append(bytes[76]));  // [60 Temperaturas]
    DatosDouble[23]=Auxiliar[23].toDouble();
    DatosDouble[24]=Auxiliar[24].toDouble();
    DatosDouble[25]=Auxiliar[25].toDouble();
    BMS_TEMP[6] = DatosDouble[23];
    BMS_TEMP[7] = DatosDouble[24];
    BMS_TEMP[8] = DatosDouble[25];

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_U(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // U 55
    Auxiliar[2]=(Auxiliar[2].append(bytes[8]).append(bytes[9]));      // [60 Temperaturas]
    Auxiliar[3]=(Auxiliar[3].append(bytes[10]).append(bytes[11]));    // [60 Temperaturas]
    Auxiliar[4]=(Auxiliar[4].append(bytes[12]).append(bytes[13]));    // [60 Temperaturas]
    DatosDouble[2]=Auxiliar[2].toDouble();
    DatosDouble[3]=Auxiliar[3].toDouble();
    DatosDouble[4]=Auxiliar[4].toDouble();
    BMS_TEMP[9] = DatosDouble[2];
    BMS_TEMP[10] = DatosDouble[3];
    BMS_TEMP[11] = DatosDouble[4];
    // V 56
    Auxiliar[5]=(Auxiliar[5].append(bytes[15]).append(bytes[16]));  // [60 Temperaturas]
    Auxiliar[6]=(Auxiliar[6].append(bytes[17]).append(bytes[18]));  // [60 Temperaturas]
    Auxiliar[7]=(Auxiliar[7].append(bytes[19]).append(bytes[20]));  // [60 Temperaturas]
    DatosDouble[5]=Auxiliar[5].toDouble();
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    BMS_TEMP[12] = DatosDouble[5];
    BMS_TEMP[13] = DatosDouble[6];
    BMS_TEMP[14] = DatosDouble[7];
    // W 57
    Auxiliar[8]=(Auxiliar[8].append(bytes[22]).append(bytes[23]));      // [60 Temperaturas]
    Auxiliar[9]=(Auxiliar[9].append(bytes[24]).append(bytes[25]));      // [60 Temperaturas]
    Auxiliar[10]=(Auxiliar[10].append(bytes[26]).append(bytes[27]));    // [60 Temperaturas]
    DatosDouble[8]=Auxiliar[8].toDouble();
    DatosDouble[9]=Auxiliar[9].toDouble();
    DatosDouble[10]=Auxiliar[10].toDouble();
    BMS_TEMP[15] = DatosDouble[8];
    BMS_TEMP[16] = DatosDouble[9];
    BMS_TEMP[17] = DatosDouble[10];
    // X 58
    Auxiliar[11]=(Auxiliar[11].append(bytes[29]).append(bytes[30]));        // [60 Temperaturas]
    Auxiliar[12]=(Auxiliar[12].append(bytes[31]).append(bytes[32]));        // [60 Temperaturas]
    Auxiliar[13]=(Auxiliar[13].append(bytes[33]).append(bytes[34]));        // [60 Temperaturas]
    DatosDouble[11]=Auxiliar[11].toDouble();
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    BMS_TEMP[18] = DatosDouble[11];
    BMS_TEMP[19] = DatosDouble[12];
    BMS_TEMP[20] = DatosDouble[13];
    // Y 59
    Auxiliar[14]=(Auxiliar[14].append(bytes[36]).append(bytes[37]));        // [60 Temperaturas]
    Auxiliar[15]=(Auxiliar[15].append(bytes[38]).append(bytes[39]));        // [60 Temperaturas]
    Auxiliar[16]=(Auxiliar[16].append(bytes[40]).append(bytes[41]));        // [60 Temperaturas]
    DatosDouble[14]=Auxiliar[14].toDouble();
    DatosDouble[15]=Auxiliar[15].toDouble();
    DatosDouble[16]=Auxiliar[16].toDouble();
    BMS_TEMP[21] = DatosDouble[14];
    BMS_TEMP[22] = DatosDouble[15];
    BMS_TEMP[23] = DatosDouble[16];
    // Z 5a
    Auxiliar[17]=(Auxiliar[17].append(bytes[43]).append(bytes[44]));    // [60 Temperaturas]
    Auxiliar[18]=(Auxiliar[18].append(bytes[45]).append(bytes[46]));    // [60 Temperaturas]
    Auxiliar[19]=(Auxiliar[19].append(bytes[47]).append(bytes[48]));    // [60 Temperaturas]
    DatosDouble[17]=Auxiliar[17].toDouble();
    DatosDouble[18]=Auxiliar[18].toDouble();
    DatosDouble[19]=Auxiliar[19].toDouble();
    BMS_TEMP[24] = DatosDouble[17];
    BMS_TEMP[25] = DatosDouble[18];
    BMS_TEMP[26] = DatosDouble[19];
    // a 61
    Auxiliar[20]=(Auxiliar[20].append(bytes[50]).append(bytes[51]));    // [60 Temperaturas]
    Auxiliar[21]=(Auxiliar[21].append(bytes[52]).append(bytes[53]));    // [60 Temperaturas]
    Auxiliar[22]=(Auxiliar[22].append(bytes[54]).append(bytes[55]));    // [60 Temperaturas]
    DatosDouble[20]=Auxiliar[20].toDouble();
    DatosDouble[21]=Auxiliar[21].toDouble();
    DatosDouble[22]=Auxiliar[22].toDouble();
    BMS_TEMP[27] = DatosDouble[20];
    BMS_TEMP[28] = DatosDouble[21];
    BMS_TEMP[29] = DatosDouble[22];
    // b 62
    Auxiliar[23]=(Auxiliar[23].append(bytes[57]).append(bytes[58]));    // [60 Temperaturas]
    Auxiliar[24]=(Auxiliar[24].append(bytes[59]).append(bytes[60]));    // [60 Temperaturas]
    Auxiliar[25]=(Auxiliar[25].append(bytes[61]).append(bytes[62]));    // [60 Temperaturas]
    DatosDouble[23]=Auxiliar[23].toDouble();
    DatosDouble[24]=Auxiliar[24].toDouble();
    DatosDouble[25]=Auxiliar[25].toDouble();
    BMS_TEMP[30] = DatosDouble[23];
    BMS_TEMP[31] = DatosDouble[24];
    BMS_TEMP[32] = DatosDouble[25];
    // c 63
    Auxiliar[26]=(Auxiliar[26].append(bytes[64]).append(bytes[65]));    // [60 Temperaturas]
    Auxiliar[27]=(Auxiliar[27].append(bytes[66]).append(bytes[67]));    // [60 Temperaturas]
    Auxiliar[28]=(Auxiliar[28].append(bytes[68]).append(bytes[69]));    // [60 Temperaturas]
    DatosDouble[26]=Auxiliar[26].toDouble();
    DatosDouble[27]=Auxiliar[27].toDouble();
    DatosDouble[28]=Auxiliar[28].toDouble();
    BMS_TEMP[33] = DatosDouble[26];
    BMS_TEMP[34] = DatosDouble[27];
    BMS_TEMP[35] = DatosDouble[28];
    // d 64
    Auxiliar[29]=(Auxiliar[29].append(bytes[71]).append(bytes[72]));  // [60 Temperaturas]
    Auxiliar[30]=(Auxiliar[30].append(bytes[73]).append(bytes[74]));  // [60 Temperaturas]
    Auxiliar[31]=(Auxiliar[31].append(bytes[75]).append(bytes[76]));  // [60 Temperaturas]
    DatosDouble[29]=Auxiliar[29].toDouble();
    DatosDouble[30]=Auxiliar[30].toDouble();
    DatosDouble[31]=Auxiliar[31].toDouble();
    BMS_TEMP[36] = DatosDouble[29];
    BMS_TEMP[37] = DatosDouble[30];
    BMS_TEMP[38] = DatosDouble[31];

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_e(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // e 65
    Auxiliar[2]=(Auxiliar[2].append(bytes[8]).append(bytes[9]));
    Auxiliar[3]=(Auxiliar[3].append(bytes[10]).append(bytes[11]));
    Auxiliar[4]=(Auxiliar[4].append(bytes[12]).append(bytes[13]));
    DatosDouble[2]=Auxiliar[2].toDouble();
    DatosDouble[3]=Auxiliar[3].toDouble();
    DatosDouble[4]=Auxiliar[4].toDouble();
    BMS_TEMP[39] = DatosDouble[2];                           // [60 Temperaturas]
    BMS_TEMP[40] = DatosDouble[3];                           // [60 Temperaturas]
    BMS_TEMP[41] = DatosDouble[4];                           // [60 Temperaturas]
    // f 66
    Auxiliar[5]=(Auxiliar[5].append(bytes[15]).append(bytes[16]));
    Auxiliar[6]=(Auxiliar[6].append(bytes[17]).append(bytes[18]));
    Auxiliar[7]=(Auxiliar[7].append(bytes[19]).append(bytes[20]));
    DatosDouble[5]=Auxiliar[5].toDouble();
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    BMS_TEMP[42] = DatosDouble[5];                           // [60 Temperaturas]
    BMS_TEMP[43] = DatosDouble[6];                           // [60 Temperaturas]
    BMS_TEMP[44] = DatosDouble[7];                           // [60 Temperaturas]
    // g 67
    Auxiliar[8]=(Auxiliar[8].append(bytes[22]).append(bytes[23]));
    Auxiliar[9]=(Auxiliar[9].append(bytes[24]).append(bytes[25]));
    Auxiliar[10]=(Auxiliar[10].append(bytes[26]).append(bytes[27]));
    DatosDouble[8]=Auxiliar[8].toDouble();
    DatosDouble[9]=Auxiliar[9].toDouble();
    DatosDouble[10]=Auxiliar[10].toDouble();
    BMS_TEMP[45] = DatosDouble[8];                           // [60 Temperaturas]
    BMS_TEMP[46] = DatosDouble[9];                           // [60 Temperaturas]
    BMS_TEMP[47] = DatosDouble[10];                           // [60 Temperaturas]
    // h 68
    Auxiliar[11]=(Auxiliar[11].append(bytes[29]).append(bytes[30]));
    Auxiliar[12]=(Auxiliar[12].append(bytes[31]).append(bytes[32]));
    Auxiliar[13]=(Auxiliar[13].append(bytes[33]).append(bytes[34]));
    DatosDouble[11]=Auxiliar[11].toDouble();
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    BMS_TEMP[48] = DatosDouble[11];                           // [60 Temperaturas]
    BMS_TEMP[49] = DatosDouble[12];                           // [60 Temperaturas]
    BMS_TEMP[50] = DatosDouble[13];                           // [60 Temperaturas]
    // i 69
    Auxiliar[14]=(Auxiliar[14].append(bytes[36]).append(bytes[37]));
    Auxiliar[15]=(Auxiliar[15].append(bytes[38]).append(bytes[39]));
    Auxiliar[16]=(Auxiliar[16].append(bytes[40]).append(bytes[41]));
    DatosDouble[14]=Auxiliar[14].toDouble();
    DatosDouble[15]=Auxiliar[15].toDouble();
    DatosDouble[16]=Auxiliar[16].toDouble();
    BMS_TEMP[51] = DatosDouble[14];                           // [60 Temperaturas]
    BMS_TEMP[52] = DatosDouble[15];                           // [60 Temperaturas]
    BMS_TEMP[53] = DatosDouble[16];                           // [60 Temperaturas]
    // j 6a
    Auxiliar[17]=(Auxiliar[17].append(bytes[43]).append(bytes[44]));
    Auxiliar[18]=(Auxiliar[18].append(bytes[45]).append(bytes[46]));
    Auxiliar[19]=(Auxiliar[19].append(bytes[47]).append(bytes[48]));
    DatosDouble[17]=Auxiliar[17].toDouble();
    DatosDouble[18]=Auxiliar[18].toDouble();
    DatosDouble[19]=Auxiliar[19].toDouble();
    BMS_TEMP[54] = DatosDouble[17];                           // [60 Temperaturas]
    BMS_TEMP[55] = DatosDouble[18];                           // [60 Temperaturas]
    BMS_TEMP[56] = DatosDouble[19];                           // [60 Temperaturas]
    // k 6b
    Auxiliar[20]=(Auxiliar[20].append(bytes[50]).append(bytes[51]));
    Auxiliar[21]=(Auxiliar[21].append(bytes[52]).append(bytes[53]));
    Auxiliar[22]=(Auxiliar[22].append(bytes[54]).append(bytes[55]));
    DatosDouble[20]=Auxiliar[20].toDouble();
    DatosDouble[21]=Auxiliar[21].toDouble();
    DatosDouble[22]=Auxiliar[22].toDouble();
    BMS_TEMP[57] = DatosDouble[20];                           // [60 Temperaturas]
    BMS_TEMP[58] = DatosDouble[21];                           // [60 Temperaturas]
    BMS_TEMP[59] = DatosDouble[22];                           // [60 Temperaturas]
    // @ A 41
    Auxiliar[23]=(Auxiliar[23].append(bytes[58]).append(bytes[59]).append(bytes[60]).append(bytes[61]).append(bytes[62]));
    DatosDouble[23]=Auxiliar[23].toDouble();
    BMS_VOLT[0] = DatosDouble[23]*0.0001;                     // [30 Voltajes]
    // @ B 42
    Auxiliar[24]=(Auxiliar[24].append(bytes[65]).append(bytes[66]).append(bytes[67]).append(bytes[68]).append(bytes[69]));
    DatosDouble[24]=Auxiliar[24].toDouble();
    BMS_VOLT[1] = DatosDouble[24]*0.0001;                     // [30 Voltajes]
    // @ C 43
    Auxiliar[25]=(Auxiliar[25].append(bytes[72]).append(bytes[73]).append(bytes[74]).append(bytes[75]).append(bytes[76]));
    DatosDouble[25]=Auxiliar[25].toDouble();
    BMS_VOLT[2] = DatosDouble[25]*0.0001;                     // [30 Voltajes]

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_D(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // @ D 44
    Auxiliar[2]=(Auxiliar[2].append(bytes[9]).append(bytes[10]).append(bytes[11]).append(bytes[12]).append(bytes[13]));
    DatosDouble[2]=Auxiliar[2].toDouble();
    BMS_VOLT[3] = DatosDouble[2]*0.0001;                     // [30 Voltajes]
    // @ E 45
    Auxiliar[3]=(Auxiliar[3].append(bytes[16]).append(bytes[17]).append(bytes[18]).append(bytes[19]).append(bytes[20]));
    DatosDouble[3]=Auxiliar[3].toDouble();
    BMS_VOLT[4] = DatosDouble[3]*0.0001;                     // [30 Voltajes]
    // @ F 46
    Auxiliar[4]=(Auxiliar[4].append(bytes[23]).append(bytes[24]).append(bytes[25]).append(bytes[26]).append(bytes[27]));
    DatosDouble[4]=Auxiliar[4].toDouble();
    BMS_VOLT[5] = DatosDouble[4]*0.0001;                     // [30 Voltajes]
    // @ G 47
    Auxiliar[5]=(Auxiliar[5].append(bytes[30]).append(bytes[31]).append(bytes[32]).append(bytes[33]).append(bytes[34]));
    DatosDouble[5]=Auxiliar[5].toDouble();
    BMS_VOLT[6] = DatosDouble[5]*0.0001;                     // [30 Voltajes]
    // @ H 48
    Auxiliar[6]=(Auxiliar[6].append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]).append(bytes[41]));
    DatosDouble[6]=Auxiliar[6].toDouble();
    BMS_VOLT[7] = DatosDouble[6]*0.0001;                     // [30 Voltajes]
    // @ I 49
    Auxiliar[7]=(Auxiliar[7].append(bytes[44]).append(bytes[45]).append(bytes[46]).append(bytes[47]).append(bytes[48]));
    DatosDouble[7]=Auxiliar[7].toDouble();
    BMS_VOLT[8] = DatosDouble[7]*0.0001;                     // [30 Voltajes]
    // @ J 4a
    Auxiliar[8]=(Auxiliar[8].append(bytes[51]).append(bytes[52]).append(bytes[53]).append(bytes[54]).append(bytes[55]));
    DatosDouble[8]=Auxiliar[8].toDouble();
    BMS_VOLT[9] = DatosDouble[8]*0.0001;                     // [30 Voltajes]
    // @ K 4b
    Auxiliar[9]=(Auxiliar[9].append(bytes[58]).append(bytes[59]).append(bytes[60]).append(bytes[61]).append(bytes[62]));
    DatosDouble[9]=Auxiliar[9].toDouble();
    BMS_VOLT[10] = DatosDouble[9]*0.0001;                     // [30 Voltajes]
    // @ L 4c
    Auxiliar[10]=(Auxiliar[10].append(bytes[65]).append(bytes[66]).append(bytes[67]).append(bytes[68]).append(bytes[69]));
    DatosDouble[10]=Auxiliar[10].toDouble();
    BMS_VOLT[11] = DatosDouble[10]*0.0001;                     // [30 Voltajes]
    // @ M 4d
    Auxiliar[11]=(Auxiliar[11].append(bytes[72]).append(bytes[73]).append(bytes[74]).append(bytes[75]).append(bytes[76]));
    DatosDouble[11]=Auxiliar[11].toDouble();
    BMS_VOLT[12] = DatosDouble[11]*0.0001;                     // [30 Voltajes]

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_N(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // @ N 4e
    Auxiliar[2]=(Auxiliar[2].append(bytes[9]).append(bytes[10]).append(bytes[11]).append(bytes[12]).append(bytes[13]));
    DatosDouble[2]=Auxiliar[2].toDouble();
    BMS_VOLT[13] = DatosDouble[2]*0.0001;                     // [30 Voltajes]
    // @ O 4f
    Auxiliar[3]=(Auxiliar[3].append(bytes[16]).append(bytes[17]).append(bytes[18]).append(bytes[19]).append(bytes[20]));
    DatosDouble[3]=Auxiliar[3].toDouble();
    BMS_VOLT[14] = DatosDouble[3]*0.0001;                     // [30 Voltajes]
    // @ P 50
    Auxiliar[4]=(Auxiliar[4].append(bytes[23]).append(bytes[24]).append(bytes[25]).append(bytes[26]).append(bytes[27]));
    DatosDouble[4]=Auxiliar[4].toDouble();
    BMS_VOLT[15] = DatosDouble[4]*0.0001;                     // [30 Voltajes]
    // @ Q 51
    Auxiliar[5]=(Auxiliar[5].append(bytes[30]).append(bytes[31]).append(bytes[32]).append(bytes[33]).append(bytes[34]));
    DatosDouble[5]=Auxiliar[5].toDouble();
    BMS_VOLT[16] = DatosDouble[5]*0.0001;                     // [30 Voltajes]
    // @ R 52
    Auxiliar[6]=(Auxiliar[6].append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]).append(bytes[41]));
    DatosDouble[6]=Auxiliar[6].toDouble();
    BMS_VOLT[17] = DatosDouble[6]*0.0001;                     // [30 Voltajes]
    // @ S 53
    Auxiliar[7]=(Auxiliar[7].append(bytes[44]).append(bytes[45]).append(bytes[46]).append(bytes[47]).append(bytes[48]));
    DatosDouble[7]=Auxiliar[7].toDouble();
    BMS_VOLT[18] = DatosDouble[7]*0.0001;                     // [30 Voltajes]
    // @ T 54
    Auxiliar[8]=(Auxiliar[8].append(bytes[51]).append(bytes[52]).append(bytes[53]).append(bytes[54]).append(bytes[55]));
    DatosDouble[8]=Auxiliar[8].toDouble();
    BMS_VOLT[19] = DatosDouble[8]*0.0001;                     // [30 Voltajes]
    // @ U 55
    Auxiliar[9]=(Auxiliar[9].append(bytes[58]).append(bytes[59]).append(bytes[60]).append(bytes[61]).append(bytes[62]));
    DatosDouble[9]=Auxiliar[9].toDouble();
    BMS_VOLT[20] = DatosDouble[9]*0.0001;                     // [30 Voltajes]
    // @ V 56
    Auxiliar[10]=(Auxiliar[10].append(bytes[65]).append(bytes[66]).append(bytes[67]).append(bytes[68]).append(bytes[69]));
    DatosDouble[10]=Auxiliar[10].toDouble();
    BMS_VOLT[21] = DatosDouble[10]*0.0001;                     // [30 Voltajes]
    // @ W 57
    Auxiliar[11]=(Auxiliar[11].append(bytes[72]).append(bytes[73]).append(bytes[74]).append(bytes[75]).append(bytes[76]));
    DatosDouble[11]=Auxiliar[11].toDouble();
    BMS_VOLT[22] = DatosDouble[11]*0.0001;                     // [30 Voltajes]

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_X(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // @ X 58
    Auxiliar[2]=(Auxiliar[2].append(bytes[9]).append(bytes[10]).append(bytes[11]).append(bytes[12]).append(bytes[13]));
    DatosDouble[2]=Auxiliar[2].toDouble();
    BMS_VOLT[23] = DatosDouble[2]*0.0001;                     // [30 Voltajes]
    // @ Y 59
    Auxiliar[3]=(Auxiliar[3].append(bytes[16]).append(bytes[17]).append(bytes[18]).append(bytes[19]).append(bytes[20]));
    DatosDouble[3]=Auxiliar[3].toDouble();
    BMS_VOLT[24] = DatosDouble[3]*0.0001;                     // [30 Voltajes]
    // @ Z 5a
    Auxiliar[4]=(Auxiliar[4].append(bytes[23]).append(bytes[24]).append(bytes[25]).append(bytes[26]).append(bytes[27]));
    DatosDouble[4]=Auxiliar[4].toDouble();
    BMS_VOLT[25] = DatosDouble[4]*0.0001;                     // [30 Voltajes]
    // @ a 61
    Auxiliar[5]=(Auxiliar[5].append(bytes[30]).append(bytes[31]).append(bytes[32]).append(bytes[33]).append(bytes[34]));
    DatosDouble[5]=Auxiliar[5].toDouble();
    BMS_VOLT[26] = DatosDouble[5]*0.0001;                     // [30 Voltajes]
    // @ b 62
    Auxiliar[6]=(Auxiliar[6].append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]).append(bytes[41]));
    DatosDouble[6]=Auxiliar[6].toDouble();
    BMS_VOLT[27] = DatosDouble[6]*0.0001;                     // [30 Voltajes]
    // m 6d
    Auxiliar[7]=(Auxiliar[7].append(bytes[43]).append(bytes[44]).append(bytes[45]).append(bytes[46]).append(bytes[47]).append(bytes[48]));
    DatosDouble[7]=Auxiliar[7].toDouble();
    MPPT1[0] = DatosDouble[7]*0.0001;                        // [0] = Voltaje IN
    // n 6e
    Auxiliar[8]=(Auxiliar[8].append(bytes[50]).append(bytes[51]).append(bytes[52]).append(bytes[53]).append(bytes[54]).append(bytes[55]));
    DatosDouble[8]=Auxiliar[8].toDouble();
    MPPT1[1] = DatosDouble[8]*0.0001;                        // [1] = Amperaje IN
    // o 6f
    Auxiliar[9]=(Auxiliar[9].append(bytes[57]).append(bytes[58]).append(bytes[59]).append(bytes[60]).append(bytes[61]).append(bytes[62]));
    DatosDouble[9]=Auxiliar[9].toDouble();
    MPPT1[2] = DatosDouble[9]*0.001;                         // [2] = Voltaje OUT
    // p 70
    Auxiliar[10]=(Auxiliar[10].append(bytes[64]));
    Auxiliar[11]=(Auxiliar[11].append(bytes[65]));
    Auxiliar[12]=(Auxiliar[12].append(bytes[66]));
    Auxiliar[13]=(Auxiliar[13].append(bytes[67]));
    Auxiliar[14]=(Auxiliar[14].append(bytes[68]).append(bytes[69]));
    DatosDouble[10]=Auxiliar[10].toDouble();
    DatosDouble[11]=Auxiliar[11].toDouble();
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    DatosDouble[14]=Auxiliar[14].toDouble();
    MPPT1[3] = DatosDouble[10];                              // [3] = BVLR
    MPPT1[4] = DatosDouble[11];                              // [4] = OVT
    MPPT1[5] = DatosDouble[12];                              // [5] = NOC
    MPPT1[6] = DatosDouble[13];                              // [6] = UNDV
    MPPT1[7] = DatosDouble[14];                              // [7] = TEMP
    // q 71
    Auxiliar[15]=(Auxiliar[15].append(bytes[71]).append(bytes[72]).append(bytes[73]).append(bytes[74]).append(bytes[75]).append(bytes[76]));
    DatosDouble[15]=Auxiliar[15].toDouble();
    MPPT2[0] = DatosDouble[15]*0.001;                        // [0] = Voltaje IN

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    Dialog::ActualizarPotenciaMPPT(MPPT1[0],MPPT1[1],MPPT2[0],MPPT2[1],MPPT3[0],MPPT3[1],MPPT4[0],MPPT4[1],MPPT5[0],MPPT5[1],MPPT6[0],MPPT6[1]);
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_r(QByteArray bytes){
    // l
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));   // speed
    Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));   // BMS[1] = Pack Current
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    speed = DatosDouble[0];
    BMS[1] = DatosDouble[1];
    // r 72
    Auxiliar[2]=(Auxiliar[2].append(bytes[8]).append(bytes[9]).append(bytes[10]).append(bytes[11]).append(bytes[12]).append(bytes[13]));
    DatosDouble[2]=Auxiliar[2].toDouble();
    MPPT2[1] = DatosDouble[2]*0.001;                        // [1] = Amperaje IN
    // s 73
    Auxiliar[3]=(Auxiliar[3].append(bytes[15]).append(bytes[16]).append(bytes[17]).append(bytes[18]).append(bytes[19]).append(bytes[20]));
    DatosDouble[3]=Auxiliar[3].toDouble();
    MPPT2[2] = DatosDouble[3]*0.001;                         // [2] = Voltaje OUT
    // t 74
    Auxiliar[4]=(Auxiliar[4].append(bytes[22]));
    Auxiliar[5]=(Auxiliar[5].append(bytes[23]));
    Auxiliar[6]=(Auxiliar[6].append(bytes[24]));
    Auxiliar[7]=(Auxiliar[7].append(bytes[25]));
    Auxiliar[8]=(Auxiliar[8].append(bytes[26]).append(bytes[27]));
    DatosDouble[4]=Auxiliar[4].toDouble();
    DatosDouble[5]=Auxiliar[5].toDouble();
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    DatosDouble[8]=Auxiliar[8].toDouble();
    MPPT2[3] = DatosDouble[4];                              // [3] = BVLR
    MPPT2[4] = DatosDouble[5];                              // [4] = OVT
    MPPT2[5] = DatosDouble[6];                              // [5] = NOC
    MPPT2[6] = DatosDouble[7];                              // [6] = UNDV
    MPPT2[7] = DatosDouble[8];                              // [7] = TEMP

    // u 75
    Auxiliar[9]=(Auxiliar[9].append(bytes[29]).append(bytes[30]).append(bytes[31]).append(bytes[32]).append(bytes[33]).append(bytes[34]));
    DatosDouble[9]=Auxiliar[9].toDouble();
    MPPT3[0] = DatosDouble[9]*0.001;                        // [0] = Voltaje IN
    // v 76
    Auxiliar[10]=(Auxiliar[10].append(bytes[36]).append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]).append(bytes[41]));
    DatosDouble[10]=Auxiliar[10].toDouble();
    MPPT3[1] = DatosDouble[10]*0.001;                        // [1] = Amperaje IN
    // w 77
    Auxiliar[11]=(Auxiliar[11].append(bytes[43]).append(bytes[44]).append(bytes[45]).append(bytes[46]).append(bytes[47]).append(bytes[48]));
    DatosDouble[11]=Auxiliar[11].toDouble();
    MPPT3[2] = DatosDouble[11]*0.001;                         // [2] = Voltaje OUT
    // x 78
    Auxiliar[12]=(Auxiliar[12].append(bytes[50]));
    Auxiliar[13]=(Auxiliar[13].append(bytes[51]));
    Auxiliar[14]=(Auxiliar[14].append(bytes[52]));
    Auxiliar[15]=(Auxiliar[15].append(bytes[53]));
    Auxiliar[16]=(Auxiliar[16].append(bytes[54]).append(bytes[55]));
    DatosDouble[12]=Auxiliar[12].toDouble();
    DatosDouble[13]=Auxiliar[13].toDouble();
    DatosDouble[14]=Auxiliar[14].toDouble();
    DatosDouble[15]=Auxiliar[15].toDouble();
    DatosDouble[16]=Auxiliar[16].toDouble();
    MPPT3[3] = DatosDouble[12];                              // [3] = BVLR
    MPPT3[4] = DatosDouble[13];                              // [4] = OVT
    MPPT3[5] = DatosDouble[14];                              // [5] = NOC
    MPPT3[6] = DatosDouble[15];                              // [6] = UNDV
    MPPT3[7] = DatosDouble[16];                              // [7] = TEMP

    // y 79
    Auxiliar[17]=(Auxiliar[17].append(bytes[57]).append(bytes[58]).append(bytes[59]).append(bytes[60]).append(bytes[61]).append(bytes[62]));
    DatosDouble[17]=Auxiliar[17].toDouble();
    MPPT4[0] = DatosDouble[17]*0.001;                        // [0] = Voltaje IN
    // z 7a
    Auxiliar[18]=(Auxiliar[18].append(bytes[64]).append(bytes[65]).append(bytes[66]).append(bytes[67]).append(bytes[68]).append(bytes[69]));
    DatosDouble[18]=Auxiliar[18].toDouble();
    MPPT4[1] = DatosDouble[18]*0.001;                        // [1] = Amperaje IN
    // { 7b
    Auxiliar[19]=(Auxiliar[19].append(bytes[71]).append(bytes[72]).append(bytes[73]).append(bytes[74]).append(bytes[75]).append(bytes[76]));
    DatosDouble[19]=Auxiliar[19].toDouble();
    MPPT4[2] = DatosDouble[19]*0.001;                         // [2] = Voltaje OUT
    // } 7d
    Auxiliar[20]=(Auxiliar[20].append(bytes[78]));
    Auxiliar[21]=(Auxiliar[21].append(bytes[79]));
    Auxiliar[22]=(Auxiliar[22].append(bytes[80]));
    Auxiliar[23]=(Auxiliar[23].append(bytes[81]));
    Auxiliar[24]=(Auxiliar[24].append(bytes[82]).append(bytes[83]));
    DatosDouble[20]=Auxiliar[20].toDouble();
    DatosDouble[21]=Auxiliar[21].toDouble();
    DatosDouble[22]=Auxiliar[22].toDouble();
    DatosDouble[23]=Auxiliar[23].toDouble();
    DatosDouble[24]=Auxiliar[24].toDouble();
    MPPT4[3] = DatosDouble[20];                              // [3] = BVLR
    MPPT4[4] = DatosDouble[21];                              // [4] = OVT
    MPPT4[5] = DatosDouble[22];                              // [5] = NOC
    MPPT4[6] = DatosDouble[23];                              // [6] = UNDV
    MPPT4[7] = DatosDouble[24];                              // [7] = TEMP

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    Dialog::ActualizarPotenciaMPPT(MPPT1[0],MPPT1[1],MPPT2[0],MPPT2[1],MPPT3[0],MPPT3[1],MPPT4[0],MPPT4[1],MPPT5[0],MPPT5[1],MPPT6[0],MPPT6[1]);
    for(int i=0;i<32;i++){
        Auxiliar[i]="";
    }
    serialSelected.clear();
}
void Dialog::Update_display_7(QByteArray bytes){
    //Speed-Current
    //qDebug() << "BYTES: " << bytes  << endl;
    if(bytes[0]==char(0x6c)){
        //qDebug() << "DISPLAY l " << endl;
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        speed = DatosDouble[0];                                 // Velocidad
        //qDebug() << "Velocidad: " << speed << endl;
        mSpeedNeedle->setCurrentValue(speed);
        ui->lcd_speed->display(speed);
        BMS[1] = DatosDouble[1];                                // [1] = Pack Current

    //Kelly DER
    }else if(bytes[0]==char(0x41)){
        //qDebug() << "DISPLAY A " << endl;
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        KELLY_DER[0] = DatosDouble[0];                          // [0] Ia
        KELLY_DER[1] = DatosDouble[1];                          // [1] Ib
        KELLY_DER[2] = DatosDouble[2];                          // [2] Ic
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    }else if(bytes[0]==char(0x42)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        KELLY_DER[3] = DatosDouble[0];                          // [3] Va
        KELLY_DER[4] = DatosDouble[1];                          // [4] Vb
        KELLY_DER[5] = DatosDouble[2];                          // [5] Vc
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    }else if(bytes[0]==char(0x43)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        KELLY_DER[6] = DatosDouble[0];                          // [6] = RPM
        KELLY_DER[8] = DatosDouble[1];                          // [8] = PWM
    }else if(bytes[0]==char(0x44)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        KELLY_DER[7] = DatosDouble[0];                          // [7] = ERROR CODE
        KELLY_DER[11] = DatosDouble[1];                         // [11] = KELLY TEMP
    }else if(bytes[0]==char(0x45)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        KELLY_DER[9] = DatosDouble[0];                          // [9] = EMR
        KELLY_DER[10] = DatosDouble[1];                         // [10] = MOTOR TEMP
        KELLY_DER[12] = DatosDouble[2];                         // [12] = Throttle Switch Status DERECHO
        KELLY_DER[13] = DatosDouble[3];                         // [13] = Reverse Switch Status DERECHO

    //Kelly IZ
    }else if(bytes[0]==char(0x46)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        KELLY_IZ[0] = DatosDouble[0];                          // [0] Ia
        KELLY_IZ[1] = DatosDouble[1];                          // [1] Ib
        KELLY_IZ[2] = DatosDouble[2];                          // [2] Ic
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    }else if(bytes[0]==char(0x47)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        KELLY_IZ[3] = DatosDouble[0];                          // [3] Va
        KELLY_IZ[4] = DatosDouble[1];                          // [4] Vb
        KELLY_IZ[5] = DatosDouble[2];                          // [5] Vc
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    }else if(bytes[0]==char(0x48)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        KELLY_IZ[6] = DatosDouble[0];                          // [6] = RPM
        KELLY_IZ[8] = DatosDouble[1];                          // [8] = PWM
    }else if(bytes[0]==char(0x49)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        KELLY_IZ[7] = DatosDouble[0];                          // [7] = ERROR CODE
        KELLY_IZ[11] = DatosDouble[1];                         // [11] = KELLY TEMP
    }else if(bytes[0]==char(0x4a)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]).append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        KELLY_IZ[9] = DatosDouble[0];                          // [9] = EMR
        KELLY_IZ[10] = DatosDouble[1];                         // [10] = MOTOR TEMP
        KELLY_IZ[12] = DatosDouble[2];                         // [12] = Throttle Switch Status DERECHO
        KELLY_IZ[13] = DatosDouble[3];                         // [13] = Reverse Switch Status DERECHO
    }else if(bytes[0]==char(0x4b)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        BMS[1] = DatosDouble[0];                                // [1] = Pack Current
        BMS[2] = DatosDouble[1];                                // [2] = Pack Inst Volt
    }else if(bytes[0]==char(0x4c)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        BMS[0] = DatosDouble[0];                                // [0] = SOC
        BMS[3] = DatosDouble[1];                                // [3] = Pack Open Volt
    }else if(bytes[0]==char(0x4d)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS[4] = DatosDouble[0];                                // [4] = Pack Abs Current
        BMS[7] = DatosDouble[1];                                // [7] = High Temp
        BMS[8] = DatosDouble[2];                                // [8] = High Temp Thermistor ID
    }else if(bytes[0]==char(0x4e)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        BMS[5] = DatosDouble[0];                                // [5] = Maximum Pack Volt
    }else if(bytes[0]==char(0x4f)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        BMS[6] = DatosDouble[0];                                // [6] = Minimum Pack Volt
    }else if(bytes[0]==char(0x50)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS[9] = DatosDouble[0];                                 // [9] = Low Temp
        BMS[10] = DatosDouble[1];                                // [10] = Low Temp Thermistor ID
        BMS[11] = DatosDouble[2];                                // [11] = Average Temp
    }else if(bytes[0]==char(0x51)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS[12] = DatosDouble[0];                                // [12] = Internal Temp
        BMS[13] = DatosDouble[1];                                // [13] = Max Volt ID
        BMS[14] = DatosDouble[2];                                // [14] = Min Volt ID

    // BMS TEMP
    }else if(bytes[0]==char(0x52)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[0] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[1] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[2] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x53)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[3] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[4] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[5] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x54)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[6] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[7] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[8] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x55)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[9] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[10] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[11] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x56)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[12] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[13] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[14] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x57)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[15] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[16] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[17] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x58)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[18] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[19] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[20] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x59)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[21] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[22] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[23] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x5a)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[24] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[25] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[26] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x61)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[27] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[28] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[29] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x62)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[30] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[31] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[32] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x63)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[33] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[34] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[35] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x64)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[36] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[37] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[38] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x65)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[39] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[40] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[41] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x66)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[42] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[43] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[44] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x67)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[45] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[46] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[47] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x68)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[48] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[49] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[50] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x69)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[51] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[52] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[53] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x6a)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[54] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[55] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[56] = DatosDouble[2];                           // [60 Temperaturas]
    }else if(bytes[0]==char(0x6b)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[3]).append(bytes[4]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        BMS_TEMP[57] = DatosDouble[0];                           // [60 Temperaturas]
        BMS_TEMP[58] = DatosDouble[1];                           // [60 Temperaturas]
        BMS_TEMP[59] = DatosDouble[2];                           // [60 Temperaturas]

    }else if(bytes[0]==char(0x6d)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT1[0] = DatosDouble[0]*0.0001;                        // [0] = Voltaje IN
    }else if(bytes[0]==char(0x6e)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT1[1] = DatosDouble[0]*0.0001;                        // [1] = Amperaje IN
    }else if(bytes[0]==char(0x6f)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT1[2] = DatosDouble[0]*0.001;                         // [2] = Voltaje OUT
    }else if(bytes[0]==char(0x70)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[3]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[4]));
        Auxiliar[4]=(Auxiliar[4].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        DatosDouble[4]=Auxiliar[4].toDouble();
        MPPT1[3] = DatosDouble[0];                              // [3] = BVLR
        MPPT1[4] = DatosDouble[1];                              // [4] = OVT
        MPPT1[5] = DatosDouble[2];                              // [5] = NOC
        MPPT1[6] = DatosDouble[3];                              // [6] = UNDV
        MPPT1[7] = DatosDouble[4];                              // [7] = TEMP

    }else if(bytes[0]==char(0x71)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT2[0] = DatosDouble[0]*0.001;                        // [0] = Voltaje IN
    }else if(bytes[0]==char(0x72)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT2[1] = DatosDouble[0]*0.001;                        // [1] = Amperaje IN
    }else if(bytes[0]==char(0x73)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT2[2] = DatosDouble[0]*0.001;                         // [2] = Voltaje OUT
    }else if(bytes[0]==char(0x74)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[3]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[4]));
        Auxiliar[4]=(Auxiliar[4].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        DatosDouble[4]=Auxiliar[4].toDouble();
        MPPT2[3] = DatosDouble[0];                              // [3] = BVLR
        MPPT2[4] = DatosDouble[1];                              // [4] = OVT
        MPPT2[5] = DatosDouble[2];                              // [5] = NOC
        MPPT2[6] = DatosDouble[3];                              // [6] = UNDV
        MPPT2[7] = DatosDouble[4];                              // [7] = TEMP

    }else if(bytes[0]==char(0x75)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT3[0] = DatosDouble[0]*0.001;                        // [0] = Voltaje IN
    }else if(bytes[0]==char(0x76)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT3[1] = DatosDouble[0]*0.001;                        // [1] = Amperaje IN
    }else if(bytes[0]==char(0x77)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT3[2] = DatosDouble[0]*0.001;                         // [2] = Voltaje OUT
    }else if(bytes[0]==char(0x78)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[3]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[4]));
        Auxiliar[4]=(Auxiliar[4].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        DatosDouble[4]=Auxiliar[4].toDouble();
        MPPT3[3] = DatosDouble[0];                              // [3] = BVLR
        MPPT3[4] = DatosDouble[1];                              // [4] = OVT
        MPPT3[5] = DatosDouble[2];                              // [5] = NOC
        MPPT3[6] = DatosDouble[3];                              // [6] = UNDV
        MPPT3[7] = DatosDouble[4];                              // [7] = TEMP

    }else if(bytes[0]==char(0x79)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT4[0] = DatosDouble[0]*0.001;                        // [0] = Voltaje IN
    }else if(bytes[0]==char(0x7a)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT4[1] = DatosDouble[0]*0.001;                        // [1] = Amperaje IN
    }else if(bytes[0]==char(0x7b)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        MPPT4[2] = DatosDouble[0]*0.001;                         // [2] = Voltaje OUT
    }else if(bytes[0]==char(0x7d)){
        Auxiliar[0]=(Auxiliar[0].append(bytes[1]));
        Auxiliar[1]=(Auxiliar[1].append(bytes[2]));
        Auxiliar[2]=(Auxiliar[2].append(bytes[3]));
        Auxiliar[3]=(Auxiliar[3].append(bytes[4]));
        Auxiliar[4]=(Auxiliar[4].append(bytes[5]).append(bytes[6]));
        DatosDouble[0]=Auxiliar[0].toDouble();
        DatosDouble[1]=Auxiliar[1].toDouble();
        DatosDouble[2]=Auxiliar[2].toDouble();
        DatosDouble[3]=Auxiliar[3].toDouble();
        DatosDouble[4]=Auxiliar[4].toDouble();
        MPPT4[3] = DatosDouble[0];                              // [3] = BVLR
        MPPT4[4] = DatosDouble[1];                              // [4] = OVT
        MPPT4[5] = DatosDouble[2];                              // [5] = NOC
        MPPT4[6] = DatosDouble[3];                              // [6] = UNDV
        MPPT4[7] = DatosDouble[4];                              // [7] = TEMP
    }

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current
    Dialog::ActualizarPotenciaMPPT(MPPT1[0],MPPT1[1],MPPT2[0],MPPT2[1],MPPT3[0],MPPT3[1],MPPT4[0],MPPT4[1],MPPT5[0],MPPT5[1],MPPT6[0],MPPT6[1]);
    Auxiliar[0]="";
    Auxiliar[1]="";
    Auxiliar[2]="";
    Auxiliar[3]="";
    Auxiliar[4]="";
    Auxiliar[5]="";
    Auxiliar[6]="";
    Auxiliar[7]="";
    Auxiliar[8]="";
    Auxiliar[9]="";
    serialSelected.clear();
}
void Dialog::Update_display_7_volt(QByteArray bytes){
    Auxiliar[0]=(Auxiliar[0].append(bytes[2]).append(bytes[3]).append(bytes[4]).append(bytes[5]).append(bytes[6]));
    DatosDouble[0]=Auxiliar[0].toDouble();
    //qDebug() << "Voltaje = " << DatosDouble[0] << endl;
    if(bytes[1] == char(0x41)){
        BMS_VOLT[0] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x42)){
        BMS_VOLT[1] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x43)){
        BMS_VOLT[2] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x44)){
        BMS_VOLT[3] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x45)){
        BMS_VOLT[4] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x46)){
        BMS_VOLT[5] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x47)){
        BMS_VOLT[6] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x48)){
        BMS_VOLT[7] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x49)){
        BMS_VOLT[8] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4a)){
        BMS_VOLT[9] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4b)){
        BMS_VOLT[10] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4c)){
        BMS_VOLT[11] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4d)){
        BMS_VOLT[12] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4e)){
        BMS_VOLT[13] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x4f)){
        BMS_VOLT[14] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x50)){
        BMS_VOLT[15] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x51)){
        BMS_VOLT[16] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x52)){
        BMS_VOLT[17] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x53)){
        BMS_VOLT[18] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x54)){
        BMS_VOLT[19] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x55)){
        BMS_VOLT[20] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x56)){
        BMS_VOLT[21] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x57)){
        BMS_VOLT[22] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x58)){
        BMS_VOLT[23] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x59)){
        BMS_VOLT[24] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x5a)){
        BMS_VOLT[25] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x61)){
        BMS_VOLT[26] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }else if(bytes[1] == char(0x62)){
        BMS_VOLT[27] = DatosDouble[0]*0.0001;                     // [30 Voltajes]
    }
    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Auxiliar[0]="";
    Auxiliar[1]="";
    Auxiliar[2]="";
    Auxiliar[3]="";
    Auxiliar[4]="";
    Auxiliar[5]="";
    Auxiliar[6]="";
    Auxiliar[7]="";
    Auxiliar[8]="";
    Auxiliar[9]="";
    serialSelected.clear();
}

/*//////////////// FIN Formato de 7 bytes NEW /////////////////*/


/*////////////////// Formato de 41 bytes  ///////////////////*/
//Cumple misma función que Update_filter1, con el mismo rendimiento pero definido de forma más compleja; usar Update_filter1
void Dialog::Update_filter(QByteArray bits){
    if ((Primer_bit_es_letra(bits))&&(bits.length()>=41*(contador+1))&&(not(Contiene_letras(bits)))){
        for(int i=0;i<41;i++){                      //CASO: A(40)(...) ,contador=0 || (...)B(40)(...) ,contador>0
            serialSelected[i]=bits[41*contador+i];
        }
        qDebug() << "serialSelected: " << serialSelected << endl;
        Dialog::Update_display(serialSelected);
        serialSelected.clear();
        contador+=1;
        Dialog::Update_filter(bits);
    }else{
        if(contador>0){
            qDebug() << "Contador>0" << contador<< " " << bits.length()<< endl;
            if(bits.length()>=41*(contador+1)){     //CASO: (...)B(12)A(40)(...) || (...)(12)BA(40)(...) || (...)(12)B(12)A(40)(...) || (...)BC(12)BDA(40)(...) ||(...)BCDA(40)(...) || (...)(12)A(40)
                bits.remove(0, 41*(contador));
                contador=0;  // Ya que eliminamos la parte anterior al error derivamos a los casos(...)  123A(40)(...) || BSCA(40)(...) || (12)BD(12)A(40)(...) || BD(12)DSA(40)(...), y aquí el contador vuelve a 0
                Dialog::Update_filter(bits); // Movemos índice hasta lelgar a A(40)(...), contador>0
            }else{
                qDebug() << "(...)A(12)" << contador<< " " << bits.length()<< endl;
                contador=0; //CASO: (...)A(12) || (...)(12)A || (...)(12)A(12) || ADB(12)ABC ,nada más que leer
                bits.clear();
                return;
            }
        }else{
            if(bits.length()>41){
                //CASO: 123A(40)(...) || BSCA(40)(...) || (12)BD(12)A(40)(...) || BD(12)DSA(40)(...) ,contador=0
                bits.remove(0, 1);                                        //Se mueve el bloque en un byte
                Dialog::Update_filter(bits); // Movemos índice hasta lelgar a A(40)(...), contador=0
            }else{
               bits.clear(); // Nada más que leer, CASOS: (21)SF , (12), SF(21), (ABC) (largos cortos)
            }
        }
    }
}

//Toma el bloque de bytes recibidos y hace display de cada cadena leíble en el bloque
void Dialog::Update_filter1(QByteArray bloque){
    //qDebug() << bloque << endl;
    if(bloque.length()>=41){
        if(Primer_bit_es_letra(bloque)&&(not(Contiene_letras(bloque)))){
            for(int i=0;i<41;i++){                                      //Seleccionamos primeros 41 bytes aparte
                serialSelected[i]=bloque[+i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_display(serialSelected);                     //Hacemos display
            serialSelected.clear();                                     //Limpiamos vector de bytes
            bloque.remove(0, 41);                                       //Cortamos lo que ya leímos
            Dialog::Update_filter1(bloque);                             //Leemos los bytes que quedan
        }else{
            bloque.remove(0, 1);                                        //Se mueve el bloque en un byte
            Dialog::Update_filter1(bloque);                             //Se vuelve a intentar
        }
    }else{
        //qDebug() << "Largo<41 " << endl;
        bloque.clear();                                                 //No hay nada más que leer
    }
}

// Verifica si el primer byte de cada cadena es letra; true= es letra, false= no es letra
bool Dialog::Primer_bit_es_letra(QByteArray bits){ //Depende del contador
    bool estado_inicial = false;
    if ((bits[0+41*contador]==char(0x40))||(bits[0+41*contador]==char(0x41))||(bits[0+41*contador]==char(0x42))||(bits[0+41*contador]==char(0x43))||(bits[0+41*contador]==char(0x44))||(bits[0+41*contador]==char(0x45))||(bits[0+41*contador]==char(0x46))||(bits[0+41*contador]==char(0x47))||(bits[0+41*contador]==char(0x48))||(bits[0+41*contador]==char(0x49))||(bits[0+41*contador]==char(0x4a))||(bits[0+41*contador]==char(0x4b))||(bits[0+41*contador]==char(0x4c))||(bits[0+41*contador]==char(0x4d))||(bits[0+41*contador]==char(0x4e))||(bits[0+41*contador]==char(0x4f))||(bits[0+41*contador]==char(0x50))||(bits[0+41*contador]==char(0x51))||(bits[0+41*contador]==char(0x52))||(bits[0+41*contador]==char(0x53))||(bits[0+41*contador]==char(0x54))||(bits[0+41*contador]==char(0x55))||(bits[0+41*contador]==char(0x56))||(bits[0+41*contador]==char(0x57))||(bits[0+41*contador]==char(0x58))||(bits[0+41*contador]==char(0x59))||(bits[0+41*contador]==char(0x5a))){
        estado_inicial = true;
    }
    return estado_inicial;
}

//Verifica en cada cadena si los siguientes 40 bytes contienen letras; true=contiene, false=no contiene
bool Dialog::Contiene_letras(QByteArray bloque){ //Depende del contador
    bool estado_inicial = false;
    if(bloque.length()>=41*(contador+1)){
        for(int a=1;a<41;a++){
            if((bloque[a+41*contador]==char(0x40))||(bloque[a+41*contador]==char(0x41))||(bloque[a+41*contador]==char(0x42))||(bloque[a+41*contador]==char(0x43))||(bloque[a+41*contador]==char(0x44))||(bloque[a+41*contador]==char(0x45))||(bloque[a+41*contador]==char(0x46))||(bloque[a+41*contador]==char(0x47))||(bloque[a+41*contador]==char(0x48))||(bloque[a+41*contador]==char(0x49))||(bloque[a+41*contador]==char(0x4a))||(bloque[a+41*contador]==char(0x4b))||(bloque[a+41*contador]==char(0x4c))||(bloque[a+41*contador]==char(0x4d))||(bloque[a+41*contador]==char(0x4e))||(bloque[a+41*contador]==char(0x4f))||(bloque[a+41*contador]==char(0x50))||(bloque[a+41*contador]==char(0x51))||(bloque[a+41*contador]==char(0x52))||(bloque[a+41*contador]==char(0x53))||(bloque[a+41*contador]==char(0x54))||(bloque[a+41*contador]==char(0x55))||(bloque[a+41*contador]==char(0x56))||(bloque[a+41*contador]==char(0x57))||(bloque[a+41*contador]==char(0x58))||(bloque[a+41*contador]==char(0x59))||(bloque[a+41*contador]==char(0x5a))){
            estado_inicial = true;
            }
        }
    }
    return estado_inicial;
}

void Dialog::Update_display(QByteArray bytes){
    Auxiliar[0]=(Auxiliar[0].append(bytes[1]).append(bytes[2]).append(bytes[3]).append(bytes[4]));
    Auxiliar[1]=(Auxiliar[1].append(bytes[5]).append(bytes[6]).append(bytes[7]).append(bytes[8]));
    Auxiliar[2]=(Auxiliar[2].append(bytes[9]).append(bytes[10]).append(bytes[11]).append(bytes[12]));
    Auxiliar[3]=(Auxiliar[3].append(bytes[13]).append(bytes[14]).append(bytes[15]).append(bytes[16]));
    Auxiliar[4]=(Auxiliar[4].append(bytes[17]).append(bytes[18]).append(bytes[19]).append(bytes[20]));
    Auxiliar[5]=(Auxiliar[5].append(bytes[21]).append(bytes[22]).append(bytes[23]).append(bytes[24]));
    Auxiliar[6]=(Auxiliar[6].append(bytes[25]).append(bytes[26]).append(bytes[27]).append(bytes[28]));
    Auxiliar[7]=(Auxiliar[7].append(bytes[29]).append(bytes[30]).append(bytes[31]).append(bytes[32]));
    Auxiliar[8]=(Auxiliar[8].append(bytes[33]).append(bytes[34]).append(bytes[35]).append(bytes[36]));
    Auxiliar[9]=(Auxiliar[9].append(bytes[37]).append(bytes[38]).append(bytes[39]).append(bytes[40]));
    DatosDouble[0]=Auxiliar[0].toDouble();
    DatosDouble[1]=Auxiliar[1].toDouble();
    DatosDouble[2]=Auxiliar[2].toDouble();
    DatosDouble[3]=Auxiliar[3].toDouble();
    DatosDouble[4]=Auxiliar[4].toDouble();
    DatosDouble[5]=Auxiliar[5].toDouble();
    DatosDouble[6]=Auxiliar[6].toDouble();
    DatosDouble[7]=Auxiliar[7].toDouble();
    DatosDouble[8]=Auxiliar[8].toDouble();
    DatosDouble[9]=Auxiliar[9].toDouble();

    if(bytes[0] == char(0x40)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        KELLY_DER[0] = DatosDouble[4];                          // [0] Ia
        KELLY_DER[1] = DatosDouble[5];                          // [1] Ib
        KELLY_DER[2] = DatosDouble[6];                          // [2] Ic
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
        KELLY_DER[3] = DatosDouble[7];                          // [3] Va
        KELLY_DER[4] = DatosDouble[8];                          // [4] Vb
        KELLY_DER[5] = DatosDouble[9];                          // [5] Vc
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]); // DER: Va | Vb | Vc | Ia | Ib | Ic | IZ: Va | Vb | Vc | Ia | Ib | Ic
    }else if(bytes[0] == char(0x41)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        KELLY_DER[6] = DatosDouble[4];                          // [6] = RPM
        KELLY_DER[7] = DatosDouble[5];                          // [7] = ERROR CODE
        KELLY_DER[8] = DatosDouble[6];                          // [8] = PWM
        KELLY_DER[9] = DatosDouble[7];                          // [9] = EMR
        KELLY_DER[10] = DatosDouble[8];                         // [10] = MOTOR TEMP
        KELLY_DER[11] = DatosDouble[9];                         // [11] = KELLY TEMP
    }else if(bytes[0] == char(0x42)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        KELLY_DER[12] = DatosDouble[4];                         // [12] = Throttle Switch Status DERECHO
        KELLY_DER[13] = DatosDouble[5];                         // [13] = Reverse Switch Status DERECHO
        KELLY_IZ[0] = DatosDouble[6];                           // [0] Ia
        KELLY_IZ[1] = DatosDouble[7];                           // [1] Ib
        KELLY_IZ[2] = DatosDouble[8];                           // [2] Ic
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]);
        KELLY_IZ[3] = DatosDouble[9];                           // [3] Va
    }else if(bytes[0] == char(0x43)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        KELLY_IZ[4] = DatosDouble[4];                           // [4] Vb
        KELLY_IZ[5] = DatosDouble[5];                           // [5] Vc
        Dialog::ActualizarPotenciaKelly(KELLY_DER[3], KELLY_DER[4], KELLY_DER[5], KELLY_DER[0], KELLY_DER[1], KELLY_DER[2], KELLY_IZ[3], KELLY_IZ[4], KELLY_IZ[5], KELLY_IZ[0], KELLY_IZ[1], KELLY_IZ[2]);
        KELLY_IZ[6] = DatosDouble[6];                           // [6] = RPM
        KELLY_IZ[7] = DatosDouble[7];                           // [7] = ERROR CODE
        KELLY_IZ[8] = DatosDouble[8];                           // [8] = PWM
        KELLY_IZ[9] = DatosDouble[9];                           // [9] = EMR
    }else if(bytes[0] == char(0x44)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        KELLY_IZ[10] = DatosDouble[4];                          // [10] = MOTOR TEMP
        KELLY_IZ[11] = DatosDouble[5];                          // [11] = KELLY TEMP
        KELLY_IZ[12] = DatosDouble[6];                          // [12] = Throttle Switch Status IZQUIERDO
        KELLY_IZ[13] = DatosDouble[7];                          // [13] = Reverse Switch Status IZQUIERDO
        MPPT1[0] = DatosDouble[8];                              // [0] = Voltaje IN
        MPPT1[1] = DatosDouble[9];                              // [1] = Amperaje IN
    }else if(bytes[0] == char(0x45)){                           // BMS
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS[3] = DatosDouble[4];                                // [3] = Pack Open Volt
        BMS[4] = DatosDouble[5];                                // [4] = Pack Abs Current
        BMS[5] = DatosDouble[6];                                // [5] = Maximum Pack Volt
        BMS[6] = DatosDouble[7];                                // [6] = Minimum Pack Volt
        BMS[7] = DatosDouble[8];                                // [7] = High Temp
        BMS[8] = DatosDouble[9];                                // [8] = High Temp Thermistor ID
    }else if(bytes[0] == char(0x46)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS[9] = DatosDouble[4];                                 // [9] = Low Temp
        BMS[10] = DatosDouble[5];                                // [10] = Low Temp Thermistor ID
        BMS[11] = DatosDouble[6];                                // [11] = Average Temp
        BMS[12] = DatosDouble[7];                                // [12] = Internal Temp
        BMS[13] = DatosDouble[8];                                // [13] = Max Volt ID
        BMS[14] = DatosDouble[9];                                // [14] = Min Volt ID
    }else if(bytes[0] == char(0x47)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[0] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[1] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[2] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[3] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[4] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[5] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x48)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[6] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[7] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[8] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[9] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[10] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[11] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x49)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[12] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[13] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[14] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[15] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[16] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[17] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4a)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[18] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[19] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[20] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[21] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[22] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[23] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4b)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[24] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[25] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[26] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[27] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[28] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[29] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4c)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[30] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[31] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[32] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[33] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[34] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[35] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4d)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[36] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[37] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[38] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[39] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[40] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[41] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4e)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[42] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[43] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[44] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[45] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[46] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[47] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x4f)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[48] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[49] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[50] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[51] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[52] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[53] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x50)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_TEMP[54] = DatosDouble[4];                           // [60 Temperaturas]
        BMS_TEMP[55] = DatosDouble[5];                           // [60 Temperaturas]
        BMS_TEMP[56] = DatosDouble[6];                           // [60 Temperaturas]
        BMS_TEMP[57] = DatosDouble[7];                           // [60 Temperaturas]
        BMS_TEMP[58] = DatosDouble[8];                           // [60 Temperaturas]
        BMS_TEMP[59] = DatosDouble[9];                           // [60 Temperaturas]
    }else if(bytes[0] == char(0x51)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_VOLT[0] = DatosDouble[4]*0.001;                     // [30 Voltajes]
        BMS_VOLT[1] = DatosDouble[5]*0.001;                     // [30 Voltajes]
        BMS_VOLT[2] = DatosDouble[6]*0.001;                     // [30 Voltajes]
        BMS_VOLT[3] = DatosDouble[7]*0.001;                     // [30 Voltajes]
        BMS_VOLT[4] = DatosDouble[8]*0.001;                     // [30 Voltajes]
        BMS_VOLT[5] = DatosDouble[9]*0.001;                     // [30 Voltajes]
    }else if(bytes[0] == char(0x52)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_VOLT[6] = DatosDouble[4]*0.001;                     // [30 Voltajes]
        BMS_VOLT[7] = DatosDouble[5]*0.001;                     // [30 Voltajes]
        BMS_VOLT[8] = DatosDouble[6]*0.001;                     // [30 Voltajes]
        BMS_VOLT[9] = DatosDouble[7]*0.001;                     // [30 Voltajes]
        BMS_VOLT[10] = DatosDouble[8]*0.001;                     // [30 Voltajes]
        BMS_VOLT[11] = DatosDouble[9]*0.001;                     // [30 Voltajes]
    }else if(bytes[0] == char(0x53)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_VOLT[12] = DatosDouble[4]*0.001;                     // [30 Voltajes]
        BMS_VOLT[13] = DatosDouble[5]*0.001;                     // [30 Voltajes]
        BMS_VOLT[14] = DatosDouble[6]*0.001;                     // [30 Voltajes]
        BMS_VOLT[15] = DatosDouble[7]*0.001;                     // [30 Voltajes]
        BMS_VOLT[16] = DatosDouble[8]*0.001;                     // [30 Voltajes]
        BMS_VOLT[17] = DatosDouble[9]*0.001;                     // [30 Voltajes]
    }else if(bytes[0] == char(0x54)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_VOLT[18] = DatosDouble[4]*0.001;                     // [30 Voltajes]
        BMS_VOLT[19] = DatosDouble[5]*0.001;                     // [30 Voltajes]
        BMS_VOLT[20] = DatosDouble[6]*0.001;                     // [30 Voltajes]
        BMS_VOLT[21] = DatosDouble[7]*0.001;                     // [30 Voltajes]
        BMS_VOLT[22] = DatosDouble[8]*0.001;                     // [30 Voltajes]
        BMS_VOLT[23] = DatosDouble[9]*0.001;                     // [30 Voltajes]
    }else if(bytes[0] == char(0x55)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        BMS_VOLT[24] = DatosDouble[4]*0.001;                     // [30 Voltajes]
        BMS_VOLT[25] = DatosDouble[5]*0.001;                     // [30 Voltajes]
        BMS_VOLT[26] = DatosDouble[6]*0.001;                     // [30 Voltajes]
        BMS_VOLT[27] = DatosDouble[7]*0.001;                     // [30 Voltajes]
        BMS_VOLT[28] = DatosDouble[8]*0.001;                     // [30 Voltajes]
        BMS_VOLT[29] = DatosDouble[9]*0.001;                     // [30 Voltajes]

    }else if(bytes[0] == char(0x56)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        MPPT1[2] = DatosDouble[4];                              // [2] = Voltaje OUT
        MPPT1[3] = DatosDouble[5];                              // [3] = BVLR
        MPPT1[4] = DatosDouble[6];                              // [4] = OVT
        MPPT1[5] = DatosDouble[7];                              // [5] = NOC
        MPPT1[6] = DatosDouble[8];                              // [6] = UNDV
        MPPT1[7] = DatosDouble[9];                              // [7] = TEMP
    }else if(bytes[0] == char(0x57)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        MPPT2[0] = DatosDouble[4];                              // [0] = Voltaje IN
        MPPT2[1] = DatosDouble[5];                              // [1] = Amperaje IN
        MPPT2[2] = DatosDouble[6];                              // [2] = Voltaje OUT
        MPPT2[3] = DatosDouble[7];                              // [3] = BVLR
        MPPT2[4] = DatosDouble[8];                              // [4] = OVT
        MPPT2[5] = DatosDouble[9];                              // [5] = NOC
    }else if(bytes[0] == char(0x58)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        MPPT2[6] = DatosDouble[4];                              // [6] = UNDV
        MPPT2[7] = DatosDouble[5];                              // [7] = TEMP
        MPPT3[0] = DatosDouble[6];                              // [0] = Voltaje IN
        MPPT3[1] = DatosDouble[7];                              // [1] = Amperaje IN
        MPPT3[2] = DatosDouble[8];                              // [2] = Voltaje OUT
        MPPT3[3] = DatosDouble[9];                              // [3] = BVLR
    }else if(bytes[0] == char(0x59)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        MPPT3[4] = DatosDouble[4];                              // [4] = OVT
        MPPT3[5] = DatosDouble[5];                              // [5] = NOC
        MPPT3[6] = DatosDouble[6];                              // [6] = UNDV
        MPPT3[7] = DatosDouble[7];                              // [7] = TEMP
        MPPT4[0] = DatosDouble[8];                              // [0] = Voltaje IN
        MPPT4[1] = DatosDouble[9];                              // [1] = Amperaje IN
    }else if(bytes[0] == char(0x5a)){
        speed = DatosDouble[0];                                 // Velocidad
        BMS[0] = DatosDouble[1];                                // [0] = SOC
        BMS[1] = DatosDouble[2];                                // [1] = Pack Current
        BMS[2] = DatosDouble[3];                                // [2] = Pack Inst Volt
        Dialog::ActualizarPotenciaBMS(BMS[2], BMS[1]);          // Voltage | Current

        MPPT4[2] = DatosDouble[4];                              // [2] = Voltaje OUT
        MPPT4[3] = DatosDouble[5];                              // [3] = BVLR
        MPPT4[4] = DatosDouble[6];                              // [4] = OVT
        MPPT4[5] = DatosDouble[7];                              // [5] = NOC
        MPPT4[6] = DatosDouble[8];                              // [6] = UNDV
        MPPT4[7] = DatosDouble[9];                              // [7] = TEMP
    }

    Dialog::Speed();
    Dialog::displayDatos();
    Dialog::SetupProgressbars();
    Dialog::makeplot();
    Dialog::savetoText();
    Auxiliar[0]="";
    Auxiliar[1]="";
    Auxiliar[2]="";
    Auxiliar[3]="";
    Auxiliar[4]="";
    Auxiliar[5]="";
    Auxiliar[6]="";
    Auxiliar[7]="";
    Auxiliar[8]="";
    Auxiliar[9]="";
    serialSelected.clear();
}

/*/////////////// FIN Formato de 41 bytes  ////////////////*/


/*////////////////// Formato de 7 bytes  ///////////////////*/
void Dialog::Update_filter_7(QByteArray bloque){
    if(bloque.length()>=7){
        if((byte_1_es_letra_7(bloque))&&(not(Contiene_letras_7(bloque)))){
            for(int i=0;i<7;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bloque[i];
            }
            qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_display_7(serialSelected);                     //Hacemos display
            serialSelected.clear();                                     //Limpiamos vector de bytes
            bloque.remove(0, 7);                                       //Cortamos lo que ya leímos
            Dialog::Update_filter_7(bloque);                           //Leemos los bytes que quedan

        }else if((byte_1_arrova_byte_2_es_letra_7(bloque))&&(not(Contiene_letras_arrova_7(bloque)))){
            //qDebug() << "ENTRAMOS" << endl;
            for(int i=0;i<7;i++){                                      //Seleccionamos primeros 7 bytes aparte
                serialSelected[i]=bloque[+i];
            }
            //qDebug() << "serialSelected: " << serialSelected << endl;
            Dialog::Update_display_7_volt(serialSelected);                     //Hacemos display
            serialSelected.clear();                                     //Limpiamos vector de bytes
            bloque.remove(0, 7);                                       //Cortamos lo que ya leímos
            Dialog::Update_filter_7(bloque);                           //Leemos los bytes que quedan

        }else{
            bloque.remove(0, 1);                                       //Se mueve el bloque en un byte
            Dialog::Update_filter_7(bloque);
        }
    }else{
        bloque.clear();                                                // Nada más que leer
    }
}

bool Dialog::byte_1_es_letra_7(QByteArray bits){
    bool estado_inicial = false;
    if ((bits[0]==char(0x41))||(bits[0]==char(0x42))||(bits[0]==char(0x43))||(bits[0]==char(0x44))||(bits[0]==char(0x45))||(bits[0]==char(0x46))||(bits[0]==char(0x47))||(bits[0]==char(0x48))||(bits[0]==char(0x49))||(bits[0]==char(0x4a))||(bits[0]==char(0x4b))||(bits[0]==char(0x4c))||(bits[0]==char(0x4d))||(bits[0]==char(0x4e))||(bits[0]==char(0x4f))||(bits[0]==char(0x50))||(bits[0]==char(0x51))||(bits[0]==char(0x52))||(bits[0]==char(0x53))||(bits[0]==char(0x54))||(bits[0]==char(0x55))||(bits[0]==char(0x56))||(bits[0]==char(0x57))||(bits[0]==char(0x58))||(bits[0]==char(0x59))||(bits[0]==char(0x5a))||(bits[0]==char(0x61))||(bits[0]==char(0x62))||(bits[0]==char(0x63))||(bits[0]==char(0x64))||(bits[0]==char(0x65))||(bits[0]==char(0x66))||(bits[0]==char(0x67))||(bits[0]==char(0x68))||(bits[0]==char(0x69))||(bits[0]==char(0x6a))||(bits[0]==char(0x6b))||(bits[0]==char(0x6c))||(bits[0]==char(0x6d))||(bits[0]==char(0x6e))||(bits[0]==char(0x6f))||(bits[0]==char(0x70))||(bits[0]==char(0x71))||(bits[0]==char(0x72))||(bits[0]==char(0x73))||(bits[0]==char(0x74))||(bits[0]==char(0x75))||(bits[0]==char(0x76))||(bits[0]==char(0x77))||(bits[0]==char(0x78))||(bits[0]==char(0x79))||(bits[0]==char(0x7a))||(bits[0]==char(0x7b))||(bits[0]==char(0x7c))||(bits[0]==char(0x7d))||(bits[0]==char(0x7e))){
        estado_inicial = true;
        //qDebug() << "Es: " << bits[0] << endl;
    }

    //if (bits[0]==char(0x6c)){
      //  qDebug() << "ES l" << endl;
    //}

    return estado_inicial;
}
bool Dialog::Contiene_letras_7(QByteArray bloque){
    bool estado_inicial = false;
    if(bloque.length()>=7){
        for(int a=1;a<7;a++){
            if((bloque[a]==char(0x40))||(bloque[a]==char(0x41))||(bloque[a]==char(0x42))||(bloque[a]==char(0x43))||(bloque[a]==char(0x44))||(bloque[a]==char(0x45))||(bloque[a]==char(0x46))||(bloque[a]==char(0x47))||(bloque[a]==char(0x48))||(bloque[a]==char(0x49))||(bloque[a]==char(0x4a))||(bloque[a]==char(0x4b))||(bloque[a]==char(0x4c))||(bloque[a]==char(0x4d))||(bloque[a]==char(0x4e))||(bloque[a]==char(0x4f))||(bloque[a]==char(0x50))||(bloque[a]==char(0x51))||(bloque[a]==char(0x52))||(bloque[a]==char(0x53))||(bloque[a]==char(0x54))||(bloque[a]==char(0x55))||(bloque[a]==char(0x56))||(bloque[a]==char(0x57))||(bloque[a]==char(0x58))||(bloque[a]==char(0x59))||(bloque[a]==char(0x5a))||(bloque[a]==char(0x61))||(bloque[a]==char(0x62))||(bloque[a]==char(0x63))||(bloque[a]==char(0x64))||(bloque[a]==char(0x65))||(bloque[a]==char(0x66))||(bloque[a]==char(0x67))||(bloque[a]==char(0x68))||(bloque[a]==char(0x69))||(bloque[a]==char(0x6a))||(bloque[a]==char(0x6b))||(bloque[a]==char(0x6c))||(bloque[a]==char(0x6d))||(bloque[a]==char(0x6e))||(bloque[a]==char(0x6f))||(bloque[a]==char(0x70))||(bloque[a]==char(0x71))||(bloque[a]==char(0x72))||(bloque[a]==char(0x73))||(bloque[a]==char(0x74))||(bloque[a]==char(0x75))||(bloque[a]==char(0x76))||(bloque[a]==char(0x77))||(bloque[a]==char(0x78))||(bloque[a]==char(0x79))||(bloque[a]==char(0x7a))||(bloque[a]==char(0x7b))||(bloque[a]==char(0x7c))||(bloque[a]==char(0x7d))||(bloque[a]==char(0x7e))){
            estado_inicial = true;
            }
        }
    }
    return estado_inicial;
}

bool Dialog::byte_1_arrova_byte_2_es_letra_7(QByteArray bits){
    bool estado_inicial = false;
    if ((bits[0]==char(0x40))&&((bits[1]==char(0x41))||(bits[1]==char(0x42))||(bits[1]==char(0x43))||(bits[1]==char(0x44))||(bits[1]==char(0x45))||(bits[1]==char(0x46))||(bits[1]==char(0x47))||(bits[1]==char(0x48))||(bits[1]==char(0x49))||(bits[1]==char(0x4a))||(bits[1]==char(0x4b))||(bits[1]==char(0x4c))||(bits[1]==char(0x4d))||(bits[1]==char(0x4e))||(bits[1]==char(0x4f))||(bits[1]==char(0x50))||(bits[1]==char(0x51))||(bits[1]==char(0x52))||(bits[1]==char(0x53))||(bits[1]==char(0x54))||(bits[1]==char(0x55))||(bits[1]==char(0x56))||(bits[1]==char(0x57))||(bits[1]==char(0x58))||(bits[1]==char(0x59))||(bits[1]==char(0x5a))||(bits[1]==char(0x61))||(bits[1]==char(0x62))||(bits[1]==char(0x63))||(bits[1]==char(0x64))||(bits[1]==char(0x65))||(bits[1]==char(0x66))||(bits[1]==char(0x67))||(bits[1]==char(0x68))||(bits[1]==char(0x69))||(bits[1]==char(0x6a))||(bits[1]==char(0x6b)))){
        estado_inicial = true;
    }
    return estado_inicial;
}
bool Dialog::Contiene_letras_arrova_7(QByteArray bloque){
    bool estado_inicial = false;
    if(bloque.length()>=7){
        for(int a=2;a<7;a++){
            if((bloque[a]==char(0x41))||(bloque[a]==char(0x42))||(bloque[a]==char(0x43))||(bloque[a]==char(0x44))||(bloque[a]==char(0x45))||(bloque[a]==char(0x46))||(bloque[a]==char(0x47))||(bloque[a]==char(0x48))||(bloque[a]==char(0x49))||(bloque[a]==char(0x4a))||(bloque[a]==char(0x4b))||(bloque[a]==char(0x4c))||(bloque[a]==char(0x4d))||(bloque[a]==char(0x4e))||(bloque[a]==char(0x4f))||(bloque[a]==char(0x50))||(bloque[a]==char(0x51))||(bloque[a]==char(0x52))||(bloque[a]==char(0x53))||(bloque[a]==char(0x54))||(bloque[a]==char(0x55))||(bloque[a]==char(0x56))||(bloque[a]==char(0x57))||(bloque[a]==char(0x58))||(bloque[a]==char(0x59))||(bloque[a]==char(0x5a))||(bloque[a]==char(0x61))||(bloque[a]==char(0x62))||(bloque[a]==char(0x63))||(bloque[a]==char(0x64))||(bloque[a]==char(0x65))||(bloque[a]==char(0x66))||(bloque[a]==char(0x67))||(bloque[a]==char(0x68))||(bloque[a]==char(0x69))||(bloque[a]==char(0x6a))||(bloque[a]==char(0x6b))){
            estado_inicial = true;
            }
        }
    }
    return estado_inicial;
}


/*/////////////// FIN Formato de 7 bytes  ////////////////*/



void Dialog::savetoText(){
    QDate tiempo = QDate::currentDate();
    QString fecha_prueba = QString::number(tiempo.day())+"."+QString::number(tiempo.month())+"."+QString::number(tiempo.year());
    //qDebug() << fecha_prueba << endl;
    QFile data_text("Prueba_CARRETERA"+fecha_prueba+".txt");
    data_text.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append );
    QTextStream stream(&data_text);
        stream << QTime::currentTime().toString() << "," << speed << ",";

                  stream << "BMS"<< ",";
                  for(int i=0;i<15;i++){
                  stream <<BMS[i] << ",";
                  }
                  stream << "VOLT"<< ",";
                  for(int i=0;i<30;i++){
                  stream <<BMS_VOLT[i] << ",";
                  }
                  stream << "TEMP"<< ",";
                  for(int i=0;i<60;i++){
                  stream <<BMS_TEMP[i] << ",";
                  }
                  stream << "KELLY_DER"<< ",";
                  for(int i=0;i<14;i++){
                  stream <<KELLY_DER[i] << ",";
                  }
                  stream << "KELLY_IZ"<< ",";
                  for(int i=0;i<14;i++){
                  stream <<KELLY_IZ[i] << ",";
                  }
                  stream << "MPPT1"<< ",";
                  for(int i=0;i<8;i++){
                  stream <<MPPT1[i] << ",";
                  }
                  stream << "MPPT2"<< ",";
                  for(int i=0;i<8;i++){
                  stream <<MPPT2[i] << ",";
                  }
                  stream << "MPPT3"<< ",";
                  for(int i=0;i<8;i++){
                  stream <<MPPT3[i] << ",";
                  }
                  stream << "MPPT4"<< ",";
                  for(int i=0;i<8;i++){
                  stream <<MPPT4[i] << ",";
                  }
                  stream <<"END"<<"\n";
}
void Dialog::RightSpeed(double incoming_speed){
    if ((-30>incoming_speed-last_speed)||(incoming_speed-last_speed>30)){
        speed=last_speed;
    }else{
        speed=incoming_speed;
        last_speed=speed;
    }
}
void Dialog::displayDatos(){
    //////////////////////////////////////// BMS /////////////////////////////////////////
    ui->lcd_packAmp->display(BMS[1]*0.1);                // [1] = Pack Current
    ui->lcd_packVolt->display(BMS[2]);               // [2] = Pack Inst Volt
    ui->lcd_openVolt->display(BMS[3]);               // [3] = Pack Open Volt
    ui->lcd_absCurrent->display(BMS[4]);             // [4] = Pack Abs Current
    ui->lcd_maxPackVolt->display(BMS[5]);            // [5] = Maximum Pack Volt
    ui->lcd_minPackVolt->display(BMS[6]);            // [6] = Minimum Pack Volt
    ui->lcd_highTemp->display(BMS[7]);               // [7] = High Temp
    ui->lcd_maxTermID->display(BMS[8]);              // [8] = High Temp Thermistor ID
    ui->lcd_lowTemp->display(BMS[9]);                // [9] = Low Temp
    ui->lcd_lowTermID->display(BMS[10]);             // [10] = Low Temp Thermistor ID
    ui->lcd_AvgTemp->display(BMS[11]);               // [11] = Average Temp
    ui->lcd_internalTemperature->display(BMS[12]);   // [12] = Internal Temp
    ui->lcd_maxPackVolt_id->display(BMS[13]);        // [13] = Max Volt ID
    ui->lcd_minPackVolt_id->display(BMS[14]);        // [14] = Min Volt ID

    /////////////////////////////////////// KELLY ////////////////////////////////////////
    ui->lcd_KellyD_IA->display(KELLY_DER[0]);           // [0] Ia
    ui->lcd_KellyD_IB->display(KELLY_DER[1]);           // [1] Ib
    ui->lcd_KellyD_IC->display(KELLY_DER[2]);           // [2] Ic
    ui->lcd_KellyD_VA->display(KELLY_DER[3]);           // [3] Va
    ui->lcd_KellyD_VB->display(KELLY_DER[4]);           // [4] Vb
    ui->lcd_KellyD_VC->display(KELLY_DER[5]);           // [5] Vc
    ui->lcd_rpm_d->display(KELLY_DER[6]);               // [6] = RPM
    ui->lcd_errorcode1->display(KELLY_DER[7]);          // [7] = ERROR CODE
    ui->lcd_motor_d_pwm->display(KELLY_DER[8]);         // [8] = PWM
    ui->lcd_motor_d_emr->display(KELLY_DER[9]);         // [9] = EMR
    ui->lcd_motor_d_temp->display(KELLY_DER[10]);       // [10] = MOTOR TEMP
    ui->lcd_motor_d_kelly_temp->display(KELLY_DER[11]); // [11] = KELLY TEMP
    ui->lcd_motor_d_throttle->display(KELLY_DER[12]);   // [12] = Throttle Switch Status DERECHO
    ui->lcd_motor_d_reverse->display(KELLY_DER[13]);    // [13] = Reverse Switch Status DERECHO

    ui->lcd_KellyI_IA->display(KELLY_IZ[0]);            // [0] Ia
    ui->lcd_KellyI_IB->display(KELLY_IZ[1]);            // [1] Ib
    ui->lcd_KellyI_IC->display(KELLY_IZ[2]);            // [2] Ic
    ui->lcd_KellyI_VA->display(KELLY_IZ[3]);            // [3] Va
    ui->lcd_KellyI_VB->display(KELLY_IZ[4]);            // [4] Vb
    ui->lcd_KellyI_VC->display(KELLY_IZ[5]);            // [5] Vc
    ui->lcd_rpm_i->display(KELLY_IZ[6]);                // [6] = RPM
    ui->lcd_errorcode2->display(KELLY_IZ[7]);           // [7] = ERROR CODE
    ui->lcd_motor_i_pwm->display(KELLY_IZ[8]);          // [8] = PWM
    ui->lcd_motor_i_emr->display(KELLY_IZ[9]);          // [9] = EMR
    ui->lcd_motor_i_temp->display(KELLY_IZ[10]);        // [10] = MOTOR TEMP
    ui->lcd_motor_i_kelly_temp->display(KELLY_IZ[11]);  // [11] = KELLY TEMP
    ui->lcd_motor_i_throttle->display(KELLY_IZ[12]);    // [12] = Throttle Switch Status DERECHO
    ui->lcd_motor_i_reverse->display(KELLY_IZ[13]);     // [13] = Reverse Switch Status DERECHO

    /////////////////////////////////////// MPPTS ////////////////////////////////////////
    ui->lcd_mppt1_vin->display(MPPT1[0]);               // [0] = Voltaje IN
    ui->lcd_mppt1_iin->display(MPPT1[1]);               // [1] = Amperaje IN
    ui->lcd_mppt1_vout->display(MPPT1[2]);              // [2] = Voltaje OUT
    ui->lcd_mppt1_bvlr->display(MPPT1[3]);              // [3] = BVLR
    ui->lcd_mppt1_ovt->display(MPPT1[4]);               // [4] = OVT
    ui->lcd_mppt1_noc->display(MPPT1[5]);               // [5] = NOC
    ui->lcd_mppt1_undv->display(MPPT1[6]);              // [6] = UNDV
    ui->lcd_mppt1_temp->display(MPPT1[7]);              // [7] = TEMP

    ui->lcd_mppt2_vin->display(MPPT2[0]);               // [0] = Voltaje IN
    ui->lcd_mppt2_iin->display(MPPT2[1]);               // [1] = Amperaje IN
    ui->lcd_mppt2_vout->display(MPPT2[2]);              // [2] = Voltaje OUT
    ui->lcd_mppt2_bvlr->display(MPPT2[3]);              // [3] = BVLR
    ui->lcd_mppt2_ovt->display(MPPT2[4]);               // [4] = OVT
    ui->lcd_mppt2_noc->display(MPPT2[5]);               // [5] = NOC
    ui->lcd_mppt2_undv->display(MPPT2[6]);              // [6] = UNDV
    ui->lcd_mppt2_temp->display(MPPT2[7]);              // [7] = TEMP

    ui->lcd_mppt3_vin->display(MPPT3[0]);               // [0] = Voltaje IN
    ui->lcd_mppt3_iin->display(MPPT3[1]);               // [1] = Amperaje IN
    ui->lcd_mppt3_vout->display(MPPT3[2]);              // [2] = Voltaje OUT
    ui->lcd_mppt3_bvlr->display(MPPT3[3]);              // [3] = BVLR
    ui->lcd_mppt3_ovt->display(MPPT3[4]);               // [4] = OVT
    ui->lcd_mppt3_noc->display(MPPT3[5]);               // [5] = NOC
    ui->lcd_mppt3_undv->display(MPPT3[6]);              // [6] = UNDV
    ui->lcd_mppt3_temp->display(MPPT3[7]);              // [7] = TEMP


    ui->lcd_mppt4_vin->display(MPPT4[0]);               // [0] = Voltaje IN
    ui->lcd_mppt4_iin->display(MPPT4[1]);               // [1] = Amperaje IN
    ui->lcd_mppt4_vout->display(MPPT4[2]);              // [2] = Voltaje OUT
    ui->lcd_mppt4_bvlr->display(MPPT4[3]);              // [3] = BVLR
    ui->lcd_mppt4_ovt->display(MPPT4[4]);               // [4] = OVT
    ui->lcd_mppt4_noc->display(MPPT4[5]);               // [5] = NOC
    ui->lcd_mppt4_undv->display(MPPT4[6]);              // [6] = UNDV
    ui->lcd_mppt4_temp->display(MPPT4[7]);              // [7] = TEMP

}
void Dialog::makeplot()
{
    // calculate two new data points:
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double lastPointKey = 0;

    if (key-lastPointKey > 0.01) // at most add point every 100 ms
    {
      // add data to lines:
      ui->Plot1->graph(0)->addData(key, potencia_total_mppts);
      ui->Plot1->graph(1)->addData(key, potencia_total_bms);
      ui->Plot1->graph(2)->addData(key, potencia_consumida);
      ui->Plot1->graph(3)->addData(key, speed*0.1);

      // set data of dots:
      ui->Plot1->graph(4)->clearData();
      ui->Plot1->graph(4)->addData(key, potencia_total_kellys);

      ui->Plot1->graph(5)->clearData();
      ui->Plot1->graph(5)->addData(key, potencia_total_bms);

      ui->Plot1->graph(6)->clearData();
      ui->Plot1->graph(6)->addData(key, potencia_total_mppts);

      ui->Plot1->graph(7)->clearData();
      ui->Plot1->graph(7)->addData(key, speed*0.1);

      // remove data of lines that's outside visible range:
      ui->Plot1->graph(0)->removeDataBefore(key-100);
      ui->Plot1->graph(1)->removeDataBefore(key-100);
      ui->Plot1->graph(2)->removeDataBefore(key-100);
      ui->Plot1->graph(3)->removeDataBefore(key-100);

      // rescale value (vertical) axis to fit the current data:
      ui->Plot1->graph(0)->rescaleValueAxis(true);
      ui->Plot1->graph(1)->rescaleValueAxis(true);
      ui->Plot1->graph(2)->rescaleValueAxis(true);
      ui->Plot1->graph(3)->rescaleValueAxis(true);

      // add data to lines:
      ui->Plot2->graph(0)->addData(key, KELLY_DER[10]); // TEMP MOTOR DERECHO
      ui->Plot2->graph(1)->addData(key, KELLY_IZ[10]);  // TEMP MOTOR IZQUIERDO
      ui->Plot2->graph(2)->addData(key, BMS[11]);

      // set data of dots:
      ui->Plot2->graph(3)->clearData();
      ui->Plot2->graph(3)->addData(key, KELLY_DER[10]); // TEMP MOTOR DERECHO

      ui->Plot2->graph(4)->clearData();
      ui->Plot2->graph(4)->addData(key, KELLY_IZ[10]);  // TEMP MOTOR IZQUIERDO

      ui->Plot2->graph(5)->clearData();
      ui->Plot2->graph(5)->addData(key, BMS[11]);

      // remove data of lines that's outside visible range:
      ui->Plot2->graph(0)->removeDataBefore(key-100);
      ui->Plot2->graph(1)->removeDataBefore(key-100);
      ui->Plot2->graph(2)->removeDataBefore(key-100);

      // rescale value (vertical) axis to fit the current data:
      ui->Plot2->graph(0)->rescaleValueAxis(true);
      ui->Plot2->graph(1)->rescaleValueAxis(true);
      ui->Plot2->graph(2)->rescaleValueAxis(true);

      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->Plot1->xAxis->setRange(key+0.1, 100, Qt::AlignRight);
    ui->Plot1->yAxis->setRange(0,15);
    ui->Plot1->replot();

    ui->Plot2->xAxis->setRange(key+0.25, 100, Qt::AlignRight);
    ui->Plot2->yAxis->setRange(0,100);
    ui->Plot2->replot();

}
void Dialog::SetupPlots(){
//    ui->Plot1->graph(0)->setChannelFillGraph(ui->Plot1->graph(1));
//    ui->Plot1->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));

    //Grafico potencias
    ui->Plot1->addGraph();  // blue line
    ui->Plot1->graph(0)->setPen(QPen(Qt::green));
    ui->Plot1->graph(0)->setAntialiasedFill(true);

    ui->Plot1->addGraph(); // red line
    ui->Plot1->graph(1)->setPen(QPen(Qt::blue));
    ui->Plot1->graph(1)->setAntialiasedFill(true);


    ui->Plot1->addGraph(); // green line
    ui->Plot1->graph(2)->setPen(QPen(Qt::red));
    ui->Plot1->graph(2)->setAntialiasedFill(true);

    ui->Plot1->addGraph(); // black line
    ui->Plot1->graph(3)->setPen(QPen(Qt::black));
    ui->Plot1->graph(3)->setAntialiasedFill(true);

    ui->Plot1->addGraph(); // blue dot
    ui->Plot1->graph(4)->setPen(QPen(Qt::green));
    ui->Plot1->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->Plot1->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot1->addGraph(); // red dot
    ui->Plot1->graph(5)->setPen(QPen(Qt::blue));
    ui->Plot1->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->Plot1->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot1->addGraph(); // green dot
    ui->Plot1->graph(6)->setPen(QPen(Qt::red));
    ui->Plot1->graph(6)->setLineStyle(QCPGraph::lsNone);
    ui->Plot1->graph(6)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot1->addGraph(); // black dot
    ui->Plot1->graph(7)->setPen(QPen(Qt::black));
    ui->Plot1->graph(7)->setLineStyle(QCPGraph::lsNone);
    ui->Plot1->graph(7)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot1->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->Plot1->xAxis->setDateTimeFormat("");
    ui->Plot1->xAxis->setAutoTickStep(false);
    ui->Plot1->xAxis->setTickStep(2);
    ui->Plot1->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->Plot1->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Plot1->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Plot1->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Plot1->yAxis2, SLOT(setRange(QCPRange)));

    //Grafico temperaturas
    ui->Plot2->addGraph();  // blue line
    ui->Plot2->graph(0)->setPen(QPen(Qt::blue));
    ui->Plot2->graph(0)->setAntialiasedFill(true);

    ui->Plot2->addGraph(); // red line
    ui->Plot2->graph(1)->setPen(QPen(Qt::red));
    ui->Plot2->graph(1)->setAntialiasedFill(true);


    ui->Plot2->addGraph(); // green line
    ui->Plot2->graph(2)->setPen(QPen(Qt::green));
    ui->Plot2->graph(2)->setAntialiasedFill(true);

    ui->Plot2->addGraph(); // blue dot
    ui->Plot2->graph(3)->setPen(QPen(Qt::blue));
    ui->Plot2->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->Plot2->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot2->addGraph(); // red dot
    ui->Plot2->graph(4)->setPen(QPen(Qt::red));
    ui->Plot2->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->Plot2->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot2->addGraph(); // green dot
    ui->Plot2->graph(5)->setPen(QPen(Qt::green));
    ui->Plot2->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->Plot2->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->Plot2->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->Plot2->xAxis->setDateTimeFormat("");
    ui->Plot2->xAxis->setAutoTickStep(false);
    ui->Plot2->xAxis->setTickStep(2);
    ui->Plot2->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->Plot2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Plot2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Plot2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Plot2->yAxis2, SLOT(setRange(QCPRange)));

}
void Dialog::Speed(){
/*    if ((-30>speed-last_speed)||(speed-last_speed>30)){
        speed=last_speed;
    }else{
        last_speed=speed;
    }*/
    mSpeedNeedle->setCurrentValue(speed);
    ui->lcd_speed->display(speed);
    if(speed>max_speed){
    max_speed=speed;
    }
}
QString Dialog::barTempStyle(double value){
    if ((value >=10)&&(value <32)){
        return fondo_verde_i;
    }else if ((value >=32)&&(value <38)){
        return fondo_amarillo_i;
    }else if ((value >=38)&&(value <40)){
        return fondo_naranjo_i;
    }else if ((value >=40)&&(value <=43)){
        return fondo_narojo_i;
    }else if ((value >=43)&&(value <=45)){
        return fondo_rojo_i;
    }else{
    return fondo_verde_i;
    }
}
QString Dialog::barTempStyle1(double value){
    if ((value >=10)&&(value <32)){
        return fondo_verde_d;
    }else if ((value >=32)&&(value <38)){
        return fondo_amarillo_d;
    }else if ((value >=38)&&(value <40)){
        return fondo_naranjo_d;
    }else if ((value >=40)&&(value <=43)){
        return fondo_narojo_d;
    }else if ((value >=43)&&(value <=45)){
        return fondo_rojo_d;
    }else{
    return fondo_verde_d;
    }
}
QString Dialog::barVoltStyle(double value){
    if ((value <= 2.8) || (value >= 4.4)){
        return fondo_rojo_v;
    }else if ((value >2.8)&&(value <= 3.25)){
        return fondo_narojo_v;
    }else if ((value >3.2)&&(value <= 3.65)){
        return fondo_naranjo_v;
    }else if ((value >3.65)&&(value <= 4.05)){
        return fondo_amarillo_v;
    }else if ((value >4.05)&&(value < 4.4)){
        return fondo_verde_v;
    }else{
        return fondo_verde_v;
    }
}
QString Dialog::barSoCStyle(double value){
    if ((value >=0)&&(value <25)){
     return danger_2_soc;
    }else if ((value >=25)&&(value <50)){
     return danger_1_soc;
    }else if ((value >=50)&&(value <75)){
     return safe_2_soc;
    }else if((value >=75)&&(value <=100)){
     return safe_1_soc;
    }
    return safe_1_soc;
}
void Dialog::SetupProgressbars(){

//Asociar valores

    // Soc Bar
    ui->SoC_bar->setValue(BMS[0]);      //Set Value por ser ProgressBar

    //Max Vel historica
    ui->lcd_max_speed->display(max_speed);

    //Temp Bar
    ui->temp_bar_01->setText(QString::number(BMS_TEMP[0], 'g', 4));
    ui->temp_bar_02->setText(QString::number(BMS_TEMP[1], 'g', 4));
    ui->temp_bar_03->setText(QString::number(BMS_TEMP[2], 'g', 4));
    ui->temp_bar_04->setText(QString::number(BMS_TEMP[3], 'g', 4));
    ui->temp_bar_05->setText(QString::number(BMS_TEMP[4], 'g', 4));
    ui->temp_bar_06->setText(QString::number(BMS_TEMP[5], 'g', 4));
    ui->temp_bar_07->setText(QString::number(BMS_TEMP[6], 'g', 4));
    ui->temp_bar_08->setText(QString::number(BMS_TEMP[7], 'g', 4));
    ui->temp_bar_09->setText(QString::number(BMS_TEMP[8], 'g', 4));
    ui->temp_bar_10->setText(QString::number(BMS_TEMP[9], 'g', 4));

    ui->temp_bar_11->setText(QString::number(BMS_TEMP[10], 'g', 4));
    ui->temp_bar_12->setText(QString::number(BMS_TEMP[11], 'g', 4));
    ui->temp_bar_13->setText(QString::number(BMS_TEMP[12], 'g', 4));
    ui->temp_bar_14->setText(QString::number(BMS_TEMP[13], 'g', 4));
    ui->temp_bar_15->setText(QString::number(BMS_TEMP[14], 'g', 4));
    ui->temp_bar_16->setText(QString::number(BMS_TEMP[15], 'g', 4));
    ui->temp_bar_17->setText(QString::number(BMS_TEMP[16], 'g', 4));
    ui->temp_bar_18->setText(QString::number(BMS_TEMP[17], 'g', 4));
    ui->temp_bar_19->setText(QString::number(BMS_TEMP[18], 'g', 4));
    ui->temp_bar_20->setText(QString::number(BMS_TEMP[19], 'g', 4));

    ui->temp_bar_21->setText(QString::number(BMS_TEMP[20], 'g', 4));
    ui->temp_bar_22->setText(QString::number(BMS_TEMP[21], 'g', 4));
    ui->temp_bar_23->setText(QString::number(BMS_TEMP[22], 'g', 4));
    ui->temp_bar_24->setText(QString::number(BMS_TEMP[23], 'g', 4));
    ui->temp_bar_25->setText(QString::number(BMS_TEMP[24], 'g', 4));
    ui->temp_bar_26->setText(QString::number(BMS_TEMP[25], 'g', 4));
    ui->temp_bar_27->setText(QString::number(BMS_TEMP[26], 'g', 4));
    ui->temp_bar_28->setText(QString::number(BMS_TEMP[27], 'g', 4));
    ui->temp_bar_29->setText(QString::number(BMS_TEMP[28], 'g', 4));
    ui->temp_bar_30->setText(QString::number(BMS_TEMP[29], 'g', 4));

    ui->temp_bar_31->setText(QString::number(BMS_TEMP[30], 'g', 4));
    ui->temp_bar_32->setText(QString::number(BMS_TEMP[31], 'g', 4));
    ui->temp_bar_33->setText(QString::number(BMS_TEMP[32], 'g', 4));
    ui->temp_bar_34->setText(QString::number(BMS_TEMP[33], 'g', 4));
    ui->temp_bar_35->setText(QString::number(BMS_TEMP[34], 'g', 4));
    ui->temp_bar_36->setText(QString::number(BMS_TEMP[35], 'g', 4));
    ui->temp_bar_37->setText(QString::number(BMS_TEMP[36], 'g', 4));
    ui->temp_bar_38->setText(QString::number(BMS_TEMP[37], 'g', 4));
    ui->temp_bar_39->setText(QString::number(BMS_TEMP[38], 'g', 4));
    ui->temp_bar_40->setText(QString::number(BMS_TEMP[39], 'g', 4));

    ui->temp_bar_41->setText(QString::number(BMS_TEMP[40], 'g', 4));
    ui->temp_bar_42->setText(QString::number(BMS_TEMP[41], 'g', 4));
    ui->temp_bar_43->setText(QString::number(BMS_TEMP[42], 'g', 4));
    ui->temp_bar_44->setText(QString::number(BMS_TEMP[43], 'g', 4));
    ui->temp_bar_45->setText(QString::number(BMS_TEMP[44], 'g', 4));
    ui->temp_bar_46->setText(QString::number(BMS_TEMP[45], 'g', 4));
    ui->temp_bar_47->setText(QString::number(BMS_TEMP[46], 'g', 4));
    ui->temp_bar_48->setText(QString::number(BMS_TEMP[47], 'g', 4));
    ui->temp_bar_49->setText(QString::number(BMS_TEMP[48], 'g', 4));
    ui->temp_bar_50->setText(QString::number(BMS_TEMP[49], 'g', 4));

    ui->temp_bar_51->setText(QString::number(BMS_TEMP[50], 'g', 4));
    ui->temp_bar_52->setText(QString::number(BMS_TEMP[51], 'g', 4));
    ui->temp_bar_53->setText(QString::number(BMS_TEMP[52], 'g', 4));
    ui->temp_bar_54->setText(QString::number(BMS_TEMP[53], 'g', 4));
    ui->temp_bar_55->setText(QString::number(BMS_TEMP[54], 'g', 4));
    ui->temp_bar_56->setText(QString::number(BMS_TEMP[55], 'g', 4));
    ui->temp_bar_57->setText(QString::number(BMS_TEMP[56], 'g', 4));
    ui->temp_bar_58->setText(QString::number(BMS_TEMP[57], 'g', 4));
    ui->temp_bar_59->setText(QString::number(BMS_TEMP[58], 'g', 4));
    ui->temp_bar_60->setText(QString::number(BMS_TEMP[59], 'g', 4));

    //Volt Bars

    ui->label_Volt_01->setText(QString::number(BMS_VOLT[0], 'g', 4));
    ui->label_Volt_02->setText(QString::number(BMS_VOLT[1], 'g', 4));
    ui->label_Volt_03->setText(QString::number(BMS_VOLT[2], 'g', 4));
    ui->label_Volt_04->setText(QString::number(BMS_VOLT[3], 'g', 4));
    ui->label_Volt_05->setText(QString::number(BMS_VOLT[4], 'g', 4));
    ui->label_Volt_06->setText(QString::number(BMS_VOLT[5], 'g', 4));
    ui->label_Volt_07->setText(QString::number(BMS_VOLT[6], 'g', 4));
    ui->label_Volt_08->setText(QString::number(BMS_VOLT[7], 'g', 4));
    ui->label_Volt_09->setText(QString::number(BMS_VOLT[8], 'g', 4));
    ui->label_Volt_10->setText(QString::number(BMS_VOLT[9], 'g', 4));

    ui->label_Volt_11->setText(QString::number(BMS_VOLT[10], 'g', 4));
    ui->label_Volt_12->setText(QString::number(BMS_VOLT[11], 'g', 4));
    ui->label_Volt_13->setText(QString::number(BMS_VOLT[12], 'g', 4));
    ui->label_Volt_14->setText(QString::number(BMS_VOLT[13], 'g', 4));
    ui->label_Volt_15->setText(QString::number(BMS_VOLT[14], 'g', 4));
    ui->label_Volt_16->setText(QString::number(BMS_VOLT[15], 'g', 4));
    ui->label_Volt_17->setText(QString::number(BMS_VOLT[16], 'g', 4));
    ui->label_Volt_18->setText(QString::number(BMS_VOLT[17], 'g', 4));
    ui->label_Volt_19->setText(QString::number(BMS_VOLT[18], 'g', 4));
    ui->label_Volt_20->setText(QString::number(BMS_VOLT[19], 'g', 4));

    ui->label_Volt_21->setText(QString::number(BMS_VOLT[20], 'g', 4));
    ui->label_Volt_22->setText(QString::number(BMS_VOLT[21], 'g', 4));
    ui->label_Volt_23->setText(QString::number(BMS_VOLT[22], 'g', 4));
    ui->label_Volt_24->setText(QString::number(BMS_VOLT[23], 'g', 4));
    ui->label_Volt_25->setText(QString::number(BMS_VOLT[24], 'g', 4));
    ui->label_Volt_26->setText(QString::number(BMS_VOLT[25], 'g', 4));
    ui->label_Volt_27->setText(QString::number(BMS_VOLT[26], 'g', 4));
    ui->label_Volt_28->setText(QString::number(BMS_VOLT[27], 'g', 4));
    ui->label_Volt_29->setText(QString::number(BMS_VOLT[28], 'g', 4));
    ui->label_Volt_30->setText(QString::number(BMS_VOLT[29], 'g', 4));

//STYLESHEET

    //SOC BAR
    ui->SoC_bar->setStyleSheet(Dialog::barSoCStyle(BMS[0]));

    //Temp bars
    ui->temp_bar_01->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[0]));
    ui->temp_bar_02->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[1]));
    ui->temp_bar_03->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[2]));
    ui->temp_bar_04->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[3]));
    ui->temp_bar_05->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[4]));
    ui->temp_bar_06->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[5]));
    ui->temp_bar_07->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[6]));
    ui->temp_bar_08->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[7]));
    ui->temp_bar_09->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[8]));
    ui->temp_bar_10->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[9]));
    ui->temp_bar_11->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[10]));
    ui->temp_bar_12->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[11]));
    ui->temp_bar_13->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[12]));
    ui->temp_bar_14->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[13]));
    ui->temp_bar_15->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[14]));
    ui->temp_bar_16->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[15]));
    ui->temp_bar_17->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[16]));
    ui->temp_bar_18->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[17]));
    ui->temp_bar_19->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[18]));
    ui->temp_bar_20->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[19]));
    ui->temp_bar_21->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[20]));
    ui->temp_bar_22->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[21]));
    ui->temp_bar_23->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[22]));
    ui->temp_bar_24->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[23]));
    ui->temp_bar_25->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[24]));
    ui->temp_bar_26->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[25]));
    ui->temp_bar_27->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[26]));
    ui->temp_bar_28->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[27]));
    ui->temp_bar_29->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[28]));
    ui->temp_bar_30->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[29]));
    ui->temp_bar_31->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[30]));
    ui->temp_bar_32->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[31]));
    ui->temp_bar_33->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[32]));
    ui->temp_bar_34->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[33]));
    ui->temp_bar_35->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[34]));
    ui->temp_bar_36->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[35]));
    ui->temp_bar_37->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[36]));
    ui->temp_bar_38->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[37]));
    ui->temp_bar_39->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[38]));
    ui->temp_bar_40->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[39]));
    ui->temp_bar_41->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[40]));
    ui->temp_bar_42->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[41]));
    ui->temp_bar_43->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[42]));
    ui->temp_bar_44->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[43]));
    ui->temp_bar_45->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[44]));
    ui->temp_bar_46->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[45]));
    ui->temp_bar_47->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[46]));
    ui->temp_bar_48->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[47]));
    ui->temp_bar_49->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[48]));
    ui->temp_bar_50->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[49]));
    ui->temp_bar_51->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[50]));
    ui->temp_bar_52->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[51]));
    ui->temp_bar_53->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[52]));
    ui->temp_bar_54->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[53]));
    ui->temp_bar_55->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[54]));
    ui->temp_bar_56->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[55]));
    ui->temp_bar_57->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[56]));
    ui->temp_bar_58->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[57]));
    ui->temp_bar_59->setStyleSheet(Dialog::barTempStyle(BMS_TEMP[58]));
    ui->temp_bar_60->setStyleSheet(Dialog::barTempStyle1(BMS_TEMP[59]));

    //Volt bars
    ui->label_Volt_01->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[0]));
    ui->label_Volt_02->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[1]));
    ui->label_Volt_03->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[2]));
    ui->label_Volt_04->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[3]));
    ui->label_Volt_05->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[4]));
    ui->label_Volt_06->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[5]));
    ui->label_Volt_07->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[6]));
    ui->label_Volt_08->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[7]));
    ui->label_Volt_09->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[8]));
    ui->label_Volt_10->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[9]));
    ui->label_Volt_11->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[10]));
    ui->label_Volt_12->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[11]));
    ui->label_Volt_13->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[12]));
    ui->label_Volt_14->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[13]));
    ui->label_Volt_15->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[14]));
    ui->label_Volt_16->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[15]));
    ui->label_Volt_17->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[16]));
    ui->label_Volt_18->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[17]));
    ui->label_Volt_19->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[18]));
    ui->label_Volt_20->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[19]));
    ui->label_Volt_21->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[20]));
    ui->label_Volt_22->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[21]));
    ui->label_Volt_23->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[22]));
    ui->label_Volt_24->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[23]));
    ui->label_Volt_25->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[24]));
    ui->label_Volt_26->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[25]));
    ui->label_Volt_27->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[26]));
    ui->label_Volt_28->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[27]));
    ui->label_Volt_29->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[28]));
    ui->label_Volt_30->setStyleSheet(Dialog::barVoltStyle(BMS_VOLT[29]));
}
void Dialog::SetupSpeedometer(){

    mSpeedGauge = new QcGaugeWidget;
    mSpeedGauge->addBackground(99);
    QcBackgroundItem *bkg1 = mSpeedGauge->addBackground(92);
    bkg1->clearrColors();
    bkg1->addColor(0.1,Qt::black);
    bkg1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkg2 = mSpeedGauge->addBackground(88);
    bkg2->clearrColors();
    bkg2->addColor(0.1,Qt::gray);
    bkg2->addColor(1.0,Qt::darkGray);

    mSpeedGauge->addArc(55);
    mSpeedGauge->addDegrees(65)->setValueRange(0,150);
    mSpeedGauge->addColorBand(50);

    mSpeedGauge->addValues(80)->setValueRange(0,150);

    mSpeedGauge->addLabel(70)->setText("Km/h");
    QcLabelItem *lab = mSpeedGauge->addLabel(40);
    lab->setText("0");
    mSpeedNeedle = mSpeedGauge->addNeedle(60);
    mSpeedNeedle->setLabel(lab);
    mSpeedNeedle->setColor(Qt::white);
    mSpeedNeedle->setValueRange(0,150);
    mSpeedGauge->addBackground(7);
    mSpeedGauge->addGlass(88);
    ui->layout_velocimetro->addWidget(mSpeedGauge);

}
void Dialog::SetupArduino(){

    // Identify the port the arduino uno is on.

        bool arduino_is_available = false;
        QString arduino_uno_port_name;

        //  For each available serial port
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            //  check if the serialport has both a product identifier and a vendor identifier
            if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
                //  check if the product ID and the vendor ID match those of the arduino uno
                if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                        && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                    arduino_is_available = true; //    arduino uno is available on this port
                    arduino_uno_port_name = serialPortInfo.portName();
                }
            }
        }

    // Open and configure the arduino port if available

        if(arduino_is_available){
            qDebug() << "Found the arduino port...\n";
            arduino->setPortName(arduino_uno_port_name);
            arduino->open(QSerialPort::ReadOnly);
            arduino->setBaudRate(QSerialPort::Baud9600);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setFlowControl(QSerialPort::NoFlowControl);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial_N()));
        }else{
            qDebug() << "Couldn't find the correct port for the arduino.\n";
            QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
        }

}
void Dialog::GetProductVendorID(){

//  Testing code, prints the description, vendor id, and product id of all ports.
//  Used it to determine the values for the arduino uno

        qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
    }
}
void Dialog::ActualizarPotenciaKelly(double volt1, double volt2, double volt3, double amp1, double amp2, double amp3, double volt4, double volt5, double volt6, double amp4, double amp5, double amp6){
    potencia_fase1_d = volt1*amp1*0.001;
    potencia_fase2_d = volt2*amp2*0.001;
    potencia_fase3_d = volt3*amp3*0.001;

    potencia_fase1_i = volt4*amp4*0.001;
    potencia_fase2_i = volt5*amp5*0.001;
    potencia_fase3_i = volt6*amp6*0.001;

    potencia_total_kelly_d = potencia_fase1_d + potencia_fase2_d + potencia_fase3_d;
    potencia_total_kelly_i = potencia_fase1_i + potencia_fase2_i + potencia_fase3_i;

    potencia_total_kellys = potencia_total_kelly_d + potencia_total_kelly_i;
}
void Dialog::ActualizarPotenciaBMS(double volt, double amp){
    potencia_total_bms = volt*amp*0.1*0.001;
    ui->lcd_consumo_neto->display(potencia_total_bms);
    potencia_consumida = potencia_total_bms-potencia_total_mppts;
    ui->lcd_consumo_motores->display(potencia_consumida);
}
void Dialog::ActualizarPotenciaMPPT(double volt1, double amp1, double volt2, double amp2, double volt3, double amp3, double volt4, double amp4, double volt5, double amp5, double volt6, double amp6){
    potencia_mppt1 = volt1*amp1*0.001;  // En KW
    potencia_mppt2 = volt2*amp2*0.001;  // En KW
    potencia_mppt3 = volt3*amp3*0.001;  // En KW
    potencia_mppt4 = volt4*amp4*0.001;  // En KW
    potencia_mppt5 = volt5*amp5*0.001;  // En KW
    potencia_mppt6 = volt6*amp6*0.001;  // En KW
    potencia_total_mppts = potencia_mppt1 + potencia_mppt2 + potencia_mppt3 + potencia_mppt4 + potencia_mppt5 + potencia_mppt6;
    potencia_consumida = potencia_total_bms-potencia_total_mppts;
    ui->lcd_consumo_motores->display(potencia_consumida);
    ui->lcd_mppt1_power->display(potencia_mppt1);
    ui->lcd_mppt2_power->display(potencia_mppt2);
    ui->lcd_mppt3_power->display(potencia_mppt3);
    ui->lcd_mppt4_power->display(potencia_mppt4);
    ui->lcd_mppt5_power->display(potencia_mppt5);
    ui->lcd_mppt6_power->display(potencia_mppt6);
    ui->all_mppt_power->display(potencia_total_mppts);
}
