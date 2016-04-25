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
 * @file IERSConv2010.hpp
 * IERS Conventions 2010
 */

#ifndef GPSTK_IERSCONV2010_HPP
#define GPSTK_IERSCONV2010_HPP

#include <iostream>
#include <cmath>
#include <string>
#include <map>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "CommonTime.hpp"
#include "EOPDataStore.hpp"
#include "LeapSecStore.hpp"
#include "SolarSystem.hpp"

namespace gpstk
{
    /** @addtogroup ephemcalc */
    //@{

    /** Relation of different Time Systems and Reference Systems
     *
     *
     *  The Chart of Different Time Systems
     *-------------------------------------------------------------------
     *
     *          -14s
     *    -----------------> BDT(Compass Time)
     *    |
     *    |         +19s             +32.184s           +relativity effects
     *   GPST ------------> TAI ----------------> TT -------------------------> TDB
     *                      T |
     *           -(UT1-TAI) | |    -leap seconds
     *   UT1 ---------------| |--------------------> UTC
     *    |
     *    |   earth rotation
     *    ---------------------> GAST
     *
     *
     *   Ritrf = POM * Theta * N * P * Ricrf
     */

    class IERSConv2010
    {
    public:
        
        // Default constructor.
        IERSConv2010()
            : pEopStore(NULL), pLeapSecStore(NULL), isPrepared(false)
        {}

        IERSConv2010(EOPDataStore& eopStore,
                     LeapSecStore& leapSecStore)
            : isPrepared(false)
        {
            pEopStore = &eopStore;
            pleapSecStore = &leapSecStore;
        }

        ~IERSConv2010()
        {}

        

    private:

        EOPDataStore* peopStore;
        LeapSecStore* pleapSecStore;

        Prepared false;

    }

    // IERS Data Handling
    //--------------------------------------------------------------------------

    /// EOP Data Store
    static EOPDataStore eopDataTable;

    /// Leap Second Store
    static LeapSecStore lsDataTable;

    /// ERP data file from IERS
    void LoadIERSERPFile(const std::string& fileName);
    /// ERP data file from IGS
    void LoadIGSERPFile(const std::string& fileName);
    /// ERP data file from STK
    void LoadSTKERPFile(const std::string& fileName);

    /// LS data file from IERS
    void LoadIERSLSFile(const std::string& fileName);

    /// Request EOP Data
    EOPDataStore::EOPData EOPData(const CommonTime& UTC);

    /// Request Leap Second
    double LeapSec(const CommonTime& UTC);

    /// in arcsecond
    double PolarMotionX(const CommonTime& UTC);

    /// in arcsecond
    double PolarMotionY(const CommonTime& UTC);

    /// in second
    double UT1mUTC(const CommonTime& UTC);

    /// in arcsecond
    double NutationDPsi(const CommonTime& UTC);

    /// in arcsecond
    double NutationDEps(const CommonTime& UTC);


    // Time Systems Handling
    // -------------------------------------------------------------------------

    double TAImUTC(const CommonTime& UTC);

    double TTmTAI(void);

    double TAImGPS(void);

//    double TTmTDB(const CommonTime& UTC);

    CommonTime GPS2UTC(const CommonTime& GPS);
    CommonTime UTC2GPS(const CommonTime& UTC);

    CommonTime UT12UTC(const CommonTime& UT1);
    CommonTime UTC2UT1(const CommonTime& UTC);

    CommonTime TAI2UTC(const CommonTime& TAI);
    CommonTime UTC2TAI(const CommonTime& UTC);

    CommonTime TT2UTC(const CommonTime& TT);
    CommonTime UTC2TT(const CommonTime& UTC);

    CommonTime BDT2UTC(const CommonTime& BDT);
    CommonTime UTC2BDT(const CommonTime& UTC);


    // Reference Systems Handling
    //--------------------------------------------------------------------------

    /// Normalize angle into the range 0 <= a <= 2PI
    double Anp(double a);

    /// Normalize angle into the range -PI <= a <= +PI
    double Anpm(double a);


    //////////// IERS 2010 EOP Correction for tide effects ////////////

    /// Evaluate the effects of zonal Earth tides on the rotation of the
    /// Earth, IERS Conventions 2010
    Vector<double> RG_ZONT2(const CommonTime& TT);

    /// Provide the diurnal/subdiurnal tidal effects on polar motion ("),
    /// UT1 (s) and LOD (s), in time domain.
    Vector<double> PMUT1_OCEANS(const CommonTime& UTC);

    /// Provide the diurnal lunisolar effect on polar motion ("), in time
    /// domain.
    Vector<double> PMSDNUT2(const CommonTime& UTC);

    /// Evaluate the model of subdiurnal libration in the axial component of
    /// rotation, expressed by UT1 and LOD.
    Vector<double> UTLIBR(const CommonTime& UTC);


