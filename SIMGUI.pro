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

QMAKE_TARGET = JOINUS-V3.0.0
TARGET = JOINUS-V3.0.0
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
    cgoptimizer.cpp \
    globaloptimizer.cpp \
    inductexgui.cpp \
    joinuscapture.cpp \
    klayoutdlg.cpp \
    lgpdispers.cpp \
        main.cpp \
        mainwindow.cpp \
    margincalc.cpp \
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
    yieldcalc.cpp


HEADERS += \
    cgoptimizer.h \
    globaloptimizer.h \
    inductexgui.h \
    joinuscapture.h \
    klayoutdlg.h \
    lgpdispers.h \
        mainwindow.h \
    margincalc.h \
    plotwindow3d.h \
    qcustomplot.h \
    plotwindow.h \
    dialogplot.h \
    myglobalvars.h \
    calcvals.h \
    setparamsdialog.h \
    simulateall.h \
    jsimsyntax.h \
    yieldcalc.h

FORMS += \
    cgoptimizer.ui \
    globaloptimizer.ui \
    inductexgui.ui \
    joinuscapture.ui \
    klayoutdlg.ui \
    lgpdispers.ui \
        mainwindow.ui \
    margincalc.ui \
    plotwindow.ui \
    dialogplot.ui \
    plotwindow3d.ui \
    setparamsdialog.ui \
    yieldcalc.ui

RESOURCES += \
    simguiresources.qrc


INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
