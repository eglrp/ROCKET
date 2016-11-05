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
   const double RKF78Integrator::c1[11] =
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
   void RKF78Integrator::integrateTo(double&            t_curr,
                                     Vector<double>&    y_curr,
                                     EquationOfMotion*  peom,
                                     double             t_next)
   {
      // Temp variables
      Vector<double> k[13];

      double t_temp;
      Vector<double> y_temp;

      while(true)
      {
         // Step size out of range or not
         if(stepSize > std::abs(t_next-t_curr))
         {
            stepSize = t_next - t_curr;
         }

         // Integrate forward or backward
         if((t_next-t_curr) < 0.0)
         {
            stepSize *= -1.0;
         }

         // k[i], i=0,12
         t_temp = t_curr;
         y_temp = y_curr;
         k[0] = peom->getDerivatives(t_temp,y_temp);

         for(int i=1; i<13; ++i)
         {
             t_temp = t_curr + a[i]*stepSize;

             y_temp = y_curr;
             for(int j=0; j<i; ++j)
             {
                 y_temp += b[i][j]*k[j]*stepSize;
             }

             k[i] = peom->getDerivatives(t_temp,y_temp);
         }

         // Update current time and state
         t_temp = t_curr + stepSize;

         y_temp = y_curr;
         for(int i=0; i<13; ++i)
         {
             y_temp += c2[i]*k[i]*stepSize;
         }

         t_curr = t_temp;
         y_curr = y_temp;

         // Continue or break
        if(std::abs(t_curr-t_next) < 1e-12) break;

/*
         // Truncation error
         Vector<double> TE;
         TE = c1[0] * stepSize*(k[0] + k[10] - k[11] - k[12]);
         Vector<double> RV_TE(6,0.0);
         RV_TE(0) = TE(0); RV_TE(1) = TE(1); RV_TE(2) = TE(2);
         RV_TE(3) = TE(3); RV_TE(4) = TE(4); RV_TE(5) = TE(5);

         double A;
         A = norm(RV_TE)/errorTol;

         if(A < 1)   // if yes
         {
            // Update current time and state
            t_curr += stepSize;
            for(int i=0; i<13; ++i)
            {
                y_curr += c2[i]*k[i]*stepSize;
            }

            // continue or break
            if(std::abs(t_curr-t_next) < 1e-12) break;
         }
         else        // if no
         {
            // adapt step size
            stepSize *= std::pow(0.0025/A,1.0/16.0);
            continue;
         }
*/

       }    // End of 'while(true)'
   }


}  // End of 'namespace gpstk'
