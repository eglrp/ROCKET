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
//  Kaifa Kuang - Wuhan University . 2015
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
 * @file EGM08GravityModel.cpp
 */

#include "EGM08GravityModel.hpp"
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"
#include "Epoch.hpp"
#include "Legendre.hpp"

using namespace std;
using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{

      /// Load file
   void EGM08GravityModel::loadFile(string file)
      throw(FileMissingException)
   {
      ifstream inpf(file.c_str());

      if(!inpf)
      {
         FileMissingException fme("Could not open EGM file " + file);
         GPSTK_THROW(fme);
      }

      // First, file header
      string temp;
      while( getline(inpf,temp) )
      {
         if(temp.substr(0,11) == "end_of_head") break;
      }

      bool ok(true);

      string line;

      // Then, file data
      while( !inpf.eof() && inpf.good() )
      {
         getline(inpf,line);
         stripTrailing(line,'\r');

         if( inpf.eof() ) break;

         if( inpf.bad() ) { ok = false; break; }

         // degree, order
         int L, M;
         L        =  asInt( line.substr( 5, 4) );
         M        =  asInt( line.substr(10, 4) );

         // Cnm, Snm, sigmaCnm, sigmaSnm
         double C, S, sigmaC, sigmaS;
         C        =  for2doub( line.substr(17,22) );
         S        =  for2doub( line.substr(42,22) );
         sigmaC   =  for2doub( line.substr(68,16) );
         sigmaS   =  for2doub( line.substr(88,16) );

//         cout << setw(4) << L
//              << setw(4) << M
//              << setw(22) << doub2sci(C,22,3)
//              << setw(22) << doub2sci(S,22,3)
//              << setw(22) << doub2sci(sigmaC,16,3)
//              << setw(22) << doub2sci(sigmaS,16,3)
//              << endl;

         int id;
         id = indexTranslator(L,M)-1;

         if(L<=desiredDegree && M<=desiredOrder)
         {
            gmData.normalizedCS(id, 0) = C;
            gmData.normalizedCS(id, 1) = S;
            gmData.normalizedCS(id, 2) = sigmaC;
            gmData.normalizedCS(id, 3) = sigmaS;
         }
         else
         {
            break;
         }

         if(debug)
         {
            cout << setw(22) << doub2sci(gmData.normalizedCS(id,0),22,3) << ' '
                 << setw(22) << doub2sci(gmData.normalizedCS(id,1),22,3) << ' '
                 << setw(22) << doub2sci(gmData.normalizedCS(id,2),22,3) << ' '
                 << setw(22) << doub2sci(gmData.normalizedCS(id,3),22,3) << endl;
         }
      }

      inpf.close();

      if( !ok )
      {
         FileMissingException fme("EGM file " + file + " is corrupted or in wrong format");
         GPSTK_THROW(fme);
      }

   }  // End of method 'EGM08Gravitation::loadFile()'


      /** Compute acceleration (and related partial derivatives) of earth
       *  gravitation.
       * @param utc     time in UTC
       * @param rb      earth body
       * @param sc      spacecraft
       */
   void EGM08GravityModel::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
//       cout << "EGM: " << CivilTime(utc) << endl;

      // make a copy of gmData.normalizedCS
      Matrix<double> CS( gmData.normalizedCS );

//      cout.unsetf(ios_base::fixed);
//      for(int i=0; i<CS.rows(); ++i)
//      {
//          for(int j=0; j<2; ++j)
//          {
//              cout << setw(25) << CS(i,j);
//          }
//          cout << endl;
//      }

      // compute time in years since J2000
      double MJD_UTC = MJD(utc).mjd;
      double ly1 = (MJD_UTC - gmData.refMJD) / 365.25;
      double ly2 = ly1 * ly1;
      double ly3 = ly2 * ly1;

      // Low-degree coefficients of the conventional geopotential model
      // see IERS Conventions 2010, Table 6.2
      // Note: the term C20 has been replaced with the corresponding
      // tide-free one
      const double value[3] =
      {
         // the value of C20, C30 and C40 at 2000.0
//         -0.484165143790815e-3, 0.957161207093473e-6, 0.539965866638991e-6
         -0.48416531e-3, 0.9571612e-6, 0.5399659e-6
//         -0.48416948e-3
      };
      const double rate[3]  =
      {
         // the rate of C20, C30 and C40
         11.6e-12,       4.9e-12,      4.7e-12
      };


      // indexes for degree = 2
      int id20, id21, id22;
      id20 = indexTranslator(2,0) - 1;
      id21 = indexTranslator(2,1) - 1;
      id22 = indexTranslator(2,2) - 1;

      // indexes for degree = 3
      int id30, id31, id32, id33;
      id30 = indexTranslator(3,0) - 1;
      id31 = indexTranslator(3,1) - 1;
      id32 = indexTranslator(3,2) - 1;
      id33 = indexTranslator(3,3) - 1;

      // indexes for degree = 4
      int id40, id41, id42, id43, id44;
      id40 = indexTranslator(4,0) - 1;
      id41 = indexTranslator(4,1) - 1;
      id42 = indexTranslator(4,2) - 1;
      id43 = indexTranslator(4,3) - 1;
      id44 = indexTranslator(4,4) - 1;


      // The instantaneous value of coefficients Cn0 to be used when computing
      // orbits
      // see IERS Conventions 2010, Equations 6.4
      CS(id20, 0) = value[0] + ly1*rate[0];  // C20
      CS(id30, 0) = value[1] + ly1*rate[1];  // C30
      CS(id40, 0) = value[2] + ly1*rate[2];  // C40


      // Coefficients of the IERS (2010) mean pole model
      // see IERS Conventions 2010, Table 7.7

      // until 2010.0, unit: mas/year
      const double xp1[4] = {  55.974, 1.8243,  0.18413,  0.007024 };
      const double yp1[4] = { 346.346, 1.7896, -0.10729, -0.000908 };
      // after 2010.0, unit: mas/year
      const double xp2[2] = {  23.513,  7.6141 };
      const double yp2[2] = { 358.891, -0.6287 };

      // get the mean pole at epoch 2000.0 from IERS Conventions 2010
      // see IERS Conventions 2010, Equation 7.25
      double xpm(0.0), ypm(0.0);

      if(MJD_UTC < 55197.0)    // until 2010.0
      {
         xpm = ( xp1[0] + xp1[1]*ly1 + xp1[2]*ly2 + xp1[3]*ly3 )*1e-3;
         ypm = ( yp1[0] + yp1[1]*ly1 + yp1[2]*ly2 + yp1[3]*ly3 )*1e-3;
      }
      else                     // after 2010.0
      {
         xpm = ( xp2[0] + xp2[1]*ly1 )*1e-3;
         ypm = ( yp2[0] + yp2[1]*ly1 )*1e-3;
      }

      // convert pole position from arcseconds to radians
      xpm = xpm * AS_TO_RAD;
      ypm = ypm * AS_TO_RAD;

      // Rotate from the Earth-fixed frame, where the coefficients are
      // pertinent, to an inertial frame, where the satellite motion is
      // computed
      // see IERS Conventions 2010, Equation 6.5
      //
      // C21 = +sqrt(3)*xpm*C20 - xpm*C22 + ypm*S22
      // S21 = -sqrt(3)*ypm*C20 - ypm*C22 - xpm*S22
      //
      double C20 = CS(id20,0);
      double C22 = CS(id22,0); double S22 = CS(id22,1);

      double C21 = +std::sqrt(3.0)*xpm*C20 - xpm*C22 + ypm*S22;
      double S21 = -std::sqrt(3.0)*ypm*C20 - ypm*C22 - xpm*S22;

      CS(id21,0) = C21; CS(id21,1) = S21;


      // corrections of CS
      Matrix<double> dCS;

      // solid tide
      if(pSolidTide != NULL)
      {
         dCS = pSolidTide->getSolidTide(utc);

         for(int i=0; i<dCS.rows(); ++i)
         {
             CS(i,0) += dCS(i,0);
             CS(i,1) += dCS(i,1);
         }
      }

      // ocean tide
      if(pOceanTide != NULL)
      {
         dCS = pOceanTide->getOceanTide(utc);

         for(int i=0; i<dCS.rows(); ++i)
         {
             CS(i,0) += dCS(i,0);
             CS(i,1) += dCS(i,1);
         }
      }

      // pole tide
      if(pPoleTide != NULL)
      {
         dCS = pPoleTide->getPoleTide(utc);

         for(int i=0; i<dCS.rows(); ++i)
         {
             CS(i,0) += dCS(i,0);
             CS(i,1) += dCS(i,1);
         }
      }


      // satellite position in ICRS
      Vector<double> r_sat_icrs = sc.getCurrentPos();

      // transformation matrixes between ICRS and ITRS
      Matrix<double> C2T = pRefSys->C2TMatrix(utc);
      Matrix<double> T2C = transpose(C2T);

      // satellite position in ITRS
      Vector<double> r_sat_itrs = C2T * r_sat_icrs;
      double rx = r_sat_itrs(0);
      double ry = r_sat_itrs(1);
      double rz = r_sat_itrs(2);

      // geocentric distance, latitude and longitude of satellite
      // the latitude is in [-PI/2,+PI/2], the longitude is in [-PI,+PI]
      double rho = norm(r_sat_itrs);
      double lat = std::atan( rz / std::sqrt(rx*rx + ry*ry) );
      double lon = std::atan2( ry, rx );

      // sine and cosine of geocentric latitude
      double slat = std::sin(lat);
      double clat = std::cos(lat);

      // sine and cosine of geocentric longitude
      //    slon(0) = sin(0*lon);
      //    slon(1) = sin(1*lon);
      //        ......
      //    slon(i) = slon(i-1)*cos(lon) + clon(i-1)*sin(lon)
      //    clon(i) = clon(i-1)*cos(lon) - slon(i-1)*sin(lon)
      Vector<double> slon(desiredDegree+1,0.0);
      Vector<double> clon(desiredDegree+1,0.0);

      slon(0) = 0.0;            clon(0) = 1.0;
      slon(1) = std::sin(lon);  clon(1) = std::cos(lon);

      for(int i=2; i<=desiredDegree; ++i)
      {
          slon(i) = slon(i-1)*clon(1) + clon(i-1)*slon(1);
          clon(i) = clon(i-1)*clon(1) - slon(i-1)*slon(1);
      }

      // fully normalized associated legendre functions and its gradients
      Vector<double> leg0, leg1, leg2;
      legendre(desiredDegree, lat, leg0, leg1, leg2, 2);

      // partials of (rho, lat, lon) in ITRS to (x, y, z) in ITRS
      Matrix<double> b(3,3,0.0);
      b(0,0) =  clat * clon(1);
      b(0,1) =  clat * slon(1);
      b(0,2) =  slat;
      b(1,0) = -slat * clon(1) / rho;
      b(1,1) = -slat * slon(1) / rho;
      b(1,2) =  clat / rho;
      b(2,0) = -slon(1) / (rho * clat);
      b(2,1) =  clon(1) / (rho * clat);


      // partials of b to (rho, lat, lon) in ITRS

      Matrix<double> db_drho(3,3,0.0);     // db / drho
      db_drho(1,0) =  slat * clon(1) / (rho*rho);
      db_drho(1,1) =  slat * slon(1) / (rho*rho);
      db_drho(1,2) = -clat / (rho*rho);
      db_drho(2,0) =  slon(1) / (rho*rho * clat);
      db_drho(2,1) = -clon(1) / (rho*rho * clat);

      Matrix<double> db_dlat(3,3,0.0);     // db / dlat
      db_dlat(0,0) = -slat * clon(1);
      db_dlat(0,1) = -slat * slon(1);
      db_dlat(0,2) =  clat;
      db_dlat(1,0) = -clat * clon(1) / rho;
      db_dlat(1,1) = -clat * slon(1) / rho;
      db_dlat(1,2) = -slat / rho;
      db_dlat(2,0) = -slat * slon(1) / (rho * clat*clat);
      db_dlat(2,1) =  slat * clon(1) / (rho * clat*clat);

      Matrix<double> db_dlon(3,3,0.0);     // db / dlon
      db_dlon(0,0) = -clat * slon(1);
      db_dlon(0,1) =  clat * clon(1);
      db_dlon(1,0) =  slat * slon(1) / rho;
      db_dlon(1,1) = -slat * clon(1) / rho;
      db_dlon(2,0) = -clon(1) / (rho * clat);
      db_dlon(2,1) = -slon(1) / (rho * clat);


      ////////////// partials of v to (rho, lat, lon) in ITRS /////////////////
      //                                                                     //
      //                            | dv / drho |                            //
      //                   f_rll =  | dv / dlat |                            //
      //                            | dv / dlon |                            //
      //                                                                     //
      /////////////////////////////////////////////////////////////////////////
      Vector<double> f_rll(3,0.0);

      ////////////// partials of f_rll to (rho, lat, lon) in ITRS /////////////
      //                                                                     //
      //          | df_rll(0) / drho, df_rll(0) / dlat, df_rll(0) / dlon |   //
      //  g_rll = | df_rll(1) / drho, df_rll(1) / dlat, df_rll(1) / dlon |   //
      //          | df_rll(2) / drho, df_rll(2) / dlat, df_rll(2) / dlon |   //
      //                                                                     //
      /////////////////////////////////////////////////////////////////////////
      Matrix<double> g_rll(3,3,0.0);


      ////// partials of df_rll to (Cnm, Snm), ([n*(n+1)/2] + (m+1)], 6) //////
      //                                                                     //
      //                   c_rll =  | df_rll(0:2) / dCnm |                   //
      //                   s_rll =  | df_rll(0:2) / dSnm |                   //
      //                                                                     //
      //                df_itrs_dcs =  | c_rll, s_rll, ... |                 //
      //                                                                     //
      /////////////////////////////////////////////////////////////////////////
      Vector<double> c_rll(3,0.0), s_rll(3,0.0);
      Matrix<double> df_itrs_dcs;
      if(satGravimetry)     // need to do: consider Sn0 = 0 !!!
      {
         int size = indexTranslator(desiredDegree,desiredOrder) - 1;
         df_itrs_dcs.resize(3,size*2,0.0);
      }

      // (GM/rho)^1, (GM/rho)^2, (GM/rho)^3
      double gm_r1 = gmData.GM / rho;
      double gm_r2 = gm_r1 / rho;
      double gm_r3 = gm_r2 / rho;

      // loop for degree
      for(int n=0; n<=desiredDegree; ++n)
      {
         // (ae/rho)^n
         double fct = std::pow(gmData.ae/rho, n);

         // loop for order
         for(int m=0; m<=n && m<=desiredOrder; ++m)
         {
            // index for (n,m)
            int idnm = indexTranslator(n,m) - 1;

            // Pnm and its derivatives wrt latitude
            double P0 = leg0(idnm);
            double P1 = leg1(idnm);
            double P2 = leg2(idnm);

            // Cnm, Snm
            double Cnm = CS(idnm, 0);
            double Snm = CS(idnm, 1);

            // sin(m*lon) and cos(m*lon)
            double smlon = slon(m);
            double cmlon = clon(m);

            // f_rll
            f_rll(0) += -gm_r2 * (n+1) * fct * P0 * (Cnm*cmlon+Snm*smlon);
            f_rll(1) +=  gm_r1 * fct * P1 * (Cnm*cmlon+Snm*smlon);
            f_rll(2) +=  gm_r1 * m * fct * P0 * (-Cnm*smlon+Snm*cmlon);

            // g_rll
            g_rll(0,0) +=  gm_r3 * (n+1)*(n+2) * fct * P0 * (Cnm*cmlon+Snm*smlon);
            g_rll(0,1) += -gm_r2 * (n+1) * fct * P1 * (Cnm*cmlon+Snm*smlon);
            g_rll(0,2) += -gm_r2 * (n+1)*m * fct * P0 * (-Cnm*smlon+Snm*cmlon);
            g_rll(1,1) +=  gm_r1 * fct * P2 * (Cnm*cmlon+Snm*smlon);
            g_rll(1,2) +=  gm_r1 * m * fct * P1 * (-Cnm*smlon+Snm*cmlon);
            g_rll(2,2) += -gm_r1 * m*m * fct * P0 * (Cnm*cmlon+Snm*smlon);

            if(satGravimetry)
            {
               // c_rll
               c_rll(0) = -gm_r2 * (n+1) * fct * P0 * cmlon;
               c_rll(1) =  gm_r1 * fct * P1 * cmlon;
               c_rll(2) = -gm_r1 * m * fct * P0 * smlon;

               // s_rll
               s_rll(0) = -gm_r2 * (n+1) * fct * P0 * smlon;
               s_rll(1) =  gm_r1 * fct * P1 * smlon;
               s_rll(2) =  gm_r1 * m * fct * P0 * cmlon;

               // df_itrs_dcs
               df_itrs_dcs(0,idnm+0) = c_rll(0)*b(0,0) + c_rll(1)*b(1,0) + c_rll(2)*b(2,0);
               df_itrs_dcs(1,idnm+0) = c_rll(0)*b(0,1) + c_rll(1)*b(1,1) + c_rll(2)*b(2,1);
               df_itrs_dcs(2,idnm+0) = c_rll(0)*b(0,2) + c_rll(1)*b(1,2) + c_rll(2)*b(2,2);
               df_itrs_dcs(0,idnm+1) = s_rll(0)*b(0,0) + s_rll(1)*b(1,0) + s_rll(2)*b(2,0);
               df_itrs_dcs(1,idnm+1) = s_rll(0)*b(0,1) + s_rll(1)*b(1,1) + s_rll(2)*b(2,1);
               df_itrs_dcs(2,idnm+1) = s_rll(0)*b(0,2) + s_rll(1)*b(1,2) + s_rll(2)*b(2,2);
            }

         }   // End of "for(int m=0; ...)"

      }   // End of "for(int n=0; ...)"


      // symmetry of gradiometry
      g_rll(1,0) = g_rll(0,1);
      g_rll(2,0) = g_rll(0,2);
      g_rll(2,1) = g_rll(1,2);

      // gravitation acceleration in ICRS
      a = T2C * transpose(b) * f_rll;


      // partials of gravitation acceleration in ICRS to (x, y, z) in ICRS
      Matrix<double> df_itrs_drll(3,3,0.0);
      for(int i=0; i<3; ++i)
      {
          df_itrs_drll(i,0) = g_rll(0,0)*b(0,i) + f_rll(0)*db_drho(0,i)
                            + g_rll(1,0)*b(1,i) + f_rll(1)*db_drho(1,i)
                            + g_rll(2,0)*b(2,i) + f_rll(2)*db_drho(2,i);
          df_itrs_drll(i,1) = g_rll(0,1)*b(0,i) + f_rll(0)*db_dlat(0,i)
                            + g_rll(1,1)*b(1,i) + f_rll(1)*db_dlat(1,i)
                            + g_rll(2,1)*b(2,i) + f_rll(2)*db_dlat(2,i);
          df_itrs_drll(i,2) = g_rll(0,2)*b(0,i) + f_rll(0)*db_dlon(0,i)
                            + g_rll(1,2)*b(1,i) + f_rll(1)*db_dlon(1,i)
                            + g_rll(2,2)*b(2,i) + f_rll(2)*db_dlon(2,i);
      }

      da_dr = T2C * df_itrs_drll * b * C2T;


      // partials of gravitation acceleration in ICRS to (vx, vy, vz) in ICRS
      da_dv.resize(3,3,0.0);


      // partials of gravitation acceleration in ICRS to (Cnm, Snm)
      if(satGravimetry)
      {
         da_dEGM = T2C * df_itrs_dcs;
      }


   }  // End of method 'EGM08GravityModel::doCompute()'


}   // End of namespace 'gpstk'
