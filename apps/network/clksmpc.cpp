#pragma ident "$Id$"

//============================================================================
//
//  Function
//  --------
//
//  Estimate GPS satellite clock products using smoothed code from global
//  distribute IGS stations.
//
//  CopyRight
//  ---------
//  
//  Shoujian Zhang, Wuhan University, 2010, 2011 
//
//  History
//  -------
//
//  2015/07/21  Create this program 
//
//============================================================================


// Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>


   // Basic framework for programs in the GPSTk. 'process()' method MUST
   // be implemented
#include "BasicFramework.hpp"

   // Class for handling observation RINEX files
#include "RinexObsStream.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class to store a list of processing objects
#include "ProcessingList.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModel.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservables.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class for easily changing reference base from ECEF to NEU
#include "XYZ2NEU.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector.hpp"
#include "LICSDetector2.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to compute the effect of solid tides
#include "PCSmoother.hpp"

   // Class to compute the effect of solid tides
#include "SolidTides.hpp"

   // Class to compute the effect of ocean loading
#include "OceanLoading.hpp"

   // Class to compute the effect of pole tides
#include "PoleTides.hpp"

   // Class to correct observables
#include "CorrectObservables.hpp"

   // Classes to deal with Antex antenna parameters
#include "Antenna.hpp"
#include "AntexReader.hpp"

   // Class to compute the effect of wind-up
#include "ComputeWindUp.hpp"

   // Class to compute the effect of satellite antenna phase center
#include "ComputeSatPCenter.hpp"

   // Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

   // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

   // Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker2.hpp"

   // Class to compute gravitational delays
#include "GravitationalDelay.hpp"

   // Class to align phases with code measurements
#include "PhaseCodeAlignment.hpp"

   // Class to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

   // Class to solve a general equation.
#include "SolverPPP.hpp"

   // Class to solve a general equation.
#include "SolverGeneral2.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"

   // Class to compute the elevation weights
#include "ComputeElevWeights.hpp"

   // Class to deal with some string input/output
#include "StringUtils.hpp"

   // Class to deal with time string
#include "TimeString.hpp"

   // Class to deal with civil time
#include "CivilTime.hpp"


#include "Epoch.hpp"

   // Class to deal with the ambiguity constraints for solver
#include "AmbiguityDatum.hpp"

   // Class to filter out the PC outliers using prefit-residual
#include "PrefitFilter.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clksmpc : public BasicFramework
{
public:

      // Constructor declaration
   clksmpc(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();


      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


      // Method that hold code to be run AFTER processing
   virtual void shutDown();


      // Method to deal with the preprocessing of the gnss data
   virtual void preprocessing();


      // Method to deal with the preprocessing of the gnss data
   virtual void solve();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // Configuration file reader
   ConfDataReader confReader;

      // Map to store dry tropospheric delay value
   std::map<SourceID, double> tropoMap;

      // Master station
   SourceID master;

      // Reference stations set
   std::set<SourceID> refStationSet;

      // Rover receiver
   SourceID rover;

      // Data structure
   gnssDataMap gdsMap;

      // Mulitmap used to store the clock data line 
   std::multimap<CommonTime, std::string> solutionMap;

      // Satellite sets for all the estimated satellite clocks
   SatIDSet satSet;

      // Source sets for all the estimated receiver clocks
   SourceIDSet sourceSet;

      // dcb program used in the header printing 
   string dcbProgram;

      // dcb source used in the header printing 
   string dcbSource;

      // antex file
   string antexFile;

      // If you want to share objects and variables among methods, you'd
      // better declare them here
      

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 5 );

      // Method to extract solution
   void getClockSoluts( const SolverGeneral2& solver,
                        const CommonTime& workEpoch );

      // Method to print solution
   void printClockHeader( ofstream& clockFile );

      // Method to print solution
   void printClockData( ofstream& clockFile );


}; // End of 'clksmpc' class declaration



   // Let's implement constructor details
