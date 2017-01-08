#pragma ident "$Id$"

/**
 * @file Rinex3ClockData.cpp
 * Encapsulate RINEX3 clock data file, including I/O
 * See more at: ftp://igscb.jpl.nasa.gov/pub/data/format/rinex_clock.txt
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================

//GPSTk
#include "TimeString.hpp"
#include "CivilTime.hpp"
#include "Rinex3ClockData.hpp"
#include "Rinex3ClockStream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
      // Debug output function.
   void Rinex3ClockData::dump(std::ostream& s) const
   {

//      cout << "WARNING: There is no implementation for "
//           << "Rinex3ClockData::dump()"
//           << endl;

		// dump record type, sat id / site, current epoch, and data
      s << " " << datatype;
      if(datatype == string("AR")) s << " " << site;
      else s << " " << sat.toString();
      s << " " << printTime(time,"%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g %P");
      s << scientific << setprecision(12)
         << " " << setw(19) << bias
         << " " << setw(19) << sig_bias;
      if(drift != 0.0) s << " " << setw(19) << drift; else s << " 0.0";
      if(sig_drift != 0.0) s << " " << setw(19) << sig_drift; else s << " 0.0";
      if(accel != 0.0) s << " " << setw(19) << accel; else s << " 0.0";
      if(sig_accel != 0.0) s << " " << setw(19) << sig_accel; else s << " 0.0";
      s << endl;

      return;

   }  // End of method 'Rinex3ClockData::dump(std::ostream& s)'



   void Rinex3ClockData::reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               StringUtils::StringException)
   {
//      cout << "WARNING: There is no implementation for "
//           << "Rinex3ClockData::reallyPutRecord()"
//           << endl;

			// cast the stream to be an RinexClockStream
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(s);

      string line;

      line = datatype;
      line += string(1,' ');

      if(datatype == string("AR")) {
         line += rightJustify(site,4);
      }
      else if(datatype == string("AS")) {
         line += string(1,sat.systemChar());
         line += rightJustify(asString(sat.id),2);
         if(line[4] == ' ') line[4] = '0';
         line += string(1,' ');
      }
      else {
         FFStreamError e("Unknown data type: " + datatype);
         GPSTK_THROW(e);
      }
      line += string(1,' ');

      line += printTime(time,"%4Y %02m %02d %02H %02M %9.6f");

      // must count the data to output
      int n(2);
      if(drift != 0.0) n=3;
      if(sig_drift != 0.0) n=4;
      if(accel != 0.0) n=5;
      if(sig_accel != 0.0) n=6;
      line += rightJustify(asString(n),3);
      line += string(3,' ');

      line += doubleToScientific(bias, 19, 12, 2);
      line += string(1,' ');
      line += doubleToScientific(sig_bias, 19, 12, 2);

      strm << line << endl;
      strm.lineNumber++;

      // continuation line
      if(n > 2) {
         line = doubleToScientific(drift, 19, 12, 2);
         line += string(1,' ');
         if(n > 3) {
            line += doubleToScientific(sig_drift, 19, 12, 2);
            line += string(1,' ');
         }
         if(n > 4) {
            line += doubleToScientific(accel, 19, 12, 2);
            line += string(1,' ');
         }
         if(n > 5) {
            line += doubleToScientific(sig_accel, 19, 12, 2);
            line += string(1,' ');
         }
         strm << line << endl;
         strm.lineNumber++;
      }


      return;

   }  // End of method 'Rinex3ClockData::reallyPutRecord()'



      // This function parses the entire header from the given stream
   void Rinex3ClockData::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, 
            StringUtils::StringException)
   {
			// cast the stream to be an Rinex3ClockStream
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

			// If the header hasn't been read, read it...
		if(!strm.headerRead) strm >> strm.header;

			// Clear out this object
		Rinex3ClockHeader& hdr = strm.header;

		Rinex3ClockData rod;
		*this = rod; 

		string line;
		strm.formattedGetLine(line,true);      // true means 'expect possible EOF'
		stripTrailing(line);
		if(line.length() < 59) {
			FFStreamError e("Short line : " + line);
			GPSTK_THROW(e);
		}

		//cout << "Data Line: /" << line << "/" << endl;
		datatype = line.substr(0,2);
		//cout << "datatype: " << datatype << endl;
		site = line.substr(3,4);
		if(datatype == string("AS")) {
			strip(site);
			int prn(asInt(site.substr(1,2)));
			if(site[0] == 'G') sat = RinexSatID(prn,RinexSatID::systemGPS);
			else if(site[0] == 'R') sat = RinexSatID(prn,RinexSatID::systemGlonass);
			else if( site[0] == 'E') 
			{
				sat = RinexSatID(prn, RinexSatID::systemGalileo);
			}
			else if( site[0] == 'C' )
			{
				sat = RinexSatID(prn, RinexSatID::systemBeiDou);
			}
			else if( site[0] == 'J' )
			{
				sat = RinexSatID(prn, RinexSatID::systemQZSS);
			}
			else {
				FFStreamError e("Invalid sat : /" + site + "/");
				GPSTK_THROW(e);
			}

			site = string();
		}  // End of 'if(datatype == string("AS"))'

		time = CivilTime(asInt(line.substr( 8,4)),
							asInt(line.substr(12,3)),
							asInt(line.substr(15,3)),
							asInt(line.substr(18,3)),
							asInt(line.substr(21,3)),
							asDouble(line.substr(24,10)),
							TimeSystem::Any);

		int n(asInt(line.substr(34,3)));

		bias = asDouble(line.substr(40,19));
		if(n > 1 && line.length() >= 59) sig_bias = asDouble(line.substr(60,19));

		if(n > 2) {
			strm.formattedGetLine(line,true);
			stripTrailing(line);
			if(int(line.length()) < (n-2)*20-1) {
				FFStreamError e("Short line : " + line);
				GPSTK_THROW(e);
			}
			drift =     asDouble(line.substr( 0,19));
			if(n > 3) sig_drift = asDouble(line.substr(20,19));
			if(n > 4) accel     = asDouble(line.substr(40,19));
			if(n > 5) sig_accel = asDouble(line.substr(60,19));
		}  

//      string line;

//      strm.formattedGetLine(line, true);
      //cout << line << endl;

      return;

   }  // End of method 'Rinex3ClockData::reallyGetRecord(FFStream& ffs)'



         /** This function constructs a CommonTime object from the given parameters.
          * @param line       the encoded time string found in the 
          *                   RINEX clock data record.
          */
   CommonTime Rinex3ClockData::parseTime(const string& line) const
   {

      int year, month, day, hour, min;
      double sec;

      year  = asInt(   line.substr( 0, 4 ));
      month = asInt(   line.substr( 4, 3 ));
      day   = asInt(   line.substr( 7, 3 ));
      hour  = asInt(   line.substr(10, 3 ));
      min   = asInt(   line.substr(13, 3 ));
      sec   = asDouble(line.substr(16, 10));

      return CivilTime(year, month, day, hour, min, sec).convertToCommonTime();

   }  // End of method 'Rinex3ClockData::parseTime()'



      /// Converts the CommonTime \a dt into a Rinex3 Clock time
      /// string for the header
   string Rinex3ClockData::writeTime(const CommonTime& dt) const
      throw(gpstk::StringUtils::StringException)
   {

      if (dt == CommonTime::BEGINNING_OF_TIME)
      {
         return string(36, ' ');
      }

      string line;
      CivilTime civTime(dt);
      line  = rightJustify(asString<short>(civTime.year), 4);
      line += rightJustify(asString<short>(civTime.month), 3);
      line += rightJustify(asString<short>(civTime.day), 3);
      line += rightJustify(asString<short>(civTime.hour), 3);
      line += rightJustify(asString<short>(civTime.minute), 3);
      line += rightJustify(asString(civTime.second, 6), 10);

      return line;

   }  // End of method 'Rinex3ClockData::writeTime(const CommonTime& dt)'


}  // End of namespace gpstk
