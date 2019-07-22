#include "simulateall.h"
#include "myglobalvars.h"
#include "mainwindow.h"
#include "calcvals.h"
#include <QFile>
#include <QString>
#include <QDir>
#include <QRegExp>
#include <QProcess>
#include <QVector>



simulateall::simulateall()
{
    //Define variable WaitTime
    //This is the base value.
    WaitTime=200;
    Tstep=10;
}


// Process the netlist and extact the parameters determined by the user.
struct processedNL simulateall::processNetlist(QString FileName)
{
    int columNumb=1;
    double tstep=0;
    QString Icrit="0.1m";
    QString Vg="2.8m";
    QString Rnorm="15";
    QString Cap="0.218p";
    QString OutFileName="/OUT.DAT";
    QStringList Commands;

    QFile file(FileName);
    Commands.clear();
    if (!file.exists())
    {
        return {Vg,Icrit,Rnorm,Cap,tstep,columNumb,OutFileName,Commands};
    }
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {

        return {Vg,Icrit,Rnorm,Cap,tstep,columNumb,OutFileName,Commands};
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList temp = line.split('*');
        if (!temp.at(0).isEmpty())
            Commands.append(temp.at(0));
        temp.clear();
    }
    int j=0;
    for (int i=0;i<Commands.length();i++){
        //QString desired = Commands.at(i).mid(0, Commands.at(i).indexOf("print"));

        if ((j==0) && Commands.at(i).contains("FILE",Qt::CaseInsensitive)){
            QStringList temp1 = Commands.at(i).split(" ");
            OutFileName="/"+temp1.at(temp1.length()-1);
            j++;
        }

        if (Commands.at(i).contains("print",Qt::CaseInsensitive))
            columNumb++;

        if (Commands.at(i).contains("model",Qt::CaseInsensitive)){
            QStringList rcommand={};
            QStringList tempe=Commands.at(i).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    rcommand.append(tempe.at(tempcntr));
            }
            for (int rcntr=0;rcntr<rcommand.length();rcntr++)
            {
                if (rcommand.at(rcntr).contains("Cap",Qt::CaseInsensitive))
                {
                    QRegExp r = QRegExp("(CAP=)(.+)(F)", Qt::CaseInsensitive);
                    r.indexIn(rcommand.at(rcntr));
                    QStringList testlist = r.capturedTexts();
                    Cap = r.cap(2);
                    //for (int tester=0;tester<testlist.length();tester++) Cap=Cap+" "+QString::number(tester)+"="+r.cap(tester);

                }else if(rcommand.at(rcntr).contains("ICRIT",Qt::CaseInsensitive))
                {
                    QRegExp r = QRegExp("(Icrit=)(.+)(A)", Qt::CaseInsensitive);
                    r.indexIn(rcommand.at(rcntr));
                    Icrit = r.cap(2);
                }else if(rcommand.at(rcntr).contains("Rn",Qt::CaseInsensitive))
                {
                    QRegExp r = QRegExp("(Rn=)(.+)(ohm)", Qt::CaseInsensitive);
                    r.indexIn(rcommand.at(rcntr));
                    Rnorm = r.cap(2);
                }else if(rcommand.at(rcntr).contains("Vg",Qt::CaseInsensitive))
                {
                    QRegExp r = QRegExp("(Vg=)(.+)(V)", Qt::CaseInsensitive);
                    r.indexIn(rcommand.at(rcntr));
                    Vg = r.cap(2);
                }

            }

        }

        if (Commands.at(i).contains("tran",Qt::CaseInsensitive)){
            QStringList rcommand={};
            QStringList tempe=Commands.at(i).split(" ");
            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                if (!tempe.at(tempcntr).isEmpty())
                    rcommand.append(tempe.at(tempcntr));
            }
            QRegExp r = QRegExp("^(.+)(ps)$", Qt::CaseInsensitive);
            r.indexIn(rcommand.at(1));
            tstep = r.cap(1).toDouble();
        }



    }
    if (j==0) OutFileName="NoFileExist";
    file.close();
    return {Vg,Icrit,Rnorm,Cap,tstep,columNumb,OutFileName,Commands};
}


// Simulation of the methods that are characterized by the Curve method.
// I-V curve and BER are part of this one
struct ConsoleOutputs simulateall::simulateivcurve(QString FileName, int SimulatorIndex, int points, int loopCnt,bool IVstatistical)
{
    QVector<double> I(points*loopCnt);
    QVector<double> V(points*loopCnt);

    //The vector for different waittimes
    QVector<double> waittimevals(points);

    if (points%2==1)
        points--;

    bool validDatax=true;
    bool validDatay=true;
    QString OutFile= QDir::currentPath()+OutputFileName;
    struct ConsoleOutputs out = simulatenetlist(FileName,SimulatorIndex);

