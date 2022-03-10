#include "margincalc.h"
#include "ui_margincalc.h"
#include "myglobalvars.h"
#include "simulateall.h"
#include "calcvals.h"
#include "mainwindow.h"

MarginCalc::MarginCalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarginCalc)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(onResetClicked()));

    //set background color
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(120, 255, 255));
    gradient.setColorAt(0.7, QColor(180, 255, 255));
    gradient.setColorAt(1, QColor(230, 255, 255));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);
    //Finished setting background

    //Set the image for start
    QPixmap pix;
    ui->label_Text_3->setText("Margin calculation method:\n"
                          "Nominal margins are set to [-90% - +200%]    \n"
                          "One parameter is swept while all other       \n"
                          "parameters remain at nominal value.          \n"
                          "The margin analyzer stops when the           \n"
                          "step to the next parameter value is below    \n"
                          "the specified maximum margin uncertainty.    ");
    if (pix.load(":/image/Image/Help/Margin.png")){
        //pix = pix.scaled(ui->label_Picture_3->size(),Qt::KeepAspectRatio);
        ui->label_Picture_3->setPixmap(pix);
    }

    QString MarginFile=documentFolderPath+"/MarginConfigwb.tmp";
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

    ui->label_24->hide();
    ui->lineEditJJSpread_3->hide();
    ui->label_23->hide();
    ui->lineEditParamVar_3->hide();
    ui->label_26->hide();
    ui->lineEditIterNum_3->hide();

}

QStringList MarginCalc::ReadOptimFile(QString filename)
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

MarginCalc::~MarginCalc()
{
    delete ui;
}

void MarginCalc::on_buttonBox_accepted()
{
    QString OptimFile=documentFolderPath+"/MarginConfigwb.tmp";
    QFile file(OptimFile);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Margin Error",file.errorString());
    }else
    {
        QTextStream Optimstream(&file);

        QString SimMode="MS";

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

void MarginCalc::on_buttonBox_rejected()
{

}

void MarginCalc::onResetClicked()
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

void MarginCalc::on_TestButton_3_clicked()
{
    QString OptimFile=documentFolderPath+"/MarginConfigwb.tmp";
        on_buttonBox_accepted();
        QStringList OptimParams=ReadOptimFile(OptimFile);
        QString netlistfile=OptimParams.at(1);
        temperature=OptimParams.at(8).toDouble();
        TempFileName=OptimParams.at(4)+"/MarginCalc.cir";
        struct SimParams simParams;
        simParams.pointNum="0";
        simulateall *simclass=new simulateall;
        CalcVals *values= new CalcVals;


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
        QVector<QVector<QString>> paramData=paramDataArrange(OptimParams.at(3));
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
        QStringList paramListvals=addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());

        QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
        placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);

        struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);
        Simindex=0;

        QMessageBox::information(this,"Simulation Finished!","Run Custom plotter from tools to see the result.");

}

void MarginCalc::on_toolButtonNetlist_3_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
    if (!NetlistFile.isEmpty())
    {
        ui->lineEditNetlist_3->setText(NetlistFile);
    }
}

void MarginCalc::on_toolButtonSolution_3_clicked()
{
    QString SolFile = QFileDialog::getOpenFileName(this, tr("Open Solution File..."),QString(), tr("Solution Files (*.sol);;All Files (*)"));
    if (!SolFile.isEmpty())
    {
        ui->lineEditSolution_3->setText(SolFile);
    }
}

void MarginCalc::on_toolButtonParam_3_clicked()
{
    QString ParFile = QFileDialog::getOpenFileName(this, tr("Open Parameters File..."),QString(), tr("Parameters Files (*.para);;All Files (*)"));
    if (!ParFile.isEmpty())
    {
        ui->lineEditParam_3->setText(ParFile);
    }
}

void MarginCalc::on_toolButtonTempF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Temporary Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditTempF_3->setText(dirFolder);
    }
}

void MarginCalc::on_toolButtonOutF_3_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Outputs Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditOutF_3->setText(dirFolder);
    }
}

