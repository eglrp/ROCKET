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
 * @file GNSSOrbit.cpp
 * This Class handles GNSS orbit.
 */

#include "GNSSOrbit.hpp"

using namespace std;

namespace gpstk
{
   // get derivative dy/dt
   Vector<double> GNSSOrbit::getDerivatives(const double&         t,
                                            const Vector<double>& y)
   {
//       cout << setw(20) << t
//            << setw(20) << y(0)
//            << setw(20) << y(1)
//            << setw(20) << y(2)
//            << endl;

      // current epoch
      CommonTime utc(utc0+t);
      sc.setCurrentTime(utc);

      // current state
      sc.setCurrentState(y);

      // current acceleration and partial derivatives
      Vector<double> a(3,0.0);          // a
      Matrix<double> da_dr(3,3,0.0);    // da/dr
      Matrix<double> da_dv(3,3,0.0);    // da/dv
      int np = sc.getNumOfParam();      // np
      Matrix<double> da_dp(3,np,0.0);   // da/dp

      if(pEGM != NULL)
      {
          pEGM->doCompute(utc,eb,sc);
          a += pEGM->getAcceleration();
          da_dr += pEGM->dA_dR();
//          da_dv += pEGM->dA_dV();
      }

      if(pThd != NULL)
      {
          pThd->doCompute(utc,eb,sc);
          a += pThd->getAcceleration();
          da_dr += pThd->dA_dR();
//          da_dv += pThd->dA_dV();
      }

      if(pSRP != NULL)
      {
          pSRP->doCompute(utc,eb,sc);
          a += pSRP->getAcceleration();
          da_dr += pSRP->dA_dR();
//          da_dv += pSRP->dA_dV();
          da_dp += pSRP->dA_dSRP();
      }

      if(pRel != NULL)
      {
          pRel->doCompute(utc,eb,sc);
          a += pRel->getAcceleration();
          da_dr += pRel->dA_dR();
//          da_dv += pRel->dA_dV();
      }

      /* Transition Matrix, (6+np, 6+np)
       *       |                           |
       *       |   dr/dr0  dr/dv0  dr/dp0  |
       *       |                           |
       * phi = |   dv/dr0  dv/dv0  dv/dp0  |
       *       |                           |
       *       |   0       0       I       |
       *       |                           |
       */
      Matrix<double> phi = sc.getTransitionMatrix();

       /* Design Matrix, (6+np, 6+np)
        *     |                           |
        *     |   0       I       0       |
        *     |                           |
        * A = |   da/dr   da/dv   da/dp   |
        *     |                           |
        *     |   0       0       0       |
        *     |                           |
        */
       Matrix<double> A(6+np,6+np,0.0);
       A(0,3) = 1.0; A(1,4) = 1.0; A(2,5) = 1.0;

       for(int i=0; i<3; ++i)
       {
           // da/dr, da/dv
           for(int j=0; j<3; ++j)
           {
               A(i+3,j+0) = da_dr(i,j);
               A(i+3,j+3) = da_dv(i,j);
           }

           // da/dp
           for(int j=0; j<np; ++j)
           {
               A(i+3,j+6) = da_dp(i,j);
           }
       }

       /* Dot of Transition Matrix, (6+np,6+np)
        *        |                          |
        *        |  dv/dr0  dv/dv0  dv/dp0  |
        *        |                          |
        * dphi = |  da/dr0  da/dv0  da/dp0  | = A * phi
        *        |                          |
        *        |  0       0       0       |
        *        |                          |
        */
       Matrix<double> dphi = A * phi;

       // v
       Vector<double> v( sc.getCurrentVel() );

       /* Dot of current state
        *      |            |          |                            |
        *      |    v       |          |                            |
        *      |    a       |          |                            |
        *      |    dv/dr0  |          |    dv/dr0  dv/dv0  dv/dp0  |
        *      |    dv/dv0  |          |                            |
        * dy = |    dv/dp0  |   dphi = |    da/dr0  da/dv0  da/dp0  |
        *      |    da/dr0  |          |                            |
        *      |    da/dv0  |          |    0       0       0       |
        *      |    da/dp0  |          |                            |
        *      |            |          |                            |
        */
       Vector<double> dy(42+6*np,0.0);

       for(int i=0; i<3; ++i)
       {
           // v
           dy(i+0) = v(i);
           // a
           dy(i+3) = a(i);
       }

       for(int i=0; i<3; ++i)
       {
           for(int j=0; j<3; ++j)
           {
               // dv/dr0
               dy( 6+3*i+j) = dphi(0+i,0+j);
               // dv/dv0
               dy(15+3*i+j) = dphi(0+i,3+j);

               // da/dr0
               dy(24+np*3+3*i+j) = dphi(3+i,0+j);
               // da/dv0
               dy(33+np*3+3*i+j) = dphi(3+i,3+j);
           }

           for(int j=0; j<np; ++j)
           {
               // dv/dp0
               dy(24+np*i+j) = dphi(0+i,6+j);

               // da/dp0
               dy(42+3*np+np*i+j) = dphi(3+i,6+j);
           }
       }

       return dy;

   }

}  // End of namespace 'gpstk'
