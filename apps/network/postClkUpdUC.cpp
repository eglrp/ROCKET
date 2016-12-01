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

	// Class to time conversion between different time system
#include "Epoch.hpp"

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
	virtual void preprocessing2();

		// Method to deal with the preprocessing of the gnss data
	virtual void solve();  


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

//      // Option for rinex file list
//   CommandOptionWithAnyArg rnxFileListOpt;
//
//      // Option for sp3 file list
//   CommandOptionWithAnyArg sp3FileListOpt;
//
//      // Option for clk file list
//   CommandOptionWithAnyArg clkFileListOpt;
//
//      // Option for eop file list
//   CommandOptionWithAnyArg eopFileListOpt;
//
//      // Option for eop file list
////   CommandOptionWithAnyArg updFileListOpt;
//
//      // Option for eop file list
//   CommandOptionWithAnyArg ionFileListOpt;
//
//      // Option for p1c1 dcb file 
////   CommandOptionWithAnyArg dcbFileOpt;
//
//      // Option for monitor coordinate file
//   CommandOptionWithAnyArg mscFileOpt;

      // Option for output file
   //CommandOptionWithAnyArg outputFileOpt;
   CommandOptionWithAnyArg outputFileOpt;
	CommandOptionWithAnyArg inputFileListOpt;

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
   string outputFileName;
	string inputFileListName;

	string inputGDSMapFileName;
	string inputRefStaFileName;
	string inputMasterStaFileName;

		// Dummy source to store ref stations
	sourceDataMap dummySourceDataMap;

      // Configuration file reader
   ConfDataReader confReader;


      // Declare our own methods to handle output
   gnssDataMap gdsMap;

		// Mulitmap used to store the clock data line
	std::multimap<CommonTime, std::string> solutionMap;

		// Master Station
	SourceID master;

		// Source sets for all the estimated receiver clocks
	SourceIDSet sourceSet;

		// Satellite sets for all the estimated satellite clocks
	SatIDSet satSet;

		// dcb program used in the header printing
	string dcbProgram;

		// antex file
	string antexFile;

		// dcb source used in the header printing
	string dcbSource;
		
		// Reference stations set 
	std::set<SourceID> refStationSet;

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 4 );

		// Method to get estimated results
	void getBiases( const CommonTime& workEpoch, 
						 const SolverGenL1L2& solverUpdL1L2 );

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
//   rnxFileListOpt( 'r',
//                   "rnxFileList",
//   "file storing a list of rinex file name ",
//                   true),
//   sp3FileListOpt( 's',
//                   "sp3FileList",
//   "file storing a list of rinex SP3 file name ",
//                   true),
//   clkFileListOpt( 'k',
//                   "clkFileList",
//   "file storing a list of rinex clk file name ",
//                   false),
////   updFileListOpt( 'u',
////                   "updFileList",
////   "file storing a list of UPD file name ",
////                   true),
//   ionFileListOpt( 'i',
//                   "ionFileList",
//   "file storing a list of ion file name ",
//                   true),
//   eopFileListOpt( 'e',
//                   "eopFileList",
//   "file storing a list of IGS erp file name ",
//                   true),
////   dcbFileOpt(    'd',
////                  "dcbFile",
////   "file storing the p1c1 dcb file name ",
////                   false),
////   outputFileOpt( 'o',
////                   "outputFile",
////   "file storing the list of output file name ",
////                   false),
   outputFileOpt( 'o',
                   "outputFileNameList",
   "file storing the list of output file name ",
                   true),
	inputFileListOpt( 'i',
							"inputFile",
							"file storing the input files",
							true )
//   mscFileOpt( 'm',
//               "mscFile",
//   "file storing monitor station coordinates ",
//               true)
{
      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clkupdUC::clkupdUC'


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

//      // Now, Let's parse the command line
//   if(rnxFileListOpt.getCount())
//   {
//      rnxFileListName = rnxFileListOpt.getValue()[0];
//   }
//   if(sp3FileListOpt.getCount())
//   {
//      sp3FileListName = sp3FileListOpt.getValue()[0];
//   }
//   if(clkFileListOpt.getCount())
//   {
//      clkFileListName = clkFileListOpt.getValue()[0];
//   }
//   if(eopFileListOpt.getCount())
//   {
//      eopFileListName = eopFileListOpt.getValue()[0];
//   }
//   if(updFileListOpt.getCount())
//   {
//      updFileListName = updFileListOpt.getValue()[0];
//   }
//   if(ionFileListOpt.getCount())
//   {
//      ionFileListName = ionFileListOpt.getValue()[0];
//   }
//   if(dcbFileOpt.getCount())
//   {
//      dcbFileName = dcbFileOpt.getValue()[0];
//   }
   if(outputFileOpt.getCount())
   {
      outputFileName = outputFileOpt.getValue()[0];
   }
	if(inputFileListOpt.getCount())
	{
		inputFileListName = inputFileListOpt.getValue()[0];
	}
//   if(outputFileListOpt.getCount())
//   {
//      outputFileListName = outputFileListOpt.getValue()[0];
//   }
//   if(mscFileOpt.getCount())
//   {
//      mscFileName = mscFileOpt.getValue()[0];
//   }

}  // End of method 'clkupdUC::spinUp()'


	// Method that will really process information
