#include "joinuscapture.h"
#include "ui_joinuscapture.h"

JOINUSCapture::JOINUSCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JOINUSCapture)
{
    ui->setupUi(this);
}

JOINUSCapture::~JOINUSCapture()
{
    delete ui;
}
