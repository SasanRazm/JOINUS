#ifndef SIMULATEALL_H
#define SIMULATEALL_H
#include <QString>


class simulateall
{
public:

    int WaitTime;
    int Tstep;

    simulateall();
    QString make_new_netlist(bool noise,QString NetlistFile,struct SimParams simParams,int SimulatorIndex,int IVstatistical);
    struct ConsoleOutputs simulatenetlist(QString FileName, int SimulatorIndex);
    struct ConsoleOutputs simulateivcurve(QString FileName, int SimulatorIndex, int points, int loopCnt,bool IVstatistical);
    struct ConsoleOutputs simulateivtest(QString FileName, int SimulatorIndex,int points, int loopCnt);
    struct ConsoleOutputs simulateivnew(QString FileName, int SimulatorIndex,int points);
    struct ConsoleOutputs simulateBER(int SimulatorIndex,float Multiplyer, double SubParamVal);
    struct ConsoleOutputs simulateFreq(int SimulatorIndex,float Multiplyer, double SubParamVal);
    QVector<double> movingAverage(QVector<double> datain,int windowSize);
    struct processedNL processNetlist(QString FileName);
};

#endif // SIMULATEALL_H
