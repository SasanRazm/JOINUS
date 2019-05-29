#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "myglobalvars.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "readoutdata.cpp"

QColor color=QColor(60,60,60);

PlotWindow::PlotWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWindow)
{
    //Initializing the graph
    ui->setupUi(this);
    ui->frameProperty->close();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->customPlot->xAxis->setRange(0, 10);
    ui->customPlot->yAxis->setRange(0, 1);
    ui->customPlot->axisRect()->setupFullAxesBox();

    ui->customPlot->plotLayout()->insertRow(0);

    switch (Simindex)
    {case 0:
        title = new QCPTextElement(ui->customPlot, "Time domain simulation", QFont("sans", 14, QFont::Bold));
        break;
    case 1:
        title = new QCPTextElement(ui->customPlot, "I-V characteristics", QFont("sans", 14, QFont::Bold));
        break;
    case 2:
        title = new QCPTextElement(ui->customPlot, "Parametric analysis", QFont("sans", 14, QFont::Bold));
        break;
    case 3:
        title = new QCPTextElement(ui->customPlot, "Temperature analysis", QFont("sans", 14, QFont::Bold));
        break;
    case 4:
        title = new QCPTextElement(ui->customPlot, "BER Calculation", QFont("sans", 14, QFont::Bold));
        break;
    case 5:
        title = new QCPTextElement(ui->customPlot, "Frequency Response", QFont("sans", 14, QFont::Bold));
        break;
    case 6:
        title = new QCPTextElement(ui->customPlot, "I-V characteristics", QFont("sans", 14, QFont::Bold));
        break;
    default:
        title = new QCPTextElement(ui->customPlot, "Plotter", QFont("sans", 14, QFont::Bold));
    }
    ui->customPlot->plotLayout()->addElement(0, 0, title);

    ui->labelColorShow->setAutoFillBackground(true);

    ui->customPlot->xAxis->setLabelFont(QFont("times", 12));
    ui->customPlot->xAxis->setTickLabelFont(QFont("times", 10));
    ui->customPlot->yAxis->setLabelFont(QFont("times", 12));
    ui->customPlot->yAxis->setTickLabelFont(QFont("times", 10));


    if (Simindex==1 || Simindex==6){

        ui->customPlot->xAxis->setLabel("Voltage (mV)");
        ui->customPlot->yAxis->setLabel("Current (µA)");

    } else if (Simindex==4){

        ui->customPlot->xAxis->setLabel("Parameter Value");
        ui->customPlot->yAxis->setLabel("BER (%)");

    } else if (Simindex==5){

        ui->customPlot->xAxis->setLabel("Frequency (GHz)");
        ui->customPlot->yAxis->setLabel("Pulse Number (#)");

    } else{

        ui->customPlot->xAxis->setLabel("Time (ns)");
        ui->customPlot->yAxis->setLabel("Voltage (mV)");

    }


    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    ui->paramSlide->setMaximum(titleVals.length()/2);

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    // setup policy and connect slot for context menu popup:
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    //Read data from the netlist file specified by the main program
    ui->qspinNum_2->setValue(columNum);
    ui->checkBox_2->setChecked(true);

    //Set the plotter mode.
    if (columNum>2 && (Simindex==2 || Simindex==3))
        ui->comboBox->setCurrentIndex(2);
    else
        ui->comboBox->setCurrentIndex(0);

    if (Simindex==1 || Simindex==4 || Simindex==5)
        ui->checkBox->setChecked(true);

    //show the slider bar for sliding between parameters
    if ((Simindex==2) || (Simindex==3))
        ui->paramSlide->show();
    else ui->paramSlide->hide();

    //Clear the data and start reading the out file
    if (Simindex!=2 && Simindex!=3)
    {
        RawData.clear();
        struct FileDataString readData={"Read function is not working correctly!",{}};
        QString DataFile = QDir::currentPath()+OutputFileName;
        if (!DataFile.isEmpty())
        {
            ui->lineEdit_2->setText(DataFile);
            readData = readoutdata(DataFile,delimator);

        if (readData.ErrMessage!="Perfect")
            QMessageBox::warning(this,"Error!",readData.ErrMessage);
        for(int i=0 ; i < readData.DataVals.length() ; i++){
            if (!readData.DataVals.at(i).isEmpty())
            {
                RawData.append(readData.DataVals.at(i));
            }
        }

        //Plotting the data
        addPlots(0);
        }
    } else {

        ui->paramSlide->setValue(0);
//        for (int simstep=0;simstep<titleVals.length()/2;simstep++)
//        {
//            RawData.clear();
//            struct FileDataString readData={"Read function is not working correctly!",{"Plotter error"}};

//            QString DataFile=QDir::currentPath()+OutputFileName;

//            if (Simindex==2)
//                DataFile = QDir::currentPath()+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
//            else if (Simindex==3)
//                DataFile = QDir::currentPath()+"/Data/Tmp"+QString::number(simstep)+"K.dat";

//            if (!DataFile.isEmpty())
//            {
//                ui->lineEdit_2->setText(DataFile);
//                readData = readoutdata(DataFile,delimator);

//            if (readData.ErrMessage!="Perfect")
//                QMessageBox::warning(this,"Error!",readData.ErrMessage);

//            for(int i=0 ; i < readData.DataVals.length() ; i++){
//                if (!readData.DataVals.at(i).isEmpty())
//                {
//                    RawData.append(readData.DataVals.at(i));
//                }

//            }
//            //Plotting the data
//            addPlots(simstep);
//            title->setText("All Graphs");
//            ui->customPlot->replot();
//            }
//        }

        RawData.clear();
        QString DataFile=QDir::currentPath()+OutputFileName;
        if (Simindex==2)
             DataFile = QDir::currentPath()+"/Data/Tmp"+titleVals.at(0)+".dat";
        else if (Simindex==3)
             DataFile = QDir::currentPath()+"/Data/Tmp"+QString::number(0)+"K.dat";

        struct FileDataString readData={"Read function is not working correctly!",{"Plotter error"}};

        if (!DataFile.isEmpty())
        {
             ui->lineEdit_2->setText(DataFile);
             readData = readoutdata(DataFile,delimator);

             if (readData.ErrMessage!="Perfect")
                 QMessageBox::warning(this,"Error!",readData.ErrMessage);

             for(int i=0 ; i < readData.DataVals.length() ; i++){
                 if (!readData.DataVals.at(i).isEmpty())
                 {
                     RawData.append(readData.DataVals.at(i));
                 }
               }
        }

        addPlots(0);
        title->setText("All Graphs");
        title->setText(titleVals.at(1)+titleVals.at(0));
        ui->customPlot->replot();

    }

}
// Giving the plot command
void PlotWindow::on_pushButton_clicked()
{
    addPlots(0);
}