void MarginCalc::MarginSimulation(QStringList OptimParams)
{
    uint starttime = QDateTime::currentDateTime().toTime_t();

    QString netlistfile=OptimParams.at(1);
    temperature=OptimParams.at(8).toDouble();
    TempFileName=OptimParams.at(4)+"/MarginCalc.cir";
    struct SimParams simParams;
    simParams.pointNum="0";
    simulateall *simclass=new simulateall;
    CalcVals *values= new CalcVals;


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
    QVector<QVector<QString>> paramData=paramDataArrange(OptimParams.at(3));

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

    //qDebug()<< "List of sweeping parameters : " <<titleVals;
    //qDebug()<<"All parameters : "<<paramData.at(3);



    //Adding effect of Shunt Resistors to Junctions
    QStringList paramListvals=addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());



    //QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
    placeParams(aa.Commands,TempFileName,paramListvals,paramLisnames);

    struct ConsoleOutputs bb = simclass->simulatenetlist(TempFileName,0);

    QString MSOUT = OptimParams.at(5)+"/MSOUTPUT.DAT";
    QString MSSTAT = OptimParams.at(5)+"/MSSTATUES.DAT";

    QFile fileout(MSOUT);

    if (!fileout.open(QFile::WriteOnly | QFile::Text))
        return;
    else{
        QTextStream stream(&fileout);

    // Test to see if the Nominal values coresponde to solution

        qDebug() << DataFile << "  " << OptimParams.at(2);
        qDebug() << '\n';


    if (CompareOutputs(DataFile,OptimParams.at(2)))
    {
        double errval,maxmargin,minmargin,livepoint,deadpoint;
        int maxiter = static_cast<int>(log(200/OptimParams.at(9).toDouble())/log(2));
        //Run simulations and determine the margins
        for (int paramcntr=0 ; paramcntr<totalparam ; paramcntr++)
        {
            //Positive part
            maxmargin = 200;
            deadpoint = maxmargin;
            livepoint=0;
            errval = 100;
        //If the parameter is fixed ("0") or dependent on another parameter ("4") this part skips that parameter.

            if ((paramData.at(paramcntr).at(3)!="4" && paramData.at(paramcntr).at(3)!="0"))
            {
                for (int iter=0 ;iter<maxiter; iter++)
                {

                        //replace the parameter for max margin
                        QString tempmaxparam =QString::number(paramData.at(paramcntr).at(1).toDouble()*(1+maxmargin/100));
                        if (paramData.at(paramcntr).at(3)=="3")
                        {
                            temParamListvals[paramcntr]=tempmaxparam;
                            paramListvals=addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());
                            temParamListvals[paramcntr]=paramData.at(paramcntr).at(1);
                        }else
                            paramListvals[paramcntr]=tempmaxparam;

                        //QString tempNetlistMargin=OptimParams.at(4)+"/MSparam"+QString::number(paramcntr)+"iter"+QString::number(iter)+"max.cir";
                        QString tempNetlistMargin=OptimParams.at(4)+"/MSparamMax.cir";

                        placeParams(aa.Commands,tempNetlistMargin,paramListvals,paramLisnames);
                        //qDebug()<< "Parameter values For Maximum: " <<paramListvals<<'\n';

                        bb = simclass->simulatenetlist(tempNetlistMargin,0);
                        if (CompareOutputs(DataFile,OptimParams.at(2)))
                            if (round(maxmargin) == 200)
                            {
                                errval=0;
                                livepoint=maxmargin;
                                break;
                            }else livepoint=maxmargin;
                         else deadpoint=maxmargin;

                        maxmargin=(deadpoint+livepoint)/2;
                        errval=errval/2;
                    }
                 // Write the Margines to the MSOUT file.
                 paramListvals[paramcntr]=paramData.at(paramcntr).at(1);
                 stream << QString::number(maxmargin) << " " << QString::number(errval) << " ";
            }



            //Negative part
            minmargin=90;
            deadpoint = minmargin;
            livepoint=0;
            errval=100;

        //If the parameter is fixed ("0") or dependent on another parameter ("4") this part skips that parameter.
            if ((paramData.at(paramcntr).at(3)!="4" && paramData.at(paramcntr).at(3)!="0"))
            {
                for (int iter=0 ;iter<maxiter; iter++)
                {
                        //replace the parameter for min margin
                        QString tempminmargin =QString::number(paramData.at(paramcntr).at(1).toDouble()*(1-minmargin/100));
                        if (paramData.at(paramcntr).at(3)=="3")
                        {
                            temParamListvals[paramcntr]=tempminmargin;
                            paramListvals=addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());
                            temParamListvals[paramcntr]=paramData.at(paramcntr).at(1);
                        }else
                            paramListvals[paramcntr]=tempminmargin;

                        //QString tempNetlistMargin=OptimParams.at(4)+"/MSparam"+QString::number(paramcntr)+"iter"+QString::number(iter)+"min.cir";
                        QString tempNetlistMargin=OptimParams.at(4)+"/MSparamMin.cir";


                        placeParams(aa.Commands,tempNetlistMargin,paramListvals,paramLisnames);
                        //qDebug()<< "Parameter values For Minimum: " <<paramListvals<<'\n';

                        bb = simclass->simulatenetlist(tempNetlistMargin,0);
                        if (CompareOutputs(DataFile,OptimParams.at(2)))
                            if (round(minmargin) == 90) {
                                errval=0;
                                livepoint=minmargin;
                                break;
                            }else livepoint=minmargin;
                        else deadpoint=minmargin;

                        minmargin=(deadpoint+livepoint)/2;
                        errval=errval/2;
                    }

                   // Write the Margines to the MSOUT file.
                   paramListvals[paramcntr]=paramData.at(paramcntr).at(1);
                   stream << QString::number(minmargin) << " " << QString::number(errval) << '\n';
            }


        }

        //Write Success in the STAT file with time it took to compelete.
        if (OptimParams.at(0)=="MS" || OptimParams.at(0)=="MCO")
        {
            QFile filestat(MSSTAT);
            if (filestat.open(QFile::WriteOnly | QFile::Text))
            {
                QTextStream stream2(&filestat);
                stream2 << "Success"<<'\n';
                stream2 <<QString::number(CritCurrent)<<'\n';
                uint endtime = QDateTime::currentDateTime().toTime_t();
                stream2 << "Margin calculation run for : " << QString::number(endtime-starttime) << " seconds" << '\n';
                stream2.flush();
            }
            filestat.close();

        }else if (OptimParams.at(0)=="MCOO")
        {
            QFile filestat(MSSTAT);
            if (filestat.open(QFile::Append | QFile::Text))
            {
                QTextStream stream2(&filestat);
                stream2 << "Success"<<'\n';
                uint endtime = QDateTime::currentDateTime().toTime_t();
                stream2 << "Margin calculation run for : " << QString::number(endtime-starttime) << " seconds" << '\n';
                stream2.flush();
            }
            filestat.close();
        }

    }

    //If the simulation of the Nominal values do not return right value
    else {
                QFile filestat(MSSTAT);
                if (filestat.open(QFile::WriteOnly | QFile::Text))
                {
                    QTextStream stream2(&filestat);
                    stream2 << "Error"<<'\n';
                    stream2 << "Nominal values do not give the same value as output definition!" << '\n';
                    stream2.flush();
                }
                filestat.close();
            }
    stream.flush();
    }
    fileout.close();
}

