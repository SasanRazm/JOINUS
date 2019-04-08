#include "dialogplot.h"
#include "ui_dialogplot.h"
#include "myglobalvars.h"

DialogPlot::DialogPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPlot)
{
    ui->setupUi(this);
    plotWindow = new PlotWindow(this);
    plotWindow->show();
}

DialogPlot::~DialogPlot()
{
    RawData.clear();
    delete plotWindow;
    delete ui;
}
