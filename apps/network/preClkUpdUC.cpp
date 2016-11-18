#pragma ident "$Id$"

//============================================================================
//
// PPP Positioning
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that computes "Precise Point
// Positioning" (PPP).
//
// For details on the PPP algorithm please consult:
//
//    Kouba, J. and P. Heroux. "Precise Point Positioning using IGS Orbit
//       and Clock Products". GPS Solutions, vol 5, pp 2-28. October, 2001.
//
// For PPP positioning with ambiguity resolution (PPP-AR), please SEE:
//
//    Collins, (2009)
//    Ge, M, (2009)
//    Laurichesse, (2009)
//
// Warning:
//
// You MUST take care that the model used in this program should
// be consistent with the model that are used in the UPD estimation.
//
// To be clear:
//
// The P1P2 DCB should be corrected for the MW/LC model in the UPD
// estimation.
// 
// Copyright 
//
// Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
// Shoujian Zhang, Wuhan University, 2015
//
//============================================================================
//
//	 Revision
//
//  2016.11.11
//  
//  Create this program to estimate upd using original GPS observables insdead
//  of ionosphere-free combination.
//  Only P1 and P2 types are considered !!!
//  Lei Zhao, a Ph.D. candidate 
//	 School of Geodesy and Geomatic, Wuhan University
//============================================================================



// Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>


#include "FFStreamError.hpp"

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

   // Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-only mode with ambiguity resolution.
//#include "SolverPPPUCAR.hpp"
//#include "SolverPPPUCAR1.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forward-backwards mode with ambiguity resolution.
//#include "SolverPPPUCARFB.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"

   // Class to compute the elevation weights
#include "ComputeElevWeights.hpp"

   // Class to store satellite precise navigation data
#include "MSCStore.hpp"

   // Class to correct the P1C1 dcb 
#include "CC2NONCC.hpp"

   // Class to correct satellite biases
#include "CorrectUPDs.hpp"

   // Class to compute ionosphere
#include "IonexModel.hpp"
#include "IonexStore.hpp"

   // Class to correct satellite biases
#include "GDSUtils.hpp"

	// Class to define variable
#include "Variable.hpp"

	// Commonly used equation
#include "CommonlyUsedEquations.hpp"

	// EquationSystem2
#include "EquationSystem2.hpp"

	// Solver of equation system
#include "SolverGenL1L2.hpp"

	// GDSSerializer
#include "GNSSDataSerializer.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clkupdUC : public BasicFramework
{
public:

      // Constructor declaration
   clkupdUC(char* arg0);


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

      // Option for rinex file list
   CommandOptionWithAnyArg rnxFileListOpt;

      // Option for sp3 file list
   CommandOptionWithAnyArg sp3FileListOpt;

      // Option for clk file list
   CommandOptionWithAnyArg clkFileListOpt;

      // Option for eop file list
   CommandOptionWithAnyArg eopFileListOpt;

      // Option for eop file list
//   CommandOptionWithAnyArg updFileListOpt;

      // Option for eop file list
   CommandOptionWithAnyArg ionFileListOpt;

      // Option for p1c1 dcb file 
//   CommandOptionWithAnyArg dcbFileOpt;

      // Option for monitor coordinate file
   CommandOptionWithAnyArg mscFileOpt;

      // Option for output file
   //CommandOptionWithAnyArg outputFileOpt;
   CommandOptionWithAnyArg outputFileListOpt;

      // If you want to share objects and variables among methods, you'd
      // better declare them here
   
   string rnxFileListName;
   string sp3FileListName;
   string clkFileListName;
   string eopFileListName;
//   string updFileListName;
   string ionFileListName;
//   string dcbFileName;
   string mscFileName;
//   string outputFileName;
   string outputFileListName;

	string outputGDSMapFileName;
	string outputRefStaFileName;
	string outputMasterStaFileName;

		// Dummy source to store ref stations
	sourceDataMap dummySourceDataMap;

      // Configuration file reader
   ConfDataReader confReader;


      // Declare our own methods to handle output
   gnssDataMap gdsMap;

		// Master Station
	SourceID master;

		// Reference stations set 
	std::set<SourceID> refStationSet;

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 4 );

	      // Method to print clock header 
   void printBiasHeader( ofstream& solutionFile );


      // Method to print clock data 
   void printBiasData( ofstream& solutionFile );

	


}; // End of 'clkupdUC' class declaration



   // Let's implement constructor details