QStringList MarginCalc::addShuntParam(QStringList oldParamVals, QStringList paramNames, double Icrit,double RnIc)
{
    //RnIc is used to calculate this effect!
    //The File OptimParams.at(3) will be updated to the new file with said params!
    QStringList newParamVals=oldParamVals;

    for (int i=0;i<paramNames.length();i++)
        for (int j=0;j<paramNames.length();j++)
            if (oldParamVals.at(i)==paramNames.at(j)){
                 newParamVals[i]=QString::number(RnIc/(oldParamVals.at(j).toDouble()*Icrit*1e6));
            }

 return newParamVals;
}

QVector<QVector<QString>> MarginCalc::paramDataArrange(QString paramfile)
{
    //First column is the parameter with (@), second column is the nominal value,
    //third is the main param name for plotting and
    //forth is the type of the parameter (0=fix, 1=No gauss dist., 2=element value, 3= Junction area, 4= dependency).
    //0 means the parameter will stay at Nominal value Ex. {@AA@ 2.16 B1 0}  @AA@ will be replaced with 2.16
    //1 means the parameter changes but has no distribution {@AA@ 2.16 B1 1}  @AA@ will be replaced with the range [2.16*0.1 ... 2.16*3]
    //2 means it has the dist determined by the Parameter variation {@AA@ 2.16 B1 2}  @AA@ will be replaced with the range [2.16(%PVP)*0.1 ... 2.16(%PVP)*3]
    //3 means it has the dist determined by the Junction spread {@AA@ 2.16 B1 3}  @AA@ will be replaced with the range [2.16(%JCCS)*0.1 ... 2.16(%JCCS)*3]
    //4 means value of parameter is dependent on another parameter {@AA@ @BB@ RS1 4}  Value for RS1 is determined from [@BB@ 2.16 B1 3]  d on RnIc

    QVector<QVector<QString>> paramData; //Parametric data in a table for easier calculations
    QVector<QString> tempParamData;


    QStringList ParamData=readMyData(paramfile," ");
    int totalparam=ParamData.length()/4; //gives the number of parameters from paramfile

    for (int iparam=0;iparam<totalparam;iparam++)
    {
        tempParamData.clear();

        for (int jparam=0;jparam<4;jparam++)
            tempParamData.append(ParamData.at(iparam*4+jparam));

        paramData.append(tempParamData);

    } //paramData has the paramfile values in an array

    return paramData;
}

