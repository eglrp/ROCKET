#pragma ident "$Id$"

//============================================================================
//
//  Function
//
//  Estimatinng clock and upds for GPS satellites and stations.
//
//  CopyRight
//  
//  Shoujian Zhang, Wuhan University, 2010, 2011 
//
//============================================================================
//
//  Revision
//
//  2015/07/25  This clk and upd estimation combines the undifferenced and
//              epoch-differenced method together to solve real-time clk 
//              and upds.
//              please refer to the paper to see the formula
//
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

   // Class to read configuration files.
#include "ConfDataReader.hpp"

#include "EpochDiffOp.hpp"

   // Class to compute the elevation weights
#include "ComputeElevWeights.hpp"

   // Class to deal with some string input/output
#include "StringUtils.hpp"

   // Class to deal with time string
#include "TimeString.hpp"

   // Class to deal with civil time
#include "CivilTime.hpp"

   // Class to decimate data.
#include "Decimate.hpp"

   // Class to time conversion between different time system
#include "Epoch.hpp"

   // Class to deal with the ambiguity constraints for solver
#include "AmbiguityDatum.hpp"

   // Class to deal with the ambiguity constraints for solver
#include "IndepAmbiguityDatum.hpp"

   // Class to smooth the ambiguities
#include "AmbiguitySmoother.hpp"

   // Class to solve a general equation.
#include "SolverZapWL.hpp"

   // Class to solve a general equation.
#include "SolverGenNL.hpp"

   // Class to solve a general equation.
#include "PrefitFilter.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clkzapdiff: public BasicFramework
{
public:

      // Constructor declaration
   clkzapdiff(char* arg0);


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
   std::multimap<CommonTime, std::string> updSolMap;

      // Mulitmap used to store the clock data line 
   std::multimap<CommonTime, std::string> clkSolMap;

      // Mulitmap used to store the fixed upd record
   std::multimap<CommonTime, std::string> updFixedMap;

      // Mulitmap used to store the fixed clk record
   std::multimap<CommonTime, std::string> clkFixedMap;

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


      // Method to get clock bias products
   void getBiases( const CommonTime& workEpoch,
                   const SolverZapWL& solverUpdWL,
                   const SolverGenNL& solverUpdNL);


      // Method to print clock header 
   void printHeader( ofstream& outFile);


      // Method to print clock data 
   void printBiasData( ofstream& updFile );
   void printClockData( ofstream& clkFile );

      // Method to print clock data 
   void printUpdFixedData( ofstream& updFile );
   void printClockFixedData( ofstream& clkFile );


}; // End of 'clkzapdiff' class declaration



   // Let's implement constructor details
clkzapdiff::clkzapdiff(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data to determine the UPDs.\n\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('clkzapdiff.conf' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clkzapdiff::clkzapdiff'



   // Method that will be executed AFTER initialization but BEFORE processing
void clkzapdiff::spinUp()
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
         confReader.open( "clkzapdiff.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'clkzapdiff.conf'"
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


}  // End of method 'clkzapdiff::spinUp()'



   // Method that will really process information
void clkzapdiff::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   solve();
}



   // Method that will really process information
void clkzapdiff::preprocessing()
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
      cout << "Starting processing for station: '" << station << "'." << endl;


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


         // Object to remove eclipsed satellites, has been removed by shjzhang
         // For that the removement of the observations, will certainly cause
         // the re-initialization of the clock products


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


         // Object to compute code combination with minus ionospheric delays
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

         // Now, Compute the new MWubbena combinations
      ComputeLinear linear5;
      if(usingC1)
      {
         linear5.addLinear(comb.mwubbenaCombWithC1);
      }
      else
      {
         linear5.addLinear(comb.mwubbenaCombination);
      }
      pList.push_back(linear5);       // Add to processing list

         // Object to compute prefit-residuals for clock estimation
         // Warning: Modified by Shoujian Zhang, to determine the exact
         //          satellite clocks 'dtSat' directly, instead of the 
         //          clock correction delta('dtSat') 
      ComputeLinear linear6(comb.pcPrefitC);
      linear6.addLinear(comb.lcPrefitC);
      pList.push_back(linear6);       // Add to processing list


         // Object to compute the time-difference of the prefitC/prefitL
      EpochDiffOp epochDiffOp;
      epochDiffOp.setDiffType(TypeID::prefitC);
      epochDiffOp.addDiffType(TypeID::prefitL);
      epochDiffOp.addDiffType(TypeID::wetMap);

         //>Now, compute the BLC with 'SolverPPP'

         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;

         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision", station ) );

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

            // Only keep the necessary types for 'SolverGeneralFB' 
         TypeIDSet types;
         types.insert(TypeID::weight);
         types.insert(TypeID::wetMap);
         types.insert(TypeID::elevation);
         types.insert(TypeID::satArc);
         types.insert(TypeID::prefitC);
         types.insert(TypeID::prefitL);
         types.insert(TypeID::MWubbena);
         types.insert(TypeID::diffWetMap);
         types.insert(TypeID::diffPrefitC);
         types.insert(TypeID::diffPrefitL);

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


      // The rest of the processing will be in method 'clkzapdiff::shutDown()'
   return;

}  // End of 'clkzapdiff::process()'


   // Method to print model values
