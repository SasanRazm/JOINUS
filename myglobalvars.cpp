#include "myglobalvars.h"
#include <QString>
#include <QStringList>
#include <QDir>

// ALL THE GLOBAL DEFINITIONS



QString OutputFileName = "/OUT.DAT";
QString delimator = " ";
QStringList RawData = {"1","2"};
QStringList titleVals={};
QString PlotPath = QDir::currentPath()+"/Graphs";
QString DataPath = QDir::currentPath()+"/Data";
QString WSIMPath = QDir::currentPath()+"/Add-ons";
QString TempFileName = QDir::currentPath()+"/Data/ModiNL.cir";
QString TempOutFile = QDir::currentPath()+"/Data/TempOut.tmp";
QString rootPath = QDir::currentPath();
int columNum = 2;
double temperature = 4.2;
double hbar=1.05457e-34;
double ecnst=1.602e-19;
double Tc=9.2;
double Kbolt=1.3806e-23;
double Pi=3.14159265358979;
double Phi0=hbar*Pi/ecnst;
double timestep=1;
int Simindex=0;

myglobalvars::myglobalvars()
{

}