//Clear the board
void PlotWindow::on_pushButton_2_clicked()
{
        removeAllGraphs();
}

//Based on the selected plot type, will add plots to the board
void PlotWindow::addPlots(int simstep)
{
    bool validDatax=true;
    bool validDatay=true;
    int columnSize = ui->qspinNum_2->value();

    int n = RawData.length()/columnSize; // number of points in graph

    QVector<double> x(n), y(n);

    //Plotting the first two column of the data in a single plot
    if (ui->comboBox->currentIndex()==0)

    //if (ui->comboBox->currentText()=="Single Plot ( X , Y )")
    {
        int j=0;
        if (Simindex!=2 && Simindex!=3){
            for(int i=0 ; i < RawData.length() ; i++)
            {
                if (i%columnSize==0) x[j]=RawData.at(i).toDouble(&validDatax);
                else if (i%columnSize==1)
                {
                    y[j]=RawData.at(i).toDouble(&validDatay);
                    if (validDatax && validDatay)
                    j++;
                }
            }

        }else{
            for(int i=0 ; i < RawData.length() ; i++)
            {
                if (i%columnSize==0) x[j]=RawData.at(i).toDouble(&validDatax)*1000000000;
                else if (i%columnSize==1)
                {
                    y[j]=RawData.at(i).toDouble(&validDatay)*1000;
                    if (validDatax && validDatay)
                    j++;
                }
            }

        }

        addPlotSingleXY(x,y,simstep);
    }

    //Plotting the first two column of the data in a single plot and inverting X and Y

    //else if (ui->comboBox->currentText()=="Single Plot ( Y , X )")
     else if (ui->comboBox->currentIndex()==1)
    {
        int j=0;
        if (Simindex!=2 && Simindex!=3){
        for(int i=0 ; i < RawData.length() ; i++)
        {
            if (i%columnSize==0) x[j]=RawData.at(i).QString::toDouble(&validDatax);
            else if (i%columnSize==1)
            {
                y[j]=RawData.at(i).QString::toDouble(&validDatay);
                if (validDatax && validDatay)
                j++;
            }
        }
        } else{
            for(int i=0 ; i < RawData.length() ; i++)
            {
                if (i%columnSize==0) x[j]=RawData.at(i).QString::toDouble(&validDatax)*1000;
                else if (i%columnSize==1)
                {
                    y[j]=RawData.at(i).QString::toDouble(&validDatay)*1000000000;
                    if (validDatax && validDatay)
                    j++;
                }
            }

        }
        addPlotSingleYX(x,y,simstep);
    }

    //Plotting the all the columns, first column is X and the rest are Y

    //else if (ui->comboBox->currentText()=="Multiple Plot ( X , Y1 , Y2 , ...)")
        else if (ui->comboBox->currentIndex()==2)
    {
         if (Simindex!=2 && Simindex!=3){
            for (int k=1; k<columnSize; k++)
            {
                int j=0;
                for(int i=0 ; i < RawData.length() ; i++)
                {
                    if (i%columnSize==0) x[j]=RawData.at(i).QString::toDouble(&validDatax);
                    else if (i%columnSize==k)
                    {
                        y[j]=RawData.at(i).QString::toDouble(&validDatay);
                        if (validDatax && validDatay)
                        j++;
                    }
                }
                addPlotMultiXYY(x,y,k-1,simstep);
            }
         } else{
             for (int k=1; k<columnSize; k++)
             {
                 int j=0;
                 for(int i=0 ; i < RawData.length() ; i++)
                 {
                     if (i%columnSize==0) x[j]=RawData.at(i).QString::toDouble(&validDatax)*1000000000;
                     else if (i%columnSize==k)
                     {
                         y[j]=RawData.at(i).QString::toDouble(&validDatay)*1000;
                         if (validDatax && validDatay)
                         j++;
                     }
                 }
                 addPlotMultiXYY(x,y,k-1,simstep);
             }

         }
    }


    //else if (ui->comboBox->currentText()=="Multiple Plot ( X1 , Y1 , X2 , Y2 , ...)")
        else if (ui->comboBox->currentIndex()==3)
    {
        if (Simindex!=2 && Simindex!=3){
        for (int k=0; k<columnSize-1; k+=2)
        {
            int j=0;
            for(int i=0 ; i < RawData.length() ; i++)
            {
                if (i%columnSize==k) x[j]=RawData.at(i).QString::toDouble(&validDatax);
                else if (i%columnSize==(k+1))
                {
                    y[j]=RawData.at(i).QString::toDouble(&validDatay);
                    if (validDatax && validDatay)
                    j++;
                }
            }
            addPlotMultiXYXY(x,y,k-1,simstep);
        }}else{
            for (int k=0; k<columnSize-1; k+=2)
            {
                int j=0;
                for(int i=0 ; i < RawData.length() ; i++)
                {
                    if (i%columnSize==k) x[j]=RawData.at(i).QString::toDouble(&validDatax)*1000000000;
                    else if (i%columnSize==(k+1))
                    {
                        y[j]=RawData.at(i).QString::toDouble(&validDatay)*1000;
                        if (validDatax && validDatay)
                        j++;
                    }
                }
                addPlotMultiXYXY(x,y,k-1,simstep);
        }}
    }

    // Log and linear axises
    if (ui->checkBox_3->isChecked()==true) setLogarithmic(ui->customPlot);
    else setLinear(ui->customPlot);

    //Auto scale method
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

void PlotWindow::addPlotSingleXY(QVector<double> x,QVector<double> y,int simstep)
{
    if (Simindex!=2 && Simindex!=3){
    QCPCurve *newCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QVector<QCPCurveData> dataCurve(x.length());

    if (Simindex==1 || Simindex==6)
        for (int i=0; i<x.length(); i++)
            dataCurve[i] = QCPCurveData(i, x[i]*1000, y[i]*1000000);
    else
        for (int i=0; i<x.length(); i++)
            dataCurve[i] = QCPCurveData(i, x[i], y[i]);


    newCurve->data()->set(dataCurve,false);

    QPen CurvePen;
    CurvePen.setWidthF(2);
    CurvePen.setColor(color);

    newCurve->setPen(CurvePen);

    if (ui->checkBox_2->isChecked()==true)
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(0));
    if (ui->checkBox->isChecked()==true)
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->rescaleAxes();

    }else
    {        
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount()));
    ui->customPlot->graph()->setData(x, y);
    if (ui->checkBox_2->isChecked()==true)

        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(1));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(4));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(4));
    if (ui->checkBox->isChecked()==true)        
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    QPen graphPen;
    graphPen.setWidthF(2);

    int bluedegree=(simstep*41)%255;
    int reddegree=(simstep*13)%255;
    int greendegree=(simstep*73)%255;

    if (simstep>0)
    {
        color.setRgb(reddegree,bluedegree,greendegree);
        graphPen.setColor(color);
    }

    ui->customPlot->graph()->setPen(graphPen);
    }
}

