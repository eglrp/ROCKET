#pragma ident "$Id$"

/**
 * @file rinex_clk_test.cpp
 * tests RinexClockStream, RinexClockHeader, RinexClockData,
 * Rinex3ClockStream, Rinex3ClockHeader, Rinex3ClockData.
 */

#include "RinexClockStream.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockData.hpp"
#include "Rinex3ClockStream.hpp"
#include "Rinex3ClockHeader.hpp"
#include "Rinex3ClockData.hpp"

using namespace std;

/// Returns 0 when successful.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "Gimme a rinex nav to chew on!  Exiting." << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << "." << endl;

      gpstk::RinexClockStream rcffs(argv[1]);
      gpstk::RinexClockHeader rch;
      gpstk::RinexClockData rce;

      rcffs.exceptions(fstream::failbit);

      rcffs >> rch;
      rch.dump(cout);

      int i = 0;
      while (rcffs >> rce)
      {
         i++;
      }

      cout << "Read " << i << " records.  Done."  << endl;
      exit(0);
   }
   catch(gpstk::Exception& e)
   {
      cout << e;
      exit(1);
   }
   catch (...)
   {
      cout << "unknown error.  Done." << endl;
      exit(1);
   }

   exit(0);
} // main()
