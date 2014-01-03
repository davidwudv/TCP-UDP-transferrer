#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T17:17:56
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Transferer
TEMPLATE = app


SOURCES += \
    baseform.cpp \
    helpform.cpp \
    logger.cpp \
    main.cpp \
    setting.cpp \
    toolkit.cpp \
    transferform.cpp \
    transferskt.cpp \
    mainwindow.cpp

HEADERS  += \
    baseform.h \
    helpform.h \
    logger.h \
    main.h \
    resource.h \
    setting.h \
    toolkit.h \
    transferform.h \
    transferskt.h \
    mainwindow.h

FORMS    += \
    helpform.ui \
    transferform.ui \
    MainWindow.ui

OTHER_FILES +=

RESOURCES += \
    icons.qrc
