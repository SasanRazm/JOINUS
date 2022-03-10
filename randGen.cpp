#include <stdio.h>
#include <math.h>

double MakeRandNumber(double meanVal, double sigmaVal)
{
    srand(QDateTime::currentDateTime().toTime_t());

    double RandomData;
    sigmaVal = sigmaVal/100;

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

int main(int argc, char const *argv[])
{
    double a = 1e-11;
    double b = 0.1;
    double c = MakeRandNumber(a,b);

    printf("%d\n", c);

    return 0;
}
