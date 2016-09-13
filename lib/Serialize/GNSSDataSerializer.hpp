#ifndef GNSS_DATA_SERIALIZER_H_H
#define GNSS_DATA_SERIALIZER_H_H

// include GNSS Data Structures
#include "DataStructures.hpp"

// include rapidjson lib
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

namespace gpstk
{
	
	/** The class declares some static functions to serialize the GDS(GNSS Data Structure)
	 *  to json and deserialize the json to GDS, so you can exchange the GDS by network/file
	 *	/pipe/etc between different modules. Also, you can dump the GDS or save GDS for debug.
	 *	
	 *  In this version, the GDS can be saved as json file and converted to string.
	 *
	 *  @code serialize to file
	 *		gnssDataMap gdMap;
	 *		GDSSerializer::serializeToFile<gnssDataMap>(gdMap, "gdmap.json");
	 *	@endcode
	 *	
	 *	@code deserialize from file
	 *		gnssDataMap gdMap;
	 *		GDSSerializer::deserializeFromFile<gnssDataMap>(gdMap, "gdmap.json");
	 *	@endcode
	 *
	 *	See More, you can go to the dir oldtest/test_gds_serialize.cpp.
	 *
	 */
class GDSSerializer{

public:

	static string serializeToString(TypeID& data);
	static string serializeToString(SatID&  data);
	static string serializeToString(SourceID& data);
	static string serializeToString(CommonTime& time);
	static string serializeToString(Triple& trip);
	static string serializeToString(sourceEpochHeader& header);
	static string serializeToString(sourceEpochTypeHeader& header);
	static string serializeToString(sourceEpochSatHeader& header);
	static string serializeToString(sourceEpochRinexHeader& header);

	static bool deserializeFromString(TypeID& data, string dataString);
	static bool deserializeFromString(SatID& data, string dataString);
	static bool deserializeFromString(SourceID& data, string dataString);
	static bool deserializeFromString(CommonTime& time, string dataString);
	static bool deserializeFromString(Triple& trip, string dataString);
	static bool deserializeFromString(sourceEpochHeader& header, string dataString);
	static bool deserializeFromString(sourceEpochTypeHeader& header, string dataString);
	static bool deserializeFromString(sourceEpochSatHeader& header, string dataString);
	static bool deserializeFromString(sourceEpochRinexHeader& header, string dataString);

	/// Serialize to string for all Data Structures
	static string serializeToString(typeValueMap& dataMap );
	static string serializeToString(satValueMap& dataMap );
	static string serializeToString(satTypeValueMap& dataMap );
	static string serializeToString(gnssSatValue& dataMap );
	static string serializeToString(gnssTypeValue& dataMap );
	static string serializeToString(gnssSatTypeValue& dataMap );
	static string serializeToString(gnssRinex& dataMap );
	static string serializeToString(sourceDataMap& dataMap );
	static string serializeToString(gnssDataMap& dataMap );
	
	/// Deserialize from string for all Data Structures
	static bool deserializeFromString(typeValueMap& dataMap, string dataString );
	static bool deserializeFromString(satValueMap& dataMap, string dataString );
	static bool deserializeFromString(satTypeValueMap& dataMap, string dataString );
	static bool deserializeFromString(gnssSatValue& dataMap, string dataString );
	static bool deserializeFromString(gnssTypeValue& dataMap, string dataString );
	static bool deserializeFromString(gnssSatTypeValue& dataMap, string dataString );
	static bool deserializeFromString(gnssRinex& dataMap, string dataString );
	static bool deserializeFromString(sourceDataMap& dataMap, string dataString);
	static bool deserializeFromString(gnssDataMap& dataMap, string dataString );

	// template function for deserialize from file
	template<typename T>
	static bool deserializeFromFile(T& dataMap, string fileName);
	
	// template function for serialize to file
	template<typename T>
	static void serializeToFile(T& dataMap, string fileName);

};



template<typename T>
bool GDSSerializer::deserializeFromFile(T& dataMap, string fileName){
	ifstream reader(fileName.c_str(), ios::in);

	if( reader.is_open() ){
		
		string dataString;
		reader >> dataString;
		reader.close();
		return deserializeFromString(dataMap, dataString);
		
	}

	reader.close();
	return false;
}

template<typename T>
void GDSSerializer::serializeToFile(T& dataMap, string fileName){
	ofstream writer(fileName.c_str(), ios::out);

	if( writer.is_open() ){
		
		writer << serializeToString(dataMap);
		writer.close();
		return;
	}

	writer.close();
	return;
}


}

#endif
