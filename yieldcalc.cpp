#include "yieldcalc.h"
#include "ui_yieldcalc.h"
#include "myglobalvars.h"
#include "simulateall.h"
#include "calcvals.h"
#include "mainwindow.h"

YieldCalc::YieldCalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::YieldCalc)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(onResetClicked()));

    QPalette pal = this->palette();
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(255, 150, 255));
        gradient.setColorAt(0.7, QColor(155, 180, 255));
        gradient.setColorAt(1, QColor(255, 230, 255));
        pal.setBrush(QPalette::Window, QBrush(gradient));
        this->setPalette(pal);
        //Finished setting background

        //Set the image for start
        QPixmap pix;
        ui->label_Text_3->setText("Yield analyzing method:\n"
                                  "1- A standard deviation of a Gaussian distribution\n"
                                  "   is selected for parameters under study. \n"
                                  "2- Values of all chosen parameters are picked randomly\n"
                                  "   within this distribution.\n"
                                  "3- They are inserted in a new netlist.\n"
                                  "4- Simulation is performed and the correct operation\n"
                                  "   of the circuit is checked (pass/fail).\n"
                                  "5- Steps 2 to 4 are run multiple times to estimate statistically\n"
                                  "   the yield of thecircuit, which is the proportion of sets of\n"
                                  "   parameters that correspond to circuits operating correctly.\n"
                                  "6- A higher standard deviation is chosen and the full \n"
                                  "   procedure from step 1 is started again.\n"
                                  "7- The yield with standard deviation curve is\n"
                                  "   plotted and can be saved.");
        if (pix.load(":/image/Image/Help/Yield.png")){
            //pix = pix.scaled(ui->label_Picture_3->size(),Qt::KeepAspectRatio);
            ui->label_Picture_3->setPixmap(pix);
        }

        QString MarginFile=documentFolderPath+"/YieldConfigwb.tmp";
        QFile marginFile(MarginFile);
        if (marginFile.exists())
        {
            QStringList OptimParams=ReadOptimFile(MarginFile);
            ui->lineEditErrorPer_3->setText(OptimParams.at(9));
            ui->lineEditIterNum_3->setText(OptimParams.at(7));
            ui->lineEditJJSpread_3->setText(OptimParams.at(10));
            ui->lineEditNetlist_3->setText(OptimParams.at(1));
            ui->lineEditOutF_3->setText(OptimParams.at(5));
            ui->lineEditParam_3->setText(OptimParams.at(3));
            ui->lineEditParamVar_3->setText(OptimParams.at(6));
            ui->lineEditRnIc_3->setText(OptimParams.at(11));
            ui->lineEditSolution_3->setText(OptimParams.at(2));
            ui->lineEditTempF_3->setText(OptimParams.at(4));
            ui->lineEditTempVal_3->setText(OptimParams.at(8));
            ui->checkBoxNoise_3->setChecked(true);
            ui->checkBoxNoise_4->setChecked(true);

        }else{
            ui->lineEditErrorPer_3->setText("10");
            ui->lineEditIterNum_3->setText("20");
            ui->lineEditJJSpread_3->setText("0");
            ui->lineEditNetlist_3->setText("./Data/DFF.js");
            ui->lineEditOutF_3->setText("./Data");
            ui->lineEditParam_3->setText("./Data/DFF.para");
            ui->lineEditParamVar_3->setText("0");
            ui->lineEditRnIc_3->setText("373");
            ui->lineEditSolution_3->setText("./Data/DFF.sol");
            ui->lineEditTempF_3->setText("./Tmp");
            ui->lineEditTempVal_3->setText("4.2");
            ui->checkBoxNoise_3->setChecked(false);
            ui->checkBoxNoise_4->setChecked(false);
        }
}

