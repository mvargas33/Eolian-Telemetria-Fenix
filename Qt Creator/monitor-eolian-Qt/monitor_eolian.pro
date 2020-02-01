#-------------------------------------------------
#
# Project created by QtCreator 2016-06-23T00:36:23
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = monitor_eolian
TEMPLATE = app

win32:RC_ICONS += eolian.ico

SOURCES += main.cpp \
           dialog.cpp \
           qcgaugewidget.cpp \
           qcustomplot.cpp \

HEADERS  += dialog.h \
            qcgaugewidget.h \
            qcustomplot.h \

FORMS    += dialog.ui \

RESOURCES += resources.qrc \

