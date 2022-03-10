#include "cgoptimizer.h"
#include "ui_cgoptimizer.h"
#include "myglobalvars.h"
#include "simulateall.h"
#include "calcvals.h"
#include "mainwindow.h"

CGOptimizer::CGOptimizer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGOptimizer)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(onResetClicked()));

    //set background color
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(255, 255, 120));
    gradient.setColorAt(0.7, QColor(255, 255, 180));
    gradient.setColorAt(1, QColor(255, 255, 230));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);
    //Finished setting background

    //Set the image for start
    QPixmap pix;
    if (pix.load(":/image/Image/Help/Optimizer.PNG")){
        //pix = pix.scaled(ui->label_Picture->size(),Qt::KeepAspectRatio);
        ui->label_Picture_3->setPixmap(pix);
    }
    ui->label_Text_3->setText("Optimization of parameters:\n"
                                "Nominal margins are calculated first with    \n"
                                "parameters set at their nominal values.      \n"
                                "Margins are calculated again with new        \n"
                                "parameter values. If the margins are         \n"
                                "improved, the new parameters are kept,       \n"
                                "otherwise new values are selected. The       \n"
                                "routines continues until the best margins    \n"
                                "are reached. ");

    QString OptimFile=documentFolderPath+"/OptimConfigwb.tmp";
    QFile optimFile(OptimFile);
    if (optimFile.exists())
    {
        QStringList OptimParams=ReadOptimFile(OptimFile);
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
    ui->label_24->hide();
    ui->lineEditJJSpread_3->hide();
    ui->label_23->hide();
    ui->lineEditParamVar_3->hide();

}

QStringList CGOptimizer::ReadOptimFile(QString filename)
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

CGOptimizer::~CGOptimizer()
{
    delete ui;
}

void CGOptimizer::on_buttonBox_accepted()
{
    QString OptimFile=documentFolderPath+"/OptimConfigwb.tmp";
       QFile file(OptimFile);

       if (!file.open(QFile::WriteOnly | QFile::Text)){
           QMessageBox::warning(this,"Margin Error",file.errorString());
       }else
       {
           QTextStream Optimstream(&file);

           QString SimMode="MCO";

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

void CGOptimizer::onResetClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Optimizer & Margin Analyzer", "Reset the parameters?",
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

void CGOptimizer::on_buttonBox_rejected()
{

}

void CGOptimizer::on_TestButton_3_clicked()
{
    QString OptimFile=documentFolderPath+"/OptimConfigwb.tmp";
            on_buttonBox_accepted();
            QStringList OptimParams=ReadOptimFile(OptimFile);
            QString netlistfile=OptimParams.at(1);
            temperature=OptimParams.at(8).toDouble();
            TempFileName=OptimParams.at(4)+"/MarginCalc.cir";
            struct SimParams simParams;
            simParams.pointNum="0";
            simulateall *simclass=new simulateall;
            CalcVals *values= new CalcVals;
            MarginCalc *autofuns=new MarginCalc;


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
            QVector<QVector<QString>> paramData=autofuns->paramDataArrange(OptimParams.at(3));
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
            QStringList paramListvals=autofuns->addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());

            QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
            autofuns->placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);

            struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);
            Simindex=0;

            QMessageBox::information(this,"Simulation Finished!","Run Custom plotter from tools to see the result.");
}

void CGOptimizer::on_toolButtonNetlist_3_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
    if (!NetlistFile.isEmpty())
    {
        ui->lineEditNetlist_3->setText(NetlistFile);
    }
}

void CGOptimizer::on_toolButtonSolution_3_clicked()
{
    QString SolFile = QFileDialog::getOpenFileName(this, tr("Open Solution File..."),QString(), tr("Solution Files (*.sol);;All Files (*)"));
    if (!SolFile.isEmpty())
    {
        ui->lineEditSolution_3->setText(SolFile);
    }
}

void CGOptimizer::on_toolButtonParam_3_clicked()
{
    QString ParFile = QFileDialog::getOpenFileName(this, tr("Open Parameters File..."),QString(), tr("Parameters Files (*.para);;All Files (*)"));
    if (!ParFile.isEmpty())
    {
        ui->lineEditParam_3->setText(ParFile);
    }
}

void CGOptimizer::on_toolButtonTempF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Temporary Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditTempF_3->setText(dirFolder);
    }
}

void CGOptimizer::on_toolButtonOutF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Outputs Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditOutF_3->setText(dirFolder);
    }
}

QStringList CGOptimizer::newParamGen(QString marginFile, QVector<QVector<QString>> oldParamVector, double Icrit,double RnIc)
{
    MarginCalc *marginFuncs=new MarginCalc;
    QStringList marginData=marginFuncs->readMyData(marginFile," ");
    QStringList newParamVals,oldParamVals,oldParamStat,oldParamNames;
    QVector<double> MaxVal,MinVal;
    double tempParamVal=0;

    int totalparam = oldParamVector.length();
    for (int paramcntr=0;paramcntr<totalparam;paramcntr++)
    {
              oldParamNames.append(oldParamVector.at(paramcntr).at(0));
              oldParamVals.append(oldParamVector.at(paramcntr).at(1));
              oldParamStat.append(oldParamVector.at(paramcntr).at(3));
    }

    int rawNum=oldParamVals.length();

    for (int dataNumCntr=0; dataNumCntr<marginData.length(); dataNumCntr++)
        if (dataNumCntr%4==0)
            MaxVal.append(marginData.at(dataNumCntr).toDouble());
        else if (dataNumCntr%4==2)
            MinVal.append(marginData.at(dataNumCntr).toDouble());


    //raw number is wrong
    //change this part to match the values!
    int dataNumCntr=0;
    for (int rawNumCntr=0; rawNumCntr<rawNum; rawNumCntr++)
    {
        if (oldParamStat.at(rawNumCntr)=="0" || oldParamStat.at(rawNumCntr)=="4")
            newParamVals.append(oldParamVals.at(rawNumCntr));
        else{
            tempParamVal=oldParamVals.at(rawNumCntr).toDouble()*(1+(MaxVal[dataNumCntr]-MinVal[dataNumCntr])/200);
            newParamVals.append(QString::number(tempParamVal));
            dataNumCntr++;
            }
    }
 return marginFuncs->addShuntParam(newParamVals, oldParamNames, Icrit, RnIc);
}

void CGOptimizer::newParamFileGen(QString oldParamFile, QString newParamFile,QStringList newParamVals)
{
    MarginCalc *marginFuncs=new MarginCalc;
    QStringList paramData=marginFuncs->readMyData(oldParamFile," ");

    for (int dataNumCntr=0; dataNumCntr<paramData.length(); dataNumCntr++)
        if (dataNumCntr%4==1)
            paramData[dataNumCntr]=newParamVals.at((dataNumCntr-1)/4);

    QFile fileNewParam(newParamFile);

    if (!fileNewParam.open(QFile::WriteOnly | QFile::Text))
        return;
    else{
        QTextStream streamParam(&fileNewParam);

        for (int rawNumCntr=0; rawNumCntr<newParamVals.length(); rawNumCntr++)
        {
            for (int colNumCntr=0; colNumCntr<4; colNumCntr++)
                streamParam<<paramData.at(rawNumCntr*4+colNumCntr)<<" ";
            streamParam<<'\n';
        }

        streamParam.flush();
    }

    fileNewParam.close();
}
