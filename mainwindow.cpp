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



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{  
    ui->setupUi(this);

    // Initialize the values for the input and output files
    ui->SaveLineEdit->setText(DataPath+"/Out.dat");
    ui->NetlistLineEdit->setText(DataPath+"/si.inp");
    ui->frame->close();
    QString NetlistFile = ui->NetlistLineEdit->text();

    // Set text editor font
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);
    ui->NetlistPlainTextEdit->setFont(font);
    font.setFamily("Arial");
    font.setPointSize(8);
    ui->TerminalPlainTextEdit->setFont(font);

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

    // ConsoleOutputs gives the outputs and errors of the terminal. In windows it is Command Prompt.
    struct ConsoleOutputs simout={"",""};

    // SimParams are parameters that are set for the simulation. Full defenition in the header file.
    struct SimParams simParams=readSimParams();
    CalcVals *calcVal=new CalcVals;

    // Testing to see if the entered parameters are legit

    bool validData1=true;
    bool validData2=true;
    validData2=simParams.pointNum.toInt(&validData1)>0;
    if (!validData1 || !validData2)
    {
        QMessageBox::warning(this,"Error!","Number of the points should be positive integer!");
        return;
    }

    validData2=simParams.tempVal.toDouble(&validData1)>=0;

    if ((!validData1 || !validData2) && Simindex!=3)
    {
        QMessageBox::warning(this,"Error!","Temperature should be real number bigger or equal to zero!");
        return;
    }
    if (Simindex==3)
        temperature=4.2;
    else
        temperature = simParams.tempVal.toDouble();

    int SimulatorIndex=ui->SimComboBox->currentIndex();
    bool noise= ui->NoiseCheckBox->isChecked();
    QString NetlistFile=ui->NetlistLineEdit->text();
    ui->ProgressBar->setValue(0);


    // *** new netlist ***
    // Make a netlist without comments with changes for temperature and noise made to it
    QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex);
    if (mnnerr!="Success"){
        QMessageBox::warning(this,"Error!",mnnerr);
        return;
    }

    // set the values for the simulation and give information about it
        ui->SimComboBox->setEnabled(false);
        ui->TypeComboBox->setEnabled(false);
        ui->StartPushButton->setEnabled(false);
        ui->TerminalPlainTextEdit->appendPlainText("Output file name: "+QDir::currentPath()+OutputFileName);
        ui->TerminalPlainTextEdit->appendPlainText("Column Number: "+QString::number(columNum));


        QString noisecond=ui->NoiseCheckBox->isChecked()?"On":"off";
        ui->TerminalPlainTextEdit->appendPlainText("Noise Simulation: "+noisecond);
        ui->TerminalPlainTextEdit->appendPlainText("Temperature value:"+  QString::number(temperature));
        ui->TerminalPlainTextEdit->appendPlainText("Delimator: '"+  delimator+"'");


        QString initSubParam=simParams.subParam;
        double stepSize=0;
        titleVals.clear();

        switch (Simindex)
        {
            // Normal time domain simulation
        case 0:

            ui->ProgressBar->setValue(50);
            simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);
            ui->ProgressBar->setValue(100);
            break;


            // I-V simulation results
        case 1:

            ui->ProgressBar->setValue(50);
            simout = simclass->simulateivcurve(TempFileName,SimulatorIndex,simParams.pointNum.toInt(),simParams.subParam.toInt());
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
            ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

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
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);
                else{
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=QDir::currentPath()+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
                    QFile::remove(Outtemp);

                    bool whileLoopLogic=QFile::copy(QDir::currentPath()+OutputFileName, Outtemp);

                    while (!whileLoopLogic){}

                }

              }

            break;

        case 3:
            simParams.subParam="4.2";


            if (simParams.pointNum.toInt()>1)
                stepSize=(calcVal->convertToValues(simParams.maxVal)-
                          calcVal->convertToValues(simParams.minVal))/(simParams.pointNum.toInt()-1);

            for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
            {
                temperature=calcVal->convertToValues(simParams.minVal)+simstep*stepSize;
                simParams.tempVal=QString::number(temperature);

                //Change the netlist with the new parameter
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);

                else{
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=QDir::currentPath()+"/Data/Tmp"+QString::number(simstep)+"K.dat";
                    QFile::remove(Outtemp);
                   bool whileLoopLogic=QFile::copy(QDir::currentPath()+OutputFileName, Outtemp);
                   while (!whileLoopLogic){}
                }

              }

            break;

        case 4:

            QMessageBox::warning(this,"Sorry!","For BER calculation, use the MC Optimizer program!");
            break;

        default:
            ui->statusBar->showMessage("The simulation type is not valid!");
            break;
        }

        ui->SimComboBox->setEnabled(true);
        ui->TypeComboBox->setEnabled(true);
        if (ui->SaveCheckBox->isChecked()==true)
            Copy_File(ui->SaveLineEdit->text(),QDir::currentPath()+OutputFileName);

        // Testing the temperatrue:
        // ***Delete afterward***
