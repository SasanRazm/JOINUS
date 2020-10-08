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

//Setting the color schem for the program
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 400, 400);
    gradient.setColorAt(0, QColor(50, 230, 255));
    gradient.setColorAt(0.7, QColor(150, 255, 255));
    gradient.setColorAt(1, QColor(230, 255, 255));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);
    runProcess=true;
    //Finished setting background

    #ifdef __APPLE__
        QDir dir = QCoreApplication::applicationDirPath();
        dir.cdUp();
        rootPath = dir.absolutePath();
        PlotPath = documentFolderPath+"/Graphs";
        DataPath = documentFolderPath+"/Data";
        WSIMPath = rootPath+"/Add-ons";
        simEngine = rootPath+"/Simulators";
        TempFileName = documentFolderPath+"/Data/ModiNL.cir";
        TempOutFile = documentFolderPath+"/Data/TempOut.tmp";
        propertiesFile=documentFolderPath+"/properties.stat";
    #endif

        //Initialization of folders
        firstRun_Init();

        //setting the timer for interruptions
//        myTimer = new QTimer(this);
//        myTimer->setInterval(500);
//        myTimer->setSingleShot(false);
//        connect(myTimer, SIGNAL(timeout()), this, SLOT(timerslot()));



    // Create authorization reference
//    AuthorizationRef authorizationRef;
//    OSStatus status;

//    status = AuthorizationCreate(nullptr, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);

//    char* tool = QApplication::instance()->applicationFilePath().toLocal8Bit().data();
//    char* args[] = { "STARTUPDATE", nullptr };
//    FILE* pipe = nullptr;

//    status = AuthorizationExecuteWithPrivileges(authorizationRef, tool, kAuthorizationFlagDefaults, args, &pipe);
//    QApplication::instance()->quit();


    // Initialize the values for the input and output files
    ui->SaveLineEdit->setText(DataPath+"/Out.dat");
    ui->NetlistLineEdit->setText(DataPath+"/JOINUS-test.cir");
    ui->frame->close();
    ui->TempLineEdit->setEnabled(false);
    ui->PlotCheckBox->setEnabled(true);
    QString NetlistFile = ui->NetlistLineEdit->text();
    ui->PlotCheckBox->setChecked(true);
    temperature=4.2;

    // Set text editor font

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);
    ui->NetlistPlainTextEdit->setFont(font);
    font.setFamily("Arial");
    font.setPointSize(10);
    ui->TerminalPlainTextEdit->setFont(font);
    ui->ASWcheckBox->setChecked(true);
    ui->ASWcheckBox->setVisible(false);
    //ui->StopPushButton->setEnabled(false);
    ui->InfoLabel->setText("Initial temperature value : "+QString::number(temperature)+" [K]");

    //Connecting stop to start
    //connect(ui->StopPushButton, SIGNAL(clicked()), ui->StartPushButton, SLOT(exit()));

    //Tests For MacOS
    //Displaying data paths
//    ui->TerminalPlainTextEdit->appendPlainText("Netlist Path: "+DataPath+"/si.inp");
//    ui->TerminalPlainTextEdit->appendPlainText("Output file name: "+dir.currentPath()+OutputFileName);
//    ui->TerminalPlainTextEdit->appendPlainText("JSIM Path: "+dir.absolutePath()+"/JSIM");


    // Initialize the highlighter for the netlist editor

    m_jsimsyntax = new Jsimsyntax(ui->NetlistPlainTextEdit->document());
    LoadNetlist(NetlistFile);

}

