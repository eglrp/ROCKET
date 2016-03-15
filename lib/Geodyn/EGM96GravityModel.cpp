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
 * @file EGM96GravityModel.cpp
 * EGM96 gravity model
 */

#include "EGM96GravityModel.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{
   const double EGM96GravityModel::gmcs[88][2] = {
       // n = 2
       // note: C20, C21 and S21 are changed to match IERS 2003
       {-4.841652090000E-04,  0.000000000000E+00},
       {-2.230000000000E-10,  14.48000000000E-10},
       { 0.243914352398E-05, -0.140016683654E-05},

       // n = 3
       { 0.957254173792E-06,  0.000000000000E+00},
       { 0.202998882184E-05,  0.248513158716E-06},
       { 0.904627768605E-06, -0.619025944205E-06},
       { 0.721072657057E-06,  0.141435626958E-05},

       // n = 4
       { 0.539873863789E-06,  0.000000000000E+00},
       {-0.536321616971E-06, -0.473440265853E-06},
       { 0.350694105785E-06,  0.662671572540E-06},
       { 0.990771803829E-06, -0.200928369177E-06},
       {-0.188560802735E-06,  0.308853169333E-06},

       // n = 5
       { 0.685323475630E-07,  0.000000000000E+00},
       {-0.621012128528E-07, -0.944226127525E-07},
       { 0.652438297612E-06, -0.323349612668E-06},
       {-0.451955406071E-06, -0.214847190624E-06},
       {-0.295301647654E-06,  0.496658876769E-07},
       { 0.174971983203E-06, -0.669384278219E-06},

       // n = 6
       {-0.149957994714E-06,  0.000000000000E+00},
       {-0.760879384947E-07,  0.262890545501E-07},
       { 0.481732442832E-07, -0.373728201347E-06},
       { 0.571730990516E-07,  0.902694517163E-08},
       {-0.862142660109E-07, -0.471408154267E-06},
       {-0.267133325490E-06, -0.536488432483E-06},
       { 0.967616121092E-08, -0.237192006935E-06},

       // n = 7
       { 0.909789371450E-07,  0.000000000000E+00},
       { 0.279872910488E-06,  0.954336911867E-07},
       { 0.329743816488E-06,  0.930667596042E-07},
       { 0.250398657706E-06, -0.217198608738E-06},
       {-0.275114355257E-06, -0.123800392323E-06},
       { 0.193765507243E-08,  0.177377719872E-07},
       {-0.358856860645E-06,  0.151789817739E-06},
       { 0.109185148045E-08,  0.244415707993E-07},

       // n = 8
       { 0.496711667324E-07,  0.000000000000E+00},
       { 0.233422047893E-07,  0.590060493411E-07},
       { 0.802978722615E-07,  0.654175425859E-07},
       {-0.191877757009E-07, -0.863454445021E-07},
       {-0.244600105471E-06,  0.700233016934E-07},
       {-0.255352403037E-07,  0.891462164788E-07},
       {-0.657361610961E-07,  0.309238461807E-06},
       { 0.672811580072E-07,  0.747440473633E-07},
       {-0.124092493016E-06,  0.120533165603E-06},

       // n = 9
       { 0.276714300853E-07,  0.000000000000E+00},
       { 0.143387502749E-06,  0.216834947618E-07},
       { 0.222288318564E-07, -0.322196647116E-07},
       {-0.160811502143E-06, -0.742287409462E-07},
       {-0.900179225336E-08,  0.194666779475E-07},
       {-0.166165092924E-07, -0.541113191483E-07},
       { 0.626941938248E-07,  0.222903525945E-06},
       {-0.118366323475E-06, -0.965152667886E-07},
       { 0.188436022794E-06, -0.308566220421E-08},
       {-0.477475386132E-07,  0.966412847714E-07},

       // n = 10
       { 0.526222488569E-07,  0.000000000000E+00},
       { 0.835115775652E-07, -0.131314331796E-06},
       {-0.942413882081E-07, -0.515791657390E-07},
       {-0.689895048176E-08, -0.153768828694E-06},
       {-0.840764549716E-07, -0.792806255331E-07},
       {-0.493395938185E-07, -0.505370221897E-07},
       {-0.375885236598E-07, -0.795667053872E-07},
       { 0.811460540925E-08, -0.336629641314E-08},
       { 0.404927981694E-07, -0.918705975922E-07},
       { 0.125491334939E-06, -0.376516222392E-07},
       { 0.100538634409E-06, -0.240148449520E-07},

       // n = 11
       {-0.509613707522E-07,  0.000000000000E+00},
       { 0.151687209933E-07, -0.268604146166E-07},
       { 0.186309749878E-07, -0.990693862047E-07},
       {-0.309871239854E-07, -0.148131804260E-06},
       {-0.389580205051E-07, -0.636666511980E-07},
       { 0.377848029452E-07,  0.494736238169E-07},
       {-0.118676592395E-08,  0.344769584593E-07},
       { 0.411565188074E-08, -0.898252808977E-07},
       {-0.598410841300E-08,  0.243989612237E-07},
       {-0.314231072723E-07,  0.417731829829E-07},
       {-0.521882681927E-07, -0.183364561788E-07},
       { 0.460344448746E-07, -0.696662308185E-07},

       // n = 12
       { 0.377252636558E-07,  0.000000000000E+00},
       {-0.540654977836E-07, -0.435675748979E-07},
       { 0.142979642253E-07,  0.320975937619E-07},
       { 0.393995876403E-07,  0.244264863505E-07},
       {-0.686908127934E-07,  0.415081109011E-08},
       { 0.309411128730E-07,  0.782536279033E-08},
       { 0.341523275208E-08,  0.391765484449E-07},
       {-0.186909958587E-07,  0.356131849382E-07},
       {-0.253769398865E-07,  0.169361024629E-07},
       { 0.422880630662E-07,  0.252692598301E-07},
       {-0.617619654902E-08,  0.308375794212E-07},
       { 0.112502994122E-07, -0.637946501558E-08},
       {-0.249532607390E-08, -0.111780601900E-07}
   };

    /** Compute the acceleration due to earth gravitation.
     * @param utc Time reference class
     * @param rb  Reference body class  
     * @param sc  Spacecraft parameters and state
     * @return the acceleration [m/s^2]
     *
     * Reference: IERS Conventions 2003, Chapter 6
     */
    void EGM96GravityModel::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
    {
        // compute time in years since J2000
        long double MJD_UTC = MJD(utc).mjd;
        double leapYears = (MJD_UTC - gmData.refMJD) / 365.25;

//        cout << setprecision(12);
//        cout << gmData.normalizedCS(index(2,0)-3-1, 0) << endl;
//        cout << gmData.normalizedCS(index(2,1)-3-1, 0) << endl;
//        cout << gmData.normalizedCS(index(2,1)-3-1, 1) << endl;
//        cout << gmData.normalizedCS(index(2,2)-3-1, 0) << endl;
//        cout << gmData.normalizedCS(index(2,2)-3-1, 1) << endl;

        gmData.normalizedCS(index(2,0)-3-1, 0) += leapYears*gmData.dotC20;
        gmData.normalizedCS(index(2,1)-3-1, 0) += leapYears*gmData.dotC21;
        gmData.normalizedCS(index(2,1)-3-1, 1) += leapYears*gmData.dotS21;

        // correct solid tide
        if(correctSolidTide)
        {
            double dC[10] = {0.0};
            double dS[10] = {0.0};
            solidTide.getSolidTide(MJD_UTC, dC, dS);
/*
            cout << "solid tide: " << endl;
            for(int i=0; i<10; i++)
            {
                cout << setw(18) << setprecision(12) << dC[i] << ' ' << dS[i] << endl;
            }
*/
            // Cnm
            gmData.normalizedCS(index(2,0)-3-1, 0) += dC[0];  // C20
            gmData.normalizedCS(index(2,1)-3-1, 0) += dC[1];  // C21
            gmData.normalizedCS(index(2,2)-3-1, 0) += dC[2];  // C22
            gmData.normalizedCS(index(3,0)-3-1, 0) += dC[3];  // C30
            gmData.normalizedCS(index(3,1)-3-1, 0) += dC[4];  // C31
            gmData.normalizedCS(index(3,2)-3-1, 0) += dC[5];  // C32
            gmData.normalizedCS(index(3,3)-3-1, 0) += dC[6];  // C33
            gmData.normalizedCS(index(4,0)-3-1, 0) += dC[7];  // C40
            gmData.normalizedCS(index(4,1)-3-1, 0) += dC[8];  // C41
            gmData.normalizedCS(index(4,2)-3-1, 0) += dC[9];  // C42

            // Snm
            gmData.normalizedCS(index(2,0)-3-1, 1) += dS[0];  // S20
            gmData.normalizedCS(index(2,1)-3-1, 1) += dS[1];  // S21
            gmData.normalizedCS(index(2,2)-3-1, 1) += dS[2];  // S22
            gmData.normalizedCS(index(3,0)-3-1, 1) += dS[3];  // S30
            gmData.normalizedCS(index(3,1)-3-1, 1) += dS[4];  // S31
            gmData.normalizedCS(index(3,2)-3-1, 1) += dS[5];  // S32
            gmData.normalizedCS(index(3,3)-3-1, 1) += dS[6];  // S33
            gmData.normalizedCS(index(4,0)-3-1, 1) += dS[7];  // S40
            gmData.normalizedCS(index(4,1)-3-1, 1) += dS[8];  // S41
            gmData.normalizedCS(index(4,2)-3-1, 1) += dS[9];  // S42

        }

        // correct ocean tide
        if(correctOceanTide)
        {
            double dC[12] = {0.0};
            double dS[12] = {0.0};
            oceanTide.getOceanTide(MJD_UTC, dC, dS);
/*
            cout << "ocean tide: " << endl;
            for(int i=0; i<12; i++)
            {
                cout << setw(18) << setprecision(12) << dC[i] << ' ' << dS[i] << endl;
            }
*/
            // Cnm
            gmData.normalizedCS(index(2,0)-3-1, 0) += dC[0];   // C20
            gmData.normalizedCS(index(2,1)-3-1, 0) += dC[1];   // C21
            gmData.normalizedCS(index(2,2)-3-1, 0) += dC[2];   // C22
            gmData.normalizedCS(index(3,0)-3-1, 0) += dC[3];   // C30
            gmData.normalizedCS(index(3,1)-3-1, 0) += dC[4];   // C31
            gmData.normalizedCS(index(3,2)-3-1, 0) += dC[5];   // C32
            gmData.normalizedCS(index(3,3)-3-1, 0) += dC[6];   // C33
            gmData.normalizedCS(index(4,0)-3-1, 0) += dC[7];   // C40
            gmData.normalizedCS(index(4,1)-3-1, 0) += dC[8];   // C41
            gmData.normalizedCS(index(4,2)-3-1, 0) += dC[9];   // C42
            gmData.normalizedCS(index(4,3)-3-1, 0) += dC[10];  // C43
            gmData.normalizedCS(index(4,4)-3-1, 0) += dC[11];  // C44
 
            // Snm
            gmData.normalizedCS(index(2,0)-3-1, 1) += dS[0];   // S20
            gmData.normalizedCS(index(2,1)-3-1, 1) += dS[1];   // S21
            gmData.normalizedCS(index(2,2)-3-1, 1) += dS[2];   // S22
            gmData.normalizedCS(index(3,0)-3-1, 1) += dS[3];   // S30
            gmData.normalizedCS(index(3,1)-3-1, 1) += dS[4];   // S31
            gmData.normalizedCS(index(3,2)-3-1, 1) += dS[5];   // S32
            gmData.normalizedCS(index(3,3)-3-1, 1) += dS[6];   // S33
            gmData.normalizedCS(index(4,0)-3-1, 1) += dS[7];   // S40
            gmData.normalizedCS(index(4,1)-3-1, 1) += dS[8];   // S41
            gmData.normalizedCS(index(4,2)-3-1, 1) += dS[9];   // S42
            gmData.normalizedCS(index(4,3)-3-1, 1) += dS[10];  // S43
            gmData.normalizedCS(index(4,4)-3-1, 1) += dS[11];  // S44

        }

        // correct pole tide
        if(correctPoleTide)
        {
            double dC21 = 0.0;
            double dS21 = 0.0;
            poleTide.getPoleTide(MJD_UTC, dC21, dS21);
/*
            cout << "pole tide: " << endl;
            cout << setw(18) << setprecision(12) << dC21 << ' ' << dS21 << endl;
*/
            gmData.normalizedCS(index(2,1)-3-1, 0) += dC21;   // C21
            gmData.normalizedCS(index(2,1)-3-1, 1) += dS21;   // S21
        }
        
//        cout << setw(18) << setprecision(12) << gmData.normalizedCS << endl;

        // satellite position in ECI
        Vector<double> r_Sat_ECI = sc.R();
        // transformation matrixs between ECI and ECEF
        Matrix<double> C2T = C2TMatrix(utc);
        Matrix<double> T2C = transpose(C2T);

        // satellite position in ECEF
        Vector<double> r_Sat_ECEF = C2T * r_Sat_ECI;

        // geocentric distance, latitude and longitude of satellite
        double rho_Sat = norm(r_Sat_ECEF);
        double lat_Sat = std::atan2( r_Sat_ECEF(2), std::sqrt(r_Sat_ECEF(0)*r_Sat_ECEF(0)+r_Sat_ECEF(1)*r_Sat_ECEF(1)) );
        double lon_Sat = std::atan2( r_Sat_ECEF(1), r_Sat_ECEF(0) );
        
        // sine and cosine of geocentric latitude and longitude of satellite
        double slat_Sat = std::sin(lat_Sat);
        double slon_Sat = std::sin(lon_Sat);
        double clat_Sat = std::cos(lat_Sat);
        double clon_Sat = std::cos(lon_Sat);

        // fully normalized associated legendre functions and its gradients
        Vector<double> leg, leg1, leg2;
        legendre(desiredDegree, lat_Sat, leg, leg1, leg2);

        // partials of (rho, lat, lon) to (x, y, z)
        Matrix<double> b(3,3,0.0);
        b(0,0) =  clat_Sat * clon_Sat;
        b(0,1) =  clat_Sat * slon_Sat;
        b(0,2) =  slat_Sat;
        b(1,0) = -slat_Sat * clon_Sat / rho_Sat;
        b(1,1) = -slat_Sat * slon_Sat / rho_Sat;
        b(1,2) =  clat_Sat / rho_Sat;
        b(2,0) = -slon_Sat / (rho_Sat * clat_Sat);
        b(2,1) =  clon_Sat / (rho_Sat * clat_Sat);
        b(2,2) =  0.0;

        // partials of b matrix to (rho, lat, lon)
        Matrix<double> db_drho(3,3,0.0);     // db / drho
        db_drho(1,0) =  slat_Sat * clon_Sat / (rho_Sat*rho_Sat);
        db_drho(1,1) =  slat_Sat * slon_Sat / (rho_Sat*rho_Sat);
        db_drho(1,2) = -clat_Sat / (rho_Sat*rho_Sat);
        db_drho(2,0) =  slon_Sat / (rho_Sat*rho_Sat * clat_Sat);
        db_drho(2,1) = -clon_Sat / (rho_Sat*rho_Sat * clat_Sat);

        Matrix<double> db_dlat(3,3,0.0);     // db / dlat
        db_dlat(0,0) = -slat_Sat * clon_Sat;
        db_dlat(0,1) = -slat_Sat * slon_Sat;
        db_dlat(0,2) =  clat_Sat;
        db_dlat(1,0) = -clat_Sat * clon_Sat / rho_Sat;
        db_dlat(1,1) = -clat_Sat * slon_Sat / rho_Sat;
        db_dlat(1,2) = -slat_Sat / rho_Sat;
        db_dlat(2,0) = -slat_Sat * slon_Sat / (rho_Sat * clat_Sat*clat_Sat);
        db_dlat(2,1) =  slat_Sat * clon_Sat / (rho_Sat * clat_Sat*clat_Sat);

        Matrix<double> db_dlon(3,3,0.0);     // db / dlon
        db_dlon(0,0) = -clat_Sat * slon_Sat;
        db_dlon(0,1) =  clat_Sat * clon_Sat;
        db_dlon(1,0) =  slat_Sat * slon_Sat / rho_Sat;
        db_dlon(1,1) = -slat_Sat * clon_Sat / rho_Sat;
        db_dlon(2,0) = -clon_Sat / (rho_Sat * clat_Sat);
        db_dlon(2,1) = -slon_Sat / (rho_Sat * clat_Sat);

        // partials of gravitation potential V to spherical coordinates (rho, lat, lon)
        //
        //       | dV / drho |
        //  dV = | dV / dlat |
        //       | dV / dlon |
        //
        Vector<double> dV(3,0.0);
        dV(0) = -gmData.GM/(rho_Sat*rho_Sat);

        //
        //        | d(dV/drho) / drho, d(dV/drho) / dlat, d(dV/drho) / dlon |
        //  ddV = | d(dV/dlat) / drho, d(dV/dlat) / dlat, d(dV/dlat) / dlon |
        //        | d(dV/dlon) / drho, d(dV/dlon) / dlat, d(dV.dlon) / dlon |
        //
        Matrix<double> ddV(3,3,0.0);
        ddV(0,0) = gmData.GM/(rho_Sat*rho_Sat*rho_Sat) * 2;

       for(int n=2; n<=desiredDegree; n++)
       {
            for(int m=0; m<=n; m++)
            {
                double Pnm = leg( index(n,m)-1 );
                double dPnm = leg1( index(n,m)-1 );
                double ddPnm = leg2( index(n,m)-1 );

                double Cnm = gmData.normalizedCS( index(n,m)-3-1, 0 );
                double Snm = gmData.normalizedCS( index(n,m)-3-1, 1 );

//                cout << Pnm << ' ' << dPnm << ' ' << ddPnm << endl;
//                cout << Cnm << ' ' << Snm << endl;

                // sin(m*lon) and cos(m*lon)
                double smlon = std::sin(m*lon_Sat);
                double cmlon = std::cos(m*lon_Sat);

                // dV / d(rho, lat, lon)
                dV(0) += -gmData.GM/(rho_Sat*rho_Sat) * (n+1) * std::pow(gmData.ae/rho_Sat,n) * Pnm * (Cnm*cmlon+Snm*smlon);
                dV(1) +=  gmData.GM/rho_Sat * std::pow(gmData.ae/rho_Sat,n) * dPnm * (Cnm*cmlon+Snm*smlon);
                dV(2) +=  gmData.GM/rho_Sat * m * std::pow(gmData.ae/rho_Sat,n) * Pnm * (-Cnm*smlon+Snm*cmlon);

//                cout << dV << endl;

                // d(dV) / d(rho, lat, lon)
                ddV(0,0) +=  gmData.GM/(rho_Sat*rho_Sat*rho_Sat) * (n+1)*(n+2) * std::pow(gmData.ae/rho_Sat,n) * Pnm * (Cnm*cmlon+Snm*smlon);
                ddV(0,1) += -gmData.GM/(rho_Sat*rho_Sat) * (n+1) * std::pow(gmData.ae/rho_Sat,n) * dPnm * (Cnm*cmlon+Snm*smlon);
                ddV(0,2) += -gmData.GM/(rho_Sat*rho_Sat) * (n+1)*m * std::pow(gmData.ae/rho_Sat,n) * Pnm * (-Cnm*smlon+Snm*cmlon);
                ddV(1,1) +=  gmData.GM/rho_Sat * std::pow(gmData.ae/rho_Sat,n) * ddPnm * (Cnm*cmlon+Snm*smlon);
                ddV(1,2) +=  gmData.GM/rho_Sat * m * std::pow(gmData.ae/rho_Sat,n) * dPnm * (-Cnm*smlon+Snm*cmlon);
                ddV(2,2) += -gmData.GM/rho_Sat * m*m * std::pow(gmData.ae/rho_Sat,n) * Pnm * (Cnm*cmlon+Snm*smlon);

//                cout << ddV << endl;
//                break;
            }
        }

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
//        Vector<double> acce(3,0.0);
//        acce(0) = b(0,0)*dV(0) + b(1,0)*dV(1) + b(2,0)*dV(2);
//        acce(1) = b(0,1)*dV(0) + b(1,1)*dV(1) + b(2,1)*dV(2);
//        acce(2) = b(0,2)*dV(0) + b(1,2)*dV(1) + b(2,2)*dV(2);

        // gravitation acceleration in ECI
        a = T2C * acce;

//        cout << "f_earth (EGM96): " << endl;
//        cout << setprecision(15) << a << endl;


        // partials of gravitation acceleration in ECEF to spherical coordinates (rho, lat, lon)
        int j;
        Matrix<double> da(3,3,0.0);
        // dax / d(rho, lat, lon)
        j = 0;
        da(0,0) = db_drho(0,j)*dV(0) + db_drho(1,j)*dV(1) + db_drho(2,j)*dV(2)
                + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
        da(0,1) = db_dlat(0,j)*dV(0) + db_dlat(1,j)*dV(1) + db_dlat(2,j)*dV(2)
                + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
        da(0,2) = db_dlon(0,j)*dV(0) + db_dlon(1,j)*dV(1) + db_dlon(2,j)*dV(2)
                + b(0,j)*ddV(2,0) + b(1,j)*ddV(2,1) + b(2,j)*ddV(2,2);
        // day / d(rho, lat, lon)
        j = 1;
        da(1,0) = db_drho(0,j)*dV(0) + db_drho(1,j)*dV(1)+ db_drho(2,j)*dV(2)
                + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
        da(1,1) = db_dlat(0,j)*dV(0) + db_dlat(1,j)*dV(1)+ db_dlat(2,j)*dV(2)
                + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
        da(1,2) = db_dlon(0,j)*dV(0) + db_dlon(1,j)*dV(1)+ db_dlon(2,j)*dV(2)
                + b(0,j)*ddV(2,0) + b(1,j)*ddV(2,1) + b(2,j)*ddV(2,2);
        // daz / d(rho, lat, lon)
        j = 2;
        da(2,0) = db_drho(0,j)*dV(0) + db_drho(1,j)*dV(1)+ db_drho(2,j)*dV(2)
                + b(0,j)*ddV(0,0) + b(1,j)*ddV(0,1) + b(2,j)*ddV(0,2);
        da(2,1) = db_dlat(0,j)*dV(0) + db_dlat(1,j)*dV(1)+ db_dlat(2,j)*dV(2)
                + b(0,j)*ddV(1,0) + b(1,j)*ddV(1,1) + b(2,j)*ddV(1,2);
        da(2,2) = db_dlon(0,j)*dV(0) + db_dlon(1,j)*dV(1)+ db_dlat(2,j)*dV(2)
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

//        cout << "df_earth/dr (EGM96): " << endl;
//        cout << setprecision(15) << da_dr << endl;

        da_dv.resize(3,3,0.0);

//        da_dp = ...


    }  // End of method "EGM96GravityModel::doCompute()"



}  // End of namespace 'gpstk'
