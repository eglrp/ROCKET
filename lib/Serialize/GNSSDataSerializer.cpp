#include "GNSSDataSerializer.hpp"
#include "StringUtils.hpp"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace gpstk{

using namespace StringUtils;


string GDSSerializer::serializeToString(TypeID& data){
	
	/// declare document
	Document doc(kObjectType);
	
	/// typeID Name
	Value typeName(kStringType);
	typeName.SetString( asString(data).c_str(), doc.GetAllocator() );
	doc.AddMember( "valueType", typeName, doc.GetAllocator() );

	/// convert to string as key
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	return string( buffer.GetString() );
}


string GDSSerializer::serializeToString(SatID& data){

	/// declare document
	Document doc(kObjectType);

	/// id
	doc.AddMember( "id", data.id, doc.GetAllocator() );

	/// system
	doc.AddMember( "system", data.system, doc.GetAllocator() );
	

	/// convert to string as key
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	return string( buffer.GetString() );

}


string GDSSerializer::serializeToString(SourceID& data){
	
	// declare document
	Document doc(kObjectType);
	
	/// source type
	doc.AddMember( "sourceType", data.type, doc.GetAllocator() );
	
	// source name
	Value sourceName(kStringType);
	sourceName.SetString(data.sourceName.c_str(), doc.GetAllocator() );
	doc.AddMember( "sourceName", sourceName, doc.GetAllocator() );
		
	// convert to string 
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	return string( buffer.GetString() );
}


