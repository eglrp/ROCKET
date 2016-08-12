#pragma ident "$Id$"

/**
* @file EOPDataStore.cpp
*
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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================


#include "EOPDataStore.hpp"
#include "MiscMath.hpp"
#include <fstream>
#include "MJD.hpp"
#include "Exception.hpp"

namespace gpstk
{
   using namespace std;

      // Add to the store directly
   void EOPDataStore::addEOPData(const CommonTime& utc,
                                 const EOPDataStore::EOPData& d)
      throw(InvalidRequest)
   {
      if(!(utc.getTimeSystem()==TimeSystem::UTC)) throw Exception();

      std::vector<double> data(12,0.0);

      data[0] = d.xp;       data[1] = d.err_xp;
      data[2] = d.yp;       data[3] = d.err_yp;
      data[4] = d.UT1mUTC;  data[5] = d.err_UT1mUTC;
      data[6] = d.LOD;      data[7] = d.err_LOD;
      data[8] = d.dX;       data[9] = d.err_dX;
      data[10] = d.dY;      data[11] = d.err_dY;

      addData(utc, data);

   }  // End of 'EOPDataStore::addEOPData()'


   EOPDataStore::EOPData EOPDataStore::getEOPData(const CommonTime& utc) const
         throw(InvalidRequest)
   {
//    if(!(utc.getTimeSystem()==TimeSystem::UTC)) throw Exception();

      std::vector<double> data = getData(utc);

      return EOPData(data[0], data[1],
                     data[2], data[3],
                     data[4], data[5],
                     data[6], data[7],
                     data[8], data[9],
                     data[10], data[11]);

   }  // End of method 'EOPDataStore::getEOPData()'



   void EOPDataStore::loadIERSFile(std::string iersFile)
      throw(FileMissingException)
   {
      ifstream inpf(iersFile.c_str());
      if(!inpf)
      {
         FileMissingException fme("Could not open IERS file " + iersFile);
         GPSTK_THROW(fme);
      }

      clear();

      bool ok (true);
      while(!inpf.eof() && inpf.good())
      {
         string line;
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');
         if(inpf.eof()) break;

         // line length is actually 185
         if(inpf.bad() || line.size() < 70) { ok = false; break; }


         // from Bulletin A
         double mjd = StringUtils::asDouble(line.substr(7,8));

         double xp = StringUtils::asDouble(line.substr(18,9));          // arcsec
         double err_xp = StringUtils::asDouble(line.substr(28,8));      // arcsec

         double yp = StringUtils::asDouble(line.substr(37,9));          // arcsec
         double err_yp = StringUtils::asDouble(line.substr(47,8));      // arcsec

         double UT1mUTC = StringUtils::asDouble(line.substr(58,10));    // seconds
         double err_UT1mUTC = StringUtils::asDouble(line.substr(69,9)); // seconds

         double LOD = StringUtils::asDouble(line.substr(80,6));         // seconds
         double err_LOD = StringUtils::asDouble(line.substr(87,6));     // seconds

         double dX = StringUtils::asDouble(line.substr(100,6))*1e-3;    // milliarcsec->arcsec
         double err_dX = StringUtils::asDouble(line.substr(110,5))*1e-3;// milliarcsec->arcsec

         double dY = StringUtils::asDouble(line.substr(119,6))*1e-3;    // milliarcsec->arcsec
         double err_dY = StringUtils::asDouble(line.substr(129,5))*1e-3;// milliarcsec->arcsec


         // from Bulletin B
         if(line.size()>=134)
         {
             xp = StringUtils::asDouble(line.substr(135,9));        // arcsec
             yp = StringUtils::asDouble(line.substr(145,9));        // arcsec
             UT1mUTC = StringUtils::asDouble(line.substr(155,10));  // seconds
             dX = StringUtils::asDouble(line.substr(169,6))*1e-3;   // milliarcsec->arcsec
             dY = StringUtils::asDouble(line.substr(179,6))*1e-3;   // milliarcsec->arcsec
         }

         addEOPData(MJD(mjd,TimeSystem::UTC),
                    EOPData(xp,err_xp,
                            yp,err_yp,
                            UT1mUTC,err_UT1mUTC,
                            LOD,err_LOD,
                            dX,err_dX,
                            dY,err_dY));
      };
      inpf.close();

      if(!ok)
      {
         FileMissingException fme("IERS File " + iersFile
                                  + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
   }

   void EOPDataStore::loadIGSFile(std::string igsFile)
      throw(FileMissingException)
   {
      ifstream inpf(igsFile.c_str());
      if(!inpf)
      {
         FileMissingException fme("Could not open IERS file " + igsFile);
         GPSTK_THROW(fme);
      }

      clear();

      // first we skip the header section
      // skip the header

      //version 2
      //EOP  SOLUTION
      //  MJD         X        Y     UT1-UTC    LOD   Xsig   Ysig   UTsig LODsig  Nr Nf Nt     Xrt    Yrt  Xrtsig Yrtsig   dpsi    deps
      //               10**-6"        .1us    .1us/d    10**-6"     .1us  .1us/d                10**-6"/d    10**-6"/d        10**-6

      string temp;
      getline(inpf,temp);
      getline(inpf,temp);
      getline(inpf,temp);
      getline(inpf,temp);

      bool ok (true);
      while(!inpf.eof() && inpf.good())
      {
         string line;
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');
         if(inpf.eof()) break;

         // line length is actually 185
         if(inpf.bad() || line.size() < 120) { ok = false; break; }

         istringstream istrm(line);

         double mjd(0.0);
         double xp(0.0),err_xp(0.0), yp(0.0),err_yp(0.0);
         double UT1mUTC(0.0),err_UT1mUTC(0.0), LOD(0.0),err_LOD(0.0);
         double dX(0.0),err_dX(0.0), dY(0.0),err_dY(0.0);

         istrm >> mjd
               >> xp >> yp >> UT1mUTC >> LOD
               >> err_xp >> err_yp >> err_UT1mUTC >> err_LOD;

         // since EOP data from IGS doesn't contain precession and nutation
         // corrections the following code doesn't need.
//         for(int i=0;i<4;i++) istrm >> temp;

//         istrm >> dX >> dY;

         xp *= 1e-6;        // convert to arcsec
         yp *= 1e-6;        // convert to arcsec
         UT1mUTC *= 1e-7;   // convert to seconds
         LOD *= 1e-7;       // convert to seconds/day

//         dX *= 1e-6;        // convert to arcsec
//         dY *= 1e-6;        // convert to arcsec

         addEOPData(MJD(mjd,TimeSystem::UTC),
                    EOPData(xp,err_xp,
                            yp,err_yp,
                            UT1mUTC,err_UT1mUTC,
                            LOD,err_LOD,
                            dX,err_dX,
                            dY,err_dY));
      };
      inpf.close();

      if(!ok)
      {
         FileMissingException fme("IGS File " + igsFile
                                  + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
   }

      /** Add EOPs to the store via a flat STK file.
       *  EOP-v1.1.txt
       *  http://celestrak.com/SpaceData/EOP-format.asp
       *
       *  @param stkFile  Name of file to read, including path.
       */
   void EOPDataStore::loadSTKFile(std::string stkFile)
      throw(FileMissingException)
   {
      std::ifstream fstk(stkFile.c_str());

      int  numData = 0;
      bool bData = false;

      std::string buf;
      while(getline(fstk,buf))
      {
         if(buf.substr(0,19) == "NUM_OBSERVED_POINTS")
         {
            numData = StringUtils::asInt(buf.substr(20));
            continue;
         }
         else if(buf.substr(0,14) == "BEGIN OBSERVED")
         {
            bData = true;
            continue;
         }
         else if(buf.substr(0,13) == "END PREDICTED")
         {
            bData = false;
            break;
         }
         if(!StringUtils::isDigitString(buf.substr(0,4)))
         {
            // for observed data and predicted data
            continue;
         }

         if(bData)
         {
            // # FORMAT(I4,I3,I3,I6,2F10.6,2F11.7,4F10.6,I4)
            //int year = StringUtils::asInt(buf.substr(0,4));
            //int month = StringUtils::asInt(buf.substr(4,3));
            //int day = StringUtils::asInt(buf.substr(7,3));
            double mjd = StringUtils::asInt(buf.substr(10,6));

            double xp = StringUtils::asDouble(buf.substr(16,10));
            double err_xp = 0.0;
            double yp = StringUtils::asDouble(buf.substr(26,10));
            double err_yp = 0.0;
            double UT1mUTC = StringUtils::asDouble(buf.substr(36,11));
            double err_UT1mUTC = 0.0;
            double LOD = StringUtils::asDouble(buf.substr(47,11));
            double err_LOD = 0.0;
//            double dPsi = StringUtils::asDouble(buf.substr(58,10));
//            double dEps = StringUtils::asDouble(buf.substr(68,10));
            double dX = StringUtils::asDouble(buf.substr(78,10));
            double err_dX = 0.0;
            double dY = StringUtils::asDouble(buf.substr(88,10));
            double err_dY = 0.0;
            int UTCmTAI = StringUtils::asDouble(buf.substr(98,4));

            addEOPData(MJD(mjd,TimeSystem::UTC),
                       EOPData(xp,err_xp,
                               yp,err_yp,
                               UT1mUTC,err_UT1mUTC,
                               LOD,err_LOD,
                               dX,err_dX,
                               dY,err_dY));
         }

      }  // End of 'while'

      fstk.close();
   }

   ostream& operator<<(std::ostream& os, const EOPDataStore::EOPData& d)
   {
      os << " " << setw(18) << setprecision(8) << d.xp
         << " " << setw(18) << setprecision(8) << d.yp
         << " " << setw(18) << setprecision(8) << d.UT1mUTC
         << " " << setw(18) << setprecision(8) << d.LOD
         << " " << setw(18) << setprecision(8) << d.dX
         << " " << setw(18) << setprecision(8) << d.dY;


      return os;
   }

}  // End of namespace 'gpstk'

