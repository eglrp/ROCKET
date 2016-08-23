#pragma ident "$Id$"

//============================================================================
//
//  Function
//  --------
//
//  Estimate GPS satellite clock products using PC/LC observables from global
//  distributed IGS tracking stations.
//
//  CopyRight
//  ---------
//  
//  Shoujian Zhang, Wuhan University, 2010, 2011 
//
//  History
//  -------
//
//  2012/05/12  
//
//  Create this program  
//
//  2014/10/14  
//
//  clk is modified from the old version, since many classes has been changed.
//  remove the 'phaseCalibration' class, instead, the 'PhaseCodeAligment' is used 
//  to aligh the phase observables to the code ones.
//
//  2015/07/24  
//
//  This version use a new designed solver, 'SolverGeneral2'. The
//  new version of clock solver impove the computational speed
//  greatly by redesign the time udpate and measurement update of
//  the kalman filter.
//  In the measurement update, the update is basen on single-
//  observable each time.
//
//  2016/05/23  
//
//  The configuration file is greatly changed.
//  only the parameters related to the models are kept in the
//  configuration file.
//  all the data and station-related parameters are given through
//  the command line.
//
//  2016/05/30  
//
//  Change the interface of the OceanLoading class.
//  The BLQ data will be loaded firstly and then inserted into the
//  OceanLoading class.
//
//  2016/06/14 
//  finish the calling method of this program.
//
//  2016/07/08
//  check and test this program
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
#include "MWCSDetector2.hpp"

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
#include "EclipsedSatMarker.hpp"

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

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

   // Class to read the dcb data from files
#include "DCBDataReader.hpp"

   // Class to read the MSC data from files
#include "MSCStore.hpp"

   // Class to correct the P1C1 biases for some receivers
#include "CC2NONCC.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clk : public BasicFramework
{
public:

      // Constructor declaration
   clk(char* arg0);


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

      // Option for several input file list
   CommandOptionWithAnyArg rnxFileListOpt;
   CommandOptionWithAnyArg sp3FileListOpt;
   CommandOptionWithAnyArg clkFileListOpt;
   CommandOptionWithAnyArg eopFileListOpt;
   CommandOptionWithAnyArg dcbFileListOpt;
   CommandOptionWithAnyArg mscFileOpt;
   CommandOptionWithAnyArg outputFileOpt;

   string rnxFileListName;
   string sp3FileListName;
   string clkFileListName;
   string eopFileListName;
   string dcbFileListName;
   string mscFileName;
   string outputFileName;

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
   void printClockHeader( ofstream& clockStream );

      // Method to print solution
   void printClockData( ofstream& clockStream );


}; // End of 'clk' class declaration



   // Let's implement constructor details
clk::clk(char* arg0)
   :
   gpstk::BasicFramework(  arg0,
"\nThis program reads parameters from a configuration file and \n"
"reads GPS data, ephemeris data, including orbit and ERP data, \n"
"from the command line, and then estimate satellite clocks by processing\n"
"the GPS data from global IGS stations.\n\n" ),
      // Option initialization. "true" means a mandatory option
   confFile(       CommandOption::stdType,
                   'c',
                   "conffile",
   "Name of configuration file ('clk.conf' by default).",
                   false ),
   rnxFileListOpt( 'r',
                   "rnxFileList",
   "file storing a list of rinex file name ",
                   true),
   sp3FileListOpt( 's',
                   "sp3FileList",
   "file storing a list of rinex SP3 file name ",
                   true),
   clkFileListOpt( 'k',
                   "clkFileList",
   "file storing a list of rinex CLK file name ",
                   false),
   eopFileListOpt( 'e',
                   "eopFileList",
   "file storing a list of IGS erp file name ",
                   true),
      // Warning: because 'd' has been used for degug option,
      //          here 'D' is used for dcb file reading.
   dcbFileListOpt( 'D',
                   "dcbFileList",
   "file storing a list of the P1C1 DCB file.",
                   false),
   mscFileOpt(     'm',
                   "mscFile",
   "file storing monitor station coordinates ",
                   true),
   outputFileOpt(  'O',
                   "outputFile",
   "file storing the computed clock data",
                   true )
{
      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clk::clk'



   // Method that will be executed AFTER initialization but BEFORE processing
void clk::spinUp()
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
         confReader.open( "clk.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'clk.conf'"
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

      // Now, Let's parse the command line
   if(rnxFileListOpt.getCount())
   {
      rnxFileListName = rnxFileListOpt.getValue()[0];
   }
   if(sp3FileListOpt.getCount())
   {
      sp3FileListName = sp3FileListOpt.getValue()[0];
   }
   if(clkFileListOpt.getCount())
   {
      clkFileListName = clkFileListOpt.getValue()[0];
   }
   if(eopFileListOpt.getCount())
   {
      eopFileListName = eopFileListOpt.getValue()[0];
   }
   if(outputFileOpt.getCount())
   {
      outputFileName = outputFileOpt.getValue()[0];
   }
   if(mscFileOpt.getCount())
   {
      mscFileName = mscFileOpt.getValue()[0];
   }
   if(dcbFileListOpt.getCount())
   {
      dcbFileListName = dcbFileListOpt.getValue()[0];
   }

}  // End of method 'clk::spinUp()'



   // Method that will really process information
void clk::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   solve();
}



   // Method that will really process information