// Initialization of the folders for all OS
void MainWindow::firstRun_Init()
{

    //Creating folders and putting files in them
    if (!QDir(documentFolderPath).exists())
    {
        QDir().mkdir(documentFolderPath);
    }

    if (!QDir(DataPath).exists())
    {
        QDir().mkdir(DataPath);
        //QFile::copy(":/Data/Data/JoSIM_n",DataPath+"/JoSIM_n");
        //QFile::copy(":/Data/Data/jsim_n",DataPath+"/jsim_n");
    }

    if (!QDir(PlotPath).exists())
    {
        QDir().mkdir(PlotPath);
    }
    if (!QDir(WSIMPath).exists())
    {
        QDir().mkdir(WSIMPath);
    }
    if (!QDir(simEngine).exists())
    {
        QDir().mkdir(simEngine);
    }
    if (!QDir(documentFolderPath+"/Help").exists())
    {
        QDir().mkdir(documentFolderPath+"/Help");
    }

    //Checking if it is the first run
    if (QDir(rootPath+"/MyResources").exists())
    {
        QMessageBox::information(this,"Initialization","JOINUS folders and files have been initialized during "
                                                       "installation, the JOINUS folder is located in your <i>Documents</i> "
                                                       "folder. It should stay there.");

        QFile::copy(rootPath+"/MyResources/JOINUS-manual-in-a-nutshell-v2.0.pdf",documentFolderPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf");
        QFile::copy(rootPath+"/MyResources/JoSIM-ReadMe.pdf",documentFolderPath+"/Help/JoSIM-ReadMe.pdf");
        QFile::copy(rootPath+"/MyResources/JSIM-manual-v12.0.pdf",documentFolderPath+"/Help/JSIM-manual-v12.0.pdf");


#ifdef __APPLE__
        //Moving engines here
        QFile::copy(rootPath+"/MyResources/jsim_n",simEngine+"/jsim_n");
        QFile(simEngine+"/jsim_n").setPermissions(QFile(rootPath+"/MyResources/jsim_n").permissions());
        QFile::copy(rootPath+"/MyResources/JoSIM_n",simEngine+"/JoSIM_n");
        QFile(simEngine+"/JoSIM_n").setPermissions(QFile(rootPath+"/MyResources/JoSIM_n").permissions());
        //Moving Add-ons here
        if (QFile(rootPath+"/MyResources/WSIM").exists()){
        QFile::copy(rootPath+"/MyResources/WSIM",WSIMPath+"/WSIM");
        QFile(simEngine+"/WSIM").setPermissions(QFile(rootPath+"/MyResources/WSIM").permissions());
        }
        if (QFile(rootPath+"/MyResources/SMP").exists()){
            QFile::copy(rootPath+"/MyResources/SMP",WSIMPath+"/SMP");
            QFile(simEngine+"/SMP").setPermissions(QFile(rootPath+"/MyResources/SMP").permissions());
        }

#elif __linux__
        //Moving engines here
        QFile::copy(rootPath+"/MyResources/jsim_n",simEngine+"/jsim_n");
        QFile(simEngine+"/jsim_n").setPermissions(QFile(rootPath+"/MyResources/jsim_n").permissions());
        QFile::copy(rootPath+"/MyResources/JoSIM_n",simEngine+"/JoSIM_n");
        QFile(simEngine+"/JoSIM_n").setPermissions(QFile(rootPath+"/MyResources/JoSIM_n").permissions());
        //Moving Add-ons here
        if (QFile(rootPath+"/MyResources/Add-ons/WSIM").exists()){
        QFile::copy(rootPath+"/MyResources/Add-ons/WSIM",WSIMPath+"/WSIM");
        QFile(simEngine+"/WSIM").setPermissions(QFile(rootPath+"/MyResources/Add-ons/WSIM").permissions());
        }
        if (QFile(rootPath+"/MyResources/Add-ons/SMP").exists()){
            QFile::copy(rootPath+"/MyResources/Add-ons/SMP",WSIMPath+"/SMP");
            QFile(simEngine+"/SMP").setPermissions(QFile(rootPath+"/MyResources/Add-ons/SMP").permissions());
        }
#elif __WIN32__
        //Moving engines here
        QFile::copy(rootPath+"/MyResources/jsim_n.exe",simEngine+"/jsim_n.exe");
        QFile::copy(rootPath+"/MyResources/JoSIM_n.exe",simEngine+"/JoSIM_n.exe");

        //Moving Add-ons here
        QDir dir;
            if( !dir.rename( rootPath+"/MyResources/Add-ons", rootPath+"/Add-ons" ) ){
              QMessageBox::information(this,"Error","Moving Failed. Please move Add-ons manualy.");
            }

#endif
        if (!QDir(DataPath+"/Demos").exists())
            QDir().rename(rootPath+"/MyResources/Demos", DataPath+"/Demos");
    }

    if (!QFile(DataPath+"/JOINUS-test.cir").exists())
    {
        QFile::copy(":/Data/Data/si.cir",DataPath+"/JOINUS-test.cir");
        QFile(DataPath+"/JOINUS-test.cir").setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
     }
        if (!QFile(DataPath+"/JOINUS-test.png").exists())
            QFile::copy(":/image/Data/si.png",DataPath+"/JOINUS-test.png");



    //Removing the resourses folder from app pack
    QDir dir(rootPath+"/MyResources");
    dir.removeRecursively();


    ui->TerminalPlainTextEdit->appendPlainText(">> Program directory : "+rootPath);
    ui->TerminalPlainTextEdit->appendPlainText(">> Graphs storage directory : "+PlotPath);
    ui->TerminalPlainTextEdit->appendPlainText(">> File storage directory : "+DataPath);
    ui->TerminalPlainTextEdit->appendPlainText(">> Add-ons directory : "+WSIMPath);
    ui->TerminalPlainTextEdit->appendPlainText(">> Simulators native directory : "+simEngine);
    //ui->TerminalPlainTextEdit->appendPlainText(TempFileName);
    //ui->TerminalPlainTextEdit->appendPlainText(TempOutFile);
    //ui->TerminalPlainTextEdit->appendPlainText(propertiesFile);
    //ui->TerminalPlainTextEdit->appendPlainText(documentFolderPath+OutputFileName);

}

// *** Main Simulation Function***
// By pushing this button, the netlist would be compiled and based on the user selections,
// the simulation would start.
// You can add new simulation types here!
// Be careful of the global parameters!

void MainWindow::on_StartPushButton_clicked()
{
    simulateall *simclass=new simulateall;
    runProcess=true;


    if (Simindex==4)
        TempFileName = documentFolderPath+"/Data/TempNetlistBER.cir";
    else if (Simindex==5)
        TempFileName = documentFolderPath+"/Data/TempNetlistFreq.cir";
    else
        TempFileName = documentFolderPath+"/Data/ModiNL.cir";


    // ConsoleOutputs gives the outputs and errors of the terminal. In windows it is Command Prompt.
    struct ConsoleOutputs simout={"",""};

    // SimParams are parameters that are set for the simulation. Full defenition in the header file.
    struct SimParams simParams=readSimParams();
    CalcVals *calcVal=new CalcVals;

    //Statistical analyze of the I-V curve activation
    bool IVstatistical = ui->ASWcheckBox->isChecked();


    ui->InfoLabel->setText("Initial temperature value : "+QString::number(temperature)+" [K]");

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
    QString BERtempfile=documentFolderPath+"/Data/TempOutputBER.DAT";
    QString Freqtempfile=documentFolderPath+"/Data/TempOutputFreq.DAT";

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
        //ui->StopPushButton->setEnabled(true);
        ui->TerminalPlainTextEdit->appendPlainText("Output file name: "+documentFolderPath+OutputFileName);
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
                TempFileName=documentFolderPath+"/Data/TempOut1.tmp";

                //make new netlist after statistical analysis        
                mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,2);
                if (mnnerr!="Success"){
                    QMessageBox::warning(this,"Error!",mnnerr);
                    ui->StartPushButton->setEnabled(true);
                    //ui->StopPushButton->setEnabled(false);
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
                if (!runProcess)
                    break;

                simParams.subParam=initSubParam+"<*>"+calcVal->convertToUnits(calcVal->convertToValues(simParams.minVal)+simstep*stepSize);

                // Change the netlist with the new parameter
                TempFileName=documentFolderPath+"/Data/TempOut"+QString::number(simstep)+".tmp";
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);
                else{

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=documentFolderPath+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
                    QFile::remove(Outtemp);

                    bool whileLoopLogic=QFile::copy(documentFolderPath+OutputFileName, Outtemp);

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
                if (!runProcess)
                    break;
                temperature=calcVal->convertToValues(simParams.minVal)+simstep*stepSize;
                ui->TempLineEdit->setText(QString::number(temperature));
                simParams.tempVal=QString::number(temperature);

                //Change the netlist with the new parameter
                TempFileName=documentFolderPath+"/Data/TempOut"+QString::number(simstep)+".tmp";
                QString mnnerr=simclass->make_new_netlist(noise,NetlistFile,simParams,SimulatorIndex,IVstatistical);
                if (mnnerr!="Success")
                    QMessageBox::warning(this,"Error!",mnnerr);

                else{

                    simout = simclass->simulatenetlist(TempFileName,SimulatorIndex);
                    ui->ProgressBar->setValue(100*(simstep+1)/simParams.pointNum.toInt());

                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolErr);
                    ui->TerminalPlainTextEdit->appendPlainText(simout.ConsolOut);

                    QString Outtemp=documentFolderPath+"/Data/Tmp"+QString::number(simstep)+"K.dat";
                    QFile::remove(Outtemp);
                   bool whileLoopLogic=QFile::copy(documentFolderPath+OutputFileName, Outtemp);
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
                if (!runProcess)
                    break;
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

            if (QFile::exists(documentFolderPath+OutputFileName))
                QFile::remove(documentFolderPath+OutputFileName);
            if(QFile::copy(BERtempfile, documentFolderPath+OutputFileName))
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
                    if (!runProcess)
                        break;

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

                if (QFile::exists(documentFolderPath+OutputFileName))
                    QFile::remove(documentFolderPath+OutputFileName);
                if(QFile::copy(Freqtempfile, documentFolderPath+OutputFileName))
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
        {
            if (Simindex==2)
            {
                //copying all the files in series to the output for parameter shift
                for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
                {
                    QString Outtemp=documentFolderPath+"/Data/Tmp"+titleVals.at(simstep*2)+".dat";
                    QFileInfo fi(ui->SaveLineEdit->text());
                    fi.completeSuffix();
                    QString seriesOutFile=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
                            +QString::number(simstep)+fi.completeSuffix();
                    Copy_File(seriesOutFile,Outtemp);

                }

            }else if (Simindex==3)
            {
                //copying all the files in series to the output for temperature shift
                for (int simstep=0 ; simstep<simParams.pointNum.toInt() ; simstep++)
                {
                    QString Outtemp=documentFolderPath+"/Data/Tmp"+QString::number(simstep)+"K.dat";
                    QFileInfo fi(ui->SaveLineEdit->text());
                    fi.completeSuffix();
                    QString seriesOutFile=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
                            +QString::number(simstep)+fi.completeSuffix();
                    Copy_File(seriesOutFile,Outtemp);
                }
            }
            else
                Copy_File(ui->SaveLineEdit->text(),documentFolderPath+OutputFileName);
        }

        // Testing the Legends value,
        for (int legendcntr=0;legendcntr<Legends.length();legendcntr++)
        {
            ui->TerminalPlainTextEdit->appendPlainText("Data "+QString::number(legendcntr)+" : "+Legends.at(legendcntr));
        }


        if (ui->PlotCheckBox->isChecked()==true)
            plotNetlist();

        ui->StartPushButton->setEnabled(true);
        //ui->StopPushButton->setEnabled(false);

}


void MainWindow::on_actionAUTO5_triggered()
{
    ui->StartPushButton->setEnabled(false);
        autooptim = new AutoOptim(this);
        autooptim->setModal(true);
        QString OptimFile=documentFolderPath+"/OptimConfigFile.tmp";

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
                    QString DataFile=documentFolderPath+OutputFileName;
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
                    QString DataFile=documentFolderPath+OutputFileName;
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
                    Simindex=11;
                    ui->ProgressBar->setValue(1);
                    ui->TerminalPlainTextEdit->appendPlainText(">> Yield analyzer started, this process can be long, please be patient! \n");

                    //Initializing the parameters
                    QString DataFile=documentFolderPath+OutputFileName;
                    QString YAOUT = OptimParams.at(5)+"/YAOUTPUT.DAT";
                    QString YASTAT = OptimParams.at(5)+"/YASTATUES.DAT";

                    QFile outFile(YAOUT);
                    QFile filestatues(YASTAT);

                    if (outFile.exists())
                        outFile.remove();
                    outFile.close();

                    if (filestatues.exists())
                        filestatues.remove();
                    filestatues.close();


                    //Start the loop for Yield analyze
                    int yieldCycle=static_cast<int>(OptimParams.at(9).toDouble()/OptimParams.at(6).toDouble());

                    for (int yieldCycleCNTR=1;yieldCycleCNTR<yieldCycle+1;yieldCycleCNTR++)
                    {
                        double standardDevVal=OptimParams.at(6).toDouble()*yieldCycleCNTR;

                        //This commands run the Yield Analysis function
                        autofuns->Yield_Analysis(OptimParams,standardDevVal);

                        ui->ProgressBar->setValue(100*((yieldCycleCNTR)/yieldCycle));
                    }



                    if (!filestatues.exists())
                    {
                        QMessageBox::warning(this,"Error!","Yield analyzer didn't run correctly!");
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

                    //Moving the result to the output file
                    QFile::remove(DataFile);
                    QFile::copy(YAOUT,DataFile);


                    //QMessageBox::warning(this,"Good","Yield Analyzer is running");
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

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About JOINUS","JOINUS is developed by S. Razmkhah in France as part of the ColdFlux/Supertools project.");
}

// Load the netlist file and copies it to the Plain text editor
void MainWindow::on_NetlistToolButton_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),documentFolderPath, tr("Netlist files (*.cir);;Other netlist files (*.js *.inp);;All files (*)"));
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
    QProcess *process=new QProcess(this);
    ui->statusBar->showMessage("GNUplot running");
    QString OutFile=documentFolderPath+OutputFileName;
    QString Commandlines="\"set terminal png;"
            "set title 'JSIM Plot';"
            "set xlabel 'Time(s)';"
            "set ylabel 'Output';"
            "set style data lines;"
            "plot "+OutFile+";\" >"+PlotPath+"/out.png";
    #ifdef __linux__
    QString plotcommand="gnuplot -e "+Commandlines;
    process->start("/bin/sh", QStringList() << "-c" << plotcommand);
    process->waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process->setProgram("gnuplot.exe");
    process->setArguments({"-e", Commandlines});
    process->setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process->startDetached();
    #else
    QString plotcommand="gnuplot -e "+Commandlines;
    process->start("/bin/sh", QStringList() << "-c" << plotcommand);
    process->waitForFinished(-1); // will wait forever until finished
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
    QString plotstderr = process->readAllStandardError();
    ui->statusBar->showMessage(plotstderr);
    QString plotstdout = process->readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(plotstdout);
}

