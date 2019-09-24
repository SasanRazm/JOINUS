#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myglobalvars.h"
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include "calcvals.h"
#include "simulateall.h"

//#ifdef __APPLE__
//#include <carbon/Carbon.h>
//#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{  
    ui->setupUi(this);

#ifdef __APPLE__
    QDir dir = QCoreApplication::applicationDirPath();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    rootPath = dir.absolutePath();
    PlotPath = rootPath+"/Graphs";
    DataPath = rootPath+"/Data";
    WSIMPath = rootPath+"/Add-ons";
    TempFileName = rootPath+"/Data/ModiNL.cir";
    TempOutFile = rootPath+"/Data/TempOut.tmp";

    // Create authorization reference
//    AuthorizationRef authorizationRef;
//    OSStatus status;

//    status = AuthorizationCreate(nullptr, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);

//    char* tool = QApplication::instance()->applicationFilePath().toLocal8Bit().data();
//    char* args[] = { "STARTUPDATE", nullptr };
//    FILE* pipe = nullptr;

//    status = AuthorizationExecuteWithPrivileges(authorizationRef, tool, kAuthorizationFlagDefaults, args, &pipe);
//    QApplication::instance()->quit();

#endif

    // Initialize the values for the input and output files
    ui->SaveLineEdit->setText(DataPath+"/Out.dat");
    ui->NetlistLineEdit->setText(DataPath+"/si.inp");
    ui->frame->close();
    ui->TempLineEdit->setEnabled(false);
    QString NetlistFile = ui->NetlistLineEdit->text();
    temperature=4.2;

    // Set text editor font
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);
    ui->NetlistPlainTextEdit->setFont(font);
    font.setFamily("Arial");
    font.setPointSize(8);
    ui->TerminalPlainTextEdit->setFont(font);
    ui->actionIV_Curve_Statistical_Noise->setChecked(true);

    //Tests For MacOS
    //Displaying data paths
//    ui->TerminalPlainTextEdit->appendPlainText("Netlist Path: "+DataPath+"/si.inp");
//    ui->TerminalPlainTextEdit->appendPlainText("Output file name: "+dir.currentPath()+OutputFileName);
//    ui->TerminalPlainTextEdit->appendPlainText("JSIM Path: "+dir.absolutePath()+"/JSIM");


    // Initialize the highlighter for the netlist editor

    m_jsimsyntax = new Jsimsyntax(ui->NetlistPlainTextEdit->document());
    LoadNetlist(NetlistFile);

}


// *** Main Simulation Function***
// By pushing this button, the netlist would be compiled and based on the user selections,
// the simulation would start.
// You can add new simulation types here!
// Be careful of the global parameters!

