#ifndef CALCVALS_H
#define CALCVALS_H

#include <QString>

class CalcVals
{
public:
    CalcVals();
    double criticalCurrent(double IcVal);
    double Rnoise(double Rval);
    double Rnorm(double Rval);
    double Vgap(double Hfield);
    double BetaParam(double Icrit, double JJcap, double Rtot);
    QString convertToUnits(double l_nvalue);
    double convertToValues(const QString& input);
};

#endif // CALCVALS_H
