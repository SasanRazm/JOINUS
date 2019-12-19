#ifndef SETPARAMSDIALOG_H
#define SETPARAMSDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SetParamsDialog;
}

class SetParamsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetParamsDialog(QWidget *parent = nullptr);
    void load_properties();
    void save_properties();
    ~SetParamsDialog();

private slots:
    void on_comboBoxEngine_currentIndexChanged(int index);
    void on_pushButtonAddNew_clicked();
    void on_buttonBox_accepted();
    void on_restoredefaults_clicked();

    void on_toolButtonJSIM_clicked();

private:
    Ui::SetParamsDialog *ui;
};

#endif // SETPARAMSDIALOG_H
