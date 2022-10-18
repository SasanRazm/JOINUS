#ifndef GLOBALOPTIMIZER_H
#define GLOBALOPTIMIZER_H

#include <QDialog>
#include <qabstractbutton.h>
#include <QAbstractTableModel>

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
    double Change_CriticalCurr(double areaJJ, double varVal);
    double Change_BiasVals(double biasRes, double varVal);
    ~GlobalOptimizer();

private slots:
    void on_toolButtonNetlist_3_clicked();
    void on_toolButtonSolution_3_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    struct GCommandList ReadCommands(QString FileName);
    void load_paramTable(QString filePath);
    QStringList WriteCommands(struct GCommandList NewList, QStringList NewCommands);
    QVector<double> MakeRandNum(double meanVal, double sigmaVal, int datacount);
    Ui::GlobalOptimizer *ui;
};

class TestModel1 : public QAbstractTableModel
{
    Q_OBJECT

public:
    TestModel1(QObject *parent = nullptr);

    void populateData(const QList<QString> &PN,const QList<QString> &ID,const QList<QString> &IE,const QList<QString> &RD);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<QString> paramName;
    QList<QString> inductanceDesign;
    QList<QString> inductanceExtracted;
    QList<QString> resistanceDesign;
    QList<QString> resistanceExtracted;
    QList<QString> absDiff;
    QList<QString> percDiff;

};

#endif // GLOBALOPTIMIZER_H
