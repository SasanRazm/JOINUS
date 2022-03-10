#include "lgpdispers.h"
#include "ui_lgpdispers.h"
#include "myglobalvars.h"
#include <qscreen.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "calcvals.h"
#include "simulateall.h"
#include <qtextstream.h>
#include "mainwindow.h"
#include <QDateTime>
#include <QRegularExpression>

LGPdispers::LGPdispers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LGPdispers)
{
    ui->setupUi(this);

    //Setting the interface
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(0, 50, 50));
    gradient.setColorAt(0.7, QColor(0, 100, 100));
    gradient.setColorAt(1, QColor(0, 200, 200));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);
    QScreen *myscreen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = myscreen->geometry();
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setGeometry(20,50,screenGeometry.width()*4/6,screenGeometry.height()*3/5);

    ui->lineEditFilePath->setText(documentFolderPath+"/Data");
    qsrand(QDateTime::currentDateTime().toTime_t());

}

struct CommandList LGPdispers::ReadCommands(QString FileName)
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
QStringList LGPdispers::WriteCommands(struct CommandList NewList, QStringList NewCommands)
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


QStringList LGPdispers::ParameterTree(struct ElementIndex indexes, QStringList NoCommentCommands, int Index_Enable[6])
{
    QStringList parameterTree={};
    //QStringList subcircuit_Names={};
    QList<int> subcircuit_Indexes={};

    QList<int> R_Index=indexes.R_Index;
    QList<int> L_Index=indexes.L_Index;
    QList<int> C_Index=indexes.C_Index;
    QList<int> J_Index=indexes.J_Index;
    QList<int> V_Index=indexes.V_Index;
    QList<int> I_Index=indexes.I_Index;
    QList<int> AllIndex={};

    if (Index_Enable[0]==1)
        AllIndex.append(R_Index);
    if (Index_Enable[1]==1)
        AllIndex.append(L_Index);
    if (Index_Enable[2]==1)
        AllIndex.append(C_Index);
    if (Index_Enable[3]==1)
        AllIndex.append(J_Index);
    if (Index_Enable[4]==1)
        AllIndex.append(V_Index);
    if (Index_Enable[5]==1)
        AllIndex.append(I_Index);
    AllIndex.append(indexes.SUB_Index);

    if(Index_Enable[0]|Index_Enable[1]|Index_Enable[2]|Index_Enable[3]|Index_Enable[4]|Index_Enable[5])
        std::sort(AllIndex.begin(),AllIndex.end());

    QStringList SeperatedCommand;

    if (!AllIndex.isEmpty())
    {
        int indexcntr=0;
        if (indexes.SUB_Index.isEmpty())
        {
            parameterTree.append("MainHeader");
            while ((indexcntr<AllIndex.length()))
            {
                 SeperatedCommand=NoCommentCommands.at(AllIndex.at(indexcntr)).split(" ");
                 parameterTree.append(SeperatedCommand.at(0));
                 indexcntr++;
            }

        }else
        {
            for (int subCntr=0;subCntr<indexes.SUB_Index.length();subCntr=+2)
            {

            }

        }


    }
//        int indexcntr=0;
//        for (int commentCNTR=0;commentCNTR<NoCommentCommands.length();commentCNTR++)
//        {
//            QRegExp subcktStartExpression = QRegExp("\\.subckt[^\n]*", Qt::CaseInsensitive);
//            QRegExp subcktEndExpression = QRegExp("\\.ends[^\n]*", Qt::CaseInsensitive);
//            subcktStartExpression.indexIn(NoCommentCommands.at(commentCNTR));
//            subcktEndExpression.indexIn(NoCommentCommands.at(commentCNTR));

//            if (!subcktStartExpression.isEmpty())
//            {
//                SeperatedCommand=NoCommentCommands.at(commentCNTR).split(" ");
//                parameterTree.append("SubHeader");
//                parameterTree.append(SeperatedCommand.at(1));
//                //subcircuit_Indexes.append(commentCNTR);
//                startVal=commentCNTR;
//            }else if (!subcktEndExpression.isEmpty())
//            {

//                endVal=commentCNTR;
//                while ((indexcntr<AllIndex.length())&(AllIndex.at(indexcntr)<commentCNTR))
//                {
//                    SeperatedCommand=NoCommentCommands.at(AllIndex.at(indexcntr)).split(" ");
//                    parameterTree.append(SeperatedCommand.at(0));
//                }

//                subcircuit_Indexes.append(commentCNTR);

//            }

//        }

//        for (int commentCNTR=endVal;commentCNTR<NoCommentCommands.length();commentCNTR++)
//        {
//            parameterTree.append("MainHeader");
//            while ((indexcntr<AllIndex.length()))
//            {
//                SeperatedCommand=NoCommentCommands.at(AllIndex.at(indexcntr)).split(" ");
//                parameterTree.append(SeperatedCommand.at(0));
//            }
//        }
    return parameterTree;
}

