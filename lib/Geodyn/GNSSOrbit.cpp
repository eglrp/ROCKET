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
#include "Epoch.hpp"


using namespace std;


namespace gpstk
{
    // get derivative dy/dt
    Vector<double> GNSSOrbit::getDerivatives(const double&         t,
                                             const Vector<double>& y)
    {
        // number of force model parameters
        int np = sc.getNumOfParam();

        /* Dot of current state
         *
         * dy = (v,a, dv/dr0,dv/dv0,da/dr0,da/dv0, dv/dp0,da/dp0)
         *
         */
        Vector<double> dy(42+6*np,0.0);

        if(y.size() != 42+6*np)
        {
            cerr << "Call GNSSOrbit::getDerivatives() error."
                 << "The size of state vector does not match."
                 << endl;

            return dy;
        }

        // current epoch
        CommonTime utc(utc0+t);
        sc.setCurrentTime(utc);

        // current state
        sc.setCurrentState(y);

        // current acceleration and partial derivatives
        Vector<double> a(3,0.0);          // a
        Matrix<double> da_dr(3,3,0.0);    // da/dr
        Matrix<double> da_dv(3,3,0.0);    // da/dv
        Matrix<double> da_dp(3,np,0.0);   // da/dp

        if(pEGM != NULL)
        {
            pEGM->doCompute(utc,eb,sc);
            a += pEGM->getAcceleration();
            da_dr += pEGM->dA_dR();
        }

        if(pThd != NULL)
        {
            pThd->doCompute(utc,eb,sc);
            a += pThd->getAcceleration();
            da_dr += pThd->dA_dR();
        }

        if(pSRP != NULL)
        {
            pSRP->doCompute(utc,eb,sc);
            a += pSRP->getAcceleration();
            da_dr += pSRP->dA_dR();
            da_dp += pSRP->dA_dSRP();
        }

        if(pRel != NULL)
        {
            pRel->doCompute(utc,eb,sc);
            a += pRel->getAcceleration();
            da_dr += pRel->dA_dR();
        }
/*
        cout << fixed << setprecision(8);
        cout << setw(20) << da_dr(0,0)
             << setw(20) << da_dr(0,1)
             << setw(20) << da_dr(0,2)
             << endl
             << setw(20) << da_dr(1,0)
             << setw(20) << da_dr(1,1)
             << setw(20) << da_dr(1,2)
             << endl
             << setw(20) << da_dr(2,0)
             << setw(20) << da_dr(2,1)
             << setw(20) << da_dr(2,2)
             << endl
             << endl;
*/
        /* Transition Matrix, (6+np, 6+np)
         *       |                           |
         *       |   dr/dr0  dr/dv0  dr/dp0  |
         *       |                           |
         * phi = |   dv/dr0  dv/dv0  dv/dp0  |
         *       |                           |
         *       |   0       0       I       |
         *       |                           |
         */
        Matrix<double> dr_dr0 = sc.dR_dR0();
        Matrix<double> dr_dv0 = sc.dR_dV0();
        Matrix<double> dr_dp0 = sc.dR_dP0();
        Matrix<double> dv_dr0 = sc.dV_dR0();
        Matrix<double> dv_dv0 = sc.dV_dV0();
        Matrix<double> dv_dp0 = sc.dV_dP0();


        /* Design Matrix, (6+np, 6+np)
         *     |                           |
         *     |   0       I       0       |
         *     |                           |
         * A = |   da/dr   da/dv   da/dp   |
         *     |                           |
         *     |   0       0       0       |
         *     |                           |
         */


        /* Dot of Transition Matrix, (6+np,6+np)
         *        |                          |
         *        |  dv/dr0  dv/dv0  dv/dp0  |
         *        |                          |
         * dphi = |  da/dr0  da/dv0  da/dp0  | = A * phi
         *        |                          |
         *        |  0       0       0       |
         *        |                          |
         */
        Matrix<double> da_dr0 = da_dr * dr_dr0;
        Matrix<double> da_dv0 = da_dr * dr_dv0;
        Matrix<double> da_dp0 = da_dr * dr_dp0 + da_dp;

/*
        cout << fixed << setprecision(8);
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                cout << setw(20) << dv_dv0(i,j);
            }
            cout << endl;
        }
        cout << endl;
*/

        // v
        Vector<double> v( sc.getCurrentVel() );

        // v, a
        dy(0) = v(0); dy(1) = v(1); dy(2) = v(2);
        dy(3) = a(0); dy(4) = a(1); dy(5) = a(2);

        for(int i=0; i<3; ++i)
        {
            // dv/dr0, dv/dv0, da/dr0, da/dv0
            for(int j=0; j<3; ++j)
            {
                dy( 6+3*i+j) = dv_dr0(i,j);
                dy(15+3*i+j) = dv_dv0(i,j);
                dy(24+3*i+j) = da_dr0(i,j);
                dy(33+3*i+j) = da_dv0(i,j);
            }

            // dv/dp0, da/dp0
            for(int j=0; j<np; ++j)
            {
                dy(42+np*(0+i)+j) = dv_dp0(i,j);
                dy(42+np*(3+i)+j) = da_dp0(i,j);
            }
        }

/*
        cout << fixed << setprecision(8);

        for(int i=15; i<24; ++i)
        {
            cout << setw(20) << y(i);
            if((i+1)%3 == 0) cout << endl;
        }
        cout << endl;
*/

        return dy;

    }  // End of method 'GNSSOrbit::getDerivatives()'

}  // End of namespace 'gpstk'
