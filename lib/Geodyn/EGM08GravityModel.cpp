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
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

   // Load EGM file
   void EGM08GravityModel::loadEGMFile(string file)
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

      // Then, file data
      while( !inpf.eof() && inpf.good() )
      {
         string line;
         getline(inpf,line);
         stripTrailing(line,'\r');

         if(inpf.eof()) break;

         if(inpf.bad()) { ok = false; break; }
 
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

         int i = index(L,M);

         if(i <= gmData.normalizedCS.rows())
         {
            gmData.normalizedCS(i-1,0) = C;
            gmData.normalizedCS(i-1,1) = S;
            gmData.normalizedCS(i-1,2) = sigmaC;
            gmData.normalizedCS(i-1,3) = sigmaS;
         }
         else
         {
            break;
         }

//         cout << setw(22) << doub2sci(gmData.normalizedCS(i,0),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,1),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,2),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,3),22,3) << endl;
      }

      inpf.close();

      if(!ok)
      {
         FileMissingException fme("EGM file" + file + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }

   }

   /** Compute the acceleration due to earth gravitation.
    * @param utc Time reference class
    * @param rb  Reference body class  
    * @param sc  Spacecraft parameters and state
    * @return the acceleration [m/s^2]
    */
   void EGM08GravityModel::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
      // compute time in years since J2000
      double MJD_UTC = MJD(utc).mjd;
      double ly1 = (MJD_UTC - gmData.refMJD) / 365.25;
      double ly2 = ly1 * ly1;
      double ly3 = ly2 * ly1;

      // Low-degree coefficients of the conventional geopotential model
      // see IERS Conventions 2010, Table 6.2
      // Note: the term C20 has been replaced with the corresponding
      // tide-free one
      const double value[3] = {
         // the value of C20, C30 and C40 at 2000.0
         -0.48416531e-3, 0.9571612e-6, 0.5399659e-6
//       -0.48416948e-3
      };
      const double rate[3]  = {
         // the rate of C20, C30 and C40
         11.6e-12,       4.9e-12,      4.7e-12
      };

      // The instantaneous value of coefficients Cn0 to be used when computing
      // orbits
      // see IERS Conventions 2010, Equations 6.4
      if(desiredDegree >= 2)
      {
         gmData.normalizedCS(index(2,0)-1, 0) = value[0] + ly1*rate[0];
      }
      if(desiredDegree >= 3)
      {
         gmData.normalizedCS(index(3,0)-1, 0) = value[1] + ly1*rate[1];
      }
      if(desiredDegree >= 4)
      {
         gmData.normalizedCS(index(4,0)-1, 1) = value[2] + ly1*rate[2];
      }


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
      double xpm, ypm;

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
      if(desiredDegree >= 2)
      {
         gmData.normalizedCS(index(2,1)-1, 0)
            = + std::sqrt(3.0)*xpm*gmData.normalizedCS(index(2,0)-1, 0)
              - xpm*gmData.normalizedCS(index(2,2)-1, 0)
              + ypm*gmData.normalizedCS(index(2,2)-1, 1);
         gmData.normalizedCS(index(2,1)-1, 1)
            = - std::sqrt(3.0)*ypm*gmData.normalizedCS(index(2,0)-1, 0)
              - ypm*gmData.normalizedCS(index(2,2)-1, 0)
              - xpm*gmData.normalizedCS(index(2,2)-1, 1);
     }

      // correct solid tide
      if(correctSolidTide)
      {
         double dC[10] = {0.0};
         double dS[10] = {0.0};
         solidTide.getSolidTide(utc, dC, dS);

//         cout << "solid tide: " << endl;
//         for(int i=0; i<10; i++)
//         {
//            cout << setw(18) << setprecision(12) << dC[i] << ' ' << dS[i] << endl;
//         }

         // degree >= 2
         if(desiredDegree >= 2)
         {
            gmData.normalizedCS(index(2,0)-1, 0) += dC[0];  // C20
            gmData.normalizedCS(index(2,1)-1, 0) += dC[1];  // C21
            gmData.normalizedCS(index(2,2)-1, 0) += dC[2];  // C22
            gmData.normalizedCS(index(2,0)-1, 1) += dS[0];  // S20
            gmData.normalizedCS(index(2,1)-1, 1) += dS[1];  // S21
            gmData.normalizedCS(index(2,2)-1, 1) += dS[2];  // S22
         }
         // degree >= 3
         if(desiredDegree >= 3)
         {
            gmData.normalizedCS(index(3,0)-1, 0) += dC[3];  // C30
            gmData.normalizedCS(index(3,1)-1, 0) += dC[4];  // C31
            gmData.normalizedCS(index(3,2)-1, 0) += dC[5];  // C32
            gmData.normalizedCS(index(3,3)-1, 0) += dC[6];  // C33
            gmData.normalizedCS(index(3,0)-1, 1) += dS[3];  // S30
            gmData.normalizedCS(index(3,1)-1, 1) += dS[4];  // S31
            gmData.normalizedCS(index(3,2)-1, 1) += dS[5];  // S32
            gmData.normalizedCS(index(3,3)-1, 1) += dS[6];  // S33
         }
         // degree >= 4
         if(desiredDegree >= 4)
         {
            gmData.normalizedCS(index(4,0)-1, 0) += dC[7];  // C40
            gmData.normalizedCS(index(4,1)-1, 0) += dC[8];  // C41
            gmData.normalizedCS(index(4,2)-1, 0) += dC[9];  // C42
            gmData.normalizedCS(index(4,0)-1, 1) += dS[7];  // S40
            gmData.normalizedCS(index(4,1)-1, 1) += dS[8];  // S41
            gmData.normalizedCS(index(4,2)-1, 1) += dS[9];  // S42
         }
      }

      // correct ocean tide
      if(correctOceanTide)
      {
         double dC[12] = {0.0};
         double dS[12] = {0.0};
         oceanTide.getOceanTide(MJD_UTC, dC, dS);
   
//         cout << "ocean tide: " << endl;
//         for(int i=0; i<12; i++)
//         {
//            cout << setw(18) << setprecision(12) << dC[i] << ' ' << dS[i] << endl;   
//         }

         // degree >= 2
         if(desiredDegree >= 2)
         {
            gmData.normalizedCS(index(2,0)-1, 0) += dC[0];   // C20
            gmData.normalizedCS(index(2,1)-1, 0) += dC[1];   // C21
            gmData.normalizedCS(index(2,2)-1, 0) += dC[2];   // C22
            gmData.normalizedCS(index(2,0)-1, 1) += dS[0];   // S20
            gmData.normalizedCS(index(2,1)-1, 1) += dS[1];   // S21
            gmData.normalizedCS(index(2,2)-1, 1) += dS[2];   // S22
         }
         // degree >= 3
         if(desiredDegree >= 3)
         {
            gmData.normalizedCS(index(3,0)-1, 0) += dC[3];   // C30
            gmData.normalizedCS(index(3,1)-1, 0) += dC[4];   // C31
            gmData.normalizedCS(index(3,2)-1, 0) += dC[5];   // C32
            gmData.normalizedCS(index(3,3)-1, 0) += dC[6];   // C33
            gmData.normalizedCS(index(3,0)-1, 1) += dS[3];   // S30
            gmData.normalizedCS(index(3,1)-1, 1) += dS[4];   // S31
            gmData.normalizedCS(index(3,2)-1, 1) += dS[5];   // S32
            gmData.normalizedCS(index(3,3)-1, 1) += dS[6];   // S33
         }
         // degree >= 4
         if(desiredDegree >= 4)
         {
            gmData.normalizedCS(index(4,0)-1, 0) += dC[7];   // C40
            gmData.normalizedCS(index(4,1)-1, 0) += dC[8];   // C41
            gmData.normalizedCS(index(4,2)-1, 0) += dC[9];   // C42
            gmData.normalizedCS(index(4,3)-1, 0) += dC[10];  // C43
            gmData.normalizedCS(index(4,4)-1, 0) += dC[11];  // C44
            gmData.normalizedCS(index(4,0)-1, 1) += dS[7];   // S40
            gmData.normalizedCS(index(4,1)-1, 1) += dS[8];   // S41
            gmData.normalizedCS(index(4,2)-1, 1) += dS[9];   // S42
            gmData.normalizedCS(index(4,3)-1, 1) += dS[10];  // S43
            gmData.normalizedCS(index(4,4)-1, 1) += dS[11];  // S44
         }

      }

      // correct pole tide
      if(correctPoleTide)
      {
         double dC21 = 0.0;
         double dS21 = 0.0;
         poleTide.getPoleTide(MJD_UTC, dC21, dS21);

//         cout << "pole tide: " << endl;
//         cout << setw(18) << setprecision(12) << dC21 << ' ' << dS21 << endl;

         // degree >= 2
         if(desiredDegree >= 2)
         {
            gmData.normalizedCS(index(2,1)-1, 0) += dC21;   // C21
            gmData.normalizedCS(index(2,1)-1, 1) += dS21;   // S21
         }
      }

