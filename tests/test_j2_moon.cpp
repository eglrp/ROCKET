#include <iostream>

#include "Vector.hpp"
#include "constants.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
    double J2( J2_EARTH );
    double GMM( GM_MOON );
    double REE( RE_EARTH );
    double rl( 384e6 );
    double rl2( rl*rl );

    double lat(0.0), lon(0.0);
    double slat(0.0), clat(0.0);
    double slon(0.0), clon(0.0);

    Vector<double> acc(3,0.0);
    Vector<double> tmp(3,0.0);

    for(int i=0; i<=180; i+=5)
    {
        lat = i*5.0*DEG_TO_RAD;
        slat = std::sin(lat);
        clat = std::cos(lat);

        tmp(0) = (5*slat*slat-1)*clat;
        tmp(1) = (5*slat*slat-1)*clat;
        tmp(2) = (5*slat*slat-3)*slat;

        acc(2) = 1.5*J2*GMM/(rl2) * std::pow(REE/rl,2) * tmp(2);

        for(int j=0; j<=360; j+=5)
        {
            lon = j*5.0*DEG_TO_RAD;
            slon = std::sin(lon);
            clon = std::cos(lon);

            tmp(0) *= clon;
            tmp(1) *= slon;
            acc(0) = 1.5*J2*GMM/(rl2) * std::pow(REE/rl,2) * tmp(0);
            acc(1) = 1.5*J2*GMM/(rl2) * std::pow(REE/rl,2) * tmp(1);

            cout << setprecision(3);
            cout << setw(10) << acc(0)
                 << setw(10) << acc(1)
                 << setw(10) << acc(2)
                 << endl;
        }
    }

    return 0;
}
