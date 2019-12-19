#ifndef INDUCTEXGUI_H
#define INDUCTEXGUI_H

#include <QDialog>
#include <QAbstractTableModel>
#include "jsimsyntax.h"


extern int tableSelect;

namespace Ui {
class InductexGUI;
}

class InductexGUI : public QDialog
{
    Q_OBJECT

public:
    explicit InductexGUI(QWidget *parent = nullptr);
    ~InductexGUI();

private slots:
    void on_pushButton_clicked();
    void on_radioButtonNormal_toggled(bool checked);
    void on_toolButtonLoadGDS_clicked();
    void on_toolButtonLoadLDF_clicked();
    void on_toolButtonLoadGDS_2_clicked();
    void on_saveall_clicked();
    void on_restoredefaults_clicked();
    void load_Table(QString InductexOUT);
    void on_pushButtonExport_clicked();
    void on_pushButtonBackAnnotate_clicked();
    void LoadNetlist(QString LayoutFile);
    void LoadGDSImage(QString LayoutFile);

    void on_pushButtonClear_clicked();

    void on_pushButtonSave_clicked();

private:
    Ui::InductexGUI *ui;
    Jsimsyntax *m_jsimsyntax;
};

class TestModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TestModel(QObject *parent = nullptr);

    void populateData(const QList<QString> &PN,const QList<QString> &ID,const QList<QString> &IE,const QList<QString> &RD,
                      const QList<QString> &RE,const QList<QString> &AD,const QList<QString> &PD);
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
#endif // INDUCTEXGUI_H