void clkzapdiff::printModel( ofstream& modelfile,
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

}  // End of method 'clkzapdiff::printModel()'



   // Method to generate and solve the general equation system
void clkzapdiff::solve()
{

   ////> In the resolution part we start configuring the general solver 
   
   ///////////////////////////////////////////////////////////////
   //
   //  Firstly, Let's define the solver for the epoch-difference 
   //  clock estimation using pc and lc observables.
   //
   ///////////////////////////////////////////////////////////////
   
      // Declare variables for the equations
  
   WhiteNoiseModel recDiffClkModel;
   Variable cdtDiff( TypeID::cdt, &recDiffClkModel, true, false);
  
   WhiteNoiseModel satDiffClockModel;
   Variable satClockDiff( TypeID::dtSat, &satDiffClockModel, false, true );
  
   TropoRandomWalkModel tropoDiffModel;
   Variable tropoDiff( TypeID::diffWetMap, &tropoDiffModel, true, false, 10.0  );
  
      //
      // PC equation
      //

      // This will be the independent term for code equations
   Variable diffPrefitC( TypeID::diffPrefitC );
  
      // Reference stations code equation description
   Equation equPCRef( diffPrefitC );
   equPCRef.addVariable(cdtDiff, true);
   equPCRef.addVariable(satClockDiff, true, -1.0);
   equPCRef.addVariable(tropoDiff);

      // Set the source of the equation
   equPCRef.header.equationSource = Variable::someSources;
  
      //
      // LC equation
      //

      // This will be the independent term for phase equations
   Variable diffPrefitL( TypeID::diffPrefitL );

      // Reference stations phase equation description
   Equation equLCRef( diffPrefitL );
   equLCRef.addVariable(cdtDiff, true); // default coefficient (1.0)
   equLCRef.addVariable(satClockDiff, true, -1.0); // default coefficient(-1.0)
   equLCRef.addVariable(tropoDiff);
  
      // Reference station phase equation has more weight
   equLCRef.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCRef.header.equationSource = Variable::someSources;

      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equPCRef.addSource2Set( (*itSet) );
      equLCRef.addSource2Set( (*itSet) );
   }


      // Master station code equation description
   Equation equPCMaster( diffPrefitC );
   equPCMaster.addVariable(satClockDiff, true, -1.0);
   equPCMaster.addVariable(tropoDiff);
  
      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Master station phase equation description
   Equation equLCMaster( diffPrefitL );
   equLCMaster.addVariable(satClockDiff, true, -1.0);
   equLCMaster.addVariable(tropoDiff);

      // Master station phase equation has more weight
   equLCMaster.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCMaster.header.equationSource = master;


      // Setup equation system
   EquationSystem2 system2;
   system2.addEquation(equPCRef);
   system2.addEquation(equLCRef);
   system2.addEquation(equPCMaster);
   system2.addEquation(equLCMaster);
  
  /////////////////////////
   
      // Forward-backward solver
   SolverGeneral2 solverClkDiff(system2);


   /////////////////////////////////////////////
   //
   //  Define the widelane ambiguity fixing equations
   //
   /////////////////////////////////////////////
   
      // SourceID-indexed variables

      // MW Receiver bias variable
   RecBiasRandomWalkModel updMWModel;
   Variable updMW( TypeID::updMW, &updMWModel, true, false, 3.0e10 );

      // SourceID and SatID-indexed variables
     
      // Ambiguity variable
   PhaseAmbiguityModel ambWLModel;
   Variable ambWL( TypeID::BWL, &ambWLModel, true, true, 4.e10 );

      // SatID-indexed only variables

      // MW Receiver bias variable
   SatBiasRandomWalkModel updSatMWModel;
   updSatMWModel.setQprime(3.0e-3); // 3.0e-3*30 =(0.3)^2
   Variable updSatMW( TypeID::updSatMW, &updSatMWModel, false, true, 5.0e10 );

      //>Equation types to be used

      // This will be the independent term for phase equations
   Variable MW( TypeID::MWubbena);

   ////> Define all equations, which will be used in the solver

      // Reference stations Melboune-Wubbena equation description
   Equation equMWRef( MW );
   equMWRef.addVariable(updMW, true); // coefficient (1.0)
   equMWRef.addVariable(updSatMW, true, -1.0); // coefficient(-1.0)
   equMWRef.addVariable(ambWL, true, -0.861918400322); // coefficient (-0.8619...)

      // If the unit sigma is 1m, then the sigma of MW is about 0.2 m
      // You should be notice that the MWubbena is smoothed by the given
      // moving window size, usually 10min, and if the sampling interval is 30s,
      // then the sigma is about 0.2/sqrt(10) = 0.0667 cm
      //
      // 2014/11/14, set weight to 1.0/0.2 = 25
   equMWRef.setWeight(25.0);     // 5.0 * 5.0
      // Set the source of the equation
   equMWRef.header.equationSource = Variable::someSources;

      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equMWRef.addSource2Set( (*itSet) );
   }

      // Master station Melbourne-Wubbena equation description
   Equation equMWMaster( MW );
   equMWMaster.addVariable(updSatMW, true, -1.0); // coefficient(-1.0)
   equMWMaster.addVariable(ambWL, true, -0.861918400); // coefficient (0.8619...)
      // Master station phase equation has more weight
   equMWMaster.setWeight(25.0);     // 15.0 * 15.0
      // Set the source of the equation
   equMWMaster.header.equationSource = master;

   ////> Define the equation system

      // Setup equation system
   EquationSystem2 system1;
   system1.addEquation(equMWRef);
   system1.addEquation(equMWMaster);

      // Forward-backward solver
   SolverZapWL solverUpdWL(system1);

   ////> Define the constraint systems.
   
      // Setup constraint equation system
   AmbiguityDatum ambConstrWL;
   ambConstrWL.setAmbType(ambWL);

      // Feed the 'solverGen' with 'constrSystem'
   solverUpdWL.setAmbiguityDatum(ambConstrWL);
   solverUpdWL.setAmbType(ambWL);

     //////////////////////////////////////////////////////////////
     //
     //  Define PC and LC equations for clock and upd estimation
     //
     //////////////////////////////////////////////////////////////

      // Declare variables for the equations
  
   RecBiasRandomWalkModel initialRecClkModel;
   Variable cdtInitial( TypeID::cdt, &initialRecClkModel, true, false);
  
   SatBiasRandomWalkModel initialSatClockModel;
   Variable satClockInitial( TypeID::dtSat, &initialSatClockModel, false, true );
  
   TropoRandomWalkModel tropoModel;
   Variable tropo( TypeID::wetMap, &tropoModel, true, false, 10.0  );
  
      // LC Receiver bias variable
   RecBiasRandomWalkModel updLCModel;
   Variable updLC( TypeID::updLC, &updLCModel, true, false);
  
   SatBiasRandomWalkModel updSatLCModel;
   Variable updSatLC( TypeID::updSatLC, &updSatLCModel, false, true );
  
   PhaseAmbiguityModel BL1Model;
   Variable BL1( TypeID::BL1, &BL1Model, true, true );
  
      //
      // PC equation
      //

      // This will be the independent term for code equations
   Variable prefitC( TypeID::prefitC );
  
      // Reference stations code equation description
   Equation equPCRef( prefitC );
   equPCRef.addVariable(cdtInitial, true);
   equPCRef.addVariable(satClockInitial, true, -1.0);
   equPCRef.addVariable(updLC, true, -1.0);
   equPCRef.addVariable(updSatLC, true, 1.0);
   equPCRef.addVariable(tropo);

      // Set the source of the equation
   equPCRef.header.equationSource = Variable::someSources;
  
      //
      // LC equation
      //

      // This will be the independent term for phase equations
   Variable prefitL( TypeID::prefitL );

      // Reference stations phase equation description
   Equation equLCRef( prefitL );
   equLCRef.addVariable(cdtInitial, true); // default coefficient (1.0)
   equLCRef.addVariable(satClockInitial, true, -1.0); // default coefficient(-1.0)
   equLCRef.addVariable(tropo);
   equLCRef.addVariable(BL1, true, -0.106953378); 
  
      // Reference station phase equation has more weight
   equLCRef.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCRef.header.equationSource = Variable::someSources;

      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equPCRef.addSource2Set( (*itSet) );
      equLCRef.addSource2Set( (*itSet) );
   }


      // Master station code equation description
   Equation equPCMaster( prefitC );
   equPCMaster.addVariable(satClockInitial, true, -1.0);
   equPCMaster.addVariable(updSatLC, true, +1.0);
   equPCMaster.addVariable(tropo);
  
      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Master station phase equation description
   Equation equLCMaster( prefitL );
   equLCMaster.addVariable(satClockInitial, true, -1.0);
   equLCMaster.addVariable(tropo);
   equLCMaster.addVariable(BL1, true, -0.106953378);

      // Master station phase equation has more weight
   equLCMaster.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCMaster.header.equationSource = master;


      // Setup equation system
   EquationSystem2 system2;
   system2.addEquation(equPCRef);
   system2.addEquation(equLCRef);
   system2.addEquation(equPCMaster);
   system2.addEquation(equLCMaster);
  
  /////////////////////////
   
      // Forward-backward solver
   SolverGenNL solverUpdNL(system2);

   ////> Define the constraint systems.
   
      // Setup constraint equation system
   IndepAmbiguityDatum ambConstrBL1;
   ambConstrBL1.setAmbType(BL1);

      // Feed the 'solverGen' with 'constrSystem'
   solverUpdNL.setAmbiguityDatum(ambConstrBL1);

   ////> Variables for the print out 

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( precision );

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

         //
         // Firstly, let's compute the epoch-difference clocks
         //
         
         // If this is the first epoch, then jump the 'solverDiffClock', 
         // because the epoch-diff values are the difference between
         // the "current" value minus the "previous" values