void clk::preprocessing()
{

      /////////////////
      // Put here what you want to apply to ALL stations
      /////////////////
      
      ////////////////////////////////////////////////
      // Now, Let's read SP3 Files
      ////////////////////////////////////////////////

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);

      // Now read sp3 files from 'sp3FileList'
   ifstream sp3FileListStream;

   sp3FileListStream.open(sp3FileListName.c_str(), ios::in);
   if(!sp3FileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "SP3 file List Name'" << sp3FileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string sp3File;
   while( sp3FileListStream >> sp3File )
   {
      try
      {
         SP3EphList.loadFile( sp3File );
      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;
         continue;
      }
   }
      // Close file
   sp3FileListStream.close();

      /////////////////////////
      // Let's read clock files
      /////////////////////////

      // If rinex clock file list is given, then use rinex clock
   if(clkFileListOpt.getCount())
   {
         // Now read clk files from 'clkFileList'
      ifstream clkFileListStream;

         // Open clkFileList File
      clkFileListStream.open(clkFileListName.c_str(), ios::in);
      if(!clkFileListStream)
      {
            // If file doesn't exist, issue a warning
         cerr << "clock file List Name'" << clkFileListName << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }

      string clkFile;
      while( clkFileListStream >> clkFile )
      {
         try
         {
            SP3EphList.loadRinexClockFile( clkFile );
         }
         catch (FileMissingException& e)
         {
               // If file doesn't exist, issue a warning
            cerr << "rinex CLK file '" << clkFile << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;
            continue;
         }
      }

         // Close file
      clkFileListStream.close();

   }  // End of 'if(...)'

      //***********************
      // Let's read ocean loading BLQ data files
      //***********************

      // BLQ data store object
   BLQDataReader blqStore;

      // Read BLQ file name from the configure file
   string blqFile = confReader.getValue( "oceanLoadingFile", "DEFAULT");

   try
   {
      blqStore.open( blqFile );
   }
   catch (FileMissingException& e)
   {
         // If file doesn't exist, issue a warning
      cerr << "BLQ file '" << blqFile << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;
      exit(-1);
   }

      /////////////////////////
      // Let's read eop files
      /////////////////////////

      // Declare a "EOPDataStore" object to handle earth rotation parameter file
   EOPDataStore eopStore;

      // Now read eop files from 'eopFileList'
   ifstream eopFileListStream;

      // Open eopFileList File
   eopFileListStream.open(eopFileListName.c_str(), ios::in);
   if(!eopFileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "erp file List Name'" << eopFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string eopFile;
   while( eopFileListStream >> eopFile )
   {
      try
      {
         eopStore.loadIGSFile( eopFile );
      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;
         continue;
      }
   }
      // Close file
   eopFileListStream.close();

      ////////////////////////////////////////////////
      // Let's read DCB files
      ////////////////////////////////////////////////
      
      // Read and store dcb data
   DCBDataReader dcbStore;

   if(dcbFileListOpt.getCount() )
   {
         // Now read dcb file from 'dcbFileName'
      ifstream dcbFileListStream;

         // Open dcbFileList File
      dcbFileListStream.open(dcbFileListName.c_str(), ios::in);
      if(!dcbFileListStream)
      {
            // If file doesn't exist, issue a warning
         cerr << "dcb file List Name '" << dcbFileListName << "' doesn't exist or you don't "
              << "have permission to read it." << endl;
         exit(-1);
      }

      string dcbFile;

         // Here is just a dcb file, we only read one month's dcb data.
      while(dcbFileListStream >> dcbFile)
      {
         try
         {
            dcbStore.open(dcbFile);
         }
         catch(FileMissingException e)
         {
            cerr << "Warning! The DCB file '"<< dcbFile <<"' does not exist!" 
                 << endl;
            exit(-1);
         }
      };

      dcbFileListStream.close();

   }

      ////////////////////////////////////////////////
      // Now, Let's read MSC data
      ////////////////////////////////////////////////
      
      // Declare a "MSCStore" object to handle msc file 
   MSCStore mscStore;

   try
   {
      mscStore.loadFile( mscFileName );
   }
   catch (gpstk::FFStreamError& e)
   {
         // If file doesn't exist, issue a warning
      cerr << e << endl;
      cerr << "MSC file '" << mscFileName << "' Format is not supported!!!"
           << "stop." << endl;
      exit(-1);
   }
   catch (FileMissingException& e)
   {
         // If file doesn't exist, issue a warning
      cerr << "MSC file '" << mscFileName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;
      exit(-1);
   }

      //***********************************************
      // Now, Let's read the rinex files 
      //***********************************************
   vector<string> rnxFileListVec;
      
      // Now read eop files from 'eopFileList'
   ifstream rnxFileListStream;

      // Open eopFileList File
   rnxFileListStream.open(rnxFileListName.c_str());
   if(!rnxFileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "rinex file List Name'" << rnxFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;
      exit(-1);
   }

   string rnxFile;
   while( rnxFileListStream >> rnxFile )
   {
      rnxFileListVec.push_back(rnxFile);
   }
      // Close file
   rnxFileListStream.close();

   if(rnxFileListVec.size() == 0 )
   {
      cerr << rnxFileListName  << "rnxFileList is empty!! "
           << endl;
      exit(-1);
   }

   /////////////////////////////
   //  Now, Let's define some classes that are common to
   //  all stations.
   /////////////////////////////

      // Object to compute the tidal effects
   SolidTides solid;

      // Object to compute the ocean loading model
   OceanLoading ocean(blqStore);

      // Object to model pole tides
   PoleTides pole(eopStore);

   /////////////////////////////
   //  Now, Let's start preprocessing for ALL stations
   /////////////////////////////

   vector<string>::const_iterator rnxit = rnxFileListVec.begin();
   while( rnxit != rnxFileListVec.end() )
   {
         // Read rinex file from the vector!
      string rnxFile = (*rnxit);


         // Create input observation file stream
      RinexObsStream rin;
      rin.exceptions(ios::failbit); // Enable exceptions

         // Try to open Rinex observations file
      try
      {
         rin.open( rnxFile, std::ios::in );
      }
      catch(...)
      {

         cerr << "Problem opening file '"
              << rnxFile
              << "'." << endl;

         cerr << "Maybe it doesn't exist or you don't have "
              << "proper read permissions."
              << endl;

         cerr << "Skipping rinex file '" << rnxFile << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

            // Index for rinex file iterator.
         ++rnxit;

         continue;

      }  // End of 'try-catch' block

         //////////////////////
         // Let's read the header firstly!!!!
         //////////////////////
      RinexObsHeader roh;
      try
      {
         rin >> roh;
      }
      catch(FFStreamError& e)
      {
         cerr << "Problem in reading file '"
              << rnxFile
              << "'." << endl;
         cerr << "Skipping receiver '" << rnxFile << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

           // Index for rinex file iterator.
         ++rnxit;

         continue;
      }

         // The Year/DayOfYear/SecOfYear format, you MUST set the timeSystem at 
         // the same time.
      YDSTime yds( confReader.getValueAsInt("year"),
                   confReader.getValueAsInt("dayOfYear"),
                   confReader.getValueAsDouble("secOfDay"), TimeSystem::GPS );

      CommonTime initialTime( yds.convertToCommonTime() );

         // Get the station name for current rinex file 
      string station = roh.markerName;

         // Let's check the ocean loading data for current station before
         // the real data processing.
      if( ! blqStore.isValid(station) )
      {
         cout << "There is no BLQ data for current station:" << station << endl;
         cout << "we will skip it !!! " << endl;
         continue;
      }

         // Show a message indicating that we are starting with this station
      cout << "Starting processing for station: '" << station << "'." << endl;

        // MSC data for this station
      initialTime.setTimeSystem(TimeSystem::Unknown);
      MSCData mscData;
      try
      {
         mscData = mscStore.findMSC( station, initialTime );
      }
      catch (InvalidRequest& ie)
      {
            // If file doesn't exist, issue a warning
         cerr << "The station " << station << " isn't included in MSC file." 
                                           << endl;

            // Warning:you must increase the iterator to process the next file,
            // or else the program will repeat opening this file
         ++rnxit;

         continue;
      }

         // Now, Let's change the system to GPS 
      initialTime.setTimeSystem(TimeSystem::GPS);

         // Read inital position from given msc data store.
      Position nominalPos( mscData.coordinates );

         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;

         //>This object will correct the P1C1 dcb for some receivers
         //>defined in the 'recTypeFile'.
      CC2NONCC cc2noncc(dcbStore);

         // Read the receiver type file.
      cc2noncc.loadRecTypeFile( confReader.getValue("recTypeFile") );
      cc2noncc.setRecType(roh.recType);
      cc2noncc.setCopyC1ToP1(true);

         // Add to processing list
      pList.push_back(cc2noncc);


         //>This object will check that all required observables are present
      RequireObservables requireObs;
      requireObs.addRequiredType(TypeID::P1);
      requireObs.addRequiredType(TypeID::P2);
      requireObs.addRequiredType(TypeID::L1);
      requireObs.addRequiredType(TypeID::L2);
         // Add 'requireObs' to processing list (it is the first)
      pList.push_back(requireObs);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilter pObsFilter;
      pObsFilter.addFilteredType(TypeID::P1);
      pObsFilter.setFilteredType(TypeID::P2);

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
      bool filterCode( confReader.getValueAsBoolean( "filterCode" ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // This object defines several handy linear combinations
      LinearCombinations comb;

         // Object to compute linear combinations for cycle slip detection
      ComputeLinear linear1;
      linear1.addLinear(comb.pdeltaCombination);
      linear1.addLinear(comb.mwubbenaCombination);
      linear1.addLinear(comb.ldeltaCombination);
      linear1.addLinear(comb.liCombination);
      pList.push_back(linear1);       // Add to processing list

         // Objects to mark cycle slips
      LICSDetector markCSLI;         // Checks LI cycle slips
      pList.push_back(markCSLI);     // Add to processing list
      MWCSDetector2 markCSMW;      // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);    // Add to processing list

         // Object to keep track of satellite arcs
         // Notes: delete unstable satellite may cause discontinuity
         //        in the network processing!
      SatArcMarker2 markArc;
      markArc.setDeleteUnstableSats(true);
      markArc.setUnstablePeriod(151.0);
      pList.push_back(markArc);       // Add to processing list


         // Object to decimate data
      Decimate decimateData(
              confReader.getValueAsDouble( "decimationInterval" ),
              confReader.getValueAsDouble( "decimationTolerance" ),
              initialTime);
      pList.push_back(decimateData);       // Add to processing list


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);

         // Set the minimum elevation
      basic.setMinElev( confReader.getValueAsDouble("cutOffElevation") );
      basic.setDefaultObservable(TypeID::P1);

         // Add to processing list
      pList.push_back(basic);


         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights;
      pList.push_back(elevWeights);       // Add to processing list


////////////////////////////////////////////////////
//       // Object to mark eclipsed satellites
//    EclipsedSatMarker markEclipse;
//    pList.push_back(markEclipse);       // Add to processing list
////////////////////////////////////////////////////


         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list


         // Vector from monument to antenna ARP [UEN], in meters
      Triple offsetARP( roh.antennaOffset );

         // Declare some antenna-related variables
      AntexReader antexReader;
      Antenna receiverAntenna;

         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex") );
      string antennaModel;
      if( useantex )
      {
            // Feed Antex reader object with Antex file
         antexReader.open( confReader.getValue( "antexFile" ) );

            // Antenna model 
         antennaModel = roh.antType;

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
      bool usepatterns( confReader.getValueAsBoolean("usePCPatterns") );
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth( confReader.getValueAsBoolean("useAzim") );
      }
      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos );
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
         windup.setAntexReader( antexReader );
      }

      pList.push_back(windup);       // Add to processing list


         // Declare a NeillTropModel object, setting its parameters
      NeillTropModel neillTM( nominalPos,
                              initialTime );

         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         // Object to compute ambiguity for L1 and L2
      ComputeLinear linear2;
      linear2.addLinear(comb.q1Combination);
      linear2.addLinear(comb.q2Combination);
      pList.push_back(linear2);       // Add to processing list
      

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL1;
      phaseAlignL1.setCodeType(TypeID::Q1);
      phaseAlignL1.setPhaseType(TypeID::L1);
      phaseAlignL1.setPhaseWavelength( 0.190293672798365 );
      pList.push_back(phaseAlignL1);       // Add to processing list

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL2;
      phaseAlignL2.setCodeType(TypeID::Q2);
      phaseAlignL2.setPhaseType(TypeID::L2);
      phaseAlignL2.setPhaseWavelength( 0.244210213424568 );
      pList.push_back(phaseAlignL2);       // Add to processing list


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinear linear3;
      linear3.addLinear(comb.pcCombination);
      linear3.addLinear(comb.lcCombination);
      pList.push_back(linear3);       // Add to processing list


         // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);

         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC" ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
         pList.push_back(pcFilter);       // Add to processing list
      }

         // Now, After calibrating the L1/L2, let's compute the "new" 
         // MWubbena combinations
      ComputeLinear linear4;
      linear4.addLinear(comb.mwubbenaCombination);
      pList.push_back(linear4);       // Add to processing list

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


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;


         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel" ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
         modelName = rnxFile + ".model";
         modelfile.open( modelName.c_str(), ios::out );
      }

         // *** The REAL preprocessing of a SPECIFIC station is done here *** //

         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Store current epoch
         CommonTime time(gRin.header.epoch);

            // Set the nominal position from the IGS solution file
         gRin.header.source.nominalPos = nominalPos;

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
         types.insert(TypeID::satArc);
         types.insert(TypeID::prefitC);
         types.insert(TypeID::prefitL);

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
      if( confReader.getValue( "masterStation" ) == station )
      {
         master = source;
      }
      else
      {
         refStationSet.insert( source );
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
      cout << "Processing finished for station: '" << station;
      if ( printmodel )
      {
         cout << "'. Model in file: '" << modelName;
      }
      cout << "'." << endl;


         // go to process the next station.
      ++rnxit;


   }  // End of 'while ( (station = confReader.getEachSection()) != "" )'


   //// End of preprocessing for ALL stations ////


      // Clear content of SP3 ephemerides object
   SP3EphList.clear();


      // The rest of the processing will be in method 'clk::shutDown()'
   return;

}  // End of 'clk::process()'



   // Method to print model values
