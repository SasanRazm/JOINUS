/*
 *  Global optimizer will calculate the changes in parameters and generate a Netlist
 * and Parametric file suitable for center of gravity optimizer.
 *
 *
*/
#include "globaloptimizer.h"
#include "ui_globaloptimizer.h"
#include "myglobalvars.h"
#include "calcvals.h"
#include <qscreen.h>
#include <qfile.h>
#include "mainwindow.h"
#include <qmessagebox.h>
#include <qtextstream.h>
#include <QDateTime>
#include <qmath.h>
#include <QRegularExpression>
#include "simulateall.h"

GlobalOptimizer::GlobalOptimizer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalOptimizer)
{
    //Setting the interface
        QPalette pal = this->palette();
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(0, 50, 50));
        gradient.setColorAt(0.7, QColor(0, 100, 100));
        gradient.setColorAt(1, QColor(200, 100, 200));
        pal.setBrush(QPalette::Window, QBrush(gradient));
        this->setPalette(pal);
        QScreen *myscreen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = myscreen->geometry();
        this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        this->setGeometry(20,50,screenGeometry.width()*4/6,screenGeometry.height()*3/5);
    ui->setupUi(this);
}


//Read the commands from netlist
struct GCommandList GlobalOptimizer::ReadCommands(QString FileName)
{
    QStringList NoCommentCommands;
    QStringList CommandswithComment;

    QFile NetlistFile(FileName);
    CommandswithComment.clear();

    if (!NetlistFile.exists())
    {
        QMessageBox::warning(this,"Warning","Netlist file do not exist in the defined location.");
        return {};
    }
    if (!NetlistFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"Warning","Cannot open netlist for read. Check for permissions.");
        return {};
    }

    QTextStream in(&NetlistFile);

    while(!in.atEnd()) {
        QString line = in.readLine();
        CommandswithComment.append(line);
    }
    NetlistFile.close();


    for (int lineCNTR=0;lineCNTR<CommandswithComment.length();lineCNTR++)
    {
        QStringList temp = CommandswithComment.at(lineCNTR).split('*');
        if (!temp.at(0).isEmpty())
            NoCommentCommands.append(temp.at(0));
        temp.clear();
    }

    return {NoCommentCommands, CommandswithComment};
}


// Gets the changed commands and make the new commandlines that contain comments from before
QStringList GlobalOptimizer::WriteCommands(struct GCommandList NewList, QStringList NewCommands)
{
    QStringList NewCommandswithCommands;

    int fileIndex=0;
    for (int lineCNTR=0;lineCNTR<NewList.CommandswithComment.length();lineCNTR++)
        {
        if (fileIndex<NewCommands.length() && NewList.CommandswithComment.at(lineCNTR).contains(NewList.NoCommentCommands.at(fileIndex)))
            {
                QString NewLine=NewList.CommandswithComment.at(lineCNTR);
                NewLine.replace(NewList.NoCommentCommands.at(fileIndex),NewCommands.at(fileIndex),Qt::CaseInsensitive);
                NewCommandswithCommands.append(NewLine);
                fileIndex++;
            }
            else
            {
                NewCommandswithCommands.append(NewList.CommandswithComment.at(lineCNTR));
            }

        //Testing the new commands
        //qDebug() << NewCommandswithCommands.at(lineCNTR) << '\n';

        }

    return NewCommandswithCommands;
}

double GlobalOptimizer::Change_Thickness(double Lk, double varVal)
{

    double initialThickness = 300e-9;
    double newThickness = initialThickness * (100 + varVal) / 100;
    double newLk = Lk;


    double constMultiplier = Lk / ((P_lambda * sinh(initialThickness/P_lambda) + initialThickness) / (cosh(initialThickness/P_lambda)-1));


    newLk = constMultiplier * ((P_lambda * sinh(newThickness/P_lambda) + newThickness) / (cosh(newThickness/P_lambda)-1));

    //This method is by calculating the derivative of the Lk

    //double trueVarVal = (newThickness - initialThickness);
    //double d_Lk = -trueVarVal * constMultiplier * newThickness * sinh(newThickness/P_lambda)/(newThickness * qPow((cosh(newThickness/P_lambda)-1),2));
    //newLk = Lk + d_Lk;

    return newLk;
}