void PlotWindow::addPlotSingleYX(QVector<double> x,QVector<double> y,int simstep)
{
    ui->customPlot->xAxis->setLabel(ui->customPlot->yAxis->label());
    ui->customPlot->yAxis->setLabel(ui->customPlot->xAxis->label());

    if (Simindex!=2 && Simindex!=3){
    QCPCurve *newCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QVector<QCPCurveData> dataCurve(x.length());

    if (Simindex==1 || Simindex==6)
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, y[i]*1000000, x[i]*1000);
    else
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, y[i], x[i]);


    newCurve->data()->set(dataCurve,false);

    QPen CurvePen;
    CurvePen.setWidthF(2);
    CurvePen.setColor(color);

    newCurve->setPen(CurvePen);

    if (ui->checkBox_2->isChecked()==true)
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(0));
    if (ui->checkBox->isChecked()==true)
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->rescaleAxes();
    }

        else
    {
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount()));
    ui->customPlot->graph()->setData(y, x);
    if (ui->checkBox_2->isChecked()==true)
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(1));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(4));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(4));
    if (ui->checkBox->isChecked()==true)
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    QPen graphPen;
    graphPen.setWidthF(2);

    int bluedegree=(simstep*41)%255;
    int reddegree=(simstep*13)%255;
    int greendegree=(simstep*73)%255;

    if (simstep>0)
    {
        color.setRgb(reddegree,bluedegree,greendegree);
        graphPen.setColor(color);
    }

    graphPen.setColor(color);

    ui->customPlot->graph()->setPen(graphPen);
    }
}

