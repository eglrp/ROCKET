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
      // current epoch
      CommonTime utc(utc0 + t);

      // current state
      sc.setStateVector(y);

      // current acceleration and partial derivatives
      pEGM->doCompute(utc,eb,sc);
      pSun->doCompute(utc,eb,sc);
      pMoon->doCompute(utc,eb,sc);
      pSRP->doCompute(utc,eb,sc);
      pRel->doCompute(utc,eb,sc);

      // a
      Vector<double> a(3,0.0);
      a = pEGM->getAcceleration()
        + pSun->getAcceleration()
        + pMoon->getAcceleration()
        + pSRP->getAcceleration()
        + pRel->getAcceleration();

      // da/dr
      Matrix<double> da_dr(3,3,0.0);
      da_dr = pEGM->dA_dR()
            + pSun->dA_dR()
            + pMoon->dA_dR()
            + pSRP->dA_dR()
            + pRel->dA_dR();

      // da/dv
      Matrix<double> da_dv(3,3,0.0);

      // da/dp
      int np = sc.getNumOfParam();

      Matrix<double> da_dp(3,np,0.0);

      da_dp = pSRP->dA_dSRP();


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

       for(int i=0; i<3; i++)
       {
           // da/dr, da/dv
           for(int j=0; j<3; j++)
           {
               A(i+3,j+0) = da_dr(i,j);
               A(i+3,j+3) = da_dv(i,j);
           }

           // da/dp
           for(int j=0; j<np; j++)
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
       Vector<double> v( sc.getVelocity() );

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

       // v
       dy(0) = v(0); dy(1) = v(1); dy(2) = v(2);

       // a
       dy(3) = a(0); dy(4) = a(1); dy(5) = a(2);

       // dv/dr0
       dy( 6) = dphi(0,0); dy( 7) = dphi(0,1); dy( 8) = dphi(0,2);
       dy( 9) = dphi(1,0); dy(10) = dphi(1,1); dy(11) = dphi(1,2);
       dy(12) = dphi(2,0); dy(13) = dphi(2,1); dy(14) = dphi(2,2);

       // dv/dv0
       dy(15) = dphi(0,3); dy(16) = dphi(0,4); dy(17) = dphi(0,5);
       dy(18) = dphi(1,3); dy(19) = dphi(1,4); dy(20) = dphi(1,5);
       dy(21) = dphi(2,3); dy(22) = dphi(2,4); dy(23) = dphi(2,5);

       // dv/dp0
       for(int i=0; i<np; i++)
       {
           dy(24+0*np+i) = dphi(0,6+i);
           dy(24+1*np+i) = dphi(1,6+i);
           dy(24+2*np+i) = dphi(2,6+i);
       }

       // da/dr0
       dy(24+3*np) = dphi(3,0); dy(25+3*np) = dphi(3,1); dy(26+3*np) = dphi(3,2);
       dy(27+3*np) = dphi(4,0); dy(28+3*np) = dphi(4,1); dy(29+3*np) = dphi(4,2);
       dy(30+3*np) = dphi(5,0); dy(31+3*np) = dphi(5,1); dy(32+3*np) = dphi(5,2);

       // da/dv0
       dy(33+3*np) = dphi(3,3); dy(34+3*np) = dphi(3,4); dy(35+3*np) = dphi(3,5);
       dy(36+3*np) = dphi(4,3); dy(37+3*np) = dphi(4,4); dy(38+3*np) = dphi(4,5);
       dy(39+3*np) = dphi(5,3); dy(40+3*np) = dphi(5,4); dy(41+3*np) = dphi(5,5);

       // da/dp0
       for(int i=0; i<np; i++)
       {
           dy(42+3*np+i) = dphi(3,6+i);
           dy(42+4*np+i) = dphi(4,6+i);
           dy(42+5*np+i) = dphi(5,6+i);
       }

       return dy;

   }

}  // End of namespace 'gpstk'
