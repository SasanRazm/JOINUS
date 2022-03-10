#ifndef AUTOOPTIM_H
#define AUTOOPTIM_H

#include <QDialog>


namespace Ui {
class AutoOptim;
}

class AutoOptim : public QDialog
{
    Q_OBJECT

public:
    explicit AutoOptim(QWidget *parent = nullptr);
    QStringList ReadOptimFile(QString filename);
    void MarginSimulation(QStringList OptimParams);
    void Yield_Analysis(QStringList OptimParams, double standardDevVal);
    QVector<QVector<QString>> paramDataArrange(QString paramfile);
    void placeParams(QStringList commands,QString tempNetlistFile, QStringList paramListvals,QStringList paramLisnames);
    void newParamFileGen(QString oldParamFile, QString newParamFile,QStringList newParamVals);
    QStringList newParamGen(QString marginFile, QVector<QVector<QString>> oldParamVector, double Icrit,double RnIc);
    QStringList addShuntParam(QStringList oldParamVals, QStringList paramNames, double Icrit,double RnIc);
    ~AutoOptim();

private slots:
    void on_buttonBox_accepted();   
    void on_buttonBox_rejected();
    void onResetClicked();


    QVector<double> MakeRandNum(double meanVal, double sigmaVal, int datacount);
    bool CompareOutputs(QString outFile,QString referenceFile);
    QStringList readMyData(QString DataFile,QString delim);


    void on_buttonBox_helpRequested();
    void on_toolButtonNetlist_clicked();
    void on_toolButtonSolution_clicked();
    void on_toolButtonParam_clicked();
    void on_toolButtonTempF_clicked();
    void on_toolButtonOutF_clicked();



    void on_TestButton_clicked();

    void on_radioButtonMargin_clicked();

    void on_radioButtonOptimMC_clicked();

    void on_radioButtonYield_clicked();


private:
    Ui::AutoOptim *ui;

};

#endif // AUTOOPTIM_H