void clk::printModel( ofstream& modelfile,
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
      modelfile << static_cast<YDSTime>(time).year  << "  ";    // Year           #1
      modelfile << static_cast<YDSTime>(time).doy   << "  ";    // DayOfYear      #2
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

}  // End of method 'clk::printModel()'



   // Method to generate and solve the general equation system
void clk::solve()
{

   ////> In the resolution part we start configuring the general solver 

      // Declare stochastic models to be used
      // The coordinates are handled as white noise with 100 m of sigma
   WhiteNoiseModel coordinatesModel(100.0);
   TropoRandomWalkModel tropoModel;
   PhaseAmbiguityModel ambiModel;

      // This variables are, by default, SourceID-indexed
   Variable dLat( TypeID::dLat, &coordinatesModel, true, false, 100.0 );
   Variable dLon( TypeID::dLon, &coordinatesModel, true, false, 100.0 );
   Variable dH( TypeID::dH, &coordinatesModel, true, false, 100.0 );

   Variable cdt( TypeID::cdt );
   cdt.setDefaultForced(true);    // Force the default coefficient (1.0)

   Variable tropo( TypeID::wetMap, &tropoModel, true, false, 10.0  );


      // The following variable is, SourceID and SatID-indexed
   Variable ambi( TypeID::BLC, &ambiModel, true, true );
   ambi.setDefaultForced(true);        // Force the default coefficient (1.0)

      // Satellite clock model
   WhiteNoiseModel satClockModel;

      // This variable will be SatID-indexed only
   Variable satClock( TypeID::dtSat, &satClockModel );
   satClock.setSourceIndexed(false);
   satClock.setSatIndexed(true);
   satClock.setDefaultForced(true); // Flags to use the default coefficient
   satClock.setDefaultCoefficient(-1.0); // set the default coefficient (-1.0)

      // This will be the independent term for code equations
   Variable prefitC( TypeID::prefitC );

      // This will be the independent term for phase equations
   Variable prefitL( TypeID::prefitL );



      // Reference stations code equation description
   Equation equPCRef( prefitC );
   equPCRef.addVariable(cdt, true);
   equPCRef.addVariable(tropo);
   equPCRef.addVariable(satClock, true, -1.0);

      // Set the source of the equation
   equPCRef.header.equationSource = Variable::someSources;

      // Reference stations phase equation description
   Equation equLCRef( prefitL );
   equLCRef.addVariable(cdt, true); // set to use the default coefficient (1.0)
   equLCRef.addVariable(tropo);
   equLCRef.addVariable(ambi, true); // set to use the default coefficient (1.0)
   equLCRef.addVariable(satClock, true, -1.0); // set to use the default coefficient(-1.0)

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
   equPCMaster.addVariable(tropo);
   equPCMaster.addVariable(satClock, true, -1.0);

      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Master station phase equation description
   Equation equLCMaster( prefitL );
   equLCMaster.addVariable(tropo);
   equLCMaster.addVariable(ambi, true);
   equLCMaster.addVariable(satClock, true, -1.0);

      // Master station phase equation has more weight
   equLCMaster.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCMaster.header.equationSource = master;


      // Setup equation system
   EquationSystem2 system;
   system.addEquation(equPCRef);
   system.addEquation(equLCRef);
   system.addEquation(equPCMaster);
   system.addEquation(equLCMaster);

      // Forward-backward solver
   SolverGeneral2 solver(system);

      // Get if we want 'forwards-backwards' or 'forwards' processing only
   bool cycles( confReader.getValueAsInt("filterCycles", "DEFAULT") );

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

         // print warning if there are no data for the master station 
         // at current epoch

         // Extract current epoch
      CommonTime workEpoch( (*gds.begin()).first );

         // Compute the solution using solverFb. This is it!!!
      solver.Process( gds );

         // Extract solutions from the solver and store it into string
      getClockSoluts(solver, workEpoch);

   }  // End of 'while( !gdsMap.empty() )'

   

   /// We are done ////

}  // End of 'clk::solve()'


   // Method to extract the solutions from solver and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printClockData'