void MainWindow::on_StartPushButton_clicked()
{
    simulateall *simclass=new simulateall;

    if (Simindex==4)
        TempFileName = rootPath+"/Data/TempNetlistBER.cir";
    else if (Simindex==5)
        TempFileName = rootPath+"/Data/TempNetlistFreq.cir";
    else
        TempFileName = rootPath+"/Data/ModiNL.cir";


    // ConsoleOutputs gives the outputs and errors of the terminal. In windows it is Command Prompt.
    struct ConsoleOutputs simout={"",""};

    // SimParams are parameters that are set for the simulation. Full defenition in the header file.
    struct SimParams simParams=readSimParams();
    CalcVals *calcVal=new CalcVals;

    //Statistical analyze of the I-V curve activation
    bool IVstatistical = ui->actionIV_Curve_Statistical_Noise->isChecked();

    // Testing to see if the entered parameters are legit

    bool validData1=true;
    bool validData2=true;
    validData2=simParams.pointNum.toInt(&validData1)>0;

    if (!validData1 || !validData2)
    {
        QMessageBox::warning(this,"Error!","Number of the points should be positive integer!");
        return;
    }

//    validData2=simParams.tempVal.toDouble(&validData1)>=0;

//    if ((!validData1 || !validData2) && Simindex!=3)
//    {
//        QMessageBox::warning(this,"Error!","Temperature should be real number bigger or equal to zero!");
//        return;
//    }
    if (Simindex==3)
        temperature=4.2;
//    else
//        temperature = simParams.tempVal.toDouble();

    int SimulatorIndex=ui->SimComboBox->currentIndex();
    bool noise= ui->NoiseCheckBox->isChecked();
    QString NetlistFile=ui->NetlistLineEdit->text();
    QString BERtempfile=rootPath+"/Data/TempOutputBER.DAT";
    QString Freqtempfile=rootPath+"/Data/TempOutputFreq.DAT";

    ui->ProgressBar->setValue(0);


    // *** new netlist ***
    // Make a netlist without comments with changes for temperature and noise made to it
    QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
    if (mnnerr!="Success"){
        QMessageBox::warning(this,"Error!",mnnerr);
        return;
    }

    // set the values for the simulation and give information about it
        ui->SimComboBox->setEnabled(false);
        ui->TypeComboBox->setEnabled(false);
        ui->StartPushButton->setEnabled(false);
        ui->TerminalPlainTextEdit->appendPlainText("Output file name: "+rootPath+OutputFileName);
        ui->TerminalPlainTextEdit->appendPlainText("Column Number: "+QString::number(columNum));


        QString noisecond=ui->NoiseCheckBox->isChecked()?"On":"off";
        ui->TerminalPlainTextEdit->appendPlainText("Noise Simulation: "+noisecond);
        ui->TerminalPlainTextEdit->appendPlainText("Temperature value:"+  QString::number(temperature));
        ui->TerminalPlainTextEdit->appendPlainText("Delimator: '"+  delimator+"'");



        QString initSubParam=simParams.subParam;
        double stepSize=0;
        titleVals.clear();

        QVector <long> DataIn(simParams.pointNum.toInt());
        QVector <long> DataOut(simParams.pointNum.toInt());

        switch (Simindex)
        {
            // Normal time domain simulation
        case 0:

            ui->ProgressBar->setValue(50);
            simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
            ui->ProgressBar->setValue(100);
            
            //testing jsim command for MacOS
            //ui->TerminalPlainTextEdit->appendPlainText(DataPath+"/jsim_n "+TempFileName);
            
            break;


            // I-V simulation results
        case 1:


            if (IVstatistical)
            {
                ui->ProgressBar->setValue(20);

                //testing the netlist

                simout = simclass->simulateivtest(TempFileName,SimulatorIndex,simParams.pointNum.toInt(),simParams.subParam.toInt());
                ui->ProgressBar->setValue(40);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
                TempFileName=rootPath+"/Data/TempOut1.tmp";

                //make new netlist after statistical analysis        
                mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,2);
                if (mnnerr!="Success"){
                    QMessageBox::warning(this,"My Error!",mnnerr);
                    ui->StartPushButton->setEnabled(true);
                    return;
                }
                ui->ProgressBar->setValue(50);

                //simulate the newly generated netlist from statistical data
                simout = simclass->simulateivcurve(TempFileName,SimulatorIndex,simParams.pointNum.toInt(),simParams.subParam.toInt(),IVstatistical);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);


            }else{
                ui->ProgressBar->setValue(50);
                simout = simclass->simulateivcurve(TempFileName,SimulatorIndex,simParams.pointNum.toInt(),simParams.subParam.toInt(),IVstatistical);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
            }

            ui->ProgressBar->setValue(100);
            columNum=2;

            break;

            // Parametric shift
        case 2:

            if (simParams.pointNum.toInt()>1)
                stepSize=(calcVal->convertToValues(simParams.maxVal)-
                          calcVal->convertToValues(simParams.minVal))/(simParams.pointNum.toInt()-1);


            for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
            {
                simParams.subParam=initSubParam+"<*>"+calcVal->convertToUnits(calcVal->convertToValues(simParams.minVal)+simstep*stepSize);

                // Change the netlist with the new parameter
                TempFileName=rootPath+"/Data/TempOut"+QString::number(simstep)+".tmp";
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);
                else{

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=rootPath+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
                    QFile::remove(Outtemp);

                    bool whileLoopLogic=QFile::copy(rootPath+OutputFileName, Outtemp);

                    while (!whileLoopLogic){}                  

                }

              }

            break;

            //Temperature sweep simulation
        case 3:
            simParams.subParam="4.2";


            if (simParams.pointNum.toInt()>1)
                stepSize=(calcVal->convertToValues(simParams.maxVal)-
                          calcVal->convertToValues(simParams.minVal))/(simParams.pointNum.toInt()-1);

            for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
            {
                temperature=calcVal->convertToValues(simParams.minVal)+simstep*stepSize;
                ui->TempLineEdit->setText(QString::number(temperature));
                simParams.tempVal=QString::number(temperature);

                //Change the netlist with the new parameter
                TempFileName=rootPath+"/Data/TempOut"+QString::number(simstep)+".tmp";
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);

                else{

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=rootPath+"/Data/Tmp"+QString::number(simstep)+"K.dat";
                    QFile::remove(Outtemp);
                   bool whileLoopLogic=QFile::copy(rootPath+OutputFileName, Outtemp);
                   while (!whileLoopLogic){}
                }

              }

            break;

        //Bit error rate calculations
        case 4:

            if (simParams.pointNum.toInt()>1)
                stepSize=(calcVal->convertToValues(simParams.maxVal)-
                          calcVal->convertToValues(simParams.minVal))/(simParams.pointNum.toInt()-1);


            if (QFile::exists(BERtempfile))
                QFile::remove(BERtempfile);


            for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
            {
                simParams.subParam=initSubParam+"<*>"+calcVal->convertToUnits(calcVal->convertToValues(simParams.minVal)+simstep*stepSize);


                // Change the netlist with the new parameter                

                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);
                else{

                    simout=simclass->simulateBER(SimulatorIndex,simParams.tempVal.toFloat(),calcVal->convertToValues(simParams.minVal)+simstep*stepSize);
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
                }
              }

            if (QFile::exists(rootPath+OutputFileName))
                QFile::remove(rootPath+OutputFileName);
            if(QFile::copy(BERtempfile, rootPath+OutputFileName))
                QFile::remove(BERtempfile);
            else
                break;

            columNum=2;

            break;

        //Shift and find maximum frequency
        case 5:

            if (QFile::exists(Freqtempfile))
                QFile::remove(Freqtempfile);

            if (simParams.pointNum.toInt()>1){
                double minV=Phi0*calcVal->convertToValues(simParams.minVal);
                double maxV=Phi0*calcVal->convertToValues(simParams.maxVal);
                stepSize=(maxV-minV)/(simParams.pointNum.toInt()-1);
                QVector<double> Multiplyers;
                QStringList Mulfields;
                Mulfields.append(simParams.tempVal.split(","));


                for (int mulcntr=0;mulcntr<Mulfields.length();mulcntr++)
                      Multiplyers.append(Mulfields.at(mulcntr).toDouble());


                for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
                {

                    simParams.subParam=initSubParam+"<*>"+calcVal->convertToUnits(minV+simstep*stepSize);

                    // Change the netlist with the new parameter

                    QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                    if (mnnerr!="Success")
                        QMessageBox::warning(this,"Error!",mnnerr);
                    else{

                        double freqvalforfunc=calcVal->convertToValues(simParams.minVal)+simstep*stepSize/Phi0;
                        simout=simclass->simulateFreq(SimulatorIndex,Multiplyers,freqvalforfunc);
                        ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());
                        ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                        ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    }
                  }
            }else{

                simParams.subParam=initSubParam+"<*>"+calcVal->convertToUnits(Phi0*calcVal->convertToValues(simParams.minVal));

                QVector<double> Multiplyers;
                QStringList Mulfields;
                Mulfields.append(simParams.tempVal.split(","));
                for (int mulcntr=0;mulcntr<Mulfields.length();mulcntr++)
                    Multiplyers[mulcntr]=Mulfields.at(mulcntr).toDouble();

                // Change the netlist with the new parameter

                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);
                else{                  
                    simout=simclass->simulateFreq(SimulatorIndex,Multiplyers,calcVal->convertToValues(simParams.minVal));
                    ui->ProgressBar->setValue(100);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
                }
            }

                if (QFile::exists(rootPath+OutputFileName))
                    QFile::remove(rootPath+OutputFileName);
                if(QFile::copy(Freqtempfile, rootPath+OutputFileName))
                    QFile::remove(Freqtempfile);
                else
                    break;      

            break;

        //New I-V calculation method
        case 6:

            ui->ProgressBar->setValue(50);
            simout = simclass->simulateivnew(TempFileName,SimulatorIndex,simParams.pointNum.toInt());
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
            ui->ProgressBar->setValue(100);
            columNum=2;
            break;


        default:
            ui->statusBar->showMessage("The simulation type is not valid!");
            break;
        }

        ui->SimComboBox->setEnabled(true);
        ui->TypeComboBox->setEnabled(true);
        if (ui->SaveCheckBox->isChecked()==true)
            Copy_File(ui->SaveLineEdit->text(),rootPath+OutputFileName);


        if (ui->PlotCheckBox->isChecked()==true)
            plotNetlist();

        ui->StartPushButton->setEnabled(true);

}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About JOINUS","JOINUS was developed at University Savoie Mont Blanc in France as part of the ColdFlux/Supertool project.\n"
                                           "The software is based upon work supported by the Office of the Director of National Intelligence (ODNI), "
                                           "Intelligence Advanced Research Projects Activity (IARPA).");
}

