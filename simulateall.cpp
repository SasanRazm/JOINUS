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
struct ConsoleOutputs simulateall::simulateivcurve(QString FileName, int SimulatorIndex, int points, int loopCnt)
{
    QVector<double> I(points*loopCnt);
    QVector<double> V(points*loopCnt);

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





    //*** Delete this!***
    //For test to see if we reach here

    QFile filet(QDir::currentPath()+"/Data/test.dat");
    if (!filet.open(QFile::WriteOnly | QFile::Text)){
        return {filet.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream stream(&filet);

//        for (int IVcntr=0;IVcntr<RawData.length();IVcntr++)
//            stream << RawData.at(IVcntr) << '\n';

        for (int IVcntr=0;IVcntr<points*loopCnt;IVcntr++)
        {

            int firstindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1)-WaitTime)/timestep));
            int secondindex=static_cast<int>(round(((Tstep+WaitTime)*(IVcntr+1))/timestep));
            I[IVcntr]=DataI[firstindex];
            double Vtemp=0;
            for (int Vsum=firstindex;Vsum<secondindex;Vsum++)
                Vtemp = Vtemp+DataV[Vsum]/(secondindex-firstindex);
            V[IVcntr]=Vtemp;
            stream << QString::number(firstindex)+" : "+QString::number(secondindex)+" -> "+QString::number(V[IVcntr])+"   "+QString::number(I[IVcntr])<<'\n';
        }
   }

   //End of the test sequence
    //*** Delete this!***


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
    //Open the outputfile to put the I-V values into it
    QFile file2(OutFile);
    if (!file2.open(QFile::WriteOnly | QFile::Text)){
        return {file2.errorString()," There is a problem in file creation. Check the permissions! "};
    }else
    {
        QTextStream stream(&file2);
        for (int fcntr=0; fcntr<V.length();fcntr++)
            stream << QString::number(V[fcntr])+"   "+QString::number(I[fcntr])<<'\n';
        file2.close();
        stream.flush();

    }

    I.clear();
    V.clear();

    return out;
}


//Run the simulator for the input netlist and generate the output
struct ConsoleOutputs simulateall::simulatenetlist(QString FileName, int SimulatorIndex)
{
    QString OutFile= QDir::currentPath()+OutputFileName;
    //QString OutFile= "."+OutputFileName;

    QString simstderr="";
    QString simstdout="";

    QProcess process;
    //QString OSname=QSysInfo::productType();

    switch(SimulatorIndex){
    case 0:
        #ifdef __linux__
                    process.start("./jsim_n "+FileName);
                    process.waitForFinished(-1); // will wait forever until finished
        #elif _WIN32
                    process.setProgram("cmd.exe");
                    process.setArguments({"/c",DataPath+"/jsim_n.exe", FileName});
                    process.setCreateProcessArgumentsModifier([] (
                        QProcess::CreateProcessArguments *args) {
                                    args->flags &= CREATE_NO_WINDOW;});
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


//Create new netlist based on the selections of the user. Considering the Temerature and Noise
//It will create a series of PWL currents to step the I, for I-V simulation mode
//Must get the parameter and the value for shifting.

QString simulateall::make_new_netlist(bool noise,QString NetlistFile, struct SimParams simParams,int SimulatorIndex)
{
    CalcVals *values= new CalcVals;
    double stepVal=0;

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

                    case 0:
                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    case 1:
                        // Find the line with parameter and make the changes
                        if (netlist.Commands.at(fcntr).contains(simParams.mainParam,Qt::CaseInsensitive))
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

                            // One increament to counter to skip the Main Parameters orginal value.
                            fcntr++;
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
                            QString newline = rcommand.at(0)+" "+rcommand.at(1)+" "+QString::number(endtime)
                                    +"PS "+rcommand.at(3)+" "+rcommand.at(4);
                            netlist.Commands.replace(fcntr,newline);

                        }

                        //Write the rest of the file to the netlist
                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

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

                    case 3:
                        if (tempIndicator){
                            titleVals.append(QString::number(temperature)+" K");
                            titleVals.append("Temperature = ");
                            tempIndicator=false;
                        }

                        stream << netlist.Commands.at(fcntr)<<'\n';
                        break;

                    case 4:
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
