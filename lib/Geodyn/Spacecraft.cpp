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
//  Kaifa Kuang - Kaifa Kuang . 2016
//
//============================================================================

/**
* @file Spacecraft.cpp
* Class to encapsulate Spacecraft related parameters.
*/

#include "Spacecraft.hpp"
#include "GNSSconstants.hpp"
#include "Exception.hpp"
#include "Epoch.hpp"


using namespace std;


namespace gpstk
{
    /// Reset state
    void Spacecraft::resetState()
    {
        // reset state vector
        r.resize(3,0.0);
        v.resize(3,0.0);

        // reset eclipse
        eclipse = false;

        // reset partial derivatives
        dr_dr0.resize(3,3,0.0);
        dr_dv0.resize(3,3,0.0);
        dv_dr0.resize(3,3,0.0);
        dv_dv0.resize(3,3,0.0);
        dr_dp0.resize(3,0,0.0);
        dv_dp0.resize(3,0,0.0);

        dr_dr0(0,0) = 1.0;
        dr_dr0(1,1) = 1.0;
        dr_dr0(2,2) = 1.0;
        dv_dv0(0,0) = 1.0;
        dv_dv0(1,1) = 1.0;
        dv_dv0(2,2) = 1.0;

    }  // End of method 'Spacecraft::resetState()'


    /// Set state vector
    Spacecraft& Spacecraft::setCurrentState(const Vector<double>& state)
    {
        // check the size of sv
        if(state.size() == (42+6*np))
        {
            /*
             * The elements in state are arranged as followings:
             *
             *          rx,     ry,     rz
             *          vx,     vy,     vz
             *
             *          rx/rx0, rx/ry0, rx/rz0
             *          ry/rx0, ry/ry0, ry/rz0
             *          rz/rx0, rz/ry0, rz/rz0
             *
             *          rx/vx0, rx/vy0, rx/vz0
             *          ry/vx0, ry/vy0, ry/vz0
             *          rz/vx0, rz/vy0, rz/vz0
             *
             *          vx/rx0, vx/ry0, vx/rz0
             *          vy/rx0, vy/ry0, vy/rz0
             *          vz/rx0, vz/ry0, vz/rz0
             *
             *          vx/vx0, vx/vy0, vx/vz0
             *          vy/vx0, vy/vy0, vy/vz0
             *          vz/vx0, vz/vy0, vz/vz0
             *
             *          rx/p01, rx/p02, rx/p0n
             *          ry/p01, ry/p02, ry/p0n
             *          rz/p01, rz/p02, rz/p0n
             *
             *          vx/p01, vx/p02, vx/p0n
             *          vy/p01, vy/p02, vy/p0n
             *          vz/p01, vz/p02, vz/p0n
             *
             */

            // r, v
            r(0) = state(0); r(1) = state(1); r(2) = state(2);
            v(0) = state(3); v(1) = state(4); v(2) = state(5);

            // resize dr_dp0 and dv_dp0
            dr_dp0.resize(3,np,0.0);
            dv_dp0.resize(3,np,0.0);

            for(int i=0; i<3; ++i)
            {
                // dr/dr0, dr/dv0, dv/dr0, dv/dv0
                for(int j=0; j<3; ++j)
                {
                    dr_dr0(i,j) = state( 6+3*i+j);
                    dr_dv0(i,j) = state(15+3*i+j);

                    dv_dr0(i,j) = state(24+3*i+j);
                    dv_dv0(i,j) = state(33+3*i+j);
                }

                // dr/dp0, dv/dp0
                for(int j=0; j<np; ++j)
                {
                    dr_dp0(i,j) = state(42+np*(0+i)+j);
                    dv_dp0(i,j) = state(42+np*(3+i)+j);
                }
            }
        }
        else
        {
            cerr << "Call Spacecraft::setCurrentState() error."
                 << "The size of state vector does not match."
                 << endl;
        }

        return (*this);

    }  // End of method 'Spacecraft::setCurrentState()'


    /// Get current state of Spacecraft
    Vector<double> Spacecraft::getCurrentState() const
    {
        Vector<double> state(42+6*np, 0.0);

        // r, v
        state(0) = r(0); state(1) = r(1); state(2) = r(2);
        state(3) = v(0); state(4) = v(1); state(5) = v(2);

        for(int i=0; i<3; ++i)
        {
            // dr/dr0, dr/dv0, dv/dr0, dv/dv0
            for(int j=0; j<3; ++j)
            {
                state( 6+3*i+j) = dr_dr0(i,j);
                state(15+3*i+j) = dr_dv0(i,j);
                state(24+3*i+j) = dv_dr0(i,j);
                state(33+3*i+j) = dv_dv0(i,j);
            }

            // dr/dp0, dv/dp0
            for(int j=0; j<np; ++j)
            {
                state(42+np*(0+i)+j) = dr_dp0(i,j);
                state(42+np*(3+i)+j) = dv_dp0(i,j);
            }
        }

        return state;

    }  // End of method 'Spacecraft::getCurrentState()'



