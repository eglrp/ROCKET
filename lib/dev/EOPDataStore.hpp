#pragma ident "$Id$"

/**
* @file EOPDataStore.hpp
*
*/

#ifndef GPSTK_EOPDATASTORE_HPP
#define GPSTK_EOPDATASTORE_HPP


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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================


#include "EpochDataStore.hpp"
#include "Triple.hpp"
#include "MiscMath.hpp"

using namespace std;

namespace gpstk
{
      /** @addtogroup Precise Orbit Determination */
      //@{

      /**
       * Class to store and manage Earth Orientation data.
       *
       */
   class EOPDataStore : public EpochDataStore
   {
   public:

      typedef struct EOPData
      {
         double xp;         /// arcsec
         double err_xp;     /// arcsec
         double yp;         /// arcsec
         double err_yp;     /// arcsec
         double UT1mUTC;    /// seconds
         double err_UT1mUTC;/// seconds
         double LOD;        /// seconds
         double err_LOD;    /// seconds
         double dX;         /// arcsec
         double err_dX;     /// arcsec
         double dY;         /// arcsec
         double err_dY;     /// arcsec

         EOPData()
             : xp(0.0), err_xp(0.0),
               yp(0.0), err_yp(0.0),
               UT1mUTC(0.0), err_UT1mUTC(0.0),
               LOD(0.0), err_LOD(0.0),
               dX(0.0), err_dX(0.0),
               dY(0.0), err_dY(0.0)
         {}

         EOPData(double x, double ex,
                 double y, double ey,
                 double dut1, double edut1,
                 double lod, double elod,
                 double X = 0.0, double eX = 0.0,
                 double Y = 0.0, double eY = 0.0)
            : xp(x), err_xp(ex),
              yp(y), err_yp(ey),
              UT1mUTC(dut1), err_UT1mUTC(edut1),
              LOD(lod), err_LOD(elod),
              dX(X), err_dX(eX),
              dY(Y), err_dY(eY)
         {}

      } EOPData;

         /// Default constructor
      EOPDataStore() : EpochDataStore(10)
      {}

         /// Default deconstructor
      virtual ~EOPDataStore() {}

         /// Add to the store directly
      void addEOPData(const CommonTime& utc,const EOPData& data)
         throw(InvalidRequest);

         /// Get the data at the given epoch and return it.
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
      };


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
      };


         /// Get the data at the given epoch and return it.
      double getUT1mUTC(const CommonTime& utc) const
         throw(InvalidRequest)
      {
            // Get the EOPData
         EOPData eopData(  getEOPData(utc) );

            // Get yp value
         double ut1mutc( eopData.UT1mUTC );

            // return
         return ut1mutc;

      };


         /// Add EOPs to the store via a flat IERS file.
         /// get finals.data from http://maia.usno.navy.mil/
      void loadIERSFile(std::string iersFile)
         throw(FileMissingException);


      void loadIGSFile(std::string igsFile)
         throw(FileMissingException);

         /** Add EOPs to the store via a flat STK file.
          *  EOP-v1.1.txt
          *  http://celestrak.com/SpaceData/EOP-format.asp
          *
          *  @param stkFile  Name of file to read, including path.
          */
      void loadSTKFile(std::string stkFile)
         throw(FileMissingException);

   protected:


   }; // End of class 'EOPDataStore'


   std::ostream& operator<<(std::ostream& s, const EOPDataStore::EOPData& d);

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_EOPDATASTORE_HPP







