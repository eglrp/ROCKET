#include <iostream>
#include "TropModel.hpp"
#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
    MJD tmjd(56141.0);
    CommonTime epoch( tmjd.convertToCommonTime() );

    double lat(48.20), lon(16.37), height(156.0);
    double elevation(10.0);

    MOPSTropModel mopsTM;
    mopsTM.setReceiverLatitude(lat);
    mopsTM.setReceiverHeight(height);
    mopsTM.setDayOfYear(epoch);

    NeillTropModel neillTM;
    neillTM.setReceiverLatitude(lat);
    neillTM.setReceiverHeight(height);
    neillTM.setDayOfYear(epoch);

    string file("/home/kfkuang/new/ROCKET/tables/gpt2_1wA.grd");

    ViennaTropModel viennaTM;
    viennaTM.loadFile(file.c_str());
    viennaTM.setReceiverLatitude(lat);
    viennaTM.setReceiverLongitude(lon);
    viennaTM.setReceiverHeight(height);
    viennaTM.setModifiedJulianDate(epoch);

    cout << fixed;
//    cout << setw(10) << lat*DEG_TO_RAD
//         << setw(10) << lon*DEG_TO_RAD
//         << endl;
    cout << setw(20) << "MOPS TropModel: "
         << setw(10) << mopsTM.correction(elevation)
         << setw(10) << mopsTM.dry_zenith_delay()
         << setw(10) << mopsTM.wet_zenith_delay()
         << setw(10) << mopsTM.dry_mapping_function(elevation)
         << setw(10) << mopsTM.wet_mapping_function(elevation)
         << endl;
    cout << setw(20) << "Neill TropModel: "
         << setw(10) << neillTM.correction(elevation)
         << setw(10) << neillTM.dry_zenith_delay()
         << setw(10) << neillTM.wet_zenith_delay()
         << setw(10) << neillTM.dry_mapping_function(elevation)
         << setw(10) << neillTM.wet_mapping_function(elevation)
         << endl;
    cout << setw(20) << "Vienna TropModel: "
         << setw(10) << viennaTM.correction(elevation)
         << setw(10) << viennaTM.dry_zenith_delay()
         << setw(10) << viennaTM.wet_zenith_delay()
         << setw(10) << viennaTM.dry_mapping_function(elevation)
         << setw(10) << viennaTM.wet_mapping_function(elevation)
         << endl;

    return 0;
}