string GDSSerializer::serializeToString(CommonTime& time){
	
	// declare document
	Document doc(kObjectType);
	
	long day, sod;
	double fsod;

	time.get(day, sod, fsod);
	
	// add day sod and fsod
	doc.AddMember( "day" , day , doc.GetAllocator() );
	doc.AddMember( "sod" , sod , doc.GetAllocator() );
	doc.AddMember( "fsod", fsod, doc.GetAllocator() );
	doc.AddMember( "timeSystem", time.getTimeSystem().getTimeSystem(), doc.GetAllocator() );
	
	// convert to string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}


string GDSSerializer::serializeToString(Triple& trip){
	
	// declare document
	Document doc(kObjectType);

	doc.AddMember( "a", trip[0], doc.GetAllocator() );
	doc.AddMember( "b", trip[1], doc.GetAllocator() );
	doc.AddMember( "c", trip[2], doc.GetAllocator() );

	// convert to string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(sourceEpochHeader& header){

	Document doc(kObjectType);
	
	// add source
	Value sourceValue(kStringType);
	sourceValue.SetString( serializeToString(header.source).c_str(), doc.GetAllocator() );
	doc.AddMember( "source", sourceValue, doc.GetAllocator() );
	
	// add epoch
	Value epochValue(kStringType);
	epochValue.SetString( serializeToString(header.epoch).c_str(), doc.GetAllocator() );
	doc.AddMember( "epoch", epochValue, doc.GetAllocator() );

	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}


string GDSSerializer::serializeToString(sourceEpochTypeHeader& header){
	
	Document doc(kObjectType);
	
	// add source
	Value sourceValue(kStringType);
	sourceValue.SetString( serializeToString(header.source).c_str(), doc.GetAllocator() );
	doc.AddMember( "source", sourceValue, doc.GetAllocator() );

	// add epoch
	Value epochValue(kStringType);
	epochValue.SetString( serializeToString(header.epoch).c_str(), doc.GetAllocator() );
	doc.AddMember( "epoch", epochValue, doc.GetAllocator() ); 
	
	// add typeid
	Value typeValue(kStringType);
	typeValue.SetString( serializeToString(header.type).c_str(), doc.GetAllocator() );
	doc.AddMember( "type", typeValue, doc.GetAllocator() );


	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(sourceEpochSatHeader& header){
	
	Document doc(kObjectType);
	
	// add source
	Value sourceValue(kStringType);
	sourceValue.SetString( serializeToString(header.source).c_str(), doc.GetAllocator() );
	doc.AddMember( "source", sourceValue, doc.GetAllocator() );

	// add epoch
	Value epochValue(kStringType);
	epochValue.SetString( serializeToString(header.epoch).c_str(), doc.GetAllocator() );
	doc.AddMember( "epoch", epochValue, doc.GetAllocator() ); 
	
	// add satID
	Value satIDValue(kStringType);
	satIDValue.SetString( serializeToString(header.satellite).c_str(), doc.GetAllocator() );
	doc.AddMember( "sat", satIDValue, doc.GetAllocator() );


	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(sourceEpochRinexHeader& header){
	
	Document doc(kObjectType);
	
	// add source
	Value sourceValue(kStringType);
	sourceValue.SetString( serializeToString(header.source).c_str(), doc.GetAllocator() );
	doc.AddMember( "source", sourceValue, doc.GetAllocator() );

	// add epoch
	Value epochValue(kStringType);
	epochValue.SetString( serializeToString(header.epoch).c_str(), doc.GetAllocator() );
	doc.AddMember( "epoch", epochValue, doc.GetAllocator() ); 
	
	// add antType
	Value antTypeValue(kStringType);
	antTypeValue.SetString( header.antennaType.c_str(), doc.GetAllocator() );
	doc.AddMember( "antType", antTypeValue, doc.GetAllocator() );
	
	// add antPos
	Value antPosValue(kStringType);
	antPosValue.SetString( serializeToString( header.antennaPosition ).c_str(), doc.GetAllocator() );
	doc.AddMember( "antPos", antPosValue, doc.GetAllocator() );

	// add flag
	Value flagValue(kNumberType);
	flagValue.SetInt( (int)header.epochFlag);
	doc.AddMember( "flag", flagValue, doc.GetAllocator() );

	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

bool GDSSerializer::deserializeFromString(TypeID& data, string dataString){
	
	/// declare document
	Document doc;
	
	/// parse json string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );
	
	/// parse error return
	if( doc.HasParseError() ) return false;
	
	/// get value type
	Value& valueTypeVal = doc["valueType"];

	if( valueTypeVal.IsString() ){
		
		data = TypeID( valueTypeVal.GetString() );

	}else{

		return false;
	}
	
	return true;
}

bool GDSSerializer::deserializeFromString(SatID& data, string dataString){
	
	/// declare document
	Document doc;

	/// parse json string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	/// parse error return
	if( doc.HasParseError() ) return false;

	Value& idValue  = doc["id"];
	Value& sysValue = doc["system"];

	if( idValue.IsInt() && sysValue.IsInt() ){
		
		data = SatID( idValue.GetInt(), (SatID::SatelliteSystem)(sysValue.GetInt()) );

		return data.isValid();

	}else{

		return false;
	}

}


bool GDSSerializer::deserializeFromString(SourceID& data, string dataString){
	
	// declare document
	Document doc;
	
	// parse json string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str() ) );

	// parse error return
	if( doc.HasParseError() ) return false;

	Value& sourceTypeValue = doc["sourceType"];
	Value& sourceNameValue = doc["sourceName"];

	if( sourceTypeValue.IsInt() && 
		sourceNameValue.IsString() ){
		
		data = SourceID( (SourceID::SourceType)sourceTypeValue.GetInt(),
					     string( sourceNameValue.GetString() ) );

		return data.isValid();

	}else{

		return false;
	}

}


bool GDSSerializer::deserializeFromString(CommonTime& time, string dataString){
	
	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& dayValue  = doc["day"];
	Value& sodValue  = doc["sod"];
	Value& fsodValue = doc["fsod"];
	Value& tSysValue = doc["timeSystem"];

	if( dayValue.IsInt64() &&
		sodValue.IsInt64() &&
		fsodValue.IsDouble() && 
		tSysValue.IsInt() ){

		time.set( dayValue.GetInt64(), sodValue.GetInt64(), 
				  fsodValue.GetDouble(), TimeSystem( (TimeSystem::Systems)(tSysValue.GetInt()) ) );

		return true;
	}else{

		return false;
	}
	
}


bool GDSSerializer::deserializeFromString(Triple& trip, string dataString){
	
	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef( dataString.c_str() ) );

	if( doc.HasParseError() ) return false;

	Value& aValue = doc["a"];
	Value& bValue = doc["b"];
	Value& cValue = doc["c"];

	if(aValue.IsDouble() &&
	   bValue.IsDouble() &&
	   cValue.IsDouble() ){
		
		trip = Triple(aValue.GetDouble(), bValue.GetDouble(), 
					cValue.GetDouble() );

		return true;
	}else{
		return false;
	}

}

bool GDSSerializer::deserializeFromString(sourceEpochHeader& header, string dataString){

	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& sourceValue = doc["source"];
	Value& epochValue  = doc["epoch"];

	if( sourceValue.IsString() &&
		epochValue.IsString() ){
		
		// deserialize sourceID
		SourceID source;
		if( !deserializeFromString( 
						source, 
						sourceValue.GetString() 
			)
		  ) return false;
	
		// deserialize commonTime
		CommonTime epoch;
		if( !deserializeFromString( 
						epoch, 
						epochValue.GetString() 
			)
		  ) return false;

		header = sourceEpochHeader(source, epoch);

		return true;

	}else{
		return false;
	}

	return true;
}

bool GDSSerializer::deserializeFromString(sourceEpochTypeHeader& header, string dataString){
	
	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& sourceValue = doc["source"];
	Value& epochValue  = doc["epoch"];
	Value& typeValue   = doc["type"];

	if( sourceValue.IsString() &&
		epochValue.IsString()  &&
		typeValue.IsString() ){
		
		// deserialize sourceID
		SourceID source;
		if( !deserializeFromString( 
						source, 
						sourceValue.GetString() 
			)
		  ) return false;
	
		// deserialize commonTime
		CommonTime epoch;
		if( !deserializeFromString( 
						epoch, 
						epochValue.GetString() 
			)
		  ) return false;
	    
		// deserialize typeID
		TypeID type;
		if( !deserializeFromString( 
						type, 
						typeValue.GetString() 
			)
		  ) return false;

		header = sourceEpochTypeHeader(source, epoch, type);

		return true;

	}else{
		return false;
	}

}


bool GDSSerializer::deserializeFromString(sourceEpochSatHeader& header, string dataString){

	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& sourceValue = doc["source"];
	Value& epochValue  = doc["epoch"];
	Value& satValue    = doc["sat"];

	if( sourceValue.IsString() &&
		epochValue.IsString()  &&
		satValue.IsString() ){
		
		// deserialize sourceID
		SourceID source;
		if( !deserializeFromString( 
						source, 
						sourceValue.GetString() 
			)
		  ) return false;
	
		// deserialize commonTime
		CommonTime epoch;
		if( !deserializeFromString( 
						epoch, 
						epochValue.GetString() 
			)
		  ) return false;
	    
		// deserialize satID
		SatID sat;
		if( !deserializeFromString( 
						sat, 
						satValue.GetString() 
			)
		  ) return false;

		header = sourceEpochSatHeader(source, epoch, sat);
		return true;
	}else{
		return false;
	}

	return true;
}


bool GDSSerializer::deserializeFromString(sourceEpochRinexHeader& header, string dataString){

	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& sourceValue    = doc["source"];
	Value& epochValue     = doc["epoch"];
	Value& antTypeValue   = doc["antType"];
	Value& antPosValue    = doc["antPos"];
	Value& flagValue      = doc["flag"];

	if( sourceValue.IsString()   &&
		epochValue.IsString()    &&
		antTypeValue.IsString()  &&
		antPosValue.IsString()   &&
		flagValue.IsInt() ){
		
		// deserialize sourceID
		SourceID source;
		if( !deserializeFromString( 
						source, 
						sourceValue.GetString() 
			)
		  ) return false;
	
		// deserialize commonTime
		CommonTime epoch;
		if( !deserializeFromString( 
						epoch, 
						epochValue.GetString() 
			)
		  ) return false;
	    
		// deserialize antPos
		Triple antPos;
		if( !deserializeFromString( 
						antPos, 
						antPosValue.GetString() 
			)
		  ) return false;

		header = sourceEpochRinexHeader(source, epoch, antTypeValue.GetString(), 
					antPos, (short)flagValue.GetInt() );

		return true;

	}else{
		return false;
	}

	return true;
}

string GDSSerializer::serializeToString(typeValueMap& dataMap){
	
	// declare json document
	Document doc(kObjectType);

	// convert TypeID double Value to json key-value
	for(typeValueMap::const_iterator it = dataMap.begin(); 
		it != dataMap.end(); it++){
		
		// convert TypeID to Value
		Value doubleKey(kStringType);
		doubleKey.SetString( serializeToString((TypeID&)(it->first)).c_str(), doc.GetAllocator() );
		
		// convert double to Value
		Value doubleValue(kNumberType);
		doubleValue.SetDouble(it->second);

		doc.AddMember( doubleKey, doubleValue, doc.GetAllocator() );
	}

	// serialize to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	string jsonString(buffer.GetString());

	return jsonString;
}


string GDSSerializer::serializeToString(satValueMap& dataMap  ){
	
	/// declare document
	Document doc(kObjectType);
	
	
	for( satValueMap::const_iterator it = dataMap.begin();
		 it != dataMap.end(); it++ ){

		// convert SatID to Value as key
		Value doubleKey(kStringType);
		doubleKey.SetString( serializeToString((SatID&)(it->first)).c_str(), doc.GetAllocator() );

		// convert double to Value
		Value doubleValue(kNumberType);
		doubleValue.SetDouble(it->second);

		doc.AddMember( doubleKey, doubleValue, doc.GetAllocator() );
	
	}

	// serialize to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(satTypeValueMap& dataMap  ){

	// declare document
	Document doc(kObjectType);

	for( satTypeValueMap::const_iterator it = dataMap.begin();
		it != dataMap.end(); it++){
		
		// convert SatID to Value as key
		Value key(kStringType);
		key.SetString( serializeToString((SatID&)(it->first)).c_str(), doc.GetAllocator() );

		// convert typeValueMap to Value
		Value val(kStringType);
		val.SetString( serializeToString((typeValueMap&)(it->second)).c_str(), doc.GetAllocator() );
		
		doc.AddMember( key, val, doc.GetAllocator() );
	}
	
	// serialize to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(gnssSatValue& dataMap){
	
	// declare document
	Document doc(kObjectType);
	
	// add header
	Value headerValue(kStringType);
	headerValue.SetString( serializeToString( dataMap.header ).c_str(), doc.GetAllocator() );
	doc.AddMember( "header", headerValue, doc.GetAllocator() );
	
	// add body
   	Value bodyValue(kStringType);
	bodyValue.SetString( serializeToString( dataMap.body ).c_str(), doc.GetAllocator() );
	doc.AddMember( "body", bodyValue, doc.GetAllocator() );
	
	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(gnssTypeValue& dataMap  ){
	
	// declare document
	Document doc(kObjectType);
	
	// add header
	Value headerValue(kStringType);
	headerValue.SetString( serializeToString( dataMap.header ).c_str(), doc.GetAllocator() );
	doc.AddMember( "header", headerValue, doc.GetAllocator() );
	
	// add body
   	Value bodyValue(kStringType);
	bodyValue.SetString( serializeToString( dataMap.body ).c_str(), doc.GetAllocator() );
	doc.AddMember( "body", bodyValue, doc.GetAllocator() );
	
	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(gnssSatTypeValue& dataMap  ){

	// declare document
	Document doc(kObjectType);
	
	// add header
	Value headerValue(kStringType);
	headerValue.SetString( serializeToString( dataMap.header ).c_str(), doc.GetAllocator() );
	doc.AddMember( "header", headerValue, doc.GetAllocator() );
	
	// add body
   	Value bodyValue(kStringType);
	bodyValue.SetString( serializeToString( dataMap.body ).c_str(), doc.GetAllocator() );
	doc.AddMember( "body", bodyValue, doc.GetAllocator() );
	
	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );

}

string GDSSerializer::serializeToString(gnssRinex& dataMap  ){

	// declare document
	Document doc(kObjectType);
	
	// add header
	Value headerValue(kStringType);
	headerValue.SetString( serializeToString( dataMap.header ).c_str(), doc.GetAllocator() );
	doc.AddMember( "header", headerValue, doc.GetAllocator() );
	
	// add body
   	Value bodyValue(kStringType);
	bodyValue.SetString( serializeToString( dataMap.body ).c_str(), doc.GetAllocator() );
	doc.AddMember( "body", bodyValue, doc.GetAllocator() );
	
	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(sourceDataMap& dataMap  ){

	// declare document
	Document doc(kObjectType);

	for(sourceDataMap::const_iterator it = dataMap.begin();
		it != dataMap.end(); it++){
	
		//key
		Value key(kStringType);
		key.SetString( serializeToString( (SourceID&)it->first ).c_str(), doc.GetAllocator() );
		
		// val
		Value val(kStringType);
		val.SetString( serializeToString( (satTypeValueMap&)it->second ).c_str(), doc.GetAllocator() );
		
		doc.AddMember( key, val, doc.GetAllocator() );
	}

	// convert to json string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}

string GDSSerializer::serializeToString(gnssDataMap& dataMap  ){
	
	// declare document
	Document doc(kObjectType);
	
	// convert multimap to map
	map< CommonTime, vector<string> > valueMap;

	for(gnssDataMap::const_iterator it = dataMap.begin();
		it != dataMap.end(); it++){
		
		map<CommonTime, vector<string> >::iterator vit = valueMap.find(it->first);

		if( vit == valueMap.end() ){
			
			vector<string> arrString;
			arrString.push_back( serializeToString( (sourceDataMap&)it->second ) );
			valueMap.insert(pair< CommonTime, vector<string> >(it->first, arrString));

		}else{
			vit->second.push_back( serializeToString( (sourceDataMap&)it->second ) );
		}
	}
	

	// convert map to Value
	for(map< CommonTime, vector<string> >::iterator it = valueMap.begin();
		it != valueMap.end(); it++){
		
		// convert CommonTime to string
		Value key(kStringType);
		key.SetString( serializeToString( (CommonTime&)it->first ).c_str(), doc.GetAllocator() );
        
		Value val(kArrayType);

		for(vector<string>::iterator vit = it->second.begin();
			vit != it->second.end(); vit++){
			
			Value stringValue(kStringType);
			stringValue.SetString( (*vit).c_str(), doc.GetAllocator() );
			
			val.PushBack( stringValue, doc.GetAllocator() );
		}

		doc.AddMember( key, val, doc.GetAllocator() );
	}

	// convert json to string
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return string( buffer.GetString() );
}
		
/// GDSSerializer::Deserialize from string for all Data Structures
bool GDSSerializer::deserializeFromString(typeValueMap& dataMap, string dataString){

	/// declare the doc
	Document doc;

	/// parse from the string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	/// parse error
	if(doc.HasParseError()) return false;
	
	/// convert json to typeValueMap
	for(Value::ConstMemberIterator it = doc.MemberBegin();
		it != doc.MemberEnd(); it++){
		
		// convert string to typeid
		TypeID id;
		if( !deserializeFromString(
						id, 
						it->name.GetString() 
			)
		  ) continue;

		// get double value
		double value = it->value.GetDouble();
		dataMap[id] = value;
	}

	return true;
}

bool GDSSerializer::deserializeFromString(satValueMap& dataMap, string dataString  ){

	/// declare document
	Document doc;

	/// parse from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	/// parse error
	if( doc.HasParseError() ) return false;

	for(Value::ConstMemberIterator it = doc.MemberBegin();
		it != doc.MemberEnd(); it++){
		
		// convert string to SatID
		SatID satID;
		if( !deserializeFromString( 
						satID, 
						it->name.GetString() 
			)
		  ) continue;

		// get double value
		double value = it->value.GetDouble();
		dataMap[satID] = value;
	}

	return true;
}

bool GDSSerializer::deserializeFromString(satTypeValueMap& dataMap, string dataString  ){

	// declare document
	Document doc;
	
	// pasre from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str() ) ); 
	
	if( doc.HasParseError() ) return false;

	for(Value::ConstMemberIterator it = doc.MemberBegin();
		it != doc.MemberEnd(); it++){

		// convert string to SatID
		SatID satID;
		if( !deserializeFromString( 
						satID, 
						it->name.GetString() 
			)
		  ) continue;

		// convert string to typeValueMap
		typeValueMap tMap;
		if( !deserializeFromString( 
						tMap, 
						it->value.GetString() 
			)
		  ) continue;

		// insert to satTypeValueMap
		dataMap[satID] = tMap;
	}

	return true;
}
bool GDSSerializer::deserializeFromString(gnssSatValue& dataMap, string dataString  ){

	// declare document
	Document doc;
	
	// parse from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& headerValue = doc["header"];
	Value& bodyValue   = doc["body"];

	if( headerValue.IsString() &&
		bodyValue.IsString() ){
		
		// convert string to header
		if( !deserializeFromString( 
					dataMap.header, 
					headerValue.GetString() 
			 ) 
		  ) return false;
	
		// convert string to body
		if( !deserializeFromString( 
					dataMap.body, 
					bodyValue.GetString() 
			) 
		  ) return false;

		return true;

	}else{
		return false;
	}
}
bool GDSSerializer::deserializeFromString(gnssTypeValue& dataMap, string dataString  ){

	// declare document
	Document doc;
	
	// parse from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& headerValue = doc["header"];
	Value& bodyValue   = doc["body"];

	if( headerValue.IsString() &&
		bodyValue.IsString() ){
		
		// convert string to header
		if( !deserializeFromString( 
						dataMap.header, 
						headerValue.GetString() 
		     )
		  ) return false;

		
		// convert string tp body
		if( !deserializeFromString( 
						dataMap.body, 
						bodyValue.GetString() 
			) 
		  ) return false;

		return true;
	}else{
		return false;
	}
}
bool GDSSerializer::deserializeFromString(gnssSatTypeValue& dataMap, string dataString  ){

	// declare document
	Document doc;
	
	// parse from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& headerValue = doc["header"];
	Value& bodyValue   = doc["body"];

	if( headerValue.IsString() &&
		bodyValue.IsString() ){
		
		// convert string to header
		if( !deserializeFromString( 
						dataMap.header, 
						headerValue.GetString() 
			) 
		  ) return false;
		
		// convert string tp body
		if( !deserializeFromString( 
						dataMap.body, 
						bodyValue.GetString() 
			) 
		  ) return false;
		
		return true;
	}else{
		return false;
	}

	return true;
}
bool GDSSerializer::deserializeFromString(gnssRinex& dataMap, string dataString  ){

	// declare document
	Document doc;
	
	// parse from string
	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	Value& headerValue = doc["header"];
	Value& bodyValue   = doc["body"];

	if( headerValue.IsString() &&
		bodyValue.IsString() ){
		
		// convert string to header
		if( !deserializeFromString( 
						dataMap.header, 
						headerValue.GetString() 
			) 
		  ) return false;
		
		// convert string tp body
		if( !deserializeFromString( 
						dataMap.body, 
						bodyValue.GetString() 
			) 
		  ) return false;
		return true;

	}else{
		return false;
	}

	return true;
}
bool GDSSerializer::deserializeFromString(sourceDataMap& dataMap, string dataString){

	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	for(Value::ConstMemberIterator it = doc.MemberBegin();
		it != doc.MemberEnd(); it++){

		// convert string to sourceID
		SourceID source;
		if( !deserializeFromString( 
						source, 
						it->name.GetString() 
			)
		  ) continue;

		// convert string to satTypeValueMap
		satTypeValueMap val;
		if( !deserializeFromString( 
						val, 
						it->value.GetString() 
			)
		  ) continue;

		dataMap[source] = val;
	}

	return true;
}
bool GDSSerializer::deserializeFromString(gnssDataMap& dataMap, string dataString  ){
	
	Document doc;

	doc.Parse<kParseDefaultFlags>( StringRef(dataString.c_str()) );

	if( doc.HasParseError() ) return false;

	for(Value::ConstMemberIterator it = doc.MemberBegin();
		it != doc.MemberEnd(); it++){
		
		// convert string to CommonTime 
		CommonTime time;
		if( !deserializeFromString(
						time,
						it->name.GetString()
			)
		  ) continue;

		if( !it->value.IsArray() ) continue;

		for(Value::ConstValueIterator itValue = it->value.Begin();
			itValue != it->value.End(); itValue++){
			
			sourceDataMap sMap;

			if( !deserializeFromString(
							sMap,
							itValue->GetString()
				)
			  ) continue;
			
			dataMap.insert(pair<CommonTime, sourceDataMap>(time,sMap));
		}

	}

	return true;
}





}
