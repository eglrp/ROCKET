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
    // Evaluate the effects of zonal Earth tides on the rotation of the Earth.
    // IERS Conventions 2010
    Vector<double> EOPDataStore2::RG_ZONT2(const CommonTime& TT) const
    {
        static const struct {
            int nl,nlp,nf,nd,nom;    // Luni-Solar argument multipliers
            double s1, c1;           // sine and cosine of DUT
            double c2, s2;           // cosine and sine of DLOD
            double c3, s3;           // cosine and sine of DOMEGA
        } x[] = {

        // 0 - 20
        {  1,  0,  2,  2,  2,     -0.0235, 0.0000, 0.2617, 0.0000, -0.2209, 0.0000 },
        {  2,  0,  2,  0,  1,     -0.0404, 0.0000, 0.3706, 0.0000, -0.3128, 0.0000 },
        {  2,  0,  2,  0,  2,     -0.0987, 0.0000, 0.9041, 0.0000, -0.7630, 0.0000 },
        {  0,  0,  2,  2,  1,     -0.0508, 0.0000, 0.4499, 0.0000, -0.3797, 0.0000 },
        {  0,  0,  2,  2,  2,     -0.1231, 0.0000, 1.0904, 0.0000, -0.9203, 0.0000 },
        {  1,  0,  2,  0,  0,     -0.0385, 0.0000, 0.2659, 0.0000, -0.2244, 0.0000 },
        {  1,  0,  2,  0,  1,     -0.4108, 0.0000, 2.8298, 0.0000, -2.3884, 0.0000 },
        {  1,  0,  2,  0,  2,     -0.9926, 0.0000, 6.8291, 0.0000, -5.7637, 0.0000 },
        {  3,  0,  0,  0,  0,     -0.0179, 0.0000, 0.1222, 0.0000, -0.1031, 0.0000 },
        { -1,  0,  2,  2,  1,     -0.0818, 0.0000, 0.5384, 0.0000, -0.4544, 0.0000 },
        { -1,  0,  2,  2,  2,     -0.1974, 0.0000, 1.2978, 0.0000, -1.0953, 0.0000 },
        {  1,  0,  0,  2,  0,     -0.0761, 0.0000, 0.4976, 0.0000, -0.4200, 0.0000 },
        {  2,  0,  2, -2,  2,      0.0216, 0.0000,-0.1060, 0.0000,  0.0895, 0.0000 },
        {  0,  1,  2,  0,  2,      0.0254, 0.0000,-0.1211, 0.0000,  0.1022, 0.0000 },
        {  0,  0,  2,  0,  0,     -0.2989, 0.0000, 1.3804, 0.0000, -1.1650, 0.0000 },
        {  0,  0,  2,  0,  1,     -3.1873, 0.2010,14.6890, 0.9266,-12.3974,-0.7820 },
        {  0,  0,  2,  0,  2,     -7.8468, 0.5320,36.0910, 2.4469,-30.4606,-2.0652 },
        {  2,  0,  0,  0, -1,      0.0216, 0.0000,-0.0988, 0.0000,  0.0834, 0.0000 },
        {  2,  0,  0,  0,  0,     -0.3384, 0.0000, 1.5433, 0.0000, -1.3025, 0.0000 },
        {  2,  0,  0,  0,  1,      0.0179, 0.0000,-0.0813, 0.0000,  0.0686, 0.0000 },

        // 21 - 40
        {  0, -1,  2,  0,  2,     -0.0244, 0.0000,  0.1082, 0.0000, -0.0913, 0.0000 },
        {  0,  0,  0,  2, -1,      0.0470, 0.0000, -0.2004, 0.0000,  0.1692, 0.0000 },
        {  0,  0,  0,  2,  0,     -0.7341, 0.0000,  3.1240, 0.0000, -2.6367, 0.0000 },
        {  0,  0,  0,  2,  1,     -0.0526, 0.0000,  0.2235, 0.0000, -0.1886, 0.0000 },
        {  0, -1,  0,  2,  0,     -0.0508, 0.0000,  0.2073, 0.0000, -0.1749, 0.0000 },
        {  1,  0,  2, -2,  1,      0.0498, 0.0000, -0.1312, 0.0000,  0.1107, 0.0000 },
        {  1,  0,  2, -2,  2,      0.1006, 0.0000, -0.2640, 0.0000,  0.2228, 0.0000 },
        {  1,  1,  0,  0,  0,      0.0395, 0.0000, -0.0968, 0.0000,  0.0817, 0.0000 },
        { -1,  0,  2,  0,  0,      0.0470, 0.0000, -0.1099, 0.0000,  0.0927, 0.0000 },
        { -1,  0,  2,  0,  1,      0.1767, 0.0000, -0.4115, 0.0000,  0.3473, 0.0000 },
        { -1,  0,  2,  0,  2,      0.4352, 0.0000, -1.0093, 0.0000,  0.8519, 0.0000 },
        {  1,  0,  0,  0, -1,      0.5339, 0.0000, -1.2224, 0.0000,  1.0317, 0.0000 },
        {  1,  0,  0,  0,  0,     -8.4046, 0.2500, 19.1647, 0.5701,-16.1749,-0.4811 },
        {  1,  0,  0,  0,  1,      0.5443, 0.0000, -1.2360, 0.0000,  1.0432, 0.0000 },
        {  0,  0,  0,  1,  0,      0.0470, 0.0000, -0.1000, 0.0000,  0.0844, 0.0000 },
        {  1, -1,  0,  0,  0,     -0.0555, 0.0000,  0.1169, 0.0000, -0.0987, 0.0000 },
        { -1,  0,  0,  2, -1,      0.1175, 0.0000, -0.2332, 0.0000,  0.1968, 0.0000 },
        { -1,  0,  0,  2,  0,     -1.8236, 0.0000,  3.6018, 0.0000, -3.0399, 0.0000 },
        { -1,  0,  0,  2,  1,      0.1316, 0.0000, -0.2587, 0.0000,  0.2183, 0.0000 },
        {  1,  0, -2,  2, -1,      0.0179, 0.0000, -0.0344, 0.0000,  0.0290, 0.0000 },

        // 41 - 60
        { -1, -1,  0,  2,  0,     -0.0855, 0.0000,  0.1542, 0.0000, -0.1302, 0.0000 },

        {  0,  2,  2, -2,  2,     -0.0573, 0.0000,  0.0395, 0.0000, -0.0333, 0.0000 },
        {  0,  1,  2, -2,  1,      0.0329, 0.0000, -0.0173, 0.0000,  0.0146, 0.0000 },
        {  0,  1,  2, -2,  2,     -1.8847, 0.0000,  0.9726, 0.0000, -0.8209, 0.0000 },
        {  0,  0,  2, -2,  0,      0.2510, 0.0000, -0.0910, 0.0000,  0.0768, 0.0000 },
        {  0,  0,  2, -2,  1,      1.1703, 0.0000, -0.4135, 0.0000,  0.3490, 0.0000 },
        {  0,  0,  2, -2,  2,    -49.7174, 0.4330, 17.1056, 0.1490,-14.4370,-0.1257 },
        {  0,  2,  0,  0,  0,     -0.1936, 0.0000,  0.0666, 0.0000, -0.0562, 0.0000 },
        {  2,  0,  0, -2, -1,      0.0489, 0.0000, -0.0154, 0.0000,  0.0130, 0.0000 },
        {  2,  0,  0, -2,  0,     -0.5471, 0.0000,  0.1670, 0.0000, -0.1409, 0.0000 },
        {  2,  0,  0, -2,  1,      0.0367, 0.0000, -0.0108, 0.0000,  0.0092, 0.0000 },
        {  0, -1,  2, -2,  1,     -0.0451, 0.0000,  0.0082, 0.0000, -0.0069, 0.0000 },
        {  0,  1,  0,  0, -1,      0.0921, 0.0000, -0.0167, 0.0000,  0.0141, 0.0000 },
        {  0, -1,  2, -2,  2,      0.8281, 0.0000, -0.1425, 0.0000,  0.1202, 0.0000 },
        {  0,  1,  0,  0,  0,    -15.8887, 0.1530,  2.7332, 0.0267, -2.3068,-0.0222 },
        {  0,  1,  0,  0,  1,     -0.1382, 0.0000,  0.0225, 0.0000, -0.0190, 0.0000 },
        {  1,  0,  0, -1,  0,      0.0348, 0.0000, -0.0053, 0.0000,  0.0045, 0.0000 },
        {  2,  0, -2,  0,  0,     -0.1372, 0.0000, -0.0079, 0.0000,  0.0066, 0.0000 },
        { -2,  0,  2,  0,  1,      0.4211, 0.0000, -0.0203, 0.0000,  0.0171, 0.0000 },
        { -1,  1,  0,  1,  0      -0.0404, 0.0000,  0.0008, 0.0000, -0.0007, 0.0000 },

        // 61 - 62
        {  0,  0,  0,  0,  2,      7.8998, 0.0000,  0.1460, 0.0000, -0.1232, 0.0000 },
        {  0,  0,  0,  0,  1,  -1617.2681, 0.0000,-14.9471, 0.0000, 12.6153, 0.0000 }

        };

        /* Number of terms in the series */
        const int NT = (int) (sizeof(x) / sizeof(x[0]));

        double el, elp, f, d, om;

        /* Interval between fundamental epoch J2000.0 and given date (JC). */
        double t = (MJD(TT).mjd - MJD_J2000) / JC;


        /* --------------------- */
        /* Fundamental arguments */
        /* --------------------- */

        // Compute the fundamental argument l.
        el = std::fmod (       485868.249036 +
                        t*( 1717915923.2178 +
                        t*(         31.8792 +
                        t*(          0.051635 +
                        t*(        - 0.00024470 )))), TURNAS ) * AS_TO_RAD;

        // Compute the fundamental argument l'.
        elp = std::fmod (       1287104.79305 +
                        t*( 129596581.0481 +
                        t*(       - 0.5532 +
                        t*(         0.000136 +
                        t*(       - 0.00001149 )))), TURNAS ) * AS_TO_RAD;

        // Compute the fundamental argument F.
        f  = std::fmod (       335779.526232 +
                        t*( 1739527262.8478 +
                        t*(       - 12.7512 +
                        t*(       -  0.001037 +
                        t*(          0.00000417 )))), TURNAS ) * AS_TO_RAD;

        // Compute the fundamental argument D.
        d = std::fmod (        1072260.70369 +
                        t*( 1602961601.2090 +
                        t*(        - 6.3706 +
                        t*(          0.006593 +
                        t*(        - 0.00003169 )))), TURNAS ) * AS_TO_RAD;

        // Compute the fundamental argument OM.
        om = std::fmod (       450160.398036 +
                        t*( - 6962890.5431 +
                        t*(         7.4722 +
                        t*(         0.007702 +
                        t*(       - 0.00005939 )))), TURNAS ) * AS_TO_RAD;

        // corrections
        Vector<double> ret(3,0.0);

        double arg;

        for(int i=0; i<NT; i++)
        {
            arg = 0.0;
            // Formation of multiples of arguments.
            arg = x[i].nl*el
                + x[i].nlp*elp
                + x[i].nf*f
                + x[i].nd*d
                + x[i].nom*om;

            arg = std::fmod(arg, TWO_PI);
            if(arg < 0.0) arg += TWO_PI;

            // Evaluate zonal tidal terms.
            ret[0] += x[i].s1*std::sin(arg) + x[i].c1*std::cos(arg);
            ret[1] += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
            ret[2] += x[i].c3*std::cos(arg) + x[i].s3*std::sin(arg);
        }

        // Rescale corrections so that they are in units involving seconds.
        ret[0] *= 1.0e-4;
        ret[1] *= 1.0e-5;
        ret[2] *= 1.0e-14;

        return ret;

    }    // End of method 'ReferenceSystem::RG_ZONT2()'


    // Provide the diurnal/subdiurnal tidal effects on polar motion ("), UT1 (s)
    // and LOD (s), in time domain. IERS Conventions 2010.
    Vector<double> EOPDataStore2::PMUT_OCEAN(const CommonTime& UTC) const
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
        double t = (MJD(UTC).mjd - MJD_J2000) / JC;
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
        Vector<double> ret(4,0.0);

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

            ret[0] += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
            ret[1] += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
            ret[2] += x[i].c3*std::cos(arg) + x[i].s3*std::sin(arg);
            ret[3] -= (-x[i].c3*std::sin(arg) + x[i].s3*std::cos(arg))*darg;
        }

        ret[0] *= 1e-6;     // cor_xp, arcsecond (")
        ret[1] *= 1e-6;     // cor_yp, arcsecond (")
        ret[2] *= 1e-6;     // cor_dut1, second (s)
        ret[3] *= 1e-6;     // cor_lod, second (s)

        return ret;

    }  // End of method 'EOPDataStore2::PMUT_OCEAN()'


    /// Provide the diurnal lunisolar effect on polar motion ("), in time domain.
    /// IERS Conventions 2010.
    Vector<double> EOPDataStore2::PM_LIBR(const CommonTime& UTC) const
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
        double t = (MJD(UTC).mjd - MJD_J2000)/ JC;
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
        Vector<double> ret(2,0.0);

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

            ret[0] += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
            ret[1] += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
        }

        ret[0] *= 1e-6;     // cor_xp, arcsecond (")
        ret[1] *= 1e-6;     // cor_yp, arcsecond (")

        return ret;

    }  // End of method 'EOPDataStore2::PM_LIBR()'


    /// Evaluate the model of subdiurnal libration in the axial component of
    /// rotation, expressed by UT1 and LOD.
    Vector<double> EOPDataStore2::UT_LIBR(const CommonTime& UTC) const
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
        double t = (MJD(UTC).mjd - MJD_J2000)/ JC;
        double t2 = t*t;
        double t3 = t2*t;
        double t4 = t3*t;

        double gmst, chi, l, lp, f, d, om;

        // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega
        chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
        chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

        gmst = 67310.54841 + t*( (8640184.812866 + 3155760000) + t*(0.093104 + t*(-0.0000062)));
        gmst = std::fmod(gmst, DAY_TO_SEC);
        chi = gmst / (DAY_TO_SEC/TWO_PI) + PI;
        chi = std::fmod(chi, TWO_PI);

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
        Vector<double> ret(2,0.0);

        double arg;

        for(int i=0; i<NT; i++)
        {
            arg = 0.0;
            arg = x[i].nchi*chi+x[i].nl*l+x[i].nlp*lp+x[i].nf*f+x[i].nd*d+x[i].nom*om;

            arg = std::fmod(arg, TWO_PI);

            ret[0] += x[i].s1 * std::sin(arg) + x[i].c1 * std::cos(arg);
            ret[1] += x[i].s2 * std::sin(arg) + x[i].c2 * std::cos(arg);
        }

        ret[0] *= 1e-6;     // cor_dut1, second (s)
        ret[1] *= 1e-6;     // cor_lod, second (s)

        return ret;

    }  // End of method 'EOPDataStore2::UT_LIBR()'



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
    EOPDataStore2::EOPData EOPDataStore2::getEOPData(const CommonTime& UTC) const
        throw(InvalidRequest)
    {
        if(!(UTC.getTimeSystem()==TimeSystem::UTC)) throw Exception();

        if( (UTC < initialTime) || (UTC > finalTime) )
        {
            InvalidRequest ire(string("Time tag (")
                + UTC.asString()
                + string("), the timespan of EOPData is not enough."));

            GPSTK_THROW(ire);
        }

        EOPData eopx;

        // first, try to get it from the original EOP Data Map
        EOPDataMap::const_iterator it = allData.find(UTC);
        if(it != allData.end())
        {
            // eop data at utc
            eopx = it->second;
//            cout << eopx.xp << endl;
        }
        // second, try to get it from the interpolated EOP Data Map
        else
        {
            // adjust interpPoints according to the length of EOP Data Map
            int half = interpPoints/2;

            it = allData.lower_bound(UTC);

            EOPDataMap::const_iterator itb = allData.begin();
            EOPDataMap::const_iterator ite = allData.end();

            int x1 = 0;
            for(EOPDataMap::const_iterator itr=it; itr!=itb; --itr)
            {
                x1++;
            }

            int x2 = 0;
            for(EOPDataMap::const_iterator itr=it; itr!=ite; ++itr)
            {
                x2++;
            }

            int x = x1<x2 ? x1 : x2;

            if(half > x)
            {
                half = x;

                cerr << "Warning: the points for interpolation are set to "
                     << half*2
                     << " according the length of EOP Data."
                     << endl;
            }

            itb = it;
            ite = it;

            for(int i=0; i<=half; ++i)
            {
                itb--;
                ite++;
            }

            // extract times and datas from the EOP Data
            const int N = 6;

            vector<double> times;
            vector< vector<double> > datas(N);

            for(EOPDataMap::const_iterator itr=itb; itr!=ite; ++itr)
            {
                // time information
                CommonTime time = itr->first;
                times.push_back(itr->first - itb->first);

                // data information
                EOPData eop = itr->second;

                if(regularization)
                {
                    // Actually, we should us TT time instead of UTC time
                    // as the input parameter of method RG_ZONT2().
                    // However, the leap second data is not available here.
                    // We use UTC here, which will not make a big effect.
                    Vector<double> zont2( RG_ZONT2(time) );

                    // (UT1R-UTC) = (UT1-UTC) - (UT1-UT1R)
                    eop.UT1mUTC -= zont2[0];
                }

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


            // check the data continuity of UT1mUTC in datas
            // if leap second accurs, adjust UT1mUTC.
            //
            // Actually, it is better to first restore UT1mUTC to UT1mTAI.
            // However, the leap second data is not available here.
            // Therefore, the use of the EOP Data with both EOP and Leap
            // Second is proposed, such as the STK EOP Data.
            double dut1[half*2];

            for(int i=0; i<half*2; ++i)
            {
                dut1[i] = (datas[2])[i];
            }

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


            // implemet Lagrange interplation
            vector<double> target(N,0.0);

            double dt = UTC - itb->first;

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


            if(regularization)
            {
                Vector<double> zont2( RG_ZONT2(UTC) );

                // (UT1-UTC) = (UT1R-UTC) + (UT1-UT1R)
                target[2] += zont2[0];
            }

            // eop data at utc
            eopx.xp = target[0];         eopx.yp = target[1];
            eopx.UT1mUTC = target[2];    eopx.LOD = target[3];
            eopx.dX = target[4];         eopx.dY = target[5];
            eopx.err_xp = 0.0;           eopx.err_yp = 0.0;
            eopx.err_UT1mUTC = 0.0;      eopx.err_LOD = 0.0;
            eopx.err_dX = 0.0;           eopx.err_dY = 0.0;
        }

        // ocean tides correction
        if(oceanTides)
        {
            Vector<double> ocean = PMUT_OCEAN(UTC);
//            cout << ocean << endl;

            eopx.xp      +=  ocean[0];
            eopx.yp      +=  ocean[1];
            eopx.UT1mUTC +=  ocean[2];
            eopx.LOD     +=  ocean[3];
        }

        // libration correction
        if(libration)
        {
            Vector<double> libr1 = PM_LIBR(UTC);
//            cout << libr1 << endl;

            Vector<double> libr2 = UT_LIBR(UTC);
//            cout << libr2 << endl;

            eopx.xp      += libr1[0];
            eopx.yp      += libr1[1];
            eopx.UT1mUTC += libr2[0];
            eopx.LOD     += libr2[1];
        }

        return eopx;

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
            if(line.size()>=134 && useBulletinB)
            {
                xp = StringUtils::asDouble(line.substr(135,9));        // arcsec
                yp = StringUtils::asDouble(line.substr(145,9));        // arcsec
                UT1mUTC = StringUtils::asDouble(line.substr(155,10));  // seconds
                dX = StringUtils::asDouble(line.substr(169,6))*1e-3;   // milliarcsec->arcsec
                dY = StringUtils::asDouble(line.substr(179,6))*1e-3;   // milliarcsec->arcsec
            }

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

