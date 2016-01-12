#pragma ident "$Id$"

//============================================================================
//
//  Function
//
//  Estimatinng uncalibrated phase delays for GPS satellites and stations.
//
//  CopyRight
//  
//  Shoujian Zhang, Wuhan University, 2010, 2011 
//
//============================================================================
//
//  Revision
//
//  2014/03/18  Get the initial time using 'YDSTime' class. You MUST set the 
//              'TimeSystem' as well, because the time system is set as 'GPS' 
//              for 'gnssRinex'. if you don't set it as 'GPS', you could 
//              operate the time correctly.  
//
//  2014/03/18  Add 'AmbiguitySmoother' to smooth the ambiguities. you can set 
//              the type using the method 'setSmoothedType()'.  
//
//  2014/03/18  'PhaseCodeAlignment' is used to calibrate the phase with code. 
//              The old class 'PhaseCalibration' is deprecated.  
//  
//  2014/11/14  Comment the 'AmbiguitySmoother' for BLC. We will determine the 
//              UPD for LC epoch by epoch, supposing that the upd has the
//              characteristics of white-noise.
//
//  2014/12/02  Comment the 'AmbiguitySmoother' for Widelane ambiguity
//              smoothing. The experiments show that if the smoothed values
//              are used to determine widelane UPD, the fixing rate of PPP with 
//              the 'smoothed' UPDs is lower that that with the 'epoch-by-epoch'
//              UPDs.
//
// 2014/12/09   Repalce the 'SolverGenWL/SolverGenNL' with 'SolverZapWL/SolverZapNL'
//
// 2014/12/09   The UPD model has two different models:
//              MW/LC <==> LW/LC 
//              L1/L2 <==> L1/LW
//              When the P1/P2 are used, the two class are identical.
//              However, when the C1 is used, 
//              MW/LC, LW/LC  </=> L1/L2, L1,LW
//
//              And, the LW/LC and L1, LW will be the two basic models. 
//              
//              If No constraints are appliced to the PPP model, 
//              in theory, the L1/LW will has fast convergence.
//              However, Owing to the spatial and temporal variations of 
//              ionospheric delays and tropospheric delays, the LW/LC and L1/LW 
//              will converge with different speed. 
//
//              Specially, for the sparse PPP-RTK, the model of LW/LC will
//              have faster convergence than the L1/LW, because of the relative
//              small variation of tropospheric delays than ionospheric delays.
//
//              So, The upd/updx are two basic upd estiamtion for diffent models. 
//
// 2015/6/04    fast upd computation method by removing the fixed ambiguities 
//              from the equations.
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

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-only mode.
#include "SolverPPP.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-backwards mode.
#include "SolverPPPFB.hpp"

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

   // Class to decimate data.
#include "Decimate.hpp"

   // Class to time conversion between different time system
#include "Epoch.hpp"

   // Class to deal with the ambiguity constraints for solver
#include "AmbiguityDatum.hpp"

   // Class to deal with the ambiguity constraints for solver
#include "AmbiguityDatum2.hpp"

   // Class to smooth the ambiguities
#include "AmbiguitySmoother.hpp"

   // Class to solve a general equation.
#include "SolverZapWL.hpp"

   // Class to solve a general equation.
#include "SolverZapNL.hpp"

   // Class to solve a general equation.
#include "PrefitFilter.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWFilter.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class upd : public BasicFramework
{
public:

      // Constructor declaration
   upd(char* arg0);


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


      // Method to get clock bias products
   void getBiases( const CommonTime& workEpoch,
                   const SolverZapWL& solverUpdWL,
                   const SolverZapNL& solverUpdNL);


      // Method to print clock header 
   void printBiasHeader( ofstream& solutionFile );


      // Method to print clock data 
   void printBiasData( ofstream& solutionFile );


}; // End of 'upd' class declaration



   // Let's implement constructor details
