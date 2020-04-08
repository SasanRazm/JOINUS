#include "autooptim.h"
#include "ui_autooptim.h"
#include "myglobalvars.h"
#include "simulateall.h"
#include "calcvals.h"
#include "mainwindow.h"


AutoOptim::AutoOptim(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoOptim)
{
    // simulateall *simall=new simulateall;
    // CalcVals *calcval=new CalcVals;

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
    if (pix.load(":/image/Image/Help/Margin.png")){
        //pix = pix.scaled(ui->label_Picture->size(),Qt::KeepAspectRatio);
        ui->label_Picture->setPixmap(pix);
    }


    QString OptimFile=documentFolderPath+"/OptimConfigFile.tmp";
    QFile optimFile(OptimFile);
    if (optimFile.exists())
    {
        QStringList OptimParams=ReadOptimFile(OptimFile);
        ui->lineEditErrorPer->setText(OptimParams.at(9));
        ui->lineEditIterNum->setText(OptimParams.at(7));
        ui->lineEditJJSpread->setText(OptimParams.at(10));
        ui->lineEditNetlist->setText(OptimParams.at(1));
        ui->lineEditOutF->setText(OptimParams.at(5));
        ui->lineEditParam->setText(OptimParams.at(3));
        ui->lineEditParamVar->setText(OptimParams.at(6));
        ui->lineEditRnIc->setText(OptimParams.at(11));
        ui->lineEditSolution->setText(OptimParams.at(2));
        ui->lineEditTempF->setText(OptimParams.at(4));
        ui->lineEditTempVal->setText(OptimParams.at(8));
        ui->radioButtonMargin->setChecked(true);
        ui->checkBoxNoise->setChecked(true);

    }else{
        ui->lineEditErrorPer->setText("10");
        ui->lineEditIterNum->setText("20");
        ui->lineEditJJSpread->setText("0");
        ui->lineEditNetlist->setText("./Data/DFF.js");
        ui->lineEditOutF->setText("./Data");
        ui->lineEditParam->setText("./Data/DFF.para");
        ui->lineEditParamVar->setText("0");
        ui->lineEditRnIc->setText("373");
        ui->lineEditSolution->setText("./Data/DFF.sol");
        ui->lineEditTempF->setText("./Tmp");
        ui->lineEditTempVal->setText("4.2");
        ui->radioButtonMargin->setChecked(true);
        ui->checkBoxNoise->setChecked(true);
    }

}

void AutoOptim::on_buttonBox_accepted()
{
    QString OptimFile=documentFolderPath+"/OptimConfigFile.tmp";
    QFile file(OptimFile);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Optimizer Error",file.errorString());
    }else
    {
        QTextStream Optimstream(&file);

        QString SimMode=ui->radioButtonMargin->isChecked()?"MS":ui->radioButtonOptimMC->isChecked()?"MCO":"YA";

        Optimstream <<"Mode::"<<SimMode<<'\n';

        Optimstream <<"NF::"<<ui->lineEditNetlist->text()<<'\n';

        Optimstream <<"PSF::"<<ui->lineEditSolution->text()<<'\n';

        Optimstream <<"PF::"<<ui->lineEditParam->text()<<'\n';

        Optimstream <<"TF::"<<ui->lineEditTempF->text()<<'\n';

        Optimstream <<"OF::"<<ui->lineEditOutF->text()<<'\n';

        Optimstream <<"PVP::"<<ui->lineEditParamVar->text()<<'\n';

        Optimstream <<"NI::"<<ui->lineEditIterNum->text()<<'\n';

        Optimstream <<"TV::"<<ui->lineEditTempVal->text()<<'\n';

        Optimstream <<"AEP::"<<ui->lineEditErrorPer->text()<<'\n';

        Optimstream <<"JCCS::"<<ui->lineEditJJSpread->text()<<'\n';

        Optimstream <<"RnIc::"<<ui->lineEditRnIc->text()<<'\n';

        Optimstream <<"TN::"<<ui->checkBoxNoise->isChecked()<<'\n';

        //<<delimator<<delimator<<QString::number(DataIn)<<delimator<<delimator<<QString::number(DataOut)
        file.close();
        Optimstream.flush();
    }
}