struct ElementIndex LGPdispers::ParamToIndexes(QStringList NoCommentCommands,QStringList ParamTree)
{
    QList<int> R_Index,L_Index,C_Index,J_Index,V_Index,I_Index,SUB_Index;

    Q_UNUSED(NoCommentCommands)
    Q_UNUSED(ParamTree)

   return {R_Index,L_Index,C_Index,J_Index,V_Index,I_Index,SUB_Index};
}

QStringList LGPdispers::ConstructCommands(struct ElementIndex indexes, QStringList NoCommentCommands)
{
    QStringList dispersCommands={};

    Q_UNUSED(indexes)
    Q_UNUSED(NoCommentCommands)


    return dispersCommands;
}

double LGPdispers::MakeRandNumber(double meanVal, double sigmaVal)
{

    double RandomData;
    sigmaVal = meanVal * sigmaVal/100;

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
            RandomData = result;
        }
    }
    else
    {
        n2_cached = 0;
        RandomData = n2*sigmaVal + meanVal;
    }

    return RandomData;

}

struct ElementIndex LGPdispers::AllocateIndexes(QStringList NoCommentCommands)
{
    QList<int> R_Index={};
    QList<int> L_Index={};
    QList<int> C_Index={};
    QList<int> J_Index={};
    QList<int> V_Index={};
    QList<int> I_Index={};
    QList<int> Sub_Index={};

    QStringList SeperatedCommand;
    QString elementName;



    for (int commentCNTR=0;commentCNTR<NoCommentCommands.length();commentCNTR++)
    {
        QRegExp r = QRegExp("^[RLCBVI]\\S+", Qt::CaseInsensitive);

        QRegExp subcktStartExpression = QRegExp("\\.subckt[^\n]*", Qt::CaseInsensitive);
        QRegExp subcktEndExpression = QRegExp("\\.ends[^\n]*", Qt::CaseInsensitive);

        subcktStartExpression.indexIn(NoCommentCommands.at(commentCNTR));
        subcktEndExpression.indexIn(NoCommentCommands.at(commentCNTR));

        SeperatedCommand=NoCommentCommands.at(commentCNTR).split(" ");
        r.indexIn(SeperatedCommand.at(0));
        elementName = r.cap(0);
        if (!r.isEmpty())
        {
            if (elementName.contains("R",Qt::CaseInsensitive))
            {
                R_Index.append(commentCNTR);
            }else if(elementName.contains("L",Qt::CaseInsensitive))
            {
                L_Index.append(commentCNTR);
            }else if(elementName.contains("C",Qt::CaseInsensitive))
            {
                C_Index.append(commentCNTR);
            }else if(elementName.contains("B",Qt::CaseInsensitive))
            {
                J_Index.append(commentCNTR);
            }else if(elementName.contains("V",Qt::CaseInsensitive))
            {
                V_Index.append(commentCNTR);
            }else if(elementName.contains("I",Qt::CaseInsensitive))
            {
                I_Index.append(commentCNTR);
            }
        }else if (!subcktStartExpression.isEmpty())
        {
            Sub_Index.append(commentCNTR);
        }else if (!subcktEndExpression.isEmpty())
        {
            Sub_Index.append(commentCNTR);
        }
    }

    return {R_Index,L_Index,C_Index,J_Index,V_Index,I_Index,Sub_Index};
}

QStringList LGPdispers::Change_Inductances(QStringList Commands, double sigmaVal)
{
    CalcVals *values= new CalcVals;


    for (int fcntr=0; fcntr<Commands.size();fcntr++)
    {
        bool correctLine = Commands.at(fcntr).contains("H",Qt::CaseInsensitive)
                           & Commands.at(fcntr).contains("L",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains("z",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains("model",Qt::CaseInsensitive)
                           & !Commands.at(fcntr).contains(".subckt",Qt::CaseInsensitive);
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
                double nLval = MakeRandNumber(Lval, sigmaVal);
                if (nLval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString InductanceParamLine = Lcommand.at(0) + "   " + Lcommand.at(1) + "   " + Lcommand.at(2) + "   " + values->convertToUnits(nLval) + "H   fcheck"; //QString::number(nLval)
                    Commands.replace(fcntr,InductanceParamLine);

                }
            }
        }

    }


    return Commands;
}

