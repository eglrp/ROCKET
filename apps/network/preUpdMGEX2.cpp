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
//  2014/03/18  
//  Get the initial time using 'YDSTime' class. You MUST set the 
//  'TimeSystem' as well, because the time system is set as 'GPS' 
//  for 'gnssRinex'. if you don't set it as 'GPS', you could 
//  operate the time correctly.  
//
//  2014/03/18  
//  Add 'AmbiguitySmoother' to smooth the ambiguities. you can set 
//  the type using the method 'setSmoothedType()'.  
//
//  2014/03/18  
//  'PhaseCodeAlignment' is used to calibrate the phase with code. 
//  The old class 'PhaseCalibration' is deprecated.  
//  
//  2014/11/14  
//  Comment the 'AmbiguitySmoother' for BLC. We will determine the 
//  UPD for LC epoch by epoch, supposing that the upd has the
//  characteristics of white-noise.
//
//  2014/12/02  
//  Comment the 'AmbiguitySmoother' for Widelane ambiguity
//  smoothing. The experiments show that if the smoothed values
//  are used to determine widelane UPD, the fixing rate of PPP with 
//  the 'smoothed' UPDs is lower that that with the 'epoch-by-epoch'
//  UPDs.
//
//  2014/12/09   
//  The UPD model has two different models:
//  MW/LC <==> LW/LC 
//  L1/L2 <==> L1/LW
//  When the P1/P2 are used, the two class are identical.
//  However, when the C1 is used, 
//  MW/LC, LW/LC  </=> L1/L2, L1,LW
//
//  And, the LW/LC and L1, LW will be the two basic models. 
//  
//  If No constraints are appliced to the PPP model, 
//  in theory, the L1/LW will has fast convergence.
//  However, Owing to the spatial and temporal variations of 
//  ionospheric delays and tropospheric delays, the LW/LC and L1/LW 
//  will converge with different speed. 
//
//  Specially, for the sparse PPP-RTK, the model of LW/LC will
//  have faster convergence than the L1/LW, because of the relative
//  small variation of tropospheric delays than ionospheric delays.
//
//  So, The upd/updx are two basic upd estiamtion for diffent models. 
//
//  2015/08/30   
//  Replace the 'SolverUpdWL' with 'SolverGenWL'
//
//  2015/09/04   
//  Reset the weight of MW, because the MW UPD is not smooth
//
//	 2016/09/09 ~ 2016/11/12
//	 Change the interface of this progrom to the user, with reference to clk.cpp
//	 Add dcb option with reference to clk.cpp
//  Lei Zhao, a Ph.D. candidate 
//	 School of Geodesy and Geomatics, Wuhan University 
//
//  2017/01/08 ~ 2017/01/15
//  Create this program to handle RINEX3 file.
//  GPS only now !!!
//  Lei Zhao, a Ph.D. candidate
//	 School of Geodesy and Geomatics, Wuhan University 
//
//	 2017/01/17
//	 Try to split updMGEX
//  Lei Zhao, a Ph.D. candidate
//	 School of Geodesy and Geomatics, Wuhan University 
//
//	 2017/02/21
//	 preUpdMGEX2, copy of preUpdMGEX	
//	 Try to process Galileo data in rinex3
//  Lei Zhao, a Ph.D. candidate
//	 School of Geodesy and Geomatics, Wuhan University 
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

	// Class for handling observation RINEX3 files
#include "Rinex3ObsStream.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class to store a list of processing objects
#include "ProcessingList.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModelMGEX.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservables.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservablesMGEX.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilterMGEX.hpp"

   // Class for easily changing reference base from ECEF to NEU
#include "XYZ2NEU.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector.hpp"
#include "LICSDetector2.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetectorMGEX.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector2MGEX.hpp"

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

   // Class to compute linear combinations
#include "ComputeLinearMGEX.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinationsMGEX.hpp"

   // Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker2.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker2MGEX.hpp"

   // Class to compute gravitational delays
#include "GravitationalDelay.hpp"

   // Class to align phases with code measurements
#include "PhaseCodeAlignment.hpp"

   // Class to align phases with code measurements
#include "PhaseCodeAlignmentMGEX.hpp"

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
#include "IndepAmbiguityDatum.hpp"

   // Class to smooth the ambiguities
#include "AmbiguitySmoother.hpp"

   // Class to solve a general equation.
#include "SolverGenWL.hpp"

   // Class to solve a general equation.