// Load the netlist file and copies it to the Plain text editor
void MainWindow::on_NetlistToolButton_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp);;All Files (*)"));
    if (!NetlistFile.isEmpty())
    {
        ui->NetlistLineEdit->setText(NetlistFile);
        LoadNetlist(NetlistFile);

    }
}

//Calls Folder Dialog for determination of the output path
void MainWindow::on_SaveToolButton_clicked()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        ui->SaveLineEdit->setText(dirFolder+OutputFileName);
    }
}

// Ploting the output file
//Based on the user choice, one fo the programs will be called to the output
void MainWindow::plotNetlist()
{
    int comboIndexVal= ui->PlotComboBox->currentIndex();
    ui->PlotComboBox->setEnabled(false);

    switch(comboIndexVal){

    case 0:
        dialogPlot = new DialogPlot(this);
        dialogPlot->show();
        break;
    case 1:
        GNUplot();
        break;
    case 2:
        XMGracePlot();
        break;
    default:
        ui->statusBar->showMessage("The Plot selection out of reach!");
    }
    ui->PlotComboBox->setEnabled(true);
}

//Calls the GNUPLot program for graph generation from the output
void MainWindow::GNUplot()
{
    QProcess process;
    ui->statusBar->showMessage("GNUplot running");
    QString OutFile=rootPath+OutputFileName;
    QString Commandlines="\"set terminal png;"
            "set title 'JSIM Plot';"
            "set xlabel 'Time(s)';"
            "set ylabel 'Output';"
            "set style data lines;"
            "plot "+OutFile+";\" >"+PlotPath+"/out.png";
    #ifdef __linux__
    QString plotcommand="gnuplot -e "+Commandlines;
    process.start("/bin/sh", QStringList() << "-c" << plotcommand);
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("gnuplot.exe");
    process.setArguments({"-e", Commandlines});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    QString plotcommand="gnuplot -e "+Commandlines;
    process.start("/bin/sh", QStringList() << "-c" << plotcommand);
    process.waitForFinished(-1); // will wait forever until finished
    #endif
    //QString OSname=QSysInfo::productType();
//    if (OSname=="windows"){
//        process.setProgram("gnuplot.exe");
//        process.setArguments({"-e", Commandlines});
//        process.setCreateProcessArgumentsModifier([] (
//            QProcess::CreateProcessArguments *args) {
//                        args->flags &= CREATE_NO_WINDOW;});
//        process.startDetached();
//    }
//    else{
//        QString plotcommand="gnuplot -e "+Commandlines;
//        process.start("/bin/sh", QStringList() << "-c" << plotcommand);
//        process.waitForFinished(-1); // will wait forever until finished
//    }
    QString plotstderr = process.readAllStandardError();
    ui->statusBar->showMessage(plotstderr);
    QString plotstdout = process.readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(plotstdout);
}

//Calls the XMGrace program for graph generation from the output
void MainWindow::XMGracePlot()
{
    QProcess process;
    ui->statusBar->showMessage("XMGrace running");
    QString OutFile=rootPath+OutputFileName;
    QString Commandlines="-nxy "+OutFile+" -legend load";
    //QString OSname=QSysInfo::productType();
    #ifdef __linux__
    QString plotcommand="xmgrace "+Commandlines;
    process.start("/bin/sh", QStringList() << "-c" << plotcommand);
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("qtgrace.exe");
    process.setArguments({Commandlines});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    QString plotcommand="xmgrace "+Commandlines;
    process.start("/bin/sh", QStringList() << "-c" << plotcommand);
    process.waitForFinished(-1); // will wait forever until finished
    #endif
//    if (OSname=="windows"){
//        process.setProgram("qtgrace.exe");
//        process.setArguments({Commandlines});
//        process.setCreateProcessArgumentsModifier([] (
//            QProcess::CreateProcessArguments *args) {
//                        args->flags &= CREATE_NO_WINDOW;});
//        process.startDetached();
//    }
//    else{
//        QString plotcommand="xmgrace "+Commandlines;
//        process.start("/bin/sh", QStringList() << "-c" << plotcommand);
//        process.waitForFinished(-1); // will wait forever until finished
//    }
    QString plotstderr = process.readAllStandardError();
    ui->statusBar->showMessage(plotstderr);
    QString plotstdout = process.readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(plotstdout);
}