//Calls the XMGrace program for graph generation from the output
void MainWindow::XMGracePlot()
{
    QProcess *process=new QProcess(this);
    ui->statusBar->showMessage("XMGrace running");
    QString OutFile=documentFolderPath+OutputFileName;
    QString Commandlines="-nxy "+OutFile+" -legend load";
    //QString OSname=QSysInfo::productType();
    #ifdef __linux__
    QString plotcommand="xmgrace "+Commandlines;
    process->start("/bin/sh", QStringList() << "-c" << plotcommand);
    process->waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process->setProgram("qtgrace.exe");
    process->setArguments({Commandlines});
    process->setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process->startDetached();
    #else
    QString plotcommand="xmgrace "+Commandlines;
    process->start("/bin/sh", QStringList() << "-c" << plotcommand);
    process->waitForFinished(-1); // will wait forever until finished
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
    QString plotstderr = process->readAllStandardError();
    ui->statusBar->showMessage(plotstderr);
    QString plotstdout = process->readAllStandardOutput();
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

    //Set the image for start
    QFileInfo fi(NetlistFile);
    QString PicFileName=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
            +"png";
    //ui->TerminalPlainTextEdit->appendPlainText(PicFileName);
    QPixmap pix;
    //QScrollArea *scrollArea=new QScrollArea(this);
    //ui->lblNetlistPic->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //ui->scrollArea->setBackgroundRole(QPalette::Dark);
    //ui->scrollArea->setWidget(ui->lblNetlistPic);
    //scrollArea->setVisible(false);

    ui->lblNetlistPic->setScaledContents(true);
    ui->lblNetlistPic->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    ui->lblNetlistPic->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        if (pix.load(PicFileName)){
            //pix = pix.scaled(ui->lblNetlistPic->size());
            ui->lblNetlistPic->setPixmap(pix);
        }
        else
        {
            pix.load(":/image/Image/actions/cancel.png");
            //pix = pix.scaled(ui->lblNetlistPic->size());
            ui->lblNetlistPic->setPixmap(pix);
        }

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
        ui->ASWcheckBox->setVisible(false);
        QMessageBox::information(this,"Time domain simulation","This is the standard default mode of simulation. "
                                                                "JOINUS calls the chosen Josephson digital simulator to simulate the netlist chosen in the Netlist file"
                                                                " field and displays results in the console and on a plotter window if the Plot results checkbox is ticked. "
                                                                "Noise is added to resistances if they Include noise checkbox is ticked. ");
        break;
    case 1:
        ui->frame->show();
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum current [A]");
        ui->label_10->setText("Maximum current [A]");
        ui->label_11->setText("Current source");
        ui->ParamLineEdit->setEnabled(true);
        ui->ASWcheckBox->setVisible(true);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText("Number of cycles");
        ui->ParamLineEdit->setText("I0");
        ui->SubParamLineEdit->setText("1");
        ui->MinValLineEdit->setText("-1m");
        ui->MaxValLineEdit->setText("1m");
        ui->StepLineEdit->setText("100");
        ui->TempLineEdit->setText(QString::number(temperature));
        ui->SubParamLineEdit->setEnabled(true);
        ui->label_12->show();
        QMessageBox::information(this,"I-V characteristics (accurate)","This is the accuarate method for I-V curve calculation.\n"
                                                            "JOINUS uses the first output of the netlist as Current (Y-axis) and the second output as Voltage (X-axis) and plot them versus each other.\n"
                                                            "The parameters that you must enter are :\n"
                                                            "- Current source : the current source name. (Ex. I0)\n"
                                                            "- Maximum and Minimum current : the boundary values for the current.\n"
                                                            "- Number of steps : the number of points per cycle for the accurate algorithm.\n"
                                                            "- Number of cycles : the number of cycles (number of times the current is swept between its boundary values).\n"
                                                            "If the Adaptive step width option is active, the program will use adaptive time step sizes. This option is slower but it is "
                                                            "useful when there is noise present.");//(or the averaging window size for the fast algorithm)
        break;
    case 2:
        ui->frame->show();
        ui->ParamLineEdit->setEnabled(true);
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum");
        ui->label_10->setText("Maximum");
        ui->label_11->setText("Parameter name");
        ui->label_12->setText("Sub-parameter name");
        ui->SubParamLineEdit->setEnabled(true);
        ui->ASWcheckBox->setVisible(false);
        ui->TempLineEdit->setEnabled(false);
        ui->ParamLineEdit->setText("VBias");
        ui->SubParamLineEdit->setText("2.5mv");
        ui->MinValLineEdit->setText("1m");
        ui->MaxValLineEdit->setText("5m");
        ui->StepLineEdit->setText("5");
        ui->TempLineEdit->setText(QString::number(temperature));
        QMessageBox::information(this,"Parameter analysis","The parametric analysis allows to choose a netlist parameter to sweep it across a specified range.\n "
                                                           "The parameter name, nominal parameter value in the netlist, boundary values and number of steps within the range must be entered on the front panel.\n"
                                                           "For Example if the parameter is the frequency of current source I1, \"Parameter name\" would be \"I1\", \"Sub-parameter name\" would be \"200GHz\""
                                                           ", the boundries would be from \"100G\"Hz to \"500G\"Hz and the number of simulation steps are \"5\".\n"
                                                           "If the JOINUS built-in plotter is used it is possible to visualize the evolution of results when "
                                                           "the parameter under study is modified by sliding the cursor at the bottom of the graph.");
        break;
    case 3:
        ui->frame->show();
        ui->label_7->setText("Number of steps");
        ui->label_8->setText("Temperature [K]");
        ui->label_9->setText("Minimum temperature");
        ui->label_10->setText("Maximum temperature");
        ui->label_11->setText(" ");
        ui->ParamLineEdit->setEnabled(false);
        ui->ASWcheckBox->setVisible(false);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText(" ");
        ui->ParamLineEdit->setText(" ");
        ui->SubParamLineEdit->setText(" ");
        ui->MinValLineEdit->setText("2");
        ui->MaxValLineEdit->setText("7");
        ui->StepLineEdit->setText("6");
        ui->TempLineEdit->setText(" ");
        ui->SubParamLineEdit->setEnabled(false);
        QMessageBox::information(this,"Temperature analysis","The temperature analysis mode is a specific type of parametric analysis that involves changes in all elements of the netlist that depend on temperature.\n "
                                                           "For this mdoe boundary values and number of steps within the range must be entered on the front panel.\n"
                                                           "For example for sweeping temperature between 2K and 7K, with 1K increamenting, there should be 6 steps.\n"
                                                           "If the JOINUS built-in plotter is used it is possible to visualize the evolution of results when "
                                                           "the temperature under study is modified by sliding the cursor at the bottom of the graph.");
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
        ui->ASWcheckBox->setVisible(false);
        ui->TempLineEdit->setEnabled(true);
        ui->ParamLineEdit->setText("VBias");
        ui->SubParamLineEdit->setText("2.5mV");
        ui->MinValLineEdit->setText("1m");
        ui->MaxValLineEdit->setText("5m");
        ui->StepLineEdit->setText("40");
        ui->TempLineEdit->setText("2");
        QMessageBox::information(this,"Bit Error Rate (BER)","For BER analysis, JOINUS needs:\n"
                                                             "- Name of element : the name of the element that is varied, such as \"VBias\".\n"
                                                             "- Nominal value of element : the value of the element from the netlist, such as \"2.5mV\".\n"
                                                             "- Boundary values : \"Maximum value\" and \"Minimum value\" of the element determines the range that parameter changes.\n"
                                                             "- Number of steps : number of simulation points within the chosen boundaries.\n"
                                                             "- Multiplier : The multiplier field is the ratio of the number of input pulses divided by the expected number of output pulses. "
                                                             "For instance, if one expects only one output pulse for two input pulses the multiplier value is 2.\n"
                                                             "By clicking on the Start button JOINUS simulates the netlist for each of the steps and generate temporary outputs for each step. "
                                                             "JOINUS will store the data points for BER after processing the outputs if the Save simulation results checkbox is ticked.");
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
        ui->ASWcheckBox->setVisible(false);
        ui->TempLineEdit->setEnabled(true);
        ui->ParamLineEdit->setText("XI12");
        ui->SubParamLineEdit->setText("3");
        ui->MinValLineEdit->setText("25G");
        ui->MaxValLineEdit->setText("150G");
        ui->StepLineEdit->setText("26");
        ui->TempLineEdit->setText("1,2,2");
        QMessageBox::information(this,"Frequency Response","This function applies a pulse train at the input node of the circuit netlist and measures the number of output pulses."
                                                           " Enter in the parameter panel :  \n"
                                                           "- node number of pulse train generator,\n"
                                                           "- pulse train minimum and maximum frequencies,\n"
                                                           "- number of steps.\n"
                                                           "- multipliers separated by commas (Ex : 1,4,2 for one input and 2 outputs)."
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
        ui->ASWcheckBox->setVisible(true);
        ui->TempLineEdit->setEnabled(false);
        ui->label_12->setText("Number of cycles");
        ui->ParamLineEdit->setText("I0");
        ui->SubParamLineEdit->setText("1");
        ui->MinValLineEdit->setText("-1m");
        ui->MaxValLineEdit->setText("1m");
        if (ui->ASWcheckBox->isChecked())
            ui->StepLineEdit->setText("3000");
        else
            ui->StepLineEdit->setText("1000");
        ui->TempLineEdit->setText(QString::number(temperature));
        ui->SubParamLineEdit->setEnabled(true);
        ui->label_12->show();
        QMessageBox::information(this,"I-V characteristics (fast)","This is the accuarate method for I-V curve calculation.\n"
                                                            "JOINUS uses the first output of the netlist as Current (Y-axis) and the second output as Voltage (X-axis) and plot them versus each other.\n"
                                                            "The parameters that you must enter are :\n"
                                                            "- Current source : the current source name. (Ex. I0)\n"
                                                            "- Maximum and Minimum current : the boundary values for the current.\n"
                                                            "- Averaging window size : the averaging window size for the fast algorithm. Bigger window will filter higher frequency information but gives smoother result.\n"
                                                            "- Number of cycles : the number of cycles (number of times the current is swept between its boundary values).\n"
                                 );
        break;

    default:
        break;
    }

}