#include "SolverGenNL.hpp"

   // Class to solve a general equation.
#include "PrefitFilter.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWFilter.hpp"

	// Class to read the MSC data from files
#include "MSCStore.hpp"

	// Class to read the dcb data from files
#include "DCBDataReader.hpp"

	// Class to correct the P1C1 biases for some receivers
//#include "CC2NONCC.hpp"

	// Class to correct the P1C1 biases for some receivers
#include "CC2NONCC2.hpp"

	// Class to store data struture
#include "GNSSDataSerializer.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class updMGEX : public BasicFramework
{
public:

      // Constructor declaration
   updMGEX(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();


      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


      // Method that hold code to be run AFTER processing
//   virtual void shutDown();


      // Method to deal with the preprocessing of the gnss data
//   virtual void preprocessing();
   virtual void preprocessing2();


      // Method to deal with the preprocessing of the gnss data
//   virtual void solve();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

	// Added by Lei Zhao vvv
	CommandOptionWithAnyArg rnxFileListOpt;
	CommandOptionWithAnyArg sp3FileListOpt;
	CommandOptionWithAnyArg clkFileListOpt;
	CommandOptionWithAnyArg eopFileListOpt;
	CommandOptionWithAnyArg dcbFileListOpt;
	CommandOptionWithAnyArg mscFileOpt;
	CommandOptionWithAnyArg outputFileListOpt;

	string rnxFileListName;
	string sp3FileListName;
	string clkFileListName;
	string eopFileListName;
	string dcbFileListName;
	string mscFileName;
	string outputFileListName;

	string outputGDSMapFileName;
	string outputRefStaFileName;
	string outputMasterStaFileName;

		// Dummy source to store ref stations
	sourceDataMap dummySourceDataMap;


	// ^^^ 

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

      // The unit of the UPD to be print out
// string unit;


      // If you want to share objects and variables among methods, you'd
      // better declare them here
      

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 5 );


      // Method to get clock bias products
   void getBiases( const CommonTime& workEpoch,
                   const SolverGenWL& solverUpdWL,
                   const SolverGenNL& solverUpdNL);


      // Method to print clock header 
   void printBiasHeader( ofstream& solutionFile );


      // Method to print clock data 
   void printBiasData( ofstream& solutionFile );


}; // End of 'updMGEX' class declaration



   // Let's implement constructor details
updMGEX::updMGEX(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data to determine the UPDs.\n\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('updMGEX.conf' by default).",
             false ),
	// Added by Lei Zhao vvv
	rnxFileListOpt( 'r',
						 "rnxFileList",
						 "file storing a list of rinex file names ",
						 true),
	mscFileOpt(     'm',
						 "mscFile",
						 "file storing monitor station coordinates ",
						  true),
	clkFileListOpt( 'k',
						 "clkFileList",
						 "file storing a list of CLK file names ",
						 false ),
	sp3FileListOpt( 's',
						 "sp3FileList",
						 "file storing a list of SP3 file names",
						 true),
	eopFileListOpt( 'e',
						 "eopFileList",
						 "file storing a list of IGS erp file name ",
						 true ),
	dcbFileListOpt( 'D',
						  "dcbFileList",
						  "file storing a list of the P1C1 DCB file.",
						  false),
	outputFileListOpt(  'O',
						 "outputFile",
						 "file storing the computed clock data",
						  true )
	// ^^^ 
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'updMGEX::updMGEX'



   // Method that will be executed AFTER initialization but BEFORE processing
void updMGEX::spinUp()
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
         confReader.open( "updMGEX.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'updMGEX.conf'"
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

	// Added by Lei Zhao vvv
	if(rnxFileListOpt.getCount())
	{
		rnxFileListName = rnxFileListOpt.getValue()[0];
	}
	if(mscFileOpt.getCount())
	{
		mscFileName = mscFileOpt.getValue()[0];
	}
	if(clkFileListOpt.getCount())
	{
		clkFileListName = clkFileListOpt.getValue()[0];
	}
	if(sp3FileListOpt.getCount())
	{
		sp3FileListName = sp3FileListOpt.getValue()[0];
	}
	if(eopFileListOpt.getCount())
	{
		eopFileListName = eopFileListOpt.getValue()[0];
	}
	if(outputFileListOpt.getCount())
	{
		outputFileListName = outputFileListOpt.getValue()[0];
	}
	if(dcbFileListOpt.getCount())
	{
		dcbFileListName = dcbFileListOpt.getValue()[0];
	}
	// ^^^ 

}  // End of method 'updMGEX::spinUp()'



   // Method that will really process information