void clk::getClockSoluts( const SolverGeneral2& solver,
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

}  // End of method 'clk::getClockSoluts()'


   // Method that hold code to be run AFTER processing
void clk::shutDown()
{
      // computed clock data output stream
   ofstream clockStream;
   clockStream.open( outputFileName.c_str(), ios::out );

      // Print the header information
   printClockHeader( clockStream );

      // print the clock products as the IGS format
   printClockData( clockStream );

      // close the solution file stream
   clockStream.close();


}  // End of 'clk::shutDown()'


   // Method to print clock data 
void clk::printClockHeader( ofstream& clockStream)
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
   clockStream << headerRecord << endl;

      // "PGM / RUN BY / DATE"
      //  Varialbes
   string program ("clk");
   string runby ("SGG");
   Epoch dt;
   dt.setLocalTime();
   string dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
      //  print out
   headerRecord  = leftJustify(program,20);
   headerRecord += leftJustify(runby,20);
   headerRecord += leftJustify(dat, 20);
   headerRecord += runByString;          
   clockStream << headerRecord << endl;


      // "TIME SYSTEM ID"
   string timeSystem("GPS");
   headerRecord  = string(3,' ');  // TD
   headerRecord += leftJustify(timeSystem,57);     
   headerRecord += timeSystemString;     
   clockStream << headerRecord << endl;

      // "COMMENT"
   std::vector<std::string> commentList;   
   for(int i=0; i<commentList.size(); ++i) 
   {
      headerRecord  = leftJustify(commentList[i],60);
      headerRecord += commentString;         // "COMMENT"
      clockStream<< headerRecord << endl;
   }