clksmpc::clksmpc(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Precise Orbits Positioning' strategy.\n\n"
"The output file format is as follows:\n\n"
" 1) Seconds of day\n"
" 2) dLat (m)\n"
" 3) dLon (m)\n"
" 4) dH (m)\n"
" 5) Zenital Tropospheric Delay - zpd (m)\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('clksmpc.conf' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clksmpc::clksmpc'



   // Method that will be executed AFTER initialization but BEFORE processing
void clksmpc::spinUp()
{

      // Check if the user provided a configuration file name
   if ( confFile.getCount() > 0 )
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
              << "read permissions." << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }
   else
   {

      try
      {
            // Try to open default configuration file
         confReader.open( "clksmpc.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'clksmpc.conf'"
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper read "
              << "permissions. Try providing a configuration file with "
              << "option '-c'."
              << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }  // End of 'if ( confFile.getCount() > 0 )'


      // If a given variable is not found in the provided section, then
      // 'confReader' will look for it in the 'DEFAULT' section.
   confReader.setFallback2Default(true);


}  // End of method 'clksmpc::spinUp()'



   // Method that will really process information
void clksmpc::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   solve();
}



   // Method that will really process information
void clksmpc::preprocessing()
{

      /////////////////
      // Put here what you want to apply to ALL stations
      /////////////////


   //// vvvv Ephemeris handling vvvv

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);

      // Load all the SP3 ephemerides files from variable list
   string sp3File;
   while ( (sp3File = confReader.fetchListValue("SP3List", "DEFAULT" ) ) != "" )
   {

         // Try to load each ephemeris file
      try
      {
         SP3EphList.loadFile( sp3File );
      }
      catch (...)
      {
            // If file doesn't exist, issue a warning
         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }

   }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "

      // Read if we will use the cross-correlation receiver data,
   bool useRinexClock( confReader.getValueAsBoolean( "useRinexClock", "DEFAULT" ) );

   if(useRinexClock)
   {
         // Load all the rinex clock files from variable list
      string rinexClockFile;
      while ( (rinexClockFile = confReader.fetchListValue("rinexClockList","DEFAULT") ) != "" )
      {
         try
         {
            SP3EphList.loadRinexClockFile( rinexClockFile );
         }
         catch (...)
         {
               // If file doesn't exist, issue a warning
            cerr << "rinex clock file '" << rinexClockFile << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;

            exit(-1);
         }
      }  // End of 'while ( (rinexClockFile = confReader.fetchListValue( ... "
   }

   //// ^^^^ Ephemeris handling ^^^^


   //// vvvv Tides handling vvvv

      // Object to compute tidal effects
   SolidTides solid;


      // Configure ocean loading model
   OceanLoading ocean;
   ocean.setFilename( confReader.getValue( "oceanLoadingFile", "DEFAULT" ) );


      // Declare a "EOPDataStore" object to handle earth rotation parameter file
   EOPDataStore eopStore;

      // Load all the EOP files from variable list
   string eopFile;
   while ( (eopFile = confReader.fetchListValue("EOPFileList", "DEFAULT" ) ) != "" )
   {
         // Try to load each ephemeris file
      try
      {
         eopStore.loadIGSFile( eopFile );
      }
      catch (...)
      {
            // If file doesn't exist, issue a warning
         cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }

   }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "

      // Object to model pole tides
   PoleTides pole(eopStore);

   //// ^^^^ Tides handling ^^^^


   //// Starting preprocessing for ALL stations ////

      // We will read each section name, which is equivalent to station name
      // Station names will be read in alphabetical order
   string station;
   while ( (station = confReader.getEachSection()) != "" )
   {

         // We will skip 'DEFAULT' section because we are waiting for
         // a specific section for each receiver. However, if data is
         // missing we will look for it in 'DEFAULT' (see how we use method
         // 'setFallback2Default()' of 'ConfDataReader' object in 'spinUp()'
      if( station == "DEFAULT" )
      {
         continue;
      }


         // Show a message indicating that we are starting with this station
      cerr << "Starting processing for station: '" << station << "'." << endl;


         // Create input observation file stream
      RinexObsStream rin;

         // Enable exceptions
      rin.exceptions(ios::failbit);

         // Try to open Rinex observations file
      try
      {

            // Open Rinex observations file in read-only mode
         rin.open( confReader("rinexObsFile", station), std::ios::in );

      }
      catch(...)
      {

         cerr << "Problem opening file '"
              << confReader.getValue("rinexObsFile", station)
              << "'." << endl;

         cerr << "Maybe it doesn't exist or you don't have "
              << "proper read permissions."
              << endl;

         cerr << "Skipping receiver '" << station << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

         continue;

      }  // End of 'try-catch' block



         // Load station nominal position
      double xn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double yn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double zn(confReader.fetchListValueAsDouble("nominalPosition",station));
         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

      Position nominalPos( xn, yn, zn );


         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;


         // This object will check that all required observables are present
      RequireObservables requireObs;
      requireObs.addRequiredType(TypeID::P2);
      requireObs.addRequiredType(TypeID::L1);
      requireObs.addRequiredType(TypeID::L2);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilter pObsFilter;
      pObsFilter.setFilteredType(TypeID::P2);

        // Read if we should use C1 instead of P1
      bool usingC1( confReader.getValueAsBoolean( "useC1", station ) );
      if ( usingC1 )
      {
         requireObs.addRequiredType(TypeID::C1);
         pObsFilter.addFilteredType(TypeID::C1);
      }
      else
      {
         requireObs.addRequiredType(TypeID::P1);
         pObsFilter.addFilteredType(TypeID::P1);
      }
      
         // Add 'requireObs' to processing list (it is the first)
      pList.push_back(requireObs);


         // IMPORTANT NOTE:
         // It turns out that some receivers don't correct their clocks
         // from drift.
         // When this happens, their code observations may drift well beyond
         // what it is usually expected from a pseudorange. In turn, this
         // effect causes that "SimpleFilter" objects start to reject a lot of
         // satellites.
         // Thence, the "filterCode" option allows you to deactivate the
         // "SimpleFilter" object that filters out C1, P1 and P2, in case you
         // need to.
      bool filterCode( confReader.getValueAsBoolean( "filterCode", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // This object defines several handy linear combinations
      LinearCombinations comb;

         // Object to compute linear combinations for cycle slip detection
      ComputeLinear linear1;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear1.addLinear(comb.pdeltaCombWithC1);
         linear1.addLinear(comb.mwubbenaCombWithC1);
      }
      else
      {
         linear1.addLinear(comb.pdeltaCombination);
         linear1.addLinear(comb.mwubbenaCombination);
      }
      linear1.addLinear(comb.ldeltaCombination);
      linear1.addLinear(comb.liCombination);
      pList.push_back(linear1);       // Add to processing list


         // Objects to mark cycle slips
      LICSDetector2 markCSLI2;         // Checks LI cycle slips
      pList.push_back(markCSLI2);      // Add to processing list
      MWCSDetector  markCSMW;          // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);       // Add to processing list


         // Object to keep track of satellite arcs
         // Notes: delete unstable satellite may cause discontinuity
         //        in the network processing!
      SatArcMarker markArc;
      markArc.setDeleteUnstableSats(true);
      markArc.setUnstablePeriod(151.0);
      pList.push_back(markArc);       // Add to processing list


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);

         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation",station));

         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
      if( !usingC1 )
      {
         basic.setDefaultObservable(TypeID::P1);
      }

         // Add to processing list
      pList.push_back(basic);


         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights;
      pList.push_back(elevWeights);       // Add to processing list


//       // Object to remove eclipsed satellites
//    EclipsedSatFilter eclipsedSV;
//    pList.push_back(eclipsedSV);       // Add to processing list


         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list


         // Vector from monument to antenna ARP [UEN], in meters
      double uARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double eARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double nARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      Triple offsetARP( uARP, eARP, nARP );


         // Declare some antenna-related variables
      Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
      AntexReader antexReader;
      Antenna receiverAntenna;

         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex", station ) );
      string antennaModel;
      if( useantex )
      {
            // Read antex file
         antexFile = confReader.getValue( "antexFile", station );

            // Feed Antex reader object with Antex file
         antexReader.open( antexFile );

            // Antenna model 
         antennaModel = confReader.getValue( "antennaModel", station );

            // Get receiver antenna parameters
            // Warning: If no corrections are not found for one specific 
            //          radome, then the antenna with radome NONE are used.
         try
         {
            receiverAntenna = antexReader.getAntenna( antennaModel );
         }
         catch(ObjectNotFound& notFound)
         {
               // new antenna model
            antennaModel.replace(16,4,"NONE");
               // new receiver antenna with new antenna model
            receiverAntenna = antexReader.getAntenna( antennaModel );
         }
      }


         // Object to compute satellite antenna phase center effect
      ComputeSatPCenter svPcenter(nominalPos);
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
         svPcenter.setAntexReader( antexReader );
      }
      pList.push_back(svPcenter);       // Add to processing list


         // Declare an object to correct observables to monument
      CorrectObservables corr(SP3EphList);
      corr.setNominalPosition(nominalPos);
      corr.setMonument( offsetARP );

         // Check if we want to use Antex patterns
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns", station ));
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim", station));
      }
      else
      {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
         offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1", station);

            // Vector from antenna ARP to L2 phase center [UEN], in meters
         offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2", station);

         corr.setL1pc( offsetL1 );
         corr.setL2pc( offsetL2 );

      }
      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos,
                            confReader.getValue( "satDataFile", station ) );
      pList.push_back(windup);       // Add to processing list



         // Declare a NeillTropModel object, setting its parameters
      NeillTropModel neillTM( nominalPos.getAltitude(),
                              nominalPos.getGeodeticLatitude(),
                              confReader.getValueAsInt("dayOfYear", station) );

         // We will need this value later for printing
      double drytropo( neillTM.dry_zenith_delay() );


         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         // Object to compute ambiguity for L1 and L2
      ComputeLinear linear2;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear2.addLinear(comb.q1CombWithC1);
         linear2.addLinear(comb.q2CombWithC1);
      }
      else
      {
         linear2.addLinear(comb.q1Combination);
         linear2.addLinear(comb.q2Combination);
      }
      pList.push_back(linear2);       // Add to processing list
      

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL1;
      phaseAlignL1.setCodeType(TypeID::Q1);
      phaseAlignL1.setPhaseType(TypeID::L1);
      phaseAlignL1.setPhaseWavelength(0.190293672798365);
      pList.push_back(phaseAlignL1);       // Add to processing list

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL2;
      phaseAlignL2.setCodeType(TypeID::Q2);
      phaseAlignL2.setPhaseType(TypeID::L2);
      phaseAlignL2.setPhaseWavelength(0.244210213424568);
      pList.push_back(phaseAlignL2);       // Add to processing list


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinear linear3;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         // WARNING: When using C1 instead of P1 to compute PC combination,
         //          be aware that instrumental errors will NOT cancel,
         //          introducing a bias that must be taken into account by
         //          other means. This won't be taken into account in this
         //          example.
         linear3.addLinear(comb.pcCombWithC1);
      }
      else
      {
         linear3.addLinear(comb.pcCombination);
      }
      linear3.addLinear(comb.lcCombination);
      pList.push_back(linear3);       // Add to processing list


         // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);

         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
         pList.push_back(pcFilter);       // Add to processing list
      }

         // Object to smooth the PC code using LC phase observables
      PCSmoother smoothPC;
      smoothPC.setMaxWindowSize(80);
      pList.push_back(smoothPC);


         // Object to compute prefit-residuals for clock estimation
         // Warning: Modified by Shoujian Zhang, to determine the exact
         //          satellite clocks 'cdt' directly, instead of the 
         //          clock correction delta('cdt') 
      ComputeLinear linear6(comb.pcPrefitC);
      linear6.addLinear(comb.lcPrefitC);
      pList.push_back(linear6);       // Add to processing list


         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list
      
         // The Year/DayOfYear/SecOfYear format, you MUST set the timeSystem at 
         // the same time.
      YDSTime yds( confReader.getValueAsInt("year", station),
                   confReader.getValueAsInt("dayOfYear", station),
                   confReader.getValueAsDouble("secOfDay", station), TimeSystem::GPS );
      CommonTime initialTime( yds.convertToCommonTime() );

         // Object to decimate data
      Decimate decimateData(
              confReader.getValueAsDouble( "decimationInterval", station ),
              confReader.getValueAsDouble( "decimationTolerance", station ),
              initialTime);
      pList.push_back(decimateData);       // Add to processing list


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;

         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel", station ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
         modelName = confReader.getValue( "modelFile", station );
         modelfile.open( modelName.c_str(), ios::out );
      }

         // *** The REAL preprocessing of a SPECIFIC station is done here *** //

         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Set the nominal position from the IGS solution file
         gRin.header.source.nominalPos = nominalPos;


            // Store current epoch
         CommonTime time(gRin.header.epoch);

            // Compute solid, oceanic and pole tides effects at this epoch
         Triple tides( solid.getSolidTide( time, nominalPos )  +
                       ocean.getOceanLoading( station, time )  +
                       pole.getPoleTide( time, nominalPos )    );

            // Update observable correction object with tides information
         corr.setExtraBiases(tides);

         try
         {

               // Let's preprocess data. Thanks to 'ProcessingList' this is
               // very simple and compact: Just one line of code!!!.
            gRin >> pList;

         }
         catch(DecimateEpoch& d)
         {
               // If we catch a DecimateEpoch exception, just continue.
            continue;
         }
         catch(SVNumException& s)
         {
               // If we catch a SVNumException exception, just continue.
            continue;
         }
         catch(Exception& e)
         {
            cerr << "Exception for receiver '" << station <<
                    "' at epoch: " << time << "; " << e << endl;
            continue;
         }
         catch(...)
         {
            cerr << "Unknown exception for receiver '" << station <<
                    " at epoch: " << time << endl;
            continue;
         }


            // Ask if we are going to print the model
         if ( printmodel )
         {
            printModel( modelfile,
                        gRin );

         }

            // Only keep the necessary types for 'solver' 
         TypeIDSet types;
         types.insert(TypeID::weight);
         types.insert(TypeID::wetMap);
         types.insert(TypeID::elevation);
         types.insert(TypeID::prefitC);

            // Delete the types not in 'types' to save the memory
         gRin.keepOnlyTypeID(types);   

            // Store observation data
         gdsMap.addGnssRinex(gRin);


         // The given epoch has been preprocessed. Let's get the next one

      }  // End of 'while(rin >> gRin)'


         // Get source
         // NOTE: 'station' is a 'string', and we need a 'SourceID'
      SourceID source( gRin.header.source );


         // Store dry tropospheric delay value to use it in the solution
      tropoMap[ source ] = neillTM.dry_zenith_delay();


         //// Here ends the preprocessing of all data for the given station ////


         // Let's check what kind of station this is
      if( confReader.getValueAsBoolean( "masterStation", station ) )
      {
         master = source;
      }
      else
      {
         if( confReader.getValueAsBoolean( "roverStation", station ) )
         {
            rover = source;
         }
         else
         {
            if( confReader.getValueAsBoolean( "refStation", station ) )
            {
                  // Note that 'reference' stations form a set
               refStationSet.insert( source );
            }
         }
      }

         // Close current Rinex observation stream
      rin.close();


         // If we printed the model, we must close the file
      if ( printmodel )
      {
            // Close model file for this station
         modelfile.close();
      }


         // We are done with this station. Let's show a message
      cerr << "Processing finished for station: '" << station;
      if ( printmodel )
      {
         cerr << "'. Model in file: '" << modelName;
      }
      cerr << "'." << endl;


   }  // End of 'while ( (station = confReader.getEachSection()) != "" )'


   //// End of preprocessing for ALL stations ////


      // Clear content of SP3 ephemerides object
   SP3EphList.clear();


      // The rest of the processing will be in method 'clksmpc::shutDown()'
   return;

}  // End of 'clksmpc::process()'



   // Method to print model values
