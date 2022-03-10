#ifndef CGOPTIMIZER_H
#define CGOPTIMIZER_H

#include <QDialog>

namespace Ui {
class CGOptimizer;
}

class CGOptimizer : public QDialog
{
    Q_OBJECT

public:
    QStringList ReadOptimFile(QString filename);
    explicit CGOptimizer(QWidget *parent = nullptr);
    void newParamFileGen(QString oldParamFile, QString newParamFile,QStringList newParamVals);
    QStringList newParamGen(QString marginFile, QVector<QVector<QString>> oldParamVector, double Icrit,double RnIc);
    ~CGOptimizer();

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
    Ui::CGOptimizer *ui;
};

#endif // CGOPTIMIZER_H
