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
//  2015/07/02  
//
//  create this program by Shoujian Zhang.
//
//  2016/06/20
//
//  Modify the usage method of this program. The eph/clk/erp files are 
//  provided from the command line.
//
//	 2016/12/08
//  Change all the interface to user, just like upd.cpp 
//  Lei Zhao, SGG, WHU
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
#include "MWFilter.hpp"

   // Class to smooth the ambiguities
#include "AmbiguitySmoother.hpp"

   // Class to solve a general equation.
#include "SolverGenWL.hpp"

   // Class to solve a general equation.
#include "SolverGenNL.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-only mode.
#include "SolverPPP.hpp"

   // Class to solve a general equation.
#include "PrefitFilter.hpp"

	// Class to serialize data struture
#include "GNSSDataSerializer.hpp"

	// Class to hold handy equation
#include "CommonlyUsedEquations.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clkupd: public BasicFramework
{
public:

      // Constructor declaration
   clkupd(char* arg0);


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

	CommandOptionWithAnyArg outputFileListOpt;
	CommandOptionWithAnyArg inputFileListOpt; 

	string outputFileListName;
	string inputFileListName;

	string inputGDSMapFileName;
	string inputRefStaFileName;
	string inputMasterStaFileName;

	string outputClkFileName;
	string outputUpdFileName;

	string outputFixedClkFileName;
	string outputFixedUpdFileName;

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
                   const SolverGenWL& solverUpdWL,
                   const SolverGenNL& solverUpdNL);


      // Method to print clock header 
   void printHeader( ofstream& outFile);


      // Method to print clock data 
   void printBiasData( ofstream& updFile );
   void printClockData( ofstream& clkFile );

      // Method to print clock data 
   void printUpdFixedData( ofstream& updFile );
   void printClockFixedData( ofstream& clkFile );


}; // End of 'clkupd' class declaration



   // Let's implement constructor details
clkupd::clkupd(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data to determine the UPDs.\n\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('clkupd.conf' by default).",
             false ),
	outputFileListOpt(  'O',
						 "outputFile",
						 "file storing the computed clock data",
						  true ),
	inputFileListOpt( 'i',
							"inputFile",
							"file storing the input files",
							true )



{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'clkupd::clkupd'



   // Method that will be executed AFTER initialization but BEFORE processing
void clkupd::spinUp()
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
         confReader.open( "clkupd.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'clkupd.conf'"
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

	if(outputFileOpt.getCount())
	{
		outputFileName = outputFileOpt.getValue()[0];
	}
	if(inputFileListOpt.getCount())
	{
		inputFileListName = inputFileListOpt.getValue()[0];
	}

}  // End of method 'clkupd::spinUp()'



   // Method that will really process information
void clkupd::process()
{
       // Preprocessing the gnss data 
   preprocessing();

       // Generate and solve equations
   solve();
}


void clkupd::preprocessing()
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


		// Specify output file name
	vector<string> outputFileListVec;

		// Now read output files from 'outputFileListName'
	ifstream outputFileListStream;
	outputFileListStream.open(outputFileListName.c_str(), ios::in);	
	if( !inputFileListStream )
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

	outputClkFileName = outputFileListVec[0];
	outputUpdFileName = outputFileListVec[1];
	outputFixedClkFileName = outputFileListVec[2];
	outputFixedUpdFileName = outputFileListVec[3];

	//std::cout << "inputGDSMapFileName: " << inputGDSMapFileName << std::endl;
	// debug code 

	cout << "Reading json files ... " << endl;

		// Read the input information using GDSSerializer class
	GDSSerializer::deserializeFromFile<gnssDataMap>(gdsMap, inputGDSMapFileName);
  
	sourceDataMap sdMap;
	GDSSerializer::deserializeFromFile<sourceDataMap>(sdMap, inputRefStaFileName);
	refStationSet = sdMap.getSourceIDSet();

	GDSSerializer::deserializeFromFile<SourceID>(master, inputMasterStaFileName);


}  // End of 'void clkupd::preprocessing()'

   // Method to generate and solve the general equation system
