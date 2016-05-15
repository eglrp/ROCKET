#include <iostream>
#include <fstream>

#include "MiscMath.hpp"

#include "StringUtils.hpp"

#include "IERSConventions.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

struct GRACE
{
   MJD mjd;
   double rx_ecef,ry_ecef,rz_ecef;
   double rx_eci, ry_eci, rz_eci;
};


int main()
{
   LoadIERSEOPFile("../../rocket/tables/finals2000A.all");
   LoadIERSLSFile("../../rocket/tables/Leap_Second_History.dat");

   vector<double> time;
   vector<double> rx_eci, ry_eci, rz_eci;
   vector<double> rx_ecef, ry_ecef, rz_ecef;

   fstream fin("GRACE.ecef");

   int i = 0;
   while(!fin.eof() && fin.good())
   {
      string line;
      getline(fin, line);

      if(fin.eof()) break;
      if(fin.bad()) break;

      // Read data
      double t = asDouble(line.substr( 0,18));
      double x = for2doub(line.substr(19,29));
      double y = for2doub(line.substr(49,29));
      double z = for2doub(line.substr(79,29));

      // T
      MJD mjd(t,TimeSystem::UTC);
      CommonTime utc( mjd.convertToCommonTime() );

      // ECEF -> ECI
      Matrix<double> c2t( C2TMatrix(utc) );
      Matrix<double> t2c( transpose(c2t) );

      Vector<double> r_ecef(3,0.0);
      r_ecef(0) = x; r_ecef(1) = y; r_ecef(2) = z;

      Vector<double> r_eci(3,0.0);
      r_eci = t2c * r_ecef;

      // Push back
      time.push_back(utc.getSecondOfDay());
      rx_ecef.push_back(r_ecef(0));
      ry_ecef.push_back(r_ecef(1));
      rz_ecef.push_back(r_ecef(2));
      rx_eci.push_back(r_eci(0));
      ry_eci.push_back(r_eci(1));
      rz_eci.push_back(r_eci(2));

      i++; if(i > 11) break;

   }


   double t0 = time[0];
   vector<double>::iterator it;
   for(it=time.begin(); it!=time.end(); ++it)
   {
      (*it) -= t0;
   }

   // Time to compute
   MJD mjd(53218.180620813691, TimeSystem::UTC);
   CommonTime utct( mjd.convertToCommonTime() );

   Matrix<double> c2t( C2TMatrix(utct) );
   Matrix<double> t2c( transpose(c2t) );

   double dt = utct.getSecondOfDay() - t0;
   double xerr,yerr,zerr;

   // Interpolation -> Transformation
   double rxt_ecef = LagrangeInterpolation(time, rx_ecef, dt, xerr);
   double ryt_ecef = LagrangeInterpolation(time, ry_ecef, dt, yerr);
   double rzt_ecef = LagrangeInterpolation(time, rz_ecef, dt, zerr);

   Vector<double> rt_ecef(3,0.0), rt_eci(3,0.0);
   rt_ecef(0) = rxt_ecef;
   rt_ecef(1) = ryt_ecef;
   rt_ecef(2) = rzt_ecef;

   rt_eci = t2c * rt_ecef;

   cout << fixed;
   cout << rt_eci(0) << ' '
        << rt_eci(1) << ' '
        << rt_eci(2) << endl;

   cout << xerr << ' '
        << yerr << ' '
        << zerr << endl;

   // Transformation -> Interpolation
   double rxt_eci = LagrangeInterpolation(time, rx_eci, dt, xerr);
   double ryt_eci = LagrangeInterpolation(time, ry_eci, dt, yerr);
   double rzt_eci = LagrangeInterpolation(time, rz_eci, dt, zerr);

   cout << rxt_eci << ' '
        << ryt_eci << ' '
        << rzt_eci << endl;

   cout << xerr << ' '
        << yerr << ' '
        << zerr << endl;

   fin.close();

   return 0;
}