//      cout << setw(18) << setprecision(12) << gmData.normalizedCS << endl;

      // satellite position in ECI
      Vector<double> r_Sat_ECI = sc.R();

      // transformation matrixs between ECI and ECEF
      Matrix<double> C2T = C2TMatrix(utc);
      Matrix<double> T2C = transpose(C2T);

      // satellite position in ECEF
      Vector<double> r_Sat_ECEF = C2T * r_Sat_ECI;
      double rx = r_Sat_ECEF(0);
      double ry = r_Sat_ECEF(1);
      double rz = r_Sat_ECEF(2);

      // geocentric distance, latitude and longitude of satellite
      double rho = norm(r_Sat_ECEF);
      double lat = std::atan2( rz, std::sqrt(rx*rx + ry*ry) );
      double lon = std::atan2( ry, rx );

      // sine and cosine of geocentric latitude and longitude of satellite
      double slat = std::sin(lat);
      double slon = std::sin(lon);
      double clat = std::cos(lat);
      double clon = std::cos(lon);

      // fully normalized associated legendre functions and its gradients
      Vector<double> leg0, leg1, leg2;
      legendre(desiredDegree, lat, leg0, leg1, leg2);

      // partials of (rho, lat, lon) to (x, y, z)
      Matrix<double> b(3,3,0.0);
      b(0,0) =  clat * clon;
      b(0,1) =  clat * slon;
      b(0,2) =  slat;
      b(1,0) = -slat * clon / rho;
      b(1,1) = -slat * slon / rho;
      b(1,2) =  clat / rho;
      b(2,0) = -slon / (rho * clat);
      b(2,1) =  clon / (rho * clat);


      // partials of b matrix to (rho, lat, lon)
      
      Matrix<double> dbdrho(3,3,0.0);     // db / drho
      dbdrho(1,0) =  slat * clon / (rho*rho);
      dbdrho(1,1) =  slat * slon / (rho*rho);
      dbdrho(1,2) = -clat / (rho*rho);
      dbdrho(2,0) =  slon / (rho*rho * clat);
      dbdrho(2,1) = -clon / (rho*rho * clat);

      Matrix<double> dbdlat(3,3,0.0);     // db / dlat
      dbdlat(0,0) = -slat * clon;
      dbdlat(0,1) = -slat * slon;
      dbdlat(0,2) =  clat;
      dbdlat(1,0) = -clat * clon / rho;
      dbdlat(1,1) = -clat * slon / rho;
      dbdlat(1,2) = -slat / rho;
      dbdlat(2,0) = -slat * slon / (rho * clat*clat);
      dbdlat(2,1) =  slat * clon / (rho * clat*clat);

      Matrix<double> dbdlon(3,3,0.0);     // db / dlon
      dbdlon(0,0) = -clat * slon;
      dbdlon(0,1) =  clat * clon;
      dbdlon(1,0) =  slat * slon / rho;
      dbdlon(1,1) = -slat * clon / rho;
      dbdlon(2,0) = -clon / (rho * clat);
      dbdlon(2,1) = -slon / (rho * clat);


      // partials of gravitation potential V to spherical coordinates (rho, lat, lon)
      //
      //       | dV / drho |  
      //  dV = | dV / dlat |
      //       | dV / dlon |
      //
      Vector<double> dV(3,0.0);  

      //        | d(dV/drho) / drho, d(dV/drho) / dlat, d(dV/drho) / dlon |
      //  ddV = | d(dV/dlat) / drho, d(dV/dlat) / dlat, d(dV/dlat) / dlon |
      //        | d(dV/dlon) / drho, d(dV/dlon) / dlat, d(dV.dlon) / dlon |
      Matrix<double> ddV(3,3,0.0);

      for(int n=0; n<=desiredDegree; ++n)
      {
         // (ae/rho)^n
         double ae_rho_n = std::pow(gmData.ae/rho, n);

         for(int m=0; m<=n; ++m)
         {
//            if(m > desiredOrder) break;

            // Pnm, dPnm, ddPnm
            double P0 = leg0(index(n,m)-1);
            double P1 = leg1(index(n,m)-1);
            double P2 = leg2(index(n,m)-1);
//            cout << setw(3) << n << " "
//                 << setw(3) << m << " "
//                 << setw(20) << P0 << " "
//                 << setw(20) << P1 << " "
//                 << setw(20) << P2 << endl;

            // Cnm, Snm
            double Cnm = gmData.normalizedCS(index(n,m)-1, 0);
            double Snm = gmData.normalizedCS(index(n,m)-1, 1);
//            cout << setw(20) << Cnm << " "
//                 << setw(20) << Snm << endl;

            // sin(m*lon) and cos(m*lon)
            double smlon = std::sin(m*lon);
            double cmlon = std::cos(m*lon);
//            cout << setw(20) << smlon << " "
//                 << setw(20) << cmlon << endl;

            // dV / d(rho, lat, lon)
            dV(0) += -gmData.GM/(rho*rho) * (n+1) * std::pow(gmData.ae/rho, n) * P0 * (Cnm*cmlon+Snm*smlon);
            dV(1) +=  gmData.GM/rho * std::pow(gmData.ae/rho, n) * P1 * (Cnm*cmlon+Snm*smlon);
            dV(2) +=  gmData.GM/rho * m * std::pow(gmData.ae/rho, n) * P0 * (-Cnm*smlon+Snm*cmlon);

            // d(dV) / d(rho, lat, lon)
            ddV(0,0) +=  gmData.GM/(rho*rho*rho) * (n+1)*(n+2) * ae_rho_n * P0 * (Cnm*cmlon+Snm*smlon);
            ddV(0,1) += -gmData.GM/(rho*rho) * (n+1) * ae_rho_n * P1 * (Cnm*cmlon+Snm*smlon);
            ddV(0,2) += -gmData.GM/(rho*rho) * (n+1)*m * ae_rho_n * P0 * (-Cnm*smlon+Snm*cmlon);
            ddV(1,1) +=  gmData.GM/rho * ae_rho_n * P2 * (Cnm*cmlon+Snm*smlon);
            ddV(1,2) +=  gmData.GM/rho * m * ae_rho_n * P1 * (-Cnm*smlon+Snm*cmlon);
            ddV(2,2) += -gmData.GM/rho * m*m * ae_rho_n * P0 * (Cnm*cmlon+Snm*smlon);

         }   // End of "for(int m=0; ...)"

      }   // End of "for(int n=2; ...)"

      ddV(1,0) = ddV(0,1);
      ddV(2,0) = ddV(0,2);
      ddV(2,1) = ddV(1,2);


      // gravitation acceleration in ECEF
      //
      //        | b11 b12 b13 |T   | dV/drho |
      // acce = | b21 b22 b23 |  * | dV/dlat |
      //        | b31 b32 b33 |    | dV/dlon |
      //
      Vector<double> acce = transpose(b) * dV;

      // gravitation acceleration in ECI
      a = T2C * acce;

      // partials of gravitation acceleration in ECEF to spherical coordinates (rho, lat, lon)
      int j;
      Matrix<double> da(3,3,0.0);
      // dax / d(rho, lat, lon)
      j = 0;
      da(0,0) = dbdrho(0,j)*dV(0) + dbdrho(1,j)*dV(1) + dbdrho(2,j)*dV(2)
              + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
      da(0,1) = dbdlat(0,j)*dV(0) + dbdlat(1,j)*dV(1) + dbdlat(2,j)*dV(2)
              + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
      da(0,2) = dbdlon(0,j)*dV(0) + dbdlon(1,j)*dV(1) + dbdlon(2,j)*dV(2)
              + b(0,j)*ddV(2,0) + b(1,j)*ddV(2,1) + b(2,j)*ddV(2,2);
      // day / d(rho, lat, lon)
      j = 1;
      da(1,0) = dbdrho(0,j)*dV(0) + dbdrho(1,j)*dV(1)+ dbdrho(2,j)*dV(2)
              + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
      da(1,1) = dbdlat(0,j)*dV(0) + dbdlat(1,j)*dV(1)+ dbdlat(2,j)*dV(2)
              + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
      da(1,2) = dbdlon(0,j)*dV(0) + dbdlon(1,j)*dV(1)+ dbdlon(2,j)*dV(2)
              + b(0,j)*ddV(2,0) + b(1,j)*ddV(2,1) + b(2,j)*ddV(2,2);
      // daz / d(rho, lat, lon)
      j = 2;
      da(2,0) = dbdrho(0,j)*dV(0) + dbdrho(1,j)*dV(1)+ dbdrho(2,j)*dV(2)
              + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
      da(2,1) = dbdlat(0,j)*dV(0) + dbdlat(1,j)*dV(1)+ dbdlat(2,j)*dV(2)
              + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
      da(2,2) = dbdlon(0,j)*dV(0) + dbdlon(1,j)*dV(1)+ dbdlat(2,j)*dV(2)
              + b(0,j)*ddV(2,0) + b(1,j)*ddV(2,1) + b(2,j)*ddV(2,2);

      // gravitation gradient in ECEF
      //
      //        | b11 b12 b13 |T   | dax / d(rho,lat,lon) |T
      // grad = | b21 b22 b23 |  * | day / d(rho,lat,lon) |
      //        | b31 b32 b33 |    | daz / d(rho,lat,lon) |
      //
      Matrix<double> grad = transpose(da * b);

      // gravitation gradient in ECI
      //
      //      grad_ECI = T2C * grad_ECEF * inverse(T2C)
      //      T2C * transpose(T2C) = I
      //  ==> inverse(T2C) = transpose(T2C) = C2T
      //  ==> grad_ECI = T2C * grad_ECEF * C2T
      //
      da_dr = T2C * grad * C2T;


      da_dv.resize(3,3,0.0);

      // Satellite Graviemtry
//      da_dp = ...

   }  // End of method "EGM08GravityModel::doCompute()"

}   // End of namespace 'gpstk'
