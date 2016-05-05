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
 * EGM08 gravity model
 */

#include "EGM08GravityModel.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{
    const double EGM08GravityModel::gmcs[88][2] = {
        // n = 2
        {   -0.484165143790815E-03,    0.000000000000000E+00},
        {   -0.206615509074176E-09,    0.138441389137979E-08},
        {    0.243938357328313E-05,   -0.140027370385934E-05},
        // n = 3
        {    0.957161207093473E-06,    0.000000000000000E+00},
        {    0.203046201047864E-05,    0.248200415856872E-06},
        {    0.904787894809528E-06,   -0.619005475177618E-06},
        {    0.721321757121568E-06,    0.141434926192941E-05},
        // n = 4 
        {    0.539965866638991E-06,    0.000000000000000E+00},
        {   -0.536157389388867E-06,   -0.473567346518086E-06},
        {    0.350501623962649E-06,    0.662480026275829E-06},
        {    0.990856766672321E-06,   -0.200956723567452E-06},
        {   -0.188519633023033E-06,    0.308803882149194E-06},
//        {    0.990856766672321E-06,   -0.200928369177000E-06},
//        {   -0.188560802735000E-06,    0.308853169333000E-06},
        // n = 5
        {    0.686702913736681E-07,    0.000000000000000E+00},
        {   -0.629211923042529E-07,   -0.943698073395769E-07},
        {    0.652078043176164E-06,   -0.323353192540522E-06},
        {   -0.451847152328843E-06,   -0.214955408306046E-06},
        {   -0.295328761175629E-06,    0.498070550102351E-07},
        {    0.174811795496002E-06,   -0.669379935180165E-06},
        // n = 6
        {   -0.149953927978527E-06,    0.000000000000000E+00},
        {   -0.759210081892527E-07,    0.265122593213647E-07},
        {    0.486488924604690E-07,   -0.373789324523752E-06},
        {    0.572451611175653E-07,    0.895201130010730E-08},
        {   -0.860237937191611E-07,   -0.471425573429095E-06},
        {   -0.267166423703038E-06,   -0.536493151500206E-06},
        {    0.947068749756882E-08,   -0.237382353351005E-06},
        // n = 7
        {    0.905120844521618E-07,    0.000000000000000E+00},
        {    0.280887555776673E-06,    0.951259362869275E-07},
        {    0.330407993702235E-06,    0.929969290624092E-07},
        {    0.250458409225729E-06,   -0.217118287729610E-06},
        {   -0.274993935591631E-06,   -0.124058403514343E-06},
        {    0.164773255934658E-08,    0.179281782751438E-07},
        {   -0.358798423464889E-06,    0.151798257443669E-06},
        {    0.150746472872675E-08,    0.241068767286303E-07},
        // n = 8
        {    0.494756003005199E-07,    0.000000000000000E+00},
        {    0.231607991248329E-07,    0.588974540927606E-07},
        {    0.800143604736599E-07,    0.652805043667369E-07},
        {   -0.193745381715290E-07,   -0.859639339125694E-07},
        {   -0.244360480007096E-06,    0.698072508472777E-07},
        {   -0.257011477267991E-07,    0.892034891745881E-07},
        {   -0.659648680031408E-07,    0.308946730783065E-06},
        {    0.672569751771483E-07,    0.748686063738231E-07},
        {   -0.124022771917136E-06,    0.120551889384997E-06},
        // n = 9
        {    0.280180753216300E-07,    0.000000000000000E+00},
        {    0.142151377236084E-06,    0.214004665077510E-07},
        {    0.214144381199757E-07,   -0.316984195352417E-07},
        {   -0.160612356882835E-06,   -0.742658786809216E-07},
        {   -0.936529556592536E-08,    0.199026740710063E-07},
        {   -0.163134050605937E-07,   -0.540394840426217E-07},
        {    0.627879491161446E-07,    0.222962377434615E-06},
        {   -0.117983924385618E-06,   -0.969222126840068E-07},
        {    0.188136188986452E-06,   -0.300538974811744E-08},
        {   -0.475568433357652E-07,    0.968804214389955E-07},
        // n = 10
        {    0.533304381729473E-07,    0.000000000000000E+00},
        {    0.837623112620412E-07,   -0.131092332261065E-06},
        {   -0.939894766092874E-07,   -0.512746772537482E-07},
        {   -0.700709997317429E-08,   -0.154139929404373E-06},
        {   -0.844715388074630E-07,   -0.790255527979406E-07},
        {   -0.492894049964295E-07,   -0.506137282060864E-07},
        {   -0.375849022022301E-07,   -0.797688616388143E-07},
        {    0.826209286523474E-08,   -0.304903703914366E-08},
        {    0.405981624580941E-07,   -0.917138622482163E-07},
        {    0.125376631604340E-06,   -0.379436584841270E-07},
        {    0.100435991936118E-06,   -0.238596204211893E-07},
        // n = 11
        {   -0.507683787085927E-07,    0.000000000000000E+00},
        {    0.156127678638183E-07,   -0.271235374123689E-07},
        {    0.201135250154855E-07,   -0.990003954905590E-07},
        {   -0.305773531606647E-07,   -0.148835345047152E-06},
        {   -0.379499015091407E-07,   -0.637669897493018E-07},
        {    0.374192407050580E-07,    0.495908160271967E-07},
        {   -0.156429128694775E-08,    0.342735099884706E-07},
        {    0.465461661449953E-08,   -0.898252194924903E-07},
        {   -0.630174049861897E-08,    0.245446551115189E-07},
        {   -0.310727993686101E-07,    0.420682585407293E-07},
        {   -0.522444922089646E-07,   -0.184216383163730E-07},
        {    0.462340571475799E-07,   -0.696711251523700E-07},
        // n = 12
        {    0.364361922614572E-07,    0.000000000000000E+00},
        {   -0.535856270449833E-07,   -0.431656037232084E-07},
        {    0.142665936828290E-07,    0.310937162901519E-07},
        {    0.396211271409354E-07,    0.250622628960907E-07},
        {   -0.677284618097416E-07,    0.383823469584472E-08},
        {    0.308775410911475E-07,    0.759066416791107E-08},
        {    0.313421100991039E-08,    0.389801868153392E-07},
        {   -0.190517957483100E-07,    0.357268620672699E-07},
        {   -0.258866871220994E-07,    0.169362538600173E-07},
        {    0.419147664170774E-07,    0.249625636010847E-07},
        {   -0.619955079880774E-08,    0.309398171578482E-07},
        {    0.113644952089825E-07,   -0.638551119140755E-08},
        {   -0.242377235648074E-08,   -0.110993698692881E-07}
    };

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
        double leapYears = (MJD_UTC - gmData.refMJD) / 365.25;