// Load the netlist file and copies it to the Plain text editor
void MainWindow::LoadNetlist(QString NetlistFile)
{
    QFile file(NetlistFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    QByteArray dataNetlist = file.readAll();
    ui->NetlistPlainTextEdit->setPlainText(dataNetlist);
    file.close();
}

//Opens and closes the frame for the parameter editor on changing the type of simulation
//Sets Global variable for the simulation type
void MainWindow::on_TypeComboBox_currentIndexChanged(int index)
{
    //Make a table to set the Simindex to the write value:
    switch (index)
    {
    case 0:
        //Time domain calculation
        Simindex=0;
        break;
    case 1:
        //Parametric Analysis
        Simindex=2;
        break;
    case 2:
        //Temperatures Analysis
        Simindex=3;
        break;
    case 3:
        //BER
        Simindex=4;
        break;
    case 4:
        //Frequency Analysis
        Simindex=5;
        break;
    case 5:
        //I-V slow
        Simindex=1;
        break;
    case 6:
        //I-V fast
        Simindex=6;
        break;
    default:
        Simindex=0;
        break;
    }

    switch (Simindex)
    {
    case 0:
        ui->frame->hide();
        break;
    case 1:
        ui->frame->show();
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum current [A]");
        ui->label_10->setText("Maximum current [A]");
        ui->label_11->setText("Current source");
        ui->ParamLineEdit->setEnabled(true);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText("Number of cycles");
        ui->ParamLineEdit->setText("I0");
        ui->SubParamLineEdit->setText("1");
        ui->MinValLineEdit->setText("-2m");
        ui->MaxValLineEdit->setText("2m");
        ui->StepLineEdit->setText("40");
        ui->TempLineEdit->setText(QString::number(temperature));
        ui->SubParamLineEdit->setEnabled(true);
        ui->label_12->show();
        QMessageBox::information(this,"I-V characteristics","First output of the netlist is current and second output is the voltage."
                                                       " Other outputs are ignored.");
        break;
    case 2:
        ui->frame->show();
        ui->ParamLineEdit->setEnabled(true);
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum");
        ui->label_10->setText("Maximum");
        ui->label_11->setText("Parameter name");
        ui->label_12->setText("Sub-Parameter name");
        ui->SubParamLineEdit->setEnabled(true);
        ui->TempLineEdit->setEnabled(false);
        ui->ParamLineEdit->setText("I1");
        ui->SubParamLineEdit->setText("200GHz");
        ui->MinValLineEdit->setText("100G");
        ui->MaxValLineEdit->setText("500G");
        ui->StepLineEdit->setText("5");
        ui->TempLineEdit->setText(QString::number(temperature));
        QMessageBox::information(this,"Parameter analysis","Enter the name of the entity (Ex. I1) to analyze in the parameter name field."
                                                       " The sub-parameter is one of the variables of the entity (Ex.200GHz).");
        break;
    case 3:
        ui->frame->show();
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum temperature");
        ui->label_10->setText("Maximum temperature");
        ui->label_11->setText(" ");
        ui->ParamLineEdit->setEnabled(false);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText(" ");
        ui->ParamLineEdit->setText(" ");
        ui->SubParamLineEdit->setText(" ");
        ui->MinValLineEdit->setText("1");
        ui->MaxValLineEdit->setText("7");
        ui->StepLineEdit->setText("6");
        ui->TempLineEdit->setText(" ");
        ui->SubParamLineEdit->setEnabled(false);
        break;

        //Continue from here
    case 4:
        ui->frame->show();
        ui->ParamLineEdit->setEnabled(true);
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Multiplier");
        ui->label_9->setText("Minimum value");
        ui->label_10->setText("Maximum value");
        ui->label_11->setText("Name of element");
        ui->label_12->setText("Nominal value of element");
        ui->SubParamLineEdit->setEnabled(true);
        ui->TempLineEdit->setEnabled(true);
        ui->ParamLineEdit->setText("VBias");
        ui->SubParamLineEdit->setText("2.5mV");
        ui->MinValLineEdit->setText("1m");
        ui->MaxValLineEdit->setText("5m");
        ui->StepLineEdit->setText("40");
        ui->TempLineEdit->setText("2");
        QMessageBox::information(this,"Bit error rate calculation","Bit error rate simulations are only for circuits with periodic input and output. "
                                         "For more complex circuits, use the \"Optimizer and margin analyzer tool\".");
        break;
    case 5:
        ui->frame->show();
        ui->ParamLineEdit->setEnabled(true);
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Multipliers");
        ui->label_9->setText("Minimum frequency");
        ui->label_10->setText("Maximum frequency");
        ui->label_11->setText("Input element");
        ui->label_12->setText("Input node");
        ui->SubParamLineEdit->setEnabled(true);
        ui->TempLineEdit->setEnabled(true);
        ui->ParamLineEdit->setText("XI1");
        ui->SubParamLineEdit->setText("11");
        ui->MinValLineEdit->setText("10G");
        ui->MaxValLineEdit->setText("200G");
        ui->StepLineEdit->setText("20");
        ui->TempLineEdit->setText("1,2,2");
        QMessageBox::information(this,"Frequency Response","This function applies a pulse train at the input node of the circuit netlist and measures the number of output pulses."
                                                           " Enter in the parameter panel :  \n"
                                                           "- node number of pulse train generator,\n"
                                                           "- pulse train minimum and maximum frequencies,\n"
                                                           "- number of steps.\n"
                                                           "- multipliers separated by commas (Ex : 1,4,2) for one input and 2 outputs."
                                                           " Multipliers are the number of input pulses divided by the number of expected output pulses. \n"
                                                           " Add in the netlist the phase or voltage of the input and output junctions to be monitored.\n"
                                                           " Example: .PRINT DEVV B1_X1 (the first line is always the input)\n"
                                                           "             .PRINT PHASE B2_X2 (the other lines are always the outputs)");
        break;
    case 6:
        ui->frame->show();
        ui->label_7->setText("Averaging window size");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum current value [A]");
        ui->label_10->setText("Maximum current value [A]");
        ui->label_11->setText("Current source");
        ui->ParamLineEdit->setEnabled(true);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText("Number of cycles");
        ui->ParamLineEdit->setText("I0");
        ui->SubParamLineEdit->setText("1");
        ui->MinValLineEdit->setText("-1m");
        ui->MaxValLineEdit->setText("1m");
        if (ui->actionIV_Curve_Statistical_Noise->isChecked())
            ui->StepLineEdit->setText("3000");
        else
            ui->StepLineEdit->setText("1000");
        ui->TempLineEdit->setText(QString::number(temperature));
        ui->SubParamLineEdit->setEnabled(true);
        ui->label_12->show();
        QMessageBox::information(this,"I-V characteristics","First output of the netlist is current and second output is the voltage."
                                                       " Other outputs are ignored. This method is using a moving average :\n"
                                                       "it is faster but inaccurate in presence of slow oscillations (at low voltages).");
        break;

    default:
        break;
    }

}

//Save the changes made in the Plain text edit to the Netlist
void MainWindow::on_SaveNetListPushBottun_clicked()
{
    QString NetlistFile = ui->NetlistLineEdit->text();
    QFile file(NetlistFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)){
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    else{
            QTextStream stream(&file);
            stream << ui->NetlistPlainTextEdit->toPlainText();
            QMessageBox::information(this,"Success","File is saved!");
            file.close();
            stream.flush();
    }
}

void MainWindow::on_ClearTerminalpushButton_clicked()
{
    ui->TerminalPlainTextEdit->clear();
}


//Copies the output of the simulations in the user defined folder
void MainWindow::Copy_File(QString outfilepath, QString OutFile)
{

    if (QFile::exists(outfilepath))
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Copy", "File already exist, replace?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
            QFile::remove(outfilepath);
            QFile::copy(OutFile, outfilepath);
          } else QMessageBox::warning(this,"Error!","Output data is not saved!");
    }else{
        QFile::copy(OutFile, outfilepath);
    }


}

