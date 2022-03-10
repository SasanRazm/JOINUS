#ifndef GLOBALOPTIMIZER_H
#define GLOBALOPTIMIZER_H

#include <QDialog>
#include <qabstractbutton.h>

struct GCommandList {
    QStringList NoCommentCommands;
    QStringList CommandswithComment;
};

namespace Ui {
class GlobalOptimizer;
}

class GlobalOptimizer : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalOptimizer(QWidget *parent = nullptr);
    QStringList Change_Inductances(QStringList Commands, QString FileName);
    QStringList Change_BiasResistances(QStringList Commands, QString FileName);
    QStringList Change_ShuntResistances(QStringList Commands, QString FileName);
    QStringList Change_Junctions(QStringList Commands, QString FileName);
    QStringList Change_Capacitances(QStringList Commands, QString FileName);

    double Change_BetaParam(double Rshunt, double varVal);
    double Change_Thickness(double Lk, double varVal);
    QStringList Change_CriticalCurr(QStringList Commands, QString FileName);
    QStringList Change_BiasVals(QStringList Commands, QString FileName);
    ~GlobalOptimizer();

private slots:
    void on_toolButtonNetlist_3_clicked();
    void on_toolButtonSolution_3_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    struct GCommandList ReadCommands(QString FileName);
    QStringList WriteCommands(struct GCommandList NewList, QStringList NewCommands);
    QVector<double> MakeRandNum(double meanVal, double sigmaVal, int datacount);
    Ui::GlobalOptimizer *ui;
};

#endif // GLOBALOPTIMIZER_H
