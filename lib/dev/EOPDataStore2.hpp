#pragma ident "$Id$"

/**
* @file EOPDataStore2.hpp
*
*/

#ifndef GPSTK_EOPDATASTORE2_HPP
#define GPSTK_EOPDATASTORE2_HPP


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

        enum InterpolationMethod
        {
            Nearest,
            Linear,
            Lagrange9th,
            CubicSpline
        };

    public:

        /// Default constructor
        EOPDataStore2()
            : initialTime(CommonTime::END_OF_TIME),
              finalTime(CommonTime::BEGINNING_OF_TIME),
              interpMethod(Lagrange9th)
        {}


        /// Default deconstructor
        virtual ~EOPDataStore2()
        {
            allData.clear();
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


        /// Provide the diurnal/subdiurnal tidal effects on polar motions ("),
        /// UT1 (s) and LOD (s), in time domain.
        void PMUT1_OCEANS(const CommonTime& utc, double cor[4]);


        /// Provide the diurnal lunisolar effect on polar motion ("), in time
        /// domain.
        void PMSDNUT2(const CommonTime& utc, double cor[2]);


        /// Evaluate the model of subdiurnal libration in the axial component
        /// of rotation, expressed by UT1 and LOD.
        void UTLIBR(const CommonTime& utc, double cor[2]);


    protected:

        /// Map holding all the EOP data
        EOPDataMap allData;

        /// Time span for the EOP data
        CommonTime initialTime;
        CommonTime finalTime;

        /// Interpolation method, default is Lagrange 9th
        InterpolationMethod interpMethod;

    }; // End of class 'EOPDataStore2'


    std::ostream& operator<<(std::ostream& s, const EOPDataStore2::EOPData& d);

    // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_EOPDATASTORE2_HPP