void clkupdUC::process()
{
       // Preprocessing the gnss data 
   preprocessing2();

       // Generate and solve equations
   solve();
}  // End of 'void clkupdUC::process()'


   // Method that will really process information
void clkupdUC::preprocessing2()
{

	// Specify input file name
	vector<string> inputFileListVec;

		// Now read output files from 'outputFileListName'
	ifstream inputFileListStream;
	inputFileListStream.open(inputFileListName.c_str(), ios::in);	
	if( !inputFileListStream )
	{
		cerr << "output file list name: '" << inputFileListName 
				<< "' doesn't exist" << endl;
		exit(-1);
	}

	string inputFileStr;
	while( inputFileListStream >> inputFileStr )
	{
		inputFileListVec.push_back( inputFileStr );
	}

	inputFileListStream.close();

	if( inputFileListVec.size() == 0 )
	{
		cerr << "'" << inputFileListName << "' is empty" << endl;
	}

	inputGDSMapFileName = inputFileListVec[0];
	inputRefStaFileName = inputFileListVec[1];
	inputMasterStaFileName = inputFileListVec[2];


		// Read the input information using GDSSerializer class

	GDSSerializer::deserializeFromFile<SourceID>(master, inputMasterStaFileName);


	cout << "Reading input files ... " << endl;

	GDSSerializer::deserializeFromFile<gnssDataMap>(gdsMap, inputGDSMapFileName);
  
	sourceDataMap sdMap;
	GDSSerializer::deserializeFromFile<sourceDataMap>(sdMap, inputRefStaFileName);
	refStationSet = sdMap.getSourceIDSet();

	
}  // End of ' void upd::preprocessing2() '




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
	
	system1.addEquation(equ.equP1Master);
	system1.addEquation(equ.equP2Master);
	system1.addEquation(equ.equL1Master);
	system1.addEquation(equ.equL2Master);
	system1.addEquation(equ.equP1Ref);
	system1.addEquation(equ.equP2Ref);
	system1.addEquation(equ.equL1Ref);
	system1.addEquation(equ.equL2Ref);
	system1.addEquation(equ.equSlantIonoL1);
//	system1.addEquation(equ.equRecDCB);

		// Setup a solver 
	cout << "hi, I am solver" << endl;
	SolverGenL1L2 solver(system1);

	TypeID ambType1(TypeID::BL1), ambType2(TypeID::BL2);

	solver.add2AmbTypeSet(ambType1);
	solver.add2AmbTypeSet(ambType2);

			// Add amb Type to solver 
//	solver.add2AmbTypeSet(Variable::BL1);
//	solver.add2AmbTypeSet(Variable::BL2);
//	solver.Process( gdsMap );

   ////> Variables for the print out 

//      // Prepare for printing
//   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( 2 );

	// Debug code vvv
	int count(0);
	// Debug code ^^^ 
      // Repeat while there is preprocesed data available
   while( !gdsMap.empty() )
   {
         //*Solve the system equation 

         // Get data out of GDS, but only the first epoch
      gnssDataMap gds( gdsMap.frontEpoch() );

			// Debug code ^^^
//		cout << " present gdsMap: " << endl;
//		gds.dump(cout);
			// Debug code ^^^

		   // Remove first element (i.e., we remove the first epoch)
      gdsMap.pop_front_epoch();

         // Extract current epoch
      CommonTime workEpoch( (*gds.begin()).first );

         // WARNING!!!!!!!!!!!!!!!!!!!!!!
         // WARNING!!!!!!!!!!!!!!!!!!!!!!
         // WARNING!!!!!!!!!!!!!!!!!!!!!!
         //
         // Firstly, you must check whether the observed satellite number 
         // of the master stationis zero? if true,  skip this epoch, 
         // or else, the equation system may be singular.

      gnssRinex gMaster( gds.getGnssRinex(master) );

		   // The satellite of the master can't be zero
         // IF it equals with zero, then skip this epoch
      if( gMaster.numSats() == 0 )
      {
         cout << "Reset of the filter because the satellite number of \
master station is zero." << endl;
            // Jump to the next epoch
         continue;
      }
      
      solver.Process(gds);

		// Debug code vvv
		if( (++count) > 3 )
		{
			break;
		}
		// Debug code ^^^ 
//		break;

			// Get solutions
		getBiases( workEpoch, solver );

   }  // End of 'while( !gdsMap.empty() )'

   /// We are done ////

}  // End of 'void clkupdUC::solve()'


	// Method to extract the solutions from SolverGenL1L2 and 
	// temprorarily store them in the string, which will be 
	// print out to the solutionfile in the method 'printBiasData'
