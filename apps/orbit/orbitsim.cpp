#pragma ident "$Id$"

//============================================================================
//
// Function
// --------
//
// Orbit simulator.
//
// CopyRight
// ---------
//
// Kaifa Kuang, Wuhan University, 2016
//
// History
// -------
//
// 2016/05/07 Create this program.
//
//============================================================================

// Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>

// Class to read configuration files
#include "ConfDataReader.hpp"

// Class to deal with civil time
#include "CivilTime.hpp"

// Functions to deal with IERS Conventions
#include "IERSConventions.hpp"

// Class to deal with EGM08 gravity model
#include "EGM08GravityModel.hpp"

// Class to do Runge-Kutta-Fehlberg integrator
#include "RungeKuttaFehlberg.hpp"


using namespace std;
using namespace gpstk;


// Orbit Configurations
class OrbitEOM : public EquationOfMotion
{
public:

   // Constructor
   OrbitEOM() {}

   // Destructor
   virtual ~OrbitEOM() {}


   // Set reference epoch
   inline OrbitEOM& setRefEpoch(const CommonTime& utc)
   {
      utc0 = utc;
      return (*this);
   }

   // Get reference epoch
   inline CommonTime getRefEpoch() const
   {
      return utc0;
   }


   // Set spacecraft
   inline OrbitEOM& setSpacecraft(const Spacecraft& s)
   {
       sc = s;
       return (*this);
   }

   // Get spacecraft
   inline Spacecraft getSpacecraft() const
   {
      return sc;
   }

   // Set EGM
   inline OrbitEOM& setEGM(string file, int degree, int order)
   {
      egm.loadFile(file);
      egm.setDesiredDegree(degree, order);

      return (*this);
   }

   // Get derivatives
   virtual Vector<double> getDerivatives(const double& t,
                                         const Vector<double>& y);

private:
   
   CommonTime utc0;        // Reference time

   Spacecraft sc;          // Spacecraft

   EarthBody eb;           // Earthbody

   EGM08GravityModel egm;  // EGM08GravityModel
};


// Get derivatives
Vector<double> OrbitEOM::getDerivatives(const double& t, const Vector<double>& y)
{
   // Current epoch
   CommonTime utc(utc0+t);

   // Current state
   Vector<double> state( sc.getStateVector() );
   state( 0) = y(0); state( 1) = y(1); state( 2) = y(2);    // r
   state(21) = y(3); state(22) = y(4); state(23) = y(5);    // v

   // Set spacecraft with current r,v
   sc.setStateVector(state);

   // Current v
   Vector<double> v(3,0.0);
   v(0) = y(3); v(1) = y(4); v(2) = y(5);

   // Current a 
   Vector<double> a(3,0.0);
   egm.doCompute(utc, eb, sc);
   a = egm.getAccel();

   // Current v,a
   Vector<double> dy(6,0.0);
   dy(0) = v(0); dy(1) = v(1); dy(2) = v(2);
   dy(3) = a(0); dy(4) = a(1); dy(5) = a(2);

    return dy;
}