//    if(Reset)
//    {
//          // Compute the widelane biases by fixing widelane ambiguities
//       solverUpdWL.Process(gds);
//
//          // Compute the widelane biases by fixing widelane ambiguities
//       solverUpdNL.Process(gds);
//
//       epoch0 = workEpoch;
//
//    }
//    else
//    {
//          // Firstly, Let's solve the epoch-difference satellite clocks
//       solverDiffClock.Process(gdsMap);   
//
//          // Let's get epoch-difference clock from 'solverDiffClock'
//       diffSatClockMap = solverDiffClock.getSatDiffClock();
//       diffRecClockMap = solverDiffClock.getRecDiffClock();
//
//          // Now, Let's compute the accumulated clock difference
//          // which is then used to convert the clocks from current epoch
//          // t(i) to the first epoch (t0)
//          // 
//       clockAccumulate.setSatDiffClock(diffSatClockMap);
//       clockAccumulate.setRecDiffClock(diffRecClockMap);
//       clockAccumulate.Process(gdsMap);
//
//          // Now, estimate the initial carrier-phase clock and the upd
//          // using PC and LC after correcting the accumulated diff clocks.
//          // Unknows:
//          // (1) initial carrier-phase satellite clocks
//          // (2) satellite upd at current epoch 
//
//          // Warning: only compute every 5min or 10 min, because the 
//          // initial clocks and the upds are both very stable 
//       solverUpdWL.Process(gds);
//       solverUpdNL.Process(gds);
//
//          // get initial clock 
//       iniSatClockMap = getInitialSatClock(solverUpdNL); 
//       iniRecClockMap = getInitialRecClock(solverUpdNL); 
//          
//          // Now, Let's recover current satellite clocks
//       satClockMap = recoverSatClocks( iniSatClockMap, diffSatClockMap )
//       recClockMap = recoverRecClocks( iniRecClockMap, diffRecClockMap )
//
//          // get initial clock 
//       updSatMap = solverUpdNL.getUpdMap(); 
//       updRecMap = solverUpdNL.getUpdMap(); 
//
//    }
         // Now, Let's print sat/sta clock and sta/sta upd
