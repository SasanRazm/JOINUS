#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include <QInputDialog>
#include "../SIMGUI/qcustomplot.h"
#include <QStringList>

extern QColor color;

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    QCPTextElement *title;
    ~PlotWindow();
    //QStringList RawData;

private slots:
    void titleDoubleClick(QMouseEvent *event);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void addPlots(int simstep);
    void addPlotSingleXY(QVector<double> x,QVector<double> y,int simstep);
    void addPlotSingleYX(QVector<double> x,QVector<double> y,int simstep);
    void addPlotMultiXYY(QVector<double> x,QVector<double> y,int graphcolor,int simstep);
    void addPlotMultiXYXY(QVector<double> x,QVector<double> y,int graphcolor,int simstep);
    void on_pushButton_clicked();
    void setLogarithmic(QCustomPlot *customPlot);
    void setLinear(QCustomPlot *customPlot);
    void on_savePushButton_clicked();
    void on_radioButton_clicked(bool checked);
    void on_toolButton_2_clicked();
    void on_pushButton_2_clicked();
    void on_colorButton_clicked();
    void barPlotter(QVector<double> x,QVector<double> errx,QVector<double> y,QVector<double> erry, QStringList parameterNames);
    void barPlotterH(QVector<double> x,QVector<double> errx,QVector<double> y,QVector<double> erry, QStringList parameterNames);
    void on_paramSlide_valueChanged(int value);

    void on_pushButton_3_clicked();

    void on_toolButtonDecrease_clicked();

    void on_toolButtonIncrease_clicked();

private:
    Ui::PlotWindow *ui;
};

#endif // PLOTWINDOW_H
