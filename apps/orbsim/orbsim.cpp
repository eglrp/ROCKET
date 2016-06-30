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

// Basic framework for programs in the GPSTk. 'process()' method MUST
// be implemented
#include "BasicFramework.hpp"

// Class to read configuration files
#include "ConfDataReader.hpp"

// Class to deal with civil time
#include "CivilTime.hpp"

// Class to deal with reference system
#include "ReferenceSystem.hpp"

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
   inline OrbitEOM& setEGM(const string& file,
                           const int& degree,
                           const int& order)
   {
      egm.setDesiredDegreeOrder(degree, order);
      egm.loadFile(file);

      return (*this);
   }

   // Set ReferenceSystem
   inline OrbitEOM& setReferenceSystem(ReferenceSystem& refSys)
   {
      egm.setReferenceSystem(refSys);

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
   // Current time
   CommonTime utc(utc0+t);

   // Current state
   Vector<double> state( sc.getStateVector() );

   // Update position and velocity
   state(0) = y(0); state(1) = y(1); state(2) = y(2);    // r
   state(3) = y(3); state(4) = y(4); state(5) = y(5);    // v

   // Set spacecraft with current position and velocity
   sc.setStateVector(state);

   // Current velocity
   Vector<double> v(3,0.0);
   v(0) = y(3); v(1) = y(4); v(2) = y(5);

   // Current acceleration 
   Vector<double> a(3,0.0);

   egm.doCompute(utc, eb, sc);
   a = egm.getAccel();

   // Current velocity and acceleration
   Vector<double> dy(6,0.0);
   dy(0) = v(0); dy(1) = v(1); dy(2) = v(2);
   dy(3) = a(0); dy(4) = a(1); dy(5) = a(2);

   return dy;
}


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class orbsim : public BasicFramework
{
public:

      // Constructor declaration
   orbsim(char* arg0);

protected:

      // Method that will take care of processing
   virtual void process();

      // Method that hold code to be run BEFORE processing
   virtual void spinUp();

private:

      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // Configuration file reader
   ConfDataReader confReader;

}; // End of 'orbsim' class declaration


// Let's implement constructor details
orbsim::orbsim(char* arg0)
   :
   BasicFramework( arg0,
"\nThis program reads initial state and tables from a configuration file\n"
"and implements numerical integrations to get a simulated orbit.\n\n"
"Please consult the default configuration file, 'orbsim.conf', for\n"
"further details.\n\n"
"The output file format is as follows:\n\n"
" 1) t (UTC)\n"
" 2) Rx (m)\n"
" 3) Ry (m)\n"
" 4) Rz (m)\n"
" 5) Vx (m/s)\n"
" 6) Vy (m/s)\n"
" 7) Vz (m/s)\n" ),
   // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
             " [-c|--conffile]   Name of configuration file ('orbsim.conf' by default).",
             false )
{
   // This option may appear just once at CLI
   confFile.setMaxCount(1);
}


   // Method that will be executed AFTER initialization but BEFORE processing
