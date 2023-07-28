#ifndef IMSOLVER_H
#define IMSOLVER_H

#include <QDialog>

namespace Ui {
class IMsolver;
}

class IMsolver : public QDialog
{
    Q_OBJECT

public:
    explicit IMsolver(QWidget *parent = nullptr);
    ~IMsolver();

private:
    Ui::IMsolver *ui;
};

#endif // IMSOLVER_H