void PlotWindow::addPlotMultiXYY(QVector<double> x,QVector<double> y,int graphcolor,int simstep)
{
    if (Simindex!=2 && Simindex!=3){
    QCPCurve *newCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QVector<QCPCurveData> dataCurve(x.length());


    if (Simindex==1 || Simindex==6)
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, x[i]*1000, y[i]*1000000);
    else
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, x[i], y[i]);

    newCurve->data()->set(dataCurve,false);

    QPen CurvePen;
    CurvePen.setWidthF(2);

    int graphcolorswitch = graphcolor%7;   

    switch (graphcolorswitch)
    {
     case 0:
        CurvePen.setColor(QColor(0, 0, 0));
        break;
    case 1:
        CurvePen.setColor(QColor(255, 0, 0));
        break;
    case 2:
        CurvePen.setColor(QColor(255, 128, 0));
        break;
    case 3:
        CurvePen.setColor(QColor(0, 255, 0));
        break;
    case 4:
        CurvePen.setColor(QColor(0, 255, 255));
        break;
    case 5:
        CurvePen.setColor(QColor(0, 0, 255));
        break;
    case 6:
        CurvePen.setColor(QColor(255, 0, 255));
        break;
    }
    newCurve->setPen(CurvePen);
    if (ui->checkBox_2->isChecked()==true)
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(0));
    if (ui->checkBox->isChecked()==true)
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->rescaleAxes();

    }else
    {
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount()));
    ui->customPlot->graph()->setData(x, y);
    if (ui->checkBox_2->isChecked()==true)
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(1));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
       ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(4));
        //ui->customPlot->graph()->setLineStyle(static_cast<QCPCurve::LineStyle>(4));
    if (ui->checkBox->isChecked()==true)
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    QPen graphPen;
    graphPen.setWidthF(2);

    int graphcolorswitch = graphcolor%7;
    int bluedegree=(simstep*41)%255;
    int reddegree=(simstep*53)%255;
    int greendegree=(simstep*73)%255;

    switch (graphcolorswitch)
    {
     case 0:
        graphPen.setColor(QColor(reddegree, greendegree, bluedegree));
        break;
    case 1:
        graphPen.setColor(QColor(255, greendegree, bluedegree));
        break;
    case 2:
        graphPen.setColor(QColor(255, 128, bluedegree));
        break;
    case 3:
        graphPen.setColor(QColor(reddegree, 255, bluedegree));
        break;
    case 4:
        graphPen.setColor(QColor(reddegree, 255, 255));
        break;
    case 5:
        graphPen.setColor(QColor(reddegree, greendegree, 255));
        break;
    case 6:
        graphPen.setColor(QColor(255, greendegree, 255));
        break;
    }
    ui->customPlot->graph()->setPen(graphPen);
    //graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    }
}