/*    // "LEAP SECONDS"
 * int leapSeconds;
 * headerRecord  = rightJustify(asString(leapSeconds), 6);
 * headerRecord += string(54,' ');
 * headerRecord += leapSecondsString;     // "LEAP SECONDS"
 * clockStream<< headerRecord << endl;
 */

      // "SYS / DCBS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify(dcbProgram,17) + string(1, ' ');
   headerRecord += leftJustify(dcbSource,40);
   headerRecord += sysDCBString;          // "SYS / DCBS APPLIED"
   clockStream<< headerRecord << endl;

      // "SYS / PCVS APPLIED"
   headerRecord  = string("G") + string(1,' ');
   headerRecord += leftJustify("clk",17) + string(1, ' ');
   headerRecord += leftJustify(antexFile, 40);
   headerRecord += sysPCVString;          // "SYS / PCVS APPLIED"
   clockStream<< headerRecord << endl;

     // "# / TYPES OF DATA"
   std::vector<std::string> dataTypes;
   dataTypes.push_back("AR");
   dataTypes.push_back("AS");

   headerRecord  = rightJustify(asString(dataTypes.size()), 6);
   for(int i=0; i<dataTypes.size(); ++i) headerRecord += string(4,' ') + dataTypes[i];
   headerRecord += string(60-headerRecord.size(),' ');
   headerRecord += numDataString;         // "# / TYPES OF DATA"
   clockStream<< headerRecord << endl;

      // "ANALYSIS CENTER"
   std::string analCenterDesignator("WHU");
   std::string analysisCenter("Wuhan University");
      // print out 
   headerRecord  = analCenterDesignator;
   headerRecord += string(2,' ');
   headerRecord += leftJustify(analysisCenter,55);
   headerRecord += analysisCenterString;  // "ANALYSIS CENTER"
   clockStream << headerRecord << endl;

       // "# OF CLK REF"
   int numClkRef(1);
   headerRecord  = rightJustify(asString(numClkRef), 6) + string(54,' ');  // TD
   headerRecord += numClockRefString;     // "# OF CLK REF"
   clockStream << headerRecord << endl;

      // "ANALYSIS CLK REF"
   headerRecord  = leftJustify((master.sourceName),4) + string(1,' ');  // TD
   headerRecord += leftJustify((master.sourceNumber),20) + string(15,' ');
   headerRecord += rightJustify(doub2sci(0.0,19,2), 19) + string(1,' ');
   headerRecord += analysisClkRefrString; // "ANALYSIS CLK REF"
   clockStream << headerRecord << endl;

        // "# OF SOLN STA / TRF"
   string terrRefFrame("IGS05");
   headerRecord  = rightJustify(asString(sourceSet.size()), 6);
   headerRecord += string(4,' ');
   headerRecord += leftJustify(terrRefFrame,50);
   headerRecord += numReceiversString;    // "# OF SOLN STA / TRF"
   clockStream << headerRecord << endl;
  
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
      clockStream << headerRecord << endl;
   }
  
      // "# OF SOLN SATS"
   headerRecord  = rightJustify(asString(satSet.size()), 6);
   headerRecord += string(54,' ');
   headerRecord += numSolnSatsString;     // "# OF SOLN SATS"
   clockStream << headerRecord << endl;
  
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
         clockStream << headerRecord << endl;
            // New line  
         headerRecord  = string();
      }
        // increment for the satellite number
      i = i+1;
   }

      // "END OF HEADER"
   headerRecord = string(60,' ');
   headerRecord+= endOfHeaderString;     // "END OF HEADER"
   clockStream << headerRecord << endl;

}  // End of printClockHeader


   // Method to print clock data 
void clk::printClockData( ofstream& clockStream)
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
      
         clockStream << pos->second  << endl;
            // It is advisable to avoid sawing off the branch we are
            // sitting on
         solutionMap.erase( pos++ );
      }
   }   // End of loop ' while( !( solutionMap.empty() ) ) '


}  // End of method 'clk::printClockData()'



   // Main function
int main(int argc, char* argv[])
{
   try
   {
      clk program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if ( !program.initialize(argc, argv, true) )
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