QStringList GlobalOptimizer::Change_Inductances(QStringList Commands, QString FileName)
{
    CalcVals *values= new CalcVals;

    QStringList InductanceChanges;
    QStringList NewCommands = Commands;
    QFile file(FileName);

    double varVal = ui->spinBoxInd->value();

    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("H",Qt::CaseInsensitive)
                           & Commands.at(fcntr).contains("L",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive);
        if (correctLine)
        {
            QStringList Lcommand={};
            QStringList tempe=Commands.at(fcntr).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    Lcommand.append(tempe.at(tempcntr));
            }
            QRegExp L = QRegExp("^(.+)(H)$", Qt::CaseInsensitive);
            L.indexIn(Lcommand.at(3));

            if (!L.isEmpty())
            {
                QString Lstr = L.cap(1);
                double Lval = values->convertToValues(Lstr);

                if (Lval > 0)
                {
                    //Change the inductance value based on variation
                    double newLk = Change_Thickness(Lval,varVal);

                    //Write the command for the parameter file for the inductance optimization
                    QString InductanceParamLine = "@L"+QString::number(fcntr)+"@   "+values->convertToUnits(newLk)+ "   " + Lcommand.at(0)+"   1";
                    InductanceChanges.append(InductanceParamLine);

                    //Find the multiplier value of the parameter
                    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
                    r.indexIn(Lstr);
                    QString unit = "";

                    if ( r.captureCount() == 2 ) {
                        unit = r.cap(2);
                    }

                    //Change the Command line to add the parameter to the netlist
                    QString NewNetlistLine = Lcommand.at(0) + "  " + Lcommand.at(1) + "  " + Lcommand.at(2)+"  " + "@L" + QString::number(fcntr)+"@" + unit + "H  fcheck";
                    NewCommands.replace(fcntr,NewNetlistLine);
                }
            }
        }

    }


    //Write the Parameter file for optimizer
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
               return {file.errorString()};
           }
    else{

        QTextStream stream(&file);

        for (int fcntr=0; fcntr<InductanceChanges.length();fcntr++)
            {
                stream << InductanceChanges.at(fcntr)<<'\n';
            }

        file.close();
        stream.flush();
    }


    return NewCommands;
}

QStringList GlobalOptimizer::Change_Junctions(QStringList Commands, QString FileName)
{
    CalcVals *values= new CalcVals;

    QStringList JunctionChanges;
    QStringList NewCommands = Commands;
    QFile file(FileName);


    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("area",Qt::CaseInsensitive)
                & Commands.at(fcntr).contains("B",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive);

        if (correctLine)
        {
            QStringList Bcommand={};
            QStringList tempe=Commands.at(fcntr).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    Bcommand.append(tempe.at(tempcntr));
            }
            QRegExp B = QRegExp("^(area=)(.+)", Qt::CaseInsensitive);
            B.indexIn(Bcommand.at(4));

            if (!B.isEmpty())
            {
                QString Bstr = B.cap(2);
                double Bval = values->convertToValues(Bstr);

                if (Bval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString JunctionParamLine = "@B"+QString::number(fcntr)+"@   "+values->convertToUnits(Bval)+ "   " + Bcommand.at(0)+"   1";
                    JunctionChanges.append(JunctionParamLine);

                    //Find the multiplier value of the parameter
                    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
                    r.indexIn(Bstr);
                    QString unit = "";

                    if ( r.captureCount() == 2 ) {
                        unit = r.cap(2);
                    }

                    //Change the Command line to add the parameter to the netlist
                    QString NewNetlistLine = Bcommand.at(0) + "   " + Bcommand.at(1) + "   " + Bcommand.at(2) + "   " + Bcommand.at(3) + "   area=" + "@B" + QString::number(fcntr)+"@" + unit;
                    NewCommands.replace(fcntr,NewNetlistLine);
                }
            }
        }

    }


    //Write the Parameter file for optimizer
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
               return {file.errorString()};
           }
    else{

        QTextStream stream(&file);

        for (int fcntr=0; fcntr<JunctionChanges.length();fcntr++)
            {
                stream << JunctionChanges.at(fcntr)<<'\n';
            }

        file.close();
        stream.flush();
    }


    return NewCommands;
}



QStringList GlobalOptimizer::Change_BiasResistances(QStringList Commands, QString FileName)
{
    CalcVals *values= new CalcVals;

    QStringList BiasResChanges;
    QStringList NewCommands = Commands;
    QFile file(FileName);


    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("ohm",Qt::CaseInsensitive)
                & Commands.at(fcntr).contains("R",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("RS",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive);

        if (correctLine)
        {
            QStringList Rcommand={};

            QStringList tempe=Commands.at(fcntr).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    Rcommand.append(tempe.at(tempcntr));
            }
            QRegExp R = QRegExp("^(.+)(ohm)$", Qt::CaseInsensitive);
            R.indexIn(Rcommand.at(3));

            if (!R.isEmpty())
            {
                QString Rstr = R.cap(1);
                double Rval = values->convertToValues(Rstr);

                if (Rval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString BiasResParamLine = "@R"+QString::number(fcntr)+"@   "+values->convertToUnits(Rval)+ "   " + Rcommand.at(0)+"   1";
                    BiasResChanges.append(BiasResParamLine);

                    //Find the multiplier value of the parameter
                    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
                    r.indexIn(Rstr);
                    QString unit = "";

                    if ( r.captureCount() == 2 ) {
                        unit = r.cap(2);
                    }

                    //Change the Command line to add the parameter to the netlist
                    QString NewNetlistLine = Rcommand.at(0) + "  " + Rcommand.at(1) + "  " + Rcommand.at(2)+"  " + "@R" + QString::number(fcntr)+"@" + unit + "ohm";
                    NewCommands.replace(fcntr,NewNetlistLine);
                }
            }
        }

    }


    //Write the Parameter file for optimizer
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
               return {file.errorString()};
           }
    else{

        QTextStream stream(&file);

        for (int fcntr=0; fcntr<BiasResChanges.length();fcntr++)
            {
                stream << BiasResChanges.at(fcntr)<<'\n';
            }

        file.close();
        stream.flush();
    }


    return NewCommands;
}

