#pragma ident "$Id$"

/**
 * @file rinex_nav_test.cpp
 * tests Rinex3NavStream, Rinex3NavHeader, Rinex3NavData.
 */

#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

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

      gpstk::Rinex3NavStream rnffs(argv[1]);
      gpstk::Rinex3NavHeader rnh;
      gpstk::Rinex3NavData rne;

      rnffs.exceptions(fstream::failbit);

      rnffs >> rnh;
      rnh.dump(cout);

      int i = 0;
      while (rnffs >> rne)
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