//Save the changes made in the Plain text edit to the Netlist
void MainWindow::on_SaveNetListPushBottun_clicked()
{
    QMessageBox::StandardButton overwriteCondition = QMessageBox::question(this,"Saving netlist","Overwrite current netlist?",QMessageBox::Yes|QMessageBox::No);
    QString NetlistFile;
    if (overwriteCondition==QMessageBox::Yes)
    {
        NetlistFile = ui->NetlistLineEdit->text();
        ui->NetlistLineEdit->setText(NetlistFile);
        QFile file(NetlistFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
             QMessageBox::warning(this,"Error!",file.errorString());
        }
        else{
                QTextStream stream(&file);
                stream << ui->NetlistPlainTextEdit->toPlainText();
                ui->TerminalPlainTextEdit->appendPlainText(">> Netlist file saved successfully!");
                file.close();
                stream.flush();
        }
    } else if(overwriteCondition==QMessageBox::No)
    {
        NetlistFile = QFileDialog::getSaveFileName(this, tr("Save Netlist File..."),QString(), tr("Netlist Files (*.js *.inp);;All Files (*)"));
        if (!NetlistFile.isEmpty()){
        ui->NetlistLineEdit->setText(NetlistFile);
        QFile file(NetlistFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
             QMessageBox::warning(this,"Error!",file.errorString());
        }
        else{
                QTextStream stream(&file);
                stream << ui->NetlistPlainTextEdit->toPlainText();
                ui->TerminalPlainTextEdit->appendPlainText(">> Netlist file saved successfully!");
                file.close();
                stream.flush();
        }
        }
    }

}

