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
//  2017/01/08 ~ 2017/01/0
//  Create this program to handle RINEX3 file.
//  GPS first !!!
//  Lei Zhao, a Ph.D. candidate
//	 School of Geodesy and Geomatics, Wuhan University 
//  
//	 2017/01/18
//  Try to solve HMW/BLC qeuations to generate upd 
//	 GPS only now
//  Lei Zhao, a Ph.D. candidate
//	 School of Geodesy and Geomatics, Wuhan University 
//  
//  2017/02/22
//  postUpsMGEX2, copy of postUpdMGEX 
//  Just to focus on GAL, need to test GPS
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
#include "SolverGenWLMGEX.hpp"
#include "SolverZapWL.hpp"

   // Class to solve a general equation.
#include "SolverGenNL.hpp"
#include "SolverGenNLMGEX.hpp"
#include "SolverZapNL.hpp"

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

	// Class to serialize data struture
#include "GNSSDataSerializer.hpp"

	// Class to define linear equations
#include "CommonlyUsedEquations.hpp"


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
   virtual void shutDown();


      // Method to deal with the preprocessing of the gnss data
//   virtual void preprocessing();
   virtual void preprocessing2();


      // Method to deal with the preprocessing of the gnss data
   virtual void solve();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;




	// Added by Lei Zhao vvv
//	CommandOptionWithAnyArg rnxFileListOpt;
//	CommandOptionWithAnyArg sp3FileListOpt;
//	CommandOptionWithAnyArg clkFileListOpt;
//	CommandOptionWithAnyArg eopFileListOpt;
//	CommandOptionWithAnyArg dcbFileListOpt;
//	CommandOptionWithAnyArg mscFileOpt;
	CommandOptionWithAnyArg outputFileOpt;
	CommandOptionWithAnyArg inputFileListOpt;

	string rnxFileListName;
	string sp3FileListName;
	string clkFileListName;
	string eopFileListName;
	string dcbFileListName;
	string mscFileName;
	string outputFileName;
	string inputFileListName;

	string inputGDSMapFileName;
	string inputRefStaFileName;
	string inputMasterStaFileName; 

	string systemStr;
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
                   const SolverGenWLMGEX& solverUpdWL,
                   const SolverGenNLMGEX& solverUpdNL);


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
//	rnxFileListOpt( 'r',
//						 "rnxFileList",
//						 "file storing a list of rinex file names ",
//						 true),
//	mscFileOpt(     'm',
//						 "mscFile",
//						 "file storing monitor station coordinates ",
//						  true),
//	clkFileListOpt( 'k',
//						 "clkFileList",
//						 "file storing a list of CLK file names ",
//						 false ),
//	sp3FileListOpt( 's',
//						 "sp3FileList",
//						 "file storing a list of SP3 file names",
//						 true),
//	eopFileListOpt( 'e',
//						 "eopFileList",
//						 "file storing a list of IGS erp file name ",
//						 true ),
//	dcbFileListOpt( 'D',
//						  "dcbFileList",
//						  "file storing a list of the P1C1 DCB file.",
//						  false),
	outputFileOpt(  'O',
						 "outputFile",
						 "file storing the computed clock data",
						  true ),
	inputFileListOpt( 'i',
							"inputFile",
							"file storing the input files",
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
//	if(rnxFileListOpt.getCount())
//	{
//		rnxFileListName = rnxFileListOpt.getValue()[0];
//	}
//	if(mscFileOpt.getCount())
//	{
//		mscFileName = mscFileOpt.getValue()[0];
//	}
//	if(clkFileListOpt.getCount())
//	{
//		clkFileListName = clkFileListOpt.getValue()[0];
//	}
//	if(sp3FileListOpt.getCount())
//	{
//		sp3FileListName = sp3FileListOpt.getValue()[0];
//	}
//	if(eopFileListOpt.getCount())
//	{
//		eopFileListName = eopFileListOpt.getValue()[0];
//	}
	if(outputFileOpt.getCount())
	{
		outputFileName = outputFileOpt.getValue()[0];
	}
	if(inputFileListOpt.getCount())
	{
		inputFileListName = inputFileListOpt.getValue()[0];
	}
//	if(dcbFileListOpt.getCount())
//	{
//		dcbFileListName = dcbFileListOpt.getValue()[0];
//	}
	// ^^^ 

}  // End of method 'updMGEX::spinUp()'



   // Method that will really process information