void updMGEX::process()
{
       // Preprocessing the gnss data 
   preprocessing2();

       // Generate and solve equations
   //solve();
}



   // Method that will really process information
void updMGEX::preprocessing2()
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

		// Now read SP3 files from 'SP3FileList'
	ifstream sp3FileListStream;

	sp3FileListStream.open( sp3FileListName.c_str(), ios::in );
	if(!sp3FileListStream)
	{
			// if file doesn't exist, issue a warning
		cerr << "SP3 file List name " << sp3FileListName << " doesn't exist or you don't"
			  << " have permission to read it. Terminate the program." << endl;  
		exit(-1);
	}

      // Load all the SP3 ephemerides files from sp3 file list
   string sp3File;
	while ( sp3FileListStream >> sp3File )
	{
		try
		{
			SP3EphList.loadFile( sp3File );
		}
		catch ( FileMissingException& e )
		{
				// If file doesn't exist, issue a warning
			cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
				  << "have permission to read it. Skipping it." << endl;
			continue;
		}
	}  // End of ' while ( sp3FileListStream >> sp3File ) '

		// Close SP3 list file 
	sp3FileListStream.close();


//      // Load all the SP3 ephemerides files from variable list
//   string sp3File;
//   while ( (sp3File = confReader.fetchListValue("SP3List", "DEFAULT" ) ) != "" )
//   {
//
//         // Try to load each ephemeris file
//      try
//      {
//         SP3EphList.loadFile( sp3File );
//      }
//      catch (...)
//      {
//            // If file doesn't exist, issue a warning
//         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
//              << "have permission to read it. Skipping it." << endl;
//
//         exit(-1);
//      }
//
//   }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "


		/////////////////////////
	   // Let's read clock files
	   /////////////////////////

		// If rinex clock file list is given, then use rinex clock
	if( clkFileListOpt.getCount() )
	{
			// Now read clk files from 'clkFileList'
		ifstream clkFileListStream;

			// Open clkFileList file
		clkFileListStream.open( clkFileListName.c_str(), ios::in );
		if( !clkFileListStream )
		{
				// If file doesn;t exist, issue a warning
			cerr << "clock file List Name'" << clkFileListName << "' doesn't exist or you don't "
				  << "have permission to read it. Terminate the program." << endl;
			exit(-1);
		}  // End of ' if( !clkFileListStream ) '

		string clkFile;
		while( clkFileListStream >> clkFile )
		{
			try
			{
				SP3EphList.loadRinex3ClockFile( clkFile );
			}
			catch( FileMissingException& e)
			{
				cerr << "rinex CLK file '" << clkFile << "' doesn't exist or you don't "
					  << "have permission to read it. Skipping it. " << endl;
				continue;
			}
		}  // End of ' while( clkFileListStream >> clkFile ) '

			// Close clklist file
		clkFileListStream.close();
	}  // End of ' if( clkFileListOpt.getCount() ) '



   //// ^^^^ Ephemeris handling ^^^^

      // Declare a "EOPDataStore" object to handle earth rotation parameter file
   EOPDataStore eopStore;

		// Now read eop files from 'eopFileList'
	ifstream eopFileListStream;

		// Open eopFileList File
	eopFileListStream.open( eopFileListName.c_str(), ios::in );
	if( !eopFileListStream )
	{
		cerr << "erp file List Name'" << eopFileListName << "' doesn't exist or yo     u don't "
			  << "have permission to read it. Terminate the program" << endl;
		exit(-1);
	}  // End of ' if( !eopFileListStream ) '

	string eopFile;
	while( eopFileListStream >> eopFile )
	{
		try
		{
			eopStore.loadIGSFile( eopFile );
		}
		catch (FileMissingException& e)
		{
			cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
				  << "have permission to read it. Skipping it." << endl;
			continue;
		}
	}
		// close eopList file
	eopFileListStream.close();



		// Declare a "MSCStore" object to handle msc file 
	MSCStore mscStore;

	try
	{
		mscStore.loadFile( mscFileName );
	}
	catch( gpstk::FFStreamError& e)
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

	
		// Load the satellite system info user chose 
	set<string> satSystemInfo;
	string sys;
	while( (sys = confReader.fetchListValue( "SatelliteSystemList") ) != "" )
	{
		satSystemInfo.insert( sys );
	}

