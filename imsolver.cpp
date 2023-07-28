#include "imsolver.h"
#include "ui_imsolver.h"
#include "myglobalvars.h"
#include "simulateall.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QDebug>
#include <QScreen>

IMsolver::IMsolver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IMsolver)
{
    ui->setupUi(this);
}

IMsolver::~IMsolver()
{
    delete ui;
}