    // copy the output to tempfile and delete the outputfile
    if (QFile::exists(TempOutFile))
        QFile::remove(TempOutFile);
    if(QFile::copy(OutFile, TempOutFile))
        QFile::remove(OutFile);
    else
      return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};

    //read the data from output that is TempOutFile now
    QFile file(TempOutFile);
    QStringList fields;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    }

    QTextStream in(&file);

    //Put the data into the RawData global variable
    bool firstline=true;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (delimator=="," && firstline)
        firstline=false;
        else
        fields.append(line.split(delimator));
    }
    file.close();
    in.flush();

    RawData.clear();
    for(int i=0 ; i < fields.length() ; i++){
        if (!fields.at(i).isEmpty())
        {
            RawData.append(fields.at(i));
        }
    }
    fields.clear();

    //Take the second colomn as I and third as V
    QVector <double> DataI(RawData.length()/columNum);
    QVector <double> DataV(RawData.length()/columNum);

        int j=0;
        for(int i=0 ; i < RawData.length() ; i++)
        {
            if (i%columNum==1) DataI[j]=RawData.at(i).toDouble(&validDatax);
            else if (i%columNum==2)
            {
                DataV[j]=RawData.at(i).toDouble(&validDatay);
                if (validDatax && validDatay)
                {
                    j++;
                    //stream << QString::number(j)+" : "+QString::number(DataI[j])+" , "+QString::number(DataV[j])<<'\n';
                }

            }
        }

        //    Calculate the I and V of the curve as the I became stable. Tstep is the
        //    rise time of the current and WaitTime is the settling time of the
        //    voltage. The voltage is averaged over the WaitTime. If graph is too noisy
        //    or unstable, increase TimeStep and WaitTime. The function would be
        //    slower, but the result would be more accurate.

        if (IVstatistical)
        {
            int sumWaitTime=0;
            QFile waitfile(QDir::currentPath()+"/testOutPut.DAT");
            if (!waitfile.open(QFile::ReadOnly | QFile::Text))
            {
                return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
            }

            QTextStream waitstream(&waitfile);
            QVector<int> waittimevals(points*loopCnt);

            for (int testcntr=0;testcntr<points*loopCnt;testcntr++)
                    waittimevals[testcntr]=waitstream.readLine().toInt();

                waitfile.close();
                waitstream.flush();

            for (int IVcntr=0;IVcntr<points*loopCnt;IVcntr++)
            {
                sumWaitTime=sumWaitTime+waittimevals[IVcntr];
                int firstindex=static_cast<int>(round((Tstep*(IVcntr+1)+sumWaitTime-waittimevals[IVcntr])/timestep));
                int secondindex=static_cast<int>(round((Tstep*(IVcntr+1)+sumWaitTime)/timestep));
                I[IVcntr]=DataI[firstindex];


                //Average the voltage over the WaitTime

                double Vtemp=0;
                for (int Vsum=firstindex;Vsum<secondindex;Vsum++)
                    Vtemp = Vtemp+DataV[Vsum]/(secondindex-firstindex);
                V[IVcntr]=Vtemp;
            }
        }

        else{

            for (int IVcntr=0;IVcntr<points*loopCnt;IVcntr++)
            {

                int firstindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1)-WaitTime)/timestep));
                int secondindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1))/timestep));
                I[IVcntr]=DataI[firstindex];


                //Average the voltage over the WaitTime

                double Vtemp=0;
                for (int Vsum=firstindex;Vsum<secondindex;Vsum++)
                    Vtemp = Vtemp+DataV[Vsum]/(secondindex-firstindex);
                V[IVcntr]=Vtemp;
            }
    }
    DataI.clear();
    DataV.clear();
    //Open the outputfile to put the I-V values into it
    QFile file2(OutFile);
    if (!file2.open(QFile::WriteOnly | QFile::Text)){
        return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream stream(&file2);
        for (int fcntr=0; fcntr<V.length();fcntr++)
            stream << QString::number(V[fcntr])+delimator+QString::number(I[fcntr])<<'\n';
        file2.close();
        stream.flush();

    }

    I.clear();
    V.clear();

    return out;
}


// This functions first run a simple I-V and process the data to make a better I-V curve

struct ConsoleOutputs simulateall::simulateivtest(QString FileName, int SimulatorIndex,int points, int loopCnt)
{
    QVector<double> I(points*loopCnt);
    QVector<double> V(points*loopCnt);
    QString testingIV= QDir::currentPath()+"/Data/testingIV.tmp";

    //The vector for different waittimes
    QVector<int> waittimevals(points*loopCnt);
//   QVector<double> CurrentSteps(points*loopCnt);
    QVector<double> diff2Volt(points*loopCnt);
    double diff2VoltMax;

    if (points%2==1)
        points--;

    bool validDatax=true;
    bool validDatay=true;
    QString OutFile= QDir::currentPath()+OutputFileName;
    QString testOutFile= QDir::currentPath()+"/testOutPut.DAT";

    struct ConsoleOutputs out = simulatenetlist(FileName,SimulatorIndex);