QStringList AutoOptim::ReadOptimFile(QString filename)
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

void AutoOptim::MarginSimulation(QStringList OptimParams)
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



    QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
    placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);

    struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);

    QString MSOUT = OptimParams.at(5)+"/MSOUTPUT.DAT";
    QString MSSTAT = OptimParams.at(5)+"/MSSTATUES.DAT";
    QFile fileout(MSOUT);

    if (!fileout.open(QFile::WriteOnly | QFile::Text))
        return;
    else{
        QTextStream stream(&fileout);

    // Test to see if the Nominal values coresponde to solution

     //   qDebug() << DataFile << "  " << OptimParams.at(2);
      //  qDebug() << '\n';


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


void AutoOptim::Yield_Analysis(QStringList OptimParams, double standardDevVal)
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

    //Adding effect of Shunt Resistors to Junctions
    QStringList paramListvals=addShuntParam(temParamListvals, paramLisnames, CritCurrent,OptimParams.at(11).toDouble());


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
    placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);
    struct ConsoleOutputs bb = simclass->simulatenetlist(tempNetlistNominal,0);

    //Define the parameters for file storage
    QString YAOUT = OptimParams.at(5)+"/YAOUTPUT.DAT";
    QString YASTAT = OptimParams.at(5)+"/YASTATUES.DAT";
    QFile fileout(YAOUT);


    if (!fileout.open(QFile::Append | QFile::Text))
        return;

    else{
        QTextStream stream(&fileout);

        if (CompareOutputs(DataFile,OptimParams.at(2)))
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
                placeParams(aa.Commands,tempNetlistNominal,paramListvals,paramLisnames);
            }

            //Simulating all the generated netlists
            for (int itercntr=0;itercntr<iterNum;itercntr++)
            {

                tempNetlistNominal=preTempFileName+QString::number(itercntr)+".cir";
                struct ConsoleOutputs bbtemp = simclass->simulatenetlist(tempNetlistNominal,0);
                if (CompareOutputs(DataFile,OptimParams.at(2)))
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


QVector<double> AutoOptim::MakeRandNum(double meanVal, double sigmaVal, int datacount)
{
    qsrand(QDateTime::currentDateTime().toTime_t());

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

bool AutoOptim::CompareOutputs(QString outFile,QString referenceFile)
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
//    for(int i=0 ; i < OutputData.length() ; i++){
//        if (!OutputData.at(i).isEmpty())
//        {
//            OutData.append(OutputData.at(i));
//        }
//    }
//    OutputData.clear();

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
                tempoutData.append(1e12*OutData.at(i*columNum).toDouble());
               else
                tempoutData.append(static_cast<int>(OutData.at(i*columNum+j).toDouble()/(2*Pi)));
             }
            outData.append(tempoutData);
            k++;
            if (k==rawNumRef)
                break;
        }


    OutData.clear();

    //testing the output
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

QStringList AutoOptim::readMyData(QString DataFile,QString delim)
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

QVector<QVector<QString>> AutoOptim::paramDataArrange(QString paramfile)
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

void AutoOptim::placeParams(QStringList commands,QString tempNetlistFile,QStringList paramListvals,QStringList paramLisnames)
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

QStringList AutoOptim::newParamGen(QString marginFile, QVector<QVector<QString>> oldParamVector, double Icrit,double RnIc)
{
    QStringList marginData=readMyData(marginFile," ");
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
 return addShuntParam(newParamVals, oldParamNames, Icrit, RnIc);
}

QStringList AutoOptim::addShuntParam(QStringList oldParamVals, QStringList paramNames, double Icrit,double RnIc)
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

