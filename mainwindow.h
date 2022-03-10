#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSysInfo>
#ifdef _WIN32
#include <ActiveQt>
#endif
#include "dialogplot.h"
#include "lgpdispers.h"
#include "globaloptimizer.h"
#include "setparamsdialog.h"
#include "margincalc.h"
#include "yieldcalc.h"
#include "cgoptimizer.h"
#include "jsimsyntax.h"
#include "joinuscapture.h"
#include "inductexgui.h"


//QT_BEGIN_NAMESPACE
//class QScrollArea;
//class QTextEdit;
//class QScrollBar;
//QT_END_NAMESPACE

struct FileDataString
{
    QString ErrMessage;
    QStringList DataVals;
};

struct ConsoleOutputs {
    QString ConsolErr;
    QString ConsolOut;
};

struct processedNL{
    QString Vg;
    QString Icrit,Rnorm,Cap;
    double tstep;
    int columNumb;
    QString OutFileName;
    QStringList Commands;
};

struct IVdata{
    QVector<double> Idata;
    QVector<double> Vdata;
};

struct SimParams{
    QString mainParam;
    QString subParam;
    QString minVal;
    QString maxVal;
    QString pointNum;
    QString tempVal;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_StartPushButton_clicked();
    void firstRun_Init();
    void on_actionAbout_triggered();
    void on_NetlistToolButton_clicked();
    void plotNetlist();
    void GNUplot();
    void XMGracePlot();
    void LoadNetlist(QString NetlistFile);
    void on_SaveToolButton_clicked();
    void on_TypeComboBox_currentIndexChanged(int index);
    void Copy_File(QString outfilepath, QString OutFile);
    void on_MenuNew_Simulation_triggered();
    void on_SaveNetListPushBottun_clicked();
    void on_TemplateNetListPushBottun_clicked();
    void on_MenuSave_triggered();
    void on_MenuOpen_triggered();
    struct SimParams readSimParams();
    void on_actionManuals_triggered();
    void on_SimComboBox_currentIndexChanged(int index);

    //void on_action_Open_Console_triggered();
    //void keyPressEvent(QKeyEvent *event);
    //void on_TerminalPlainTextEdit_textChanged();



    void on_actionFont_Properties_triggered();
    void on_actionRun_Custom_Plotter_triggered();
    void on_actionSet_Temperature_triggered();
    void on_actionChange_background_color_triggered();
    void on_action_WSIM_triggered();
    void on_actionSQUID_MAP_triggered();
    void on_actionMAR_GINOPT_triggered();
    void on_actionJSIM_manual_triggered();
    void on_actionJ_oSIM_manual_triggered();
    void on_ClearTerminalpushButton_clicked();
    void on_StopPushButton_clicked();
    void on_actionQuit_triggered();
    void on_actionSet_Advanced_values_triggered();
    void on_convertPushButton_clicked();
    void on_actionAbout_Qt_triggered();
    void on_actionSchamatic_Capture_triggered();
    void on_actionSet_Workspace_triggered();
    void on_actionInductex_triggered();
    void on_actionSet_Magnetic_Field_triggered();
    void on_actionMargin_Analyzer_triggered();
    void on_actionYield_Analyzer_triggered();
    void on_actionCenter_Gravity_Optimizer_triggered();

    void on_actionParameter_Dispertion_triggered();

    void on_actionLocal_Parameters_triggered();

    void on_actionGlobal_Parameters_triggered();

private:
    Ui::MainWindow *ui;
    DialogPlot *dialogPlot;
    LGPdispers *lgpDispers;
    SetParamsDialog *setparamdialog;
    JOINUSCapture *joinuscapture;
    CGOptimizer *cgoptimizer;
    YieldCalc *yieldcalc;
    MarginCalc *margincalc;
    Jsimsyntax *m_jsimsyntax;
    InductexGUI *inductex;
    GlobalOptimizer *globalOptimier;
    QTimer* myTimer;
};

#endif // MAINWINDOW_H