    // copy the output to tempfile and delete the outputfile
    if (QFile::exists(testingIV))
        QFile::remove(testingIV);
    if(QFile::copy(OutFile, testingIV))
        QFile::remove(OutFile);
    else
      return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};

    //read the data from output that is TempOutFile now
    QFile file(testingIV);
    QStringList fields;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    }

    QTextStream in(&file);

    //Put the data into the RawData global variable
    bool firstline=true;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (delimator=="," && firstline)
        firstline=false;
        else
        fields.append(line.split(delimator));
    }
    file.close();
    in.flush();

    RawData.clear();
    for(int i=0 ; i < fields.length() ; i++){
        if (!fields.at(i).isEmpty())
        {
            RawData.append(fields.at(i));
        }
    }
    fields.clear();

    //Take the second colomn as I and third as V
    QVector <double> DataI(RawData.length()/columNum);
    QVector <double> DataV(RawData.length()/columNum);

    int j=0;
    for(int i=0 ; i < RawData.length() ; i++)
    {
        if (i%columNum==1) DataI[j]=RawData.at(i).toDouble(&validDatax);
        else if (i%columNum==2)
        {
            DataV[j]=RawData.at(i).toDouble(&validDatay);
            if (validDatax && validDatay)
            {
                j++;
                //stream << QString::number(j)+" : "+QString::number(DataI[j])+" , "+QString::number(DataV[j])<<'\n';
            }

        }
    }

    //    Calculate the I and V of the curve as the I became stable. Tstep is the
    //    rise time of the current and WaitTime is the settling time of the
    //    voltage. The voltage is averaged over the WaitTime. If graph is too noisy
    //    or unstable, increase TimeStep and WaitTime. The function would be
    //    slower, but the result would be more accurate.

    for (int IVcntr=0;IVcntr<points*loopCnt;IVcntr++)
    {

        int firstindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1)-WaitTime)/timestep));
        int secondindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1))/timestep));
        I[IVcntr]=DataI[firstindex];


        //Average the voltage over the WaitTime

        double Vtemp=0;
        for (int Vsum=firstindex;Vsum<secondindex;Vsum++)
            Vtemp = Vtemp+DataV[Vsum]/(secondindex-firstindex);
        V[IVcntr]=Vtemp;

    }

    DataI.clear();
    DataV.clear();


    //double Vthreshold=V[V.length()/2]-V[0];
    //Process the data for finding the points for higher wait time
    diff2Volt[0]=std::abs(V[2]-2*V[1]+V[0]);
    diff2VoltMax=diff2Volt[0];
    diff2Volt[V.length()]=std::abs(V[V.length()]-2*V[V.length()-1]+V[V.length()-2]);
    for (int fcntr=1; fcntr<V.length()-1;fcntr++)
    {
        diff2Volt[fcntr]=std::abs(V[fcntr+1]-2*V[fcntr]+V[fcntr-1]);
        if (diff2Volt[fcntr]>diff2VoltMax)
            diff2VoltMax=diff2Volt[fcntr];
    }

    for (int fcntr=0; fcntr<V.length();fcntr++)
    {
        waittimevals[fcntr]=static_cast<int>(1+20*diff2Volt[fcntr]/diff2VoltMax)*WaitTime;
    }


    //Open the outputfile to put the I-V values into it
    QFile file2(testOutFile);
    if (!file2.open(QFile::WriteOnly | QFile::Text)){
        return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream stream(&file2);
        for (int fcntr=0; fcntr<V.length();fcntr++)
            stream << QString::number(waittimevals[fcntr])+'\n';
        file2.close();
        stream.flush();

    }

    I.clear();
    V.clear();
    waittimevals.clear();
    QFile::remove(testingIV);
    return out;
}


// Moving average I-V calculation
struct ConsoleOutputs simulateall::simulateivnew(QString FileName, int SimulatorIndex,int points)
{

    //The vector for different waittimes
    QVector<double> waittimevals(points);

    if (points%2==1)
        points--;

    bool validDatax=true;
    bool validDatay=true;
    QString OutFile= QDir::currentPath()+OutputFileName;
    struct ConsoleOutputs out = simulatenetlist(FileName,SimulatorIndex);

    // copy the output to tempfile and delete the outputfile
    if (QFile::exists(TempOutFile))
        QFile::remove(TempOutFile);
    if(QFile::copy(OutFile, TempOutFile))
        QFile::remove(OutFile);
    else
      return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};

    //read the data from output that is TempOutFile now
    QFile file(TempOutFile);
    QStringList fields;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    }

    QTextStream in(&file);

    //Put the data into the RawData global variable
    bool firstline=true;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (delimator=="," && firstline)
        firstline=false;
        else
        fields.append(line.split(delimator));
    }
    file.close();
    in.flush();

    RawData.clear();
    for(int i=0 ; i < fields.length() ; i++){
        if (!fields.at(i).isEmpty())
        {
            RawData.append(fields.at(i));
        }
    }
    fields.clear();

    //Take the second colomn as I and third as V
    QVector <double> DataI(RawData.length()/columNum);
    QVector <double> DataV(RawData.length()/columNum);

        int j=0;
        for(int i=0 ; i < RawData.length() ; i++)
        {
            if (i%columNum==1) DataI[j]=RawData.at(i).toDouble(&validDatax);
            else if (i%columNum==2)
            {
                DataV[j]=RawData.at(i).toDouble(&validDatay);
                if (validDatax && validDatay)
                {
                    j++;
                }
            }
        }

        //Moving average
        QVector<double> I= movingAverage(DataI,points);
        QVector<double> V= movingAverage(DataV,points);

        DataI.clear();
        DataV.clear();

        //Open the outputfile to put the I-V values into it
        QFile file2(OutFile);
        if (!file2.open(QFile::WriteOnly | QFile::Text)){
            return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
        }else
        {
            QTextStream stream(&file2);
            for (int fcntr=0; fcntr<V.length();fcntr++)
                stream << QString::number(V[fcntr])+delimator+QString::number(I[fcntr])<<'\n';
            file2.close();
            stream.flush();
        }

        I.clear();
        V.clear();

        return out;
}


// Moving average function
QVector<double> simulateall::movingAverage(QVector<double> datain,int windowSize)
{

    QVector<double> dataOut(datain.length());

    if (datain.length()>windowSize){
        dataOut[0]=datain[0]/windowSize;

        for (int avecntr=1 ; avecntr<windowSize ; avecntr++)
          dataOut[avecntr]=dataOut[avecntr-1]+datain[avecntr]/windowSize;

        for (int avecntr=windowSize ; avecntr<datain.length() ; avecntr++)
          dataOut[avecntr]=dataOut[avecntr-1]+(datain[avecntr]-datain[avecntr-windowSize])/windowSize;
    } else{
        for (int avecntr=1 ; avecntr<datain.length() ; avecntr++)
          dataOut[avecntr]=dataOut[avecntr-1]+datain[avecntr]/windowSize;
    }

    return dataOut;

}