upd::upd(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data to determine the UPDs.\n\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('upd.conf' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'upd::upd'



   // Method that will be executed AFTER initialization but BEFORE processing
void upd::spinUp()
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
         confReader.open( "upd.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'upd.conf'"
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


}  // End of method 'upd::spinUp()'



   // Method that will really process information
void upd::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   solve();
}



   // Method that will really process information
void upd::preprocessing()
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
            cerr << "rinex clock file '" << rinexClockFile << "' doesn't exist or error"
                 << "occurs in readinng it. Skipping it." << endl;

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

         // Add 'requireObs' to processing list
      pList.push_back(requireObs);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilter pObsFilter;
      pObsFilter.addFilteredType(TypeID::P2);


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

         // Objects to mark cycle slips
      LICSDetector   markCSLI ;         // Checks LI cycle slips
      pList.push_back(markCSLI );       // Add to processing list
      MWCSDetector markCSMW;            // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);        // Add to processing list


         // Object to keep track of satellite arcs
         // Notes: delete unstable satellite may cause discontinuity
         //        in the network processing!
      SatArcMarker2 markArc;
      markArc.setDeleteUnstableSats(false);
      markArc.setUnstablePeriod(151.0); // Delete the starting 5min data
      pList.push_back(markArc);         // Add to processing list




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

         // Object to compute prefit-residuals for PC and LC combination
         // @author Shoujian Zhang
      ComputeLinear linear4(comb.pcPrefit);
      linear4.addLinear(comb.lcPrefit);
      pList.push_back(linear4);       // Add to processing list


         // Object to filter out PC outliers using the prefit-residuals
      PrefitFilter prefitFilter;
      pList.push_back(prefitFilter);

         //>Now, compute the BLC with 'SolverPPP'

         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list
      

         // Get if we want results in ECEF or NEU reference system
      bool isNEU( confReader.getValueAsBoolean( "USENEU", station ) );

         // Declare ppp solver
      SolverPPP pppSolver(isNEU);

         // Add to processing list
      pList.push_back(pppSolver);

         // Now, Smooth the BLC and MWubbena 

         // Declare solver objects to smooth the MWubbena combinations
      MWFilter mwfilter;
      pList.push_back(mwfilter);

      double decimateInterv(
               confReader.getValueAsDouble( "decimationInterval", "DEFAULT" ) );
      double sampleInterv(
               confReader.getValueAsDouble( "sampleInterval", "DEFAULT") );

         // Get the window size for phase ambiguity smoothing
      int windowSize( decimateInterv/sampleInterv );

         // Declare solver objects to smooth the MWubbena combinations
      AmbiguitySmoother smoothMW;
      smoothMW.setSmoothedType( TypeID::MWubbena );
      smoothMW.setWindowSize( windowSize );
      pList.push_back(smoothMW);


         //>Now, decimating the data

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
         types.insert(TypeID::elevation);
         types.insert(TypeID::satArc);
         types.insert(TypeID::BWL);
         types.insert(TypeID::BLC);
         types.insert(TypeID::MWubbena);

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


      // The rest of the processing will be in method 'upd::shutDown()'
   return;

}  // End of 'upd::process()'


   // Method to print model values
void upd::printModel( ofstream& modelfile,
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

}  // End of method 'upd::printModel()'



   // Method to generate and solve the general equation system