void clksmpc::printModel( ofstream& modelfile,
                         const gnssRinex& gData,
                         int   precision )
{

      // Prepare for printing
   modelfile << fixed << setprecision( precision );

      // Get epoch out of GDS
   CommonTime time(gData.header.epoch);

      // Iterate through the GNSS Data Structure
   for ( satTypeValueMap::const_iterator it = gData.body.begin();
         it!= gData.body.end();
         it++ )
   {

         // Print epoch
      modelfile << static_cast<YDSTime>(time).year         << "  ";    // Year           #1
      modelfile << static_cast<YDSTime>(time).doy          << "  ";    // DayOfYear      #2
      modelfile << static_cast<YDSTime>(time).sod   << "  ";    // SecondsOfDay   #3

         // Print satellite information (Satellite system and ID number)
      modelfile << (*it).first << " ";             // System         #4
                                                   // ID number      #5

         // Print model values
      for( typeValueMap::const_iterator itObs  = (*it).second.begin();
           itObs != (*it).second.end();
           itObs++ )
      {
            // Print type names and values
         modelfile << (*itObs).first << " ";
         modelfile << (*itObs).second << " ";

      }  // End of 'for( typeValueMap::const_iterator itObs = ...'

      modelfile << endl;

   }  // End for (it = gData.body.begin(); ... )

}  // End of method 'clksmpc::printModel()'



   // Method to generate and solve the general equation system