void PlotWindow::addPlotMultiXYXY(QVector<double> x,QVector<double> y,int graphcolor,int simstep)
{
//    if (Simindex==1){
//    QCPCurve *newCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
//    QVector<QCPCurveData> dataCurve(x.length());


//    for (int i=0; i<x.length(); ++i)
//        dataCurve[i] = QCPCurveData(i, x[i]*1000, y[i]*1000000);
    if (Simindex!=2 && Simindex!=3){
    QCPCurve *newCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QVector<QCPCurveData> dataCurve(x.length());


    if (Simindex==1 && Simindex==6)
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, x[i]*1000, y[i]*1000000);
    else
        for (int i=0; i<x.length(); ++i)
            dataCurve[i] = QCPCurveData(i, x[i], y[i]);

    newCurve->data()->set(dataCurve,false);

    QPen CurvePen;
    CurvePen.setWidthF(2);
    int graphcolorswitch = graphcolor%7;
    switch (graphcolorswitch)
    {
     case 0:
        CurvePen.setColor(QColor(0, 0, 0));
        break;
    case 1:
        CurvePen.setColor(QColor(255, 0, 0));
        break;
    case 2:
        CurvePen.setColor(QColor(255, 128, 0));
        break;
    case 3:
        CurvePen.setColor(QColor(0, 255, 0));
        break;
    case 4:
        CurvePen.setColor(QColor(0, 255, 255));
        break;
    case 5:
        CurvePen.setColor(QColor(0, 0, 255));
        break;
    case 6:
        CurvePen.setColor(QColor(255, 0, 255));
        break;
    }
    newCurve->setPen(CurvePen);
    if (ui->checkBox_2->isChecked()==true)
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(1));
    else
        newCurve->setLineStyle(static_cast<QCPCurve::LineStyle>(0));
    if (ui->checkBox->isChecked()==true)
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        newCurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->rescaleAxes();

    }else
    {
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount()));
    ui->customPlot->graph()->setData(x,y);
    if (ui->checkBox_2->isChecked()==true)
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(1));
    else
        ui->customPlot->graph()->setLineStyle(static_cast<QCPGraph::LineStyle>(4));
    if (ui->checkBox->isChecked()==true)
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(6)));
    else
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(1)));
    QPen graphPen;
    graphPen.setWidthF(2);

    int graphcolorswitch = graphcolor%7;
    int bluedegree=(simstep*41)%255;
    int reddegree=(simstep*53)%255;
    int greendegree=(simstep*73)%255;

    switch (graphcolorswitch)
    {
     case 0:
        graphPen.setColor(QColor(reddegree, greendegree, bluedegree));
        break;
    case 1:
        graphPen.setColor(QColor(255, greendegree, bluedegree));
        break;
    case 2:
        graphPen.setColor(QColor(255, 128, bluedegree));
        break;
    case 3:
        graphPen.setColor(QColor(reddegree, 255, bluedegree));
        break;
    case 4:
        graphPen.setColor(QColor(reddegree, 255, 255));
        break;
    case 5:
        graphPen.setColor(QColor(reddegree, greendegree, 255));
        break;
    case 6:
        graphPen.setColor(QColor(255, greendegree, 255));
        break;
    }
    ui->customPlot->graph()->setPen(graphPen);
    }
}