    //////////// IERS Conventions 1996 ////////////

    /// Mean obliquity of the ecliptic, IAU 1980 model
    double MeanObliquity80(const CommonTime& TT);

    /// Precession matrix from J2000.0 to a specified date, IAU 1976 model
    Matrix<double> PreMatrix76(const CommonTime& TT);

    /// Nutation, IAU 1980 model
    void NutAngles80(const CommonTime& TT, double& dpsi, double& deps);

    /// Form the matrix of nutation for a given date, IAU 1980 model
    Matrix<double> NutMatrix80(const CommonTime& TT);

    /// Equation of the equinoxes, IAU 1994 model
    double EqnEquinox94(const CommonTime& TT);

    /// Universal Time to Greenwich mean sidereal time (IAU 1982 model)
    double GMST82(const CommonTime& UT1);

    /// Greenwich apparent sidereal time (consistent with IAU 1982/94
    /// resolutions)
    double GAST94(const CommonTime& UT1);

    /// Transformation matrix from true equator and equinox to Earth equator
    /// and Greenwich meridian system
    ///
    /// Greenwich Hour Angle Matrix
    Matrix<double> GHAMatrix(const CommonTime& UT1);

    /// Transformation matrix from pseudo Earth-fixed to Earth-fixed
    /// coordinates for a given date
    ///
    /// Pole Matrix 
    Matrix<double> PoleMatrix(const CommonTime& UTC);


    //////////// IERS Conventions 2010 ////////////


    // X,Y coordinates of celestial intermediate pole from series based
    // on IAU 2006 precession and IAU 2000A nutation.
    void XY06(const CommonTime& tt, double& x, double& y);

    // The CIO locator s, positioning the Celestial Intermediate Origin on
    // the equator of the Celestial Intermediate Pole, given the CIP's X,Y
    // coordinates. Compatible with IAU 2006/2000A precession-nutation.
    double S06(const CommonTime& tt, const double& x, const double& y);

    // Form the celestial to intermediate-frame-of-date matrix given the CIP
    // X,Y and the CIO locator s.
    Matrix<double> C2IXYS(const double& x, const double& y, const double& s);

    // Earth rotation angle (IAU 2000 model).
    double ERA00(const CommonTime& ut1);

    // The TIO locator s', positioning the Terrestrial Intermediate Origin
    // on the equator of the Celestial Intermediate Pole.
    double SP00(const CommonTime& tt);

    // Form the matrix of polar motion for a given date, IAU 2000.
    Matrix<double> POM00(const double& xp, const double& yp, const double& sp);


    /// Transformation matrix from CRS to TRS coordinates for a given date
    Matrix<double> C2TMatrix(const CommonTime& UTC);

      /// Transformation matrix form TRS to CRS coordinates for a given date
   Matrix<double> T2CMatrix(const CommonTime& UTC);

      /// Earth rotation angle (IAU 2000 model)
   double EarthRotationAngle(const CommonTime& UT1);

      /// Earth rotation angle first order rate
   double EarthRotationAngleRate1(const CommonTime& UT1);

      /// Time derivative of transformation matrix from CRS to TRS coordinates
      /// for a given date
   Matrix<double> dC2TMatrix(const CommonTime& UTC);

      /// Time derivative of transformation matrix from TRS to CRS coordinates
      /// for a given date
   Matrix<double> dT2CMatrix(const CommonTime& UTC);


      /// Compute doodson's fundamental arguments (BETA)
      /// and fundamental arguments for nutation (FNUT)
   void DoodsonArguments(const  CommonTime& UT1,
                         const  CommonTime& TT,
                         double BETA[6],
                         double FNUT[5]         );


   // JPL Ephemeris Data Handling
   //--------------------------------------------------------------------------

      // Objects to handle the JPL Ephemeris
   static SolarSystem jplEph;

      // JPL Ephemeris file
   void LoadJPLEphFile(std::string fileName);


      // Planet position and velocity in J2000
   Vector<double> J2kPosVel(const CommonTime&   TT,
                            SolarSystem::Planet entity,
                            SolarSystem::Planet center = SolarSystem::Earth);

      // Planet position in J2000
   Vector<double> J2kPosition(const CommonTime&   TT,
                              SolarSystem::Planet entity = SolarSystem::Earth);

      // Planet velocity in J2000
   Vector<double> J2kVelocity(const CommonTime&   TT,
                              SolarSystem::Planet entity = SolarSystem::Earth);


   // Low-Precision Solar and Lunar coordinates
   //-------------------------------------------------------------------------

      // Sun position in J2000
   Vector<double> SunJ2kPosition(const CommonTime& TT);

      // Moon position in J2000
   Vector<double> MoonJ2kPosition(const CommonTime& TT);

      //@}

} // namespace gpstk

#endif  // GPSTK_IERSCONVENTIONS_HPP
