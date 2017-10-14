//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2004, The University of Texas at Austin
//
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
 * @file RKF78Integrator.cpp
 * This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
 */

#include "RKF78Integrator.hpp"


using namespace std;


namespace gpstk
{
    // coefficients, ai
    const double RKF78Integrator::a[13] =
    {
       0.0,
       2.0 / 27.0,
       1.0 / 9.0,
       1.0 / 6.0,
       5.0 / 12.0,
       1.0 / 2.0,
       5.0 / 6.0,
       1.0 / 6.0,
       2.0 / 3.0,
       1.0 / 3.0,
       1.0,
       0.0,
       1.0,
    };

    // coefficients, bij
    const double RKF78Integrator::b[13][12] =
    {
        {        0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        2.0 / 27.0,        0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        1.0 / 36.0,        1.0 / 12.0,     0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        1.0 / 24.0,        0.0,            1.0 / 8.0,          0.0,
                 0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        5.0 / 12.0,        0.0,          -25.0 / 16.0,        25.0 / 16.0,
                 0.0,               0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        1.0 / 20.0,        0.0,            0.0,                1.0 / 4.0,
                 1.0 /  5.0,        0.0,            0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {      -25.0 / 108.0,       0.0,            0.0,              125.0 / 108.0,
               -65.0 / 27.0,      125.0 / 54.0,     0.0,                0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {       31.0 / 300.0,       0.0,            0.0,                0.0,
                61.0 / 225.0,      -2.0 / 9.0,     13.0 / 900.0,        0.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {        2.0,               0.0,            0.0,              -53.0 / 6.0,
               704.0 / 45.0,     -107.0 / 9.0,     67.0 / 90.0,         3.0,
                 0.0,               0.0,            0.0,                0.0          } ,

        {      -91.0 / 108.0,       0.0,            0.0,               23.0 / 108.0,
              -976.0 / 135.0,     311.0 / 54.0,   -19.0 / 60.0,        17.0 / 6.0,
                -1.0 / 12.0,        0.0,            0.0,                0.0          } ,

        {     2383.0 / 4100.0,      0.0,            0.0,             -341.0 / 164.0,
              4496.0 / 1025.0,   -301.0 / 82.0,  2133.0 / 4100,        45.0 / 82.0,
                45.0 /  164.0,     18.0 / 41.0,     0.0,                0.0          } ,

        {        3.0 / 205.0,       0.0,            0.0,                0.0,
                 0.0,              -6.0 / 41.0,    -3.0 / 205.0,       -3.0 / 41.0,
                 3.0 / 41.0,        6.0 / 41.0,     0.0,                0.0          } ,

        {    -1777.0 / 4100.0,             0.0,              0.0,    -341.0 / 164.0,
              4496.0 / 1025.0,   -289.0 / 82.0,  2193.0 / 4100.0,      51.0 /  82.0,
                33.0 / 164.0,      12.0 / 41.0,              0.0,               1.0  } ,
    };

    // coefficients, c1i
    const double RKF78Integrator::c1[13] =
    {
       41.0 / 840.0,
       0.0,
       0.0,
       0.0,
       0.0,
       34.0 / 105.0,
       9.0 / 35.0,
       9.0 / 35.0,
       9.0 / 280.0,
       9.0 / 280.0,
       41.0 / 840.0,
       0.0,
       0.0,
    };

    // coefficients, c2i
    const double RKF78Integrator::c2[13] =
    {
       0.0,
       0.0,
       0.0,
       0.0,
       0.0,
       34.0 / 105.0,
       9.0 / 35.0,
       9.0 / 35.0,
       9.0 / 280.0,
       9.0 / 280.0,
       0.0,
       41.0 / 840.0,
       41.0 / 840.0,
    };


    /// Real implementation of RKF78
    satVectorMap RKF78Integrator::integrateTo( const CommonTime& t_next )
    {

        satVectorMap y_next(y_curr);

        // forwared or backward
        double forward(1.0);
        if(t_curr > t_next) forward = -1.0;

        if(t_next-t_curr != stepSize)
        {
           stepSize = std::abs(t_next-t_curr);
        }

        double dt(stepSize);

        SatID sat;


        map<SatID,double> satStep;

        for(satVectorMap::iterator it = y_curr.begin();
            it != y_curr.end();
            ++it)
        {
            satStep[it->first] = dt;
        }

        satVectorMap k[13];

        CommonTime t_temp(t_curr);
        satVectorMap y_temp(y_curr);

        Vector<double> k0, k10, k11, k12;
        Vector<double> orbit;
        Vector<double> diff(3,0.0);
        double A(0.0);

        k[0] = pEOM->getDerivatives(t_temp, y_temp);

        while( true )
        {
            if(y_temp.empty()) break;

            for(int i=1; i<13; ++i)
            {
                // reset y_temp to y_curr
                for(satVectorMap::iterator it = y_temp.begin();
                    it != y_temp.end();
                    ++it)
                {
                    sat = it->first;
                    it->second = y_curr[sat];
                }

                for(int j=0; j<=i-1; ++j)
                {
                    for(satVectorMap::iterator it = y_temp.begin();
                        it != y_temp.end();
                        ++it)
                    {
                        sat = it->first;

                        dt = satStep[sat];

                        t_temp = t_curr + a[i]*dt*forward;

                        it->second += b[i][j]*(k[j])[sat]*dt*forward;
                    }
                }

                k[i] = pEOM->getDerivatives(t_temp,y_temp);
            }

            // reset y_temp to y_curr
            for(satVectorMap::iterator it = y_temp.begin();
                it != y_temp.end();
                ++it)
            {
                sat = it->first;
                it->second = y_curr[sat];
            }

            // test convergence
            for(satVectorMap::iterator it = y_temp.begin();
                it != y_temp.end();
                ++it)
            {
                sat = it->first;
                dt = satStep[sat];

                k0  = (k[0])[sat];
                k10 = (k[10])[sat];
                k11 = (k[11])[sat];
                k12 = (k[12])[sat];

                orbit = c1[0]*dt*(k0+k10-k11-k12);

                diff(0) = orbit(0);
                diff(1) = orbit(1);
                diff(2) = orbit(2);

                A = norm(diff)/errorTol;

                // if converge, update y_next and remove this satellite
                if(A < 1)
                {
                    for(int i=0; i<13; ++i)
                    {
                        it->second += c2[i]*(k[i])[sat]*dt*forward;
                    }

                    y_next[sat] = it->second;

                    y_temp.erase(sat);
                }
                // if not converge, update step size and continue
                else
                {
                    dt *= std::pow(0.0025/A,1.0/16.0);

                    satStep[sat] = dt;
                }

            } // End of 'for(satVectorMap::iterator...)'

        }  // End of 'while(true)'

        return y_next;

    }  // End of method 'RKF78Integrator::integrateTo()'

}  // End of 'namespace gpstk'