//    printBiases(workEpoch, satClockMap, recClockMap, updSatMap, updRecMap);


   }  // End of 'while( !gdsMap.empty() )'

   /// We are done ////

}  // End of 'clkzapdiff::solve()'


   // Method to extract the solutions from solverGen and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printBiasData'
void clkzapdiff::getBiases( const CommonTime& workEpoch,
                        const SolverZapWL& solverUpdWL,
                        const SolverGenNL& solverUpdNL)
{
      // Mulitmap used to store the clock data line 
   std::string updRecord;
   std::string clkRecord;
   std::string updRecFixed;
   std::string clkRecFixed;

      // Satellite sets for all the estimated satellite clocks
   SatIDSet currentSatSet;

      // Source sets for all the estimated receiver clocks
   SourceIDSet currentSourceSet;

      // Clock bias
   double updMW; 
   double updLC; 
   double cdt;
   double updLCFixed; 
   double cdtfixed;

      //*Now, read and print the receiver clock data record
      
      // Read receiver clocks from the stateMap
   currentSourceSet = solverUpdWL.getCurrentSources();

      // Insert the current sources into sourceSet
   sourceSet.insert(currentSourceSet.begin(), currentSourceSet.end());

      // Read the receiver clocks for the current sources
   for(SourceIDSet::iterator itSource = currentSourceSet.begin();
       itSource != currentSourceSet.end();
       ++itSource)
   {
         // receiver clock bias and instrumental bias
      try
      {
         updMW = solverUpdWL.getSolution(TypeID::updMW, *itSource);

            // Now, Let's get float values firstly
            // In the SolverGenNL, only the independent ambiguities
            // are constrainted to the equations. 
         updLC = solverUpdNL.getSolution(TypeID::updLC, *itSource);
         cdt   = solverUpdNL.getSolution(TypeID::cdt, *itSource);

            // Now, Let's get fixed values 
            //
         updLCFixed = solverUpdNL.getFixedSolution(TypeID::updLC, *itSource);
         cdtfixed   = solverUpdNL.getFixedSolution(TypeID::cdt, *itSource);

      }
      catch(InvalidRequest& e)
      {
         continue;
      }
         // Convert from meter to second;
      cdt  = cdt/C_MPS;
      cdtfixed = cdtfixed/C_MPS;

         // Store the upd data record into updRecord 
      updRecord  = "AR";  // record flag
      updRecord += string(1,' '); // parse space
      updRecord += rightJustify((*itSource).sourceName,4); // Source name
      updRecord += string(1,' '); // parse space
      updRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      updRecord += rightJustify(asString(2),3); // clock data number
      updRecord += string(3,' '); // parse space
      updRecord += rightJustify(asString(updLC, 3), 8); // instrument bias LC
      updRecord += string(1,' ');
      updRecord += rightJustify(asString(updMW, 3), 8); // instrument bias MW

        // Store the data record line into 'updSolMap'
      updSolMap.insert(make_pair(workEpoch, updRecord));

         // Store the clock data record into clkRecord 
      clkRecord  = "AR";  // record flag
      clkRecord += string(1,' '); // parse space
      clkRecord += rightJustify((*itSource).sourceName,4); // Source name
      clkRecord += string(1,' '); // parse space
      clkRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      clkRecord += rightJustify(asString(1),3); // clock data number
      clkRecord += string(3,' '); // parse space
      clkRecord += rightJustify(doub2sci(cdt, 19, 2), 19); // clock bias

        // Store the data record line into 'clkSolMap'
      clkSolMap.insert(make_pair(workEpoch, clkRecord));


         // Store the upd data record into updRecFixed 
      updRecFixed  = "AR";  // record flag
      updRecFixed += string(1,' '); // parse space
      updRecFixed += rightJustify((*itSource).sourceName,4); // Source name
      updRecFixed += string(1,' '); // parse space
      updRecFixed += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      updRecFixed += rightJustify(asString(2),3); // clock data number
      updRecFixed += string(3,' '); // parse space
      updRecFixed += rightJustify(asString(updLCFixed, 3), 8); // instrument bias LC
      updRecFixed += string(1,' ');
      updRecFixed += rightJustify(asString(updMW, 3), 8); // instrument bias MW

        // Store the data record line into 'updSolMap'
      updFixedMap.insert(make_pair(workEpoch, updRecFixed));

         // Store the clock data record into clkRecFixed
      clkRecFixed  = "AR";  // record flag
      clkRecFixed += string(1,' '); // parse space
      clkRecFixed += rightJustify((*itSource).sourceName,4); // Source name
      clkRecFixed += string(1,' '); // parse space
      clkRecFixed += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      clkRecFixed += rightJustify(asString(1),3); // clock data number
      clkRecFixed += string(3,' '); // parse space
      clkRecFixed += rightJustify(doub2sci(cdtfixed, 19, 2), 19); // clock bias

        // Store the data record line into 'clkSolMap'
      clkFixedMap.insert(make_pair(workEpoch, clkRecFixed));

   }

      //*Now, read and print the receiver clock data record
   double updSatMW; 
   double updSatLC; 
   double dtSat;
   double updSatLCFixed; 
   double dtSatFixed;
      
      // Read satellite clocks from the stateMap
   currentSatSet = solverUpdWL.getCurrentSats();
      
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
         updSatMW = solverUpdWL.getSolution(TypeID::updSatMW, *itSat);
         updSatLC = solverUpdNL.getSolution(TypeID::updSatLC, *itSat);
         dtSat    = solverUpdNL.getSolution(TypeID::dtSat, *itSat);

         updSatLCFixed = solverUpdNL.getFixedSolution(TypeID::updSatLC, *itSat);
         dtSatFixed    = solverUpdNL.getFixedSolution(TypeID::dtSat, *itSat);
      }
      catch(InvalidRequest& e)
      {
         continue;
      }
         // Convert from meter to second;
      dtSat = dtSat/C_MPS;
      dtSatFixed = dtSatFixed/C_MPS;

         // Satellite ID for rinex
      RinexSatID satID(*itSat);
      satID.setfill('0'); // set the fill char for output

         // Store the upd data record into updRecord 
      updRecord  = "AS";  // record flag
      updRecord += string(1,' '); // parse space
      updRecord += leftJustify(satID.toString(),4); // Source name
      updRecord += string(1,' '); // parse space
      updRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      updRecord += rightJustify(asString(2),3); // clock data number
      updRecord += string(3,' '); // parse space
      updRecord += rightJustify(asString(updSatLC, 3), 8); // instrument bias LC
      updRecord += string(1,' ');
      updRecord += rightJustify(asString(updSatMW, 3), 8); // instrument bias MW

        // Store the data record line into 'updSolMap'
      updSolMap.insert(make_pair(workEpoch, updRecord));

         // Store the clock data record into clkRecord 
      clkRecord  = "AS";  // record flag
      clkRecord += string(1,' '); // parse space
      clkRecord += leftJustify(satID.toString(),4); // Source name
      clkRecord += string(1,' '); // parse space
      clkRecord += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      clkRecord += rightJustify(asString(1),3); // clock data number
      clkRecord += string(3,' '); // parse space
      clkRecord += rightJustify(doub2sci(dtSat, 19, 2), 19); // clock bias

        // Store the data record line into 'clkSolMap'
      clkSolMap.insert(make_pair(workEpoch, clkRecord));


         // Store the upd data record into updRecFixed
      updRecFixed  = "AS";  // record flag
      updRecFixed += string(1,' '); // parse space
      updRecFixed += leftJustify(satID.toString(),4); // Source name
      updRecFixed += string(1,' '); // parse space
      updRecFixed += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      updRecFixed += rightJustify(asString(2),3); // clock data number
      updRecFixed += string(3,' '); // parse space
      updRecFixed += rightJustify(asString(updSatLCFixed, 3), 8); // bias LC
      updRecFixed += string(1,' ');
      updRecFixed += rightJustify(asString(updSatMW, 3), 8); // instrument bias MW

        // Store the data record line into 'updSolMap'
      updFixedMap.insert(make_pair(workEpoch, updRecFixed));

         // Store the clock data record into clkRecFixed 
      clkRecFixed  = "AS";  // record flag
      clkRecFixed += string(1,' '); // parse space
      clkRecFixed += leftJustify(satID.toString(),4); // Source name
      clkRecFixed += string(1,' '); // parse space
      clkRecFixed += printTime(workEpoch,"%4Y %02m %02d %02H %02M %9.6f"); // Time 
      clkRecFixed += rightJustify(asString(1),3); // clock data number
      clkRecFixed += string(3,' '); // parse space
      clkRecFixed += rightJustify(doub2sci(dtSatFixed, 19, 2), 19); // clock bias

        // Store the data record line into 'clkSolMap'
      clkFixedMap.insert(make_pair(workEpoch, clkRecFixed));

   }

   multimap<CommonTime,string>::iterator begPos = 
                                      updSolMap.lower_bound(workEpoch);
   multimap<CommonTime,string>::iterator endPos = 
                                      updSolMap.upper_bound(workEpoch);

   while(begPos != endPos)
   {
       cout << begPos->second << endl;
       ++ begPos;
   }

}  // End of method 'clkzapdiff::getBiasesWL()'


   // Method that hold code to be run AFTER processing