//Read the parameters from the setting box
struct SimParams MainWindow::readSimParams()
{
    return {ui->ParamLineEdit->text(),ui->SubParamLineEdit->text(),ui->MinValLineEdit->text(),
                ui->MaxValLineEdit->text(),ui->StepLineEdit->text(),ui->TempLineEdit->text()};
}

//Opens the PDF file called Manual.pdf placed in the /Data folder
//Uses Adope acrobat reader ActiveX
void MainWindow::on_TemplateNetListPushBottun_clicked()
{
    #ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JSIM-manual-v12.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
    #elif _WIN32
        QDialog *help_object2 = new QDialog();
        help_object2->setWindowTitle("User Manual");
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        help_object2->setFixedWidth(screenGeometry.width()/2);
        help_object2->setFixedHeight(screenGeometry.height()*4/5);
        QAxWidget *pdf2= new QAxWidget();
        pdf2->setParent(help_object2);
        pdf2->setFixedWidth(screenGeometry.width()/2);
        pdf2->setFixedHeight(screenGeometry.height()*4/5);
        pdf2->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
        if(!pdf2->setControl("Adobe PDF Reader"))
            QMessageBox::critical(this, "Error", "Make sure you have Adobe Reader (and its ActiveX) installed!");
        QString callcommand="LoadFile(\""+rootPath+"/Help/JSIM-manual-v12.0.pdf"+"\")";
        pdf2->dynamicCall(callcommand.toStdString().c_str());
        help_object2->exec();
    #else

        QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JSIM-manual-v12.0.pdf"));
        //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
    #endif
}

void MainWindow::on_actionJSIM_manual_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JSIM-manual-v12.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
#elif _WIN32
    QDialog *help_object2 = new QDialog();
    help_object2->setWindowTitle("User Manual");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    help_object2->setFixedWidth(screenGeometry.width()/2);
    help_object2->setFixedHeight(screenGeometry.height()*4/5);
    QAxWidget *pdf2= new QAxWidget();
    pdf2->setParent(help_object2);
    pdf2->setFixedWidth(screenGeometry.width()/2);
    pdf2->setFixedHeight(screenGeometry.height()*4/5);
    pdf2->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
    if(!pdf2->setControl("Adobe PDF Reader"))
        QMessageBox::critical(this, "Error", "Make sure you have Adobe Reader (and its ActiveX) installed!");
    QString callcommand="LoadFile(\""+rootPath+"/Help/JSIM-manual-v12.0.pdf"+"\")";
    pdf2->dynamicCall(callcommand.toStdString().c_str());
    help_object2->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JSIM-manual-v12.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
#endif
}

void MainWindow::on_actionJ_oSIM_manual_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JoSIM-ReadMe.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
#elif _WIN32
    QDialog *help_object2 = new QDialog();
    help_object2->setWindowTitle("User Manual");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    help_object2->setFixedWidth(screenGeometry.width()/2);
    help_object2->setFixedHeight(screenGeometry.height()*4/5);
    QAxWidget *pdf2= new QAxWidget();
    pdf2->setParent(help_object2);
    pdf2->setFixedWidth(screenGeometry.width()/2);
    pdf2->setFixedHeight(screenGeometry.height()*4/5);
    pdf2->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
    if(!pdf2->setControl("Adobe PDF Reader"))
        QMessageBox::critical(this, "Error", "Make sure you have Adobe Reader (and its ActiveX) installed!");
    QString callcommand="LoadFile(\""+rootPath+"/Help/JoSIM-ReadMe.pdf"+"\")";
    pdf2->dynamicCall(callcommand.toStdString().c_str());
    help_object2->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JoSIM-ReadMe.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
#endif
}

//Clear Everything and reset to the new condition
void MainWindow::on_MenuNew_Simulation_triggered()
{
    ui->SimComboBox->setCurrentIndex(0);
    ui->NetlistLineEdit->clear();
    ui->NetlistPlainTextEdit->clear();
    ui->TerminalPlainTextEdit->clear();
    ui->NoiseCheckBox->setChecked(false);
    ui->TypeComboBox->setCurrentIndex(0);
    ui->ParamLineEdit->clear();
    ui->SubParamLineEdit->clear();
    ui->MinValLineEdit->clear();
    ui->MaxValLineEdit->clear();
    ui->StepLineEdit->clear();
    ui->TempLineEdit->clear();
    ui->PlotCheckBox->setChecked(false);
    ui->PlotComboBox->setCurrentIndex(0);
    ui->SaveLineEdit->clear();
    ui->SaveCheckBox->setChecked(false);
    ui->ProgressBar->setValue(0);
    ui->frame->close();
    OutputFileName="/OUT.DAT";
    delimator=" ";
    RawData.clear();
    columNum=1;
}

//Saves the state of the main window in a file named "STATE.TMP"

