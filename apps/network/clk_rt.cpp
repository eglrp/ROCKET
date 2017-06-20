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
//  2016/11/13
//  Modify the processing-flow to Real-Time and add interface to deal with 
//  gnssDataMap
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

   // Class to filter out the PC 
   // outliers using prefit-residual
#include "PrefitFilter.hpp"

   // Class to read the dcb data from files
#include "DCBDataReader.hpp"

   // Class to read the MSC data from files
#include "MSCStore.hpp"

   // Class to correct the P1C1 biases for some receivers
#include "CC2NONCC.hpp"

#include "YDSTime.hpp"

   // Class to synchronize rinex observation data stream 
#include "NetworkObsStreams.hpp"

#include "Counter.hpp"

#include "GNSSDataSerializer.hpp"

   // Class to time update
#include "TimeUpdate.hpp"

   // Class to meas update
#include "MeasUpdate.hpp"

   // Class to state store
#include "StateStore.hpp"

   // Class to valid
#include "ValidOp.hpp"

#include "StochasticModel2.hpp"

// filter satellite system
#include "SystemFilter.hpp"

#include "CodeGrossFilter.hpp"

#include "ClkPRCorr.hpp"

#include "ClkEDCorr.hpp"

/// class add Broadcast ephemeris information
#include "Rinex3EphemerisStore.hpp"


#include "SatelliteFilter.hpp"

#include "AmbiChk.hpp"


#include "EclipsedSatFilter.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class clk_rt : public BasicFramework
{
public:

      // Constructor declaration
   clk_rt(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();


      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


      // Method that hold code to be run AFTER processing
   virtual void shutDown();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // Option for several input file list
   CommandOptionWithAnyArg rnxFileListOpt;
   CommandOptionWithAnyArg sp3FileListOpt;
   CommandOptionWithAnyArg navFileListOpt;
   CommandOptionWithAnyArg clkFileListOpt;
   CommandOptionWithAnyArg eopFileListOpt;
   CommandOptionWithAnyArg dcbFileListOpt;
   CommandOptionWithAnyArg mscFileOpt;
   CommandOptionWithAnyArg outputFileOpt;

   string rnxFileListName;
   string sp3FileListName;
   string navFileListName;
   string clkFileListName;
   string eopFileListName;
   string dcbFileListName;
   string mscFileName;
   string outputFileName;

      // Configuration file reader
   ConfDataReader confReader;

      // Mulitmap used to store the clock data line 
   std::multimap<CommonTime, std::string> solutionMap;

      // dcb program used in the header printing 
   string dcbProgram;

      // dcb source used in the header printing 
   string dcbSource;

      // antex file
   string antexFile;

      /// store the state
   StateStore m_StateStore;

   SourceIDSet  sourceSet;
   
   SatIDSet     satSet;

      // If you want to share objects and variables among methods, you'd
      // better declare them here

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 5 );

      // Method to extract solution
   void getClockSoluts(gnssDataMap& gData);

      // Method to print solution
   void printClockHeader( ofstream& clockStream );

      // Method to print solution
   void printClockData( ofstream& clockStream );


}; // End of 'clk' class declaration



   // Let's implement constructor details