void clkzapdiff::shutDown()
{
      // Solution file
   string updFileName;
   string clkFileName;


   updFileName = confReader.getValue( "updFile", "DEFAULT" );
   clkFileName = confReader.getValue( "clockFile", "DEFAULT" );


      // Solution output stream
   ofstream updFile;
   ofstream clkFile;
   updFile.open( updFileName.c_str(), ios::out );
   clkFile.open( clkFileName.c_str(), ios::out );

      // Print the UPD information
   printHeader( updFile );
   printBiasData( updFile );

      // Print clock data
   printHeader( clkFile );
   printClockData( clkFile );


      // close the solution file stream
   updFile.close();
   clkFile.close();


   string updFileName2;
   string clkFileName2;
   updFileName2 = confReader.getValue( "updFixedFile", "DEFAULT" );
   clkFileName2 = confReader.getValue( "clockFixedFile", "DEFAULT" );

   ofstream updFile2;
   ofstream clkFile2;
   updFile2.open( updFileName2.c_str(), ios::out );
   clkFile2.open( clkFileName2.c_str(), ios::out );

      // Print the UPD information
   printHeader( updFile2 );
   printUpdFixedData( updFile2 );

      // Print clock data
   printHeader( clkFile2 );
   printClockFixedData( clkFile2 );

   updFile2.close();
   clkFile2.close();


}  // End of 'clkzapdiff::shutDown()'


   // Method to print clock data 