void PlotWindow::titleDoubleClick(QMouseEvent* event)
{
  Q_UNUSED(event)
  if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "JOINUS", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      ui->customPlot->replot();
    }
  }
}

void PlotWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "JOINUS", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void PlotWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "JOINUS", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->customPlot->replot();
    }
  }
}

void PlotWindow::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {

    QCPGraph *graph = ui->customPlot->graph(i);

    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}
void PlotWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void PlotWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
void PlotWindow::removeSelectedGraph()
{
  if (ui->customPlot->selectedPlottables().size() > 0)
  {
    //ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->removePlottable(ui->customPlot->selectedPlottables().first());
    ui->customPlot->replot();
  }
}

void PlotWindow::removeAllGraphs()
{
  //ui->customPlot->clearGraphs();
  ui->customPlot->clearPlottables();
  ui->customPlot->replot();
}

void PlotWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignLeft));
  } else  // general context menu on graphs requested
  {
    if (ui->customPlot->selectedPlottables().size() > 0)
      menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    if (ui->customPlot->plottableCount()> 0)
      menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
  }

  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void PlotWindow::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, static_cast<Qt::Alignment>(dataInt));
      ui->customPlot->replot();
    }
  }
}

void PlotWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  setToolTip(QString::number(dataValue));
}

//Change the axis from linear to log
void PlotWindow::setLogarithmic(QCustomPlot *customPlot)
{
  customPlot->setNoAntialiasingOnDrag(true); // more performance/responsiveness during dragging
  customPlot->yAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  customPlot->yAxis->setTicker(logTicker);
  customPlot->yAxis2->setTicker(logTicker);
  customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
  customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
}
void PlotWindow::setLinear(QCustomPlot *customPlot)
{
  customPlot->setNoAntialiasingOnDrag(true); // more performance/responsiveness during dragging
  customPlot->yAxis->grid()->setSubGridVisible(false);
  customPlot->xAxis->grid()->setSubGridVisible(false);
  customPlot->yAxis->setScaleType(QCPAxis::stLinear);
  customPlot->yAxis2->setScaleType(QCPAxis::stLinear);
  QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
  customPlot->yAxis->setTicker(linTicker);
  customPlot->yAxis2->setTicker(linTicker);
  customPlot->yAxis->setNumberFormat("gb"); // e = exponential, b = beautiful decimal powers
  customPlot->yAxis->setNumberPrecision(2); // makes sure "1*10^4" is displayed only as "10^4"
}

void PlotWindow::on_savePushButton_clicked()
{
    QString PlotFile = QFileDialog::getSaveFileName(this, tr("Save Plot to..."),PlotPath, tr("PNG Files (*.png);;JPEG Files (*.jpg);;Bitmap Files (*.bmp)"));
    if (!PlotFile.isEmpty()){
        QFile file(PlotFile);
        if (!file.open(QIODevice::WriteOnly))
            {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            } else {
            if (PlotFile.endsWith(".png", Qt::CaseInsensitive))
                ui->customPlot->savePng(PlotFile,ui->customPlot->width(),ui->customPlot->height(),2,-1,300);
            else if (PlotFile.endsWith(".jpg", Qt::CaseInsensitive))
                ui->customPlot->saveJpg(PlotFile,ui->customPlot->width(),ui->customPlot->height(),2,-1,300);
            else
                ui->customPlot->saveBmp(PlotFile,ui->customPlot->width(),ui->customPlot->height(),2,-1);
            file.flush();
            file.close();
            }
    }

}



