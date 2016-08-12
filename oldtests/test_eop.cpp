#include <iostream>

#include "ConfDataReader.hpp"

#include "EOPDataStore.hpp"
#include "EOPDataStore2.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{

   ConfDataReader confReader;
   try
   {
      confReader.open("../../ROCKET/oldtests/test.conf");
   }
   catch(...)
   {
      cerr << "Conf File Open Error." << endl;
      return 1;
   }

   EOPDataStore eopDataStore;
   EOPDataStore2 eopDataStore2;

   string eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");
   try
   {
      eopDataStore.loadIERSFile(eopFile);
      eopDataStore2.loadIERSFile(eopFile);
   }
   catch(...)
   {
      cerr << "IERS EOP File Load Error." << endl;
      return 1;
   }

   CivilTime CT(2015,6,30,12,0,0.0, TimeSystem::UTC);
   CommonTime UTC( CT.convertToCommonTime() );

   EOPDataStore::EOPData eopData = eopDataStore.getEOPData(UTC);
   cout << "EOPDataStore: " << endl;
   cout << eopData << endl;

   EOPDataStore2::EOPData eopData2 = eopDataStore2.getEOPData(UTC);

   double cor[4] = {0.0};
   eopDataStore2.PMUT1_OCEANS(UTC,cor);

   eopData2.xp += cor[0];
   eopData2.yp += cor[1];
   eopData2.UT1mUTC += cor[2];
   eopData2.LOD += cor[3];

   cout << "EOPDataStore2: " << endl;
   cout << eopData2 << endl;

   return 0;
}
