#pragma ident "$Id$"

/**
 * @file LinearCombinations.cpp
 * This class defines handy linear combinations of GDS data.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "LinearCombinations.hpp"


namespace gpstk
{

    LinearCombinations::LinearCombinations()
    {
         // Coefficients of GPS Q combinations, L1 and L2
        const double xg( ( GAMMA_GPS + 1.0 )/( GAMMA_GPS - 1.0 ) );
        const double yg( ( 2.0             )/( GAMMA_GPS - 1.0 ) );
        const double zg( ( 2.0 * GAMMA_GPS )/( GAMMA_GPS - 1.0 ) );

         // Coefficients of Galileo Q combinations, E1 and E5a
        const double xe( ( GAMMA_GAL + 1.0 )/( GAMMA_GAL - 1.0 ) );
        const double ye( ( 2.0             )/( GAMMA_GAL - 1.0 ) );
        const double ze( ( 2.0 * GAMMA_GAL )/( GAMMA_GAL - 1.0 ) );

         // Coefficients of BDS Q combinations, B1 and B2
        const double xc( ( GAMMA_BDS + 1.0 )/( GAMMA_BDS - 1.0 ) );
        const double yc( ( 2.0             )/( GAMMA_BDS - 1.0 ) );
        const double zc( ( 2.0 * GAMMA_BDS )/( GAMMA_BDS - 1.0 ) );


         // Definition to compute the code with minus ionospheric delays
         // in the GPS L1 frequency
        q1CombOfGPS.header                =   TypeID::Q1;
        q1CombOfGPS.body[TypeID::C1]      =   +xg;
        q1CombOfGPS.body[TypeID::C2]      =   -yg;

         // Definition to compute the code with minus ionospheric delays
         // in the GPS L2 frequency
        q2CombOfGPS.header                =   TypeID::Q2;
        q2CombOfGPS.body[TypeID::C1]      =   +zg;
        q2CombOfGPS.body[TypeID::C2]      =   -yg;


         // Definition to compute the code with minus ionospheric delays
         // in the Galileo L1 frequency
        q1CombOfGAL.header                =   TypeID::Q1;
        q1CombOfGAL.body[TypeID::C1]      =   +xe;
        q1CombOfGAL.body[TypeID::C5]      =   -ye;

         // Definition to compute the code with minus ionospheric delays
         // in the Galileo L5 frequency
        q5CombOfGAL.header                =   TypeID::Q5;
        q5CombOfGAL.body[TypeID::C1]      =   +ze;
        q5CombOfGAL.body[TypeID::C5]      =   -ye;


         // Definition to compute the code with minus ionospheric delays
         // in the BDS L2 frequency
        q2CombOfBDS.header                =   TypeID::Q2;
        q2CombOfBDS.body[TypeID::C2]      =   +xc;
        q2CombOfBDS.body[TypeID::C7]      =   -yc;

         // Definition to compute the code with minus ionospheric delays
         // in the BDS L7 frequency
        q7CombOfBDS.header                =   TypeID::Q7;
        q7CombOfBDS.body[TypeID::C2]      =   +zc;
        q7CombOfBDS.body[TypeID::C7]      =   -yc;




        // Coefficients of GPS PC/LC combinations, L1 and L2
        const double ag( GAMMA_GPS/( GAMMA_GPS - 1.0 ) );
        const double bg( 1.0      /( GAMMA_GPS - 1.0 ) );

        // Coefficients of Galileo PC/LC combinations, L1 and L5
        const double ae( GAMMA_GAL/( GAMMA_GAL - 1.0 ) );
        const double be( 1.0      /( GAMMA_GAL - 1.0 ) );

        // Coefficients of BDS PC/LC combinations, L2 and L7
        const double ac( GAMMA_BDS/( GAMMA_BDS - 1.0 ) );
        const double bc( 1.0      /( GAMMA_BDS - 1.0 ) );


         // Definition to compute PC combination of GPS, C1 and C2
        pcCombOfGPS.header                =   TypeID::PC;
        pcCombOfGPS.body[TypeID::C1]      =   +ag;
        pcCombOfGPS.body[TypeID::C2]      =   -bg;

         // Definition to compute LC combination of GPS, L1 and L2
        lcCombOfGPS.header                =   TypeID::LC;
        lcCombOfGPS.body[TypeID::L1]      =   +ag;
        lcCombOfGPS.body[TypeID::L2]      =   -bg;


         // Definition to compute PC combination of Galileo, C1 and C5
        pcCombOfGAL.header                =   TypeID::PC;
        pcCombOfGAL.body[TypeID::C1]      =   +ae;
        pcCombOfGAL.body[TypeID::C5]      =   -be;

         // Definition to compute LC combination of Galileo, L1 and L5
        lcCombOfGAL.header                =   TypeID::LC;
        lcCombOfGAL.body[TypeID::L1]      =   +ae;
        lcCombOfGAL.body[TypeID::L5]      =   -be;


         // Definition to compute PC combination of BDS, C2 and C7
        pcCombOfBDS.header                =   TypeID::PC;
        pcCombOfBDS.body[TypeID::C2]      =   +ac;
        pcCombOfBDS.body[TypeID::C7]      =   -bc;

         // Definition to compute LC combination of BDS, L2 and L7
        lcCombOfBDS.header                =   TypeID::LC;
        lcCombOfBDS.body[TypeID::L2]      =   +ac;
        lcCombOfBDS.body[TypeID::L7]      =   -bc;




         // Definition to compute PI combination of GPS, C1 and C2
        piCombOfGPS.header                =   TypeID::PI;
        piCombOfGPS.body[TypeID::C1]      =   -1.0;
        piCombOfGPS.body[TypeID::C2]      =   +1.0;

         // Definition to compute LI combination of GPS, L1 and L2
        liCombOfGPS.header                =   TypeID::LI;
        liCombOfGPS.body[TypeID::L1]      =   -1.0;
        liCombOfGPS.body[TypeID::L2]      =   +1.0;


         // Definition to compute PI combination of Galileo, C1 and C5
        piCombOfGAL.header                =   TypeID::PI;
        piCombOfGAL.body[TypeID::C1]      =   -1.0;
        piCombOfGAL.body[TypeID::C5]      =   +1.0;

         // Definition to compute LI combination of Galileo, L1 and L5
        liCombOfGAL.header                =   TypeID::LI;
        liCombOfGAL.body[TypeID::L1]      =   -1.0;
        liCombOfGAL.body[TypeID::L5]      =   +1.0;


         // Definition to compute PI combination of BDS, C2 and C7
        piCombOfBDS.header                =   TypeID::PI;
        piCombOfBDS.body[TypeID::C2]      =   -1.0;
        piCombOfBDS.body[TypeID::C7]      =   +1.0;

         // Definition to compute LI combination of BDS, L2 and L7
        piCombOfBDS.header                =   TypeID::PI;
        piCombOfBDS.body[TypeID::L2]      =   -1.0;
        piCombOfBDS.body[TypeID::L7]      =   +1.0;




         // Coefficients of GPS PW/LW/PN/LN combinations, L1 and L2
        double cg( L1_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );
        double dg( L2_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );
        double eg( L1_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );
        double fg( L2_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );

         // Coefficients of Galileo PW/LW/PN/LN combinations, L1 and L5
        double ce( L1_FREQ_GAL/(L1_FREQ_GAL - L5_FREQ_GAL) );
        double de( L5_FREQ_GAL/(L1_FREQ_GAL - L5_FREQ_GAL) );
        double ee( L1_FREQ_GAL/(L1_FREQ_GAL + L5_FREQ_GAL) );
        double fe( L5_FREQ_GAL/(L1_FREQ_GAL + L5_FREQ_GAL) );

         // Coefficients of BDS PW/LW/PN/LN combinations, L2 and L7
        double cc( L2_FREQ_BDS/(L2_FREQ_BDS - L7_FREQ_BDS) );
        double dc( L7_FREQ_BDS/(L2_FREQ_BDS - L7_FREQ_BDS) );
        double ec( L2_FREQ_BDS/(L2_FREQ_BDS + L7_FREQ_BDS) );
        double fc( L7_FREQ_BDS/(L2_FREQ_BDS + L7_FREQ_BDS) );


         // Definition to compute PW combination of GPS, L1 and L2
        pwCombOfGPS.header                =   TypeID::PW;
        pwCombOfGPS.body[TypeID::C1]      =   +cg;
        pwCombOfGPS.body[TypeID::C2]      =   -dg;

         // Definition to compute LW combination of GPS, L1 and L2
        lwCombOfGPS.header                =   TypeID::LW;
        lwCombOfGPS.body[TypeID::L1]      =   +cg;
        lwCombOfGPS.body[TypeID::L2]      =   -dg;


         // Definition to compute PW combination of Galileo, L1 and L5
        pwCombOfGAL.header                =   TypeID::PW;
        pwCombOfGAL.body[TypeID::C1]      =   +ce;
        pwCombOfGAL.body[TypeID::C5]      =   -de;

         // Definition to compute LW combination of Galileo, L1 and L5
        lwCombOfGAL.header                =   TypeID::LW;
        lwCombOfGAL.body[TypeID::L1]      =   +ce;
        lwCombOfGAL.body[TypeID::L5]      =   -de;


         // Definition to compute PW combination of BDS, L2 and L7
        pwCombOfBDS.header                =   TypeID::PW;
        pwCombOfBDS.body[TypeID::C2]      =   +cc;
        pwCombOfBDS.body[TypeID::C7]      =   -dc;

         // Definition to compute LW combination of BDS, L2 and L7
        lwCombOfBDS.header                =   TypeID::LW;
        lwCombOfBDS.body[TypeID::L2]      =   +cc;
        lwCombOfBDS.body[TypeID::L7]      =   -dc;




         // Definition to compute PN combination of GPS, L1 and L2
        pnCombOfGPS.header                =   TypeID::PN;
        pnCombOfGPS.body[TypeID::C1]      =   +eg;
        pnCombOfGPS.body[TypeID::C2]      =   +fg;

         // Definition to compute LN combination of GPS, L1 and L2
        lnCombOfGPS.header                =   TypeID::LN;
        lnCombOfGPS.body[TypeID::L1]      =   +eg;
        lnCombOfGPS.body[TypeID::L2]      =   +fg;


         // Definition to compute PN combination of Galileo, C1 and C5
        pnCombOfGAL.header                =   TypeID::PN;
        pnCombOfGAL.body[TypeID::C1]      =   +ee;
        pnCombOfGAL.body[TypeID::C5]      =   +fe;

         // Definition to compute LN combination of Galileo, L1 and L5
        lnCombOfGAL.header                =   TypeID::LN;
        lnCombOfGAL.body[TypeID::L1]      =   +ee;
        lnCombOfGAL.body[TypeID::L5]      =   +fe;


         // Definition to compute PN combination of BDS, C2 and C7
        pnCombOfBDS.header                =   TypeID::PN;
        pnCombOfBDS.body[TypeID::C2]      =   +ec;
        pnCombOfBDS.body[TypeID::C7]      =   +fc;

         // Definition to compute LN combination of BDS, C2 and C7
        lnCombOfBDS.header                =   TypeID::LN;
        lnCombOfBDS.body[TypeID::L2]      =   +ec;
        lnCombOfBDS.body[TypeID::L7]      =   +fc;




         // Definition to compute the MW combination of GPS, L1 and L2
        mwCombOfGPS.header                =   TypeID::MW;
        mwCombOfGPS.body[TypeID::L1]      =   +cg;
        mwCombOfGPS.body[TypeID::L2]      =   -dg;
        mwCombOfGPS.body[TypeID::C1]      =   -eg;
        mwCombOfGPS.body[TypeID::C2]      =   -fg;

         // Definition to compute the MW combination of Galileo, L1 and L5
        mwCombOfGAL.header                =   TypeID::MW;
        mwCombOfGAL.body[TypeID::L1]      =   +ce;
        mwCombOfGAL.body[TypeID::L5]      =   -de;
        mwCombOfGAL.body[TypeID::C1]      =   -ee;
        mwCombOfGAL.body[TypeID::C5]      =   -fe;

         // Definition to compute the MW combination of BDS, L2 and L5
        mwCombOfBDS.header                =   TypeID::MW;
        mwCombOfBDS.body[TypeID::L2]      =   +cc;
        mwCombOfBDS.body[TypeID::L7]      =   -dc;
        mwCombOfBDS.body[TypeID::C2]      =   -ec;
        mwCombOfBDS.body[TypeID::C7]      =   -fc;




         // Definition to compute prefit residual of C1
        c1Prefit.header                       =   TypeID::prefitC1;
        c1Prefit.body[TypeID::C1]             =   +1.0;
        c1Prefit.body[TypeID::rho]            =   -1.0;
        c1Prefit.body[TypeID::cdtSta]         =   -1.0;
        c1Prefit.body[TypeID::cdtSat]         =   +1.0;
        c1Prefit.body[TypeID::relativity]     =   -1.0;
        c1Prefit.body[TypeID::gravDelay]      =   -1.0;
        c1Prefit.body[TypeID::satPCenter]     =   -1.0;
        c1Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c1Prefit.body[TypeID::ionoL1]         =   -1.0;
        c1Prefit.body[TypeID::instC1]         =   -1.0;

         // Definition to compute prefit residual of C2
        c2Prefit.header                       =   TypeID::prefitC2;
        c2Prefit.body[TypeID::C2]             =   +1.0;
        c2Prefit.body[TypeID::rho]            =   -1.0;
        c2Prefit.body[TypeID::cdtSta]         =   -1.0;
        c2Prefit.body[TypeID::cdtSat]         =   +1.0;
        c2Prefit.body[TypeID::relativity]     =   -1.0;
        c2Prefit.body[TypeID::gravDelay]      =   -1.0;
        c2Prefit.body[TypeID::satPCenter]     =   -1.0;
        c2Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c2Prefit.body[TypeID::ionoL2]         =   -1.0;
        c2Prefit.body[TypeID::instC2]         =   -1.0;

         // Definition to compute prefit residual of C3
        c3Prefit.header                       =   TypeID::prefitC3;
        c3Prefit.body[TypeID::C1]             =   +1.0;
        c3Prefit.body[TypeID::rho]            =   -1.0;
        c3Prefit.body[TypeID::cdtSta]         =   -1.0;
        c3Prefit.body[TypeID::cdtSat]         =   +1.0;
        c3Prefit.body[TypeID::relativity]     =   -1.0;
        c3Prefit.body[TypeID::gravDelay]      =   -1.0;
        c3Prefit.body[TypeID::satPCenter]     =   -1.0;
        c3Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c3Prefit.body[TypeID::ionoL3]         =   -1.0;
        c3Prefit.body[TypeID::instC3]         =   -1.0;

         // Definition to compute prefit residual of C5
        c5Prefit.header                       =   TypeID::prefitC5;
        c5Prefit.body[TypeID::C5]             =   +1.0;
        c5Prefit.body[TypeID::rho]            =   -1.0;
        c5Prefit.body[TypeID::cdtSta]         =   -1.0;
        c5Prefit.body[TypeID::cdtSat]         =   +1.0;
        c5Prefit.body[TypeID::relativity]     =   -1.0;
        c5Prefit.body[TypeID::gravDelay]      =   -1.0;
        c5Prefit.body[TypeID::satPCenter]     =   -1.0;
        c5Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c5Prefit.body[TypeID::ionoL5]         =   -1.0;
        c5Prefit.body[TypeID::instC5]         =   -1.0;

         // Definition to compute prefit residual of C6
        c6Prefit.header                       =   TypeID::prefitC6;
        c6Prefit.body[TypeID::C6]             =   +1.0;
        c6Prefit.body[TypeID::rho]            =   -1.0;
        c6Prefit.body[TypeID::cdtSta]         =   -1.0;
        c6Prefit.body[TypeID::cdtSat]         =   +1.0;
        c6Prefit.body[TypeID::relativity]     =   -1.0;
        c6Prefit.body[TypeID::gravDelay]      =   -1.0;
        c6Prefit.body[TypeID::satPCenter]     =   -1.0;
        c6Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c6Prefit.body[TypeID::ionoL6]         =   -1.0;
        c6Prefit.body[TypeID::instC6]         =   -1.0;

         // Definition to compute prefit residual of C7
        c7Prefit.header                       =   TypeID::prefitC7;
        c7Prefit.body[TypeID::C1]             =   +1.0;
        c7Prefit.body[TypeID::rho]            =   -1.0;
        c7Prefit.body[TypeID::cdtSta]         =   -1.0;
        c7Prefit.body[TypeID::cdtSat]         =   +1.0;
        c7Prefit.body[TypeID::relativity]     =   -1.0;
        c7Prefit.body[TypeID::gravDelay]      =   -1.0;
        c7Prefit.body[TypeID::satPCenter]     =   -1.0;
        c7Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c7Prefit.body[TypeID::ionoL1]         =   -1.0;
        c7Prefit.body[TypeID::instC1]         =   -1.0;

         // Definition to compute prefit residual of C8
        c8Prefit.header                       =   TypeID::prefitC8;
        c8Prefit.body[TypeID::C8]             =   +1.0;
        c8Prefit.body[TypeID::rho]            =   -1.0;
        c8Prefit.body[TypeID::cdtSta]         =   -1.0;
        c8Prefit.body[TypeID::cdtSat]         =   +1.0;
        c8Prefit.body[TypeID::relativity]     =   -1.0;
        c8Prefit.body[TypeID::gravDelay]      =   -1.0;
        c8Prefit.body[TypeID::satPCenter]     =   -1.0;
        c8Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c8Prefit.body[TypeID::ionoL8]         =   -1.0;
        c8Prefit.body[TypeID::instC8]         =   -1.0;

         // Definition to compute prefit residual of C9
        c9Prefit.header                       =   TypeID::prefitC9;
        c9Prefit.body[TypeID::C1]             =   +1.0;
        c9Prefit.body[TypeID::rho]            =   -1.0;
        c9Prefit.body[TypeID::cdtSta]         =   -1.0;
        c9Prefit.body[TypeID::cdtSat]         =   +1.0;
        c9Prefit.body[TypeID::relativity]     =   -1.0;
        c9Prefit.body[TypeID::gravDelay]      =   -1.0;
        c9Prefit.body[TypeID::satPCenter]     =   -1.0;
        c9Prefit.body[TypeID::tropoSlant]     =   -1.0;
        c9Prefit.body[TypeID::ionoL9]         =   -1.0;
        c9Prefit.body[TypeID::instC9]         =   -1.0;




         // Definition to compute prefit residual of GPS L1
        l1PrefitOfGPS.header                      =   TypeID::prefitL1;
        l1PrefitOfGPS.body[TypeID::L1]            =   +1.0;
        l1PrefitOfGPS.body[TypeID::rho]           =   -1.0;
        l1PrefitOfGPS.body[TypeID::cdtSta]        =   -1.0;
        l1PrefitOfGPS.body[TypeID::cdtSat]        =   +1.0;
        l1PrefitOfGPS.body[TypeID::relativity]    =   -1.0;
        l1PrefitOfGPS.body[TypeID::gravDelay]     =   -1.0;
        l1PrefitOfGPS.body[TypeID::satPCenter]    =   -1.0;
        l1PrefitOfGPS.body[TypeID::tropoSlant]    =   -1.0;
        l1PrefitOfGPS.body[TypeID::ionoL1]        =   +1.0;
         // Coefficient for GPS L1 windUp is GPS L1 wavelength/2*PI
        l1PrefitOfGPS.body[TypeID::windUp]        =   -L1_WAVELENGTH_GPS/TWO_PI;
        l1PrefitOfGPS.body[TypeID::instL1]        =   -1.0;

         // Definition to compute prefit residual of GPS L2
        l2PrefitOfGPS.header                      =   TypeID::prefitL2;
        l2PrefitOfGPS.body[TypeID::L2]            =   +1.0;
        l2PrefitOfGPS.body[TypeID::rho]           =   -1.0;
        l2PrefitOfGPS.body[TypeID::cdtSta]        =   -1.0;
        l2PrefitOfGPS.body[TypeID::cdtSat]        =   +1.0;
        l2PrefitOfGPS.body[TypeID::relativity]    =   -1.0;
        l2PrefitOfGPS.body[TypeID::gravDelay]     =   -1.0;
        l2PrefitOfGPS.body[TypeID::satPCenter]    =   -1.0;
        l2PrefitOfGPS.body[TypeID::tropoSlant]    =   -1.0;
        l2PrefitOfGPS.body[TypeID::ionoL2]        =   +1.0;
         // Coefficient for GPS L2 windUp is GPS L2 wavelength/2*PI
        l2PrefitOfGPS.body[TypeID::windUp]        =   -L2_WAVELENGTH_GPS/TWO_PI;
        l2PrefitOfGPS.body[TypeID::instL2]        =   -1.0;

         // Definition to compute prefit residual of Galileo L1
        l1PrefitOfGAL.header                      =   TypeID::prefitL1;
        l1PrefitOfGAL.body[TypeID::L1]            =   +1.0;
        l1PrefitOfGAL.body[TypeID::rho]           =   -1.0;
        l1PrefitOfGAL.body[TypeID::cdtSta]        =   -1.0;
        l1PrefitOfGAL.body[TypeID::cdtSat]        =   +1.0;
        l1PrefitOfGAL.body[TypeID::relativity]    =   -1.0;
        l1PrefitOfGAL.body[TypeID::gravDelay]     =   -1.0;
        l1PrefitOfGAL.body[TypeID::satPCenter]    =   -1.0;
        l1PrefitOfGAL.body[TypeID::tropoSlant]    =   -1.0;
        l1PrefitOfGAL.body[TypeID::ionoL1]        =   +1.0;
         // Coefficient for Galileo L1 windUp is Galileo L1 wavelength/2*PI
        l1PrefitOfGAL.body[TypeID::windUp]        =   -L1_WAVELENGTH_GAL/TWO_PI;
        l1PrefitOfGAL.body[TypeID::instL1]        =   -1.0;

         // Definition to compute prefit residual of Galileo L5
        l5PrefitOfGAL.header                      =   TypeID::prefitL5;
        l5PrefitOfGAL.body[TypeID::L5]            =   +1.0;
        l5PrefitOfGAL.body[TypeID::rho]           =   -1.0;
        l5PrefitOfGAL.body[TypeID::cdtSta]        =   -1.0;
        l5PrefitOfGAL.body[TypeID::cdtSat]        =   +1.0;
        l5PrefitOfGAL.body[TypeID::relativity]    =   -1.0;
        l5PrefitOfGAL.body[TypeID::gravDelay]     =   -1.0;
        l5PrefitOfGAL.body[TypeID::satPCenter]    =   -1.0;
        l5PrefitOfGAL.body[TypeID::tropoSlant]    =   -1.0;
        l5PrefitOfGAL.body[TypeID::ionoL5]        =   +1.0;
         // Coefficient for L2 windUp is L2 wavelength/2*PI
        l5PrefitOfGAL.body[TypeID::windUp]        =   -L5_WAVELENGTH_GAL/TWO_PI;
        l5PrefitOfGAL.body[TypeID::instL5]        =   -1.0;



         // Definition to compute prefit residual of BDS L2
        l2PrefitOfBDS.header                      =   TypeID::prefitL2;
        l2PrefitOfBDS.body[TypeID::L2]            =   +1.0;
        l2PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l2PrefitOfBDS.body[TypeID::cdtSta]        =   -1.0;
        l2PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l2PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l2PrefitOfBDS.body[TypeID::satPCenter]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::ionoL2]        =   +1.0;
         // Coefficient for BDS L2 windUp is BDS L2 wavelength/2*PI
        l2PrefitOfBDS.body[TypeID::windUp]        =   -L2_WAVELENGTH_BDS/TWO_PI;
        l2PrefitOfBDS.body[TypeID::instL2]        =   -1.0;

         // Definition to compute prefit residual of BDS L7
        l7PrefitOfBDS.header                      =   TypeID::prefitL7;
        l7PrefitOfBDS.body[TypeID::L7]            =   +1.0;
        l7PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l7PrefitOfBDS.body[TypeID::cdtSta]        =   -1.0;
        l7PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l7PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l7PrefitOfBDS.body[TypeID::satPCenter]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::ionoL7]        =   +1.0;
         // Coefficient for BDS L7 windUp is BDS L7 wavelength/2*PI
        l7PrefitOfBDS.body[TypeID::windUp]        =   -L7_WAVELENGTH_BDS/TWO_PI;
        l7PrefitOfBDS.body[TypeID::instL7]        =   -1.0;




         // Definition to compute prefit residual of GPS/Galileo/BDS PC
        pcPrefit.header                           =   TypeID::prefitC;
        pcPrefit.body[TypeID::PC]                 =   +1.0;
        pcPrefit.body[TypeID::rho]                =   -1.0;
        pcPrefit.body[TypeID::cdtSta]             =   -1.0;
        pcPrefit.body[TypeID::cdtSat]             =   +1.0;
        pcPrefit.body[TypeID::relativity]         =   -1.0;
        pcPrefit.body[TypeID::gravDelay]          =   -1.0;
        pcPrefit.body[TypeID::satPCenter]         =   -1.0;
        pcPrefit.body[TypeID::tropoSlant]         =   -1.0;
        pcPrefit.body[TypeID::instPC]             =   -1.0;

         // Definition to compute prefit residual of GPS/Galileo/BDS PC, without clock
        pcPrefitWithoutClock.header                   =   TypeID::prefitCWithoutClock;
        pcPrefitWithoutClock.body[TypeID::PC]         =   +1.0;
        pcPrefitWithoutClock.body[TypeID::rho]        =   -1.0;
        pcPrefitWithoutClock.body[TypeID::relativity] =   -1.0;
        pcPrefitWithoutClock.body[TypeID::gravDelay]  =   -1.0;
        pcPrefitWithoutClock.body[TypeID::satPCenter] =   -1.0;
        pcPrefitWithoutClock.body[TypeID::tropoSlant] =   -1.0;
        pcPrefitWithoutClock.body[TypeID::instPC]     =   -1.0;

         // Definition to compute prefit residual of GPS/Galileo/BDS PC, with sat clock
        pcPrefitWithSatClock.header                   =   TypeID::prefitCWithSatClock;
        pcPrefitWithSatClock.body[TypeID::PC]         =   +1.0;
        pcPrefitWithSatClock.body[TypeID::rho]        =   -1.0;
        pcPrefitWithSatClock.body[TypeID::cdtSat]     =   +1.0;
        pcPrefitWithSatClock.body[TypeID::relativity] =   -1.0;
        pcPrefitWithSatClock.body[TypeID::gravDelay]  =   -1.0;
        pcPrefitWithSatClock.body[TypeID::satPCenter] =   -1.0;
        pcPrefitWithSatClock.body[TypeID::tropoSlant] =   -1.0;
        pcPrefitWithSatClock.body[TypeID::instPC]     =   -1.0;

         // Definition to compute prefit residual of GPS/Galileo/BDS PC, with sta clock
        pcPrefitWithStaClock.header                   =   TypeID::prefitCWithStaClock;
        pcPrefitWithStaClock.body[TypeID::PC]         =   +1.0;
        pcPrefitWithStaClock.body[TypeID::rho]        =   -1.0;
        pcPrefitWithStaClock.body[TypeID::cdtSta]     =   -1.0;
        pcPrefitWithStaClock.body[TypeID::relativity] =   -1.0;
        pcPrefitWithStaClock.body[TypeID::gravDelay]  =   -1.0;
        pcPrefitWithStaClock.body[TypeID::satPCenter] =   -1.0;
        pcPrefitWithStaClock.body[TypeID::tropoSlant] =   -1.0;
        pcPrefitWithStaClock.body[TypeID::instPC]     =   -1.0;

        // Definition to compute prefit residual of GPS/Galileo/BDS PC, for POD
        pcPrefitForPOD.header                         =   TypeID::prefitCForPOD;
        pcPrefitForPOD.body[TypeID::PC]               =   +1.0;
        pcPrefitForPOD.body[TypeID::cdtSta]           =   -1.0;
        pcPrefitForPOD.body[TypeID::relativity]       =   -1.0;
        pcPrefitForPOD.body[TypeID::gravDelay]        =   -1.0;
        pcPrefitForPOD.body[TypeID::satPCenter]       =   -1.0;
        pcPrefitForPOD.body[TypeID::tropoSlant]       =   -1.0;
        pcPrefitForPOD.body[TypeID::instPC]           =   -1.0;


         // Definition to compute prefit residual of GPS LC
        lcPrefitOfGPS.header                      =   TypeID::prefitL;
        lcPrefitOfGPS.body[TypeID::LC]            =   +1.0;
        lcPrefitOfGPS.body[TypeID::rho]           =   -1.0;
        lcPrefitOfGPS.body[TypeID::cdtSta]        =   -1.0;
        lcPrefitOfGPS.body[TypeID::cdtSat]        =   +1.0;
        lcPrefitOfGPS.body[TypeID::relativity]    =   -1.0;
        lcPrefitOfGPS.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitOfGPS.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitOfGPS.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for GPS LC windUp is GPS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitOfGPS.body[TypeID::windUp]        =   -LC_WAVELENGTH_GPS/TWO_PI;
        lcPrefitOfGPS.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of GPS LC, without clock
        lcPrefitWithoutClockOfGPS.header                    =   TypeID::prefitLWithoutClock;
        lcPrefitWithoutClockOfGPS.body[TypeID::LC]          =   +1.0;
        lcPrefitWithoutClockOfGPS.body[TypeID::rho]         =   -1.0;
        lcPrefitWithoutClockOfGPS.body[TypeID::relativity]  =   -1.0;
        lcPrefitWithoutClockOfGPS.body[TypeID::gravDelay]   =   -1.0;
        lcPrefitWithoutClockOfGPS.body[TypeID::satPCenter]  =   -1.0;
        lcPrefitWithoutClockOfGPS.body[TypeID::tropoSlant]  =   -1.0;
         // Coefficient for GPS LC windUp is GPS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithoutClockOfGPS.body[TypeID::windUp]      =   -LC_WAVELENGTH_GPS/TWO_PI;
        lcPrefitWithoutClockOfGPS.body[TypeID::instLC]      =   -1.0;

         // Definition to compute prefit residual of GPS LC, with sat clock
        lcPrefitWithSatClockOfGPS.header                    =   TypeID::prefitLWithSatClock;
        lcPrefitWithSatClockOfGPS.body[TypeID::LC]          =   +1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::rho]         =   -1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::cdtSat]      =   +1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::relativity]  =   -1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::gravDelay]   =   -1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::satPCenter]  =   -1.0;
        lcPrefitWithSatClockOfGPS.body[TypeID::tropoSlant]  =   -1.0;
         // Coefficient for GPS LC windUp is GPS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithSatClockOfGPS.body[TypeID::windUp]      =   -LC_WAVELENGTH_GPS/TWO_PI;
        lcPrefitWithSatClockOfGPS.body[TypeID::instLC]      =   -1.0;

         // Definition to compute prefit residual of GPS LC, with sta clock
        lcPrefitWithStaClockOfGPS.header                    =   TypeID::prefitLWithStaClock;
        lcPrefitWithStaClockOfGPS.body[TypeID::LC]          =   +1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::rho]         =   -1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::cdtSta]      =   -1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::relativity]  =   -1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::gravDelay]   =   -1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::satPCenter]  =   -1.0;
        lcPrefitWithStaClockOfGPS.body[TypeID::tropoSlant]  =   -1.0;
         // Coefficient for GPS LC windUp is GPS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithStaClockOfGPS.body[TypeID::windUp]      =   -LC_WAVELENGTH_GPS/TWO_PI;
        lcPrefitWithStaClockOfGPS.body[TypeID::instLC]      =   -1.0;

        // Definition to compute prefit residual of GPS LC, for POD
        lcPrefitForPODOfGPS.header                          =   TypeID::prefitLForPOD;
        lcPrefitForPODOfGPS.body[TypeID::PC]                =   +1.0;
        lcPrefitForPODOfGPS.body[TypeID::cdtSta]            =   -1.0;
        lcPrefitForPODOfGPS.body[TypeID::relativity]        =   -1.0;
        lcPrefitForPODOfGPS.body[TypeID::gravDelay]         =   -1.0;
        lcPrefitForPODOfGPS.body[TypeID::satPCenter]        =   -1.0;
        lcPrefitForPODOfGPS.body[TypeID::tropoSlant]        =   -1.0;
         // Coefficient for GPS LC windUp is GPS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitForPODOfGPS.body[TypeID::windUp]            =   -LC_WAVELENGTH_GPS/TWO_PI;
        lcPrefitForPODOfGPS.body[TypeID::instLC]            =   -1.0;


         // Definition to compute prefit residual of Galileo LC
        lcPrefitOfGAL.header                      =   TypeID::prefitL;
        lcPrefitOfGAL.body[TypeID::LC]            =   +1.0;
        lcPrefitOfGAL.body[TypeID::rho]           =   -1.0;
        lcPrefitOfGAL.body[TypeID::cdtSta]        =   -1.0;
        lcPrefitOfGAL.body[TypeID::cdtSat]        =   +1.0;
        lcPrefitOfGAL.body[TypeID::relativity]    =   -1.0;
        lcPrefitOfGAL.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitOfGAL.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitOfGAL.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for Galileo LC windUp is Galileo LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitOfGAL.body[TypeID::windUp]        =   -LC_WAVELENGTH_GAL/TWO_PI;
        lcPrefitOfGAL.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of Galileo LC, without clock
        lcPrefitWithoutClockOfGAL.header                      =   TypeID::prefitLWithoutClock;
        lcPrefitWithoutClockOfGAL.body[TypeID::LC]            =   +1.0;
        lcPrefitWithoutClockOfGAL.body[TypeID::rho]           =   -1.0;
        lcPrefitWithoutClockOfGAL.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithoutClockOfGAL.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithoutClockOfGAL.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithoutClockOfGAL.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for Galileo LC windUp is Galileo LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithoutClockOfGAL.body[TypeID::windUp]        =   -LC_WAVELENGTH_GAL/TWO_PI;
        lcPrefitWithoutClockOfGAL.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of Galileo LC, with sat clock
        lcPrefitWithSatClockOfGAL.header                      =   TypeID::prefitLWithSatClock;
        lcPrefitWithSatClockOfGAL.body[TypeID::LC]            =   +1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::rho]           =   -1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::cdtSat]        =   +1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithSatClockOfGAL.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for Galileo LC windUp is Galileo LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithSatClockOfGAL.body[TypeID::windUp]        =   -LC_WAVELENGTH_GAL/TWO_PI;
        lcPrefitWithSatClockOfGAL.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of Galileo LC, with sta clock
        lcPrefitWithStaClockOfGAL.header                      =   TypeID::prefitLWithStaClock;
        lcPrefitWithStaClockOfGAL.body[TypeID::LC]            =   +1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::rho]           =   -1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::cdtSta]        =   -1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithStaClockOfGAL.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for Galileo LC windUp is Galileo LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithStaClockOfGAL.body[TypeID::windUp]        =   -LC_WAVELENGTH_GAL/TWO_PI;
        lcPrefitWithStaClockOfGAL.body[TypeID::instLC]        =   -1.0;

        // Definition to compute prefit residual of Galileo LC, for POD
        lcPrefitForPODOfGAL.header                          =   TypeID::prefitLForPOD;
        lcPrefitForPODOfGAL.body[TypeID::PC]                =   +1.0;
        lcPrefitForPODOfGAL.body[TypeID::cdtSta]            =   -1.0;
        lcPrefitForPODOfGAL.body[TypeID::relativity]        =   -1.0;
        lcPrefitForPODOfGAL.body[TypeID::gravDelay]         =   -1.0;
        lcPrefitForPODOfGAL.body[TypeID::satPCenter]        =   -1.0;
        lcPrefitForPODOfGAL.body[TypeID::tropoSlant]        =   -1.0;
         // Coefficient for Galileo LC windUp is Galileo LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitForPODOfGAL.body[TypeID::windUp]            =   -LC_WAVELENGTH_GAL/TWO_PI;
        lcPrefitForPODOfGAL.body[TypeID::instLC]            =   -1.0;


         // Definition to compute prefit residual of BDS LC
        lcPrefitOfBDS.header                      =   TypeID::prefitL;
        lcPrefitOfBDS.body[TypeID::LC]            =   +1.0;
        lcPrefitOfBDS.body[TypeID::rho]           =   -1.0;
        lcPrefitOfBDS.body[TypeID::cdtSta]        =   -1.0;
        lcPrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        lcPrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        lcPrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitOfBDS.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for BDS LC windUp is BDS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitOfBDS.body[TypeID::windUp]        =   -LC_WAVELENGTH_BDS/TWO_PI;
        lcPrefitOfBDS.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of BDS LC, without clock
        lcPrefitWithoutClockOfBDS.header                      =   TypeID::prefitLWithoutClock;
        lcPrefitWithoutClockOfBDS.body[TypeID::LC]            =   +1.0;
        lcPrefitWithoutClockOfBDS.body[TypeID::rho]           =   -1.0;
        lcPrefitWithoutClockOfBDS.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithoutClockOfBDS.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithoutClockOfBDS.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithoutClockOfBDS.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for BDS LC windUp is BDS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithoutClockOfBDS.body[TypeID::windUp]        =   -LC_WAVELENGTH_BDS/TWO_PI;
        lcPrefitWithoutClockOfBDS.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of BDS LC, with sat clock
        lcPrefitWithSatClockOfBDS.header                      =   TypeID::prefitLWithSatClock;
        lcPrefitWithSatClockOfBDS.body[TypeID::LC]            =   +1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::rho]           =   -1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::cdtSat]        =   +1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithSatClockOfBDS.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for BDS LC windUp is BDS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithSatClockOfBDS.body[TypeID::windUp]        =   -LC_WAVELENGTH_BDS/TWO_PI;
        lcPrefitWithSatClockOfBDS.body[TypeID::instLC]        =   -1.0;

         // Definition to compute prefit residual of BDS LC, with sta clock
        lcPrefitWithStaClockOfBDS.header                      =   TypeID::prefitLWithStaClock;
        lcPrefitWithStaClockOfBDS.body[TypeID::LC]            =   +1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::rho]           =   -1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::cdtSta]        =   -1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::relativity]    =   -1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::gravDelay]     =   -1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::satPCenter]    =   -1.0;
        lcPrefitWithStaClockOfBDS.body[TypeID::tropoSlant]    =   -1.0;
         // Coefficient for BDS LC windUp is BDS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitWithStaClockOfBDS.body[TypeID::windUp]        =   -LC_WAVELENGTH_BDS/TWO_PI;
        lcPrefitWithStaClockOfBDS.body[TypeID::instLC]        =   -1.0;

        // Definition to compute prefit residual of BDS LC, for POD
        lcPrefitForPODOfBDS.header                          =   TypeID::prefitLForPOD;
        lcPrefitForPODOfBDS.body[TypeID::PC]                =   +1.0;
        lcPrefitForPODOfBDS.body[TypeID::cdtSta]            =   -1.0;
        lcPrefitForPODOfBDS.body[TypeID::relativity]        =   -1.0;
        lcPrefitForPODOfBDS.body[TypeID::gravDelay]         =   -1.0;
        lcPrefitForPODOfBDS.body[TypeID::satPCenter]        =   -1.0;
        lcPrefitForPODOfBDS.body[TypeID::tropoSlant]        =   -1.0;
         // Coefficient for BDS LC windUp is BDS LC wavelenght/2*PI, c/(f1+f2)
        lcPrefitForPODOfBDS.body[TypeID::windUp]            =   -LC_WAVELENGTH_BDS/TWO_PI;
        lcPrefitForPODOfBDS.body[TypeID::instLC]            =   -1.0;




         // Definition to compute prefit residual of GPS/Galileo/BDS PN
        pnPrefit.header                           =   TypeID::prefitPN;
        pnPrefit.body[TypeID::PN]                 =   +1.0;
        pnPrefit.body[TypeID::rho]                =   -1.0;
        pnPrefit.body[TypeID::cdtSta]             =   -1.0;
        pnPrefit.body[TypeID::cdtSat]             =   +1.0;
        pnPrefit.body[TypeID::relativity]         =   -1.0;
        pnPrefit.body[TypeID::gravDelay]          =   -1.0;
        pnPrefit.body[TypeID::satPCenter]         =   -1.0;
        pnPrefit.body[TypeID::tropoSlant]         =   -1.0;
        pnPrefit.body[TypeID::ionoPN]             =   -1.0;
        pnPrefit.body[TypeID::instPN]             =   -1.0;

         // Definition to compute prefit residual of GPS LW, L1-L2
        lwPrefitOfGPS.header                      =   TypeID::prefitLW;
        lwPrefitOfGPS.body[TypeID::LW]            =   +1.0;
        lwPrefitOfGPS.body[TypeID::rho]           =   -1.0;
        lwPrefitOfGPS.body[TypeID::cdtSta]        =   -1.0;
        lwPrefitOfGPS.body[TypeID::cdtSat]        =   +1.0;
        lwPrefitOfGPS.body[TypeID::updSatWL]      =   +1.0;
        lwPrefitOfGPS.body[TypeID::relativity]    =   -1.0;
        lwPrefitOfGPS.body[TypeID::gravDelay]     =   -1.0;
        lwPrefitOfGPS.body[TypeID::satPCenter]    =   -1.0;
        lwPrefitOfGPS.body[TypeID::tropoSlant]    =   -1.0;
        lwPrefitOfGPS.body[TypeID::ionoLW]        =   +1.0;
        lwPrefitOfGPS.body[TypeID::instLW]        =   -1.0;

         // Definition to compute prefit residual of Galileo LW, L1-L5
        lwPrefitOfGAL.header                      =   TypeID::prefitLW;
        lwPrefitOfGAL.body[TypeID::LW]            =   +1.0;
        lwPrefitOfGAL.body[TypeID::rho]           =   -1.0;
        lwPrefitOfGAL.body[TypeID::cdtSta]        =   -1.0;
        lwPrefitOfGAL.body[TypeID::cdtSat]        =   +1.0;
        lwPrefitOfGAL.body[TypeID::updSatWL]      =   +1.0;
        lwPrefitOfGAL.body[TypeID::relativity]    =   -1.0;
        lwPrefitOfGAL.body[TypeID::gravDelay]     =   -1.0;
        lwPrefitOfGAL.body[TypeID::satPCenter]    =   -1.0;
        lwPrefitOfGAL.body[TypeID::tropoSlant]    =   -1.0;
        lwPrefitOfGAL.body[TypeID::ionoLW]        =   +1.0;
        lwPrefitOfGAL.body[TypeID::instLW]        =   -1.0;

         // Definition to compute prefit residual of BDS LW, L2-L7
        lwPrefitOfBDS.header                      =   TypeID::prefitLW;
        lwPrefitOfBDS.body[TypeID::LW]            =   +1.0;
        lwPrefitOfBDS.body[TypeID::rho]           =   -1.0;
        lwPrefitOfBDS.body[TypeID::cdtSta]        =   -1.0;
        lwPrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        lwPrefitOfBDS.body[TypeID::updSatWL]      =   +1.0;
        lwPrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        lwPrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        lwPrefitOfBDS.body[TypeID::satPCenter]    =   -1.0;
        lwPrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        lwPrefitOfBDS.body[TypeID::ionoLW]        =   +1.0;
        lwPrefitOfBDS.body[TypeID::instLW]        =   -1.0;


         // Definition to compute the prefit residual of MW combination
        mwPrefit.header                           =   TypeID::prefitMW;
        mwPrefit.body[TypeID::MW]                 =   +1.0;
//      mwPrefit.body[TypeID::updSatMW]           =   +1.0;  // UPD for MW
        mwPrefit.body[TypeID::instMW]             =   +1.0;


    }  // End of constructor 'LinearCombinations::LinearCombinations()'


      // Return the frequency of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::freqOfLC(int i, int j, double f1 , double f2 )
    {
        return ( double(i)*f1+double(j)*f2 );
    }

    /// Return the wavelength of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::wavelengthOfLC(int i,int j,double f1,double f2)
    {
        return C_MPS / freqOfLC(i,j,f1,f2);
    }

    /// Return the f1 factor of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::firstFactorOfLC(int i,int j,double f1,double f2)
    {
        return double(i)*f1/freqOfLC(i,j,f1,f2);
    }

    /// Return the f2 factor of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::secondFactorOfLC(int i,int j,double f1,double f2 )
    {
        return double(j)*f2/freqOfLC(i,j,f1,f2);
    }

} // End of namespace gpstk
