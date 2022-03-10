#ifndef MARGINCALC_H
#define MARGINCALC_H

#include <QDialog>

namespace Ui {
class MarginCalc;
}

class MarginCalc : public QDialog
{
    Q_OBJECT

public:
    QStringList ReadOptimFile(QString filename);
    explicit MarginCalc(QWidget *parent = nullptr);
    void MarginSimulation(QStringList OptimParams);
    QStringList readMyData(QString DataFile,QString delim);
    QVector<QVector<QString>> paramDataArrange(QString paramfile);
    bool CompareOutputs(QString outFile,QString referenceFile);
    void placeParams(QStringList commands,QString tempNetlistFile, QStringList paramListvals,QStringList paramLisnames);
    QStringList addShuntParam(QStringList oldParamVals, QStringList paramNames, double Icrit,double RnIc);
    ~MarginCalc();

private slots:

    void on_buttonBox_accepted();
    void onResetClicked();
    void on_buttonBox_rejected();
    void on_TestButton_3_clicked();
    void on_toolButtonNetlist_3_clicked();
    void on_toolButtonSolution_3_clicked();
    void on_toolButtonParam_3_clicked();
    void on_toolButtonTempF_3_clicked();
    void on_toolButtonOutF_3_clicked();

private:
    Ui::MarginCalc *ui;
};

#endif // MARGINCALC_H