void MainWindow::on_ClearTerminalpushButton_clicked()
{
    ui->TerminalPlainTextEdit->clear();
    ui->TerminalPlainTextEdit->appendPlainText(">>");
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
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"));
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
        QString callcommand="LoadFile(\""+documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"+"\")";
        pdf2->dynamicCall(callcommand.toStdString().c_str());
        help_object2->exec();
    #else

        QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"));
        //QDesktopServices::openUrl(QUrl::fromLocalFile(":/Help/Help/JoSIM-ReadMe.pdf"));
        //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
    #endif
}

void MainWindow::on_actionJSIM_manual_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"));
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
    QString callcommand="LoadFile(\""+documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"+"\")";
    pdf2->dynamicCall(callcommand.toStdString().c_str());
    help_object2->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JSIM-manual-v12.0.pdf"));
    //QMessageBox::information(this,"Error","Please see the attached JSIM manual or go to the website: http//:www..com .");
#endif
}

void MainWindow::on_actionJ_oSIM_manual_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JoSIM-ReadMe.pdf"));
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
    QString callcommand="LoadFile(\""+documentFolderPath+"/Help/JoSIM-ReadMe.pdf"+"\")";
    pdf2->dynamicCall(callcommand.toStdString().c_str());
    help_object2->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JoSIM-ReadMe.pdf"));
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

    QString StateFile=documentFolderPath+"/STATE.STA";

    StateFile = QFileDialog::getSaveFileName(this, tr("Save State File..."),documentFolderPath, tr("State Files (*.STA);;All Files (*)"));
    QFile file(StateFile);
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

    QString StateFile=documentFolderPath+"/STATE.STA";
    StateFile = QFileDialog::getOpenFileName(this, tr("Open State File..."),documentFolderPath, tr("State Files (*.STA);;All Files (*)"));

    QFile file(StateFile);
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
        //Simindex=loadmaterial.at(3).toInt();

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

}