QStringList YieldCalc::ReadOptimFile(QString filename)
{
    QFile file(filename);
    QStringList optimparams;
    QStringList fields;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"error"};
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        fields.append(line.split("::"));
    }
    file.close();
    in.flush();
    for (int i=0;i<13;i++)
    {
            if (fields.at(i*2+1).isEmpty())
            {
                 switch (i)
                 {
                    case 1:
                     QMessageBox::warning(this,"Optimizer Error","You must select a netlist!");
                     return {"error"};
                    case 2:
                     QMessageBox::warning(this,"Optimizer Error","You must select a phase solution file!");
                     return {"error"};
                    case 3:
                     QMessageBox::warning(this,"Optimizer Error","You must select parameters file!");
                     return {"error"};
                    case 4:
                     optimparams.append("./Tmp");
                     QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for temporary folder!");
                     break;
                    case 5:
                      optimparams.append("./Data");
                      QMessageBox::warning(this,"Optimizer Warning","Default Value is placed output folder!");
                      break;
                     case 6:
                       optimparams.append("0");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for PVP!");
                       break;
                     case 7:
                       optimparams.append("20");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for NI!");
                       break;
                     case 8:
                       optimparams.append("4.2");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for TV!");
                       break;
                     case 9:
                       optimparams.append("10");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for AEP!");
                       break;
                     case 10:
                       optimparams.append("0");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for JCCS!");
                       break;
                     case 11:
                       optimparams.append("373");
                       QMessageBox::warning(this,"Optimizer Warning","Default Value is placed for RnIc!");
                       break;
                     default:
                        return {"error"};
                 }
            }else
            optimparams.append(fields.at(i*2+1));
    }
    return optimparams;
}

YieldCalc::~YieldCalc()
{
    delete ui;
}

void YieldCalc::on_TestButton_3_clicked()
{
    QString OptimFile=documentFolderPath+"/YieldConfigwb.tmp";
            on_buttonBox_accepted();
            QStringList OptimParams=ReadOptimFile(OptimFile);
            QString netlistfile=OptimParams.at(1);
            temperature=OptimParams.at(8).toDouble();
            TempFileName=OptimParams.at(4)+"/MarginCalc.cir";
            struct SimParams simParams;
            simParams.pointNum="0";
            simulateall *simclass=new simulateall;
            CalcVals *values= new CalcVals;
            MarginCalc *marginFunc=new MarginCalc;


            //Make the netlist with noise and temperature
            QString mnnerr=simclass->make_new_netlist(OptimParams.at(12).toInt(),netlistfile,simParams,0,0);

            //get params from netlist
            struct processedNL aa= simclass->processNetlist(TempFileName);
            OutputFileName = aa.OutFileName;
            QString DataFile = documentFolderPath+OutputFileName;
            columNum = aa.columNumb;
            timestep = aa.tstep;
            double CritCurrent=values->convertToValues(aa.Icrit);
            //Now aa.commands contain the netlist values.


            //Read Parameters from the Param file
            QVector<QVector<QString>> paramData=marginFunc->paramDataArrange(OptimParams.at(3));
            int totalparam = paramData.length();


            //Create the netlist with Nominal parameter svalues for simulation
            QStringList paramLisnames;
            QStringList temParamListvals;

            for (int paramcntr=0;paramcntr<totalparam;paramcntr++)
            {
                paramLisnames.append(paramData.at(paramcntr).at(0));
                temParamListvals.append(paramData.at(paramcntr).at(1));
                if (paramData.at(paramcntr).at(3)!=4 && paramData.at(paramcntr).at(3)!="0")
                    titleVals.append(paramData.at(paramcntr).at(2));
            }

            //Adding effect of Shunt Resistors to Junctions
            QStringList paramListvals=marginFunc->addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());

            QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
            marginFunc->placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);

            struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);
            Simindex=0;

            QMessageBox::information(this,"Simulation Finished!","Run Custom plotter from tools to see the result.");
}

void YieldCalc::on_buttonBox_accepted()
{
    QString OptimFile=documentFolderPath+"/YieldConfigwb.tmp";
        QFile file(OptimFile);

        if (!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this,"Yield Error",file.errorString());
        }else
        {
            QTextStream Optimstream(&file);

            QString SimMode="YA";

            Optimstream <<"Mode::"<<SimMode<<'\n';

            Optimstream <<"NF::"<<ui->lineEditNetlist_3->text()<<'\n';

            Optimstream <<"PSF::"<<ui->lineEditSolution_3->text()<<'\n';

            Optimstream <<"PF::"<<ui->lineEditParam_3->text()<<'\n';

            Optimstream <<"TF::"<<ui->lineEditTempF_3->text()<<'\n';

            Optimstream <<"OF::"<<ui->lineEditOutF_3->text()<<'\n';

            Optimstream <<"PVP::"<<ui->lineEditParamVar_3->text()<<'\n';

            Optimstream <<"NI::"<<ui->lineEditIterNum_3->text()<<'\n';

            Optimstream <<"TV::"<<ui->lineEditTempVal_3->text()<<'\n';

            Optimstream <<"AEP::"<<ui->lineEditErrorPer_3->text()<<'\n';

            Optimstream <<"JCCS::"<<ui->lineEditJJSpread_3->text()<<'\n';

            Optimstream <<"RnIc::"<<ui->lineEditRnIc_3->text()<<'\n';

            Optimstream <<"TN::"<<ui->checkBoxNoise_3->isChecked()<<'\n';

            //<<delimator<<delimator<<QString::number(DataIn)<<delimator<<delimator<<QString::number(DataOut)
            file.close();
            Optimstream.flush();
        }

}

