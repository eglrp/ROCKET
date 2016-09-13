#include <iostream>

#include "Legendre.hpp"

#include "GNSSconstants.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
    double lat = -0.140368963;
    double slat = std::sin(lat);
    double clat = std::cos(lat);

    Vector<double> leg0, leg1, leg2;
    legendre(3, lat, leg0, leg1, leg2, 0);

    double id20 = indexTranslator(2,0) - 1;
    double id21 = indexTranslator(2,1) - 1;
    double id22 = indexTranslator(2,2) - 1;

    cout << "P20: " << leg0(id20) << endl
         << "P21: " << leg0(id21) << endl
         << "P22: " << leg0(id22) << endl;

    double P20 = 0.5 * (3.0*slat*slat - 1.0) * std::sqrt(5.0);
    double P21 = 3.0 * slat * clat * std::sqrt(5.0/3.0);
    double P22 = 3.0 * (1.0 - slat*slat) * std::sqrt(5.0/12.0);

    cout << "P20: " << P20 << endl
         << "P21: " << P21 << endl
         << "P22: " << P22 << endl;

    return 0;
}