//Run the simulator for the input netlist and generate the output
struct ConsoleOutputs simulateall::simulatenetlist(QString FileName, int SimulatorIndex)
{
    QString OutFile= QDir::currentPath()+OutputFileName;
    //QString OutFile= "."+OutputFileName;

    QString simstderr="";
    QString simstdout="";
//    QString commandline=DataPath+"/jsim_n.exe "+FileName;

    QProcess process;
    //QString OSname=QSysInfo::productType();

    switch(SimulatorIndex){
    case 0:
        #ifdef __linux__
                    process.start("./jsim_n "+FileName);
                    process.waitForFinished(-1); // will wait forever until finished
        #elif _WIN32

//        QProcess::execute(commandline);

                    process.setProgram("cmd.exe");
                    process.setArguments({"/c",DataPath+"/jsim_n.exe", FileName});
                    process.setCreateProcessArgumentsModifier([] (
                        QProcess::CreateProcessArguments *args) {
                                    args->flags &= CREATE_NO_WINDOW;});
                    //qint64 pid;
                    //process.startDetached(&pid);
                    process.start();
                    process.waitForFinished(-1);

        #else
                    process.start("./jsim_n "+FileName);
                    process.waitForFinished(-1); // will wait forever until finished
        #endif
//        if (OSname=="windows"){
//            process.setProgram("cmd.exe");
//            process.setArguments({"/c",DataPath+"/jsim_n.exe", FileName});
//            process.setCreateProcessArgumentsModifier([] (
//                QProcess::CreateProcessArguments *args) {
//                            args->flags &= CREATE_NO_WINDOW;});
//            process.start();
//            process.waitForFinished(-1);
//        }
//        else{
//            process.start("./jsim_n "+FileName);
//            process.waitForFinished(-1); // will wait forever until finished
//        }
        simstdout = process.readAllStandardOutput();
        simstderr = process.readAllStandardError();
        delimator=" ";
        break;
    case 1:
        #ifdef __linux__
                    process.start("./JoSIM -o "+OutFile+" "+FileName);
                    process.waitForFinished(-1); // will wait forever until finished
        #elif _WIN32
                    process.setProgram("cmd.exe");
                    process.setArguments({"/c",DataPath+"/JoSIM_n.exe","-o", OutFile,FileName});
                    process.setCreateProcessArgumentsModifier([] (
                        QProcess::CreateProcessArguments *args) {
                                    args->flags &= CREATE_NO_WINDOW;});
                    process.start();
                    process.waitForFinished(-1);

        #else
                    process.start("./JoSIM -o "+OutFile+" "+FileName);
                    process.waitForFinished(-1); // will wait forever until finished
        #endif
//        if (OSname=="windows"){
//            process.setProgram("cmd.exe");
//            process.setArguments({"/c",DataPath+"/JoSIM_n.exe","-o", OutFile,FileName});
//            process.setCreateProcessArgumentsModifier([] (
//                QProcess::CreateProcessArguments *args) {
//                            args->flags &= CREATE_NO_WINDOW;});
//            process.start();
//            process.waitForFinished(-1);
//        }
//        else{
//            process.start("./JoSIM -o "+OutFile+" "+FileName);
//            process.waitForFinished(-1); // will wait forever until finished
//        }
        simstdout = process.readAllStandardOutput();
        simstderr = process.readAllStandardError();
        delimator=",";
        break;
    default:
        simstderr="Simulator not recognized!";
    }

    return {simstdout,simstderr};
}

// BER calculations are done here
struct ConsoleOutputs simulateall::simulateBER(int SimulatorIndex,float Multiplyer,double SubParamVal)
{
    QString OutFile= QDir::currentPath()+OutputFileName;
    //TempFileName=QDir::currentPath()+"/Data/TempNetlistBER.tmp";
    ConsoleOutputs simstd={"",""};
    float BER=100;
    long DataIn=1;
    long DataOut=0;

    simstd=simulatenetlist(TempFileName,SimulatorIndex);

    QFile file(QDir::currentPath()+OutputFileName);
    QStringList lastline;
    QStringList PhaseData;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    }

    QTextStream in(&file);
    QString line;
    // Get the last line of the output

    while(!in.atEnd()) line = in.readLine();
    lastline.append(line.split(delimator));
    file.close();
    in.flush();

    for(int i=0 ; i < lastline.length() ; i++)
        if (!lastline.at(i).isEmpty())
        {
            PhaseData.append(lastline.at(i));
        }
    for(int i=0 ; i < PhaseData.length() ; i++)
        {
            if (i%columNum==1) DataIn=static_cast<int>(PhaseData.at(i).toDouble()/(2*Pi));
            if (i%columNum==2) DataOut=static_cast<int>(PhaseData.at(i).toDouble()/(2*Pi));
        }

    lastline.clear();
    PhaseData.clear();

    if (DataIn==0)
        BER=100;
    else if (DataIn!=0 && abs(DataIn-DataOut)<=Multiplyer)
        BER=0;
    else
    {
        BER=std::abs(100*(1-DataOut*Multiplyer/DataIn));
        if (BER>100)
            BER=100;
    }


    QString BERtempfile=QDir::currentPath()+"/Data/TempOutputBER.DAT";
    QFile file2(BERtempfile);

    if (!file2.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
        return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream BERstream(&file2);
        BERstream << QString::number(SubParamVal)+delimator+QString::number(static_cast<double>(BER))<<'\n';
        //<<delimator<<delimator<<QString::number(DataIn)<<delimator<<delimator<<QString::number(DataOut)
        file2.close();
        BERstream.flush();
    }

    QString outmessage="The BER calculation error is : %"+QString::number(static_cast<double>(100*abs(DataIn-DataOut)/DataIn));

    simstd.ConsolOut.append(outmessage);
    return simstd;

}