void updMGEX::process()
{
       // Preprocessing the gnss data 
   preprocessing2();

       // Generate and solve equations
   solve();
}



   // Method that will really process information
void updMGEX::preprocessing2()
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

	// debug code vvv
	//std::cout << "inputGDSMapFileName: " << inputGDSMapFileName << std::endl;
	// debug code 

		// Read the input information using GDSSerializer class
	GDSSerializer::deserializeFromFile<gnssDataMap>(gdsMap, inputGDSMapFileName);
  
	sourceDataMap sdMap;
	GDSSerializer::deserializeFromFile<sourceDataMap>(sdMap, inputRefStaFileName);
	refStationSet = sdMap.getSourceIDSet();

	GDSSerializer::deserializeFromFile<SourceID>(master, inputMasterStaFileName);


      // The rest of the processing will be in method 'updMGEX::shutDown()'
   return;

}  // End of 'updMGEX::preprocessing2()'


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



   // Method to generate and solve the general equation system
void updMGEX::solve()
{

//	// Debug code vvv
//   ////> Now, define the widelane ambiguity fixing equations
//   
//      // SourceID-indexed variables
//
//      // MW Receiver bias variable
//   RecBiasRandomWalkModel updMWModel;
//   Variable updMW( TypeID::updMW, &updMWModel, true, false, 9.0e10 );
//
//      // SourceID and SatID-indexed variables
//     
//      // Ambiguity variable
//   PhaseAmbiguityModel ambWLModel;
//   Variable ambWL( TypeID::BWL, &ambWLModel, true, true );
//
//      // SatID-indexed only variables
//
//      // MW Receiver bias variable
//   SatBiasRandomWalkModel updSatMWModel;
//   updSatMWModel.setQprime(3.0e-7);
//   Variable updSatMW( TypeID::updSatMW, &updSatMWModel, false, true);
//
//      //>Equation types to be used
//
//      // This will be the independent term for phase equations
//   Variable MW( TypeID::MWubbena);
//
//   ////> Define all equations, which will be used in the solver
//
//      // Reference stations Melboune-Wubbena equation description
//   Equation equMWRef( MW );
//   equMWRef.addVariable(updMW, true); // coefficient (1.0)
//   equMWRef.addVariable(updSatMW, true, -1.0); // coefficient(-1.0)
//   equMWRef.addVariable(ambWL, true, -0.861918400322); // coefficient (-0.8619...)
//
//      // If the unit sigma is 1m, then the sigma of MW is about 0.2 m
//      // You should be notice that the MWubbena is smoothed by the given
//      // moving window size, usually 10min, and if the sampling interval is 30s,
//      // then the sigma is about 0.2/sqrt(10) = 0.0667 cm
//      //
//      // 2014/11/14, set weight to 1.0/0.25 = 4
//   equMWRef.setWeight(25.0);     // 5.0 * 5.0
//      // Set the source of the equation
//   equMWRef.header.equationSource = Variable::someSources;
//
//      // Add all reference stations. Remember that they form a set
//   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
//        itSet != refStationSet.end();
//        ++itSet )
//   {
//      equMWRef.addSource2Set( (*itSet) );
//   }
//
//      // Master station Melbourne-Wubbena equation description
//   Equation equMWMaster( MW );
//   equMWMaster.addVariable(updSatMW, true, -1.0); // coefficient(-1.0)
//   equMWMaster.addVariable(ambWL, true, -0.861918400); // coefficient (0.8619...)
//      // Master station phase equation has more weight
//   equMWMaster.setWeight(25.0);     // 15.0 * 15.0
//      // Set the source of the equation
//   equMWMaster.header.equationSource = master;
//
//   ////> Define the equation system
//
//      // Setup equation system
//   EquationSystem2 system1;
//   system1.addEquation(equMWRef);
//   system1.addEquation(equMWMaster);
//
//      // Forward-backward solver
//   SolverGenWL solverUpdWL(system1);
//
//   ////> Define the constraint systems.
//   
//      // Setup constraint equation system
//   IndepAmbiguityDatum ambConstrWL;
//   ambConstrWL.setAmbType(ambWL);
//
//      // Feed the 'solverGen' with 'constrSystem'
//   solverUpdWL.setIndepAmbiguityDatum(ambConstrWL);
//   solverUpdWL.setAmbType(ambWL);
//
//   ////
//   ////> Now, define the narrowlane ambiguity fixing equations
//   ////
//   
//      // LC Receiver bias variable
//   RecBiasRandomWalkModel updLCModel;
//   Variable updLC( TypeID::updLC, &updLCModel, true, false, 9.0e10 );
//
//      // SourceID and SatID-indexed variables
//     
//      // Ambiguity variable
//   PhaseAmbiguityModel ambNLModel;
//   Variable ambNL( TypeID::BL1, &ambNLModel, true, true );
//
//      // SatID-indexed only variables
//
//      // LC satellite bias variable
//   SatBiasRandomWalkModel updSatLCModel;
//   Variable updSatLC( TypeID::updSatLC, &updSatLCModel, false, true );
//
//      //>Equation types to be used
//
//      // This will be the independent term for phase equations
//      // Warning: 
//      // Now the LC value is assigned to the float ambiguities, 
//      // which are solved by fixing the satellite clock to 
//      // the IGS or CODE precise clock products.
//   Variable BLC( TypeID::BLC );
//
//   ////> Define all equations, which will be used in the solver
//
//      // Reference stations phase equation description
//   Equation equBLCRef( BLC );
//   equBLCRef.addVariable(updLC, true);  // coefficient = 1.0
//   equBLCRef.addVariable(updSatLC, true, -1.0); // coefficient(-1.0)
//   equBLCRef.addVariable(ambNL, true, -0.106953378); // coefficient = -0.10695...
//
//      // If the unit sigma is 1m, then the sigma of BLC is about 0.01 m
//      // if the smooth window size is 10, then the sigma is about 0.01/sqrt(10) = 0.003 cm
//   equBLCRef.setWeight(10000.0);     // 333.3 * 333.3
//      // Set the source of the equation
//   equBLCRef.header.equationSource = Variable::someSources;
//
//
//      // Add all reference stations. Remember that they form a set
//   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
//        itSet != refStationSet.end();
//        ++itSet )
//   {
//      equBLCRef.addSource2Set( (*itSet) );
//   }
//
//
//      // Master station phase equation description
//   Equation equBLCMaster( BLC );
//   equBLCMaster.addVariable(updSatLC, true, -1.0); 
//   equBLCMaster.addVariable(ambNL, true, -0.106953378);
//      // Master station phase equation has more weight
//   equBLCMaster.setWeight(10000.0);     // 100.0 * 100.0
//      // Set the source of the equation
//   equBLCMaster.header.equationSource = master;
//
//   ////> Define the equation system
//
//      // Setup equation system
//   EquationSystem2 system2;
//   system2.addEquation(equBLCRef);
//   system2.addEquation(equBLCMaster);
//
//      // Forward-backward solver
//   SolverGenNL solverUpdNL(system2);
//
//   ////> Define the constraint systems.
//   
//      // Setup constraint equation system
//   IndepAmbiguityDatum ambConstrNL;
//   ambConstrNL.setAmbType(ambNL);
//
//      // Feed the 'solverGen' with 'constrSystem'
//   solverUpdNL.setIndepAmbiguityDatum(ambConstrNL);
//
//
//
//	// Debug code ^^^ 

   ////> In the resolution part we start configuring the general solver 

		// Common Equations
	CommonlyUsedEquations equ;

		// Equation setting
	bool employGPS(true);
	bool employGalileo(false);
	
		// For HMW equations
	if( employGPS )
	{
		systemStr = "GPS";
		equ.equMWMaster.setWeight(25.0);
		equ.equMWMaster.header.equationSource = master;

		equ.equMWRef.setWeight(25);
		equ.equMWRef.header.equationSource = Variable::someSources;
	}

	if( employGalileo )
	{
		systemStr = "GAL";
		equ.equMWMasterGAL.setWeight(25.0);
		equ.equMWMasterGAL.header.equationSource = master;

		equ.equMWRefGAL.setWeight(25);
		equ.equMWRefGAL.header.equationSource = Variable::someSources;
	}

		// For BLC equations
	if( employGPS )
	{
		equ.equBLCMaster.setWeight(10000.0);
		equ.equBLCMaster.header.equationSource = master;

		equ.equBLCRef.setWeight(10000.0);
		equ.equBLCRef.header.equationSource = Variable::someSources;
	}

	if( employGalileo )
	{
		equ.equBLCMasterGAL.setWeight(10000.0);
		equ.equBLCMasterGAL.header.equationSource = master;

		equ.equBLCRefGAL.setWeight(10000.0);
		equ.equBLCRefGAL.header.equationSource = Variable::someSources;
	}



		// Add all reference stations. Remember that they form a set
	for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
		  itSet != refStationSet.end();
		  ++itSet )
	{
		if( employGPS )
		{
			equ.equMWRef.addSource2Set( (*itSet) );
			equ.equBLCRef.addSource2Set( (*itSet) );
		}

		if( employGalileo )
		{
			equ.equMWRefGAL.addSource2Set( (*itSet) );
			equ.equBLCRefGAL.addSource2Set( (*itSet) );
		}
	}

		// Setup equation system
			// system1 is for HMW equations
	EquationSystem2 system1;
			// system2 is for BLC equations
	EquationSystem2 system2;
			// ambiguity datum for BWL
	IndepAmbiguityDatum ambConstrWL;
	ambConstrWL.setAmbType(Variable::ambWL);
			// ambiguity datum for BLC
	IndepAmbiguityDatum ambConstrNL;
	ambConstrNL.setAmbType(Variable::ambNL);

	if( employGPS )
	{
		system1.addEquation(equ.equMWMaster);
		system1.addEquation(equ.equMWRef);

		system2.addEquation(equ.equBLCMaster);
		system2.addEquation(equ.equBLCRef);

	}

	if( employGalileo )
	{
		system1.addEquation(equ.equMWMasterGAL);
		system1.addEquation(equ.equMWRefGAL);

		system2.addEquation(equ.equBLCMasterGAL);
		system2.addEquation(equ.equBLCRefGAL);

	}

		// Equation Solver setting
	SolverGenWLMGEX solverUpdWL;
	SolverGenNLMGEX solverUpdNL;

	solverUpdWL.setEquationSystem( system1 );
	solverUpdWL.setIndepAmbiguityDatum(ambConstrWL);
	solverUpdWL.setAmbType(Variable::ambWL);

	solverUpdNL.setEquationSystem( system2 );
	solverUpdNL.setIndepAmbiguityDatum(ambConstrNL);


   ////> Variables for the print out 

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision" ) );
   cout << fixed << setprecision( precision );

		// Issue a message 
	cout << "Starting computing UPDs  ... " << endl;

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
			CivilTime ct( workEpoch );
			cout << ct.year << " " << ct.month << " " << ct.day << " "
					<< ct.hour << " " << ct.minute << " " << ct.second << endl;

         cout << "Reset of the filter because the satellite number of \
master station is zero." << endl;
            // Jump to the next epoch
         continue;
      }

