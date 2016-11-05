#include <iostream>
#include <iomanip>

#include "ConfDataReader.hpp"

#include "SP3EphemerisStore.hpp"

#include "ReferenceSystem.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    // Configuration File
    ConfDataReader confReader;

    try
    {
        confReader.open("../../ROCKET/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Configuration File Open Error." << endl;
        return 1;
    }

    // SP3 File
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(901);
    sp3Eph.setPosMaxInterval(8101);

    string sp3File;
    while( (sp3File=confReader.fetchListValue("IGSSP3List", "DEFAULT")) != "" )
    {
        try
        {
            sp3Eph.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "IGS SP3 File Load Error." << endl;
            return 1;
        }
    }


    // EOP Data
    EOPDataStore2 eopDataStore;

    string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
    try
    {
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "IERS EOP File Load Error." << endl;
        return 1;
    }

    // Leap Second Data
    LeapSecStore leapSecStore;

    string lsFile = confReader.getValue("IERSLSFile", "DEFAULT");
    try
    {
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS LeapSecond File Load Error." << endl;
        return 1;
    }

    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);

    // Initial Time
    int year, mon, day, hour, min;
    double sec;
    try
    {
        year = confReader.getValueAsInt("YEAR", "DEFAULT");
        mon  = confReader.getValueAsInt("MON", "DEFAULT");
        day  = confReader.getValueAsInt("DAY", "DEFAULT");
        hour = confReader.getValueAsInt("HOUR", "DEFAULT");
        min  = confReader.getValueAsInt("MIN", "DEFAULT");
        sec = confReader.getValueAsDouble("SEC", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Initial Time Error." << endl;
        return 1;
    }

    CivilTime ct0(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime gps0( ct0.convertToCommonTime() );
    CommonTime utc0( refSys.GPS2UTC(gps0) );

    // Sat ID
    int prn;
    try
    {
        prn = confReader.getValueAsInt("SATPRN", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Sat PRN Error." << endl;
        return 1;
    }
    SatID sat(prn,SatID::systemGPS);


    Matrix<double>  c2t( refSys.C2TMatrix(utc0) );
    Matrix<double> dc2t( refSys.dC2TMatrix(utc0) );

    Vector<double> sp3Pos(3,0.0), sp3Vel(3,0.0);
    Vector<double> eciPos(3,0.0), eciVel(3,0.0);

    int width;

    try
    {
        sp3Pos = sp3Eph.getXvt(sat, gps0).x.toVector();
        sp3Vel = sp3Eph.getXvt(sat, gps0).v.toVector();

        eciPos = transpose(c2t) * sp3Pos;
        eciVel = transpose(c2t) * sp3Vel + transpose(dc2t) * sp3Pos;

        cout << fixed << setprecision(6);
        cout << CivilTime(gps0);
        width = 18;
        cout << setw(width) << eciPos(0)
             << setw(width) << eciPos(1)
             << setw(width) << eciPos(2);
        width = 15;
        cout << setw(width) << eciVel(0)
             << setw(width) << eciVel(1)
             << setw(width) << eciVel(2)
             << endl;
    }
    catch(...)
    {
        cerr << "Get SP3 Position and Velocity Error." << endl;
        return 1;
    }

    return 0;
}
