#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T11:19:06
# Project is created by Sasan Razmkhah as part of
# the ColdFlux project.
#
#-------------------------------------------------

QT       += core gui

win32 {
    QT       +=axcontainer
}

macx {

CONFIG+=app_bundle

}


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

QMAKE_TARGET = JOINUS-V2.1.5
TARGET = JOINUS-V2.1.5
TEMPLATE = app
RC_ICONS = IconJOINUS2.ico

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
    inductexgui.cpp \
    joinuscapture.cpp \
    klayoutdlg.cpp \
        main.cpp \
        mainwindow.cpp \
    plotwindow3d.cpp \
    qcustomplot.cpp \
    plotwindow.cpp \
    readoutdata.cpp \
    dialogplot.cpp \
    myglobalvars.cpp \
    calcvals.cpp \
    setparamsdialog.cpp \
    simulateall.cpp \
    jsimsyntax.cpp \
    autooptim.cpp


HEADERS += \
    inductexgui.h \
    joinuscapture.h \
    klayoutdlg.h \
        mainwindow.h \
    plotwindow3d.h \
    qcustomplot.h \
    plotwindow.h \
    dialogplot.h \
    myglobalvars.h \
    calcvals.h \
    setparamsdialog.h \
    simulateall.h \
    jsimsyntax.h \
    autooptim.h

FORMS += \
    inductexgui.ui \
    joinuscapture.ui \
    klayoutdlg.ui \
        mainwindow.ui \
    plotwindow.ui \
    dialogplot.ui \
    autooptim.ui \
    plotwindow3d.ui \
    setparamsdialog.ui

RESOURCES += \
    simguiresources.qrc


INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
