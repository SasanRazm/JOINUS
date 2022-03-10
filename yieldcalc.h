#ifndef YIELDCALC_H
#define YIELDCALC_H

#include <QDialog>

namespace Ui {
class YieldCalc;
}

class YieldCalc : public QDialog
{
    Q_OBJECT

public:
    explicit YieldCalc(QWidget *parent = nullptr);
    QStringList ReadOptimFile(QString filename);
    void Yield_Analysis(QStringList OptimParams, double standardDevVal);
    ~YieldCalc();

private slots:
    QVector<double> MakeRandNum(double meanVal, double sigmaVal, int datacount);
    void on_TestButton_3_clicked();
    void onResetClicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_toolButtonNetlist_3_clicked();
    void on_toolButtonSolution_3_clicked();
    void on_toolButtonParam_3_clicked();
    void on_toolButtonTempF_3_clicked();
    void on_toolButtonOutF_3_clicked();


private:
    Ui::YieldCalc *ui;
};

#endif // YIELDCALC_H