clkupdUC::clkupdUC(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Precise Point Positioning' strategy.\n\n"
"Please consult the default configuration file, 'clkupdUC.conf', for\n"
"further details.\n\n"
"The output file format is as follows:\n\n"
"\n 1) Year"
"\n 2) doy"
"\n 3) Seconds of day"
"\n 4) dx/dLat (m)"
"\n 5) dy/dLon (m)"
"\n 6) dz/dH (m)"
"\n 7) Zenital Tropospheric Delay (zpd) (m)"
"\n 8) Number of satellites"
"\n 9) GDOP"
"\n10) PDOP\n"),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   "Name of configuration file ('clkupdUC.conf' by default).",
             false ),
      // Option for rinex file list reading
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
   "file storing a list of rinex clk file name ",
                   false),
//   updFileListOpt( 'u',
//                   "updFileList",
//   "file storing a list of UPD file name ",
//                   true),
   ionFileListOpt( 'i',
                   "ionFileList",
   "file storing a list of ion file name ",
                   true),
   eopFileListOpt( 'e',
                   "eopFileList",
   "file storing a list of IGS erp file name ",
                   true),
//   dcbFileOpt(    'd',
//                  "dcbFile",
//   "file storing the p1c1 dcb file name ",
//                   false),
//   outputFileOpt( 'o',
//                   "outputFile",
//   "file storing the list of output file name ",
//                   false),
   outputFileListOpt( 'o',
                   "outputFileNameList",
   "file storing the list of output file name ",
                   true),
   mscFileOpt( 'm',
               "mscFile",
   "file storing monitor station coordinates ",
               true)
{
      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clkupdUC::clkupdUC'


	// Method that will really process information
void clkupdUC::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   //solve();
}  // End of 'void clkupdUC::process()'



   // Method that will really process information
