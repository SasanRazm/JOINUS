#include "dialogplot.h"
#include "ui_dialogplot.h"
#include "myglobalvars.h"

DialogPlot::DialogPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPlot)
{

        ui->setupUi(this);
        //set background color
        QPalette pal = this->palette();
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(0, 120, 255));
        gradient.setColorAt(0.7, QColor(0, 255, 255));
        gradient.setColorAt(1, QColor(230, 255, 255));
        pal.setBrush(QPalette::Window, QBrush(gradient));
        this->setPalette(pal);
        //Finished setting background


        plotWindow = new PlotWindow(this);
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        this->setFixedWidth(screenGeometry.width()*5/6);
        this->setFixedHeight(screenGeometry.height()*4/5);
        plotWindow->setFixedWidth(screenGeometry.width()*5/6);
        plotWindow->setFixedHeight(screenGeometry.height()*4/5);
        plotWindow->show();


}

//void DialogPlot::barPlotter()
//{
//    // set dark background gradient:
//    QLinearGradient gradient(0, 0, 0, 400);
//    gradient.setColorAt(0, QColor(90, 90, 90));
//    gradient.setColorAt(0.38, QColor(105, 105, 105));
//    gradient.setColorAt(1, QColor(70, 70, 70));
//    ui->BarPlotTool->setBackground(QBrush(gradient));

//    //set title
//    ui->BarPlotTool->xAxis->setLabel("Parameters");
//    ui->BarPlotTool->yAxis->setLabel("Margin Percentage");
//    ui->BarPlotTool->xAxis->setLabelFont(QFont("times", 14));
//    ui->BarPlotTool->xAxis->setTickLabelFont(QFont("times", 12));
//    ui->BarPlotTool->yAxis->setLabelFont(QFont("times", 14));
//    ui->BarPlotTool->yAxis->setTickLabelFont(QFont("times", 12));


//    //title = new QCPTextElement(ui->BarPlotTool, "Margin Calculation", QFont("sans", 14, QFont::Bold));


//    RawData.clear();
//    QString DataFile = QDir::currentPath()+OutputFileName;
//    QStringList readData;
//    if (!DataFile.isEmpty())
//        RawData = readmyData(DataFile,delimator);
//    int paramNums=titleVals.length();
//    QVector<double> MaxVals(paramNums),MinVals(paramNums);
//    QVector<double> errMaxVals(paramNums),errMinVals(paramNums);
//    for (int paramCntr=0;paramCntr<RawData.length();paramCntr++)
//        switch (paramCntr%4)
//        {
//        case 0:
//            MaxVals[paramCntr/4]=RawData.at(paramCntr).toDouble();
//            break;
//        case 1:
//            errMaxVals[(paramCntr-1)/4]=RawData.at(paramCntr).toDouble();
//            break;
//        case 2:
//            MinVals[(paramCntr-2)/4]=RawData.at(paramCntr).toDouble();
//            break;
//        case 3:
//            errMinVals[(paramCntr-3)/4]=RawData.at(paramCntr).toDouble();
//            break;
//        default:
//            break;
//        }



//    // create empty bar chart objects:
//    QCPBars *MarginVal = new QCPBars(ui->BarPlotTool->xAxis, ui->BarPlotTool->yAxis);
//    QCPBars *ErrorVal = new QCPBars(ui->BarPlotTool->xAxis, ui->BarPlotTool->yAxis);

//    MarginVal->setAntialiased(true); // gives more crisp, pixel aligned bar borders
//    ErrorVal->setAntialiased(true);
//    MarginVal->setStackingGap(0);
//    ErrorVal->setStackingGap(0);
//    ErrorVal->setName("Error");
//    ErrorVal->setPen(QPen(QColor(250, 170, 20).lighter(150)));
//    ErrorVal->setBrush(QColor(250, 170, 20));
//    MarginVal->setName("Margin");
//    MarginVal->setPen(QPen(QColor(0, 168, 140).lighter(130)));
//    MarginVal->setBrush(QColor(0, 168, 140));


//    // stack bars on top of each other:
//    ErrorVal->moveAbove(MarginVal);

//    // prepare x axis with country labels:
//    QVector<double> ticks;
//    QVector<QString> labels;

//    for (int tickval=0; tickval < titleVals.length() ; tickval++)
//    {
//        ticks.append(tickval);
//        labels.append(titleVals.at(tickval));
//    }

//    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
//    textTicker->addTicks(ticks, labels);
//    ui->BarPlotTool->xAxis->setTicker(textTicker);
//    ui->BarPlotTool->xAxis->setTickLabelRotation(60);
//    ui->BarPlotTool->xAxis->setSubTicks(false);
//    ui->BarPlotTool->xAxis->setTickLength(0, 4);
//    ui->BarPlotTool->xAxis->setRange(0, titleVals.length()+1);
//    ui->BarPlotTool->xAxis->setBasePen(QPen(Qt::white));
//    ui->BarPlotTool->xAxis->setTickPen(QPen(Qt::white));
//    ui->BarPlotTool->xAxis->grid()->setVisible(true);
//    ui->BarPlotTool->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
//    ui->BarPlotTool->xAxis->setTickLabelColor(Qt::white);
//    ui->BarPlotTool->xAxis->setLabelColor(Qt::white);

//    // prepare y axis:
//    //ui->BarPlotTool->yAxis->setRange(0, 12.1);
//    ui->BarPlotTool->yAxis->setPadding(5); // a bit more space to the left border
//    //ui->BarPlotTool->yAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
//    ui->BarPlotTool->yAxis->setBasePen(QPen(Qt::white));
//    ui->BarPlotTool->yAxis->setTickPen(QPen(Qt::white));
//    ui->BarPlotTool->yAxis->setSubTickPen(QPen(Qt::white));
//    ui->BarPlotTool->yAxis->grid()->setSubGridVisible(true);
//    ui->BarPlotTool->yAxis->setTickLabelColor(Qt::white);
//    ui->BarPlotTool->yAxis->setLabelColor(Qt::white);
//    ui->BarPlotTool->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
//    ui->BarPlotTool->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

//    // Add data:

//    ErrorVal->setData(ticks, errMaxVals);
//    MarginVal->setData(ticks, MaxVals);

//    // setup legend:
//    ui->BarPlotTool->legend->setVisible(true);
//    ui->BarPlotTool->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
//    ui->BarPlotTool->legend->setBrush(QColor(255, 255, 255, 100));
//    ui->BarPlotTool->legend->setBorderPen(Qt::NoPen);
//    QFont legendFont = font();
//    legendFont.setPointSize(10);
//    ui->BarPlotTool->legend->setFont(legendFont);
//    ui->BarPlotTool->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
//}

//QStringList DialogPlot::readmyData(QString DataFile,QString delim)
//    {
//        QFile file(DataFile);
//        QStringList fields,tempfields;
//        fields.clear();
//        if (!file.exists())
//        {
//            return fields;
//        }
//        if (!file.open(QFile::ReadOnly | QFile::Text))
//        {
//            return fields;
//        }

//        QTextStream in(&file);

//        while(!in.atEnd()) {
//            QString line = in.readLine();
//            tempfields.append(line.split(delim));
//        }

//        for(int i=0 ; i < tempfields.length() ; i++){
//            if (!tempfields.at(i).isEmpty())
//            {
//                fields.append(tempfields.at(i));
//            }
//        }
//        file.close();
//        in.flush();
//    return fields;
//    }

DialogPlot::~DialogPlot()
{
    RawData.clear();
    delete plotWindow;
    delete ui;
}
