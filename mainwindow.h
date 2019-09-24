#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSysInfo>
#ifdef _WIN32
#include <ActiveQt>
#endif
#include "dialogplot.h"
#include "autooptim.h"
#include "jsimsyntax.h"

//QT_BEGIN_NAMESPACE
//class QTextEdit;
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
    void on_actionAUTO5_triggered();
    void on_actionChange_background_color_triggered();
    void on_action_WSIM_triggered();
    void on_actionSQUID_MAP_triggered();
    void on_actionMAR_GINOPT_triggered();

    void on_actionJSIM_manual_triggered();

    void on_actionJ_oSIM_manual_triggered();

    void on_ClearTerminalpushButton_clicked();

private:
    Ui::MainWindow *ui;
    DialogPlot *dialogPlot;
    //PlotWindow *plotWindow;
    AutoOptim *autooptim;
    Jsimsyntax *m_jsimsyntax;
};

#endif // MAINWINDOW_H
