#ifndef LGPDISPERS_H
#define LGPDISPERS_H

#include <QDialog>

struct CommandList {
    QStringList NoCommentCommands;
    QStringList CommandswithComment;
};

struct ElementIndex{
    QList<int> R_Index;
    QList<int> L_Index;
    QList<int> C_Index;
    QList<int> J_Index;
    QList<int> V_Index;
    QList<int> I_Index;
    QList<int> SUB_Index;
};

namespace Ui {
class LGPdispers;
}

class LGPdispers : public QDialog
{
    Q_OBJECT

public:
    explicit LGPdispers(QWidget *parent = nullptr);
    struct CommandList ReadCommands(QString FileName);
    QStringList ConstructCommands(struct ElementIndex indexes, QStringList NoCommentCommands);
    QStringList ParameterTree(struct ElementIndex indexes, QStringList NoCommentCommands, int Index_Enable[6]);
    struct ElementIndex AllocateIndexes(QStringList NoCommentCommands);
    QStringList WriteCommands(struct CommandList NewList, QStringList NewCommands);
    double MakeRandNumber(double meanVal, double sigmaVal);
    struct ElementIndex ParamToIndexes(QStringList NoCommentCommands,QStringList ParamTree);


    ~LGPdispers();

private slots:
    QStringList Change_Inductances(QStringList Commands, double sigmaVal);
    QStringList Change_BiasResistances(QStringList Commands, double sigmaVal);
    QStringList Change_ShuntResistances(QStringList Commands, double sigmaVal);
    QStringList Change_Junctions(QStringList Commands, double sigmaVal);
    QStringList Change_Capacitances(QStringList Commands, double sigmaVal);
    void on_buttonBox_accepted();

    void on_toolButtonFilePath_clicked();

private:
    Ui::LGPdispers *ui;
};

#endif // LGPDISPERS_H