void clkupdUC::getBiases( const CommonTime& workEpoch, 
						 const SolverGenL1L2& solverUpdL1L2 )
{
		// Mulitmap used to store the clock data line 
	std::string solutionRecord;

		// Satellite sets for all the estimated satellite clocks
	SatIDSet currentSatSet;

		// Source sets for all the estimated receiver clocks
	SourceIDSet currentSourceSet;

		// Clock bias
	double updL1;
	double updL2;

		//*Now, read and print the receiver clock data record

		// Read receiver clocks from the stateMap
	currentSourceSet = solverUpdL1L2.getCurrentSources();

		// Insert the current sources into sourceSet
	sourceSet.insert(currentSourceSet.begin(), currentSourceSet.end());

		// Read the receiver clocks for the current sources
	for( SourceIDSet::iterator itSource = currentSourceSet.begin();
		  itSource != currentSourceSet.end();
		  ++itSource )
	{

			// receiver clock bias and instrumental bias
		try
		{
			updL1 = solverUpdL1L2.getFixedSolution( TypeID::updL1, *itSource);
			updL2 = solverUpdL1L2.getFixedSolution( TypeID::updL2, *itSource);

				// Change to cycle unit
			updL1 = updL1/L1_WAVELENGTH_GPS;
			updL2 = updL2/L2_WAVELENGTH_GPS;
			
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
      solutionRecord += rightJustify(asString(updL1, 3), 8); // instrument bias LC
      solutionRecord += string(1,' ');
      solutionRecord += rightJustify(asString(updL2, 3), 8); // instrument bias MW
      solutionRecord += string(1,' ');


			// Store the data record line into 'solutionMap'
		solutionMap.insert(make_pair(workEpoch, solutionRecord));

			// Store the clock data record into solutionRecord
	}  // End of ' for( SourceIDSet::iterator itSource ... '

		//*Now, read and print the receiver clock data record
   double updSatL1; 
   double updSatL2; 
      
      // Read satellite clocks from the stateMap
   currentSatSet = solverUpdL1L2.getCurrentSats();
      
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
         updSatL1 = solverUpdL1L2.getFixedSolution(TypeID::updSatL1, *itSat);
         updSatL2 = solverUpdL1L2.getFixedSolution(TypeID::updSatL2, *itSat);

				// Change to cycle unit
         updSatL1 = updSatL1/L1_WAVELENGTH_GPS;
         updSatL2 = updSatL2/L2_WAVELENGTH_GPS;
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
      solutionRecord += rightJustify(asString(updSatL1, 3), 8); // instrument bias LC
      solutionRecord += string(1,' ');
      solutionRecord += rightJustify(asString(updSatL2, 3), 8); // instrument bias MW
      solutionRecord += string(1,' ');

        // Store the data record line into 'solutionMap'
      solutionMap.insert(make_pair(workEpoch, solutionRecord));

   }


}  // End of 'void clkupdUC::getBiases( const CommonTime& workEpoch ... '






   // Method that will be executed AFTER the 'Process'
void clkupdUC::shutDown()
{
   // do nothing until now!
      // Solution output stream
   ofstream solutionFile;
   solutionFile.open( outputFileName.c_str(), ios::out );

      // Print the header information
   printBiasHeader( solutionFile );

      // print the clock products as the IGS format
   printBiasData( solutionFile );

      // close the solution file stream
   solutionFile.close();

		// Show a message indicating that we've finished calculating upd
	cout << "CLK and UPD solutions are in file: " << outputFileName << endl;

}


   // Method to print clock data 
void clkupdUC::printBiasHeader( ofstream& solutionFile)
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
 
}

   // Method to print clock data 
void clkupdUC::printBiasData( ofstream& solutionFile)
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