//        cout << "(2,0): " << index(2,0)-3-1 << endl;
//        cout << "(2,1): " << index(2,1)-3-1 << endl;
//        cout << "(2,2): " << index(2,2)-3-1 << endl;

        gmData.normalizedCS(index(2,0)-3-1, 0) += leapYears*gmData.dotC20;
        gmData.normalizedCS(index(3,0)-3-1, 0) += leapYears*gmData.dotC30;
        gmData.normalizedCS(index(4,0)-3-1, 1) += leapYears*gmData.dotC40;

//        gmData.normalizedCS(index(11,0)-3-1, 0) = 0.0;
//        gmData.normalizedCS(index(12,0)-3-1, 0) = 0.0;

        // get the mean pole at epoch 2000.0 from IERS Conventions 2010
        // see IERS Conventions 2010, Equation 7.25
        double xpm, ypm;

        if(MJD_UTC < 55197.0)   // until 2010.0
        {
            xpm = ( gmData.xp(0,0) + gmData.xp(1,0)*leapYears + gmData.xp(2,0)*leapYears*leapYears + gmData.xp(3,0)*leapYears*leapYears*leapYears )*1e-3;
            ypm = ( gmData.yp(0,0) + gmData.yp(1,0)*leapYears + gmData.yp(2,0)*leapYears*leapYears + gmData.yp(3,0)*leapYears*leapYears*leapYears )*1e-3;
        }
        else                // after 2010.0
        {
            xpm = ( gmData.xp(0,1) + gmData.xp(1,1)*leapYears )*1e-3;
            ypm = ( gmData.yp(0,1) + gmData.yp(1,1)*leapYears )*1e-3;
        }

        // convert pole position to radians
        xpm = xpm * AS_TO_RAD;
        ypm = ypm * AS_TO_RAD;

        // 
        // C21 = +sqrt(3)*xpm*C20 - xpm*C22 + ypm*S22
        // S21 = -sqrt(3)*ypm*C20 - ypm*C22 - xpm*S22
        //
        gmData.normalizedCS(index(2,1)-3-1, 0)
            = +std::sqrt(3.0)*xpm*gmData.normalizedCS(index(2,0)-3-1, 0)
              -xpm*gmData.normalizedCS(index(2,2)-3-1, 0)
              +ypm*gmData.normalizedCS(index(2,2)-3-1, 1);
        gmData.normalizedCS(index(2,1)-3-1, 1)
            = -std::sqrt(3.0)*ypm*gmData.normalizedCS(index(2,0)-3-1, 0)
              -ypm*gmData.normalizedCS(index(2,2)-3-1, 0)
              -xpm*gmData.normalizedCS(index(2,2)-3-1, 1);

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
        Matrix<double> dbdrho(3,3,0.0);     // db / drho
        dbdrho(1,0) =  slat_Sat * clon_Sat / (rho_Sat*rho_Sat);
        dbdrho(1,1) =  slat_Sat * slon_Sat / (rho_Sat*rho_Sat);
        dbdrho(1,2) = -clat_Sat / (rho_Sat*rho_Sat);
        dbdrho(2,0) =  slon_Sat / (rho_Sat*rho_Sat * clat_Sat);
        dbdrho(2,1) = -clon_Sat / (rho_Sat*rho_Sat * clat_Sat);

        Matrix<double> dbdlat(3,3,0.0);     // db / dlat
        dbdlat(0,0) = -slat_Sat * clon_Sat;
        dbdlat(0,1) = -slat_Sat * slon_Sat;
        dbdlat(0,2) =  clat_Sat;
        dbdlat(1,0) = -clat_Sat * clon_Sat / rho_Sat;
        dbdlat(1,1) = -clat_Sat * slon_Sat / rho_Sat;
        dbdlat(1,2) = -slat_Sat / rho_Sat;
        dbdlat(2,0) = -slat_Sat * slon_Sat / (rho_Sat * clat_Sat*clat_Sat);
        dbdlat(2,1) =  slat_Sat * clon_Sat / (rho_Sat * clat_Sat*clat_Sat);

        Matrix<double> dbdlon(3,3,0.0);     // db / dlon
        dbdlon(0,0) = -clat_Sat * slon_Sat;
        dbdlon(0,1) =  clat_Sat * clon_Sat;
        dbdlon(1,0) =  slat_Sat * slon_Sat / rho_Sat;
        dbdlon(1,1) = -slat_Sat * clon_Sat / rho_Sat;
        dbdlon(2,0) = -clon_Sat / (rho_Sat * clat_Sat);
        dbdlon(2,1) = -slon_Sat / (rho_Sat * clat_Sat);

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

        // gravitation acceleration in ECI
        a = T2C * acce;

//        cout << "f_earth (EGM08): " << endl;
//        cout << setprecision(15) << a << endl;


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

//        cout << "df_earth/dr (EGM08): " << endl;
//        cout << setprecision(15) << da_dr << endl;

        da_dv.resize(3,3,0.0);

//        da_dp = ...


    }  // End of method "EGM08GravityModel::doCompute()"


}   // End of namespace 'gpstk'