void upd::solve()
{

   ////> In the resolution part we start configuring the general solver 
   

   ////> Now, define the widelane ambiguity fixing equations
   
      // SourceID-indexed variables

      // MW Receiver bias variable
   RecBiasRandomWalkModel updMWModel;
   Variable updMW( TypeID::updMW, &updMWModel, true, false, 9.0e10 );

      // SourceID and SatID-indexed variables
     
      // Ambiguity variable
   PhaseAmbiguityModel ambWLModel;
   Variable ambWL( TypeID::BWL, &ambWLModel, true, true );

      // SatID-indexed only variables

   double updSatMWProcessNoise( 
            confReader.getValueAsDouble( "processNoiseUpdSatMW", "DEFAULT" ) );

   double mwWeight( 
            confReader.getValueAsDouble( "weightMW", "DEFAULT" ) );

      // MW Receiver bias variable
   SatBiasRandomWalkModel updSatMWModel;
      // 1.0e-4^600 = 18e-2 = 0.06 cycle/600s
   updSatMWModel.setQprime(updSatMWProcessNoise); // 3.0e-3*30 =(0.3)^2
   Variable updSatMW( TypeID::updSatMW, &updSatMWModel, false, true);

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
   equMWRef.setWeight(mwWeight);     // 5.0 * 5.0
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
   equMWMaster.setWeight(mwWeight);     // 15.0 * 15.0
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
   IndepAmbiguityDatum ambConstrWL;
   ambConstrWL.setAmbType(ambWL);

      // Feed the 'solverGen' with 'constrSystem'
   solverUpdWL.setIndepAmbiguityDatum(ambConstrWL);
   solverUpdWL.setAmbType(ambWL);

   ////
   ////> Now, define the narrowlane ambiguity fixing equations
   ////
   
      // LC Receiver bias variable
   RecBiasRandomWalkModel updLCModel;
   Variable updLC( TypeID::updLC, &updLCModel, true, false, 9.0e10 );

      // SourceID and SatID-indexed variables
     
      // Ambiguity variable
   PhaseAmbiguityModel ambNLModel;
   Variable ambNL( TypeID::BL1, &ambNLModel, true, true );

      // SatID-indexed only variables

      // LC satellite bias variable
   SatBiasRandomWalkModel updSatLCModel;
   Variable updSatLC( TypeID::updSatLC, &updSatLCModel, false, true );

      //>Equation types to be used

      // This will be the independent term for phase equations
      // Warning: 
      // Now the LC value is assigned to the float ambiguities, 
      // which are solved by fixing the satellite clock to 
      // the IGS or CODE precise clock products.
   Variable BLC( TypeID::BLC );

   ////> Define all equations, which will be used in the solver

      // Reference stations phase equation description
   Equation equBLCRef( BLC );
   equBLCRef.addVariable(updLC, true);  // coefficient = 1.0
   equBLCRef.addVariable(updSatLC, true, -1.0); // coefficient(-1.0)
   equBLCRef.addVariable(ambNL, true, -0.106953378); // coefficient = -0.10695...

      // If the unit sigma is 1m, then the sigma of BLC is about 0.01 m
      // if the smooth window size is 10, then the sigma is about 0.01/sqrt(10) = 0.003 cm
   equBLCRef.setWeight(10000.0);     // 333.3 * 333.3
      // Set the source of the equation
   equBLCRef.header.equationSource = Variable::someSources;


      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equBLCRef.addSource2Set( (*itSet) );
   }


      // Master station phase equation description
   Equation equBLCMaster( BLC );
   equBLCMaster.addVariable(updSatLC, true, -1.0); 
   equBLCMaster.addVariable(ambNL, true, -0.106953378);
      // Master station phase equation has more weight
   equBLCMaster.setWeight(10000.0);     // 100.0 * 100.0
      // Set the source of the equation
   equBLCMaster.header.equationSource = master;

   ////> Define the equation system

      // Setup equation system
   EquationSystem2 system2;
   system2.addEquation(equBLCRef);
   system2.addEquation(equBLCMaster);

      // Forward-backward solver
   SolverZapNL solverUpdNL(system2);

   ////> Define the constraint systems.
   
      // Setup constraint equation system
   IndepAmbiguityDatum ambConstrNL;
   ambConstrNL.setAmbType(ambNL);

      // Feed the 'solverGen' with 'constrSystem'
   solverUpdNL.setIndepAmbiguityDatum(ambConstrNL);

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

clock_t start,finish;
double totaltime;
start=clock();

      try
      {
            // Compute the widelane biases by fixing widelane ambiguities
         solverUpdWL.Process(gds);
      }
      catch(ProcessingException& e)
      {
         cerr << "Exception at epoch: " << workEpoch << "; " << e << endl;
         continue;
      }
      catch(...)
      {
         cerr << "Unknown error" << endl;
         continue;
      }

finish=clock();
totaltime=(double)(finish-start)/CLOCKS_PER_SEC;

      int numUnknowns = solverUpdWL.getCurrentUnknownsNumber();
      int numFixed = solverUpdWL.getFixedAmbNumber();

      cout << "WL " 
           << printTime(workEpoch,"%4Y %02m %02d %8s") << " "
           << totaltime << " " 
           << numUnknowns << " " 
           << numFixed << endl;

