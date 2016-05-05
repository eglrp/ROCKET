#include <iostream>
#include "IERSConventions.hpp"
#include "CivilTime.hpp"

using namespace std;
using namespace gpstk;


int main(void)
{
    try
    {
        LoadIERSERPFile("../../rocket/tables/finals2000A.all");
    }
    catch(...)
    {
        cerr << "Load IERS ERP File error." << endl;

        return 0;
    }

    // UTC
    CommonTime UTC( CivilTime(2000,1,1,0,0,0.0, TimeSystem::UTC).convertToCommonTime() );

    for(int i=0; i<100; i++)
    {
        // TT
        CommonTime TT( UTC2TT(UTC) );

        // EOP from IERS
        EOPDataStore::EOPData eop0(EOPData(UTC));

        // Correction for Zonal Tides
        Vector<double> cor_zonalTide( RG_ZONT2(TT) );

        // EOP after zonal tide correction for UT1-UTC
        EOPDataStore::EOPData eop1(eop0);
        eop1.UT1mUTC -= cor_zonalTide(0);


        // Correction for Ocean Tides
        Vector<double> cor_oceanTides( PMUT1_OCEANS(UTC) );


        UTC.addDays(1);

        cout << UTC << endl;
    }

    return 0;
}