int main(void)
{

   // Configuration file
   ConfDataReader confReader;
   try
   {
      confReader.open("../../../ROCKET/workplace/orbit/orbitsim.conf");
   }
   catch(...)
   {
      cerr << "Configuration file open error." << endl;

      return 1;
   }

   // EOP file
   string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
   try
   {
      LoadIERSEOPFile(eopFile);
   }
   catch(...)
   {
      cerr << "EOP File Load Error." << endl;

      return 1;
   }

   // LeapSecond file
   string lsFile = confReader.getValue("IERSLSFile", "DEFAULT");
   try
   {
      LoadIERSLSFile (lsFile);
   }
   catch(...)
   {
      cerr << "LS File Load Error." << endl;

      return 1;
   }


   // Initial time
   int    yea = confReader.getValueAsInt("Year", "DEFAULT");
   int    mon = confReader.getValueAsInt("Month", "DEFAULT");
   int    day = confReader.getValueAsInt("Day", "DEFAULT");
   int    hou = confReader.getValueAsInt("Hour", "DEFAULT");
   int    min = confReader.getValueAsInt("Minute", "DEFAULT");
   double sec = confReader.getValueAsDouble("Second", "DEFAULT");

   CivilTime ct(yea,mon,day,hou,min,sec, TimeSystem::UTC);
   CommonTime utc0( ct.convertToCommonTime() );
 
   // Transform matrix at utc0
   Matrix<double> c2t( C2TMatrix(utc0) );
   Matrix<double> t2c( transpose(c2t) );
   Matrix<double> dc2t( dC2TMatrix(utc0) );
   Matrix<double> dt2c( transpose(dc2t) );

   // Initial state in ITRS
   double rx = confReader.getValueAsDouble("Rx", "DEFAULT");
   double ry = confReader.getValueAsDouble("Ry", "DEFAULT");
   double rz = confReader.getValueAsDouble("Rz", "DEFAULT");
   double vx = confReader.getValueAsDouble("Vx", "DEFAULT");
   double vy = confReader.getValueAsDouble("Vy", "DEFAULT");
   double vz = confReader.getValueAsDouble("Vz", "DEFAULT");

   Vector<double> r0_itrs(3,0.0), v0_itrs(3,0.0);
   r0_itrs(0) = rx; r0_itrs(1) = ry; r0_itrs(2) = rz;
   v0_itrs(0) = vx; v0_itrs(1) = vy; v0_itrs(2) = vz;

   // Initial state in ICRS
   Vector<double> r0_icrs(3,0.0), v0_icrs(3,0.0);
   r0_icrs = t2c * r0_itrs;
   v0_icrs = t2c * v0_itrs + dt2c * r0_itrs;

   Vector<double> rv0_icrs(6,0.0);
   rv0_icrs(0) = r0_icrs(0); rv0_icrs(1) = r0_icrs(1); rv0_icrs(2) = r0_icrs(2);
   rv0_icrs(3) = v0_icrs(0); rv0_icrs(4) = v0_icrs(1); rv0_icrs(5) = v0_icrs(2);

   Vector<double> p0(0,0.0);

   // Spacecraft
   Spacecraft sc;

   // Orbit configuration
   string egmFile = confReader.getValue("EGM08File", "DEFAULT");
   int    degree  = confReader.getValueAsInt("EGM_Degree", "DEFAULT");
   int    order   = confReader.getValueAsInt("EGM_Order", "DEFAULT");

   OrbitEOM orbit;
   orbit.setRefEpoch(utc0);
   try
   {
      orbit.setEGM(egmFile, degree, order);
   }
   catch(...)
   {
      cerr << "EGM Set Error." << endl;
   }

   // Orbit integrator
   RungeKuttaFehlberg rkf;

   // Stepsize, unit: second
   double stepSize = confReader.getValueAsDouble("StepSize", "DEFAULT");
   rkf.setStepSize(stepSize);

   // Length, unit: hour
   double length    = confReader.getValueAsDouble("Length", "DEFAULT");

   double t0(0.0);

   // Outfile
   string outFile   = confReader.getValue("Outfile", "DEFAULT");
   ofstream fout;
   try
   {
      fout.open(outFile.c_str(), ios::out);
   }
   catch(...)
   {
      cerr << "Out File Open Error." << endl;

      return 1;
   }

   cout << "Start of Integration." << endl;
   // Loop
   for(int i=0; i<=int(length*3600/stepSize); ++i)
   {
      // Current time
      CommonTime utc(utc0+t0);

      // Current transform matrix
      c2t = C2TMatrix(utc);
      dc2t = dC2TMatrix(utc);

      // Current state in ICRS
      Vector<double> rv_icrs(rv0_icrs);
      Vector<double> state( sc.getStateVector() );
      state( 0) = rv_icrs(0); state( 1) = rv_icrs(1); state( 2) = rv_icrs(2);
      state(21) = rv_icrs(3); state(22) = rv_icrs(4); state(23) = rv_icrs(5);

      Vector<double> r_icrs(3,0.0), v_icrs(3,0.0);
      r_icrs(0) = rv_icrs(0); r_icrs(1) = rv_icrs(1); r_icrs(2) = rv_icrs(2);
      v_icrs(0) = rv_icrs(3); v_icrs(1) = rv_icrs(4); v_icrs(2) = rv_icrs(5);

      // Current state in ITRS
      Vector<double> r_itrs(3,0.0), v_itrs(3,0.0);
      r_itrs = c2t * r_icrs;
      v_itrs = c2t * v_icrs + dc2t * r_icrs;

      fout << CivilTime(utc) << " "
           << fixed << setprecision(6)
           << setw(18) << r_itrs(0)
           << setw(18) << r_itrs(1)
           << setw(18) << r_itrs(2)
           << setw(14) << v_itrs(0)
           << setw(14) << v_itrs(1)
           << setw(14) << v_itrs(2)
           << endl;

      // Spacecraft settings
      sc.setEpoch(utc);
      sc.setStateVector(state);

      // Orbit settings
      orbit.setSpacecraft(sc);

      // Integration
      rv_icrs = rkf.integrateTo(t0, rv0_icrs, &orbit, t0+10.0);

      // Update
      t0 += 10.0;
      rv0_icrs = rv_icrs;
   }

   cout << "End of Integration." << endl;

   fout.close();

   return 0;
}