//Frequency response calculations are done here
struct ConsoleOutputs simulateall::simulateFreq(int SimulatorIndex,QVector<double> Multiplyers,double SubParamVal)
{
    QString OutFile= QDir::currentPath()+OutputFileName;
    //TempFileName=QDir::currentPath()+"/Data/TempNetlistFreq.tmp";
    ConsoleOutputs simstd={"",""};
    QVector<long> DataVals;

    simstd=simulatenetlist(TempFileName,SimulatorIndex);

    QFile file(QDir::currentPath()+OutputFileName);
    QStringList lastline;
    QStringList PhaseData;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {"Problem with permissions!","There is a problem in file creation. Check the permissions!"};
    }

    QTextStream in(&file);
    QString line;
    // Get the last line of the output

    while(!in.atEnd()) line = in.readLine();
    lastline.append(line.split(delimator));
    file.close();
    in.flush();

    for(int i=0 ; i < lastline.length() ; i++)
        if (!lastline.at(i).isEmpty())
            PhaseData.append(lastline.at(i));

    for(int i=0 ; i < PhaseData.length()-1 ; i++)
         DataVals.append(static_cast<int>(PhaseData.at(i+1).toDouble()/(2*Pi)));


    lastline.clear();
    PhaseData.clear();

    QString Freqtempfile=QDir::currentPath()+"/Data/TempOutputFreq.DAT";
    QFile file2(Freqtempfile);

    if (!file2.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
        return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream Freqstream(&file2);
        Freqstream << QString::number(SubParamVal/1e9)+delimator;
        for(int j=0 ; j < columNum-1 ; j++)
        Freqstream << QString::number(Multiplyers[j]*DataVals[j])+delimator;
        Freqstream<<'\n';
        file2.close();
        Freqstream.flush();
    }

    return simstd;
}


//Create new netlist based on the selections of the user. Considering the Temerature and Noise
//It will create a series of PWL currents to step the I, for I-V simulation mode
//Must get the parameter and the value for shifting.