//			// Debug code vvv
//		for( set<string>::iterator it = satSystemInfo.begin(); 
//			  it != satSystemInfo.end();
//			  ++it )
//		{
//			cout << *it << endl;
//		}
//			// Debug code ^^^ 

		// Satellite system flag
	bool employGPS(false);
	bool employGLONASS(false);
	bool employGALILEO(false);
		
		// Output employed system
	cout << "Employed sat system: ";
	if( satSystemInfo.find("GPS") != satSystemInfo.end() )
	{
			// GPS is employed by the user
		cout << "GPS" << " ";
		employGPS = true;
	}
	if( satSystemInfo.find("GLONASS") != satSystemInfo.end() )
	{
			// GLONASS is employed by the user
		cout << "Glonass" << " ";
		employGLONASS = true;
	}
	if( satSystemInfo.find("GALILEO") != satSystemInfo.end() )
	{
			// GALILEO is employed by the user
		cout << "Galileo" << " " <<  endl;
		employGALILEO = true;
	}

		// New line
	cout << endl;


	////////////////////////////////////////////////////////////////
	/// Load other common configuration for all the station from 'updMGEX.conf' 
	////////////////////////////////////////////////////////////////

		// The Year/DayOfYear/SecOfYear format, you MUST set the timeSystem at
		// the same time.
	YDSTime yds( confReader.getValueAsInt( "year" ),
					 confReader.getValueAsInt( "dayOfYear" ),
					 confReader.getValueAsDouble( "secOfDay" ), TimeSystem::GPS );

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



	//***********************
	// Let's define common objects used for all stations 
	//***********************

		// Object to compute tidal effects
	SolidTides solid;

		// Configure ocean loading model
	OceanLoading ocean(blqStore);

		// Object to model pole tides
	PoleTides pole(eopStore);

		// Specify output file name
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




   //// Starting preprocessing for ALL stations ////

	// Changed by Lei Zhao vvv 
	vector<string> rnxFileListVec;

		// Now read rinex file from 'rnxFileList'
	ifstream rnxFileListStream;
	rnxFileListStream.open( rnxFileListName.c_str(), ios::in);
	if( !rnxFileListStream )
	{
		cerr << "rinex file List Name'" << rnxFileListName << "' doesn't exist or you don't "
				<< "have permission to read it. Skipping it." << endl;
		exit(-1);
	}
	
	string rnxFile;
	while( rnxFileListStream >> rnxFile )
	{
		rnxFileListVec.push_back( rnxFile );
	}  // End of ' while( rnxFileListStream >> rnxFile ) '

		// Close file
	rnxFileListStream.close();

	if( rnxFileListVec.size() == 0 )
	{
		cerr << rnxFileListName  << "rnxFileList is empty!! "
			  << endl;
		exit(-1);
	}  // End of ' if( rnxFileListVec.size() == 0 ) '



	vector<string>::const_iterator rnxit = rnxFileListVec.begin();
	while( rnxit != rnxFileListVec.end() )
	{
			// Read rinex file from the vector
		string rnxFile = (*rnxit);


	// ^^^
         // Show a message indicating that we are starting with this station
      //cout << "Starting processing for station: '" << station << "'." << endl;
      //cout << "Starting processing for station: '" << rnxFile << "'." << endl;


         // Create input observation file stream
      Rinex3ObsStream rin;

         // Enable exceptions
      rin.exceptions(ios::failbit);

         // Try to open Rinex observations file
      try
      {

            // Open Rinex observations file in read-only mode
         //rin.open( confReader("rinexObsFile", station), std::ios::in );
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

			//////////////////////
			// Let's read the header firstly!!!!
			//////////////////////
		Rinex3ObsHeader roh;
		try
		{
			rin >> roh;

		}
		catch( FFStreamError& e )
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

		}  // End of 'try-catch' block

		
		CommonTime initialTime( yds.convertToCommonTime() );


			// Get the station name for current rinex file
		string station = roh.markerName;

         // Let's check the ocean loading data for current station before
         // the real data processing.
		bool validOceanLoadingSta( blqStore.isValid(station) );
//      if( ! blqStore.isValid(station) )
		if( !validOceanLoadingSta )
      {
         cout << "There is no BLQ data for current station:" << station << endl;
//         cout << "Current staion will be not processed !!!!" << endl;
//         continue;
      }



      cout << "Starting processing for station: '" << station << "'." << endl;

			// MSC data for this station
		initialTime.setTimeSystem(TimeSystem::Unknown);
		MSCData mscData;

		try
		{
			mscData = mscStore.findMSC( station, initialTime );
		}
		catch( InvalidRequest& ie)
		{
			cerr << "The station " << station << " isn't included in MSC file."
				  << endl;

			// Warning:you must increase the iterator to process the next file,
			// or else the program will repeat opening this file
         rin.close();
         ++rnxit;
			continue;
		}  // End of 'try-catch' block


			// Now, Let's change the system to GPS
		initialTime.setTimeSystem(TimeSystem::GPS);

			// Read inital position from given msc data store.
		Position nominalPos( mscData.coordinates );


//		// debug code vvv
//		std::cout << "station: " << station << " X Y Z: " 
//						<< nominalPos << std::endl;
//		// debug code ^^^

         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;

			//>This object will correct the P1C1 dcb for some receivers
			//>defined in the 'recTypeFile'.

		CC2NONCC2 cc2noncc;

		if( employGPS )
		{
			if(dcbFileListOpt.getCount())  // There is a dcb file
			{
	
				cc2noncc.setDCBDataReader(dcbStore);
	
					// Read the receiver type file.
				cc2noncc.loadRecTypeFile( confReader.getValue("recTypeFile") );
				cc2noncc.setRecType(roh.recType);
		
			}  // End of ' if(dcbFileListOpt.getCount())  '
	
			cc2noncc.setCopyC1ToP1(true);
			}
			// Add tp processing list
		pList.push_back(cc2noncc);



         // This object will check that all required observables are present
		RequireObservablesMGEX requireObs;
		
		if( employGPS )
		{
	      requireObs.addRequiredType(SatID::systemGPS, TypeID::P1);
	      requireObs.addRequiredType(SatID::systemGPS, TypeID::P2);
	      requireObs.addRequiredType(SatID::systemGPS, TypeID::L1);
	      requireObs.addRequiredType(SatID::systemGPS, TypeID::L2);
		}

		if( employGALILEO )
		{
	      requireObs.addRequiredType(SatID::systemGalileo, TypeID::C1);
	      requireObs.addRequiredType(SatID::systemGalileo, TypeID::C5);
	      requireObs.addRequiredType(SatID::systemGalileo, TypeID::L1);
	      requireObs.addRequiredType(SatID::systemGalileo, TypeID::L5);
		}

         // Add 'requireObs' to processing list
      pList.push_back(requireObs);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilterMGEX pObsFilter;

		if( employGPS )
		{
			pObsFilter.addFilteredType( SatID::systemGPS, TypeID::P1 );
			pObsFilter.addFilteredType( SatID::systemGPS, TypeID::P2 );
		}

		if( employGALILEO )
		{
			pObsFilter.addFilteredType( SatID::systemGalileo, TypeID::C1 );
			pObsFilter.addFilteredType( SatID::systemGalileo, TypeID::C5 );
		}


         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // This object defines several handy linear combinations
      LinearCombinationsMGEX comb;

			// Object to compute linear combinations for cycle slip detection
      ComputeLinearMGEX linear1;
		if( employGPS )
		{
			linear1.addLinear(SatID::systemGPS, comb.pdeltaCombinationGPS);
			linear1.addLinear(SatID::systemGPS, comb.mwubbenaCombinationGPS);
			linear1.addLinear(SatID::systemGPS, comb.ldeltaCombinationGPS);
			linear1.addLinear(SatID::systemGPS, comb.liCombinationGPS);
		}
		if( employGALILEO )
		{
			linear1.addLinear(SatID::systemGalileo, comb.pdeltaCombinationGalileo);
			linear1.addLinear(SatID::systemGalileo, comb.mwubbenaCombinationGalileo);
			linear1.addLinear(SatID::systemGalileo, comb.ldeltaCombinationGalileo);
			linear1.addLinear(SatID::systemGalileo, comb.liCombinationGalileo);
		}
      pList.push_back(linear1);       // Add to processing list


         // Declare a basic modeler
      BasicModelMGEX basic(nominalPos, SP3EphList);

         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation"));

         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
			// We need to set the type according to satellite system
		if( employGPS )
		{
			basic.setDefaultObservable( SatID::systemGPS, TypeID::P1 );
		}

		if( employGALILEO )
		{
			basic.setDefaultObservable( SatID::systemGalileo, TypeID::C1);	
		}
         // Add to processing list
      pList.push_back(basic);

         // Objects to mark cycle slips
      LICSDetectorMGEX   markCSLI ;         // Checks LI cycle slips
      pList.push_back(markCSLI );       // Add to processing list
      MWCSDetector2MGEX markCSMW;            // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);        // Add to processing list


         // Object to keep track of satellite arcs
         // Notes: delete unstable satellite may cause discontinuity
         //        in the network processing!
      SatArcMarker2MGEX markArc;
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
		Triple offsetARP( roh.antennaDeltaHEN );

         // Declare some antenna-related variables
      AntexReader antexReader;
      Antenna receiverAntenna;

         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex" ) );
      string antennaModel;
      if( useantex )
      {
            // Read antex file
         antexFile = confReader.getValue( "antexFile" );

            // Feed Antex reader object with Antex file
         antexReader.open( antexFile );

            // Antenna model 
			antennaModel = roh.antType;
            // Get receiver antenna parameters
            // Warning: If no corrections are not found for one specific 
            //          radome, then the antenna with radome NONE are used.
         try
         {
					// Debug code vvv
				cout << "antennaModel: " << antennaModel << endl;
					// Debug code ^^^ 

            receiverAntenna = antexReader.getAntenna( antennaModel );

					
         }
         catch(ObjectNotFound& notFound)
         {
					
					// Debug code vvv
				cout << "notFound: " << antennaModel
						<< " replace by NONE " << endl; 
					// Debug code ^^^ 

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
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns"));
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim" ));
      }
  //    else
  //    {
  //          // Fill vector from antenna ARP to L1 phase center [UEN], in meters
  //       offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1", station);
  //       offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1", station);
  //       offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1", station);

  //          // Vector from antenna ARP to L2 phase center [UEN], in meters
  //       offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2", station);
  //       offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2", station);
  //       offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2", station);

  //       corr.setL1pc( offsetL1 );
  //       corr.setL2pc( offsetL2 );

  //    }
      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos,
                            confReader.getValue( "satDataFile" ) );
      pList.push_back(windup);       // Add to processing list



         // Declare a NeillTropModel object, setting its parameters
//      NeillTropModel neillTM( nominalPos.getAltitude(),
//                              nominalPos.getGeodeticLatitude(),
//                              confReader.getValueAsInt("dayOfYear") );

		NeillTropModel neillTM( nominalPos,
											initialTime ); 
         // We will need this value later for printing
      double drytropo( neillTM.dry_zenith_delay() );


         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         // Object to compute code combination with minus ionospheric delays
      ComputeLinearMGEX linear2;

		if( employGPS )
		{
			linear2.addLinear( SatID::systemGPS, comb.q1CombinationGPS);
			linear2.addLinear( SatID::systemGPS, comb.q2CombinationGPS);
		}
		if( employGALILEO )
		{
			linear2.addLinear( SatID::systemGalileo, comb.q1CombinationGalileo);
			linear2.addLinear( SatID::systemGalileo, comb.q5CombinationGalileo);
		}
      pList.push_back(linear2);       // Add to processing list


         // Object to align phase with code measurements
     	PhaseCodeAlignmentMGEX phaseAlignL1;
      PhaseCodeAlignmentMGEX phaseAlignL2;

      PhaseCodeAlignmentMGEX phaseAlignGALL1;
      PhaseCodeAlignmentMGEX phaseAlignGALL5;

		if( employGPS )
		{
			phaseAlignL1.setSatSystem( SatID::systemGPS);
	      phaseAlignL1.setCodeType(TypeID::Q1);
	      phaseAlignL1.setPhaseType(TypeID::L1);
	      phaseAlignL1.setPhaseWavelength( 0.190293672798);
	
	      pList.push_back(phaseAlignL1);       // Add to processing list
	
	         // Object to align phase with code measurements
//	      PhaseCodeAlignment phaseAlignL2;
			phaseAlignL2.setSatSystem( SatID::systemGPS);
	      phaseAlignL2.setCodeType(TypeID::Q2);
	      phaseAlignL2.setPhaseType(TypeID::L2);
	      phaseAlignL2.setPhaseWavelength( 0.244210213425);
	      pList.push_back(phaseAlignL2);       // Add to processing list
		}

		if( employGALILEO )
		{
			phaseAlignGALL1.setSatSystem( SatID::systemGalileo );
			phaseAlignGALL1.setCodeType( TypeID::Q1 );
			phaseAlignGALL1.setPhaseType( TypeID::L1 );	
			phaseAlignGALL1.setPhaseWavelength( L1_WAVELENGTH_GAL );
			pList.push_back( phaseAlignGALL1 );

			phaseAlignGALL5.setSatSystem( SatID::systemGalileo );
			phaseAlignGALL5.setCodeType( TypeID::Q5 );
			phaseAlignGALL5.setPhaseType( TypeID::L5 );	
			phaseAlignGALL5.setPhaseWavelength( L5_WAVELENGTH_GAL );
			pList.push_back( phaseAlignGALL5 );
		}


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinearMGEX linear3;

		if( employGPS )
		{
			linear3.addLinear( SatID::systemGPS, comb.pcCombinationGPS );
			linear3.addLinear( SatID::systemGPS, comb.lcCombinationGPS );
		}

		if( employGALILEO )
		{
			linear3.addLinear( SatID::systemGalileo, comb.pcCombinationGalileo );
			linear3.addLinear( SatID::systemGalileo, comb.lcCombinationGalileo );
		}
      pList.push_back(linear3);       // Add to processing list

         // Declare a simple filter object to screen PC
      SimpleFilterMGEX pcFilter;

		if( employGPS )
		{
			pcFilter.setFilteredType( SatID::systemGPS, TypeID::PC );
		}

		if( employGALILEO )
		{
			pcFilter.setFilteredType( SatID::systemGalileo, TypeID::PC );
		}


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

         // Now, Compute the new MWubbena combinations
      ComputeLinearMGEX linear5;

		if( employGPS )
		{
			linear5.addLinear( SatID::systemGPS, comb.mwubbenaCombinationGPS );
		}
		if( employGALILEO )
		{
			linear5.addLinear( SatID::systemGalileo, 
									 comb.mwubbenaCombinationGalileo );
		}
      pList.push_back(linear5);       // Add to processing list


         // Object to compute prefit-residuals for PC and LC combination
         // @author Shoujian Zhang
      ComputeLinearMGEX linear4;

		if( employGPS )
		{
			linear4.addLinear( SatID::systemGPS, comb.pcPrefitGPS);
			linear4.addLinear( SatID::systemGPS, comb.lcPrefitGPS);
		}

		if( employGALILEO )
		{
			linear4.addLinear( SatID::systemGalileo, comb.pcPrefitGalileo);
			linear4.addLinear( SatID::systemGalileo, comb.lcPrefitGalileo);
		}

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
      bool isNEU( confReader.getValueAsBoolean( "USENEU" ) );

         // Declare ppp solver
      SolverPPP pppSolver(isNEU);

		bool fixCoorsFlag( confReader.getValueAsBoolean( "fixCoordinate" ) );
		pppSolver.fixCoordinates( fixCoorsFlag );

         // Add to processing list
      pList.push_back(pppSolver);

         // Now, Smooth the MWubbena 

         // Declare solver objects to smooth the MWubbena combinations
      MWFilter mwfilter;
      pList.push_back(mwfilter);


      double decimateInterv(
               confReader.getValueAsDouble( "decimationInterval") );
		double decimateTol(
					confReader.getValueAsDouble( "decimationTolerance"));
      double sampleInterv(
               confReader.getValueAsDouble( "sampleInterval") );

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
//      YDSTime yds( confReader.getValueAsInt("year", station),
//                   confReader.getValueAsInt("dayOfYear", station),
//                   confReader.getValueAsDouble("secOfDay", station), TimeSystem::GPS );
//      CommonTime initialTime( yds.convertToCommonTime() );


         // Object to decimate data
      Decimate decimateData( decimateInterv,
									  decimateTol,
									  initialTime );
      pList.push_back(decimateData);       // Add to processing list


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;

         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision") );

         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel" ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
				// Get the path for model file
			string modelFilePath( confReader.getValue( "modelFilePath" ) );

				// Get the basename od rnxFile
				// Referenced from ssc2msc.cpp written by KM. Zhu
				
				// find last of '/'
			int where =  rnxFile.find_last_of('/');

				// find the dot in filename
//			int dot = rnxFile.find_last_of('.');

			string baseRnxFile( rnxFile.substr(where+1, rnxFile.length()-1 ) );

         //modelName = confReader.getValue( "modelFile", station );
			modelName = modelFilePath + baseRnxFile + ".model";

			// Debug code vvv
			cout << "modleName: " << modelName << endl;
			// Debug code ^^^ 

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

				// Get ocean loading
			Triple oceanLoad(0.0, 0.0, 0.0);
			if( validOceanLoadingSta )
			{
				oceanLoad = ocean.getOceanLoading( station, time ); 
			}


            // Compute solid, oceanic and pole tides effects at this epoch
         Triple tides( solid.getSolidTide( time, nominalPos )  +
							  oceanLoad +
//                       ocean.getOceanLoading( station, time )  +
                       pole.getPoleTide( time, nominalPos )    );

            // Update observable correction object with tides information
         corr.setExtraBiases(tides);

         try
         {

				SatSystemSet satSysSet;
				
					// We should insert sat system according to user's choice
				if( employGPS )
				{
					satSysSet.insert( SatID::systemGPS );
				}

				if( employGALILEO )
				{
					satSysSet.insert( SatID::systemGalileo );
				}

					// Delete sat system not in 'satSysSet' to save the memory
				gRin.keepOnlySatSystem( satSysSet );


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


//         // Let's check what kind of station this is
//      if( confReader.getValueAsBoolean( "masterStation", station ) )
//      {
//         master = source;
//      }
//      else
//      {
//         if( confReader.getValueAsBoolean( "roverStation", station ) )
//         {
//            rover = source;
//         }
//         else
//         {
//            if( confReader.getValueAsBoolean( "refStation", station ) )
//            {
//                  // Note that 'reference' stations form a set
//               refStationSet.insert( source );
//            }
//         }
//      }
		if( confReader.getValue( "masterStation" ) ==  station )
		{
			master = source;
		}
		else
		{
			satTypeValueMap dummyMap;
			dummySourceDataMap[source] = dummyMap;

//			refStationSet.insert( source );
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

			// Go to process the next station
		++rnxit;

   }  // End of 'while ( (station = confReader.getEachSection()) != "" )'


   //// End of preprocessing for ALL stations ////
		// Now store gdsMap, 
		//				 refStationSet
		//				 master 
		//	using GDSSerializer class
	GDSSerializer::serializeToFile<gnssDataMap>(gdsMap, outputGDSMapFileName);
	GDSSerializer::serializeToFile<sourceDataMap>(dummySourceDataMap, outputRefStaFileName);
	GDSSerializer::serializeToFile<SourceID>(master, outputMasterStaFileName);
//



      // Clear content of SP3 ephemerides object
   SP3EphList.clear();


//	// debug code vvv
//	exit(-1);
//	// debug code ^^^
      // The rest of the processing will be in method 'updMGEX::shutDown()'
   return;

}  // End of 'updMGEX::process()'


   // Method to print model values
void updMGEX::printModel( ofstream& modelfile,
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

}  // End of method 'updMGEX::printModel()'


   // Method to extract the solutions from solverGen and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printBiasData'
void updMGEX::getBiases( const CommonTime& workEpoch,
                     const SolverGenWL& solverUpdWL,
                     const SolverGenNL& solverUpdNL)
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
         updLC = solverUpdNL.getFixedSolution(TypeID::updLC, *itSource);
         updMW = solverUpdWL.getFixedSolution(TypeID::updMW, *itSource);

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
         updSatLC = solverUpdNL.getFixedSolution(TypeID::updSatLC, *itSat);
         updSatMW = solverUpdWL.getFixedSolution(TypeID::updSatMW, *itSat);

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

//   while(begPos != endPos)
//   {
//       cout << begPos->second << endl;
//       ++ begPos;
//   }

}  // End of method 'updMGEX::getBiasesWL()'


   // Method that hold code to be run AFTER processing
//void updMGEX::shutDown()
//{
//      // Solution file
//   //string solutionName;
//   //solutionName = confReader.getValue( "solutionFile", "DEFAULT" );
//
//
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
//
//
//}  // End of 'upd::shutDown()'


   // Method to print clock data 
void updMGEX::printBiasHeader( ofstream& solutionFile)
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
   const string unitString            = "UPD UNIT";
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

//    // "RINEX VERSION / TYPE"
// headerRecord  = rightJustify(unit, 10);
// headerRecord += string(50,' ');
// headerRecord += unitString;         
// solutionFile << headerRecord << endl;

      // "END OF HEADER"
   headerRecord = string(60,' ');
   headerRecord+= endOfHeaderString;     // "END OF HEADER"
   solutionFile << headerRecord << endl;

}  // End of printBiasHeader


   // Method to print clock data 
void updMGEX::printBiasData( ofstream& solutionFile)
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


}  // End of method 'updMGEX::printBiasData()'



   // Main function
int main(int argc, char* argv[])
{
   try
   {
      updMGEX program(argv[0]);

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