void clkupdUC::preprocessing()
{

      //**********************************************
      // Now, Let's read SP3 Files
      //**********************************************

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

      //***********************
      // Let's read clock files
      //***********************

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
      // Let's read ion files
      //***********************
      
      // Declare a "IonexStore" object to handle satellite bias
   IonexStore ionexStore;

      // Now read ionex files from 'ionFileList'
   ifstream ionFileListStream;

      // Open updFileList File
   ionFileListStream.open(ionFileListName.c_str(), ios::in);
   if( !ionFileListStream )
   {
         // If file doesn't exist, issue a warning
      cerr << "ION file List Name'" << ionFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;
      exit(-1);
   }

   string ionFile;
   while( ionFileListStream >> ionFile )
   {
      try
      {
         ionexStore.loadFile( ionFile );
      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "IONEX file '" << ionFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;
         continue;
      }
   }

      // Close file
   ionFileListStream.close();

      //***********************
      // Let's read eop files
      //***********************

      // Declare a "EOPDataStore" object to handle earth rotation parameter file
   EOPDataStore eopStore;

      // Now read eop files from 'eopFileList'
   ifstream eopFileListStream;

      // Open eopFileList File
   eopFileListStream.open(eopFileListName.c_str(), ios::in);
   if( !eopFileListStream )
   {
         // If file doesn't exist, issue a warning
      cerr << "EOP file List Name'" << eopFileListName << "' doesn't exist or you don't "
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

      //**********************************************
      // Now, Let's read MSC data
      //**********************************************
      
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

		//***********************
		// Let's read ocean loading BLQ data files
		//***********************

		// BLQ data store object
	BLQDataReader blqStore;

		// Read BLQ file name from the configure file
	string blqFile = confReader.getValue( "oceanLoadingFile");

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

		//***********************
		// Specify output file names
		//***********************
	vector<string> outputFileListVec;

		// Now read output files from 'outputFileListName'
	ifstream outputFileListStream;
	outputFileListStream.open(outputFileListName.c_str(), ios::in);	
	if( !outputFileListStream )
	{
		cerr << "output file list name: '" << outputFileListName 
				<< "' doesn't exist" << endl;
		exit(-1);
	}

	string outputFileStr;
	while( outputFileListStream >> outputFileStr )
	{
		outputFileListVec.push_back( outputFileStr );
	}

	outputFileListStream.close();

	if( outputFileListVec.size() == 0 )
	{
		cerr << "'" << outputFileListName << "' is empty" << endl;
	}

	outputGDSMapFileName = outputFileListVec[0];
	outputRefStaFileName = outputFileListVec[1];
	outputMasterStaFileName = outputFileListVec[2];

	// debug code vvv
	cout << "outputGDSMapFileName: " << outputGDSMapFileName << endl
			<< "outputRefStaFileName: " << outputRefStaFileName << endl
				<< "outputMasterStaFileName: " << outputMasterStaFileName << endl;
	// debug code ^^^



      //**********************************************************
      // Now, read the raw obsrvation rinex file name list 
      //**********************************************************

   vector<string> rnxFileListVec;
      
      // Now read eop files from 'eopFileList'
   ifstream rnxFileListStream;

      // Open eopFileList File
   rnxFileListStream.open(rnxFileListName.c_str(), ios::in);
   if( !rnxFileListStream )
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
   }


			// ===========================================
         // Let's process the rinex files one by one !!!
         // ===========================================

      // We will read each rinex file
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

         cerr << "Skipping receiver '" << rnxFile << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

            // Index for rinex file iterator.
         ++rnxit;

         continue;

      }  // End of 'try-catch' block

			// ===================
         // Let's read the header firstly!!!!
         // ===================
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

         // Get the station name for current rinex file 
      string station = roh.markerName;

         // First time for this rinex file
      CommonTime initialTime( roh.firstObs ) ;

         // Show a message indicating that we are starting with this station
      cout << "Starting processing for station: '" << station << "'." << endl;

		   // MSC data for this station
      initialTime.setTimeSystem(TimeSystem::Unknown);
      MSCData mscData( mscStore.findMSC( station, initialTime ) );
      initialTime.setTimeSystem(TimeSystem::GPS);

         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

      Position nominalPos( mscData.coordinates );

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


      requireObs.addRequiredType(TypeID::P1);
      pObsFilter.addFilteredType(TypeID::P1);


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
      LICSDetector markCSLI;          // Checks LI cycle slips
      pList.push_back(markCSLI);      // Add to processing list
      MWCSDetector markCSMW;          // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);      // Add to processing list


         // Object to keep track of satellite arcs
      SatArcMarker2 markArc;
      markArc.setDeleteUnstableSats(false);
      markArc.setUnstablePeriod(61.0);
      pList.push_back(markArc);       // Add to processing list


         // Object to decimate data
      Decimate decimateData(
               confReader.getValueAsDouble( "decimationInterval"),
               confReader.getValueAsDouble( "decimationTolerance"),
               initialTime );
      pList.push_back(decimateData);       // Add to processing list


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);
         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation"));
         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
      
      basic.setDefaultObservable(TypeID::P1);
      
         // Add to processing list
      pList.push_back(basic);

         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights;
      pList.push_back(elevWeights);       // Add to processing list


         // Object to remove eclipsed satellites
      EclipsedSatFilter eclipsedSV;
      pList.push_back(eclipsedSV);       // Add to processing list


         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list


         // Vector from monument to antenna ARP [UEN], in meters
      Triple offsetARP( roh.antennaOffset );

         // Declare some antenna-related variables
      Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
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
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns" ));
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim" ));
      }
      else
      {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
         offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1");
         offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1");
         offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1");

            // Vector from antenna ARP to L2 phase center [UEN], in meters
         offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2");
         offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2");
         offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2");

         corr.setL1pc( offsetL1 );
         corr.setL2pc( offsetL2 );

      }

      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos);
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
         windup.setAntexReader( antexReader );
      }

      pList.push_back(windup);       // Add to processing list


         // Declare a NeillTropModel object, setting its parameters
      NeillTropModel neillTM( nominalPos, 
                              initialTime );

         // We will need this value later for printing
      double drytropo( neillTM.dry_zenith_delay() );


         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         // Declare a IonexModel object to compute the ionospheric delays
         // and the P1P2 instrumental biases at the same time 
      IonexModel ionex(nominalPos, 
                       ionexStore);
      pList.push_back(ionex); // Add to processing list

         // Object to compute code combination with minus ionospheric delays
         // for L1/L2 calibration
      ComputeLinear linear2;

         // Read if we should use C1 instead of P1
      linear2.addLinear(comb.q1Combination);
      linear2.addLinear(comb.q2Combination);
      
      pList.push_back(linear2);       // Add to processing list


         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL1;
      phaseAlignL1.setCodeType(TypeID::Q1);
      phaseAlignL1.setPhaseType(TypeID::L1);
      phaseAlignL1.setPhaseWavelength(0.190293672798);

      pList.push_back(phaseAlignL1);       // Add to processing list

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL2;
      phaseAlignL2.setCodeType(TypeID::Q2);
      phaseAlignL2.setPhaseType(TypeID::L2);
      phaseAlignL2.setPhaseWavelength(0.244210213425);
      pList.push_back(phaseAlignL2);       // Add to processing list

		   // Object to compute new MWubbena combinations with 
         // the calibrated L1 and L2
      ComputeLinear linear3;
      linear3.addLinear(comb.mwubbenaCombination);

      pList.push_back(linear3);       // Add to processing list


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinear linear4;

      linear4.addLinear(comb.pcCombination);
      
      linear4.addLinear(comb.lcCombination);
      pList.push_back(linear4);       // Add to processing list

		   // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);

         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC") );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
         pList.push_back(pcFilter);       // Add to processing list
      }

		   // Object to compute c1 or p1/p2/l1/l2 prefit-residuals
      ComputeLinear linear6;
      linear6.addLinear(comb.p1Prefit);
      linear6.addLinear(comb.p2Prefit);
      linear6.addLinear(comb.l1Prefit);
      linear6.addLinear(comb.l2Prefit);

      pList.push_back(linear6);       // Add to processing list

		   // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list


         // Get if we want results in ECEF or NEU reference system
      bool isNEU( confReader.getValueAsBoolean( "USENEU") );


		   // Object to compute tidal effects
      SolidTides solid;

         // Configure ocean loading model
      OceanLoading ocean(blqStore);