void MainWindow::on_MenuSave_triggered()
{
    QStringList savematerial={};
    savematerial.append(QString::number(ui->SimComboBox->currentIndex()));
    savematerial.append(ui->NetlistLineEdit->text());
    savematerial.append(QString::number(ui->NoiseCheckBox->isChecked()));
    savematerial.append(QString::number(ui->TypeComboBox->currentIndex()));

    savematerial.append(ui->ParamLineEdit->text());
    savematerial.append(ui->SubParamLineEdit->text());
    savematerial.append(ui->MinValLineEdit->text());
    savematerial.append(ui->MaxValLineEdit->text());
    savematerial.append(ui->StepLineEdit->text());
    savematerial.append(ui->TempLineEdit->text());

    savematerial.append(QString::number(ui->PlotCheckBox->isChecked()));
    savematerial.append(QString::number(ui->PlotComboBox->currentIndex()));
    savematerial.append(ui->SaveLineEdit->text());
    savematerial.append(QString::number(ui->SaveCheckBox->isChecked()));

    savematerial.append(OutputFileName);
    savematerial.append(delimator);
    savematerial.append(QString::number(columNum));

    QFile file("STATE.TMP");
    if (!file.open(QFile::WriteOnly | QFile::Text)){
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    else{
            QTextStream stream(&file);
            for (int fcntr=0; fcntr<savematerial.size();fcntr++)
                stream << savematerial.at(fcntr)<<'\n';
            file.close();
            stream.flush();
    }
}

//Recall the file "STATE.TMP" and if the file exists, loads the state of the simulation
void MainWindow::on_MenuOpen_triggered()
{
    QStringList loadmaterial={};
    QFile file("STATE.TMP");
    if (!file.open(QFile::ReadOnly | QFile::Text)){
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    else{
        QTextStream stream(&file);
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            loadmaterial.append(line);
        }
        file.close();
        stream.flush();
        ui->SimComboBox->setCurrentIndex(loadmaterial.at(0).toInt());
        ui->NetlistLineEdit->setText(loadmaterial.at(1));
        LoadNetlist(loadmaterial.at(1));
        ui->NoiseCheckBox->setChecked(loadmaterial.at(2).toInt());
        ui->TypeComboBox->setCurrentIndex(loadmaterial.at(3).toInt());


        ui->ParamLineEdit->setText(loadmaterial.at(4));
        ui->SubParamLineEdit->setText(loadmaterial.at(5));
        ui->MinValLineEdit->setText(loadmaterial.at(6));
        ui->MaxValLineEdit->setText(loadmaterial.at(7));
        ui->StepLineEdit->setText(loadmaterial.at(8));
        ui->TempLineEdit->setText(loadmaterial.at(9));

        ui->PlotCheckBox->setChecked(loadmaterial.at(10).toInt());
        ui->PlotComboBox->setCurrentIndex(loadmaterial.at(11).toInt());
        ui->SaveLineEdit->setText(loadmaterial.at(12));
        ui->SaveCheckBox->setChecked(loadmaterial.at(13).toInt());

        OutputFileName=loadmaterial.at(14);
        delimator=loadmaterial.at(15);
        columNum=loadmaterial.at(15).toInt();
        Simindex=loadmaterial.at(3).toInt();
    }

}


void MainWindow::on_SimComboBox_currentIndexChanged(int index)
{
    if (index==0)
    {
        OutputFileName = "/OUT.DAT";
        TempFileName = rootPath+"/Data/ModiNL.cir";
        delimator = " ";
    }
    else
    {
        OutputFileName = "/OUT.csv";
        TempFileName = rootPath+"/Data/ModiNL.js";
        delimator = ",";
    }
}

void MainWindow::on_actionManuals_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JOINUS manual or go to the website: http//:www..com .");
#elif _WIN32
    QDialog *help_object = new QDialog();
    help_object->setWindowTitle("User Manual");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    help_object->setFixedWidth(screenGeometry.width()/2);
    help_object->setFixedHeight(screenGeometry.height()*4/5);
    QAxWidget *pdf= new QAxWidget();
    pdf->setParent(help_object);
    pdf->setFixedWidth(screenGeometry.width()/2);
    pdf->setFixedHeight(screenGeometry.height()*4/5);
    pdf->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
    if(!pdf->setControl("Adobe PDF Reader"))
        QMessageBox::critical(this, "Error", "Make sure you have Adobe Reader (and its ActiveX) installed!");
    QString callcommand="LoadFile(\""+rootPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"+"\")";
    pdf->dynamicCall(callcommand.toStdString().c_str());
    help_object->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JOINUS manual or go to the website: http//:www..com .");
#endif
}


//void MainWindow::on_action_Open_Console_triggered()
//{
//    QProcess OpTer;
//    ui->TerminalPlainTextEdit->clear();
//    ui->TerminalPlainTextEdit->appendPlainText(">>");
//    ui->TerminalPlainTextEdit->setFocus();
//    ui->TerminalPlainTextEdit->setFocusPolicy(Qt::StrongFocus);
//    //ui->TerminalPlainTextEdit->installEventFilter(this);

//    #ifdef __linux__
//        //linux code goes here
//    #elif _WIN32

//    #else

//    #endif

//}

//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    if( (event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
//            ui->TerminalPlainTextEdit->appendPlainText("Enter Key pushed!");
//    else
//            ui->TerminalPlainTextEdit->appendPlainText("Enter Key NOT pushed!");
//}


//void MainWindow::on_TerminalPlainTextEdit_textChanged()
//{

//}

void MainWindow::on_actionFont_Properties_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok);
    ui->NetlistPlainTextEdit->setFont(font);
    ui->TerminalPlainTextEdit->setFont(font);
}

void MainWindow::on_actionRun_Custom_Plotter_triggered()
{
    dialogPlot = new DialogPlot(this);
    dialogPlot->show();
}

// This function would get the value of the temperature from user
void MainWindow::on_actionSet_Temperature_triggered()
{
    bool ok;
    bool validData1=true;
    bool validData2=true;
    QString tempValString = QInputDialog::getText(  this,  tr("Temperature Value"),
               tr("Enter the value of the temperature for simulation."),
               QLineEdit::Normal, "", &ok );
    if( ok && !tempValString.isEmpty() )
    {
        validData2=tempValString.toDouble(&validData1)>=0;
        if (!validData1 || !validData2)
        {
            QMessageBox::warning(this,"Error!","Temperature should be real number bigger or equal to zero!");
            return;
        }
            temperature = tempValString.toDouble();
            if (Simindex!=5 && Simindex!=4)
                ui->TempLineEdit->setText(QString::number(temperature));
    }
    else
    {
        temperature = 4.2;
        if (Simindex!=5 && Simindex!=4)
            ui->TempLineEdit->setText("4.2");
    }
}

