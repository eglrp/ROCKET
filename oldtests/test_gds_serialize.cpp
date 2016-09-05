#include "GNSSDataSerializer.hpp"


using namespace gpstk;


int main(int argc, char **argv){
	
	
	// TypeID
	TypeID id1(TypeID::recX);
	TypeID id2;
	GDSSerializer::serializeToFile<TypeID>(id1, "typeid.json");
	GDSSerializer::deserializeFromFile<TypeID>(id2, "typeid.json");
	cout<<"deserialize[TypeID]="<<id2<<endl;

	// SatID
	SatID sat1(2,SatID::systemGPS);
	SatID sat2;
	GDSSerializer::serializeToFile<SatID>(sat1, "satid.json");
	GDSSerializer::deserializeFromFile<SatID>(sat2, "satid.json");
	cout<<"deserialize[SatID]="<<sat2<<endl;
    
	// SourceID
	SourceID sourceid1(SourceID::GPS,"BJFS");
	SourceID sourceid2;
	GDSSerializer::serializeToFile<SourceID>(sourceid1, "sourceid.json");
	GDSSerializer::deserializeFromFile<SourceID>(sourceid2, "sourceid.json");
	cout<<"deserialize[SourceID]="<<sourceid2<<endl;
	
	// CommonTime
	CommonTime time1;
	time1.set(2014,0,0,TimeSystem(TimeSystem::GPS));
	CommonTime time2;
	GDSSerializer::serializeToFile<CommonTime>(time1, "commontime.json");
	GDSSerializer::deserializeFromFile<CommonTime>(time2, "commontime.json");
	cout<<"deserialize[CommonTime]="<<time2<<endl;
	
	// Triple
	Triple trip1(100,200,300);
	Triple trip2;
	GDSSerializer::serializeToFile<Triple>(trip1, "triple.json");
	GDSSerializer::deserializeFromFile<Triple>(trip2,"triple.json");
	cout<<"deserialize[Tripe]="<<trip2<<endl;
    
    // sourceEpochHeader
	sourceEpochHeader se_header1;
	se_header1.source=sourceid1;
	se_header1.epoch=time1;
	sourceEpochHeader se_header2;
	GDSSerializer::serializeToFile<sourceEpochHeader>(se_header1,"sourceepochheader.json");
	GDSSerializer::deserializeFromFile<sourceEpochHeader>(se_header2,"sourceepochheader.json");
	cout<<"deserialize[sourceEpochHeader]="<<se_header2<<endl;
	
	// sourceEpochTypeHeader
	sourceEpochTypeHeader set_header1;
	set_header1.source=sourceid1;
	set_header1.epoch=time1;
	set_header1.type=id1;
	sourceEpochTypeHeader set_header2;
	GDSSerializer::serializeToFile<sourceEpochTypeHeader>(set_header1,"sourceepochtypeheader.json");
	GDSSerializer::deserializeFromFile<sourceEpochTypeHeader>(set_header2,"sourceepochtypeheader.json");
	cout<<"deserialize[sourceEpochTypeHeader]="<<set_header2<<endl;
	
	// sourceEpochSatHeader
	sourceEpochSatHeader ses_header1;
	ses_header1.source=sourceid1;
	ses_header1.epoch=time1;
	ses_header1.satellite=sat1;
	sourceEpochSatHeader ses_header2;
	GDSSerializer::serializeToFile<sourceEpochSatHeader>(ses_header1,"sourceepochsatheader.json");
	GDSSerializer::deserializeFromFile<sourceEpochSatHeader>(ses_header2,"sourceepochsatheader.json");
	cout<<"deserialize[sourceEpochSatHeader]="<<ses_header2<<endl;

	// sourceEpochRinexHeader
	sourceEpochRinexHeader ser_header1;
	ser_header1.source=sourceid1;
	ser_header1.epoch=time1;
	ser_header1.antennaType="TB";
	ser_header1.antennaPosition = Triple(100,200,300);
	ser_header1.epochFlag=0;
	sourceEpochRinexHeader ser_header2;
	GDSSerializer::serializeToFile<sourceEpochRinexHeader>(ser_header1,"sourceepochrinexheader.json");
	GDSSerializer::deserializeFromFile<sourceEpochRinexHeader>(ser_header2,"sourceepochrinexheader.json");
	cout<<"deserialize[sourceEpochRinexHeader]="<<ser_header2<<endl;

	// typeValueMap
	typeValueMap tvmap1,tvmap2;
	tvmap1[TypeID::recX]=100;
	tvmap1[TypeID::recY]=200;
	tvmap1[TypeID::recZ]=300;
	GDSSerializer::serializeToFile<typeValueMap>(tvmap1,"typevaluemap.json");
	GDSSerializer::deserializeFromFile<typeValueMap>(tvmap2,"typevaluemap.json");
	cout<<"deserialize[typeValueMap]="<<GDSSerializer::serializeToString(tvmap2)<<endl;
	
	// satValueMap
	satValueMap svmap1, svmap2;
	svmap1[SatID(1,SatID::systemGPS)]=10;
	svmap1[SatID(2,SatID::systemGPS)]=20;
	svmap1[SatID(3,SatID::systemGPS)]=30;
	GDSSerializer::serializeToFile<satValueMap>(svmap1,"satvaluemap.json");
	GDSSerializer::deserializeFromFile<satValueMap>(svmap2,"satvaluemap.json");
	cout<<"deserialize[satValueMap]="<<GDSSerializer::serializeToString(svmap2)<<endl;
	
	// satTypeValueMap
	satTypeValueMap stvmap1, stvmap2;
	stvmap1[SatID(4,SatID::systemGPS)]=tvmap1;
	stvmap1[SatID(5,SatID::systemGPS)]=tvmap1;
	stvmap1[SatID(6,SatID::systemGPS)]=tvmap1;
	GDSSerializer::serializeToFile<satTypeValueMap>(stvmap1,"sattypevaluemap.json");
	GDSSerializer::deserializeFromFile<satTypeValueMap>(stvmap2,"sattypevaluemap.json");
	cout<<"deserialize[satTypeValueMap]="<<GDSSerializer::serializeToString(stvmap2)<<endl;

	// gnssSatValue
	gnssSatValue gsvalue1, gsvalue2;
	gsvalue1.header = set_header1;
	gsvalue1.body   = svmap1;
	GDSSerializer::serializeToFile<gnssSatValue>(gsvalue1,"gnsssatvalue.json");
	GDSSerializer::deserializeFromFile<gnssSatValue>(gsvalue2,"gnsssatvalue.json");
	cout<<"deserialize[gnssSatValue]="<<GDSSerializer::serializeToString(gsvalue2)<<endl;
		
	// gnssTypeValue
	gnssTypeValue gtvalue1, gtvalue2;
	gtvalue1.header = ses_header1;
	gtvalue1.body   = tvmap1;
	GDSSerializer::serializeToFile<gnssTypeValue>(gtvalue1,"gnsstypevalue.json");
	GDSSerializer::deserializeFromFile<gnssTypeValue>(gtvalue2,"gnsstypevalue.json");
	cout<<"deserialize[gnssTypeValue]="<<GDSSerializer::serializeToString(gtvalue2)<<endl;
	
	// gnssSatTypeValue
	gnssSatTypeValue gstvalue1, gstvalue2;
	gstvalue1.header = se_header1;
	gstvalue1.body   = stvmap1;
	GDSSerializer::serializeToFile<gnssSatTypeValue>(gstvalue1,"gnsssattypevalue.json");
	GDSSerializer::deserializeFromFile<gnssSatTypeValue>(gstvalue2,"gnsssattypevalue.json");
	cout<<"deserialize[gnssSatTypeValue]="<<GDSSerializer::serializeToString(gstvalue2)<<endl;

	// gnssRinex
	gnssRinex grin1, grin2;
	grin1.header = ser_header1;
	grin1.body   = stvmap1;
	GDSSerializer::serializeToFile<gnssRinex>(grin1,"gnssrinex.json");
	GDSSerializer::deserializeFromFile<gnssRinex>(grin2,"gnssrinex.json");
	cout<<"deserialize[gnssRinex]="<<GDSSerializer::serializeToString(grin2)<<endl;

	// sourceDataMap
	sourceDataMap sdmap1, sdmap2;
	sdmap1[SourceID(SourceID::GPS,"BJFS")] = stvmap1;
	sdmap1[SourceID(SourceID::GPS,"WHUN")] = stvmap1;
	sdmap1[SourceID(SourceID::GPS,"SHAO")] = stvmap1;
	GDSSerializer::serializeToFile<sourceDataMap>(sdmap1,"sourcedatamap.json");
	GDSSerializer::deserializeFromFile<sourceDataMap>(sdmap2,"sourcedatamap.json");
	cout<<"deserialize[sourceDataMap]="<<GDSSerializer::serializeToString(sdmap2)<<endl;

	// gnssDataMap
	gnssDataMap gdmap1, gdmap2;
	gdmap1.insert(pair<CommonTime, sourceDataMap>(time1, sdmap1));
	gdmap1.insert(pair<CommonTime, sourceDataMap>(time1, sdmap1));
	gdmap1.insert(pair<CommonTime, sourceDataMap>(time1+2000, sdmap1));
	gdmap1.insert(pair<CommonTime, sourceDataMap>(time1+3000, sdmap1));
	GDSSerializer::serializeToFile<gnssDataMap>(gdmap1, "gnssdatamap.json");
	GDSSerializer::deserializeFromFile<gnssDataMap>(gdmap2, "gnssdatamap.json");
	cout<<"deserialize[gnssDataMap]="<<GDSSerializer::serializeToString(gdmap2)<<endl;

	return 0;
}