void MainWindow::on_SimComboBox_currentIndexChanged(int index)
{
    if (index==0)
    {
        OutputFileName = "/OUT.DAT";
        TempFileName = documentFolderPath+"/Data/ModiNL.cir";
        delimator = " ";
    }
    else
    {
        OutputFileName = "/OUT.csv";
        TempFileName = documentFolderPath+"/Data/ModiNL.js";
        delimator = ",";
    }
}

void MainWindow::on_actionManuals_triggered()
{
#ifdef __linux__
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"));
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
    QString callcommand="LoadFile(\""+documentFolderPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"+"\")";
    pdf->dynamicCall(callcommand.toStdString().c_str());
    help_object->exec();
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(documentFolderPath+"/Help/JOINUS-manual-in-a-nutshell-v2.0.pdf"));
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

    ui->InfoLabel->setText("Initial temperature value : "+QString::number(temperature)+" [K]");
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
    QProcess *process=new QProcess(this);
    #ifdef __linux__
    process->start("./Add-ons/Wsim");
    process->waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process->setProgram("cmd.exe");
    process->setArguments({"/c", WSIMPath+"/Wsim.exe"});
    process->setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process->startDetached();
    #else
    process->start(WSIMPath+"/Wsim");
    process->waitForFinished(-1); // will wait forever until finished
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

    QString stderrWsim = process->readAllStandardError();
    ui->statusBar->showMessage(stderrWsim);
    QString stdmssWsim = process->readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssWsim);
}