void orbsim::spinUp()
{

      // Check if the user provided a configuration file name
   if( confFile.getCount() > 0)
   {

         // Enable exceptions
      confReader.exceptions(ios::failbit);

      try
      {    
            // Try to open the provided configuration file
         confReader.open( confFile.getValue()[0] );

      }
      catch(...)
      {

         cerr << "Problem opening file "
              << confFile.getValue()[0]
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper "
              << "read permissions."
              << endl;

         exit(-1);

      }  // End of 'try-catch' block

   }
   else
   {

      try
      {
            // Try to open default configuration file
         confReader.open( "orbsim.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'orbsim.conf'"
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper read "
              << "permissions. Try providing a configuration file with "
              << "option '-c'."
              << endl;

         exit(-1);

      }  // End of 'try-catch' block

   }  // End of 'if( confFile.getCount() > 0 )'


      // If a given variable is not found in the provided section, then
      // 'confReader' will look for it in the 'DEFAULT' section.
   confReader.setFallback2Default(true);


}  // End of method 'orbsim::spinUp()'


void orbsim::process()
{
      // EOP Data Store
   EOPDataStore eopDataStore;

      // EOP file
   string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
   try
   {
      eopDataStore.loadIERSFile(eopFile);
   }
   catch(...)
   {
      cerr << "EOP File Load Error." << endl;

      return;
   }

      // Leap Sec Store
   LeapSecStore leapSecStore;

      // Leap Second file
   string lsFile = confReader.getValue("IERSLSFile", "DEFAULT");

   try
   {
      leapSecStore.loadFile(lsFile);
   }
   catch(...)
   {
      cerr << "LS File Load Error." << endl;

      return;
   }

      // Reference System
   ReferenceSystem rs;
   rs.setEOPDataStore(eopDataStore);
   rs.setLeapSecStore(leapSecStore);

      // Initial time
   int    year = confReader.getValueAsInt("Year",     "DEFAULT");
   int    mon  = confReader.getValueAsInt("Month",    "DEFAULT");
   int    day  = confReader.getValueAsInt("Day",      "DEFAULT");
   int    hour = confReader.getValueAsInt("Hour",     "DEFAULT");
   int    min  = confReader.getValueAsInt("Minute",   "DEFAULT");
   double sec  = confReader.getValueAsDouble("Second","DEFAULT");

   CivilTime ct(year,mon,day,hour,min,sec, TimeSystem::UTC);

   CommonTime utc0( ct.convertToCommonTime() );

      // Transform matrix at utc0
   Matrix<double> c2t( rs.C2TMatrix(utc0) );
   Matrix<double> t2c( transpose(c2t) );
   Matrix<double> dc2t( rs.dC2TMatrix(utc0) );
   Matrix<double> dt2c( transpose(dc2t) );

//   cout << c2t << endl;

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
   string egmFile = confReader.getValue("EGM08File",        "DEFAULT");
   int    degree  = confReader.getValueAsInt("EGM_Degree",  "DEFAULT");
   int    order   = confReader.getValueAsInt("EGM_Order",   "DEFAULT");

   OrbitEOM orbit;
   orbit.setRefEpoch(utc0);
   try
   {
      orbit.setEGM(egmFile, degree, order);
   }
   catch(...)
   {
      cerr << "EGM Set Error." << endl;

      return;
   }

   orbit.setReferenceSystem(rs);

      // Orbit integrator
   RungeKuttaFehlberg rkf;

      // StepSize, unit: second
   double stepSize = confReader.getValueAsDouble("StepSize", "DEFAULT");
   rkf.setStepSize(stepSize);

      // Length, unit: hour
   double length    = confReader.getValueAsDouble("Length", "DEFAULT");

      // t0
   double t0(0.0);

      // Outfile
   string outFile_ECEF  = confReader.getValue("OutFile_ECEF", "DEFAULT");
   string outFile_ECI   = confReader.getValue("OutFile_ECI", "DEFAULT");

   ofstream fout_ECEF;

   try
   {
      fout_ECEF.open(outFile_ECEF.c_str(), ios::out);
   }
   catch(...)
   {
      cerr << "Out File for ECEF Open Error." << endl;

      return;
   }

   ofstream fout_ECI;
   try
   {
      fout_ECI.open(outFile_ECI.c_str(), ios::out);
   }
   catch(...)
   {
      cerr << "Out File for ECI Open Error." << endl;

      return;
   }

   cout << "Start of Integration." << endl;

      // Loop
   for(int i=0; i<=int(length*3600/900); ++i)
   {
         // Current time
      CommonTime utc(utc0+t0);

         // Current transform matrix
      c2t = rs.C2TMatrix(utc);
      dc2t = rs.dC2TMatrix(utc);

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

      fout_ECEF << CivilTime(utc) << " "
                << fixed << setprecision(6)
                << setw(18) << r_itrs(0)
                << setw(18) << r_itrs(1)
                << setw(18) << r_itrs(2)
                << setw(14) << v_itrs(0)
                << setw(14) << v_itrs(1)
                << setw(14) << v_itrs(2)
                << endl;

      fout_ECI  << CivilTime(utc) << " "
                << fixed << setprecision(6)
                << setw(18) << r_icrs(0)
                << setw(18) << r_icrs(1)
                << setw(18) << r_icrs(2)
                << setw(14) << v_icrs(0)
                << setw(14) << v_icrs(1)
                << setw(14) << v_icrs(2)
                << endl;

         // Spacecraft settings
      sc.setCurrentTime(utc);
      sc.setStateVector(state);

         // Orbit settings
      orbit.setSpacecraft(sc);

         // Integration
      rkf.integrateTo(t0, rv0_icrs, &orbit, t0+900);

         // Update
//      t0 += stepSize;
//      rv0_icrs = rv_icrs;
   }

   cout << "End of Integration." << endl;

   fout_ECEF.close();
   fout_ECI.close();

}  // End of 'orbsim::process()'


int main(int argc, char* argv[])
{

   try
   {
      orbsim program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if( !program.initialize(argc, argv, false) )
      {
         return 0;
      }

      if( !program.run() )
      {
         return 1;
      }

      return 0;
   }
   catch(Exception& e)
   {
      cerr << "Problem: " << e << endl;

      return 1;
   }
   catch(...)
   {
      cerr << "Unknown error." << endl;

      return 1;
   }

   return 0;

}  // End of 'main()'
