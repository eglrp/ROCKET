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
#include "Counter.hpp"


using namespace std;


namespace gpstk
{
    // get derivative dy/dt
    satVectorMap GNSSOrbit::getDerivatives( const CommonTime&   tt,
                                            const satVectorMap& states )
    {
        /* Dot of current state
         *
         * dy = (v,a, dv/dr0,dv/dv0,da/dr0,da/dv0, dv/dp0,da/dp0)
         *
         */

        satVectorMap dStates;

        if(pEGM != NULL) pEGM->Compute(tt, states);
        if(pThd != NULL) pThd->Compute(tt, states);
        if(pSRP != NULL) pSRP->Compute(tt, states);
        if(pRel != NULL) pRel->Compute(tt, states);

        int size( (states.begin()->second).size() );
        int numSRP( (size-42)/6 );

        SatID sat;
        Vector<double> state(size,0.0);
        Vector<double> dState(size,0.0);

        for( satVectorMap::const_iterator it = states.begin();
             it != states.end();
             ++it )
        {
            sat = it->first;
            state = it->second;

            // current acceleration and partial derivatives
            Vector<double> a(3,0.0);
            Matrix<double> da_dr(3,3,0.0);
            Matrix<double> da_dv(3,3,0.0);
            Matrix<double> da_dp(3,numSRP,0.0);

            if(pEGM != NULL)
            {
                a += pEGM->getAcceleration(sat);
                da_dr += pEGM->dA_dR(sat);
            }

            if(pThd != NULL)
            {
                a += pThd->getAcceleration(sat);
                da_dr += pThd->dA_dR(sat);
            }

            if(pSRP != NULL)
            {
                a += pSRP->getAcceleration(sat);
                da_dp += pSRP->dA_dSRP(sat);
            }

            if(pRel != NULL)
            {
                a += pRel->getAcceleration(sat);
                da_dr += pRel->dA_dR(sat);
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

            Matrix<double> dr_dr0(3,3,0.0), dr_dv0(3,3,0.0);
            Matrix<double> dv_dr0(3,3,0.0), dv_dv0(3,3,0.0);
            Matrix<double> dr_dp0(3,numSRP,0.0), dv_dp0(3,numSRP,0.0);

            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    dr_dr0(i,j) = state( 6+3*i+j);
                    dr_dv0(i,j) = state(15+3*i+j);

                    dv_dr0(i,j) = state(24+3*i+j);
                    dv_dv0(i,j) = state(33+3*i+j);
                }

                for(int j=0; j<numSRP; ++j)
                {
                    dr_dp0(i,j) = state(42+numSRP*(i+0)+j);
                    dv_dp0(i,j) = state(42+numSRP*(i+3)+j);
                }
            }


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

            // v
            Vector<double> v(3,0.0);
            v(0) = state(3); v(1) = state(4); v(2) = state(5);

            // v, a
            dState(0) = v(0); dState(1) = v(1); dState(2) = v(2);
            dState(3) = a(0); dState(4) = a(1); dState(5) = a(2);

            for(int i=0; i<3; ++i)
            {
                // dv/dr0, dv/dv0, da/dr0, da/dv0
                for(int j=0; j<3; ++j)
                {
                    dState( 6+3*i+j) = dv_dr0(i,j);
                    dState(15+3*i+j) = dv_dv0(i,j);
                    dState(24+3*i+j) = da_dr0(i,j);
                    dState(33+3*i+j) = da_dv0(i,j);
                }

                // dv/dp0, da/dp0
                for(int j=0; j<numSRP; ++j)
                {
                    dState(42+numSRP*(0+i)+j) = dv_dp0(i,j);
                    dState(42+numSRP*(3+i)+j) = da_dp0(i,j);
                }
            }

            dStates[sat] = dState;

        } // End of 'for(satVectorMap::const_iterator...)'

        return dStates;

    }  // End of method 'GNSSOrbit::getDerivatives()'


}  // End of namespace 'gpstk'