//      ocean.setFilename( confReader.getValue( "oceanLoadingFile") );

         // Object to model pole tides
      PoleTides pole(eopStore);


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;

         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision" ) );

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

		   //// *** Now comes the REAL forwards processing part *** ////

         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Store current epoch
         CommonTime time(gRin.header.epoch);

            // Store the nominal position into 'SourceID'
         gRin.header.source.nominalPos = nominalPos;

            // Compute solid, oceanic and pole tides effects at this epoch
         Triple tides( solid.getSolidTide( time, nominalPos )  +
                       ocean.getOceanLoading( station, time )  +
                       pole.getPoleTide( time, nominalPos )    );

            // Update observable correction object with tides information
         corr.setExtraBiases(tides);


			// Test Code vvv
//			SourceID sid(SourceID::GPS, station);
//			double dcb( ionexStore.findStationDCB( sid, time) );
			// Test Code ^^^

		   try
         {
               // Let's process data. Thanks to 'ProcessingList' this is
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
               // If we catch a SVNumException, just continue.
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
                        gRin);

         }

				// Only keep the necessary types for 'SolverGeneralFB'
			TypeIDSet types;
			types.insert(TypeID::weight);
			types.insert(TypeID::wetMap);
			types.insert(TypeID::elevation);
			types.insert(TypeID::satArc);

				// Observables
			types.insert(TypeID::prefitP1);
			types.insert(TypeID::prefitP2);
			types.insert(TypeID::prefitL1);
			types.insert(TypeID::prefitL2);
			types.insert(TypeID::ionoL1);
//			types.insert(TypeID::recInstP2);  // P1-P2 DCB at receiver end

				// Delete the types not in 'types' to save the memory
			gRin.keepOnlyTypeID(types);

				// Add receiver dcb
			SourceID sid(SourceID::GPS, station);
			double dcb( ionexStore.findStationDCB( sid, time) );
//			std::cout << "dcb of " << station << ": " << dcb  << std::endl;
			gRin.header.source.zwdMap[TypeID::recInstP2] = dcb; 

				// Store observation data
			gdsMap.addGnssRinex(gRin);


		
		}  // End of 'while(rin >> gRin)'


		 // Get source
       // NOTE: 'station' is a 'string', and we need a 'SourceID'
		SourceID source( gRin.header.source );

		if( confReader.getValue( "masterStation" ) ==  station )
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

	


		// WARNING:::::::::::::::::
		// Index for rinex file iterator.
		// before goto the next file
		++rnxit;


	}  // End of ' while( rnxit != rnxFileListVec.end() ) '

		// Now store gdsMap, 
		//				 refStationSet
		//				 master 
		//	using GDSSerializer class
	GDSSerializer::serializeToFile<gnssDataMap>(gdsMap, outputGDSMapFileName);
	GDSSerializer::serializeToFile<sourceDataMap>(dummySourceDataMap, outputRefStaFileName);
	GDSSerializer::serializeToFile<SourceID>(master, outputMasterStaFileName);



		// Clear content of SP3 ephemerides object
	SP3EphList.clear();

	return;

}  // End of ' void clkupdUC::preprocessing() '



   // Method to generate and solve the general equation system
