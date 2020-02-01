#include "dialog.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/styles.qss");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }

    Dialog w;

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/Roboto-Regular.ttf");
    fontDB.addApplicationFont(":/fonts/Roboto-Bold.ttf");



    w.setWindowTitle("Interfaz Eolian");
    w.show();

    return a.exec();
}