void clksmpc::solve()
{

   ////> In the resolution part we start configuring the general solver 

      // Declare stochastic models to be used
      // The coordinates are handled as white noise with 100 m of sigma
   WhiteNoiseModel satClockModel;
   TropoRandomWalkModel tropoModel;

   Variable cdt( TypeID::cdt );
   Variable tropo( TypeID::wetMap, &tropoModel, true, false, 10.0  );
   Variable satClock( TypeID::dtSat, &satClockModel, false, true );

      // This will be the independent term for code equations
   Variable prefitC( TypeID::prefitC );

      // Reference stations code equation description
   Equation equPCRef( prefitC );
   equPCRef.addVariable(cdt, true);
   equPCRef.addVariable(tropo);
   equPCRef.addVariable(satClock, true, -1.0);

      // Set the source of the equation
   equPCRef.header.equationSource = Variable::someSources;

      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equPCRef.addSource2Set( (*itSet) );
   }

      // Master station code equation description
   Equation equPCMaster( prefitC );
   equPCMaster.addVariable(tropo);
   equPCMaster.addVariable(satClock, true, -1.0);

      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Setup equation system
   EquationSystem2 system;
   system.addEquation(equPCRef);
   system.addEquation(equPCMaster);

      // Forward-backward solver
   SolverGeneral2 solver(system);

      // Get if we want 'forwards-backwards' or 'forwards' processing only
   bool cycles( confReader.getValueAsInt("forwardBackwardCycles", "DEFAULT") );

   ////> Variables for the print out 

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( precision );

   ////> Now, Let's solve the equation system 

      // Repeat while there is preprocesed data available
   while( !gdsMap.empty() )
   {

         //*Solve the system equation 

         // Get data out of GDS, but only the first epoch
      gnssDataMap gds( gdsMap.frontEpoch() );

         // Remove first element (i.e., we remove the first epoch)
      gdsMap.pop_front_epoch();

         // Extract current epoch
      CommonTime workEpoch( (*gds.begin()).first );

         // Compute the solution using solverFb. This is it!!!
      solver.Process( gds );

         // Extract solutions from the solver and store it into string
      getClockSoluts(solver, workEpoch);

   }  // End of 'while( !gdsMap.empty() )'

   

   /// We are done ////

}  // End of 'clksmpc::solve()'


   // Method to extract the solutions from solver and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printClockData'