QStringList LGPdispers::Change_Junctions(QStringList Commands, double sigmaVal)
{
    CalcVals *values= new CalcVals;


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

                double nBval = MakeRandNumber(Bval, sigmaVal);
                if (nBval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString JunctionParamLine = Bcommand.at(0) + "   " + Bcommand.at(1) + "   " + Bcommand.at(2) + "   " + Bcommand.at(3) + "   area=" + values->convertToUnits(nBval);
                    Commands.replace(fcntr,JunctionParamLine);
                }
            }

        }
    }



    return Commands;
}

QStringList LGPdispers::Change_BiasResistances(QStringList Commands, double sigmaVal)
{
    CalcVals *values= new CalcVals;


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
                double nRval = MakeRandNumber(Rval, sigmaVal);
                if (nRval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString BiasParamLine = Rcommand.at(0) + "   " + Rcommand.at(1) + "   " + Rcommand.at(2) + "   " + values->convertToUnits(nRval) + "ohm";
                    Commands.replace(fcntr,BiasParamLine);

                }
            }
        }

    }


    return Commands;
}

QStringList LGPdispers::Change_ShuntResistances(QStringList Commands, double sigmaVal)
{
    CalcVals *values= new CalcVals;


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
                double nRval = MakeRandNumber(Rval, sigmaVal);
                if (nRval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString ShuntParamLine = Rcommand.at(0) + "   " + Rcommand.at(1) + "   " + Rcommand.at(2) + "   " + values->convertToUnits(nRval) + "ohm";
                    Commands.replace(fcntr,ShuntParamLine);

                }
            }
        }

    }


    return Commands;
}

QStringList LGPdispers::Change_Capacitances(QStringList Commands, double sigmaVal)
{

    CalcVals *values= new CalcVals;


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
                double nCval = MakeRandNumber(Cval, sigmaVal);
                if (nCval > 0)
                {
                    //Write the command for the parameter file for the inductance optimization
                    QString CapParamLine = Rcommand.at(0) + "   " + Rcommand.at(1) + "   " + Rcommand.at(2) + "   " + values->convertToUnits(nCval) + "F";
                    Commands.replace(fcntr,CapParamLine);

                }
            }
        }

    }


    return Commands;
}

void LGPdispers::on_toolButtonFilePath_clicked()
{
    QString NetlistFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),documentFolderPath, tr("Netlist Files (*.js *.inp *.cir);;All Files (*)"));
        if (!NetlistFile.isEmpty())
        {
            ui->lineEditFilePath->setText(NetlistFile);
        }
}

void LGPdispers::on_buttonBox_accepted()
{
    QStringList savematerial={};

    simulateall *simclass=new simulateall;

    QString netlistfile=ui->lineEditFilePath->text();
    QFileInfo Myfile(netlistfile);

    QString lgpdispersFile=Myfile.absolutePath()+"/PD"+Myfile.fileName().remove(Myfile.completeSuffix(),Qt::CaseInsensitive)+"cir";


    struct SimParams simParams;
    simParams.pointNum="0";


    struct processedNL aa= simclass->processNetlist(netlistfile);
    OutputFileName = aa.OutFileName;
    QString DataFile = documentFolderPath+OutputFileName;
    columNum = aa.columNumb;
    timestep = aa.tstep;

    struct CommandList MyCommandList=ReadCommands(netlistfile);

    QStringList NewCommands = MyCommandList.NoCommentCommands;
    QStringList TempNewCommands = NewCommands;





    if (ui->tabWidget->currentIndex()==0)
    {


    }

    else if (ui->tabWidget->currentIndex()==1)
    {
            if (ui->checkBoxInd->isChecked())
            {
                TempNewCommands = Change_Inductances(NewCommands,ui->spinBoxInd->value());
                NewCommands = TempNewCommands;
            }

            if (ui->checkBoxRes->isChecked())
            {
                TempNewCommands = Change_BiasResistances(NewCommands,ui->spinBoxRes->value());
                NewCommands = TempNewCommands;
            }

            if (ui->checkBoxCap->isChecked())
            {
                TempNewCommands = Change_Capacitances(NewCommands,ui->spinBoxCap->value());
                NewCommands = TempNewCommands;

            }
            if (ui->checkBoxJJ->isChecked())
            {
                TempNewCommands = Change_Junctions(NewCommands,ui->spinBoxJJ->value());
                NewCommands = TempNewCommands;
            }

            if (ui->checkBoxRS->isChecked())
            {
                TempNewCommands = Change_ShuntResistances(NewCommands,ui->spinBoxSrc->value());
                NewCommands = TempNewCommands;
            }
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


LGPdispers::~LGPdispers()
{
    delete ui;
}


