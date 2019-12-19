#ifndef MYGLOBALVARS_H
#define MYGLOBALVARS_H
#include <qstring.h>


QT_BEGIN_NAMESPACE
class QString;
class QStringList;
QT_END_NAMESPACE

extern QString OutputFileName;
extern QString TempFileName;
extern QString TempOutFile;
extern QString PlotPath;
extern QString documentFolderPath;
extern QString DataPath;
extern QString WSIMPath;
extern QString simEngine;
extern QString rootPath;
extern QString delimator;
extern QStringList RawData;
extern QStringList titleVals;
extern QStringList Legends;
extern QString propertiesFile;
extern int columNum;
extern double temperature;
extern double timestep;
extern int Simindex;
extern bool runProcess;

//Physical constants
extern double hbar;
extern double ecnst;
extern double Tc;
extern double Kbolt;
extern double Pi;
extern double Phi0;

class myglobalvars
{
public:

    myglobalvars();

};

#endif // MYGLOBALVARS_H
