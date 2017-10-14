#pragma ident "$Id$"

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"

#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

#include "Rinex3EphemerisStore2.hpp"

#include "SP3EphemerisStore.hpp"

#include "DataStructures.hpp"

#include "ProcessingList.hpp"

#include "ComputeSatPCenter.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


int main(int argc, char* argv[])
{

    // conf file
    string confFileName("conf.txt");

    ConfDataReader confReader;

    try
    {
        confReader.open(confFileName);
    }
    catch(...)
    {
        cerr << "conf file '" << confFileName
             << "' open error." << endl;
        exit(-1);
    }

    confReader.setFallback2Default(true);


    // initial time
    string time;
    try
    {
        time = confReader.getValue("initialTime", "DEFAULT");
    }
    catch(...)
    {
        cerr << "initial time get error." << endl;
        exit(-1);
    }

    int year = asInt( time.substr( 0,4) );
    int mon  = asInt( time.substr( 5,2) );
    int day  = asInt( time.substr( 8,2) );
    int hour = asInt( time.substr(11,2) );
    int min  = asInt( time.substr(14,2) );
    int sec  = asDouble( time.substr(17,2) );

    CivilTime civilTime(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime t0_gps( civilTime.convertToCommonTime() );


    // eop file
    string eopFileName;

    try
    {
        eopFileName = confReader.getValue("eopFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "eop file name get error." << endl;
        exit(-1);
    }

    EOPDataStore2 eopStore;

    try
    {
        eopStore.loadIERSFile(eopFileName);
    }
    catch(...)
    {
        cerr << "eop file '" << eopFileName
             << "' load error." << endl;
        exit(-1);
    }


    // leap second file
    string lsFileName;

    try
    {
        lsFileName = confReader.getValue("lsFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "leap second file name get error." << endl;
        exit(-1);
    }

    LeapSecStore lsStore;

    try
    {
        lsStore.loadFile(lsFileName);
    }
    catch(...)
    {
        cerr << "leap second file '" << lsFileName
             << "' load error." << endl;
        exit(-1);
    }


    // reference system
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopStore);
    refSys.setLeapSecStore(lsStore);


    // nav file
    string navFileListName;

    try
    {
        navFileListName = confReader.getValue("navFileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "nav file list name get error." << endl;
        exit(-1);
    }

    vector<string> navFileListVec;

    ifstream navFileListStream;

    navFileListStream.open(navFileListName.c_str());

    if(!navFileListStream)
    {
        cerr << "nav file list '" << navFileListName
             << "' open error." << endl;
        exit(-1);
    }

    string navFile;
    while(navFileListStream >> navFile)
    {
        navFileListVec.push_back(navFile);
    }

    navFileListStream.close();

    if(navFileListVec.size() == 0)
    {
        cerr << "nav file list is empty." << endl;
        exit(-1);
    }

    Rinex3EphemerisStore2 navStore;

    vector<string>::const_iterator nav_it = navFileListVec.begin();
    while(nav_it != navFileListVec.end())
    {
        string navFile = (*nav_it);

        try
        {
            navStore.loadFile(navFile);
        }
        catch(...)
        {
            cerr << "nav file '" << navFile
                 << "' load error." << endl;

            ++nav_it;

            continue;
        }

        ++nav_it;
    }


    // sp3 file
    string sp3FileListName;

    try
    {
        sp3FileListName = confReader.getValue("sp3FileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "sp3 file list name get error." << endl;
        exit(-1);
    }

    vector<string> sp3FileListVec;

    ifstream sp3FileListStream;

    sp3FileListStream.open(sp3FileListName.c_str());

    if(!sp3FileListStream)
    {
        cerr << "sp3 file list '" << sp3FileListName
             << "' open error." << endl;
        exit(-1);
    }

    string sp3File;
    while(sp3FileListStream >> sp3File)
    {
        sp3FileListVec.push_back(sp3File);
    }

    sp3FileListStream.close();

    if(sp3FileListVec.size() == 0)
    {
        cerr << "sp3 file list is empty." << endl;
        exit(-1);
    }

    // clk file
    string clkFileListName;

    try
    {
        clkFileListName = confReader.getValue("clkFileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "clk file list name get error." << endl;
        exit(-1);
    }

    vector<string> clkFileListVec;

    ifstream clkFileListStream;

    clkFileListStream.open(clkFileListName.c_str());

    if(!clkFileListStream)
    {
        cerr << "clk file list '" << clkFileListName
             << "' open error." << endl;
        exit(-1);
    }

    string clkFile;
    while(clkFileListStream >> clkFile)
    {
        clkFileListVec.push_back(clkFile);
    }

    clkFileListStream.close();

    if(clkFileListVec.size() == 0)
    {
        cerr << "clk file list is empty." << endl;
        exit(-1);
    }


    SP3EphemerisStore sp3Store;

    vector<string>::const_iterator sp3_it = sp3FileListVec.begin();
    while(sp3_it != sp3FileListVec.end())
    {
        string sp3File = (*sp3_it);

        try
        {
            sp3Store.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "sp3 file '" << sp3File
                 << "' load error." << endl;

            ++sp3_it;

            continue;
        }

        ++sp3_it;
    }

    vector<string>::const_iterator clk_it = clkFileListVec.begin();
    while(clk_it != clkFileListVec.end())
    {
        string clkFile = (*clk_it);

        try
        {
            sp3Store.loadRinexClockFile(clkFile);
        }
        catch(...)
        {
            cerr << "clk file '" << clkFile
                 << "' load error." << endl;

            ++clk_it;

            continue;
        }

        ++clk_it;
    }


    // nominal position
    Position nominalPos(0.0, 0.0, 0.0);


    // antex file
    string atxFileName;

    try
    {
        atxFileName = confReader.getValue("atxFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "antex file name get error." << endl;
        exit(-1);
    }

    AntexReader antexReader;

    try
    {
        antexReader.open( atxFileName );
    }
    catch(...)
    {
        cerr << "antex file '" << atxFileName
             << "' open error." << endl;
        exit(-1);
    }


    // satellite phase center
    ComputeSatPCenter satPCenter;
    satPCenter.setNominalPosition( nominalPos );
    satPCenter.setEphStore( navStore );
    satPCenter.setAntexReader( antexReader );


    // orbit
    CommonTime epoch;

    for(int i=1; i<=(MAX_PRN_GPS+MAX_PRN_GAL+MAX_PRN_BDS); i++)
    {
        SatID sat;

        if( i >= 1 && i <= MAX_PRN_GPS )
        {
            sat.id = i;
            sat.system = SatID::systemGPS;
        }
        else if( i > MAX_PRN_GPS &&
                 i <= (MAX_PRN_GPS+MAX_PRN_GAL) )
        {
            sat.id = i - MAX_PRN_GPS;
            sat.system = SatID::systemGalileo;
        }
        else if( i > (MAX_PRN_GPS+MAX_PRN_GAL) &&
                 i <= (MAX_PRN_GPS+MAX_PRN_GAL+MAX_PRN_BDS) )
        {
            sat.id = i - MAX_PRN_GPS - MAX_PRN_GAL;
            sat.system = SatID::systemBDS;
        }

        cerr << "Satellite " << sat << " Processing." << endl;

        string outFile;

        string dir( "./compare/" );
        string id( rightJustify(asString(sat.id), 2, '0') );
        string sys( SatID::convertSatelliteSystemToString(sat.system) );

        outFile = dir + sys + id + string(".txt");

        ofstream fout;
        fout.open(outFile.c_str());

        fout << fixed;

        for(int j=96*0; j<=96*1; j++)
        {
            epoch = t0_gps + j*900.0;

            Vector<double> r_sp3(3,0.0), v_sp3(3,0.0);
            Vector<double> r_nav(3,0.0), v_nav(3,0.0);

            Vector<double> dxyz1(3,0.0), dxyz2(3,0.0);
            Vector<double> drtn1(3,0.0), drtn2(3,0.0);

            double cb_sp3(0.0), cd_sp3(0.0);
            double cb_nav(0.0), cd_nav(0.0);
            double dclk(0.0);

            try
            {
                r_sp3 = sp3Store.getXvt(sat,epoch).x.toVector();
                v_sp3 = sp3Store.getXvt(sat,epoch).v.toVector();

                r_nav = navStore.getXvt(sat,epoch).x.toVector();
                v_nav = navStore.getXvt(sat,epoch).v.toVector();

                cb_sp3 = sp3Store.getXvt(sat,epoch).clkbias;
                cd_sp3 = sp3Store.getXvt(sat,epoch).clkdrift;

                cb_nav = navStore.getXvt(sat,epoch).clkbias;
                cd_nav = navStore.getXvt(sat,epoch).clkdrift;

                dclk = (cb_nav + cd_nav) - (cb_sp3 + cd_sp3);
            }
            catch(...)
            {
                fout << setprecision(0)
                     << setw( 5) << YDSTime(epoch).year
                     << setw( 5) << YDSTime(epoch).doy
                     << setw(10) << YDSTime(epoch).sod
                     << setprecision(3)
                     << setw(10) << drtn1(0)
                     << setw(10) << drtn1(1)
                     << setw(10) << drtn1(2)
                     << setw(10) << drtn2(0)
                     << setw(10) << drtn2(1)
                     << setw(10) << drtn2(2)
                     << setw(10) << dclk*C_MPS
                     << endl;
                continue;
            }

            typeValueMap tvmap;
            tvmap[TypeID::satX] = r_nav(0);
            tvmap[TypeID::satY] = r_nav(1);
            tvmap[TypeID::satZ] = r_nav(2);

            satTypeValueMap stvmap;
            stvmap[sat] = tvmap;

            Vector<double> dSatPC(3,0.0);

            satPCenter.Process(epoch, stvmap);

            dSatPC(0) = stvmap.getValue(sat,TypeID::satPCenterX);
            dSatPC(1) = stvmap.getValue(sat,TypeID::satPCenterY);
            dSatPC(2) = stvmap.getValue(sat,TypeID::satPCenterZ);

            dxyz1 = r_nav - r_sp3;
            dxyz2 = r_nav - dSatPC - r_sp3;

            Vector<double> dvxyz = v_nav - v_sp3;

            if(sat.system == SatID::systemGPS)
            {
                if( std::abs(dxyz1(0))>10.0 ||
                    std::abs(dxyz1(1))>10.0 ||
                    std::abs(dxyz1(2))>10.0 ||
                    std::abs(dclk*C_MPS)>100.0 )
                {
                    continue;
                }
                else
                {
                    drtn1 = ( refSys.XYZ2RTN(dxyz1, r_sp3, v_sp3) );
                }

                if( std::abs(dxyz2(0))>10.0 ||
                    std::abs(dxyz2(1))>10.0 ||
                    std::abs(dxyz2(2))>10.0 )
                {
                    continue;
                }
                else
                {
                    drtn2 = ( refSys.XYZ2RTN(dxyz2, r_sp3, v_sp3) );
                }
            }
            else if(sat.system == SatID::systemGalileo)
            {
                if( std::abs(dxyz1(0))>20.0 ||
                    std::abs(dxyz1(1))>20.0 ||
                    std::abs(dxyz1(2))>20.0 ||
                    std::abs(dclk*C_MPS)>100.0 )
                {
                    continue;
                }
                else
                {
                    drtn1 = ( refSys.XYZ2RTN(dxyz1, r_sp3, v_sp3) );
                }

                if( std::abs(dxyz2(0))>20.0 ||
                    std::abs(dxyz2(1))>20.0 ||
                    std::abs(dxyz2(2))>20.0 )
                {
                    continue;
                }
                else
                {
                    drtn2 = ( refSys.XYZ2RTN(dxyz2, r_sp3, v_sp3) );
                }
            }
            else if(sat.system == SatID::systemBDS)
            {
                if( std::abs(dxyz1(0))>20.0 ||
                    std::abs(dxyz1(1))>20.0 ||
                    std::abs(dxyz1(2))>20.0 ||
                    std::abs(dclk*C_MPS)>100.0 )
                {
                    continue;
                }
                else
                {
                    drtn1 = ( refSys.XYZ2RTN(dxyz1, r_sp3, v_sp3) );
                }

                if( std::abs(dxyz2(0))>20.0 ||
                    std::abs(dxyz2(1))>20.0 ||
                    std::abs(dxyz2(2))>20.0 )
                {
                    continue;
                }
                else
                {
                    drtn2 = ( refSys.XYZ2RTN(dxyz2, r_sp3, v_sp3) );
                }
            }

            fout << setprecision(0)
                 << setw( 5) << YDSTime(epoch).year
                 << setw( 5) << YDSTime(epoch).doy
                 << setw(10) << YDSTime(epoch).sod
                 << setprecision(3)
                 << setw(10) << drtn1(0)
                 << setw(10) << drtn1(1)
                 << setw(10) << drtn1(2)
                 << setw(10) << drtn2(0)
                 << setw(10) << drtn2(1)
                 << setw(10) << drtn2(2)
                 << setw(10) << dvxyz(0)
                 << setw(10) << dvxyz(1)
                 << setw(10) << dvxyz(2)
                 << setw(10) << dclk*C_MPS
                 << endl;
        }

        fout.close();

    }

    return 0;
}
