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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file RungeKuttaFehlberg7.cpp
 * This class does numerical integrations with Runge Kutta Fehlberg 7th-8th
 * Order Integration Algorithm.
 */

#include "RungeKuttaFehlberg7.hpp"

using namespace std;

namespace gpstk
{
   // coefficients, ai
   const double RungeKuttaFehlberg7::a[13] =
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
   const double RungeKuttaFehlberg7::b[13][12] =
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
   const double RungeKuttaFehlberg7::c1[13] =
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
   const double RungeKuttaFehlberg7::c2[13] =
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


      /* The Runge Kutta Fehlberg 7th-8th Order Integration Algorithm.
       */
   void RungeKuttaFehlberg7::integrateTo(double nextTime,
                                         Vector<double>& error,
                                         EquationOfMotion* peom,
                                         double stepSize)
   {
       if(stepSize == 0.0)
           stepSize = nextTime - currentTime;

       bool done = false;

       while(!done)
       {
           // k0
           k0 = peom->getDerivatives(currentTime, currentState);

           // k1
           tempy = currentState + stepSize * b[1][0]*k0;
           k1 = peom->getDerivatives(currentTime+a[1]*stepSize, tempy);

           // k2
           tempy = currentState + stepSize * (b[2][0]*k0 + b[2][1]*k1);
           k2 = peom->getDerivatives(currentTime+a[2]*stepSize, tempy);

           // k3
           tempy = currentState + stepSize * (b[3][0]*k0 + b[3][1]*k1 + b[3][2]*k2);
           k3 = peom->getDerivatives(currentTime+a[3]*stepSize, tempy);

           // k4
           tempy = currentState + stepSize * (b[4][0]*k0 + b[4][1]*k1 + b[4][2]*k2 + b[4][3]*k3);
           k4 = peom->getDerivatives(currentTime+a[4]*stepSize, tempy);

           // k5
           tempy = currentState + stepSize * (b[5][0]*k0 + b[5][1]*k1 + b[5][2]*k2 + b[5][3]*k3 + b[5][4]*k4);
           k5 = peom->getDerivatives(currentTime+a[5]*stepSize, tempy);

           // k6
           tempy = currentState + stepSize * (b[6][0]*k0 + b[6][1]*k1 + b[6][2]*k2 + b[6][3]*k3 + b[6][4]*k4
                                            + b[6][5]*k5);
           k6 = peom->getDerivatives(currentTime+a[6]*stepSize, tempy);

           // k7
           tempy = currentState + stepSize * (b[7][0]*k0 + b[7][1]*k1 + b[7][2]*k2 + b[7][3]*k3 + b[7][4]*k4
                                            + b[7][5]*k5 + b[7][6]*k6);
           k7 = peom->getDerivatives(currentTime+a[7]*stepSize, tempy);

           // k8
           tempy = currentState + stepSize * (b[8][0]*k0 + b[8][1]*k1 + b[8][2]*k2 + b[8][3]*k3 + b[8][4]*k4
                                            + b[8][5]*k5 + b[8][6]*k6 + b[8][7]*k7);
           k8 = peom->getDerivatives(currentTime+a[8]*stepSize, tempy);

           // k9
           tempy = currentState + stepSize * (b[9][0]*k0 + b[9][1]*k1 + b[9][2]*k2 + b[9][3]*k3 + b[9][4]*k4
                                            + b[9][5]*k5 + b[9][6]*k6 + b[9][7]*k7 + b[9][8]*k8);
           k9 = peom->getDerivatives(currentTime+a[9]*stepSize, tempy);

           // k10
           tempy = currentState + stepSize * (b[10][0]*k0 + b[10][1]*k1 + b[10][2]*k2 + b[10][3]*k3 + b[10][4]*k4
                                            + b[10][5]*k5 + b[10][6]*k6 + b[10][7]*k7 + b[10][8]*k8 + b[10][9]*k9);
           k10 = peom->getDerivatives(currentTime+a[10]*stepSize, tempy);

           // k11
           tempy = currentState + stepSize * (b[11][0]*k0 + b[11][1]*k1 + b[11][2]*k2 + b[11][3]*k3 + b[11][4]*k4
                                            + b[11][5]*k5 + b[11][6]*k6 + b[11][7]*k7 + b[11][8]*k8 + b[11][9]*k9
                                            + b[11][10]*k10);
           k11 = peom->getDerivatives(currentTime+a[11]*stepSize, tempy);

           // k12
           tempy = currentState + stepSize * (b[12][0]*k0 + b[12][1]*k1 + b[12][2]*k2 + b[12][3]*k3 + b[12][4]*k4
                                            + b[12][5]*k5 + b[12][6]*k6 + b[12][7]*k7 + b[12][8]*k8 + b[12][9]*k9
                                            + b[12][10]*k10 + b[12][11]*k11);
           k12 = peom->getDerivatives(currentTime+a[12]*stepSize, tempy);


           currentState += stepSize * (c2[0]*k0 + c2[1]*k1 + c2[2]*k2 + c2[3]*k3 + c2[4]*k4 + c2[5]*k5 + c2[6]*k6
                                     + c2[7]*k7 + c2[8]*k8 + c2[9]*k9 + c2[10]*k10 + c2[11]*k11 + c2[12]*k12);

           error = c1[0] * stepSize * (k0 + k10 - k11 - k12);

           // If we are within teps of the goal time, we are done.
           if(fabs(currentTime + stepSize - nextTime) < teps)
               done = true;

           // If we are about to overstep, change the stepsize appropriately to
           // hit our target final time.
           if(stepSize > 0.0)
           {
               if( (currentTime + stepSize) > nextTime )
                   stepSize = (nextTime - currentTime);
           }
           else
           {
               if( (currentTime + stepSize) < nextTime)
                   stepSize = (nextTime - currentTime);
           }

           currentTime += stepSize;

       }

       currentTime = nextTime;

   }  // End of 'RungeKuttaFehlberg7::integrateTo(nextTime, error, stepSize)'


}  // End of 'namespace gpstk'