start=finish;
  
         // Compute the widelane biases by fixing widelane ambiguities
      solverUpdNL.Process(gds);

finish=clock();
totaltime=(double)(finish-start)/CLOCKS_PER_SEC;

      numUnknowns = solverUpdNL.getCurrentUnknownsNumber();
      numFixed = solverUpdNL.getFixedAmbNumber();
         
      cout << "NL " 
           << printTime(workEpoch,"%4Y %02m %02d %8s") << " "
           << totaltime << " " 
           << numUnknowns << " " 
           << numFixed << endl;

         // Get clock-related solution 
      getBiases(workEpoch, solverUpdWL, solverUpdNL);


   }  // End of 'while( !gdsMap.empty() )'

   /// We are done ////

}  // End of 'upd::solve()'


   // Method to extract the solutions from solverGen and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printBiasData'
void upd::getBiases( const CommonTime& workEpoch,
                     const SolverZapWL& solverUpdWL,
                     const SolverZapNL& solverUpdNL)
{
      // Mulitmap used to store the clock data line 
   std::string solutionRecord;

      // Satellite sets for all the estimated satellite clocks
   SatIDSet currentSatSet;

      // Source sets for all the estimated receiver clocks
   SourceIDSet currentSourceSet;

      // Clock bias
   double updLC; 
   double updMW; 

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
         updLC = solverUpdNL.getSolution(TypeID::updLC, *itSource);
         updMW = solverUpdWL.getSolution(TypeID::updMW, *itSource);

         updLC = updLC/0.106953378;
         updMW = updMW/0.861918400;
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
      solutionRecord += rightJustify(asString(2),2); // clock data number
      solutionRecord += string(3,' '); // parse space
      solutionRecord += rightJustify(asString(updLC, 3), 8); // instrument bias LC
      solutionRecord += string(1,' ');
      solutionRecord += rightJustify(asString(updMW, 3), 8); // instrument bias MW
      solutionRecord += string(1,' ');

        // Store the data record line into 'solutionMap'
      solutionMap.insert(make_pair(workEpoch, solutionRecord));

   }

      //*Now, read and print the receiver clock data record
   double updSatMW; 
   double updSatLC; 
      
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
         updSatLC = solverUpdNL.getSolution(TypeID::updSatLC, *itSat);
         updSatMW = solverUpdWL.getSolution(TypeID::updSatMW, *itSat);

         updSatLC = updSatLC/0.106953378;
         updSatMW = updSatMW/0.861918400;
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
      solutionRecord += rightJustify(asString(2),2); // clock data number
      solutionRecord += string(3,' '); // parse space
      solutionRecord += rightJustify(asString(updSatLC, 3), 8); // instrument bias LC
      solutionRecord += string(1,' ');
      solutionRecord += rightJustify(asString(updSatMW, 3), 8); // instrument bias MW
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

}  // End of method 'upd::getBiasesWL()'


   // Method that hold code to be run AFTER processing
void upd::shutDown()
{
      // Solution file
   string solutionName;
   solutionName = confReader.getValue( "solutionFile", "DEFAULT" );

      // Solution output stream
   ofstream solutionFile;
   solutionFile.open( solutionName.c_str(), ios::out );

      // Print the header information
   printBiasHeader( solutionFile );

      // print the clock products as the IGS format
   printBiasData( solutionFile );

      // close the solution file stream
   solutionFile.close();


}  // End of 'upd::shutDown()'


   // Method to print clock data 
