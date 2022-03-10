#include "calcvals.h"
#include "myglobalvars.h"
#include "QtMath"
#include "QRegExp"

CalcVals::CalcVals()
{

}

//Claculation of critical current based on temperatrue
double CalcVals::criticalCurrent(double IcVal)
{
    double IcritVal0 = IcVal/((1-qPow((4.2/Tc),2))*qSqrt(1-qPow((4.2/Tc),4)));
    double Bcrit = Bc2 * (1-qPow((temperature/Tc),2));
    if (globalMagField > Bcrit)
        globalMagField=Bcrit;
    double IcritValNew = IcritVal0 * (1-qPow((temperature/Tc),2)) * qSqrt(1-qPow((temperature/Tc),4)) * (1-qPow((globalMagField/Bcrit),2));
    return IcritValNew;
}

//Claculation of noise current based on temperatrue and resistance
double CalcVals::Rnoise(double Rval)
{
    double noise = round((qSqrt((4*Kbolt*temperature)/Rval)*1e15))/1000;
    return noise;
}

//Claculation of normal resistance value
double CalcVals::Rnorm(double IcVal)
{

    double vg= Vgap();
    double rnorm = tanh((vg*ecnst/4*Kbolt*temperature))*(Pi*vg)/(2*criticalCurrent(IcVal));
    return rnorm;
}

//Calculation of gap voltage depending on the temperature and external magnetic field
double CalcVals::Vgap()
{
    double Delta0=1.76*Kbolt*Tc;
    double DeltaT=Delta0*qSqrt(cos(Pi/2*qPow((temperature/Tc),2)));
    double vgap=2*DeltaT/ecnst;
    return vgap;
}

//Calculation of beta parameters with circuit parameters
double CalcVals::BetaParam(double Icrit, double JJcap, double Rtot)
{
    double betac=2*Pi*Icrit*JJcap*qPow(Rtot,2)/Phi0;
    return betac;
}

//Unit Convertion for better understanding
QString CalcVals::convertToUnits(double l_nvalue)
{
     QString unit;
     double value;

     if(l_nvalue < 0) {
         value = l_nvalue * -1;
     } else {
         value = l_nvalue;
     }

     if(value >= 1e9) {
         value = value/1000000000;
         unit = "G";
     }
     if(value >= 1e6 && value < 1e9) {
         value = value/1000000;
         unit = "Meg";
     }
     else if(value>=1e3 && value<1e6){
         value = value/1000;
         unit = "K";
     }
     else if( value>=1 && value<1000) {
         value = value*1;
     }
     else if( (value*1000)>=1 && value<1) {
         value = value*1000;
         unit = "m";
     }
     else if((value*1e6)>=1 && value*1e3<1){
         value = value*1e6;
         unit = "u";
     }
     else if((value*1e9)>=1 && value*1e6<1){
         value = value*1e9;
         unit = "n";
     }
     else if(value*1e9<1){
         value = value*1e12;
         unit = "p";
     }

     if(l_nvalue>0) {
         return (QString::number(value)+unit);
     } else
     if(l_nvalue<0) {
         return (QString::number(value*-1)+unit);
     }

     return QString::number(0);
}

//Unit Convertion for better understanding
double CalcVals::convertToValues(const QString& input) {
    QRegExp r = QRegExp("^(.+)([pnµumKG]|meg)$", Qt::CaseInsensitive);
    r.indexIn(input);

    if ( r.captureCount() == 2 ) {

        QString strValue = r.cap(1);
        QString unit = r.cap(2);

        bool ok = false;
        double value = strValue.toDouble(&ok);
        if ( !ok ) {
            return input.toDouble();
        }

        if ( unit == "p" || unit == "P") {
            return (value*1e-12);
        } else
        if ( unit == "n" || unit == "N") {
            return (value*1e-9);
        } else
        if ( unit == "u" || unit == "µ" || unit == "U" ) {
            return (value*1e-6);
        } else
        if ( unit == "m" || unit == "M") {
            return (value*1e-3);
        } else
        if ( unit == "K" || unit == "k") {
            return (value*1e3);
        } else
        if ( unit == "MEG" || unit == "meg" || unit == "Meg") {
            return (value*1e6);
        } else
        if ( unit == "G" || unit == "g") {
            return (value*1e9);
        }

        return value;
    }

    return 0;
}