void MainWindow::on_actionSQUID_MAP_triggered()
{
    QProcess *process=new QProcess(this);

    #ifdef __linux__
    process->start("./Add-ons/SMP");
    process->waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process->setProgram("cmd.exe");
    process->setArguments({"/c", WSIMPath+"/SMP.exe"});
    process->setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process->startDetached();
    #else
    process->start(WSIMPath+"/SMP");
    process->waitForFinished(-1); // will wait forever until finished
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

    QString stderrSMP = process->readAllStandardError();
    ui->statusBar->showMessage(stderrSMP);
    QString stdmssSMP = process->readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssSMP);
}

void MainWindow::on_actionMAR_GINOPT_triggered()
{
    QProcess *process=new QProcess(this);

    #ifdef __linux__
    process->start("./Add-ons/MCO");
    process->waitForFinished(-1); // will wait forever until finished
    #elif _WIN32
    process->setProgram("cmd.exe");
    process->setArguments({"/c", WSIMPath+"/MCO.exe"});
    process->setCreateProcessArgumentsModifier([] (
        QProcess::CreateProcessArguments *args) {
                    args->flags &= CREATE_NO_WINDOW;});
    process->startDetached();
    #else
    process->start(WSIMPath+"/MCO");
    process->waitForFinished(-1); // will wait forever until finished
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

    QString stderrMCO = process->readAllStandardError();
    ui->statusBar->showMessage(stderrMCO);
    QString stdmssMCO = process->readAllStandardOutput();
    ui->TerminalPlainTextEdit->appendPlainText(stdmssMCO);
}