QStringList GlobalOptimizer::Change_ShuntResistances(QStringList Commands, QString FileName)
{
    CalcVals *values= new CalcVals;

    QStringList ShuntResChanges;
    QStringList NewCommands = Commands;
    QFile file(FileName);


    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("ohm",Qt::CaseInsensitive)
                & Commands.at(fcntr).contains("RS",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive);

        if (correctLine)
        {
            QStringList Rcommand={};

            QStringList tempe=Commands.at(fcntr).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    Rcommand.append(tempe.at(tempcntr));
            }
            QRegExp R = QRegExp("^(.+)(ohm)$", Qt::CaseInsensitive);
            R.indexIn(Rcommand.at(3));

            if (!R.isEmpty())
            {
                QString Rstr = R.cap(1);
                double Rval = values->convertToValues(Rstr);

                if (Rval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString BiasResParamLine = "@R"+QString::number(fcntr)+"@   "+values->convertToUnits(Rval)+ "   " + Rcommand.at(0)+"   1";
                    ShuntResChanges.append(BiasResParamLine);

                    //Find the multiplier value of the parameter
                    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
                    r.indexIn(Rstr);
                    QString unit = "";

                    if ( r.captureCount() == 2 ) {
                        unit = r.cap(2);
                    }

                    //Change the Command line to add the parameter to the netlist
                    QString NewNetlistLine = Rcommand.at(0) + "  " + Rcommand.at(1) + "  " + Rcommand.at(2)+"  " + "@R" + QString::number(fcntr)+"@" + unit + "ohm";
                    NewCommands.replace(fcntr,NewNetlistLine);
                }
            }
        }

    }


    //Write the Parameter file for optimizer
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
               return {file.errorString()};
           }
    else{

        QTextStream stream(&file);

        for (int fcntr=0; fcntr<ShuntResChanges.length();fcntr++)
            {
                stream << ShuntResChanges.at(fcntr)<<'\n';
            }

        file.close();
        stream.flush();
    }


    return NewCommands;
}

QStringList GlobalOptimizer::Change_Capacitances(QStringList Commands, QString FileName)
{

    CalcVals *values= new CalcVals;

    QStringList CapChanges;
    QStringList NewCommands = Commands;
    QFile file(FileName);


    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("F",Qt::CaseInsensitive)
                & Commands.at(fcntr).contains("C",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("X",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("fcheck",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive)
                & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive);

        if (correctLine)
        {
            QStringList Rcommand={};

            QStringList tempe=Commands.at(fcntr).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    Rcommand.append(tempe.at(tempcntr));
            }

            QRegExp C = QRegExp("^(.+)(F)$", Qt::CaseInsensitive);
            C.indexIn(Rcommand.at(3));

            if (!C.isEmpty())
            {
                QString Cstr = C.cap(1);
                double Cval = values->convertToValues(Cstr);

                if (Cval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString CapParamLine = "@C"+QString::number(fcntr)+"@   "+values->convertToUnits(Cval)+ "   " + Rcommand.at(0)+"   1";
                    CapChanges.append(CapParamLine);

                    //Find the multiplier value of the parameter
                    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
                    r.indexIn(Cstr);
                    QString unit = "";

                    if ( r.captureCount() == 2 ) {
                        unit = r.cap(2);
                    }

                    //Change the Command line to add the parameter to the netlist
                    QString NewNetlistLine = Rcommand.at(0) + "  " + Rcommand.at(1) + "  " + Rcommand.at(2)+"  " + "@C" + QString::number(fcntr)+"@" + unit + "F";
                    NewCommands.replace(fcntr,NewNetlistLine);
                }
            }
        }

    }


    //Write the Parameter file for optimizer
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
               return {file.errorString()};
           }
    else{

        QTextStream stream(&file);

        for (int fcntr=0; fcntr<CapChanges.length();fcntr++)
            {
                stream << CapChanges.at(fcntr)<<'\n';
            }

        file.close();
        stream.flush();
    }


    return NewCommands;
}