void PlotWindow::on_radioButton_clicked(bool checked)
{
    if (checked)
        ui->frameProperty->show();
    else
        ui->frameProperty->close();
}

void PlotWindow::on_toolButton_2_clicked()
{

    RawData.clear();
    struct FileDataString readData={"Read function is not working correctly!",{}};
    QString DataFile = QFileDialog::getOpenFileName(this, tr("Open Data File for Plot..."),QString(), tr("Data Files (*.Dat *.txt);;All Files (*)"));
    if (!DataFile.isEmpty())
    {
        ui->lineEdit_2->setText(DataFile);
        readData = readoutdata(DataFile,delimator);
    }
    if (readData.ErrMessage!="Perfect")
        QMessageBox::warning(this,"Error!",readData.ErrMessage);
    for(int i=0 ; i < readData.DataVals.length() ; i++){
        if (!readData.DataVals.at(i).isEmpty())
        {
            RawData.append(readData.DataVals.at(i));
        }
    }
}

void PlotWindow::on_colorButton_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    color=  dialog->getColor();
    QVariant variant= color;
    QString colcode = variant.toString();
    ui->labelColorShow->setStyleSheet("QLabel { background-color :"+colcode+" ; color : black; }");
}


PlotWindow::~PlotWindow()
{
    delete ui;
}

void PlotWindow::on_paramSlide_valueChanged(int value)
{
    removeAllGraphs();
    if (value==titleVals.length()/2){
        for (int simstep=0;simstep<titleVals.length()/2;simstep++)
        {
            RawData.clear();
            struct FileDataString readData={"Read function is not working correctly!",{}};


            QString DataFile = QDir::currentPath()+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
            if (Simindex==3)
                DataFile = QDir::currentPath()+"/Data/Tmp"+QString::number(simstep)+"K.dat";


            if (!DataFile.isEmpty())
            {
                ui->lineEdit_2->setText(DataFile);
                readData = readoutdata(DataFile,delimator);
            }
            if (readData.ErrMessage!="Perfect")
                QMessageBox::warning(this,"Error!",readData.ErrMessage);
            for(int i=0 ; i < readData.DataVals.length() ; i++){
                if (!readData.DataVals.at(i).isEmpty())
                {
                    RawData.append(readData.DataVals.at(i));
                }

            }
            //Plotting the data
            addPlots(simstep);
        }
    } else
        {
                RawData.clear();
                struct FileDataString readData={"Read function is not working correctly!",{}};

                QString DataFile = QDir::currentPath()+"/Data/Tmp"+titleVals.at((value)*2)+".dat";
                if (Simindex==3)
                    DataFile = QDir::currentPath()+"/Data/Tmp"+QString::number(value)+"K.dat";


                if (!DataFile.isEmpty())
                {
                    ui->lineEdit_2->setText(DataFile);
                    readData = readoutdata(DataFile,delimator);
                }
                if (readData.ErrMessage!="Perfect")
                    QMessageBox::warning(this,"Error!",readData.ErrMessage);
                for(int i=0 ; i < readData.DataVals.length() ; i++){
                    if (!readData.DataVals.at(i).isEmpty())
                    {
                        RawData.append(readData.DataVals.at(i));
                    }

                }
                //Plotting the data
                addPlots(0);
            }

        if (value==titleVals.length()/2)
            title->setText("All Graphs");
        else
            // Change it for temperature sweep!
            title->setText(titleVals.at(value*2+1)+titleVals.at(value*2));
        ui->customPlot->replot();


}

void PlotWindow::on_pushButton_3_clicked()
{
    //Auto scale method
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}
