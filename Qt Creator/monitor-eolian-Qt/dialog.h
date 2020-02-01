#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QByteArray>
#include <qcgaugewidget.h>
#include <QVector>
#include <QElapsedTimer>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:

    void readSerial_N();
    void Update_filter(QByteArray bytes);
    void Update_filter1(QByteArray bytes);
    void Update_display(QByteArray bytes);
    bool Contiene_letras(QByteArray bytes);
    bool Primer_bit_es_letra(QByteArray bytes);
    void displayDatos();

    /*/FORMATO DE 7 BYTES NEW /*/
    void Recorrido(int indice, int largo, QByteArray bytes);
    bool Es_letra(QByteArray bit, int index);
    bool Es_numero(QByteArray bit, int index);
    void Update_A(QByteArray bytes);
    void Update_K(QByteArray bytes);
    void Update_U(QByteArray bytes);
    void Update_e(QByteArray bytes);
    void Update_D(QByteArray bytes);    //@D
    void Update_N(QByteArray bytes);    //@N
    void Update_X(QByteArray bytes);    //@X
    void Update_r(QByteArray bytes);
    void Update_display_7(QByteArray bytes);
    void Update_display_7_volt(QByteArray bytes);
    /*//////////////////////*/



    /*/FORMATO DE 7 BYTES/*/
    void Update_filter_7(QByteArray bytes);                     //DONE
    bool byte_1_es_letra_7(QByteArray bytes);                   //DONE
    bool Contiene_letras_7(QByteArray bytes);                   //DONE
    //Voltajes
    bool byte_1_arrova_byte_2_es_letra_7(QByteArray bytes);     //DONE
    bool Contiene_letras_arrova_7(QByteArray bytes);            //DONE
    /*//////////////////////*/



    void Speed();
    void makeplot();
    void savetoText();
    void RightSpeed(double incoming_speed);

    void SetupProgressbars();
    void SetupSpeedometer();
    void SetupPlots();
    void SetupArduino();
    void GetProductVendorID();

    void ActualizarPotenciaKelly(double volt1, double volt2, double volt3, double amp1, double amp2, double amp3, double volt4, double volt5, double volt6, double amp4, double amp5, double amp6);
    void ActualizarPotenciaBMS(double volt, double amp);
    void ActualizarPotenciaMPPT(double volt1, double amp1, double volt2, double amp2, double volt3, double amp3, double volt4, double amp4, double volt5, double amp5, double volt6, double amp6);

    QString barTempStyle(double value);
    QString barTempStyle1(double value);
    QString barVoltStyle(double value);
    QString barSoCStyle(double value);