//On exiting program, destroys all the objects and files created by the code.




void MainWindow::on_convertPushButton_clicked()
{
    QStringList Commands;
    QString FileName=ui->NetlistLineEdit->text();
    Jsimsyntax *syntaxChange=new Jsimsyntax;

    QFile NetlistFile(FileName);
    Commands.clear();

    if (!NetlistFile.exists())
    {
        QMessageBox::warning(this,"Warning","Netlist file do not exist in the defined location.");
        return;
    }
    if (!NetlistFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"Warning","Cannot open netlist for read. Check for permissions.");
        return;
    }

    QTextStream in(&NetlistFile);

    while(!in.atEnd()) {
        QString line = in.readLine();
        Commands.append(line);
    }
    NetlistFile.close();


    QStringList NewCommands=syntaxChange->convertToJSIM(Commands);


    QString JSIMFileName=documentFolderPath+"/JSIMNetList.js";
    int fileIndex=0;
    while (QFile::exists(JSIMFileName)){
        fileIndex++;
        JSIMFileName=documentFolderPath+"/JSIMNetList"+QString::number(fileIndex)+".js";
    }

    //Writing the result to the output file
    QFile  NetlistFile1(JSIMFileName);

    if (!NetlistFile1.open(QFile::WriteOnly | QFile::Text))
    {

        QMessageBox::warning(this,"Warning","Cannot open new netlist to write. Check for permissions.");
        return;
    }

    fileIndex=0;
    QTextStream stream(&NetlistFile1);
    for (int cmdCNTR=0;cmdCNTR<NewCommands.length();cmdCNTR++)
        stream<<NewCommands.at(cmdCNTR)<<'\n';

    stream.flush();
    NetlistFile1.close();

    ui->NetlistLineEdit->setText(JSIMFileName);
    LoadNetlist(JSIMFileName);

}


void MainWindow::on_StopPushButton_clicked()
{
//    QMessageBox::StandardButton ContinueCondition = QMessageBox::question(this,"Stop Process","Stop the simulation?",QMessageBox::Yes|QMessageBox::No);
//    QString NetlistFile;
//    if (ContinueCondition==QMessageBox::Yes)
//    {

//    }
    //this->myTimer->stop();
runProcess=false;
ui->SimComboBox->setEnabled(true);
ui->TypeComboBox->setEnabled(true);
ui->StartPushButton->setEnabled(true);
ui->TerminalPlainTextEdit->appendPlainText(">> Simulation stopped!");
QMessageBox::information(this,"Stop","The function is stopped.");

}


void MainWindow::on_actionSet_Workspace_triggered()
{
    QString dirFolder = QFileDialog::getExistingDirectory(this, tr("Open Workspace"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (!dirFolder.isEmpty())
    {
        documentFolderPath=dirFolder;

        if (!QDir(DataPath).exists())
        {
            QDir().mkdir(DataPath);
        }

        if (!QDir(PlotPath).exists())
        {
            QDir().mkdir(PlotPath);
        }

        if (!QDir(documentFolderPath+"/Help").exists())
        {
            QDir().mkdir(documentFolderPath+"/Help");
        }
    }
    ui->TerminalPlainTextEdit->appendPlainText("Workspace set to : " + documentFolderPath);
}

void MainWindow::on_actionSchamatic_Capture_triggered()
{
    joinuscapture = new JOINUSCapture(this);
    joinuscapture->show();
}

void MainWindow::on_actionSet_Advanced_values_triggered()
{
    setparamdialog = new SetParamsDialog(this);
    setparamdialog->show();
}

void MainWindow::on_actionInductex_triggered()
{
    inductex = new InductexGUI(this);
    inductex->show();
}

void MainWindow::on_actionQuit_triggered()
{

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}

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
    if (file.exists())
        file.remove();
    file.close();

    QFile file1(TempOutFile);
    if (file1.exists())
        file1.remove();
    file1.close();

    QFile file2(TempOutFile);
    if (file2.exists())
        file2.remove();
    file2.close();
    delete ui;
}


