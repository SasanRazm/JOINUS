#ifndef JOINUSCAPTURE_H
#define JOINUSCAPTURE_H

#include <QMainWindow>

namespace Ui {
class JOINUSCapture;
}

class JOINUSCapture : public QMainWindow
{
    Q_OBJECT

public:
    explicit JOINUSCapture(QWidget *parent = nullptr);
    ~JOINUSCapture();

private:
    Ui::JOINUSCapture *ui;
};

#endif // JOINUSCAPTURE_H
