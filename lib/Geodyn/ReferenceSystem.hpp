/**
 * @file ReferenceSystem.hpp
 * This class ease handling reference system transformation.
 */

#ifndef GPSTK_REFERENCE_SYSTEM_HPP
#define GPSTK_REFERENCE_SYSTEM_HPP

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Last Modified:
//
//  2013/05/29   Create this file
//
//  Shoujian Zhang, Wuhan University
//
//============================================================================

#include <string>
#include "GNSSconstants.hpp"
#include "Exception.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "CommonTime.hpp"
#include "EOPDataStore.hpp"
#include "LeapSecStore.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** Class to handle the reference system transformation, i.e. 
       *  Transforming the ICRS to ITRS and vice versa.
       *
       *  All the transformation matrix are computed using the 
       *  analytic fomulas.
       *
       *  The Earth Orentation Parameters are from the EOPDataStore, 
       *  and the leap seconds data are from LeapSecStore. 
       *
       *  When convert the ICRS to ITRS, you will use the time system 
       *  conversion, and the relation between different time system
       *  follows as:
       * 
       *           -14s
       *     -----------------> BDT(Compass Time)
       *     |
       *     |         +19s             +32.184s           +rel.effects
       *    GPST ------------> TAI ----------------> TT -----------------> TDB
       *                       T |
       *            -(UT1-TAI) | |    -leap seconds
       *    UT1 ---------------| |--------------------> UTC
       *     |
       *     |   earth rotation
       *     ---------------------> GAST
       *
       *  Most of the methods are modified from the code provided by
       *  Montenbruck, Oliver, 2003, as the attachement of the book
       *  "Satellite Orbits: Models, Methods and applications".
       *
       */
   class ReferenceSystem
   {
   public:


         /// Modified Julian Date of J2000.0
      static const double MJD_J2000;


         /// Default constructor.
      ReferenceSystem()
        : pEopStore(NULL), pLeapSecStore(NULL), isPrepared(false)
      { }


         /** Constructor.
          */
      ReferenceSystem(EOPDataStore& eopStore,
                      LeapSecStore& leapSecStore)
        : isPrepared(false)
      {
         pEopStore = &eopStore;
         pLeapSecStore = &leapSecStore;
      }


         /// Set the EOP data store.
      ReferenceSystem& setEOPDataStore(EOPDataStore& eopStore)
      { pEopStore = &eopStore; return (*this); };


         /// Get the EOP data store.
      EOPDataStore* getEOPDataStore() const
      { return pEopStore; };


         /// Set the leapsec data store.
      ReferenceSystem& setLeapSecStore(LeapSecStore& leapSecStore)
      { pLeapSecStore = &leapSecStore; return (*this); };


         /// Get the leapsec data store.
      LeapSecStore* getLeapSecStore() const
      { return pLeapSecStore; };


         /// Get the x pole displacement parameter, in arcseconds
      double getXPole(const CommonTime& UTC) const
         throw (InvalidRequest);


         /// Get the y pole displacement parameter, in arcseconds
      double getYPole(const CommonTime& UTC) const
         throw (InvalidRequest);


         /// Get the value of (UT1 - UTC), in seconds
      double getUT1mUTC(const CommonTime& UTC) const
         throw (InvalidRequest);

         /// Get the value of dPsi, in arcseconds
      double getDPsi(const CommonTime& UTC) const
          throw(InvalidRequest);

         /// Get the value of dEps, in arcseconds
      double getDEps(const CommonTime& UTC) const
          throw(InvalidRequest);


         /// Get the EOP data at the given UTC time.
      EOPDataStore::EOPData getEOPData(const CommonTime& UTC) const
         throw(InvalidRequest);


         /// Get the leapsec data at the given UTC time.
      double getLeapSec(const CommonTime& UTC) const
         throw(InvalidRequest);


      ///------ Methods to get the difference of different time systems ------//


         /// Get the value of (TAI - UTC) (= leapsec).
      double getTAImUTC(const CommonTime& UTC)
         throw(InvalidRequest);


         /// Get the value of (TT - UTC) (= 32.184 + leapsec).
      double getTTmUTC(const CommonTime& UTC)
         throw(InvalidRequest);


         /// Get the value of (TT - TAI) (= 32.184s).
      double getTTmTAI(void)
         throw(InvalidRequest);


         /// Get the value of (TAI - GPST) (= -19s).
      double getTAImGPST(void)
         throw(InvalidRequest);



      ///------ Methods to convert between different time systems ------//


         /// Convert GPST to UTC.
      CommonTime convertGPST2UTC(const CommonTime& GPST);


         /// Convert UTC to GPST.
      CommonTime convertUTC2GPST(const CommonTime& UTC);


         /// Convert UT1 to UTC.
      CommonTime convertUT12UTC(const CommonTime& UT1);


         /// Convert UTC to UT1.
      CommonTime convertUTC2UT1(const CommonTime& UTC);


         /// Convert TAI to UTC.
      CommonTime convertTAI2UTC(const CommonTime& TAI);


         /// Convert UTC to TAI.
      CommonTime convertUTC2TAI(const CommonTime& UTC);


         /// Convert TT to UTC.
      CommonTime convertTT2UTC(const CommonTime& TT);


         /// Convert UTC to TT.
      CommonTime convertUTC2TT(const CommonTime& UTC);



         /// Convert TDB to UTC.
//    CommonTime convertTDB2UTC(const CommonTime& TDB);


         /// Convert UTC to TDB.
//    CommonTime convertUTC2TDB(const CommonTime& UTC);



      ///------ Methods to compute reference system transformation ------//


         /**Computes the mean obliquity of the ecliptic.
          *
          * @param   Mjd_TT    Modified Julian Date (Terrestrial Time)
          * @return            Mean obliquity of the ecliptic
          */
      double MeanObliquity(double Mjd_TT);


         /**Transformation of equatorial to ecliptical coordinates.
          *
          *@param Mjd_TT    Modified Julian Date (Terrestrial Time)
          *@return          Transformation matrix
          */
      Matrix<double> EclMatrix(double Mjd_TT);


         /**Precession transformation of equatorial coordinates.
          *
          *@param Mjd_1     Epoch given (Modified Julian Date TT)
          *@param MjD_2     Epoch to precess to (Modified Julian Date TT)
          *@return          Precession transformation matrix
          */
      Matrix<double> PrecMatrix(double Mjd_1, double Mjd_2);


         /**Nutation in longitude and obliquity.
          *
          *@param  Mjd_TT    Modified Julian Date (Terrestrial Time)
          *@return           Nutation matrix
          */
      void NutAngles(double Mjd_TT, double& dpsi, double& deps);


         /**Transformation from mean to true equator and equinox.
          *
          *@param Mjd_TT    Modified Julian Date (Terrestrial Time)
          *@return          Nutation matrix
          */
      Matrix<double> NutMatrix(double Mjd_TT);


         /**Computation of the equation of the equinoxes.
          *
          *@param  Mjd_TT    Modified Julian Date (Terrestrial Time)
          *@return           Equation of the equinoxes

          * \warning
          * The equation of the equinoxes dpsi*cos(eps) is the right ascension of the 
          * mean equinox referred to the true equator and equinox and is equal to the 
          * difference between apparent and mean sidereal time.
          */
      double EqnEquinox(double Mjd_TT);


         /**Greenwich Mean Sidereal Time.
          *
          *@param  Mjd_UT1   Modified Julian Date UT1
          *@return           GMST in [rad]
          */
      double GMST(double Mjd_UT1);


         /**Greenwich Apparent Sidereal Time.
          *
          *@paam  Mjd_UT1   Modified Julian Date UT1
          *@return          GMST in [rad]
          */
      double GAST(double Mjd_UT1);


         /* Transformation from true equator and equinox to Earth equator and
          * Greenwich meridian system.
          *
          *@param Mjd_UT1   Modified Julian Date UT1
          *@return          Greenwich Hour Angle matrix
          */
      Matrix<double> GHAMatrix(double Mjd_UT1);


         /**Transformation from pseudo Earth-fixed to Earth-fixed coordinates
          * for a given date.
          *
          *@param  Mjd_UTC   Modified Julian Date UTC
          *@return           Pole matrix
          */
      Matrix<double> PoleMatrix(double Mjd_UTC);


         /**Transformation from ICRS to ITRS coordinates for a given time.
          *
          *@param  GPST      GPS Time
          *@return           Rotation matrix
          */
      Matrix<double> getC2TMatrix(CommonTime& GPST);


         /**Transformation from ITRS to ICRS coordinates for a given time.
          *
          *@param  GPST      GPS Time
          *@return           Rotation matrix
          */
      Matrix<double> getT2CMatrix(CommonTime& GPST);



         /**Get the precession matrix.
          *
          * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix'
          * firstly, otherwise, this method will throw 'InvalidRequest'
          */
      Matrix<double> getPrecMatrix( void )
         throw(InvalidRequest);


         /**Get the Nutation matrix.
          *
          * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix'
          * firstly, otherwise, this method will throw 'InvalidRequest'
          */
      Matrix<double> getNutMatrix( void )
         throw(InvalidRequest);


         /**Get the Nutation matrix.
          *
          * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix'
          * firstly, otherwise, this method will throw 'InvalidRequest'
          */
      Matrix<double> getThetaMatrix( void )
         throw(InvalidRequest);


         /**Get the Nutation matrix.
          *
          * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix'
          * firstly, otherwise, this method will throw 'InvalidRequest'
          */
      Matrix<double> getPiMatrix( void )
         throw(InvalidRequest);

        /**Normalize angle into the range -PI <= a <= +PI.
         *
         * @param  a    Angle (radians)
         * @return      Angle in range +/-PI
         */
      double Anpm(double a)
      {
          double w = std::fmod(a, TWO_PI);
          if(fabs(w) >= PI)
          {
              if(a > 0.0)   w -=  TWO_PI;
              else          w -= -TWO_PI;
          }

          return w;
      }


        /**Normalize angle into the range 0 <= a <= 2PI.
         *
         * @param  a    Angle (radians)
         * @return      Angle in range 0-2PI
         */
      double Anp(double a)
      {
          double w = std::fmod(a, TWO_PI);

          if(w < 0.0)
          {
              w += TWO_PI;
          }

          return w;
      }

         // Deconstrutor
      virtual ~ReferenceSystem() {};



         /// Pointer to the EOPDataStore
      EOPDataStore* pEopStore;


         /// Pointer to the leap second store 
      LeapSecStore* pLeapSecStore;


         /// Precession Matrix
      Matrix<double> P;


         /// Nutation Matrix
      Matrix<double> N;


         /// Rotation Matrix
      Matrix<double> Theta;


         /// Polar motion matrix
      Matrix<double> Pi;


         /// whether the transformation matrix is prepared
      bool isPrepared;


   }; // End of class 'ReferenceSystem'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_REFERENCE_SYSTEM_HPP