void MarginCalc::placeParams(QStringList commands,QString tempNetlistFile,QStringList paramListvals,QStringList paramLisnames)
{

    QFile filenominal(tempNetlistFile);
    if (!filenominal.open(QFile::WriteOnly | QFile::Text))
        return;
    else{
            QTextStream streamnominal(&filenominal);
        for (int commandcntr=0 ; commandcntr<commands.length() ; commandcntr++)
        {
            for (int paramcntr=0 ; paramcntr<paramLisnames.length() ; paramcntr++)
            {
                if (commands.at(commandcntr).contains(paramLisnames.at(paramcntr),Qt::CaseInsensitive))
                {
                    QString newline=commands.at(commandcntr);
                    newline.replace(paramLisnames.at(paramcntr), paramListvals.at(paramcntr));
                    commands.replace(commandcntr,newline);
                }
            }

            //Change the Devv or devI to phase
            if (commands.at(commandcntr).contains("print",Qt::CaseInsensitive))
            {
                QString newline=commands.at(commandcntr);
                if (commands.at(commandcntr).contains("devv",Qt::CaseInsensitive))
                    newline.replace("devv","PHASE",Qt::CaseInsensitive);
                else if (commands.at(commandcntr).contains("devi",Qt::CaseInsensitive))
                    newline.replace("devi","PHASE",Qt::CaseInsensitive);
                else if (commands.at(commandcntr).contains("phase",Qt::CaseInsensitive))
                    newline.replace("phase","PHASE",Qt::CaseInsensitive);
                else newline="";
                commands.replace(commandcntr,newline);
            }

            streamnominal << commands.at(commandcntr)<<'\n';
        }
        streamnominal.flush();
    }
    filenominal.close();
}

QStringList MarginCalc::readMyData(QString DataFile,QString delim)
{
    QFile file(DataFile);
    QStringList fields,tempfields;
    fields.clear();
    if (!file.exists())
    {
        return fields;
    }
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return fields;
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        tempfields.append(line.split(delim));
    }

    for(int i=0 ; i < tempfields.length() ; i++){
        if (!tempfields.at(i).isEmpty())
        {
            fields.append(tempfields.at(i));
        }
    }
    file.close();
    in.flush();
return fields;
}

bool MarginCalc::CompareOutputs(QString outFile,QString referenceFile)
{

    QStringList RefData;
    QStringList OutData;
    //bool compatibility=0;
    QVector<QVector<double>> refData;
    QVector<QVector<double>> outData;


    RefData=readMyData(referenceFile," ");

//    for(int i=0 ; i < RefData.length() ; i++){
//        qDebug() << RefData.at(i) << "  ";
//        qDebug() << '\n';
//    }


    int rawNumRef= RefData.length()/columNum;


    QVector<double> temprefData;
    for (int i=0 ; i < rawNumRef ; i++)
    {
        temprefData.clear();
        for (int j=0 ; j < columNum ; j++)
            temprefData.append(RefData.at(i*columNum+j).toDouble());

        refData.append(temprefData);
    }

    RefData.clear();


    OutData=readMyData(outFile," ");

    int rawNumOut= OutData.length()/columNum;

    int k=0;
    QVector<double> tempoutData;

    //qDebug() << QString::number(refData.at(k).at(0)) << "  "<<QString::number(OutData.at(50*columNum).toDouble()*1e12)<<'\n';

    for (int i=0 ; i < rawNumOut ; i++)
        if ((OutData.at(i*columNum).toDouble()*1e12) > refData.at(k).at(0)) //&& OutData.at(i*columNum).toDouble()*1e12<refData.at(k).at(0)+timestep
        {
            tempoutData.clear();
            for (int j=0 ; j < columNum ; j++)
            {
               if (j==0)
                tempoutData.append(static_cast<int>(1e12*OutData.at(i*columNum).toDouble()));
               else
                tempoutData.append(static_cast<int>(OutData.at(i*columNum+j).toDouble()/(2*Pi)));
             }
            outData.append(tempoutData);
            k++;

//            qDebug() << QString::number(rawNumRef-k)<<"  "<< QString::number(tempoutData.at(0)) << '\n';
//            qDebug() << QString::number(rawNumOut-i)<< '\n';
            if (k==rawNumRef)
                break;
        }


    OutData.clear();

    //testing the output

//    qDebug() << "REF DATA" << "  ";
//    qDebug() << '\n';
//    for (int i=0 ; i < rawNumRef ; i++)
//    {
//        for (int j=0 ; j < columNum ; j++)
//        qDebug() << QString::number(refData[i][j]) << "  ";
//        qDebug() << '\n';
//    }


//    qDebug() << "OUT DATA" << "  ";
//    qDebug() << '\n';
//    for (int i=0 ; i < rawNumRef ; i++)
//    {
//        for (int j=0 ; j < columNum ; j++)
//        qDebug() << QString::number(outData[i][j]) << "  ";
//        qDebug() << '\n';
//    }

    for (int i=0 ; i < rawNumRef ; i++)
        for (int j=1 ; j < columNum ; j++)
            if (static_cast<int>(refData[i][j]-outData[i][j])!=0)
                return false;

    return true; //compatibility;
}