void clkupdUC::solve()
{
		// Common equations
	CommonlyUsedEquations equ;

		// Equations setting

		// For P1/P2/L1/L2 equations
	equ.equP1Master.setWeight(11.111); // (1m)^2/(0.3)^2 = 11.111
	equ.equP1Master.header.equationSource = master;
	equ.equP1Ref.setWeight(11.111);
	equ.equP1Ref.header.equationSource = Variable::someSources; 

	equ.equP2Master.setWeight(11.111); // (1m)^2/(0.3)^2 = 11.111
	equ.equP2Master.header.equationSource = master;
	equ.equP2Ref.setWeight(11.111);
	equ.equP2Ref.header.equationSource = Variable::someSources; 

	equ.equL1Master.setWeight(111111.11); // (1m)^2/(0.003)^2 = 111111.11
	equ.equL1Master.header.equationSource = master;
	equ.equL1Ref.setWeight(111111.11);
	equ.equL1Ref.header.equationSource = Variable::someSources; 

	equ.equL2Master.setWeight(111111.11); // (1m)^2/(0.003)^2 = 111111.11
	equ.equL2Master.header.equationSource = master;
	equ.equL2Ref.setWeight(111111.11);
	equ.equL2Ref.header.equationSource = Variable::someSources; 

		// For iono equation
   equ.equSlantIonoL1.setWeight(0.01); // 1m^2/10m^2

		// For dcb equation
	equ.equRecDCB.setWeight(100); // 1m^2/0.1m^2 

		// Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equ.equP1Ref.addSource2Set( (*itSet) );
		equ.equP2Ref.addSource2Set( (*itSet) );
		equ.equL1Ref.addSource2Set( (*itSet) );
		equ.equL2Ref.addSource2Set( (*itSet) );
   }  // End of 'for( std::set<SourceID>::const_iterator itSet ... '

		// Setup equation system
	EquationSystem2 system1;
	
	system1.addEquation(equ.equP1Ref);
	system1.addEquation(equ.equP2Ref);
	system1.addEquation(equ.equL1Ref);
	system1.addEquation(equ.equL2Ref);
	system1.addEquation(equ.equSlantIonoL1);
	system1.addEquation(equ.equRecDCB);

		// Setup a solver 
	SolverGenL1L2 solver(system1);
	


}  // End of 'void clkupdUC::solve()'



   // Method that will be executed AFTER initialization but BEFORE processing
void clkupdUC::spinUp()
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
         confReader.open( "clkupdUC.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'clkupdUC.conf'"
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
//   if(updFileListOpt.getCount())
//   {
//      updFileListName = updFileListOpt.getValue()[0];
//   }
   if(ionFileListOpt.getCount())
   {
      ionFileListName = ionFileListOpt.getValue()[0];
   }
//   if(dcbFileOpt.getCount())
//   {
//      dcbFileName = dcbFileOpt.getValue()[0];
//   }
//   if(outputFileOpt.getCount())
//   {
//      outputFileName = outputFileOpt.getValue()[0];
//   }
   if(outputFileListOpt.getCount())
   {
      outputFileListName = outputFileListOpt.getValue()[0];
   }
   if(mscFileOpt.getCount())
   {
      mscFileName = mscFileOpt.getValue()[0];
   }

}  // End of method 'clkupdUC::spinUp()'





   // Method that will be executed AFTER the 'Process'
void clkupdUC::shutDown()
{
//   // do nothing until now!
//      // Solution output stream
//   ofstream solutionFile;
//   solutionFile.open( outputFileName.c_str(), ios::out );
//
//      // Print the header information
//   printBiasHeader( solutionFile );
//
//      // print the clock products as the IGS format
//   printBiasData( solutionFile );
//
//      // close the solution file stream
//   solutionFile.close();
//
//		// Show a message indicating that we've finished calculating upd
//	cout << "UPD solution is in file: " << outputFileName << endl;

}


   // Method to print clock data 
void clkupdUC::printBiasHeader( ofstream& solutionFile)
{

}

   // Method to print clock data 
void clkupdUC::printBiasData( ofstream& solutionFile)
{

}


   // Method to print model values
void clkupdUC::printModel( ofstream& modelfile,
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
      modelfile << static_cast<YDSTime>(time).sod    << "  ";    // SecondsOfDay   #3

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

}  // End of method 'clkupdUC::printModel()'





   // Main function
int main(int argc, char* argv[])
{

   try
   {

      clkupdUC program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if ( !program.initialize(argc, argv, true) )
      {
         return 0;
      }

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