void YieldCalc::on_buttonBox_rejected()
{

}

void YieldCalc::onResetClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Yield Analyzer", "Reset the parameters?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->lineEditErrorPer_3->clear();
        ui->lineEditIterNum_3->clear();
        ui->lineEditJJSpread_3->clear();
        ui->lineEditNetlist_3->clear();
        ui->lineEditOutF_3->clear();
        ui->lineEditParam_3->clear();
        ui->lineEditParamVar_3->clear();
        ui->lineEditRnIc_3->clear();
        ui->lineEditSolution_3->clear();
        ui->lineEditTempF_3->clear();
        ui->lineEditTempVal_3->clear();
        ui->checkBoxNoise_3->setChecked(false);
        ui->checkBoxNoise_4->setChecked(false);
    }

}


void YieldCalc::on_toolButtonNetlist_3_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
    if (!NetlistFile.isEmpty())
    {
        ui->lineEditNetlist_3->setText(NetlistFile);
    }
}

void YieldCalc::on_toolButtonSolution_3_clicked()
{
    QString SolFile = QFileDialog::getOpenFileName(this, tr("Open Solution File..."),QString(), tr("Solution Files (*.sol);;All Files (*)"));
    if (!SolFile.isEmpty())
    {
        ui->lineEditSolution_3->setText(SolFile);
    }
}

void YieldCalc::on_toolButtonParam_3_clicked()
{
    QString ParFile = QFileDialog::getOpenFileName(this, tr("Open Parameters File..."),QString(), tr("Parameters Files (*.para);;All Files (*)"));
    if (!ParFile.isEmpty())
    {
        ui->lineEditParam_3->setText(ParFile);
    }
}

void YieldCalc::on_toolButtonTempF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Temporary Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditTempF_3->setText(dirFolder);
    }
}

void YieldCalc::on_toolButtonOutF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Outputs Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditOutF_3->setText(dirFolder);
    }
}

