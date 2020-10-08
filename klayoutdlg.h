#ifndef KLAYOUTDLG_H
#define KLAYOUTDLG_H

#include <QDialog>

namespace Ui {
class KlayoutDlg;
}

class KlayoutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit KlayoutDlg(QWidget *parent = nullptr);
    ~KlayoutDlg();

private slots:
    void on_setLayerFile_clicked(bool checked);

    void on_toolButtonPath_clicked();

    void on_toolButtonLayer_clicked();

private:
    Ui::KlayoutDlg *ui;
};

#endif // KLAYOUTDLG_H
