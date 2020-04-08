#include "myglobalvars.h"
#include <QString>
#include <QStringList>
#include <QStandardPaths>
#include <QProcess>
#include <QDir>

// ALL THE GLOBAL DEFINITIONS



QString OutputFileName = "/OUT.DAT";
QString delimator = " ";
QStringList RawData = {"1","2"};
QStringList titleVals={};
QStringList Legends={};

QString documentFolderPath=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/JOINUS";
QString rootPath = QDir::currentPath();

QString PlotPath = documentFolderPath+"/Graphs";
QString DataPath = documentFolderPath+"/Data";
QString WSIMPath = rootPath+"/Add-ons";
QString simEngine = rootPath+"/Simulators";
QString TempFileName = DataPath+"/ModiNL.cir";
QString TempOutFile = DataPath+"/TempOut.tmp";
QString propertiesFile=documentFolderPath+"/properties.stat";

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
bool runProcess=true;

myglobalvars::myglobalvars()
{

}

