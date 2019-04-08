#ifndef DIALOGPLOT_H
#define DIALOGPLOT_H

#include <QDialog>
#include "plotwindow.h"

namespace Ui {
class DialogPlot;
}

class DialogPlot : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPlot(QWidget *parent = nullptr);
    ~DialogPlot();

private:
    Ui::DialogPlot *ui;
    PlotWindow *plotWindow;
};

#endif // DIALOGPLOT_H