void clkzapdiff::printHeader( ofstream& outFile)
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
   outFile << headerRecord << endl;

      // "PGM / RUN BY / DATE"
      //  Varialbes
   string program ("clkzapdiff");
   string runby ("SGG");
   Epoch dt;
   dt.setLocalTime();
   string dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
      //  print out
   headerRecord  = leftJustify(program,20);
   headerRecord += leftJustify(runby,20);
   headerRecord += leftJustify(dat, 20);
   headerRecord += runByString;          
   outFile << headerRecord << endl;


      // "TIME SYSTEM ID"
   string timeSystem("GPS");
   headerRecord  = string(3,' ');  // TD
   headerRecord += leftJustify(timeSystem,57);     
   headerRecord += timeSystemString;     
   outFile << headerRecord << endl;

      // "COMMENT"
   std::vector<std::string> commentList;   
   for(int i=0; i<commentList.size(); ++i) 
   {
      headerRecord  = leftJustify(commentList[i],60);
      headerRecord += commentString;         // "COMMENT"
      outFile<< headerRecord << endl;
   }

      // "SYS / DCBS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify(dcbProgram,17) + string(1, ' ');
   headerRecord += leftJustify(dcbSource,40);
   headerRecord += sysDCBString;          // "SYS / DCBS APPLIED"
   outFile<< headerRecord << endl;

      // "SYS / PCVS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify("clkzapdiff",17) + string(1, ' ');
   headerRecord += leftJustify(antexFile, 40);
   headerRecord += sysPCVString;          // "SYS / PCVS APPLIED"
   outFile<< headerRecord << endl;

     // "# / TYPES OF DATA"
   std::vector<std::string> dataTypes;
   dataTypes.push_back("AR");
   dataTypes.push_back("AS");

   headerRecord  = rightJustify(asString(dataTypes.size()), 6);
   for(int i=0; i<dataTypes.size(); ++i) headerRecord += string(4,' ') + dataTypes[i];
   headerRecord += string(60-headerRecord.size(),' ');
   headerRecord += numDataString;         // "# / TYPES OF DATA"
   outFile<< headerRecord << endl;

      // "ANALYSIS CENTER"
   std::string analCenterDesignator("WHU");
   std::string analysisCenter("Wuhan University");
      // print out 
   headerRecord  = analCenterDesignator;
   headerRecord += string(2,' ');
   headerRecord += leftJustify(analysisCenter,55);
   headerRecord += analysisCenterString;  // "ANALYSIS CENTER"
   outFile << headerRecord << endl;

       // "# OF CLK REF"
   int numClkRef(1);
   headerRecord  = rightJustify(asString(numClkRef), 6) + string(54,' ');  // TD
   headerRecord += numClockRefString;     // "# OF CLK REF"
   outFile << headerRecord << endl;

      // "ANALYSIS CLK REF"
   headerRecord  = leftJustify((master.sourceName),4) + string(1,' ');  // TD
   headerRecord += leftJustify((master.sourceNumber),20) + string(15,' ');
   headerRecord += rightJustify(doub2sci(0.0,19,2), 19) + string(1,' ');
   headerRecord += analysisClkRefrString; // "ANALYSIS CLK REF"
   outFile << headerRecord << endl;

        // "# OF SOLN STA / TRF"
   string terrRefFrame("IGS05");
   headerRecord  = rightJustify(asString(sourceSet.size()), 6);
   headerRecord += string(4,' ');
   headerRecord += leftJustify(terrRefFrame,50);
   headerRecord += numReceiversString;    // "# OF SOLN STA / TRF"
   outFile << headerRecord << endl;
  
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
      outFile << headerRecord << endl;
   }
  
      // "# OF SOLN SATS"
   headerRecord  = rightJustify(asString(satSet.size()), 6);
   headerRecord += string(54,' ');
   headerRecord += numSolnSatsString;     // "# OF SOLN SATS"
   outFile << headerRecord << endl;
  
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
         outFile << headerRecord << endl;
            // New line  
         headerRecord  = string();
      }
        // increment for the satellite number
      i = i+1;
   }

      // "END OF HEADER"
   headerRecord = string(60,' ');
   headerRecord+= endOfHeaderString;     // "END OF HEADER"
   outFile << headerRecord << endl;

}  // End of printBiasHeader


   // Method to print clock data 