//       ui->TerminalPlainTextEdit->appendPlainText(QString::number(titleVals.length())+"    "+simParams.pointNum);
//        for (int simstep=0;simstep<titleVals.length()/2;simstep++){
//             QString DataFileTest = QDir::currentPath()+"/Data/Tmp"+QString::number(simstep)+"K.dat"+"      "+titleVals.at(simstep*2+1)+" "+titleVals.at(simstep*2);
//              ui->TerminalPlainTextEdit->appendPlainText(DataFileTest);
//       }
        // ***Delete afterward***


        if (ui->PlotCheckBox->isChecked()==true)
            plotNetlist();

        ui->StartPushButton->setEnabled(true);

}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About SimGUI","This program is generated in Savoie Mont Blanc University as part of the ColdFlux Project.");
}

// Load the Werthamer Model program
void MainWindow::on_WSIMPushBottun_clicked()
{
    QProcess process;
    #ifdef __linux__
    process.start("./WSIM/Wsim");
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("cmd.exe");
    process.setArguments({"/c", WSIMPath+"/Wsim.exe"});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    process.start("./WSIM/Wsim");
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

// Load the SQUID Map program
void MainWindow::on_SQUIDPushBottun_clicked()
{
    QProcess process;

    #ifdef __linux__
    process.start("./WSIM/SMP");
    process.waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process.setProgram("cmd.exe");
    process.setArguments({"/c", WSIMPath+"/SMP.exe"});
    process.setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process.startDetached();
    #else
    process.start("./WSIM/SMP");
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

// Load the netlist file and copies it to the Plain text editor
// ***Does not compile netlist for now***
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
    QString OutFile=QDir::currentPath()+OutputFileName;
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
    QString OutFile=QDir::currentPath()+OutputFileName;
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
    Simindex = index;
    switch (index)
    {
    case 0:
        ui->frame->hide();
        break;
    case 1:
        ui->frame->show();
        ui->label_7->setText("Number of Steps");
        ui->label_8->setText("Temperature");
        ui->TempLineEdit->setEnabled(true);
        ui->label_9->setText("Minimum");
        ui->label_10->setText("Maximum");
        ui->label_11->setText("Current Source");
        ui->ParamLineEdit->setEnabled(true);
        ui->label_12->setText("Loop Count");
        ui->ParamLineEdit->setText("I0");
        ui->SubParamLineEdit->setText("1");
        ui->MinValLineEdit->setText("-2m");
        ui->MaxValLineEdit->setText("2m");
        ui->StepLineEdit->setText("40");
        ui->TempLineEdit->setText("4.2");
        ui->SubParamLineEdit->setEnabled(true);
        ui->label_12->show();
        QMessageBox::information(this,"I-V characteristic","For I-V measurement, the program takes first output as Current and Second "
                                                       "as Voltage. The other outputs would be ignored!"
                                                       " See examples for more information.");
        break;
    case 2:
        ui->frame->show();
        ui->ParamLineEdit->setEnabled(true);
        ui->label_7->setText("Number of Steps");
        ui->label_8->setText("Temperature");
        ui->TempLineEdit->setEnabled(true);
        ui->label_9->setText("Minimum");
        ui->label_10->setText("Maximum");
        ui->label_11->setText("Parameter");
        ui->label_12->setText("Sub-Parameter");
        ui->SubParamLineEdit->setEnabled(true);
        ui->ParamLineEdit->setText("I1");
        ui->SubParamLineEdit->setText("200GHz");
        ui->MinValLineEdit->setText("100G");
        ui->MaxValLineEdit->setText("500G");
        ui->StepLineEdit->setText("5");
        ui->TempLineEdit->setText("4.2");
        QMessageBox::information(this,"Parameter Shift","For parametric shift, in the main parameter part, enter the name of your object (Ex.I1)."
                                                       "In sub-parameter window enter the object part that you want to change (Ex.200GHz)."
                                                       " See examples for more information.");
        break;
    case 3:
        ui->frame->show();
        ui->label_7->setText("Number of Steps");
        ui->label_8->setText("Temperature");
        ui->TempLineEdit->setEnabled(false);
        ui->label_9->setText("Minimum");
        ui->label_10->setText("Maximum");
        ui->label_11->setText("Temperature");
        ui->ParamLineEdit->setEnabled(false);
        ui->label_12->setText("Sub-Parameter");
        ui->ParamLineEdit->setText("Temperature");
        ui->SubParamLineEdit->setText(" ");
        ui->MinValLineEdit->setText("1");
        ui->MaxValLineEdit->setText("7");
        ui->StepLineEdit->setText("6");
        ui->TempLineEdit->setText(" ");
        ui->SubParamLineEdit->setEnabled(false);
        break;
    case 4:
        QMessageBox::information(this,"Parameter Shift","This part would be functional in next version!");
        ui->frame->hide();
        break;
    default:
        QMessageBox::warning(this,"Error","How?!");
        Simindex=0;
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
        QMessageBox::information(this,"Please see the attached JSIM manual or go to the website: http//:www..com .");
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
        QString callcommand="LoadFile(\""+QDir::currentPath()+"/Help/Manual.pdf"+"\")";
        pdf2->dynamicCall(callcommand.toStdString().c_str());
        help_object2->exec();
    #else
        QMessageBox::information(this,"Please see the attached JSIM manual or go to the website: http//:www..com .");
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

//On exiting program, destroys all the objects and files created by the code.
MainWindow::~MainWindow()
{
    QFile file(TempFileName);
    file.remove();
    file.close();
    QFile file1(TempOutFile);
    file1.remove();
    file1.close();
    delete ui;
}

void MainWindow::on_SimComboBox_currentIndexChanged(int index)
{
    if (index==0)
    {
        OutputFileName = "/OUT.DAT";
        TempFileName = QDir::currentPath()+"/Data/ModiNL.cir";
        delimator = " ";
    }
    else
    {
        OutputFileName = "/OUT.csv";
        TempFileName = QDir::currentPath()+"/Data/ModiNL.js";
        delimator = ",";
    }
}

void MainWindow::on_actionManuals_triggered()
{
#ifdef __linux__
    QMessageBox::information(this,"Please see the attached JOINUS manual or go to the website: http//:www..com .");
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
    QString callcommand="LoadFile(\""+QDir::currentPath()+"/Help/Manual2.pdf"+"\")";
    pdf->dynamicCall(callcommand.toStdString().c_str());
    help_object->exec();
#else
    QMessageBox::information(this,"Please see the attached JOINUS manual or go to the website: http//:www..com .");
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
