#pragma ident "$Id$"

/**
* @file EOPDataStor2.cpp
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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================


#include "EOPDataStore2.hpp"
#include "MiscMath.hpp"
#include <fstream>
#include "Epoch.hpp"
#include "Exception.hpp"
#include "GNSSconstants.hpp"


using namespace std;


namespace gpstk
{

    // Provide the diurnal/subdiurnal tidal effects on polar motion ("), UT1 (s)
    // and LOD (s), in time domain. IERS Conventions 2010.
    void EOPDataStore2::PMUT1_OCEANS(const CommonTime& utc, double cor[4])
    {
        static const struct {
            int nchi;            // coefficients of GMST + PI
            int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
            double s1, c1;       // sine and cosine of xp
            double s2, c2;       // sine and cosine of yp
            double s3, c3;       // sine and cosine of ut1
        } x[] = {
            { 1,-1, 0,-2,-2,-2,  -0.05,   0.94,  -0.94,  -0.05,  0.396, -0.078 },
            { 1,-2, 0,-2, 0,-1,   0.06,   0.64,  -0.64,   0.06,  0.195, -0.059 },
            { 1,-2, 0,-2, 0,-2,   0.30,   3.42,  -3.42,   0.30,  1.034, -0.314 },
            { 1, 0, 0,-2,-2,-1,   0.08,   0.78,  -0.78,   0.08,  0.224, -0.073 },
            { 1, 0, 0,-2,-2,-2,   0.46,   4.15,  -4.15,   0.45,  1.187, -0.387 },
            { 1,-1, 0,-2, 0,-1,   1.19,   4.96,  -4.96,   1.19,  0.966, -0.474 },
            { 1,-1, 0,-2, 0,-2,   6.24,  26.31, -26.31,   6.23,  5.118, -2.499 },
            { 1, 1, 0,-2,-2,-1,   0.24,   0.94,  -0.94,   0.24,  0.172, -0.090 },
            { 1, 1, 0,-2,-2,-2,   1.28,   4.99,  -4.99,   1.28,  0.911, -0.475 },
            { 1, 0, 0,-2, 0, 0,  -0.28,  -0.77,   0.77,  -0.28, -0.093,  0.070 },
            { 1, 0, 0,-2, 0,-1,   9.22,  25.06, -25.06,   9.22,  3.025, -2.280 },
            { 1, 0, 0,-2, 0,-2,  48.82, 132.91,-132.90,  48.82, 16.020,-12.069 },
            { 1,-2, 0, 0, 0, 0,  -0.32,  -0.86,   0.86,  -0.32, -0.103,  0.078 },
            { 1, 0, 0, 0,-2, 0,  -0.66,  -1.72,   1.72,  -0.66, -0.194,  0.154 },
            { 1,-1, 0,-2, 2,-2,  -0.42,  -0.92,   0.92,  -0.42, -0.083,  0.074 },
            { 1, 1, 0,-2, 0,-1,  -0.30,  -0.64,   0.64,  -0.30, -0.057,  0.050 },
            { 1, 1, 0,-2, 0,-2,  -1.61,  -3.46,   3.46,  -1.61, -0.308,  0.271 },
            { 1,-1, 0, 0, 0, 0,  -4.48,  -9.61,   9.61,  -4.48, -0.856,  0.751 },
            { 1,-1, 0, 0, 0,-1,  -0.90,  -1.93,   1.93,  -0.90, -0.172,  0.151 },
            { 1, 1, 0, 0,-2, 0,  -0.86,  -1.81,   1.81,  -0.86, -0.161,  0.137 },
            { 1, 0,-1,-2, 2,-2,   1.54,   3.03,  -3.03,   1.54,  0.315, -0.189 },
            { 1, 0, 0,-2, 2,-1,  -0.29,  -0.58,   0.58,  -0.29, -0.062,  0.035 },
            { 1, 0, 0,-2, 2,-2,  26.13,  51.25, -51.25,  26.13,  5.512, -3.095 },
            { 1, 0, 1,-2, 2,-2,  -0.22,  -0.42,   0.42,  -0.22, -0.047,  0.025 },
            { 1, 0,-1, 0, 0, 0,  -0.61,  -1.20,   1.20,  -0.61, -0.134,  0.070 },
            { 1, 0, 0, 0, 0, 1,   1.54,   3.00,  -3.00,   1.54,  0.348, -0.171 },
            { 1, 0, 0, 0, 0, 0, -77.48,-151.74, 151.74, -77.48,-17.620,  8.548 },
            { 1, 0, 0, 0, 0,-1, -10.52, -20.56,  20.56, -10.52, -2.392,  1.159 },
            { 1, 0, 0, 0, 0,-2,   0.23,   0.44,  -0.44,   0.23,  0.052, -0.025 },
            { 1, 0, 1, 0, 0, 0,  -0.61,  -1.19,   1.19,  -0.61, -0.144,  0.065 },
            { 1, 0, 0, 2,-2, 2,  -1.09,  -2.11,   2.11,  -1.09, -0.267,  0.111 },
            { 1,-1, 0, 0, 2, 0,  -0.69,  -1.43,   1.43,  -0.69, -0.288,  0.043 },
            { 1, 1, 0, 0, 0, 0,  -3.46,  -7.28,   7.28,  -3.46, -1.610,  0.187 },
            { 1, 1, 0, 0, 0,-1,  -0.69,  -1.44,   1.44,  -0.69, -0.320,  0.037 },
            { 1, 0, 0, 0, 2, 0,  -0.37,  -1.06,   1.06,  -0.37, -0.407, -0.005 },
            { 1, 2, 0, 0, 0, 0,  -0.17,  -0.51,   0.51,  -0.17, -0.213, -0.005 },
            { 1, 0, 0, 2, 0, 2,  -1.10,  -3.42,   3.42,  -1.09, -1.436, -0.037 },
            { 1, 0, 0, 2, 0, 1,  -0.70,  -2.19,   2.19,  -0.70, -0.921, -0.023 },
            { 1, 0, 0, 2, 0, 0,  -0.15,  -0.46,   0.46,  -0.15, -0.193, -0.005 },
            { 1, 1, 0, 2, 0, 2,  -0.03,  -0.59,   0.59,  -0.03, -0.396, -0.024 },
            { 1, 1, 0, 2, 0, 1,  -0.02,  -0.38,   0.38,  -0.02, -0.253, -0.015 },
            { 2,-3, 0,-2, 0,-2,  -0.49,  -0.04,   0.63,   0.24, -0.089, -0.011 },
            { 2,-1, 0,-2,-2,-2,  -1.33,  -0.17,   1.53,   0.68, -0.224, -0.032 },
            { 2,-2, 0,-2, 0,-2,  -6.08,  -1.61,   3.13,   3.35, -0.637, -0.177 },
            { 2, 0, 0,-2,-2,-2,  -7.59,  -2.05,   3.44,   4.23, -0.745, -0.222 },
            { 2, 0, 1,-2,-2,-2,  -0.52,  -0.14,   0.22,   0.29, -0.049, -0.015 },
            { 2,-1,-1,-2, 0,-2,   0.47,   0.11,  -0.10,  -0.27,  0.033,  0.013 },
            { 2,-1, 0,-2, 0,-1,   2.12,   0.49,  -0.41,  -1.23,  0.141,  0.058 },
            { 2,-1, 0,-2, 0,-2, -56.87, -12.93,  11.15,  32.88, -3.795, -1.556 },
            { 2,-1, 1,-2, 0,-2,  -0.54,  -0.12,   0.10,   0.31, -0.035, -0.015 },
            { 2, 1, 0,-2,-2,-2, -11.01,  -2.40,   1.89,   6.41, -0.698, -0.298 },
            { 2, 1, 1,-2,-2,-2,  -0.51,  -0.11,   0.08,   0.30, -0.032, -0.014 },
            { 2,-2, 0,-2, 2,-2,   0.98,   0.11,  -0.11,  -0.58,  0.050,  0.022 },
            { 2, 0,-1,-2, 0,-2,   1.13,   0.11,  -0.13,  -0.67,  0.056,  0.025 },
            { 2, 0, 0,-2, 0,-1,  12.32,   1.00,  -1.41,  -7.31,  0.605,  0.266 },
            { 2, 0, 0,-2, 0,-2,-330.15, -26.96,  37.58, 195.92,-16.195, -7.140 },
            { 2, 0, 1,-2, 0,-2,  -1.01,  -0.07,   0.11,   0.60, -0.049, -0.021 },
            { 2,-1, 0,-2, 2,-2,   2.47,  -0.28,  -0.44,  -1.48,  0.111,  0.034 },
            { 2, 1, 0,-2, 0,-2,   9.40,  -1.44,  -1.88,  -5.65,  0.425,  0.117 },
            { 2,-1, 0, 0, 0, 0,  -2.35,   0.37,   0.47,   1.41, -0.106, -0.029 },
            { 2,-1, 0, 0, 0,-1,  -1.04,   0.17,   0.21,   0.62, -0.047, -0.013 },
            { 2, 0,-1,-2, 2,-2,  -8.51,   3.50,   3.29,   5.11, -0.437, -0.019 },
            { 2, 0, 0,-2, 2,-2,-144.13,  63.56,  59.23,  86.56, -7.547, -0.159 },
            { 2, 0, 1,-2, 2,-2,   1.19,  -0.56,  -0.52,  -0.72,  0.064,  0.000 },
            { 2, 0, 0, 0, 0, 1,   0.49,  -0.25,  -0.23,  -0.29,  0.027, -0.001 },
            { 2, 0, 0, 0, 0, 0, -38.48,  19.14,  17.72,  23.11, -2.104,  0.041 },
            { 2, 0, 0, 0, 0,-1, -11.44,   5.75,   5.32,   6.87, -0.627,  0.015 },
            { 2, 0, 0, 0, 0,-2,  -1.24,   0.63,   0.58,   0.75, -0.068,  0.002 },
            { 2, 1, 0, 0, 0, 0,  -1.77,   1.79,   1.71,   1.04, -0.146,  0.037 },
            { 2, 1, 0, 0, 0,-1,  -0.77,   0.78,   0.75,   0.45, -0.064,  0.017 },
            { 2, 0, 0, 2, 0, 2,  -0.33,   0.62,   0.65,   0.19, -0.049,  0.018 }
        };

        /* Number of terms in the series */
        const int NT = (int)(sizeof(x) / sizeof(x[0]));

        /* Interval between fundamental epoch J2000.0 and given date (JC). */
        double t = (MJD(utc).mjd - MJD_J2000) / JC;
        double t2 = t*t;
        double t3 = t2*t;
        double t4 = t3*t;

        double chi, l, lp, f, d, om;
        double dchi, dl, dlp, df, dd, dom;

        // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega

        chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
        chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

        dchi = (876600.0*3600 + 8640184.812866 + 2*0.093104*t - 3*6.2e-6*t2)*15.0;
        dchi = dchi * AS_TO_RAD / JC;    // rad/day

        l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
        l = std::fmod(l, TURNAS) * AS_TO_RAD;

        dl = -4*0.00024470*t3 + 3*0.051635*t2 + 2*31.8792*t + 1717915923.2178;
        dl = dl * AS_TO_RAD / JC;        // rad/day

        lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
        lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

        dlp = -4*0.00001149*t3 - 3*0.000136*t2 - 2*0.5532*t + 129596581.0481;
        dlp = dlp * AS_TO_RAD / JC;      // rad/day

        f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
        f = std::fmod(f, TURNAS) * AS_TO_RAD;

        df = 4*0.00000417*t3 - 3*0.001037*t2 - 2*12.7512*t + 1739527262.8478;
        df = df * AS_TO_RAD / JC;        // rad/day

        d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
        d = std::fmod(d, TURNAS) * AS_TO_RAD;

        dd = -4*0.00003169*t3 + 3*0.006593*t2 - 2*6.3706*t + 1602961601.2090;
        dd = dd * AS_TO_RAD / JC;        // rad/day

        om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
        om = std::fmod(om, TURNAS) * AS_TO_RAD;

        dom = -4*0.00005939*t3 + 3*0.007702*t2 + 2*7.4722*t - 6962890.2665;
        dom = dom * AS_TO_RAD / JC;      // rad/day

        // corrections
        double arg, darg;

        for(int i=0; i<NT; i++)
        {
            arg = 0.0; darg = 0.0;
            arg += x[i].nchi*chi
                 + x[i].nl*l
                 + x[i].nlp*lp
                 + x[i].nf*f
                 + x[i].nd*d
                 + x[i].nom*om;
            darg += x[i].nchi*dchi
                  + x[i].nl*dl
                  + x[i].nlp*dlp
                  + x[i].nf*df
                  + x[i].nd*dd
                  + x[i].nom*dom;

            arg = std::fmod(arg, TWO_PI);

            cor[0] += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
            cor[1] += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
            cor[2] += x[i].c3*std::cos(arg) + x[i].s3*std::sin(arg);
            cor[3] -= (-x[i].c3*std::sin(arg) + x[i].s3*std::cos(arg))*darg;
        }

        cor[0] *= 1e-6;     // cor_xp, arcsecond (")
        cor[1] *= 1e-6;     // cor_yp, arcsecond (")
        cor[2] *= 1e-6;     // cor_dut1, second (s)
        cor[3] *= 1e-6;     // cor_lod, second (s)

    }  // End of method 'EOPDataStore2::PMUT1_OCEANS()'


    /// Provide the diurnal lunisolar effect on polar motion ("), in time domain.
    /// IERS Conventions 2010.
    void EOPDataStore2::PMSDNUT2(const CommonTime& utc, double cor[2])
    {
        static const struct {
            int nchi;            // coefficients of GMST+PI
            int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
            double s1, c1;       // sine and cosine of xp
            double s2, c2;       // sine and cosine of yp
        } x[] = {
            { 1,-1, 0,-2, 0,-1,    -.44,   .25,   -.25,  -.44 },
            { 1,-1, 0,-2, 0,-2,   -2.31,  1.32,  -1.32, -2.31 },
            { 1, 1, 0,-2,-2,-2,    -.44,   .25,   -.25,  -.44 },
            { 1, 0, 0,-2, 0,-1,   -2.14,  1.23,  -1.23, -2.14 },
            { 1, 0, 0,-2, 0,-2,  -11.36,  6.52,  -6.52,-11.36 },
            { 1,-1, 0, 0, 0, 0,     .84,  -.48,    .48,   .84 },
            { 1, 0, 0,-2, 2,-2,   -4.76,  2.73,  -2.73, -4.76 },
            { 1, 0, 0, 0, 0, 0,   14.27, -8.19,   8.19, 14.27 },
            { 1, 0, 0, 0, 0,-1,    1.93, -1.11,   1.11,  1.93 },
            { 1, 1, 0, 0, 0, 0,     .76,  -.43,    .43,   .76 }
        };

        /* Number of terms in the series */
        const int NT = (int)(sizeof(x) / sizeof(x[0]));

        /* Interval between fundamental epoch J2000.0 and given date (JC). */
        double t = (MJD(utc).mjd - MJD_J2000)/ JC;
        double t2 = t*t;
        double t3 = t2*t;
        double t4 = t3*t;

        double chi, l, lp, f, d, om;
        // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega

        chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
        chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

        l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
        l = std::fmod(l, TURNAS) * AS_TO_RAD;

        lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
        lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

        f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
        f = std::fmod(f, TURNAS) * AS_TO_RAD;

        d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
        d = std::fmod(d, TURNAS) * AS_TO_RAD;

        om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
        om = std::fmod(om, TURNAS) * AS_TO_RAD;

        // corrections
        double arg;

        for(int i=0; i<NT; i++)
        {
            arg = 0.0;
            arg += x[i].nchi * chi
                 + x[i].nl * l
                 + x[i].nlp * lp
                 + x[i].nf * f
                 + x[i].nd * d
                 + x[i].nom * om;

            arg = std::fmod(arg, TWO_PI);

            cor[0] += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
            cor[1] += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
        }

        cor[0] *= 1e-6;     // cor_xp, arcsecond (")
        cor[1] *= 1e-6;     // cor_yp, arcsecond (")


    }  // End of method 'EOPDataStore2::PMSDNUT2()'


    /// Evaluate the model of subdiurnal libration in the axial component of
    /// rotation, expressed by UT1 and LOD.
    void EOPDataStore2::UTLIBR(const CommonTime& utc, double cor[2])
    {
        static const struct {
            int nchi;            // coefficients of GMST+PI
            int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
            double period;       // period of the tide
            double s1, c1;       // sine and cosine of UT1
            double s2, c2;       // sine and cosine of LOD
        } x[] = {
            { 2, -2,  0, -2,  0, -2, 0.5377239,  0.05, -0.03,  -0.3,  -0.6 },
            { 2,  0,  0, -2, -2, -2, 0.5363232,  0.06, -0.03,  -0.4,  -0.7 },
            { 2, -1,  0, -2,  0, -2, 0.5274312,  0.35, -0.20,  -2.4,  -4.1 },
            { 2,  1,  0, -2, -2, -2, 0.5260835,  0.07, -0.04,  -0.5,  -0.8 },
            { 2,  0,  0, -2,  0, -1, 0.5175645, -0.07,  0.04,   0.5,   0.8 },
            { 2,  0,  0, -2,  0, -2, 0.5175251,  1.75, -1.01, -12.2, -21.3 },
            { 2,  1,  0, -2,  0, -2, 0.5079842, -0.05,  0.03,   0.3,   0.6 },
            { 2,  0, -1, -2,  2, -2, 0.5006854,  0.04, -0.03,  -0.3,  -0.6 },
            { 2,  0,  0, -2,  2, -2, 0.5000000,  0.76, -0.44,  -5.5,  -9.6 },
            { 2,  0,  0,  0,  0,  0, 0.4986348,  0.21, -0.12,  -1.5,  -2.6 },
            { 2,  0,  0,  0,  0, -1, 0.4985982,  0.06, -0.04,  -0.4,  -0.8 }
        };

        /* Number of terms in the series */
        const int NT = (int)(sizeof(x) / sizeof(x[0]));

        /* Interval between fundamental epoch J2000.0 and given date (JC). */
        double t = (MJD(utc).mjd - MJD_J2000)/ JC;
        double t2 = t*t;
        double t3 = t2*t;
        double t4 = t3*t;

        double gmst, chi, l, lp, f, d, om;

        // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega
        chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
        chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

        l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
        l = std::fmod(l, TURNAS) * AS_TO_RAD;

        lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
        lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

        f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
        f = std::fmod(f, TURNAS) * AS_TO_RAD;

        d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
        d = std::fmod(d, TURNAS) * AS_TO_RAD;

        om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
        om = std::fmod(om, TURNAS) * AS_TO_RAD;

        // corrections
        double arg;

        for(int i=0; i<NT; i++)
        {
            arg = 0.0;
            arg += x[i].nchi * chi
                 + x[i].nl * l
                 + x[i].nlp * lp
                 + x[i].nf * f
                 + x[i].nd * d
                 + x[i].nom * om;

            arg = std::fmod(arg, TWO_PI);

            cor[0] += x[i].s1 * std::sin(arg) + x[i].c1 * std::cos(arg);
            cor[1] += x[i].s2 * std::sin(arg) + x[i].c2 * std::cos(arg);
        }

        cor[0] *= 1e-6;     // cor_dut1, second (s)
        cor[1] *= 1e-6;     // cor_lod, second (s)

    }  // End of method 'EOPDataStore2::UTLIBR()'



    // Add to the store directly
    void EOPDataStore2::addEOPData(const CommonTime& utc,
                                   const EOPDataStore2::EOPData& data)
        throw(InvalidRequest)
    {
        if(!(utc.getTimeSystem()==TimeSystem::UTC)) throw Exception();

        allData[utc] = data;

        if((initialTime == CommonTime::END_OF_TIME) ||
           (finalTime == CommonTime::BEGINNING_OF_TIME))
        {
            initialTime = finalTime = utc;
        }
        else if(utc < initialTime)
        {
            initialTime = utc;
        }
        else if(utc > finalTime)
        {
            finalTime = utc;
        }

    }  // End of method 'EOPDataStore2::addEOPData()'



    // Get the data at the given epoch
    EOPDataStore2::EOPData EOPDataStore2::getEOPData(const CommonTime& utc) const
        throw(InvalidRequest)
    {
        if(!(utc.getTimeSystem()==TimeSystem::UTC)) throw Exception();

        if( (utc < initialTime) || (utc > finalTime) )
        {
            InvalidRequest ire(string("Time tag (")
                + utc.asString()
                + string("), the timespan of EOPData is not enough."));

            GPSTK_THROW(ire);
        }

        // first, try to get it from the original EOP data map
        EOPDataMap::const_iterator it = allData.find(utc);
        if(it != allData.end())
        {
            return it->second;
        }

        // second, try to get it from the interpolated EOP data map
        if(interpMethod == Lagrange9th)
        {
            // check the length of EOP data. since we adopt central interpolation
            // of Lagranth 9th, the data length in either side should be longer
            // than 5.
            const int half = 5;

            it = allData.lower_bound(utc);

//            cout << CivilTime(it->first) << endl;
//            cout << CivilTime(utc) << endl;

            EOPDataMap::const_iterator itb = it;
            EOPDataMap::const_iterator ite = it;

            for(int i=0; i<half; ++i)
            {
                itb--;
                if(itb == allData.begin())
                {
                    InvalidRequest ire(string("Time tag(")
                            + utc.asString()
                            + string("), the length of EOPData is not enough."));
                    GPSTK_THROW(ire);
                }
            }

            for(int i=0; i<half; ++i)
            {
                ite++;
                if(ite == allData.end())
                {
                    InvalidRequest ire(string("Time tag(")
                            + utc.asString()
                            + string("), the length of EOPData is not enough."));
                    GPSTK_THROW(ire);
                }
            }

            // extract times and datas from the EOP data map
            const int N = 6;

            vector<double> times;
            vector< vector<double> > datas(N);

//            cout << CivilTime(itb->first) << endl;
//            cout << CivilTime(ite->first) << endl;

            for(EOPDataMap::const_iterator itr=itb; itr!=ite; ++itr)
            {
                // time information
                CommonTime time = itr->first;
                times.push_back(itr->first - itb->first);

                // data information
                EOPData eop = itr->second;
                std::vector<double> data;
                data.push_back(eop.xp);     data.push_back(eop.yp);
                data.push_back(eop.UT1mUTC);
                data.push_back(eop.LOD);
                data.push_back(eop.dX);     data.push_back(eop.dY);

                for(int i=0; i<N; ++i)
                {
                    datas[i].push_back( data[i] );
                }
            }

//            cout << datas[2].size() << endl;

            // check the data continuity of UT1mUTC in datas
            // if leap second accurs, adjust UT1mUTC.
            double dut1[half*2];

            for(int i=0; i<half*2; ++i)
            {
                dut1[i] = (datas[2])[i];
            }

//            cout << dut1[0] << " " << dut1[half*2-1] << endl;

            bool leap(false);
            int index(0);

            for(int i=0; i<half*2-1; ++i)
            {
                double x1 = dut1[i];
                double x2 = dut1[i+1];
                double dx = std::abs(dut1[i+1] - dut1[i]);

                if(std::abs(dx-1.0) < 1e-2)
                {
                    leap = true;
                    index = i+1;
                    break;
                }
            }

//            cout << "Leap: " << leap << endl;

            if(leap)
            {
                for(int i=index; i<half*2; ++i)
                {
                    dut1[i] -= 1.0;
                }

                for(int i=0; i<half*2; ++i)
                {
                    (datas[2])[i] = dut1[i];
                }

            }


            // implete Lagrange interplation
            vector<double> target(N,0.0);

            double dt = utc - itb->first;

            double err;

            for(int i=0; i<N; ++i)
            {
                target[i] = LagrangeInterpolation(times, datas[i], dt, err);
            }

            // if leap second accurs, restore UT1mUTC.
            if(leap)
            {
                if(index<=half-1)
                {
                    target[2] += 1.0;
                }
            }

            EOPData ret;
            ret.xp = target[0];         ret.yp = target[1];
            ret.UT1mUTC = target[2];    ret.LOD = target[3];
            ret.dX = target[4];         ret.dY = target[5];
            ret.err_xp = 0.0;           ret.err_yp = 0.0;
            ret.err_UT1mUTC = 0.0;      ret.err_LOD = 0.0;
            ret.err_dX = 0.0;           ret.err_dY = 0.0;

            return ret;
        }

    }  // End of method 'EOPDataStore2::getEOPData()'



    // Add EOPs to the store via a flat IERS file.
    void EOPDataStore2::loadIERSFile(std::string iersFile)
        throw(FileMissingException)
    {
        ifstream inpf(iersFile.c_str());
        if(!inpf)
        {
            FileMissingException fme("Could not open IERS file " + iersFile);
            GPSTK_THROW(fme);
        }

        allData.clear();

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
//            if(line.size()>=134)
//            {
//                xp = StringUtils::asDouble(line.substr(135,9));        // arcsec
//                yp = StringUtils::asDouble(line.substr(145,9));        // arcsec
//                UT1mUTC = StringUtils::asDouble(line.substr(155,10));  // seconds
//                dX = StringUtils::asDouble(line.substr(169,6))*1e-3;   // milliarcsec->arcsec
//                dY = StringUtils::asDouble(line.substr(179,6))*1e-3;   // milliarcsec->arcsec
//            }

            CommonTime utc( (MJD(mjd,TimeSystem::UTC)).convertToCommonTime() );

            EOPDataStore2::EOPData data;
            data.xp = xp;               data.err_xp = err_xp;
            data.yp = yp;               data.err_yp = err_yp;
            data.UT1mUTC = UT1mUTC;     data.err_UT1mUTC = err_UT1mUTC;
            data.LOD = LOD;             data.err_LOD = err_LOD;
            data.dX = dX;               data.err_dX = err_dX;
            data.dY = dY;               data.err_dY = err_dY;

            addEOPData(MJD(utc),data);
        }

        inpf.close();

        if(!ok)
        {
            FileMissingException fme("IERS File " + iersFile
                                + " is corrupted or wrong format");
            GPSTK_THROW(fme);
        }
    }


    // Add EOPs to the store via a flat IGS file
    void EOPDataStore2::loadIGSFile(std::string igsFile)
        throw(FileMissingException)
    {
        ifstream inpf(igsFile.c_str());
        if(!inpf)
        {
            FileMissingException fme("Could not open IERS file " + igsFile);
            GPSTK_THROW(fme);
        }

        allData.clear();

        // first we skip the header section
        // skip the header
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

            xp *= 1e-6;        // convert to arcsec
            yp *= 1e-6;        // convert to arcsec
            UT1mUTC *= 1e-7;   // convert to seconds
            LOD *= 1e-7;       // convert to seconds/day

            CommonTime utc( (MJD(mjd,TimeSystem::UTC)).convertToCommonTime() );

            EOPDataStore2::EOPData data;
            data.xp = xp;               data.err_xp = err_xp;
            data.yp = yp;               data.err_yp = err_yp;
            data.UT1mUTC = UT1mUTC;     data.err_UT1mUTC = err_UT1mUTC;
            data.LOD = LOD;             data.err_LOD = err_LOD;
            data.dX = dX;               data.err_dX = err_dX;
            data.dY = dY;               data.err_dY = err_dY;

            addEOPData(utc,data);
        }

        inpf.close();

        if(!ok)
        {
            FileMissingException fme("IGS File " + igsFile
                                + " is corrupted or wrong format");
            GPSTK_THROW(fme);
        }
    }

    // Add EOPs to the store via a flat STK file.
    void EOPDataStore2::loadSTKFile(std::string stkFile)
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
//                double dPsi = StringUtils::asDouble(buf.substr(58,10));
//                double dEps = StringUtils::asDouble(buf.substr(68,10));
                double dX = StringUtils::asDouble(buf.substr(78,10));
                double err_dX = 0.0;
                double dY = StringUtils::asDouble(buf.substr(88,10));
                double err_dY = 0.0;
                int UTCmTAI = StringUtils::asDouble(buf.substr(98,4));

                CommonTime utc( (MJD(mjd,TimeSystem::UTC)).convertToCommonTime() );

                EOPDataStore2::EOPData data;
                data.xp = xp;               data.err_xp = err_xp;
                data.yp = yp;               data.err_yp = err_yp;
                data.UT1mUTC = UT1mUTC;     data.err_UT1mUTC = err_UT1mUTC;
                data.LOD = LOD;             data.err_LOD = err_LOD;
                data.dX = dX;               data.err_dX = err_dX;
                data.dY = dY;               data.err_dY = err_dY;

                addEOPData(utc,data);
            }

        }  // End of 'while'

        fstk.close();
    }

    ostream& operator<<(std::ostream& os, const EOPDataStore2::EOPData& d)
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

