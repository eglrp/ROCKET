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
 * @file GlobalParameters.cpp
 * Class to do Global Parameters calculation.
 */

#include "GlobalParameters.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{

    /** Compute partial derivatives of Satellite Position in ICRS
     *  wrt Global Parameters.
     *
     * @param utc   time in utc
     * @param eb    earth body
     * @param sc    spacecraft
     *
     */
    void GlobalParameters::Process(const CommonTime&    utc,
                                   const EarthBody&     rb,
                                   const Spacecraft&    sc  )
    {


    }  // End of method 'GlobalParameters::Process(...)'



    /** Compute partial derivatives of Satellite Position in ICRS
     *  wrt Earth Rotation Parameters.
     *
     * @param utc   time in utc
     * @param eb    earth body
     * @param sc    spacecraft
     *
     */
    void GlobalParameters::PartialsOfERP(const CommonTime&  utc,
                                         const EarthBody&   eb,
                                         const Spacecraft&  sc  )
    {
        // time from reference epoch
        double dt( utc - utc0 );

        // EOP Data
        EOPDataStore2::EOPData eop( pRefSys->getEOPData(utc) );
        double xp = eop.xp * AS_TO_RAD;
        double yp = eop.yp * AS_TO_RAD;
        double dX = eop.dX * AS_TO_RAD;
        double dY = eop.dY * AS_TO_RAD;

        // TT, UT1
        CommonTime tt ( pRefSys->UTC2TT (utc) );
        CommonTime ut1( pRefSys->UTC2UT1(utc) );

        // raw CIP X,Y coordinates
        double X(0.0), Y(0.0);
        pRefSys->XY06(tt, X, Y);

        // the CIO locator s
        double s = pRefSys->S06(tt, X, Y);

        // corrected CIP X,Y coordinates
        X += dX;
        Y += dY;

        // GCRS-to-CIRS matrix
        Matrix<double> Q = pRefSys->C2IXYS(X, Y, s);

        // CIRS-to-GCRS matrix
        Matrix<double> Qt = transpose(Q);

        // earth rotation angle
        double ERA  = pRefSys->ERA00(ut1);

        // CIRS-to-TIRS matrix
        Matrix<double> R = rotation(ERA,3);

        // TIRS-to-CIRS matrix
        Matrix<double> Rt = transpose(R);

        // Earth rotation angular rate
        double dERA = pRefSys->dERA00(ut1);

        // the TIO locator s'
        double sp = pRefSys->SP00(tt);

        // TIRS-to-ITRS matrix
        Matrix<double> W = rotation(-yp,1) * rotation(-xp,2) * rotation(sp,3);

        // ITRS-to-TIRS matrix
        Matrix<double> Wt = transpose(W);


        /*
         *      X = Q * R * W * x
         *
         *  X    -   Position in ICRS
         *  x    -   Position in ITRS
         *  Q    -   CIRS-to-GCRS matrix
         *  R    -   TIRS-to-CIRS matrix
         *  W    -   ITRS-to-TIRS matrix
         *
         */


        /* partial derivatives of R(t) wrt UT1mUTC(t0) and LOD(t0)
         *
         *   dR(t)     dR(t)    dERA(t)   dUT1(t)
         * -------- = ------- * ------- * --------
         * dUT1(t0)   dERA(t)   dUT1(t)   dUT1(t0)
         *
         *   dR(t)     dR(t)    dERA(t)   dUT1(t)
         * -------- = ------- * ------- * --------
         * dLOD(t0)   dERA(t)   dUT1(t)   dLOD(t0)
         *
         *  dR(t)    | -sin(ERA) -cos(ERA)  0 |
         * ------- = | +cos(ERA) -sin(ERA)  0 |
         * dERA(t)   |     0         0      0 |
         *
         * dERA(t)
         * ------- = 2*PI * 1.00273781191135448 / 86400.0
         * dUT1(t)
         *
         * dUT1(t)       dUT1(t)
         * -------- = 1, -------- = -(t-t0) = -dt
         * dUT1(t0)      dUT1(t0)
         *
         */
        Matrix<double> dR_dUT1(3,3,0.0);
        dR_dUT1(0,0) = -std::sin(ERA)*dERA;
        dR_dUT1(0,1) = -std::cos(ERA)*dERA;
        dR_dUT1(1,0) = +std::cos(ERA)*dERA;
        dR_dUT1(1,1) = -std::sin(ERA)*dERA;

        Matrix<double> dR_dLOD(3,3,0.0);
        dR_dLOD(0,0) = -std::sin(ERA)*dERA*(-dt);
        dR_dLOD(0,1) = -std::cos(ERA)*dERA*(-dt);
        dR_dLOD(1,0) = +std::cos(ERA)*dERA*(-dt);
        dR_dLOD(1,1) = -std::sin(ERA)*dERA*(-dt);


        /* partial derivatives of W(t) wrt [xp(t0),yp(t0)]
         * and [xpdot(t0),ypdot(t0)]
         *
         *  dW(t)              dR2(xp)   dxp(t)
         * ------- = R3(-s') * ------- * ------- * R1(yp)
         *   dxp               dxp(t)    dxp(t0)
         *
         *  dW(t)                       dR1(yp)   dyp(t)
         * ------- = R3(-s') * R2(xp) * ------- * -------
         *   dyp                        dyp(t)    dyp(t0)
         *
         *  dW(t)              dR2(xp)     dxp(t)
         * ------- = R3(-s') * ------- * ---------- * R1(yp)
         * dxpdot              dxp(t)    dxpdot(t0)
         *
         *  dW(t)                       dR1(yp)     dyp(t)
         * ------- = R3(-s') * R2(xp) * ------- * ----------
         * dypdot                       dyp(t)    dypdot(t0)
         *
         *
         * dR2(xp)   | -sin(xp)  0  -cos(xp) |
         * ------- = |    0      0     0     |
         * dxp(t)    | +cos(xp)  0  -sin(xp) |
         *
         * dxp(t)         dxp(t)
         * ------- = 1, ---------- = +(t-t0) = dt
         * dxp(t0)      dxpdot(t0)
         *
         * dR1(yp)   | 0     0         0     |
         * ------- = | 0  -sin(yp)  +cos(yp) |
         * dyp(t)    | 0  -cos(yp)  -sin(yp) |
         *
         * dyp(t)         dyp(t)
         * ------- = 1, ---------- = +(t-t0) = dt
         * dyp(t0)      dypdot(t0)
         *
         */

        Matrix<double> tempx(3,3,0.0);
        tempx(0,0) = -std::sin(xp);
        tempx(0,2) = -std::cos(xp);
        tempx(2,0) = +std::cos(xp);
        tempx(2,2) = -std::sin(xp);

        Matrix<double> tempy(3,3,0.0);
        tempy(1,1) = -std::sin(yp);
        tempy(1,2) = +std::cos(yp);
        tempy(2,1) = -std::cos(yp);
        tempy(2,2) = -std::sin(yp);

        Matrix<double> dW_dxp(3,3,0.0);
        dW_dxp = rotation(-sp,3) * tempx * rotation(yp,1);

        Matrix<double> dW_dyp(3,3,0.0);
        dW_dyp = rotation(-sp,3) * rotation(xp,2) * tempy;

        Matrix<double> dW_dxpdot(3,3,0.0);
        dW_dxpdot = rotation(-sp,3) * tempx * rotation(yp,1) * dt;

        Matrix<double> dW_dypdot(3,3,0.0);
        dW_dypdot = rotation(-sp,3) * rotation(xp,2) * tempy * dt;


        // satellite position in ICRS
        Vector<double> rc( sc.getCurrentPos() );

        // transformation matrixes between ICRS and ITRS
        Matrix<double> C2T( W*R*Q );
        Matrix<double> T2C( transpose(C2T) );

        // satellite position in ITRS
        Vector<double> rt( C2T*rc );

        // partial derivative of rc(t) wrt UT1mUTC(t0) and LOD(t0)
        Vector<double> drc_dUT1 = Qt * dR_dUT1 * Wt * rt;
        Vector<double> drc_dLOD = Qt * dR_dLOD * Wt * rt;

        // partial derivative of rc(t) wrt [xp(t0),yp(t0)]
        Vector<double> drc_dxp = Qt * Rt * dW_dxp * rt;
        Vector<double> drc_dyp = Qt * Rt * dW_dyp * rt;

        // partial derivative of rc(t) wrt [xpdot(t0),ypdot(t0)]
        Vector<double> drc_dxpdot = Qt * Rt * dW_dxpdot * rt;
        Vector<double> drc_dypdot = Qt * Rt * dW_dypdot * rt;

        /*
         *           | dX/dxp dX/dyp dX/dUT1 dX/dxpdot dX/dypdot dX/dLOD |
         *           |                                                   |
         * dr_derp = | dY/dxp dY/dyp dY/dUT1 dY/dxpdot dY/dypdot dY/dLOD |
         *           |                                                   |
         *           | dZ/dxp dZ/dyp dZ/dUT1 dZ/dxpdot dZ/dypdot dZ/dLOD |
         */
        dr_derp.resize(3,6,0.0);
        for(int i=0; i<3; ++i)
        {
            dr_derp(i,0) = drc_dxp(i);
            dr_derp(i,1) = drc_dyp(i);
            dr_derp(i,2) = drc_dUT1(i);
            dr_derp(i,3) = drc_dxpdot(i);
            dr_derp(i,4) = drc_dypdot(i);
            dr_derp(i,5) = drc_dLOD(i);
        }

    }  // End of method 'GlobalParameters::PartialsOfERP(...)'


    /** Get partial derivatives of Satellite Position in ICRS
     *  wrt Helmert Parameters.
     *
     * @param utc   time in utc
     * @param eb    earth body
     * @param sc    spacecraft
     *
     */
    void GlobalParameters::PartialsOfHP (const CommonTime&  utc,
                                         const EarthBody&   eb,
                                         const Spacecraft&  sc  )
    {
        // satellite position in ICRS
        Vector<double> rc( sc.getCurrentPos() );

        // Helmert Parameters
        Vector<double> T(3,0.0);
        Vector<double> R(3,0.0);
        double D(0.0);

        /*
         *         | dx/dT1 dx/dT2 dx/dT3 |
         * dr_dT = | dy/dT1 dy/dT2 dy/dT3 |
         *         | dz/dT1 dz/dT2 dz/dT3 |
         *
         */
        Matrix<double> dr_dT(3,3,0.0);
        dr_dT(0,0) = 1.0;
        dr_dT(1,1) = 1.0;
        dr_dT(2,2) = 1.0;

        /*
         *         | dx/dD |
         * dr_dD = | dy/dD |
         *         | dz/dD |
         *
         */
        Vector<double> dr_dD(3,0.0);
        dr_dD(0) = rc(0) + R(1)*rc(2) - R(2)*rc(1);
        dr_dD(1) = rc(1) + R(2)*rc(0) - R(0)*rc(2);
        dr_dD(2) = rc(2) + R(0)*rc(1) - R(1)*rc(0);


        /*
         *         | dx/dR1 dx/dR2 dx/dR3 |
         * dr_dR = | dy/dR1 dy/dR2 dy/dR3 |
         *         | dz/dR1 dz/dR2 dz/dR3 |
         *
         */
        Matrix<double> dr_dR(3,3,0.0);
        dr_dR(0,1) = +(1+D)*rc(2);
        dr_dR(0,2) = -(1+D)*rc(1);
        dr_dR(1,0) = -(1+D)*rc(2);
        dr_dR(1,2) = +(1+D)*rc(0);
        dr_dR(2,0) = +(1+D)*rc(1);
        dr_dR(2,1) = -(1+D)*rc(0);

        /*
         *          |                   |
         * dr_dhp = | dr/dT dr/dD dr/dR |
         *          |                   |
         */
        dr_dhp.resize(3,7,0.0);
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                dr_dhp(i,j+0) = dr_dT(i,j);
            }

            dr_dhp(i,3) = dr_dD(i);

            for(int j=0; j<3; ++j)
            {
                dr_dhp(i,j+4) = dr_dR(i,j);
            }
        }

    }  // End of method 'GlobalParameters::PartialsOfHP (...)'


}  // End of namespace 'gpstk'