void MainWindow::on_actionChange_background_color_triggered()
{
    QPalette pal = this->palette();
    QColorDialog *dialog = new QColorDialog(this);

    //QLinearGradient gradient(0, 0, 0, 400);
    //gradient.setColorAt(0, QColor(50, 50, 50));
    //gradient.setColorAt(0.38, dialog->getColor());
    //gradient.setColorAt(1, QColor(200, 200, 200));

    //pal.setBrush(QPalette::Window, QBrush(gradient));
    pal.setColor(QPalette::Window, dialog->getColor());
    this->setPalette(pal);

}


void MainWindow::on_action_WSIM_triggered()
{
    QProcess process;
    #ifdef __linux__
    process.start("./Add-ons/Wsim");
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("cmd.exe");
    process.setArguments({"/c", WSIMPath+"/Wsim.exe"});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    process.start(rootPath+"/Add-ons/Wsim");
    process.waitForFinished(-1); // will wait forever until finished
    #endif
//    QString OSname=QSysInfo::productType();
//    if (OSname=="windows"){
//        process.setProgram("cmd.exe");
//        process.setArguments({"/c", WSIMPath+"/Wsim.exe"});
//        process.setCreateProcessArgumentsModifier([] (
//            QProcess::CreateProcessArguments *args) {
//                        args->flags &= CREATE_NO_WINDOW;});
//        process.startDetached();
//    }
//    else{
//        process.start("./WSIM/Wsim");
//        process.waitForFinished(-1); // will wait forever until finished
//    }

    QString stderrWsim = process.readAllStandardError();
    ui->statusBar->showMessage(stderrWsim);
    QString stdmssWsim = process.readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssWsim);
}

void MainWindow::on_actionSQUID_MAP_triggered()
{
    QProcess process;

    #ifdef __linux__
    process.start("./Add-ons/SMP");
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("cmd.exe");
    process.setArguments({"/c", WSIMPath+"/SMP.exe"});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    process.start(rootPath+"/Add-ons/SMP");
    process.waitForFinished(-1); // will wait forever until finished
    #endif
//    QString OSname=QSysInfo::productType();
//    if (OSname=="windows"){
//        process.setProgram("cmd.exe");
//        process.setArguments({"/c", WSIMPath+"/SMP.exe"});
//        process.setCreateProcessArgumentsModifier([] (
//            QProcess::CreateProcessArguments *args) {
//                        args->flags &= CREATE_NO_WINDOW;});
//        process.startDetached();
//    }
//    else{
//        process.start("./WSIM/SMP");
//        process.waitForFinished(-1); // will wait forever until finished
//    }

    QString stderrSMP = process.readAllStandardError();
    ui->statusBar->showMessage(stderrSMP);
    QString stdmssSMP = process.readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssSMP);
}

void MainWindow::on_actionMAR_GINOPT_triggered()
{
    QProcess process;

    #ifdef __linux__
    process.start("./Add-ons/MCO");
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("cmd.exe");
    process.setArguments({"/c", WSIMPath+"/MCO.exe"});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    process.start(rootPath+"/Add-ons/MCO");
    process.waitForFinished(-1); // will wait forever until finished
    #endif
//    QString OSname=QSysInfo::productType();
//    if (OSname=="windows"){
//        process.setProgram("cmd.exe");
//        process.setArguments({"/c", WSIMPath+"/MCO.exe"});
//        process.setCreateProcessArgumentsModifier([] (
//            QProcess::CreateProcessArguments *args) {
//                        args->flags &= CREATE_NO_WINDOW;});
//        process.startDetached();
//    }
//    else{
//        process.start("./WSIM/SMP");
//        process.waitForFinished(-1); // will wait forever until finished
//    }

    QString stderrMCO = process.readAllStandardError();
    ui->statusBar->showMessage(stderrMCO);
    QString stdmssMCO = process.readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssMCO);
}

//On exiting program, destroys all the objects and files created by the code.
MainWindow::~MainWindow()
{
    //Find the temporary file list and delete them all

    //QFile file(TemporaryFiles.tmp);
    //QStringList fields;

    //if (!file.open(QFile::ReadOnly | QFile::Text))
    //{
    //    return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    //}

    //    while(!in.atEnd()) {
    //        QString line = in.readLine();
    //    if (QFile::exists(line))
    //       QFile::remove(line);
    //    }
    //    file.close();
    //    in.flush();


    QFile file(TempFileName);
    file.remove();
    file.close();
    QFile file1(TempOutFile);
    file1.remove();
    file1.close();
    delete ui;
}