void clkzapdiff::printBiasData( ofstream& updFile)
{
      // Time tolerance  
   double tolerance(0.1);

      // Check if the structure isn't empty  
   while( !( updSolMap.empty() ) )
   {
         // Get the 'CommonTime' of the first element 
      CommonTime firstEpoch( (*updSolMap.begin()).first );

         // Find the position of the first element PAST
      multimap<CommonTime,string>::iterator endPos( 
                                updSolMap.upper_bound(firstEpoch+tolerance) );
         // Remove values
      for( multimap<CommonTime,string>::iterator pos = updSolMap.begin();
           pos != endPos; )
      {
      
         updFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         updSolMap.erase( pos++ );
      }
   }   // End of loop ' while( !( updSolMap.empty() ) ) '

}  // End of method 'clkzapdiff::printBiasData()'


   // Method to print clock data 
void clkzapdiff::printClockData( ofstream& clkFile)
{
      // Time tolerance  
   double tolerance(0.1);

      // Check if the structure isn't empty  
   while( !( clkSolMap.empty() ) )
   {
         // Get the 'CommonTime' of the first element 
      CommonTime firstEpoch( (*clkSolMap.begin()).first );

         // Find the position of the first element PAST
      multimap<CommonTime,string>::iterator endPos( 
                                clkSolMap.upper_bound(firstEpoch+tolerance) );
         // Remove values
      for( multimap<CommonTime,string>::iterator pos = clkSolMap.begin();
           pos != endPos; )
      {
      
         clkFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         clkSolMap.erase( pos++ );
      }
   }   // End of loop ' while( !( clkSolMap.empty() ) ) '

}  // End of method 'clkzapdiff::printClockData()'


   // Method to print clock data 