void clksmpc::getClockSoluts( const SolverGeneral2& solver,
                             const CommonTime& workEpoch )
{
      // Mulitmap used to store the clock data line 
   std::string solutionRecord;

      // Satellite sets for all the estimated satellite clocks
   SatIDSet currentSatSet;

      // Source sets for all the estimated receiver clocks
   SourceIDSet currentSourceSet;

      // Clock bias
   double clockBias; 

      //*Now, read and print the receiver clock data record
      
      // Read receiver clocks from the stateMap
   currentSourceSet = solver.getCurrentSources();

      // Insert the current sources into sourceSet
   sourceSet.insert(currentSourceSet.begin(), currentSourceSet.end());

      // Read the receiver clocks for the current sources
   for(SourceIDSet::iterator itSource = currentSourceSet.begin();
       itSource != currentSourceSet.end();
       ++itSource)
   {
         // receiver clock bias
      try
      {
         clockBias = solver.getSolution(TypeID::cdt, *itSource)/C_MPS;
      }
      catch(InvalidRequest& e)
      {
         continue;
      }

         // Store the clock data record into solutionRecord
      solutionRecord  = "AR";  // record flag
      solutionRecord += string(1,' '); // parse space
      solutionRecord += rightJustify((*itSource).sourceName,4); // Source name
      solutionRecord += string(1,' '); // parse space
      solutionRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      solutionRecord += rightJustify(asString(1),3); // clock data number
      solutionRecord += string(3,' '); // parse space
      solutionRecord += rightJustify(doub2sci(clockBias, 19, 2), 19); // clock bias
      solutionRecord += string(1,' ');

        // Store the data record line into 'solutionMap'
      solutionMap.insert(make_pair(workEpoch, solutionRecord));

   }

      //*Now, read and print the receiver clock data record
      
      // Read satellite clocks from the stateMap
   currentSatSet = solver.getCurrentSats();
      
      // Insert current satellites into satSet
   satSet.insert(currentSatSet.begin(), currentSatSet.end());

      // Read the receiver clocks for the current sources
   for(SatIDSet::iterator itSat = currentSatSet.begin();
       itSat != currentSatSet.end();
       ++itSat)
   {
         // Satellite clock bias
      try
      {
         clockBias = solver.getSolution(TypeID::dtSat, *itSat)/C_MPS;
      }
      catch(InvalidRequest& e)
      {
         continue;
      }

         // Satellite ID for rinex
      RinexSatID satID(*itSat);
      satID.setfill('0'); // set the fill char for output

         // Store the clock data record into solutionRecord
      solutionRecord  = "AS";  // record flag
      solutionRecord += string(1,' '); // parse space
      solutionRecord += leftJustify(satID.toString(),4); // Source name
      solutionRecord += string(1,' '); // parse space
      solutionRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      solutionRecord += rightJustify(asString(1),3); // clock data number
      solutionRecord += string(3,' '); // parse space
      solutionRecord += rightJustify(doub2sci(clockBias, 19, 2), 19); // clock bias
      solutionRecord += string(1,' ');

        // Store the data record line into 'solutionMap'
      solutionMap.insert(make_pair(workEpoch, solutionRecord));

   }

   multimap<CommonTime,string>::iterator begPos = 
                                      solutionMap.lower_bound(workEpoch);
   multimap<CommonTime,string>::iterator endPos = 
                                      solutionMap.upper_bound(workEpoch);

   while(begPos != endPos)
   {
       cout << begPos->second << endl;
       ++ begPos;
   }

}  // End of method 'clksmpc::getClockSoluts()'


   // Method that hold code to be run AFTER processing