void clkupd::solve()
{

   ////> In the resolution part we start configuring the general solver 
   

   /////////////////////////////////////////////
   //
   //  Define the widelane ambiguity fixing equations
   //
   /////////////////////////////////////////////
   
      // SourceID-indexed variables

      // MW Receiver bias variable
   RecBiasRandomWalkModel updMWModel;
   Variable updMW( TypeID::updMW, &updMWModel, true, false, 9.0e10 );

      // SourceID and SatID-indexed variables
     
      // Ambiguity variable
   PhaseAmbiguityModel ambWLModel;
   Variable ambWL( TypeID::BWL, &ambWLModel, true, true );

      // SatID-indexed only variables
//   double updSatMWProcessNoise( 
//         confReader.getValueAsInt( "updSatMWProcessNoise", "DEFAULT" ) );

      // MW Receiver bias variable
   SatBiasRandomWalkModel updSatMWModel;
//   updSatMWModel.setQprime(updSatMWProcessNoise); // 1.67e-4(cr/s)*600 s = 0.1(cr) 
	updSatMWModel.setQprime( 3.0e-7 );
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
      // 2014/11/14, set weight to 1.0/0.5 = 2
   equMWRef.setWeight(25.0);     // 5 * 5
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
   SolverGenWL solverUpdWL(system1);

   ////> Define the constraint systems.
   
      // Setup constraint equation system
   IndepAmbiguityDatum ambConstrWL;
   ambConstrWL.setAmbType(ambWL);

      // Feed the 'solverGen' with 'constrSystem'
   solverUpdWL.setIndepAmbiguityDatum(ambConstrWL);
   solverUpdWL.setAmbType(ambWL);

     //////////////////////////////////////////////////////////////
     //
     //  Define PC and LC equations for clock and upd estimation
     //
     //////////////////////////////////////////////////////////////

      // Declare variables for the equations
  
   WhiteNoiseModel recvClockModel;
   Variable cdt( TypeID::cdt, &recvClockModel, true, false);
  
   WhiteNoiseModel satClockModel;
   Variable satClock( TypeID::dtSat, &satClockModel, false, true );
  
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
   equPCRef.addVariable(cdt, true);
   equPCRef.addVariable(satClock, true, -1.0);
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
   equLCRef.addVariable(cdt, true); // default coefficient (1.0)
   equLCRef.addVariable(satClock, true, -1.0); // default coefficient(-1.0)
   equLCRef.addVariable(updLC, true);
   equLCRef.addVariable(updSatLC, true, -1.0);
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
   equPCMaster.addVariable(satClock, true, -1.0);
   equPCMaster.addVariable(tropo);
  
      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Master station phase equation description
   Equation equLCMaster( prefitL );
   equLCMaster.addVariable(satClock, true, -1.0);
   equLCMaster.addVariable(updSatLC, true, -1.0);
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
   solverUpdNL.setIndepAmbiguityDatum(ambConstrBL1);

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

         // Compute the widelane biases by fixing widelane ambiguities
      solverUpdWL.Process(gds);

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

}  // End of 'clkupd::solve()'


   // Method to extract the solutions from solverGen and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printBiasData'
void clkupd::getBiases( const CommonTime& workEpoch,
                        const SolverGenWL& solverUpdWL,
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

   double updMWFixed; 
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
         updLC = solverUpdNL.getSolution(TypeID::updLC, *itSource);
         cdt   = solverUpdNL.getSolution(TypeID::cdt, *itSource);

         updLC = updLC/0.106953378;
         updMW = updMW/0.861918400;

            // Now, Let's get fixed values 
            //
         updMWFixed = solverUpdWL.getFixedSolution(TypeID::updMW, *itSource);
         updLCFixed = solverUpdNL.getFixedSolution(TypeID::updLC, *itSource);

         updLCFixed = updLCFixed/0.106953378;
         updMWFixed = updMWFixed/0.861918400;

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
      updRecFixed += rightJustify(asString(updMWFixed, 3), 8); // instrument bias MW

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

   double updSatMWFixed; 
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

         updSatLC = updSatLC/0.106953378;
         updSatMW = updSatMW/0.861918400;

         updSatMWFixed = solverUpdWL.getFixedSolution(TypeID::updSatMW, *itSat);
         updSatLCFixed = solverUpdNL.getFixedSolution(TypeID::updSatLC, *itSat);
         dtSatFixed    = solverUpdNL.getFixedSolution(TypeID::dtSat, *itSat);

         updSatLCFixed = updSatLCFixed/0.106953378;
         updSatMWFixed = updSatMWFixed/0.861918400;

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
      updRecFixed += rightJustify(asString(updSatMWFixed, 3), 8); // instrument bias MW

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

// while(begPos != endPos)
// {
//     cout << begPos->second << endl;
//     ++ begPos;
// }

}  // End of method 'clkupd::getBiasesWL()'


   // Method that hold code to be run AFTER processing
void clkupd::shutDown()
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


}  // End of 'clkupd::shutDown()'


   // Method to print clock data 
void clkupd::printHeader( ofstream& outFile)
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
   string program ("clkupd");
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
   headerRecord += leftJustify("clkupd",17) + string(1, ' ');
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
void clkupd::printBiasData( ofstream& updFile)
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

}  // End of method 'clkupd::printBiasData()'


   // Method to print clock data 
void clkupd::printClockData( ofstream& clkFile)
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

}  // End of method 'clkupd::printClockData()'


   // Method to print clock data 
void clkupd::printUpdFixedData( ofstream& updFile)
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

}  // End of method 'clkupd::printBiasData()'


   // Method to print clock data 
void clkupd::printClockFixedData( ofstream& clkFile)
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

}  // End of method 'clkupd::printClockFixedData()'

   // Main function
int main(int argc, char* argv[])
{
   try
   {
      clkupd program(argv[0]);

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
