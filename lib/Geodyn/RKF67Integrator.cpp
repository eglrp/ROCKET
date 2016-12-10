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
 * @file RKF67Integrator.cpp
 * This class implements Runge Kutta Fehlberg 6(7)-th order algorithm.
 */

#include "RKF67Integrator.hpp"


using namespace std;


namespace gpstk
{
    // coefficients, ai
    const double RKF67Integrator::a[10] =
    {
        0.0,            // a0
        2.0 / 33.0,     // a1
        4.0 / 33.0,     // a2
        2.0 / 11.0,     // a3
        1.0 / 2.0,      // a4
        2.0 / 3.0,      // a5
        6.0 / 7.0,      // a6
        1.0,            // a7
        0.0,            // a8
        1.0,            // a9
    };

    // coefficients, bij
    const double RKF67Integrator::b[10][9] =
    {
        {       0.0,                0.0,                0.0,
                0.0,                0.0,                0.0,
                0.0,                0.0,                0.0          } ,     // b0j

        {       2.0 / 33.0,         0.0,                0.0,
                0.0,                0.0,                0.0,
                0.0,                0.0,                0.0          } ,     // b1j

        {       0.0,                4.0 / 33.0,         0.0,
                0.0,                0.0,                0.0,
                0.0,                0.0,                0.0          } ,     // b2j

        {       1.0 / 22.0,         0.0,                3.0 / 22.0,
                0.0,                0.0,                0.0,
                0.0,                0.0,                0.0          } ,     // b3j

        {       43.0 / 64.0,        0.0,                -165.0 / 64.0,
                77.0 / 32.0,        0.0,                0.0,
                0.0,                0.0,                0.0          } ,     // b4j

        {       -2383.0 / 486.0,    0.0,                1067.0 / 54.0,
                -26312.0 / 1701.0,  2176.0 / 1701.0,    0.0,
                0.0,                0.0,                0.0          } ,     // b5j

        {       10077.0 / 4802.0,   0.0,                -5643.0 / 686.0,
                116259.0 / 16807.0, -6240.0 / 16807.0,  1053.0 / 2401.0,
                0.0,                0.0,                0.0          } ,     // b6j

        {       -733.0 / 176.0,     0.0,                141.0 / 8.0,
                335763.0 / 23296.0, 216.0 / 77.0,       -4617.0 / 2816.0,
                7203.0 / 9152.0,    0.0,                0.0          } ,     // b7j

        {       15.0 / 352.0,       0.0,                0.0,
                -5445.0 / 46592.0,  18.0 / 77.0,        -1215.0 / 5632.0,
                1029.0 / 18304.0,   0.0,                0.0          } ,     // b8j

        {       -1833.0 / 352.0,    0.0,                141.0 / 8.0,
                -51237.0 / 3584.0,  18.0 / 7.0,         -729.0 / 512.0,
                1029.0 / 1408.0,    0.0,                1.0          } ,     // b9j

    };

    // coefficients, c1i
    const double RKF67Integrator::c1[10] =
    {
        77.0 / 1440.0,          // c10
        0.0,                    // c11
        0.0,                    // c12
        1771561.0 / 6289920.0,  // c13
        32.0 / 105.0,           // c14
        243.0 / 2560.0,         // c15
        16807.0 / 74880.0,      // c16
        11.0 / 270.0,           // c17
        0.0,                    // c18
        0.0,                    // c19
    };

    // coefficients, c2i
    const double RKF67Integrator::c2[10] =
    {
        11 / 864.0,             // c20
        0.0,                    // c21
        0.0,                    // c22
        1771561.0 / 6289920.0,  // c23
        32.0 / 105.0,           // c24
        243.0 / 2560.0,         // c25
        16807.0 / 74880.0,      // c26
        0.0,                    // c27
        11.0 / 270.0,           // c28
        11.0 / 270.0,           // c29
    };



    /// Real implementation of RKF67
    void RKF67Integrator::integrateTo(double&            t_curr,
                                      Vector<double>&    y_curr,
                                      EquationOfMotion*  peom,
                                      double             t_next)
    {
        // forwared or backward
        double forward(0.0);

        if(t_curr<t_next)
            forward = +1.0;
        else
            forward = -1.0;

        // temp variables
        Vector<double> k[10] = {0.0};
        double t_temp(t_curr);
        Vector<double> y_temp(y_curr);

        while(true)
        {
            // step size out of range or not
            if(stepSize != std::abs(t_next-t_curr))
            {
                stepSize = std::abs(t_next-t_curr);
            }

            // k[i], i=0,10
            t_temp = t_curr;
            y_temp = y_curr;

            k[0] = peom->getDerivatives(t_temp,y_temp);

            for(int i=1; i<10; ++i)
            {
                t_temp = t_curr + a[i]*stepSize*forward;

                y_temp = y_curr;
                for(int j=0; j<=i-1; ++j)
                {
                    y_temp += b[i][j]*k[j]*stepSize*forward;
                }

                k[i] = peom->getDerivatives(t_temp,y_temp);
            }

            // update current time and state
            t_temp = t_curr + stepSize*forward;

            y_temp = y_curr;
            for(int i=0; i<10; ++i)
            {
                y_temp += c2[i]*k[i]*stepSize*forward;
            }

            t_curr = t_temp;
            y_curr = y_temp;

            if(std::abs(t_curr-t_next) < 1e-12) break;

        }  // End of 'while(true)'

    }    // End of method 'RKF67Integrator::integrateTo()'

}  // End of 'namespace gpstk'