void clksmpc::shutDown()
{
      // Solution file
   string solutionName;
   solutionName = confReader.getValue( "clockFile", "DEFAULT" );

      // Solution output stream
   ofstream clockFile;
   clockFile.open( solutionName.c_str(), ios::out );

      // Print the header information
   printClockHeader( clockFile );

      // print the clock products as the IGS format
   printClockData( clockFile );

      // close the solution file stream
   clockFile.close();


}  // End of 'clksmpc::shutDown()'


   // Method to print clock data 
void clksmpc::printClockHeader( ofstream& clockFile)
{
   using namespace StringUtils;

      // Define the header formatting strings
   const string versionString         = "RINEX VERSION / TYPE";
   const string runByString           = "PGM / RUN BY / DATE";
   const string commentString         = "COMMENT";
   const string sysString             = "SYS / # / OBS TYPES";
   const string timeSystemString      = "TIME SYSTEM ID";
   const string leapSecondsString     = "LEAP SECONDS";
   const string sysDCBString          = "SYS / DCBS APPLIED";
   const string sysPCVString          = "SYS / PCVS APPLIED";
   const string numDataString         = "# / TYPES OF DATA";
   const string stationNameString     = "STATION NAME / NUM";
   const string stationClockRefString = "STATION CLK REF";
   const string analysisCenterString  = "ANALYSIS CENTER";
   const string numClockRefString     = "# OF CLK REF";
   const string analysisClkRefrString = "ANALYSIS CLK REF";
   const string numReceiversString    = "# OF SOLN STA / TRF";
   const string solnStateString       = "SOLN STA NAME / NUM";
   const string numSolnSatsString     = "# OF SOLN SATS";
   const string prnListString         = "PRN LIST";
   const string endOfHeaderString     = "END OF HEADER";

      // header line record
   string headerRecord;
   
      // "RINEX VERSION / TYPE"
   double version(2.0);
   headerRecord  = rightJustify(asString(version,2), 9);
   headerRecord += string(11,' ');
   headerRecord += string("CLOCK") + string(15,' ');
   headerRecord += string("GPS") + string(17,' ');      // TD fix
   headerRecord += versionString;         
   clockFile << headerRecord << endl;

      // "PGM / RUN BY / DATE"
      //  Varialbes
   string program ("clksmpc");
   string runby ("SGG");
   Epoch dt;
   dt.setLocalTime();
   string dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
      //  print out
   headerRecord  = leftJustify(program,20);
   headerRecord += leftJustify(runby,20);
   headerRecord += leftJustify(dat, 20);
   headerRecord += runByString;          
   clockFile << headerRecord << endl;


      // "TIME SYSTEM ID"
   string timeSystem("GPS");
   headerRecord  = string(3,' ');  // TD
   headerRecord += leftJustify(timeSystem,57);     
   headerRecord += timeSystemString;     
   clockFile << headerRecord << endl;

      // "COMMENT"
   std::vector<std::string> commentList;   
   for(int i=0; i<commentList.size(); ++i) 
   {
      headerRecord  = leftJustify(commentList[i],60);
      headerRecord += commentString;         // "COMMENT"
      clockFile<< headerRecord << endl;
   }

/*    // "LEAP SECONDS"
 * int leapSeconds;
 * headerRecord  = rightJustify(asString(leapSeconds), 6);
 * headerRecord += string(54,' ');
 * headerRecord += leapSecondsString;     // "LEAP SECONDS"
 * clockFile<< headerRecord << endl;
 */

      // "SYS / DCBS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify(dcbProgram,17) + string(1, ' ');
   headerRecord += leftJustify(dcbSource,40);
   headerRecord += sysDCBString;          // "SYS / DCBS APPLIED"
   clockFile<< headerRecord << endl;

      // "SYS / PCVS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify("clksmpc",17) + string(1, ' ');
   headerRecord += leftJustify(antexFile, 40);
   headerRecord += sysPCVString;          // "SYS / PCVS APPLIED"
   clockFile<< headerRecord << endl;

     // "# / TYPES OF DATA"
   std::vector<std::string> dataTypes;
   dataTypes.push_back("AR");
   dataTypes.push_back("AS");

   headerRecord  = rightJustify(asString(dataTypes.size()), 6);
   for(int i=0; i<dataTypes.size(); ++i) headerRecord += string(4,' ') + dataTypes[i];
   headerRecord += string(60-headerRecord.size(),' ');
   headerRecord += numDataString;         // "# / TYPES OF DATA"
   clockFile<< headerRecord << endl;

      // "ANALYSIS CENTER"
   std::string analCenterDesignator("WHU");
   std::string analysisCenter("Wuhan University");
      // print out 
   headerRecord  = analCenterDesignator;
   headerRecord += string(2,' ');
   headerRecord += leftJustify(analysisCenter,55);
   headerRecord += analysisCenterString;  // "ANALYSIS CENTER"
   clockFile << headerRecord << endl;

       // "# OF CLK REF"
   int numClkRef(1);
   headerRecord  = rightJustify(asString(numClkRef), 6) + string(54,' ');  // TD
   headerRecord += numClockRefString;     // "# OF CLK REF"
   clockFile << headerRecord << endl;

      // "ANALYSIS CLK REF"
   headerRecord  = leftJustify((master.sourceName),4) + string(1,' ');  // TD
   headerRecord += leftJustify((master.sourceNumber),20) + string(15,' ');
   headerRecord += rightJustify(doub2sci(0.0,19,2), 19) + string(1,' ');
   headerRecord += analysisClkRefrString; // "ANALYSIS CLK REF"
   clockFile << headerRecord << endl;

        // "# OF SOLN STA / TRF"
   string terrRefFrame("IGS05");
   headerRecord  = rightJustify(asString(sourceSet.size()), 6);
   headerRecord += string(4,' ');
   headerRecord += leftJustify(terrRefFrame,50);
   headerRecord += numReceiversString;    // "# OF SOLN STA / TRF"
   clockFile << headerRecord << endl;
  
        // "SOLN STA NAME / NUM"
   for(SourceIDSet::iterator itSource = sourceSet.begin(); 
       itSource != sourceSet.end(); 
       ++itSource) 
   {
         // sourcename and sourceNumber number
      headerRecord  = leftJustify((*itSource).sourceName,  4) + string( 1, ' ');
      headerRecord += leftJustify((*itSource).sourceNumber,20) ;
  
         // coordinate
      double coordX = floor(1000.0*(*itSource).nominalPos.X());
      double coordY = floor(1000.0*(*itSource).nominalPos.Y());
      double coordZ = floor(1000.0*(*itSource).nominalPos.Z());
  
         // print Coordiante 
      headerRecord += rightJustify(asString(coordX, 0), 11);
      headerRecord += string(1,' ');
      headerRecord += rightJustify(asString(coordY, 0), 11);
      headerRecord += string(1,' ');
      headerRecord += rightJustify(asString(coordZ, 0), 11);
  
         // print the line
      headerRecord += solnStateString;       // "SOLN STA NAME / NUM"
      clockFile << headerRecord << endl;
   }
  
      // "# OF SOLN SATS"
   headerRecord  = rightJustify(asString(satSet.size()), 6);
   headerRecord += string(54,' ');
   headerRecord += numSolnSatsString;     // "# OF SOLN SATS"
   clockFile << headerRecord << endl;
  
      // "PRN LIST"
   headerRecord = string();
   int i = 0;
   for(SatIDSet::iterator itSat = satSet.begin();
       itSat != satSet.end();
       ++itSat)
   {  
         // Satellite ID for rinex
      RinexSatID satID(*itSat);
      satID.setfill('0'); // set the fill char for output
  
         // Add each PRN satellite ...
      headerRecord += rightJustify(satID.toString(), 3) + string(1,' ');
         // End of this line
      if(((i+1) % 15) == 0 || (i==satSet.size()-1) ) 
      {
           // Add empty to the remaiding places
         headerRecord += string(60-headerRecord.size(),' ');
         headerRecord += prnListString;         // "PRN LIST"
         clockFile << headerRecord << endl;
            // New line  
         headerRecord  = string();
      }
        // increment for the satellite number
      i = i+1;
   }

      // "END OF HEADER"
   headerRecord = string(60,' ');
   headerRecord+= endOfHeaderString;     // "END OF HEADER"
   clockFile << headerRecord << endl;

}  // End of printClockHeader


   // Method to print clock data 
void clksmpc::printClockData( ofstream& clockFile)
{
      // Time tolerance  
   double tolerance(0.1);

      // Check if the structure isn't empty  
   while( !( solutionMap.empty() ) )
   {
         // Get the 'CommonTime' of the first element 
      CommonTime firstEpoch( (*solutionMap.begin()).first );

         // Find the position of the first element PAST
      multimap<CommonTime,string>::iterator endPos( 
                                solutionMap.upper_bound(firstEpoch+tolerance) );
         // Remove values
      for( multimap<CommonTime,string>::iterator pos = solutionMap.begin();
           pos != endPos; )
      {
      
         clockFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         solutionMap.erase( pos++ );
      }
   }   // End of loop ' while( !( solutionMap.empty() ) ) '


}  // End of method 'clksmpc::printClockData()'



   // Main function
int main(int argc, char* argv[])
{
   try
   {
      clksmpc program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if ( !program.initialize(argc, argv, false) )
      {
         return 0;
      }
         // Run program
      if ( !program.run() )
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