void MainWindow::on_actionAUTO5_triggered()
{
    ui->StartPushButton->setEnabled(false);
        autooptim = new AutoOptim(this);
        autooptim->setModal(true);
        QString OptimFile=rootPath+"/OptimConfigFile.tmp";

        AutoOptim *autofuns=new AutoOptim;

        if(autooptim->exec() == QDialog::Accepted)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Optimizer & Margin Analyzer", "Start Optimization process?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {

                ui->ProgressBar->setValue(0);
                //Start the optimizatioon process here

                //Reading the values
                QStringList OptimParams=autofuns->ReadOptimFile(OptimFile);

                //Act on error in configuration
                if (OptimParams.at(0)=="error")
                {
                    QMessageBox::warning(this,"Error!","Optimizer cannot continue!");
                    ui->StartPushButton->setEnabled(true);
                    return;
                }

                //Run the function for selected mode

                //Run function for Bias Margin calculation
                else if (OptimParams.at(0)=="MS")
                {
                    Simindex=10;
                    ui->ProgressBar->setValue(20);

                    //This commands run the Margin Calculation
                    autofuns->MarginSimulation(OptimParams);

                    //Initializing the parameters
                    ui->ProgressBar->setValue(70);
                    QString DataFile=rootPath+OutputFileName;
                    QString MSOUT = OptimParams.at(5)+"/MSOUTPUT.DAT";
                    QString MSSTAT = OptimParams.at(5)+"/MSSTATUES.DAT";

                    QFile filestatues(MSSTAT);
                    if (!filestatues.exists())
                    {
                        QMessageBox::warning(this,"Error!","Optimizer didn't run correctly!");
                    }
                    else if (!filestatues.open(QFile::ReadOnly | QFile::Text))
                    {
                        QMessageBox::warning(this,"Error!","Cannot open statue file, check the permissions!");
                    }else{
                        QTextStream instat(&filestatues);
                        while(!instat.atEnd()) {
                            QString statline = instat.readLine();
                            ui->TerminalPlainTextEdit->appendPlainText(statline);
                        }
                        filestatues.close();
                        instat.flush();
                    }

                    QFile::remove(DataFile);
                    QFile::copy(MSOUT,DataFile);


                }



                //Run function for Optimization (This method uses center of gravity)
                else if (OptimParams.at(0)=="MCO")
                {

                    // Define Parameters for the Calculation
                    Simindex=10;
                    int IterationNumber = OptimParams.at(7).toInt();
                    double CritCurrent=1e-4;
                    QString DataFile=rootPath+OutputFileName;
                    QString MSOUT = OptimParams.at(5)+"/MSOUTPUT.DAT";
                    QString MSSTAT = OptimParams.at(5)+"/MSSTATUES.DAT";
                    QString tempNetlistNominal=OptimParams.at(4)+"/MSNominal.cir";
                    QString ParamFile=OptimParams.at(3);

                    autofuns->MarginSimulation(OptimParams);

                    //Write the result in terminal
                    QFile filestatues(MSSTAT);
                    if (!filestatues.exists())
                    {
                        QMessageBox::warning(this,"Error!","Optimizer didn't run correctly!");
                    }
                    else if (!filestatues.open(QFile::ReadOnly | QFile::Text))
                    {
                        QMessageBox::warning(this,"Error!","Cannot open statue file, check the permissions!");
                    }else{
                        int linecount=0;
                        QTextStream instat(&filestatues);
                        while(!instat.atEnd()) {
                            QString statline = instat.readLine();
                            if (linecount==0 && statline!="Success")
                            {
                                ui->TerminalPlainTextEdit->appendPlainText(statline);
                                return;
                            }else
                            {
                                if (linecount==1)
                                    CritCurrent=statline.toDouble();

                                else
                                ui->TerminalPlainTextEdit->appendPlainText(statline);

                                linecount++;
                            }

                        }
                        filestatues.close();
                        instat.flush();
                    }

                    for (int IterCntr=0; IterCntr<IterationNumber;IterCntr++)
                    {

                        //Write new parameter file based on the output
                        QString tempMSOUT = OptimParams.at(5)+"/MSOUTPUT"+QString::number(IterCntr)+".DAT";
                        QString tempParamFile=OptimParams.at(4)+"/tempParamFile"+QString::number(IterCntr)+".para";
                        QFile::remove(tempMSOUT);
                        QFile::copy(MSOUT,tempMSOUT);


                        QVector<QVector<QString>> oldParamVector =  autofuns->paramDataArrange(ParamFile);


                        //Change the ParamFile -> OptimParams.at(3) value to match new parametric file.
                        QStringList newParamValues = autofuns->newParamGen(tempMSOUT, oldParamVector, CritCurrent, OptimParams.at(11).toDouble());
                        autofuns->newParamFileGen(ParamFile, tempParamFile ,newParamValues);
                        OptimParams[3]=tempParamFile;
                        ParamFile=tempParamFile;

                        ui->ProgressBar->setValue(100*((IterCntr+1)/IterationNumber));

                        autofuns->MarginSimulation(OptimParams);


                        qDebug()<< "Iteration Number: " <<QString::number(IterCntr+1)<<'\n';

                        //Write the result in terminal
                        QFile fileStatues(MSSTAT);

                        if (!fileStatues.exists())
                        {
                            QMessageBox::warning(this,"Error!","Optimizer didn't run correctly!");
                        }
                        else if (!fileStatues.open(QFile::ReadOnly | QFile::Text))
                        {
                            QMessageBox::warning(this,"Error!","Cannot open statue file, check the permissions!");
                        }
                        else {
                            int linecount=0;
                            QTextStream instat2(&fileStatues);
                            while(!instat2.atEnd()) {
                                QString statline = instat2.readLine();
                                    if (linecount!=1)
                                        ui->TerminalPlainTextEdit->appendPlainText(statline);
                                    linecount++;
                            }
                            fileStatues.close();
                            instat2.flush();
                        }
                    }


                    qDebug() << "End of simulation" << '\n';

                        QVector<QVector<QString>> displayParamVector =  autofuns->paramDataArrange(ParamFile);
                        for (int dispCntr=0;dispCntr<displayParamVector.length();dispCntr++)
                        {
                            QString displayLine=displayParamVector.at(dispCntr).at(2)+" = "+displayParamVector.at(dispCntr).at(1);
                            ui->TerminalPlainTextEdit->appendPlainText(displayLine);
                        }

                        QFile::remove(DataFile);
                        QFile::copy(MSOUT,DataFile);
                        //for (int mydelay=0;mydelay<1000;mydelay++){}

                }


                //Run function for Yield calculation
                else if (OptimParams.at(0)=="YA")
                {

                    QMessageBox::warning(this,"YeeeHaaaa","Yield Analyzer is running");



                }

                //End Optim process here
                ui->ProgressBar->setValue(100);

                dialogPlot = new DialogPlot(this);
                dialogPlot->show();
            }
            ui->StartPushButton->setEnabled(true);
        }else
        {
            ui->StartPushButton->setEnabled(true);
        }

        int index = ui->TypeComboBox->currentIndex();
        switch (index)
        {
        case 0:
            //Time domain calculation
            Simindex=0;
            break;
        case 1:
            //Parametric Analysis
            Simindex=2;
            break;
        case 2:
            //Temperatures Analysis
            Simindex=3;
            break;
        case 3:
            //BER
            Simindex=4;
            break;
        case 4:
            //Frequency Analysis
            Simindex=5;
            break;
        case 5:
            //I-V slow
            Simindex=1;
            break;
        case 6:
            //I-V fast
            Simindex=6;
            break;
        default:
            Simindex=0;
            break;
        }

}