private:
    Ui::Dialog *ui;

    // Variables del Serial
    QSerialPort *arduino;
    QByteArray serialData_aux;
    int contador=0;
    QByteArray Auxiliar[10];
    double DatosDouble[10];
    static const quint16 arduino_uno_vendor_id = 1027;      // Arduino UNO:9025 |Xbee:1027 | Arduino(?): 2341 | Arduino MEGA:9025 | Arduino Leonardo:9025
    static const quint16 arduino_uno_product_id = 24577;    // Arduino UNO:67   |Xbee:24577| Arduino(?): 0043 | Arduino MEGA:66   | Arduino Leonardo:32822
    // Variables 7 bytes NEW
    QByteArray serialData;
    QByteArray serialSelected;
    int indice=0;
    int largo=0;

    // Vectores globales
    double lastRead=0;
    QElapsedTimer timer;
    double BMS[15];
    double BMS_TEMP[60];
    double BMS_VOLT[30];
    double KELLY_DER[14];
    double KELLY_IZ[14];
    double MPPT1[8];
    double MPPT2[8];
    double MPPT3[8];
    double MPPT4[8];
    double MPPT5[8];
    double MPPT6[8];

    //Variables globales especiales
    double speed;               // Velocidad
    double max_speed;           // Máxima velocidad histórica
    double last_speed;

    //POTENCIA//
    double potencia_total_bms;
    double potencia_fase1_d;
    double potencia_fase2_d;
    double potencia_fase3_d;
    double potencia_fase1_i;
    double potencia_fase2_i;
    double potencia_fase3_i;
    double potencia_total_kelly_d;
    double potencia_total_kelly_i;
    double potencia_total_kellys;
    double potencia_mppt1;  //Potencia calculada
    double potencia_mppt2;
    double potencia_mppt3;
    double potencia_mppt4;
    double potencia_mppt5;
    double potencia_mppt6;
    double potencia_total_mppts;
    double potencia_consumida;

    //Variables de Odómetro
    QcGaugeWidget * mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;

    //Estilo variable SOC
    QString safe_1_soc    = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #93d600,stop: 0.4999 #68d600,stop: 0.5 #68d600,stop: 1 #29D600 );border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px; border: 0px solid black;}QProgressBar{border: 0px;background: white; padding: 1px; text-align: center;border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px;font: 75 10pt;}";
    QString safe_2_soc   = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #f0ff00,stop: 0.4999 #ecee00,stop: 0.5 #ecee00,stop: 1 #93d600 );border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px; border: 0px solid black;}QProgressBar{border: 0px;background: white; padding: 1px; text-align: center;border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px;font: 75 10pt;}";
    QString danger_1_soc  = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #ff8100,stop: 0.4999 #ffb400,stop: 0.5 #ffb400,stop: 1 #f0ff00 );border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px; border: 0px solid black;}QProgressBar{border: 0px;background: white; padding: 1px; text-align: center;border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px;font: 75 10pt;}";
    QString danger_2_soc = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #ff0000,stop: 0.4999 #ff4000,stop: 0.5 #ff4000,stop: 1 #ff8100 );border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px; border: 0px solid black;}QProgressBar{border: 0px;background: white; padding: 1px; text-align: center;border-top-left-radius: 3px ;border-top-right-radius: 3px;border-bottom-right-radius: 3px ;border-bottom-left-radius: 3px;font: 75 10pt;}";

    //Estilo variable temperatura izquierda
    QString fondo_verde_i     = "background-color: #26D000;text-align: center;border-radius: 8px; color: #FFFFFF; font-weight: bold;font-size: 15px;border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_amarillo_i  = "background-color: #FDEB00;text-align: center;border-radius: 8px; color: #FFFFFF; font-weight: bold;font-size: 15px;border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_naranjo_i   = "background-color: #F1B400;text-align: center;border-radius: 8px; color: #FFFFFF; font-weight: bold;font-size: 15px;border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_narojo_i    = "background-color: #DE6013;text-align: center;border-radius: 8px; color: #FFFFFF; font-weight: bold;font-size: 15px;border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_rojo_i      = "background-color: #E9292E;text-align: center;border-radius: 8px; color: #FFFFFF; font-weight: bold;font-size: 15px;border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";

    //Estilo variable temperatura derecha
    QString fondo_verde_d     = "background-color: #26D000;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 8px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_amarillo_d  = "background-color: #FDEB00;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 8px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_naranjo_d   = "background-color: #F1B400;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 8px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_narojo_d    = "background-color: #DE6013;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 8px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";
    QString fondo_rojo_d      = "background-color: #E9292E;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 8px;border-bottom-right-radius: 0px ;border-bottom-left-radius: 0px; border: 0px solid black;";

    //Estilo variable voltaje (abajo)
    QString fondo_verde_v     = "background-color: #26D000;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 8px ;border-bottom-left-radius: 8px; border: 0px solid black;";
    QString fondo_amarillo_v  = "background-color: #FDEB00;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 8px ;border-bottom-left-radius: 8px; border: 0px solid black;";
    QString fondo_naranjo_v   = "background-color: #F1B400;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 8px ;border-bottom-left-radius: 8px; border: 0px solid black;";
    QString fondo_narojo_v    = "background-color: #DE6013;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 8px ;border-bottom-left-radius: 8px; border: 0px solid black;";
    QString fondo_rojo_v      = "background-color: #E9292E;text-align: center; border-radius: 5px; color: #FFFFFF; font-weight: bold;font-size: 15px; border-top-left-radius: 0px ;border-top-right-radius: 0px;border-bottom-right-radius: 8px ;border-bottom-left-radius: 8px; border: 0px solid black;";

};

#endif // DIALOG_H