//		// Debug code vvv
//
//		cout << "sats observed at master station: " << endl;
			// SatSet observed at master station
//		SatIDSet satSet( gMaster.body.getSatID() );

			// keep only this satset in gds
//		gds.keepOnlySatID( satSet );

//		for( SatIDSet::iterator itSat = satSet.begin();
//			  itSat != satSet.end();
//			  ++itSat )
//		{
//			cout << *itSat << endl;
//
//		}  // End of ' for( SatIDSet::iterator itSat = satSet.begin(); ... '
//		// Debug code ^^^ 

		

		
      cout << printTime(workEpoch,"%4Y %02m %02d %02H %02M %02S") << " "
			  << endl;
		
		// Test code vvv
			
			// To determine the valid stations
			// The most idael case is that all stations are used. 

				// Minuim num of sats, which is determined by the base station
		SatIDSet satSetByBase( gMaster.body.getSatID() );

				// Keep only stations observing the above sats
		gnssDataMap tempGds( gds.extractSatID( satSetByBase ) );

				// Generated stations in tempGds
		SourceIDSet initialValidSource( tempGds.getSourceIDSet() );

		SourceIDSet totalSourceSet( gds.getSourceIDSet() );
		SourceIDSet newSourceSet( initialValidSource );
		while(1)
		{
				// With these recognised sources, we can obtain more sats 	
			gnssDataMap newGds( gds.extractSourceID( newSourceSet ) );
			SatIDSet newSats( newGds.getSatIDSet() );

				// New Sources introduced by new sats
			gnssDataMap tempGds2( gds.extractSatID( newSats ) );
			newSourceSet = tempGds2.getSourceIDSet();
	
				// Still the initial sources???
			if( newSourceSet == initialValidSource ) { break; }

				// Already the total sources???
			if( newSourceSet == totalSourceSet ) { break; }

				// Update initialValidSource 
			initialValidSource = newSourceSet;
		}

			// So we have got the final valid stations
			// Keep only these sources in gds
		gds.keepOnlySourceID( newSourceSet );


      