QVector<double> GlobalOptimizer::MakeRandNum(double meanVal, double sigmaVal, int datacount)
{
    qsrand(QDateTime::currentDateTime().toTime_t());

    QVector<double> RandomData(datacount);
    sigmaVal = meanVal * sigmaVal/100;

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

double GlobalOptimizer::Change_BetaParam(double Rshunt, double varVal)
{


    double betaVar = (100 + varVal) / 100;
    double newRshunt = Rshunt * qSqrt(betaVar);


    return newRshunt;
}



QStringList GlobalOptimizer::Change_CriticalCurr(QStringList Commands, QString FileName)
{
    QStringList NewCommands = Commands;


    return NewCommands;
}

QStringList GlobalOptimizer::Change_BiasVals(QStringList Commands, QString FileName)
{
    QStringList NewCommands = Commands;

    return NewCommands;
}

void GlobalOptimizer::on_buttonBox_clicked(QAbstractButton *button)
{
    //In case of apply, generate netlist and put the parameters in the table

    if (button->text()=="Apply")
    {

    }


}


void GlobalOptimizer::on_buttonBox_accepted()
{

    simulateall *simclass=new simulateall;

    QString netlistfile=ui->lineEditNetlist->text();

        QFileInfo Myfile(netlistfile);

        QString GlobalOptimizerFile=Myfile.absolutePath()+"/GO"+Myfile.fileName().remove(Myfile.completeSuffix(),Qt::CaseInsensitive)+"cir";
        QString GlobalOptimizerParamFile=Myfile.absolutePath()+"/GO"+Myfile.fileName().remove(Myfile.completeSuffix(),Qt::CaseInsensitive)+"para";
        QString GlobalOptimizerSolFile=Myfile.absolutePath()+"/GO"+Myfile.fileName().remove(Myfile.completeSuffix(),Qt::CaseInsensitive)+"sol";


        struct SimParams simParams;
        simParams.pointNum="0";


        struct processedNL aa= simclass->processNetlist(netlistfile);
        OutputFileName = aa.OutFileName;
        QString DataFile = documentFolderPath+OutputFileName;
        columNum = aa.columNumb;
        timestep = aa.tstep;

        struct GCommandList MyCommandList=ReadCommands(netlistfile);

        QStringList NewCommands = MyCommandList.NoCommentCommands;
        QStringList TempNewCommands = NewCommands;




                if (ui->checkBoxInd->isChecked())
                {
                    TempNewCommands = Change_Inductances(NewCommands,);
                    NewCommands = TempNewCommands;
                }

                if (ui->checkBoxRes->isChecked())
                {
                    TempNewCommands = Change_BiasResistances(NewCommands,ui->spinBoxRes->value());
                    NewCommands = TempNewCommands;
                }

                if (ui->checkBoxJC->isChecked())
                {
                    TempNewCommands = Change_Junctions(NewCommands,ui->spinBoxJJ->value());
                    NewCommands = TempNewCommands;
                }

                if (ui->checkBoxShunt->isChecked())
                {
                    TempNewCommands = Change_ShuntResistances(NewCommands,ui->spinBoxSrc->value());
                    NewCommands = TempNewCommands;
                }


        QStringList AllCommands = WriteCommands(MyCommandList, NewCommands);



        QFile file(lgpdispersFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
             QMessageBox::warning(this,"Error!",file.errorString());
        }
        else{
                QTextStream stream(&file);

                for (int fcntr=0; fcntr<AllCommands.length();fcntr++)
                    {
                        stream << AllCommands.at(fcntr)<<'\n';
                    }

                file.close();
                stream.flush();

                QString message = "File generated at the same path as netlist. File name : \n" + Myfile.absolutePath() + "/PD" + Myfile.fileName().remove(Myfile.completeSuffix(),Qt::CaseInsensitive)+"cir";
                QMessageBox::information(this,"Dispertion File",message);
        }

}


void GlobalOptimizer::on_toolButtonNetlist_3_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),documentFolderPath, tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
            if (!NetlistFile.isEmpty())
            {
                ui->lineEditNetlist->setText(NetlistFile);
            }
}

void GlobalOptimizer::on_toolButtonSolution_3_clicked()
{
    QString SolutionFile = QFileDialog::getOpenFileName(this, tr("Open Solution File..."),documentFolderPath, tr("Solution Files (*.sol *.solo);;All Files (*)"));
            if (!SolutionFile.isEmpty())
            {
                ui->lineEditSolution->setText(SolutionFile);
            }
}

void GlobalOptimizer::on_buttonBox_rejected()
{

}

GlobalOptimizer::~GlobalOptimizer()
{
    delete ui;
}
