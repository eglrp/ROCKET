#pragma ident "$Id$"

/**
* @file EOPDataStore2.hpp
*
*/

#ifndef EOPDATASTORE2_HPP
#define EOPDATASTORE2_HPP


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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================


#include <iostream>
#include <map>
#include "CommonTime.hpp"
#include "Vector.hpp"


namespace gpstk
{
    /** @addtogroup Precise Orbit Determination */
    //@{

    /**
     * Class to store and manage EOP data.
     *
     */
    class EOPDataStore2
    {
    public:

        struct EOPData
        {
            double xp, yp;          /// arcsec
            double UT1mUTC;         /// seconds
            double LOD;             /// seconds/day
            double dX, dY;          /// arcsec

            double err_xp, err_yp;  /// arcsec
            double err_UT1mUTC;     /// seconds
            double err_LOD;         /// seconds/day
            double err_dX, err_dY;  /// arcsec
        };

        typedef std::map<CommonTime, EOPData> EOPDataMap;

    public:

        /// Default constructor
        EOPDataStore2()
            : initialTime(CommonTime::END_OF_TIME),
              finalTime(CommonTime::BEGINNING_OF_TIME),
              interpPoints(8),
              useBulletinB(false),
              regularization(false),
              oceanTides(false),
              libration(false)
        {}


        /// Default deconstructor
        virtual ~EOPDataStore2()
        {
            allData.clear();
        }


        /// Set interpolation points
        inline EOPDataStore2& setInterpolationPoints(const int& points)
        {
            interpPoints = points;

            return (*this);
        }


        /// Set use Bulletin B, only valid for IERS EOP data.
        /// Note: this method MUST be called before the file
        /// Load methods to be valid.
        inline EOPDataStore2& setUseBulletinB(const bool& use)
        {
            useBulletinB = use;

            return (*this);
        }


        /// Set regularization
        inline EOPDataStore2& setRegularization(const bool& reg)
        {
            regularization = reg;

            return (*this);
        }


        /// Set ocean tides
        inline EOPDataStore2& setOceanTides(const bool& ocean)
        {
            oceanTides = ocean;

            return (*this);
        }


        /// Set libration
        inline EOPDataStore2& setLibration(const bool& libr)
        {
            libration = libr;

            return (*this);
        }


        /// Add to the store directly
        void addEOPData(const CommonTime& utc,const EOPData& data)
            throw(InvalidRequest);


        /// Get the data at the given epoch
        EOPData getEOPData(const CommonTime& utc) const
            throw(InvalidRequest);


        /// Get the data at the given epoch and return it.
        double getXPole(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get yp value
            double xp( eopData.xp );

            // return
            return xp;
        }


        /// Get the data at the given epoch and return it.
        double getYPole(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get yp value
            double yp( eopData.yp );

            // return
            return yp;
      }


        /// Get the data at the given epoch and return it.
        double getUT1mUTC(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get UT1mUTC value
            double UT1mUTC( eopData.UT1mUTC );

            // return
            return UT1mUTC;
        }


        /// Get the data at the given epoch and return it.
        double getLOD(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get LOD value
            double LOD( eopData.LOD );

            // return
            return LOD;
        }


        /// Get the data at the given epoch and return it.
        double getDX(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get dX value
            double dX( eopData.dX );

            // return
            return dX;
        }


        /// Get the data at the given epoch and return it.
        double getDY(const CommonTime& utc) const
            throw(InvalidRequest)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get dY value
            double dY( eopData.dY );

            // return
            return dY;
        }


        /// Add EOPs to the store via a flat IERS file.
        /// get finals.data from http://maia.usno.navy.mil/
        void loadIERSFile(std::string iersFile)
            throw(FileMissingException);


        /// Add EOPs to the store via a flat IGS file.
        /// get igs*.erp data from IGS ftps
        void loadIGSFile(std::string igsFile)
            throw(FileMissingException);


        /// Add EOPs to the store via a flat STK file.
        /// get eop*.txt from http://celestrak.com/
        void loadSTKFile(std::string stkFile)
            throw(FileMissingException);


        ///------ Methods to compute EOP corrections for tide effects ------//


        /// Evaluate the effects of zonal Earth tides on the rotation of the
        /// Earth, IERS Conventions 2010
        Vector<double> RG_ZONT2(const CommonTime& TT) const;


        /// Provide the diurnal/subdiurnal tidal effects on polar motions ("),
        /// UT1 (s) and LOD (s), in time domain.
        Vector<double> PMUT_OCEAN(const CommonTime& UTC) const;


        /// Provide the diurnal lunisolar effect on polar motion ("), in time
        /// domain.
        Vector<double> PM_LIBR(const CommonTime& UTC) const;


        /// Evaluate the model of subdiurnal libration in the axial component
        /// of rotation, expressed by UT1 and LOD.
        Vector<double> UT_LIBR(const CommonTime& UTC) const;


    protected:

        /// Map holding all the EOP data
        EOPDataMap allData;

        /// Time span for the EOP data
        CommonTime initialTime;
        CommonTime finalTime;

        /// Points of interpolation, default is 8
        int interpPoints;

        /// Use Bulletin B or not, only valid for IERS EOP Data
        bool useBulletinB;

        /// Do regularization or not, default false
        bool regularization;

        /// Do ocean tides correction or not, default false
        bool oceanTides;

        /// Do libration correction or not, default false
        bool libration;

    }; // End of class 'EOPDataStore2'


    std::ostream& operator<<(std::ostream& s, const EOPDataStore2::EOPData& d);

    // @}

}  // End of namespace 'gpstk'

#endif   // EOPDATASTORE2_HPP