void YieldCalc::Yield_Analysis(QStringList OptimParams, double standardDevVal)
{
    uint starttime = QDateTime::currentDateTime().toTime_t();

    QString netlistfile=OptimParams.at(1);
    int iterNum=OptimParams.at(7).toInt();

    temperature=OptimParams.at(8).toDouble();
    QString preTempFileName=OptimParams.at(4)+"/YieldClac";

    struct SimParams simParams;
    simParams.pointNum="0";
    simulateall *simclass=new simulateall;
    CalcVals *values= new CalcVals;
    MarginCalc *marginFunc=new MarginCalc;


    //Make the netlist with noise and temperature
    QString mnnerr=simclass->make_new_netlist(OptimParams.at(12).toInt(),netlistfile,simParams,0,0);

    //get params from netlist
    struct processedNL aa= simclass->processNetlist(TempFileName);

    //Now aa.commands contain the netlist values.
    OutputFileName = aa.OutFileName;
    QString DataFile = documentFolderPath+OutputFileName;
    columNum = aa.columNumb;
    timestep = aa.tstep;
    double CritCurrent=values->convertToValues(aa.Icrit); //To calculate shunt value!


    //Read Parameters from the Param file
    QVector<QVector<QString>> paramData=marginFunc->paramDataArrange(OptimParams.at(3));

    int totalparam = paramData.length();


    //Create the netlist with Nominal parameter svalues for simulation
    QStringList paramLisnames;
    QStringList temParamListvals;
    titleVals.clear();

    for (int paramcntr=0;paramcntr<totalparam;paramcntr++)
    {
        paramLisnames.append(paramData.at(paramcntr).at(0));
        temParamListvals.append(paramData.at(paramcntr).at(1));
        if (paramData.at(paramcntr).at(3)!="4" && paramData.at(paramcntr).at(3)!="0")
            titleVals.append(paramData.at(paramcntr).at(2));

    }

    //Adding effect of Shunt Resistors to Junctions
    QStringList paramListvals=marginFunc->addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());


    QVector<QStringList> paramList;

    for (int paramcntr=0;paramcntr<totalparam;paramcntr++)
    {
        //MakeRandNum(double meanVal, double sigmaVal, int datacount);
        QVector<double> paramValDouble=MakeRandNum(paramListvals.at(paramcntr).toDouble(),standardDevVal,iterNum);
        temParamListvals.clear();
        for (int itercntr=0;itercntr<iterNum;itercntr++)
        {
            temParamListvals.append(QString::number(paramValDouble.at(itercntr)));
        }
        //puttin the values of each parameter in a list and
        paramList.append(temParamListvals);

    }

    //Testing if primary netlist works

    QString tempNetlistNominal=OptimParams.at(4)+"/YANominal.cir";
    marginFunc->placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);
    struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);

    //Define the parameters for file storage
    QString YAOUT = OptimParams.at(5)+"/YAOUTPUT.DAT";
    QString YASTAT = OptimParams.at(5)+"/YASTATUES.DAT";
    QFile fileout(YAOUT);


    if (!fileout.open(QFile::Append | QFile::Text))
        return;

    else{
        QTextStream stream(&fileout);

        if (marginFunc->CompareOutputs(DataFile,OptimParams.at(2)))
        {
            int YAcounter=0;

            //making the netlists for each iteration
            for (int itercntr=0;itercntr<iterNum;itercntr++)
            {
                tempNetlistNominal=preTempFileName+QString::number(itercntr)+".cir";
                paramListvals.clear();

                for (int paramcntr=0;paramcntr<totalparam;paramcntr++)
                {
                    paramListvals.append(paramList.at(paramcntr).at(itercntr));
                }
                marginFunc->placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);
            }

            //Simulating all the generated netlists
            for (int itercntr=0;itercntr<iterNum;itercntr++)
            {

                tempNetlistNominal=preTempFileName+QString::number(itercntr)+".cir";
                struct ConsoleOutputs bbtemp = simclass->simulatenetlist(tempNetlistNominal,0);
                if (marginFunc->CompareOutputs(DataFile,OptimParams.at(2)))
                    YAcounter++;
            }

            // Calculating the yield value and write it to the file
            stream << QString::number(standardDevVal) << " " << QString::number(static_cast<double>(100*YAcounter/iterNum)) << " " << '\n';


            //Write Success in the STAT file with time it took to compelete.
            uint endTime = QDateTime::currentDateTime().toTime_t();

            QFile filestat(YASTAT);
            if (filestat.open(QFile::Append | QFile::Text))
                {
                   QTextStream stream2(&filestat);
                   stream2 << "Success!"<<'\n';
                   stream2 << "Standard deviation value : "<<QString::number(standardDevVal)<<'\n';
                   stream2 << "Margin calculation run for : " << QString::number(endTime-starttime) << " seconds" << '\n';
                   stream2.flush();
                }
            filestat.close();




        }
        else
        {
                QFile filestat(YASTAT);
                if (filestat.open(QFile::WriteOnly | QFile::Text))
                {
                    QTextStream stream2(&filestat);
                    stream2 << "Error!"<<'\n';
                    stream2 << "Nominal values do not give the same value as output definition!" << '\n';
                    stream2.flush();
                }
                filestat.close();
        }
        stream.flush();

        }
    fileout.close();
}

QVector<double> YieldCalc::MakeRandNum(double meanVal, double sigmaVal, int datacount)
{
    qsrand(QDateTime::currentDateTime().toTime_t());

    sigmaVal=sigmaVal*meanVal/100;


    QVector<double> RandomData(datacount);
    for (int datacntr=0;datacntr<datacount;datacntr++)
    {
    static double n2 = 0.0;
    static int n2_cached = 0;
    if (!n2_cached)
    {
        double x, y, r;
        do
        {
            x = 2.0*qrand()/RAND_MAX - 1;
            y = 2.0*qrand()/RAND_MAX - 1;

            r = x*x + y*y;
        }
        while (r == 0.0 || r > 1.0);
        {
            double d = sqrt(-2.0*log(r)/r);
            double n1 = x*d;
            n2 = y*d;
            double result = n1*sigmaVal + meanVal;
            n2_cached = 1;
            RandomData.append(result);
        }
    }
    else
    {
        n2_cached = 0;
        RandomData.append(n2*sigmaVal + meanVal);
    }
    }
    return RandomData;

}