QString simulateall::make_new_netlist(bool noise,QString NetlistFile, struct SimParams simParams,int SimulatorIndex,int IVstatistical)
{
    CalcVals *values= new CalcVals;
    double stepVal=0;
    bool firstencounter=true;

    // For case6 new I-V
    int timeInterval=10;
    if (IVstatistical==1)
        timeInterval=30;

    if (IVstatistical==1)
        WaitTime=50;
    else if (IVstatistical==2)
        WaitTime=250;
    int waitTimeSum=0;


    struct processedNL netlist = processNetlist(NetlistFile);
    if (netlist.OutFileName=="NoFileExist"){
        return "There is no output specified in the netlist!";
    } else if (netlist.columNumb==1){
        return "There is no outputs specified in the netlist!";
    } else {

        //Set the parameters for making the new netlist

        if (SimulatorIndex==0)
            OutputFileName = netlist.OutFileName;

        columNum = netlist.columNumb;
        timestep = netlist.tstep;


        //New critical current based on temperature

        QString Icritnew = values->convertToUnits(values->criticalCurrent(values->convertToValues(netlist.Icrit)));
        QString Vg=values->convertToUnits(values->Vgap(0));

        //Value of step for shifting parameters
        if (simParams.pointNum.toInt()>1)
        stepVal=2*(values->convertToValues(simParams.maxVal)-values->convertToValues(simParams.minVal))/(simParams.pointNum.toDouble()-1);


        //TempFileName is temporary file generated by program
        //This is the netlist that we simulate

        QFile file(TempFileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
            return file.errorString();
        }
        else{
                QTextStream stream(&file);
                bool tempIndicator=true;

                for (int fcntr=0; fcntr<netlist.Commands.size();fcntr++)
                {

                    //Change the value of critical current
                    if (netlist.Commands.at(fcntr).contains("model",Qt::CaseInsensitive))
                    {
                        //stream << "*** Cap Val="+netlist.Cap+" Icrit="+netlist.Icrit+" Icritnew="+Icritnew<<'\n';
                        QString newline=netlist.Commands.at(fcntr);
                        newline.replace(netlist.Icrit, Icritnew);
                        newline.replace(netlist.Vg,Vg);
                        netlist.Commands.replace(fcntr,newline);
                    }

                    // change netlist depending on the simulation type.
                    switch (Simindex)
                    {

                    // Normal time transient simulation
                    case 0:
                            stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    // I-V characteristic simulation
                    case 1:
                        // Find the line with parameter and make the changes
                        if (firstencounter && netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
                        {
                            QStringList rcommand={};
                            QStringList tempe=netlist.Commands.at(fcntr).split(" ");
                            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                                if (!tempe.at(tempcntr).isEmpty())
                                    rcommand.append(tempe.at(tempcntr));
                            }

                            int halfloopVal=simParams.pointNum.toInt()/2;
                            QString newLine="*** Problem occured in loop! ***";

                            // If the user enters less than two points, only Imin is applied to the system.

                            if (simParams.pointNum.toInt()<2)
                            {
                                newLine="IPWL0   "+rcommand.at(1)+
                                        "   "+rcommand.at(2)+"   "+"PWL(0ps 0mA "+ QString::number(Tstep)+
                                        "ps "+ simParams.minVal+"A)";
                                stream << newLine <<'\n';
                            }


                            //When the number of points is bigger than one, the loop starts and depending on the nuber of periods,
                            //  the loop would generate PWL signals.

                            else{

                                //If the I-V curve in testing stage is measured
                                if (IVstatistical==1)
                                    for (int loopcntr=0;loopcntr<simParams.subParam.toInt();loopcntr++)
                                    {

                                        //First half of the loop
                                        for (int pointCntr=0;pointCntr<halfloopVal;pointCntr++)
                                        {
                                            // Make the index for the PWL current name
                                            int PWLindex = loopcntr*simParams.pointNum.toInt()+pointCntr;


                                            // The first part is applying the Imin to the loop
                                            if (PWLindex == 0)
                                                newLine="IPWL0   "+rcommand.at(1)+
                                                        "   "+rcommand.at(2)+"   "+"PWL(0ps 0mA "+ QString::number(Tstep)+
                                                        "ps "+ simParams.minVal+"A)";

                                            //Next we increase the I value by steps
                                            else
                                                newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                    + "PWL(0ps 0mA "+ QString::number((Tstep+WaitTime)*PWLindex)+"ps "
                                                    +"0mA " + QString::number((Tstep+WaitTime)*PWLindex+Tstep)+"ps "+values->convertToUnits(stepVal)+"A)";

                                            //Write the value to the temporary netlist file
                                            stream << newLine <<'\n';
                                        }

                                        for (int pointCntr=halfloopVal;pointCntr<2*halfloopVal;pointCntr++)
                                        {

                                            int PWLindex = 2*loopcntr*halfloopVal+pointCntr;

                                            newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                    +"PWL(0ps 0mA "+ QString::number((Tstep+WaitTime)*PWLindex)+"ps 0mA "
                                                    + QString::number((Tstep+WaitTime)*PWLindex+Tstep)+"ps "+values->convertToUnits(-1*stepVal)+"A)";

                                            //Write the value to the temporary netlist file
                                            stream << newLine <<'\n';

                                        }
                                    }

                                //If the I-V curve in statistical stage is measured
                                else if (IVstatistical==2)
                                {
                                    //Load the datafile containing the infornmation for Waittimes
                                    QString testOutFile= QDir::currentPath()+"/testOutPut.DAT";
                                    QFile newfile(testOutFile);
                                    if (!newfile.open(QFile::ReadOnly | QFile::Text)){
                                        return newfile.errorString();
                                    }
                                    else{
                                            QTextStream waitstream(&newfile);
                                            QVector<int> waittimevals(simParams.subParam.toInt()*simParams.pointNum.toInt());
                                            for (int testcntr=0;testcntr<simParams.subParam.toInt()*simParams.pointNum.toInt();testcntr++)
                                                    waittimevals[testcntr]=waitstream.readLine().toInt();

                                            //testing to see if we read correctly


                                                newfile.close();
                                                waitstream.flush();


                                    //Generating the netlist                           

                                    for (int loopcntr=0;loopcntr<simParams.subParam.toInt();loopcntr++)
                                    {

                                        //First half of the loop
                                        for (int pointCntr=0;pointCntr<halfloopVal;pointCntr++)
                                        {
                                            // Make the index for the PWL current name
                                            int PWLindex = loopcntr*simParams.pointNum.toInt()+pointCntr;
                                            waitTimeSum=waitTimeSum+waittimevals[PWLindex];


                                            // The first part is applying the Imin to the loop
                                            if (PWLindex == 0)
                                                newLine="IPWL0   "+rcommand.at(1)+
                                                        "   "+rcommand.at(2)+"   "+"PWL(0ps 0mA "+ QString::number(Tstep)+
                                                        "ps "+ simParams.minVal+"A)";

                                            //Next we increase the I value by steps
                                            else
                                                newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                    + "PWL(0ps 0mA "+ QString::number(Tstep*PWLindex+waitTimeSum)+"ps "
                                                    +"0mA " + QString::number(Tstep*(PWLindex+1)+waitTimeSum)+"ps "+values->convertToUnits(stepVal)+"A)";

                                            //Write the value to the temporary netlist file
                                            stream << newLine <<'\n';
                                        }

                                        for (int pointCntr=halfloopVal;pointCntr<2*halfloopVal;pointCntr++)
                                        {

                                            int PWLindex = 2*loopcntr*halfloopVal+pointCntr;
                                            waitTimeSum=waitTimeSum+waittimevals[PWLindex];

                                            newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                    +"PWL(0ps 0mA "+ QString::number(Tstep*PWLindex+waitTimeSum)+"ps 0mA "
                                                    + QString::number((Tstep+1)*PWLindex+waitTimeSum)+"ps "+values->convertToUnits(-1*stepVal)+"A)";

                                            //Write the value to the temporary netlist file
                                            stream << newLine <<'\n';

                                        }
                                    }
                                }
                                }

                                //If the I-V curve is normally measured
                                else
                                    for (int loopcntr=0;loopcntr<simParams.subParam.toInt();loopcntr++)
                                {
                                    //First half of the loop
                                    for (int pointCntr=0;pointCntr<halfloopVal;pointCntr++)
                                    {
                                        // Make the index for the PWL current name
                                        int PWLindex = loopcntr*simParams.pointNum.toInt()+pointCntr;


                                        // The first part is applying the Imin to the loop
                                        if (PWLindex == 0)
                                            newLine="IPWL0   "+rcommand.at(1)+
                                                    "   "+rcommand.at(2)+"   "+"PWL(0ps 0mA "+ QString::number(Tstep)+
                                                    "ps "+ simParams.minVal+"A)";

                                        //Next we increase the I value by steps
                                        else
                                            newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                + "PWL(0ps 0mA "+ QString::number((Tstep+WaitTime)*PWLindex)+"ps "
                                                +"0mA " + QString::number((Tstep+WaitTime)*PWLindex+Tstep)+"ps "+values->convertToUnits(stepVal)+"A)";

                                        //Write the value to the temporary netlist file
                                        stream << newLine <<'\n';
                                    }

                                    for (int pointCntr=halfloopVal;pointCntr<2*halfloopVal;pointCntr++)
                                    {

                                        int PWLindex = 2*loopcntr*halfloopVal+pointCntr;

                                        newLine="IPWL"+QString::number(PWLindex)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                +"PWL(0ps 0mA "+ QString::number((Tstep+WaitTime)*PWLindex)+"ps 0mA "
                                                + QString::number((Tstep+WaitTime)*PWLindex+Tstep)+"ps "+values->convertToUnits(-1*stepVal)+"A)";

                                        //Write the value to the temporary netlist file
                                        stream << newLine <<'\n';

                                    }
                                }
                            }


                            // One increament to counter to skip the Main Parameters orginal value.
                            fcntr++;
                            firstencounter=false;
                        }


                        //Change the simulation time for the new simulation, will be the last value from above

                        if (netlist.Commands.at(fcntr).contains("tran",Qt::CaseInsensitive)){
                            QStringList rcommand={};
                            QStringList tempe=netlist.Commands.at(fcntr).split(" ");
                            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                                if (!tempe.at(tempcntr).isEmpty())
                                    rcommand.append(tempe.at(tempcntr));
                            }
                            //sample line: ".TRAN 0.5PS 500PS 0.1PS 0.1PS"
                            int endtime = (2*simParams.subParam.toInt()*(simParams.pointNum.toInt()/2))*(Tstep+WaitTime)+WaitTime;

                            if (IVstatistical==2)
                                endtime = (2*simParams.subParam.toInt()*(simParams.pointNum.toInt()/2))*Tstep+waitTimeSum+WaitTime;

                            QString newline = rcommand.at(0)+" "+rcommand.at(1)+" "+QString::number(endtime)
                                    +"PS "+rcommand.at(3)+" "+rcommand.at(4);
                            netlist.Commands.replace(fcntr,newline);

                        }

                        //Write the rest of the file to the netlist
                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    // Parametric sweep characteristic
                    case 2:

                        if (simParams.subParam.contains("<*>") && netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
                        {

                            //Replacing the new parameter applied from Main window to the simulation line
                            QRegExp r = QRegExp("^(-?\\d+\\.*\\d*[nµumKG]?)(.*)(<\\*>)(-?\\d+\\.*\\d*[nµumKG]?)(.*)$", Qt::CaseInsensitive);
                            r.indexIn(simParams.subParam);
                            QString newline=netlist.Commands.at(fcntr);
                            newline.replace(r.cap(1)+r.cap(2),r.cap(4)+r.cap(2),Qt::CaseInsensitive);
                            netlist.Commands.replace(fcntr,newline);
                            titleVals.append(r.cap(4)+r.cap(2));

                            if (r.cap(2).compare("Hz",Qt::CaseInsensitive)==0)
                                titleVals.append("Frequency = ");
                            else if (r.cap(2).compare("a",Qt::CaseInsensitive)==0)
                                titleVals.append("Current = ");
                            else if (r.cap(2).compare("v",Qt::CaseInsensitive)==0)
                                titleVals.append("Voltage = ");
                            else if (r.cap(2).compare("H",Qt::CaseInsensitive)==0)
                                titleVals.append("Inductance = ");
                            else if (r.cap(2).compare("Ohm",Qt::CaseInsensitive)==0)
                                titleVals.append("Resistance = ");
                            else if (r.cap(2).compare("F",Qt::CaseInsensitive)==0)
                                titleVals.append("Capacitance = ");
                            else
                                titleVals.append("Parameter = ");
                        }
                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    // Temperature sweep characteristic
                    case 3:
                        if (tempIndicator){
                            titleVals.append(QString::number(temperature)+" K");
                            titleVals.append("Temperature = ");
                            tempIndicator=false;
                        }

                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    // Bit Error rate calculation
                    case 4:

                        if (simParams.subParam.contains("<*>") && netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
                        {

                            //Replacing the new parameter applied from Main window to the simulation line
                            QRegExp r = QRegExp("^(-?\\d+\\.*\\d*[nµumKG]?)(.*)(<\\*>)(-?\\d+\\.*\\d*[nµumKG]?)(.*)$", Qt::CaseInsensitive);
                            r.indexIn(simParams.subParam);
                            QString newline=netlist.Commands.at(fcntr);
                            newline.replace(r.cap(1)+r.cap(2),r.cap(4)+r.cap(2),Qt::CaseInsensitive);
                            netlist.Commands.replace(fcntr,newline);
                        }

                        //Changing the outputs to phase output
                        if (netlist.Commands.at(fcntr).contains("print",Qt::CaseInsensitive))
                        {
                            QString newline=netlist.Commands.at(fcntr);
                            if (netlist.Commands.at(fcntr).contains("devv",Qt::CaseInsensitive))
                                newline.replace("devv","PHASE",Qt::CaseInsensitive);
                            else if (netlist.Commands.at(fcntr).contains("devi",Qt::CaseInsensitive))
                                newline.replace("devi","PHASE",Qt::CaseInsensitive);
                            else if (netlist.Commands.at(fcntr).contains("phase",Qt::CaseInsensitive))
                                newline.replace("phase","PHASE",Qt::CaseInsensitive);
                            else newline="";


                            netlist.Commands.replace(fcntr,newline);

                        }

                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    // Frequency sweep calculation
                    case 5:
                        if (firstencounter && simParams.subParam.contains("<*>") && netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
                        {

                            //Replacing the new parameter applied from Main window to the simulation line
                            QRegExp r = QRegExp("^(-?\\S.*)(<\\*>)(-?\\d+\\.*\\d*[nµumKG]?)$", Qt::CaseInsensitive);
                            r.indexIn(simParams.subParam);
                            QString nodename=r.cap(1);
                            QString paramval=r.cap(3);
                            QString newline="VINFREQ "+nodename+" 0 PWL(0ps 0mv 10ps "+paramval+"v)";
                            stream << newline<<'\n';
                            newline="BINFREQ "+nodename+" 0 jjmod area=2.16";
                            stream << newline<<'\n';
                            newline="RSINFREQ "+nodename+" 0 1.73ohm *SHUNT=3.73";
                            stream << newline<<'\n';
                            firstencounter=false;
                        }


                        //Changing the outputs to phase output
                        if (netlist.Commands.at(fcntr).contains("print",Qt::CaseInsensitive))
                        {
                            QString newline=netlist.Commands.at(fcntr);
                            if (netlist.Commands.at(fcntr).contains("devv",Qt::CaseInsensitive))
                                newline.replace("devv","PHASE",Qt::CaseInsensitive);
                            else if (netlist.Commands.at(fcntr).contains("devi",Qt::CaseInsensitive))
                                newline.replace("devi","PHASE",Qt::CaseInsensitive);
                            else if (netlist.Commands.at(fcntr).contains("phase",Qt::CaseInsensitive))
                                newline.replace("phase","PHASE",Qt::CaseInsensitive);
                            else {
                                newline="";
                                columNum--;
                            }

                            netlist.Commands.replace(fcntr,newline);
                        }

                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                   // Moving average IV characteristic simulation
                    case 6:

                        if (firstencounter && netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
                        {

                            int loopcnt=simParams.subParam.toInt();
                            QStringList rcommand={};
                            QStringList tempe=netlist.Commands.at(fcntr).split(" ");
                            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                                if (!tempe.at(tempcntr).isEmpty())
                                    rcommand.append(tempe.at(tempcntr));
                            }

                            QString newLine="*** Problem occured in loop! ***";

                            //When the number of loops is bigger than one, the loop starts and depending on the number of periods,
                            //  the loop would generate PWL signals.

                                    for (int loopcntr=0;loopcntr<loopcnt;loopcntr++)
                                    {

                                            int PWLindex = loopcntr*timeInterval*4;


                                            // The first part is applying the Imin to the loop
                                            if (PWLindex == 0)
                                                newLine="IPWL0   "+rcommand.at(1)+
                                                        "   "+rcommand.at(2)+"   " + "PWL(0ps 0mA "+QString::number(timeInterval)+"ns "
                                                        +simParams.maxVal + "A  "+QString::number(timeInterval*3)+"ns  " + simParams.minVal + "A   "
                                                        +QString::number(timeInterval*4)+"ns  0mA)";

                                            //Next we increase the I value by steps
                                            else
                                                newLine="IPWL"+QString::number(loopcntr)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "
                                                        + "PWL(0ps 0mA "+QString::number(PWLindex)+"ns 0mA"+QString::number(PWLindex+timeInterval)+"ns "
                                                        + simParams.maxVal + "A  "+QString::number(PWLindex+timeInterval*3)+"ns  " + simParams.minVal + "A   "
                                                        + QString::number(PWLindex+timeInterval*4)+"ns  0mA)";

                                            //Write the value to the temporary netlist file
                                            stream << newLine <<'\n';
                                        }

                                    fcntr++;
                                    firstencounter=false;

                        }

                        if (netlist.Commands.at(fcntr).contains("tran",Qt::CaseInsensitive)){
                            QStringList rcommand={};
                            QStringList tempe=netlist.Commands.at(fcntr).split(" ");
                            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                                if (!tempe.at(tempcntr).isEmpty())
                                    rcommand.append(tempe.at(tempcntr));
                            }
                            //sample line: ".TRAN 0.5PS 500PS 0.1PS 0.1PS"
                            int endtime = simParams.subParam.toInt()*timeInterval*4+timeInterval;

                            QString newline = rcommand.at(0)+" "+rcommand.at(1)+" "+QString::number(endtime)
                                    +"NS "+rcommand.at(3)+" "+rcommand.at(4);
                            netlist.Commands.replace(fcntr,newline);
                        }

                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    default:
                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    }


                        //Adding Noise to the resistors
                        //This part finds the lines with "ohm" in them and add the noise line to them


                    bool addnoisecond=noise
                                & netlist.Commands.at(fcntr).contains("ohm",Qt::CaseInsensitive)
                                & !netlist.Commands.at(fcntr).contains("source",Qt::CaseInsensitive)
                                & !netlist.Commands.at(fcntr).contains("model",Qt::CaseInsensitive);

                        if (addnoisecond)
                        {
                            QStringList rcommand={};
                            QStringList tempe=netlist.Commands.at(fcntr).split(" ");
                            for (int tempcntr=0;tempcntr<tempe.length();tempcntr++){
                                if (!tempe.at(tempcntr).isEmpty())
                                    rcommand.append(tempe.at(tempcntr));
                            }
                            QRegExp r = QRegExp("^(.+)(ohm)$", Qt::CaseInsensitive);
                            r.indexIn(rcommand.at(3));
                            QString rstr = r.cap(1);
                            double rval = values->convertToValues(rstr);
                            if (rval > 0)
                            {
                                QString NoiseLine = "I"+rcommand.at(0)+"   "+rcommand.at(1)+"   "+rcommand.at(2)+"   "+"NOISE("+ QString::number(values->Rnoise(rval)) +"p 0.0 1.0p)";
                                stream<<NoiseLine<<'\n';
                            }
                        }


                }
                file.close();
                stream.flush();
    }
}
    return "Success";
}