void upd::printBiasHeader( ofstream& solutionFile)
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
   solutionFile << headerRecord << endl;

      // "PGM / RUN BY / DATE"
      //  Varialbes
   string program ("upd");
   string runby ("SGG");
   Epoch dt;
   dt.setLocalTime();
   string dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
      //  print out
   headerRecord  = leftJustify(program,20);
   headerRecord += leftJustify(runby,20);
   headerRecord += leftJustify(dat, 20);
   headerRecord += runByString;          
   solutionFile << headerRecord << endl;


      // "TIME SYSTEM ID"
   string timeSystem("GPS");
   headerRecord  = string(3,' ');  // TD
   headerRecord += leftJustify(timeSystem,57);     
   headerRecord += timeSystemString;     
   solutionFile << headerRecord << endl;

      // "COMMENT"
   std::vector<std::string> commentList;   
   for(int i=0; i<commentList.size(); ++i) 
   {
      headerRecord  = leftJustify(commentList[i],60);
      headerRecord += commentString;         // "COMMENT"
      solutionFile<< headerRecord << endl;
   }

      // "SYS / DCBS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify(dcbProgram,17) + string(1, ' ');
   headerRecord += leftJustify(dcbSource,40);
   headerRecord += sysDCBString;          // "SYS / DCBS APPLIED"
   solutionFile<< headerRecord << endl;

      // "SYS / PCVS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify("upd",17) + string(1, ' ');
   headerRecord += leftJustify(antexFile, 40);
   headerRecord += sysPCVString;          // "SYS / PCVS APPLIED"
   solutionFile<< headerRecord << endl;

     // "# / TYPES OF DATA"
   std::vector<std::string> dataTypes;
   dataTypes.push_back("AR");
   dataTypes.push_back("AS");

   headerRecord  = rightJustify(asString(dataTypes.size()), 6);
   for(int i=0; i<dataTypes.size(); ++i) headerRecord += string(4,' ') + dataTypes[i];
   headerRecord += string(60-headerRecord.size(),' ');
   headerRecord += numDataString;         // "# / TYPES OF DATA"
   solutionFile<< headerRecord << endl;

      // "ANALYSIS CENTER"
   std::string analCenterDesignator("WHU");
   std::string analysisCenter("Wuhan University");
      // print out 
   headerRecord  = analCenterDesignator;
   headerRecord += string(2,' ');
   headerRecord += leftJustify(analysisCenter,55);
   headerRecord += analysisCenterString;  // "ANALYSIS CENTER"
   solutionFile << headerRecord << endl;

       // "# OF CLK REF"
   int numClkRef(1);
   headerRecord  = rightJustify(asString(numClkRef), 6) + string(54,' ');  // TD
   headerRecord += numClockRefString;     // "# OF CLK REF"
   solutionFile << headerRecord << endl;

      // "ANALYSIS CLK REF"
   headerRecord  = leftJustify((master.sourceName),4) + string(1,' ');  // TD
   headerRecord += leftJustify((master.sourceNumber),20) + string(15,' ');
   headerRecord += rightJustify(doub2sci(0.0,19,2), 19) + string(1,' ');
   headerRecord += analysisClkRefrString; // "ANALYSIS CLK REF"
   solutionFile << headerRecord << endl;

        // "# OF SOLN STA / TRF"
   string terrRefFrame("IGS05");
   headerRecord  = rightJustify(asString(sourceSet.size()), 6);
   headerRecord += string(4,' ');
   headerRecord += leftJustify(terrRefFrame,50);
   headerRecord += numReceiversString;    // "# OF SOLN STA / TRF"
   solutionFile << headerRecord << endl;
  
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
      solutionFile << headerRecord << endl;
   }
  
      // "# OF SOLN SATS"
   headerRecord  = rightJustify(asString(satSet.size()), 6);
   headerRecord += string(54,' ');
   headerRecord += numSolnSatsString;     // "# OF SOLN SATS"
   solutionFile << headerRecord << endl;
  
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
         solutionFile << headerRecord << endl;
            // New line  
         headerRecord  = string();
      }
        // increment for the satellite number
      i = i+1;
   }

      // "END OF HEADER"
   headerRecord = string(60,' ');
   headerRecord+= endOfHeaderString;     // "END OF HEADER"
   solutionFile << headerRecord << endl;

}  // End of printBiasHeader


   // Method to print clock data 
void upd::printBiasData( ofstream& solutionFile)
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
      
         solutionFile << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         solutionMap.erase( pos++ );
      }
   }   // End of loop ' while( !( solutionMap.empty() ) ) '


}  // End of method 'upd::printBiasData()'



   // Main function
int main(int argc, char* argv[])
{
   try
   {
      upd program(argv[0]);

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
