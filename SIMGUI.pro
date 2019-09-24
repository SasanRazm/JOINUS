#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T11:19:06
#
#-------------------------------------------------

QT       += core gui

win32 {
    QT       +=axcontainer
}


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

QMAKE_TARGET = JOINUS-V2.0
TARGET = JOINUS-V2.0
TEMPLATE = app
RC_ICONS = icon.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qcustomplot.cpp \
    plotwindow.cpp \
    readoutdata.cpp \
    dialogplot.cpp \
    myglobalvars.cpp \
    calcvals.cpp \
    simulateall.cpp \
    jsimsyntax.cpp \
    autooptim.cpp


HEADERS += \
        mainwindow.h \
    qcustomplot.h \
    plotwindow.h \
    dialogplot.h \
    myglobalvars.h \
    calcvals.h \
    simulateall.h \
    jsimsyntax.h \
    autooptim.h

FORMS += \
        mainwindow.ui \
    plotwindow.ui \
    dialogplot.ui \
    dialogsetparam.ui \
    autooptim.ui

RESOURCES += \
    simguiresources.qrc


INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