    /// Get transition matrix
    Matrix<double> Spacecraft::getTransitionMatrix() const
    {
        ////////////////// Transition Matrix ////////////////
        //                                                 //
        //             |                          |        //
        //             | dr_dr0   dr_dv0   dr_dp0 |        //
        //             |                          |        //
        //    tMat  =  | dv_dr0   dv_dv0   dv_dp0 |        //
        //             |                          |        //
        //             | 0        0        I      |        //
        //             |                          |        //
        //                                                 //
        /////////////////////////////////////////////////////

        Matrix<double> tMat(6+np,6+np, 0.0);

        for(int i=0; i<3; ++i)
        {
            // state transition matrix
            for(int j=0; j<3; ++j)
            {
                tMat(i+0,j+0) = dr_dr0(i,j);
                tMat(i+0,j+3) = dr_dv0(i,j);
                tMat(i+3,j+0) = dv_dr0(i,j);
                tMat(i+3,j+3) = dv_dv0(i,j);
            }

            // sensitivity matrix
            for(int j=0; j<np; ++j)
            {
                tMat(i+0,j+6+i*np) = dr_dp0(i,j);
                tMat(i+3,j+6+i*np) = dv_dp0(i,j);
            }
        }

        // identity part
        for(int i=6; i<6+np; ++i)
        {
            tMat(i,i) = 1.0;
        }

        return tMat;

    }  // End of method 'Spacecraft::getTransitionMatrix()'



    /// Get state transition matrix
    Matrix<double> Spacecraft::getStateTransitionMatrix() const
    {
        ////////////// State Transition Matrix //////////////
        //                                                 //
        //             |                  |                //
        //             | dr_dr0   dr_dv0  |                //
        //    stMat =  |                  |                //
        //             | dv_dr0   dv_dv0  |                //
        //             |                  |                //
        //                                                 //
        /////////////////////////////////////////////////////

        Matrix<double> stMat(6,6,0.0);

        // state transition matrix
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                stMat(i+0,j+0)  =  dr_dr0(i,j);
                stMat(i+0,j+3)  =  dr_dv0(i,j);
                stMat(i+3,j+0)  =  dv_dr0(i,j);
                stMat(i+3,j+3)  =  dv_dv0(i,j);
            }
        }

        return stMat;

    }  // End of method 'Spacecraft::getStateTransitionMatrix()'


    /// Get sensitivity matrix
    Matrix<double> Spacecraft::getSensitivityMatrix() const
    {
        ////////// Sensitivity Matrix //////////////
        //                                        //
        //                |        |              //
        //                | dr_dp0 |              //
        //       sMat  =  |        |              //
        //                | dv_dp0 |              //
        //                |        |              //
        //                                        //
        ////////////////////////////////////////////

        Matrix<double> sMat(6,np, 0.0);

        // sensitivity matrix
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<np; ++j)
            {
                sMat(i+0,j)  =  dr_dp0(i,j);
                sMat(i+3,j)  =  dv_dp0(i,j);
            }
        }

        return sMat;

    }  // End of method 'Spacecraft::getSensitivityMatrix()'



    // Convert position and velocity from kepler orbit elements
    Spacecraft& Spacecraft::convertFromKepler(const Vector<double>& kepler)
    {

        return (*this);

    }  // End of method 'Spacecraft::convertFromKepler()'


    // Convert position and velocity to kepler orbit elements
    Vector<double> Spacecraft::convertToKepler() const
    {
        // z-axis vector
        Vector<double> zv(3,0.0);
        zv(2) = 1.0;

        // angular momentum
        Vector<double> h = cross(r,v);

        // eccentricity vector
        Vector<double> ev = cross(v,h) - r/norm(r);

        // eccentricity
        double e = norm(ev);

        // semimajor axis
        double a = -GM_EARTH/(norm(v)-2*GM_EARTH/norm(r));

        Vector<double> vnu = cross(zv,h);
        Vector<double> eta = cross(h,vnu);

        // argument of perifocus
        double omega = std::atan2(dot(ev,eta)/norm(h), dot(ev,vnu));

        // right ascension of the ascending node if in a celestial system
        // longitude of the ascending node if in an earth-fixed coordinate system
        double OMEGA = std::atan2(vnu(1), vnu(0));

        if(omega < 0.0) omega += TWO_PI;
        if(OMEGA < 0.0) OMEGA += TWO_PI;

        // orbital inclination
        double i = std::atan2(std::sqrt(h(0)*h(0)+h(1)*h(1)), h(2));
        if(i < 0.0) i+= TWO_PI;

        // eccentric anomaly
        double E0 = std::atan2( dot(r,v)/std::sqrt(GM_EARTH*a),
                                norm(r)*norm(v)/GM_EARTH-1.0 );

        // mean anomaly
        double M0 = E0 - e*std::sin(E0);
        if(M0 < 0.0) M0 += TWO_PI;

        // kepler orbit elements
        Vector<double> kepler(6,0.0);
        kepler(0) = a;
        kepler(1) = e;
        kepler(2) = i;
        kepler(3) = OMEGA;
        kepler(4) = omega;
        kepler(5) = M0;

        // return
        return kepler;

    }    // End of method 'Spacecraft::convertToKepler()'



    /** Stream output for Spacecraft objects. Typically used for debugging.
     * @param s    stream to append formatted Spacecraft to
     * @param sc   Spacecraft to append to stream \c s
     */
    ostream& operator<<( ostream& s,
                         const gpstk::Spacecraft& sc )
    {
        s << sc.getSatID() << ' '
          << CivilTime(sc.getCurrentTime()) << ' '
          << sc.getCurrentBlock() << ' '
          << sc.getCurrentMass();

        return s;

    }  // End of 'ostream& operator<<()'


}  // End of namespace 'gpstk'