clock_t start,finish;
double totaltime;
start=clock();

			// Debug code vvv
//      cout << "WL " 
//           << printTime(workEpoch,"%4Y %02m %02d %8s") << " "
//           << printTime(workEpoch,"%4Y %02m %02d %02H %02M %02S") << " "
//			  << endl;
			
			// Debug code ^^^ 

         // Compute the widelane biases by fixing widelane ambiguities
//		if( employGPS )
//		{
			solverUpdWL.Process(gds);
//		}

finish=clock();
totaltime=(double)(finish-start)/CLOCKS_PER_SEC;

//      cout << "WL " 
//           << printTime(workEpoch,"%4Y %02m %02d %8s") << " "
//			  << endl;
//           << totaltime << " " << endl;

start=finish;
  

         // Compute the widelane biases by fixing widelane ambiguities
//		if( employGPS )
//		{
	      solverUpdNL.Process(gds);
//		}


finish=clock();
totaltime=(double)(finish-start)/CLOCKS_PER_SEC;

//      cout << "NL " 
//           << printTime(workEpoch,"%4Y %02m %02d %8s") << " "
//           << totaltime << " " << endl;

         // Get clock-related solution 
//		if( employGPS )
//		{
			getBiases(workEpoch, solverUpdWL, solverUpdNL);