void clkzapdiff::printUpdFixedData( ofstream& updFile)
{
      // Time tolerance  
   double tolerance(0.1);

      // Check if the structure isn't empty  
   while( !( updFixedMap.empty() ) )
   {
         // Get the 'CommonTime' of the first element 
      CommonTime firstEpoch( (*updFixedMap.begin()).first );

         // Find the position of the first element PAST
      multimap<CommonTime,string>::iterator endPos( 
                                updFixedMap.upper_bound(firstEpoch+tolerance) );
         // Remove values
      for( multimap<CommonTime,string>::iterator pos = updFixedMap.begin();
           pos != endPos; )
      {
      
         updFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         updFixedMap.erase( pos++ );
      }
   }   // End of loop ' while( !( updFixedMap.empty() ) ) '

}  // End of method 'clkzapdiff::printBiasData()'


   // Method to print clock data 
void clkzapdiff::printClockFixedData( ofstream& clkFile)
{
      // Time tolerance  
   double tolerance(0.1);

      // Check if the structure isn't empty  
   while( !( clkFixedMap.empty() ) )
   {
         // Get the 'CommonTime' of the first element 
      CommonTime firstEpoch( (*clkFixedMap.begin()).first );

         // Find the position of the first element PAST
      multimap<CommonTime,string>::iterator endPos( 
                                clkFixedMap.upper_bound(firstEpoch+tolerance) );
         // Remove values
      for( multimap<CommonTime,string>::iterator pos = clkFixedMap.begin();
           pos != endPos; )
      {
      
         clkFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         clkFixedMap.erase( pos++ );
      }
   }   // End of loop ' while( !( clkFixedMap.empty() ) ) '

}  // End of method 'clkzapdiff::printClockFixedData()'

   // Main function
int main(int argc, char* argv[])
{
   try
   {
      clkzapdiff program(argv[0]);

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