void AutoOptim::newParamFileGen(QString oldParamFile, QString newParamFile,QStringList newParamVals)
{
    QStringList paramData=readMyData(oldParamFile," ");

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

//Interface Functions

void AutoOptim::onResetClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Optimizer & Margin Analyzer", "Reset the parameters?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->lineEditErrorPer->clear();
        ui->lineEditIterNum->clear();
        ui->lineEditJJSpread->clear();
        ui->lineEditNetlist->clear();
        ui->lineEditOutF->clear();
        ui->lineEditParam->clear();
        ui->lineEditParamVar->clear();
        ui->lineEditRnIc->clear();
        ui->lineEditSolution->clear();
        ui->lineEditTempF->clear();
        ui->lineEditTempVal->clear();
        ui->radioButtonMargin->setChecked(true);
        ui->checkBoxNoise->setChecked(false);
    }

}

void AutoOptim::on_buttonBox_rejected()
{

}

void AutoOptim::on_toolButtonNetlist_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
    if (!NetlistFile.isEmpty())
    {
        ui->lineEditNetlist->setText(NetlistFile);
    }
}

void AutoOptim::on_toolButtonSolution_clicked()
{
    QString SolFile = QFileDialog::getOpenFileName(this, tr("Open Solution File..."),QString(), tr("Solution Files (*.sol);;All Files (*)"));
    if (!SolFile.isEmpty())
    {
        ui->lineEditSolution->setText(SolFile);
    }
}

void AutoOptim::on_toolButtonParam_clicked()
{
    QString ParFile = QFileDialog::getOpenFileName(this, tr("Open Parameters File..."),QString(), tr("Parameters Files (*.para);;All Files (*)"));
    if (!ParFile.isEmpty())
    {
        ui->lineEditParam->setText(ParFile);
    }
}

void AutoOptim::on_toolButtonTempF_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Temporary Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditTempF->setText(dirFolder);
    }
}

void AutoOptim::on_toolButtonOutF_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Outputs Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->lineEditOutF->setText(dirFolder);
    }
}
void AutoOptim::on_buttonBox_helpRequested()
{
    QMessageBox::information(this,"Optimizer","This optimizer uses the same syntax for files as "
                                              "AUTO5. See the \"Syntax Help\" from main window.");
}


void AutoOptim::on_TestButton_clicked()
{
    QString OptimFile=documentFolderPath+"/OptimConfigFile.tmp";
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

void AutoOptim::on_radioButtonMargin_clicked()
{
   ui->label_Text->setText("Margin calculation method:\n"
                         "Nominal margins are set to [-90% - +200%]    \n"
                         "One parameter is swept while all other       \n"
                         "parameters remain at nominal value.          \n"
                         "The margin analyzer stops when the           \n"
                         "step to the next parameter value is below    \n"
                         "the specified maximum margin uncertainty.    ");
   QPixmap pix;
   if (pix.load(":/image/Image/Help/Margin.png")){
       pix = pix.scaled(ui->label_Picture->size(),Qt::KeepAspectRatio);
       ui->label_Picture->setPixmap(pix);
   }
}

void AutoOptim::on_radioButtonOptimMC_clicked()
{
    ui->label_Text->setText("Optimization of parameters:\n"
                            "Nominal margins are calculated first with    \n"
                            "parameters set at their nominal values.      \n"
                            "Margins are calculated again with new        \n"
                            "parameter values. If the margins are         \n"
                            "improved, the new parameters are kept,       \n"
                            "otherwise new values are selected. The       \n"
                            "routines continues until the best margins    \n"
                            "are reached. ");
    QPixmap pix;
    if (pix.load(":/image/Image/Help/Optimizer.PNG")){
        pix = pix.scaled(ui->label_Picture->size(),Qt::KeepAspectRatio);
        ui->label_Picture->setPixmap(pix);
    }
}

void AutoOptim::on_radioButtonYield_clicked()
{
    ui->label_Text->setText("Yield analyzing method:\n"
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
                            "   plotted and can be saved."
                            );
    QPixmap pix;
    if (pix.load(":/image/Image/Help/Yield.png")){
        pix = pix.scaled(ui->label_Picture->size(),Qt::KeepAspectRatio);
        ui->label_Picture->setPixmap(pix);
    }
}

AutoOptim::~AutoOptim()
{
    delete ui;
}