//		}


   }  // End of 'while( !gdsMap.empty() )'

   /// We are done ////

}  // End of 'updMGEX::solve()'


   // Method to extract the solutions from solverGen and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printBiasData'
void updMGEX::getBiases( const CommonTime& workEpoch,
                     const SolverGenWLMGEX& solverUpdWL,
                     const SolverGenNLMGEX& solverUpdNL)
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

         //updLC = updLC/0.106953378;
         //updMW = updMW/0.861918400;
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

			SatID sat( *itSat );
			if( sat.system == SatID::systemGPS )
			{
				updSatLC = updSatLC/0.106953378;
				updSatMW = updSatMW/0.861918400;
			}

			if( sat.system == SatID::systemGalileo )
			{
				updSatLC = updSatLC/LC_WAVELENGTH_GAL_L1L5;
				updSatMW = updSatMW/WL_WAVELENGTH_GAL_L1L5;
			}
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
void updMGEX::shutDown()
{
      // Solution file
   //string solutionName;
   //solutionName = confReader.getValue( "solutionFile", "DEFAULT" );


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
	cout << "UPD solution is in file: " << outputFileName << endl;


}  // End of 'upd::shutDown()'


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
   double version(3.0);
   headerRecord  = rightJustify(asString(version,2), 9);
   headerRecord += string(11,' ');
   headerRecord += string("CLOCK") + string(15,' ');
   //headerRecord += string("GPS") + string(17,' ');      // TD fix
   headerRecord += systemStr + string(17,' ');      // TD fix
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