clk_rt::clk_rt(char* arg0)
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
   navFileListOpt( 'n',
		   "navFileList",
   "file storing a list of rinex navigation file name",
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
void clk_rt::spinUp()
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
   if(navFileListOpt.getCount())
   {
      navFileListName = navFileListOpt.getValue()[0];
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
void clk_rt::process()
{
      
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
   ifstream sp3FileListStream( sp3FileListName.c_str(), ios::in );
   if( !sp3FileListStream.is_open() )
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


   ///////// read navigation epherimes file
  Rinex3EphemerisStore navEphStore;
  navEphStore.SearchNear();   

 
   ifstream navFileListStream( navFileListName.c_str(), ios::in );
   if( !navFileListStream.is_open() )
   {
         // If file doesn't exist, issue a warning
      cerr << "nav file List Name'" << navFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string navFile;
   while( navFileListStream >> navFile )
   {
      try
      {
	 std::cout<<"navFile:"<<navFile<<std::endl;
         navEphStore.loadFile( navFile);
      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "nav file '" << navFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;
         continue;
      }
   }
      // Close file
   navFileListStream.close();
   m_StateStore.setNavEphStore(navEphStore);

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
      if( !clkFileListStream.is_open() )
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
   if( !eopFileListStream.is_open() )
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
      if( !dcbFileListStream.is_open() )
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
      m_StateStore.setMSCStore( mscStore );
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


   //// antenna model
   AntexReader antexReader;
   bool useantex = confReader.getValueAsBoolean( "useAntex" );
   bool usepatterns = confReader.getValueAsBoolean( "usePCPatterns" );
   bool useazimuth  = confReader.getValueAsBoolean( "useAzim" );
   if( useantex ){ antexReader.open( confReader.getValue( "antexFile" ) ); }
      

   /// Define pre-processing flow

   // Define processing list
   ProcessingList procList;
   
   // Define linear combinations
   LinearCombinations comb;

   /// satellite number filter.
   SatelliteFilter satelliteFilter;
   //procList.push_back(satelliteFilter);
   
   AmbiChk ambiChk;
   ambiChk.setStateStore( m_StateStore );
	

   /// filter satellite system
   SystemFilter sysFilter;
   sysFilter.addFilterSatSys(SatID::systemGPS);
   procList.push_back(sysFilter);
   


   // correct bias for P1-C1
   CC2NONCC cc2noncc(dcbStore);
   cc2noncc.loadRecTypeFile( confReader.getValue("recTypeFile") );
   cc2noncc.setCopyC1ToP1(true);
   procList.push_back(cc2noncc);
   
   // check observables 
   RequireObservables requireObs;
   requireObs.addRequiredType(TypeID::P1);
   requireObs.addRequiredType(TypeID::P2);
   requireObs.addRequiredType(TypeID::L1);
   requireObs.addRequiredType(TypeID::L2);
   procList.push_back(requireObs);

   // check code observation within reasonable limits
   SimpleFilter pObsFilter;
   pObsFilter.addFilteredType(TypeID::P1);
   pObsFilter.setFilteredType(TypeID::P2);
   bool filterCode = confReader.getValueAsBoolean( "filterCode" );
   if( filterCode ){ procList.push_back(pObsFilter); }

   // form linear combination for cycle slip detection
   ComputeLinear linear1;
   linear1.addLinear(comb.pdeltaCombination);
   linear1.addLinear(comb.mwubbenaCombination);
   linear1.addLinear(comb.ldeltaCombination);
   linear1.addLinear(comb.liCombination);
   linear1.addLinear(comb.piCombination);
   procList.push_back(linear1);


   // check the pesedorange gross error and filter out abnormal
   // satellite Need TypeID::PI
   CodeGrossFilter psedoFilter;
   psedoFilter.setMaxLimit(30);
   procList.push_back(psedoFilter);
   
   /// build basic model.
   //BasicModel basicModel;
   //basicModel.setMinElev( confReader.getValueAsDouble( "cutOffElevation" ) );
   //basicModel.setDefaultObservable(TypeID::P1);
   //basicModel.setDefaultEphemeris(SP3EphList);
   //procList.push_back(basicModel);
  
   // define weight computation
   //ComputeElevWeights elevWeights;
   //procList.push_back(elevWeights);

   // form PC and LC observations
   //ComputeLinear linear31;
   //linear31.addLinear(comb.pcCombination);
   //linear31.addLinear(comb.lcCombination);
   //linear31.addLinear(comb.lcPrefitC);
   //procList.push_back(linear31);
 
   // screen PC
   //SimpleFilter pcFilter;
   //pcFilter.setFilteredType(TypeID::PC);
   //bool filterPC = confReader.getValueAsBoolean( "filterPC" );
   //if( filterPC ){procList.push_back(pcFilter);}
 
   // mark cycle slips
   LICSDetector markCSLI;
   procList.push_back(markCSLI);
   MWCSDetector2 markCSMW;
   procList.push_back(markCSMW);
	
   // carrier phase epoch difference.
   //ClkEDCorr clkEDCorr;
   //clkEDCorr.setStateStore(m_StateStore);   
   //clkEDCorr.setNavEphStore(navEphStore);
   //procList.push_back(clkEDCorr);
	
   // object to keep track of satellite arcs
   SatArcMarker2 markArc;
   markArc.setStateStore(m_StateStore);
   markArc.setDeleteUnstableSats(true);
   markArc.setUnstablePeriod(151.0);
   procList.push_back(markArc);
   
   
   // decimate data to sampling interval.
   //Decimate decimateData(
   //		confReader.getValueAsDouble( "decimationInterval" ),
   //		confReader.getValueAsDouble( "decimationTolerance" ),
   //		CommonTime::BEGINNING_OF_TIME );
   //m_StateStore.setDecimateInterval( confReader.getValueAsDouble("decimationInterval") );
   //procList.push_back(decimateData);

 
   /// declare TimeUpdate process.
   //TimeUpdate timeUpdate;
   //timeUpdate.setStateStore( m_StateStore );
   //procList.push_back(timeUpdate);   

   // define basic model 
   BasicModel basicModel;
   basicModel.setMinElev( confReader.getValueAsDouble( "cutOffElevation" ) );
   basicModel.setDefaultObservable(TypeID::P1);
   basicModel.setDefaultEphemeris(SP3EphList);
   procList.push_back(basicModel);

	
   //EclipsedSatFilter eclipseFilter;
   //procList.push_back(eclipseFilter);


   // define weight computation
   ComputeElevWeights elevWeights;
   procList.push_back(elevWeights);
	
   EclipsedSatFilter eclipseFilter;
   procList.push_back(eclipseFilter);


   // define gravitational delay
   GravitationalDelay graviDelay;
   procList.push_back(graviDelay);

   // define satellite phase center correction
   ComputeSatPCenter svPCenter;
   if( useantex ) {svPCenter.setAntexReader( antexReader );}
   procList.push_back(svPCenter);
   
   // correct observable for receiver PCV PCO
   CorrectObservables corr(SP3EphList);
   corr.setUseAntex( useantex );
   corr.setUsePatterns( usepatterns );
   if( useantex && usepatterns )
   {
      corr.setAntexReader( antexReader );
      corr.setUseAzimuth( useazimuth );
   }
   ComputeTides tideCorr(blqStore,eopStore);
   corr.setTideCorr(tideCorr);
   procList.push_back(corr);

   // correct windup
   ComputeWindUp windUp;
   windUp.setEphemeris(SP3EphList);
   if( useantex ){windUp.setAntexReader( antexReader);}
   procList.push_back(windUp);
   
   // correct Trop model
   NeillTropModel neillTM;
   ComputeTropModel computeTM(neillTM);
   procList.push_back(computeTM);
      
   // compute linear combination for PhaseCodeAlignment
   ComputeLinear linear2;
   linear2.addLinear(comb.q1Combination);
   linear2.addLinear(comb.q2Combination);
   procList.push_back(linear2);
   
   // Align L1
   PhaseCodeAlignment phaseAlignL1;
   phaseAlignL1.setCodeType(TypeID::Q1);
   phaseAlignL1.setPhaseType(TypeID::L1);
   phaseAlignL1.setPhaseWavelength( 0.190293672798365 );
   procList.push_back(phaseAlignL1);
   
   // Align L2
   PhaseCodeAlignment phaseAlignL2;
   phaseAlignL2.setCodeType(TypeID::Q2);
   phaseAlignL2.setPhaseType(TypeID::L2);
   phaseAlignL2.setPhaseWavelength( 0.244210213424568 );
   procList.push_back(phaseAlignL2);
   
   // form PC and LC observations
   ComputeLinear linear3;
   linear3.addLinear(comb.pcCombination);
   linear3.addLinear(comb.lcCombination);
   procList.push_back(linear3);
   
   // screen PC
   SimpleFilter pcFilter;
   pcFilter.setFilteredType(TypeID::PC);
   bool filterPC = confReader.getValueAsBoolean( "filterPC" );
   if( filterPC ){procList.push_back(pcFilter);}

   // form mw combination
   ComputeLinear linear4;
   linear4.addLinear(comb.mwubbenaCombination);
   procList.push_back(linear4);

   // form pcPrefit and lcPrefit
   ComputeLinear linear5;
   linear5.addLinear(comb.pcPrefitC);
   linear5.addLinear(comb.lcPrefitC);
   procList.push_back(linear5);


   // epoch-difference for carrier validation.
   //ClkEDCorr clkEDCorr;
   //clkEDCorr.setStateStore(m_StateStore);   
   //clkEDCorr.setNavEphStore(navEphStore);
   //procList.push_back(clkEDCorr);
   

   // PRSolution for code validation.
   //ClkPRCorr clkPRCorr;
   //clkPRCorr.setStateStore(m_StateStore);
   //clkPRCorr.setNavEphStore(navEphStore);
   //procList.push_back(clkPRCorr);

   //ClkEDCorr clkEDCorr;
   //clkEDCorr.setStateStore(m_StateStore);   
   //clkEDCorr.setNavEphStore(navEphStore);
   //procList.push_back(clkEDCorr);


   // transform from ECEF to NEU
   XYZ2NEU baseChange;
   procList.push_back(baseChange);
   
   // Compute DOP
   ComputeDOP cDOP;
   procList.push_back(cDOP);
 
   /// declare ValidOp
   ValidOp validOp;
   validOp.setStateStore( m_StateStore );
   validOp.setBLQStore( blqStore );
   procList.push_back(validOp);

   // decimate data to sampling interval.
   Decimate decimateData(
		confReader.getValueAsDouble( "decimationInterval" ),
		confReader.getValueAsDouble( "decimationTolerance" ),
		CommonTime::BEGINNING_OF_TIME );
   m_StateStore.setDecimateInterval( confReader.getValueAsDouble("decimationInterval") );
   procList.push_back(decimateData);

   /// satellite number filter.
   //SatelliteFilter satelliteFilter;
   procList.push_back(satelliteFilter);

   // Only keep the necessary types for 'solver' 
   TypeIDSet keepTypes;
   keepTypes.insert(TypeID::weight);
   keepTypes.insert(TypeID::wetMap);
   keepTypes.insert(TypeID::elevation);
   keepTypes.insert(TypeID::satArc);
   keepTypes.insert(TypeID::prefitC);
   keepTypes.insert(TypeID::prefitL);	


   ////  define Equation System
   std::string masterStationName = confReader.getValue( "masterStation" );
   m_StateStore.setMasterName( confReader.getValue( "masterStation" ) );

   // define variable of receiver clock bias
   WhiteNoiseModel2 recClkModel;
   recClkModel.addTypeID( TypeID::cdt );
   Variable recClkBias( TypeID::cdt, &recClkModel );
   recClkBias.setSourceIndexed(true);
   recClkBias.setSatIndexed(false);
   recClkBias.setDefaultCoefficient(1.0);
   recClkBias.setDefaultForced(true);
   
   // define variable of satellite clock bias
   //SatClkRandomWalkModel satClkModel;
   //WhiteNoiseModel2 satClkModel2;
   RandomWalkModel2 satClkModel2;
   satClkModel2.addTypeID( TypeID::dtSat );
   satClkModel2.setQprime(3e-8*C_MPS, TypeID::dtSat);
   Variable satClkBias( TypeID::dtSat, &satClkModel2 );
   satClkBias.setSourceIndexed(false);
   satClkBias.setSatIndexed(true);
   satClkBias.setDefaultCoefficient(-1.0);
   satClkBias.setDefaultForced(true);
   //std::cout<<"2222"<<std::endl;


   // define variable of satellite clock drift
   //Variable satClkDrift( TypeID::dtSatdot, &satClkModel);
   //satClkDrift.setSourceIndexed(false);
   //satClkDrift.setSatIndexed(true);
   //satClkDrift.setDefaultCoefficient(-1.0);
   //satClkDrift.setDefaultForced(true);


   // define variable of troposphere wet component
   TropoRandomWalkModel2 tropoModel;
   Variable tropoWet( TypeID::wetMap, &tropoModel );
   tropoWet.setSourceIndexed(true);
   tropoWet.setSatIndexed(false);
   tropoWet.setDefaultCoefficient(10.0);

   // define variable of ambiguity
   PhaseAmbiguityModel2 ambiModel;
   ambiModel.addTypeID( TypeID::BLC );
   Variable ambi( TypeID::BLC, &ambiModel );
   ambi.setSourceIndexed(true);
   ambi.setSatIndexed(true);
   ambi.setDefaultCoefficient(1.0);
   ambi.setDefaultForced(true);

   // define independent variable 
   Variable prefitPC( TypeID::prefitC );  // PC Observation
   Variable prefitLC( TypeID::prefitL );  // LC Observation

   // define reference Equation of PC
   Equation equPCRef( prefitPC );
   equPCRef.addVariable( recClkBias, true );
   equPCRef.addVariable( tropoWet );
   equPCRef.addVariable( satClkBias, true, -1.0 );
   //equPCRef.addVariable( satClkDrift, true, 0.0 );

   // define master Equation of PC
   Equation equPCMas( prefitPC );
   equPCMas.addVariable( tropoWet );
   equPCMas.addVariable( satClkBias, true, -1.0 );
   //equPCMas.addVariable( satClkDrift, true, 0.0 );

   // define reference Equation of LC
   Equation equLCRef( prefitLC );
   equLCRef.addVariable( recClkBias, true );
   equLCRef.addVariable( tropoWet );
   equLCRef.addVariable( ambi, true );
   equLCRef.addVariable( satClkBias, true, -1.0 );
   //equLCRef.addVariable( satClkDrift, true, 0.0 );
   equLCRef.setWeight(10000.0);

   // define master Equation of LC
   Equation equLCMas( prefitLC );
   equLCMas.addVariable( tropoWet );
   equLCMas.addVariable( ambi, true );
   equLCMas.addVariable( satClkBias, true, -1.0 );
   //equLCMas.addVariable( satClkDrift, true, 0.0 );
   equLCMas.setWeight(10000.0);
   
   /// declare TimeUpdate process.
   TimeUpdate timeUpdate;
   timeUpdate.setStateStore( m_StateStore );
  
   /// declare MeasUpdate process.
   MeasUpdate measUpdate;
   measUpdate.setStateStore( m_StateStore );

   /// Open all rinex files to set up NetworkObsStream
   NetworkObsStreams obsReader;
   obsReader.setSampleInterval( confReader.getValueAsDouble("preprocessingInterval") );


      // Now read rinex files from 'eopFileList'
   ifstream rnxFileListStream( rnxFileListName.c_str() );
   if( !rnxFileListStream.is_open() )
   {
         // If file doesn't exist, issue a warning
      cerr << "rinex file List Name'" << rnxFileListName << "' doesn't exist or you don't "
          << "have permission to read it. Skipping it." << endl;
      exit(-1);
   }
   
   std::string rnxFile;
   while( rnxFileListStream >> rnxFile )
   {
      if( !obsReader.addRinexObsFile( rnxFile ) )
      {
         cerr << "add rinex file :" << rnxFile << " failed !!"<< endl;
      }
   }
	
   bool firstTime = true;

	
   //ofstream posFile("blh.txt");

   /// process epoch by epoch
   gnssDataMap gData;
   while( obsReader.readEpochData( gData ) )
   {
      // clock begin
      double clock_start = Counter::now();

      CommonTime procEpoch( gData.begin()->first );
      
      std::cout<<std::endl;
      std::cout<<"time:"<<procEpoch<<std::endl;
      try
      {

	 // set initialize time for decimate.
	 if( firstTime ) {  
	    decimateData.setInitialEpoch(procEpoch);
	    firstTime = false; 
         }
	
	 // set current epoch.
 	 m_StateStore.setStateEpoch( procEpoch );
	
         // update receiver position from MSC 
         // data file
         m_StateStore.updateNominalPos( gData );
         

	 /*for( gnssDataMap::iterator iter = gData.begin();
		iter != gData.end(); iter++ )
	 {
	    for( sourceDataMap::iterator sdmIter = (*iter).second.begin();
		  sdmIter != (*iter).second.end(); sdmIter++ )
	    {
		std::string name = (*sdmIter).first.sourceName;
		Position pos = (*sdmIter).first.nominalPos;
		double lat = pos.getGeodeticLatitude();
		double lng = pos.getLongitude();
		double h = pos.getAltitude();
		posFile<<name<<","<<lat<<","<<lng<<std::endl;//<<","h<<std::endl;		
	    }
	 }*/


	//posFile.close();
	//return;
	
         // valid gnssDataMap.
         validOp.Process( gData ) ;
        
	
	 //gData >> satelliteFilter;
 
	 // pre-processing.
	 gData >> procList;


         // add equations to time update
         // clear all equation before
         timeUpdate.clearEquations();
         SourceIDSet allSource( gData.getSourceIDSet() );
         for( SourceIDSet::iterator iter = allSource.begin();
              iter != allSource.end(); iter++ )
         {
            if( (*iter).sourceName != masterStationName )
            {
               timeUpdate.addEquation2Source( equPCRef, *iter );
               timeUpdate.addEquation2Source( equLCRef, *iter );
            }
            else
            {
               timeUpdate.addEquation2Source( equPCMas, *iter );
               timeUpdate.addEquation2Source( equLCMas, *iter );
            }
         }

	 // time update
	 timeUpdate.Process( gData );

         // Delete the types not in 'keepTypes' to save the memory.
        // gData.keepOnlyTypeID( keepTypes );

	/*for( gnssDataMap::iterator gdmIter = gData.begin();
	      gdmIter != gData.end(); gdmIter++ )
	{
	    for( sourceDataMap::iterator sdmIter = gdmIter->second.begin();
		  sdmIter != gdmIter->second.end(); sdmIter++ )
	     {
		std::string sourceName = sdmIter->first.sourceName;
 		std::string modelName  = "/home/gaokang/gitcode/ROCKET_3/workplace/clk/model/"+sourceName + ".model2";
		ofstream modelFile( modelName.c_str(), ios::app );
		gnssRinex gRin;
		gRin.header.epoch = gdmIter->first;
		gRin.body = sdmIter->second;
		printModel( modelFile, gRin );
		modelFile.close();
	     }
	}*/	

	gData.keepOnlyTypeID( keepTypes );
         
	// add equations to meas update
        // clear all equation before
        measUpdate.clearEquations();
        SourceIDSet allSource2( gData.getSourceIDSet() );
        for( SourceIDSet::iterator iter = allSource2.begin();
              iter != allSource2.end(); iter++ )
        {
            if( (*iter).sourceName != masterStationName )
            {
               measUpdate.addEquation2Source( equPCRef, *iter );
               measUpdate.addEquation2Source( equLCRef, *iter );
            }
            else
            {
               measUpdate.addEquation2Source( equPCMas, *iter );
               measUpdate.addEquation2Source( equLCMas, *iter );
            }
        }

        // meas update process.
        measUpdate.Process( gData );
    
	
	/// output satellite stations.
	satelliteFilter.Process( gData );       
	
	

	for( gnssDataMap::iterator iter = gData.begin();
		iter != gData.end(); iter++ )

	{
	   for( sourceDataMap::iterator sdmIter = (*iter).second.begin();
		 sdmIter != (*iter).second.end(); sdmIter++ )
	   {
		for(satTypeValueMap::iterator stvmIter = (*sdmIter).second.begin();
		      stvmIter != (*sdmIter).second.end(); stvmIter++ )
		{
		   std::cout<<(*sdmIter).first.sourceName<<","<<(*stvmIter).first<<","
			<<m_StateStore.getSolution(TypeID::BLC, (*sdmIter).first, (*stvmIter).first)<<std::endl;

		}
	   }
	}	



	for( gnssDataMap::iterator gdmIter = gData.begin();
	      gdmIter != gData.end(); gdmIter++ )
	{
	    for( sourceDataMap::iterator sdmIter = gdmIter->second.begin();
		  sdmIter != gdmIter->second.end(); sdmIter++ )
	     {
		
		for( satTypeValueMap::iterator stvmIter = (*sdmIter).second.begin();
		      stvmIter != (*sdmIter).second.end(); stvmIter++ )
		{
		   std::string sourceName = sdmIter->first.sourceName;
		   int id = (*stvmIter).first.id;
		   stringstream ss;
		   ss << "/home/gaokang/gitcode/ROCKET_3/workplace/clk/blc/"<<sourceName<<"_"<<id<<".blc";
 		   std::string modelName  = ss.str();//"/home/gaokang/gitcode/ROCKET_3/workplace/clk/blc/"+sourceName+"_"+id+ ".model2";
		   ofstream modelFile( modelName.c_str(), ios::app );
					   
		   double blc = m_StateStore.getSolution(TypeID::BLC, (*sdmIter).first, (*stvmIter).first );
		   
		   modelFile << procEpoch << std::endl;
		   modelFile << blc << std::endl;

		   modelFile.close();
		}
	     }
	}	


//	gData >> ambiChk;	

        getClockSoluts(gData);		
      }
      catch(SVNumException& exceSVN)
      {
         cerr <<"Epoch:"<< procEpoch << ", SVNumException" << endl;
         continue;
      }
      catch(DecimateEpoch& decimateException )
      {
	 continue;
      }
      catch(NotFoundMaster& masterNotFound )
      {
         cerr <<"Epoch:"<< procEpoch << ",Exception : " << masterNotFound << endl;
         continue;
      }
      catch(MasterLessSatNum& masterLessSatNum )
      {
         cerr << "Epoch:"<< procEpoch << ",Exception : " << masterLessSatNum << endl;
      }
      catch(...)
      {
         cerr << "Epoch:"<<procEpoch<<",Unknown exception!"<< endl;
         continue;
      }
      
      double clock_end = Counter::now();
      
      cout << "epoch:"<< procEpoch << ",time cost:"<<clock_end-clock_start<<endl;
      std::cout<<std::endl;

   }  // End of 'while( obsReader.readEpochData(gData) )'

      // Clear content of SP3 ephemerides object
   SP3EphList.clear();

   return;

}  // End of 'clk::process()'



   // Method to print model values
void clk_rt::printModel( ofstream& modelfile,
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


   // Method to extract the solutions from solver and 
   // temprorarily store them in the string, which will be 
   // print out to the solutionfile in the method 'printClockData'
void clk_rt::getClockSoluts(gnssDataMap& gData)
{

   CommonTime workEpoch = m_StateStore.getStateEpoch();

      // Mulitmap used to store the clock data line 
   std::string solutionRecord;

      // Satellite sets for all the estimated satellite clocks
   SatIDSet currentSatSet;

      // Source sets for all the estimated receiver clocks
   SourceIDSet currentSourceSet;

      // Clock bias
   double clockBias;

      // sat clk drift.
   double satClkDrift;

      //*Now, read and print the receiver clock data record
      
      // Read receiver clocks from the stateMap
   m_StateStore.getSourceSet(currentSourceSet);
   
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
	 double initClkBias = (*itSource).recClkBias;
         clockBias = m_StateStore.getSolution(TypeID::cdt, *itSource)/C_MPS;
	 std::cout<<(*itSource).sourceName<<",initClkBias:"<<initClkBias/C_MPS<<",clockBias:"<<clockBias<<std::endl;
	 clockBias += initClkBias/C_MPS;
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
   m_StateStore.getSatIDSet(currentSatSet);
      
      // Insert current satellites into satSet
   satSet.insert(currentSatSet.begin(), currentSatSet.end());
   
   double decimateInterval = m_StateStore.getDecimateInterval();

      // Read the receiver clocks for the current sources
   for(SatIDSet::iterator itSat = currentSatSet.begin();
       itSat != currentSatSet.end();
       ++itSat)
   {
         // Satellite clock bias
      try
      {
         clockBias = m_StateStore.getSolution(TypeID::dtSat, *itSat)/C_MPS;
         //satClkDrift = m_StateStore.getSolution(TypeID::dtSatdot, *itSat)/( C_MPS * decimateInterval );
         //m_StateStore.setSolution( TypeID::dtSatdot, *itSat, satClkDrift/decimateInterval );
      }
      catch(InvalidRequest& e)
      {
         cerr << e.what() << endl;
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
      //solutionRecord += string(2,' ');
      //solutionRecord += rightJustify(doub2sci(satClkDrift, 19, 2), 19); // sat clock drift

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
void clk_rt::shutDown()
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
void clk_rt::printClockHeader( ofstream& clockStream)
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
   SourceID masterSource = m_StateStore.getMasterSource();
   headerRecord  = leftJustify((masterSource.sourceName),4) + string(1,' ');  // TD
   headerRecord += leftJustify((masterSource.sourceNumber),20) + string(15,' ');
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
void clk_rt::printClockData( ofstream& clockStream)
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
      clk_rt program(argv[0]);

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
