#pragma ident "$Id$"

/**
 * @file TypeID.cpp
 * gpstk::TypeID - Identifies types of values
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================

#include "TypeID.hpp"


namespace gpstk
{

   std::map< TypeID::ValueType, std::string > TypeID::tStrings;


   TypeID::Initializer TypeIDsingleton;

      // It should be initialize by false, NEVER CHANGE IT!!!
   bool TypeID::bUserTypeIDRegistered = false;

      // Map holding user defined TypeIDs by a string
   std::map<std::string,TypeID> TypeID::mapUserTypeID;


      // Let's assign type descriptions
   TypeID::Initializer::Initializer()
   {
      tStrings[Unknown]             = "UnknownType";

      tStrings[C1C]                 = "C1C";
      tStrings[C1S]                 = "C1S";
      tStrings[C1L]                 = "C1L";
      tStrings[C1X]                 = "C1X";
      tStrings[C1P]                 = "C1P";
      tStrings[C1W]                 = "C1W";
      tStrings[C1Y]                 = "C1Y";
      tStrings[C1M]                 = "C1M";
      tStrings[C1A]                 = "C1A";
      tStrings[C1B]                 = "C1B";
      tStrings[C1Z]                 = "C1Z";

      tStrings[L1C]                 = "L1C";
      tStrings[L1S]                 = "L1S";
      tStrings[L1L]                 = "L1L";
      tStrings[L1X]                 = "L1X";
      tStrings[L1P]                 = "L1P";
      tStrings[L1W]                 = "L1W";
      tStrings[L1Y]                 = "L1Y";
      tStrings[L1M]                 = "L1M";
      tStrings[L1N]                 = "L1N";
      tStrings[L1A]                 = "L1A";
      tStrings[L1B]                 = "L1B";
      tStrings[L1Z]                 = "L1Z";

      tStrings[D1C]                 = "D1C";
      tStrings[D1S]                 = "D1S";
      tStrings[D1L]                 = "D1L";
      tStrings[D1X]                 = "D1X";
      tStrings[D1P]                 = "D1P";
      tStrings[D1W]                 = "D1W";
      tStrings[D1Y]                 = "D1Y";
      tStrings[D1M]                 = "D1M";
      tStrings[D1N]                 = "D1N";
      tStrings[D1A]                 = "D1A";
      tStrings[D1B]                 = "D1B";
      tStrings[D1Z]                 = "D1Z";

      tStrings[S1C]                 = "S1C";
      tStrings[S1S]                 = "S1S";
      tStrings[S1L]                 = "S1L";
      tStrings[S1X]                 = "S1X";
      tStrings[S1P]                 = "S1P";
      tStrings[S1W]                 = "S1W";
      tStrings[S1Y]                 = "S1Y";
      tStrings[S1M]                 = "S1M";
      tStrings[S1N]                 = "S1N";
      tStrings[S1A]                 = "S1A";
      tStrings[S1B]                 = "S1B";
      tStrings[S1Z]                 = "S1Z";


      tStrings[C2C]                 = "C2C";
      tStrings[C2D]                 = "C2D";
      tStrings[C2S]                 = "C2S";
      tStrings[C2L]                 = "C2L";
      tStrings[C2X]                 = "C2X";
      tStrings[C2P]                 = "C2P";
      tStrings[C2W]                 = "C2W";
      tStrings[C2Y]                 = "C2Y";
      tStrings[C2M]                 = "C2M";
      tStrings[C2I]                 = "C2I";
      tStrings[C2Q]                 = "C2Q";

      tStrings[L2C]                 = "L2C";
      tStrings[L2D]                 = "L2D";
      tStrings[L2S]                 = "L2S";
      tStrings[L2L]                 = "L2L";
      tStrings[L2X]                 = "L2X";
      tStrings[L2P]                 = "L2P";
      tStrings[L2W]                 = "L2W";
      tStrings[L2Y]                 = "L2Y";
      tStrings[L2M]                 = "L2M";
      tStrings[L2N]                 = "L2N";
      tStrings[L2I]                 = "L2I";
      tStrings[L2Q]                 = "L2Q";

      tStrings[D2C]                 = "D2C";
      tStrings[D2D]                 = "D2D";
      tStrings[D2S]                 = "D2S";
      tStrings[D2L]                 = "D2L";
      tStrings[D2X]                 = "D2X";
      tStrings[D2P]                 = "D2P";
      tStrings[D2W]                 = "D2W";
      tStrings[D2Y]                 = "D2Y";
      tStrings[D2M]                 = "D2M";
      tStrings[D2N]                 = "D2N";
      tStrings[D2I]                 = "D2I";
      tStrings[D2Q]                 = "D2Q";

      tStrings[S2C]                 = "S2C";
      tStrings[S2D]                 = "S2D";
      tStrings[S2S]                 = "S2S";
      tStrings[S2L]                 = "S2L";
      tStrings[S2X]                 = "S2X";
      tStrings[S2P]                 = "S2P";
      tStrings[S2W]                 = "S2W";
      tStrings[S2Y]                 = "S2Y";
      tStrings[S2M]                 = "S2M";
      tStrings[S2N]                 = "S2N";
      tStrings[S2I]                 = "S2I";
      tStrings[S2Q]                 = "S2Q";


      tStrings[C3I]                 = "C3I";
      tStrings[C3Q]                 = "C3Q";
      tStrings[C3X]                 = "C3X";

      tStrings[L3I]                 = "L3I";
      tStrings[L3Q]                 = "L3Q";
      tStrings[L3X]                 = "L3X";

      tStrings[D3I]                 = "D3I";
      tStrings[D3Q]                 = "D3Q";
      tStrings[D3X]                 = "D3X";

      tStrings[S3I]                 = "S3I";
      tStrings[S3Q]                 = "S3Q";
      tStrings[S3X]                 = "S3X";


      tStrings[C5I]                 = "C5I";
      tStrings[C5Q]                 = "C5Q";
      tStrings[C5X]                 = "C5X";
      tStrings[C5A]                 = "C5A";
      tStrings[C5B]                 = "C5B";
      tStrings[C5C]                 = "C5C";

      tStrings[L5I]                 = "L5I";
      tStrings[L5Q]                 = "L5Q";
      tStrings[L5X]                 = "L5X";
      tStrings[L5A]                 = "L5A";
      tStrings[L5B]                 = "L5B";
      tStrings[L5C]                 = "L5C";

      tStrings[D5I]                 = "D5I";
      tStrings[D5Q]                 = "D5Q";
      tStrings[D5X]                 = "D5X";
      tStrings[D5A]                 = "D5A";
      tStrings[D5B]                 = "D5B";
      tStrings[D5C]                 = "D5C";

      tStrings[S5I]                 = "S5I";
      tStrings[S5Q]                 = "S5Q";
      tStrings[S5X]                 = "S5X";
      tStrings[S5A]                 = "S5A";
      tStrings[S5B]                 = "S5B";
      tStrings[S5C]                 = "S5C";


      tStrings[C6A]                 = "C6A";
      tStrings[C6B]                 = "C6B";
      tStrings[C6C]                 = "C6C";
      tStrings[C6X]                 = "C6X";
      tStrings[C6Z]                 = "C6Z";
      tStrings[C6S]                 = "C6S";
      tStrings[C6L]                 = "C6L";
      tStrings[C6I]                 = "C6I";
      tStrings[C6Q]                 = "C6Q";

      tStrings[L6A]                 = "L6A";
      tStrings[L6B]                 = "L6B";
      tStrings[L6C]                 = "L6C";
      tStrings[L6X]                 = "L6X";
      tStrings[L6Z]                 = "L6Z";
      tStrings[L6S]                 = "L6S";
      tStrings[L6L]                 = "L6L";
      tStrings[L6I]                 = "L6I";
      tStrings[L6Q]                 = "L6Q";

      tStrings[D6A]                 = "D6A";
      tStrings[D6B]                 = "D6B";
      tStrings[D6C]                 = "D6C";
      tStrings[D6X]                 = "D6X";
      tStrings[D6Z]                 = "D6Z";
      tStrings[D6S]                 = "D6S";
      tStrings[D6L]                 = "D6L";
      tStrings[D6I]                 = "D6I";
      tStrings[D6Q]                 = "D6Q";

      tStrings[S6A]                 = "S6A";
      tStrings[S6B]                 = "S6B";
      tStrings[S6C]                 = "S6C";
      tStrings[S6X]                 = "S6X";
      tStrings[S6Z]                 = "S6Z";
      tStrings[S6S]                 = "S6S";
      tStrings[S6L]                 = "S6L";
      tStrings[S6I]                 = "S6I";
      tStrings[S6Q]                 = "S6Q";


      tStrings[C7I]                 = "C7I";
      tStrings[C7Q]                 = "C7Q";
      tStrings[C7X]                 = "C7X";

      tStrings[L7I]                 = "L7I";
      tStrings[L7Q]                 = "L7Q";
      tStrings[L7X]                 = "L7X";

      tStrings[D7I]                 = "D7I";
      tStrings[D7Q]                 = "D7Q";
      tStrings[D7X]                 = "D7X";

      tStrings[S7I]                 = "S7I";
      tStrings[S7Q]                 = "S7Q";
      tStrings[S7X]                 = "S7X";


      tStrings[C8I]                 = "C8I";
      tStrings[C8Q]                 = "C8Q";
      tStrings[C8X]                 = "C8X";

      tStrings[L8I]                 = "L8I";
      tStrings[L8Q]                 = "L8Q";
      tStrings[L8X]                 = "L8X";

      tStrings[D8I]                 = "D8I";
      tStrings[D8Q]                 = "D8Q";
      tStrings[D8X]                 = "D8X";

      tStrings[S8I]                 = "S8I";
      tStrings[S8Q]                 = "S8Q";
      tStrings[S8X]                 = "S8X";


      tStrings[C9A]                 = "C9A";
      tStrings[C9B]                 = "C9B";
      tStrings[C9C]                 = "C9C";
      tStrings[C9X]                 = "C9X";

      tStrings[L9A]                 = "L9A";
      tStrings[L9B]                 = "L9B";
      tStrings[L9C]                 = "L9C";
      tStrings[L9X]                 = "L9X";

      tStrings[D9A]                 = "D9A";
      tStrings[D9B]                 = "D9B";
      tStrings[D9C]                 = "D9C";
      tStrings[D9X]                 = "D9X";

      tStrings[S9A]                 = "S9A";
      tStrings[S9B]                 = "S9B";
      tStrings[S9C]                 = "S9C";
      tStrings[S9X]                 = "S9X";

      tStrings[LLI1]                = "LLI1";
      tStrings[LLI2]                = "LLI2";
      tStrings[LLI3]                = "LLI3";
      tStrings[LLI5]                = "LLI5";
      tStrings[LLI6]                = "LLI6";
      tStrings[LLI7]                = "LLI7";
      tStrings[LLI8]                = "LLI8";
      tStrings[LLI9]                = "LLI9";

      tStrings[SSI1]                = "SSI1";
      tStrings[SSI2]                = "SSI2";
      tStrings[SSI3]                = "SSI3";
      tStrings[SSI5]                = "SSI5";
      tStrings[SSI6]                = "SSI6";
      tStrings[SSI7]                = "SSI7";
      tStrings[SSI8]                = "SSI8";
      tStrings[SSI9]                = "SSI9";

      tStrings[C1]                  = "C1";
      tStrings[C2]                  = "C2";
      tStrings[C3]                  = "C3";
      tStrings[C5]                  = "C5";
      tStrings[C6]                  = "C6";
      tStrings[C7]                  = "C7";
      tStrings[C8]                  = "C8";
      tStrings[C9]                  = "C9";

      tStrings[P1]                  = "P1";
      tStrings[P2]                  = "P2";
      tStrings[P3]                  = "P3";
      tStrings[P5]                  = "P5";
      tStrings[P6]                  = "P6";
      tStrings[P7]                  = "P7";
      tStrings[P8]                  = "P8";
      tStrings[P9]                  = "P9";

      tStrings[L1]                  = "L1";
      tStrings[L2]                  = "L2";
      tStrings[L3]                  = "L3";
      tStrings[L5]                  = "L5";
      tStrings[L6]                  = "L6";
      tStrings[L7]                  = "L7";
      tStrings[L8]                  = "L8";
      tStrings[L9]                  = "L9";

      tStrings[D1]                  = "D1";
      tStrings[D2]                  = "D2";
      tStrings[D3]                  = "D3";
      tStrings[D5]                  = "D5";
      tStrings[D6]                  = "D6";
      tStrings[D7]                  = "D7";
      tStrings[D8]                  = "D8";
      tStrings[D9]                  = "D9";

      tStrings[S1]                  = "S1";
      tStrings[S2]                  = "S2";
      tStrings[S3]                  = "S3";
      tStrings[S5]                  = "S5";
      tStrings[S6]                  = "S6";
      tStrings[S7]                  = "S7";
      tStrings[S8]                  = "S8";
      tStrings[S9]                  = "S9";


      tStrings[CSL1]                = "CSL1";
      tStrings[CSL2]                = "CSL2";
      tStrings[CSL3]                = "CSL3";
      tStrings[CSL5]                = "CSL5";
      tStrings[CSL6]                = "CSL6";
      tStrings[CSL7]                = "CSL7";
      tStrings[CSL8]                = "CSL8";
      tStrings[CSL9]                = "CSL9";

      tStrings[PC]                  = "PC";
      tStrings[LC]                  = "LC";
      tStrings[PI]                  = "PI";
      tStrings[LI]                  = "LI";
      tStrings[PW]                  = "PW";
      tStrings[LW]                  = "LW";
      tStrings[PN]                  = "PN";
      tStrings[LN]                  = "LN";
      tStrings[MW]                  = "MW";
      tStrings[WL]                  = "WL";
      tStrings[WL1]                 = "WL1";
      tStrings[WL2]                 = "WL2";
      tStrings[WL3]                 = "WL3";
      tStrings[WL4]                 = "WL4";
      tStrings[EWL]                 = "EWL";

      tStrings[Q1]                  = "Q1";
      tStrings[Q2]                  = "Q2";
      tStrings[Q3]                  = "Q3";
      tStrings[Q5]                  = "Q5";
      tStrings[Q6]                  = "Q6";
      tStrings[Q7]                  = "Q7";
      tStrings[Q8]                  = "Q8";
      tStrings[Q9]                  = "Q9";


      tStrings[prefitC1]            = "prefitResidualC1";
      tStrings[prefitC2]            = "prefitResidualC2";
      tStrings[prefitC3]            = "prefitResidualC3";
      tStrings[prefitC5]            = "prefitResidualC5";
      tStrings[prefitC6]            = "prefitResidualC6";
      tStrings[prefitC7]            = "prefitResidualC7";
      tStrings[prefitC8]            = "prefitResidualC8";
      tStrings[prefitC9]            = "prefitResidualC9";
      tStrings[prefitC]             = "prefitResidualC";
      tStrings[prefitCWithoutClock] = "prefitResidualCWithoutClock";
      tStrings[prefitCWithSatClock] = "prefitResidualCWithSatClock";
      tStrings[prefitCWithStaClock] = "prefitResidualCWithStaClock";
      tStrings[prefitCForPOD]       = "prefitResidualCForPOD";


      tStrings[prefitP1]            = "prefitResidualP1";
      tStrings[prefitP2]            = "prefitResidualP2";

      tStrings[prefitL1]            = "prefitResidualL1";
      tStrings[prefitL2]            = "prefitResidualL2";
      tStrings[prefitL3]            = "prefitResidualL3";
      tStrings[prefitL5]            = "prefitResidualL5";
      tStrings[prefitL6]            = "prefitResidualL6";
      tStrings[prefitL7]            = "prefitResidualL7";
      tStrings[prefitL8]            = "prefitResidualL8";
      tStrings[prefitL9]            = "prefitResidualL9";
      tStrings[prefitL]             = "prefitResidualL";
      tStrings[prefitLWithoutClock] = "prefitResidualLWithoutClock";
      tStrings[prefitLWithSatClock] = "prefitResidualLWithSatClock";
      tStrings[prefitLWithStaClock] = "prefitResidualLWithStaClock";
      tStrings[prefitLForPOD]       = "prefitResidualLForPOD";


      tStrings[prefitPC]            = "prefitResidualPC";
      tStrings[prefitLC]            = "prefitResidualLC";
      tStrings[prefitPI]            = "prefitResidualPI";
      tStrings[prefitLI]            = "prefitResidualLI";
      tStrings[prefitPW]            = "prefitResidualPW";
      tStrings[prefitLW]            = "prefitResidualLW";
      tStrings[prefitPN]            = "prefitResidualPN";
      tStrings[prefitLN]            = "prefitResidualLN";
      tStrings[prefitMW]            = "prefitResidualMW";
      tStrings[prefitWL]            = "prefitResidualWL";
      tStrings[prefitWL1]           = "prefitResidualWL1";
      tStrings[prefitWL2]           = "prefitResidualWL2";
      tStrings[prefitWL3]           = "prefitResidualWL3";
      tStrings[prefitWL4]           = "prefitResidualWL4";


      tStrings[postfitC1]           = "postfitResidualC1";
      tStrings[postfitC2]           = "postfitResidualC2";
      tStrings[postfitC3]           = "postfitResidualC3";
      tStrings[postfitC5]           = "postfitResidualC5";
      tStrings[postfitC6]           = "postfitResidualC6";
      tStrings[postfitC7]           = "postfitResidualC7";
      tStrings[postfitC8]           = "postfitResidualC8";
      tStrings[postfitC9]           = "postfitResidualC9";
      tStrings[postfitC]            = "postfitResidualC";

      tStrings[postfitP1]           = "postfitResidualP1";
      tStrings[postfitP2]           = "postfitResidualP2";

      tStrings[postfitL1]           = "postfitResidualL1";
      tStrings[postfitL2]           = "postfitResidualL2";
      tStrings[postfitL3]           = "postfitResidualL3";
      tStrings[postfitL5]           = "postfitResidualL5";
      tStrings[postfitL6]           = "postfitResidualL6";
      tStrings[postfitL7]           = "postfitResidualL7";
      tStrings[postfitL8]           = "postfitResidualL8";
      tStrings[postfitL9]           = "postfitResidualL9";
      tStrings[postfitL]            = "postfitResidualL";

      tStrings[postfitPC]           = "postfitResidualPC";
      tStrings[postfitLC]           = "postfitResidualLC";
      tStrings[postfitPI]           = "postfitResidualPI";
      tStrings[postfitLI]           = "postfitResidualLI";
      tStrings[postfitPW]           = "postfitResidualPW";
      tStrings[postfitLW]           = "postfitResidualLW";
      tStrings[postfitPN]           = "postfitResidualPN";
      tStrings[postfitLN]           = "postfitResidualLN";
      tStrings[postfitMW]           = "postfitResidualMW";
      tStrings[postfitWL]           = "postfitResidualWL";
      tStrings[postfitWL1]          = "postfitResidualWL1";
      tStrings[postfitWL2]          = "postfitResidualWL2";
      tStrings[postfitWL3]          = "postfitResidualWL3";
      tStrings[postfitWL4]          = "postfitResidualWL4";

      tStrings[BL1]                 = "ambiguityL1";
      tStrings[BL2]                 = "ambiguityL2";
      tStrings[BL3]                 = "ambiguityL3";
      tStrings[BL5]                 = "ambiguityL5";
      tStrings[BL6]                 = "ambiguityL6";
      tStrings[BL7]                 = "ambiguityL7";
      tStrings[BL8]                 = "ambiguityL8";
      tStrings[BL9]                 = "ambiguityL9";
      tStrings[BLC]                 = "ambiguityLC";
      tStrings[BWL]                 = "ambiguityWL";
      tStrings[BWL1]                = "ambiguityWL1";
      tStrings[BWL2]                = "ambiguityWL2";
      tStrings[BWL3]                = "ambiguityWL3";
      tStrings[BWL4]                = "ambiguityWL4";

      tStrings[updSatL1]            = "updSatL1";
      tStrings[updSatL2]            = "updSatL2";
      tStrings[updSatL3]            = "updSatL3";
      tStrings[updSatL5]            = "updSatL5";
      tStrings[updSatL6]            = "updSatL6";
      tStrings[updSatL7]            = "updSatL7";
      tStrings[updSatL8]            = "updSatL8";
      tStrings[updSatL9]            = "updSatL9";
      tStrings[updSatLC]            = "updSatLC";
      tStrings[updSatWL]            = "updSatWL";
      tStrings[updSatMW]            = "updSatMW";

      tStrings[instC1]              = "instrumentalC1";
      tStrings[instC2]              = "instrumentalC2";
      tStrings[instC3]              = "instrumentalC3";
      tStrings[instC5]              = "instrumentalC5";
      tStrings[instC6]              = "instrumentalC6";
      tStrings[instC7]              = "instrumentalC7";
      tStrings[instC8]              = "instrumentalC8";
      tStrings[instC9]              = "instrumentalC9";
      tStrings[instPC]              = "instrumentalPC";

      tStrings[instL1]              = "instrumentalL1";
      tStrings[instL2]              = "instrumentalL2";
      tStrings[instL3]              = "instrumentalL3";
      tStrings[instL5]              = "instrumentalL5";
      tStrings[instL6]              = "instrumentalL6";
      tStrings[instL7]              = "instrumentalL7";
      tStrings[instL8]              = "instrumentalL8";
      tStrings[instL9]              = "instrumentalL9";
      tStrings[instLC]              = "instrumentalLC";

      tStrings[ISB_GLO]             = "ISB_GLO";
      tStrings[ISB_GAL]             = "ISB_GAL";
      tStrings[ISB_BDS]             = "ISB_BDS";


      tStrings[rho]                 = "rho";
      tStrings[rhoDot]              = "rhoDot";
      tStrings[rhoDot2]             = "rhoDot2";

      tStrings[relativity]          = "relativity";

      tStrings[gravDelay]           = "gravDelay";

      tStrings[tropo]               = "tropo";
      tStrings[tropoWeight]         = "tropoWeight";
      tStrings[dryTropo]            = "dryTropo";
      tStrings[dryMap]              = "dryTropoMap";
      tStrings[wetTropo]            = "wetTropo";
      tStrings[wetMap]              = "wetTropoMap";
      tStrings[tropoSlant]          = "slantTropo";

      tStrings[iono]                = "verticalIono";
      tStrings[ionoTEC]             = "TotalElectronContent";
      tStrings[ionoMap]             = "ionoMap";
      tStrings[ionoMap2]            = "ionoMap2";
      tStrings[ionoL1]              = "slantIonoL1";
      tStrings[ionoL2]              = "slantIonoL2";
      tStrings[ionoL3]              = "slantIonoL3";
      tStrings[ionoL5]              = "slantIonoL5";
      tStrings[ionoL6]              = "slantIonoL6";
      tStrings[ionoL7]              = "slantIonoL7";
      tStrings[ionoL8]              = "slantIonoL8";
      tStrings[ionoL9]              = "slantIonoL9";
      tStrings[ionoL1Weight]        = "ionoWeightL1";


      tStrings[diffPrefitC1]        = "diffPrefitC1";
      tStrings[diffPrefitC2]        = "diffPrefitC2";
      tStrings[diffPrefitC]         = "diffPrefitC";
      tStrings[diffPrefitL1]        = "diffPrefitL1";
      tStrings[diffPrefitL2]        = "diffPrefitL2";
      tStrings[diffPrefitL]         = "diffPrefitL";
      tStrings[diffWetTropo]        = "diffWetTropo";


      tStrings[windUp]              = "windUp";

      tStrings[eclipse]             = "eclipse";

      tStrings[satPCenter]          = "satPhaseCenter";
      tStrings[satPCenterX]         = "satPhaseCenterX";
      tStrings[satPCenterY]         = "satPhaseCenterY";
      tStrings[satPCenterZ]         = "satPhaseCenterZ";

      tStrings[AntOffU]             = "AntOffU";
      tStrings[AntOffN]             = "AntOffN";
      tStrings[AntOffE]             = "AntOffE";

      tStrings[elevation]           = "elevation";
      tStrings[azimuth]             = "azimuth";

      tStrings[satArc]              = "satArc";

      tStrings[weight]              = "weight";
      tStrings[weightC]             = "weightC";
      tStrings[weightL]             = "weightL";


      tStrings[staX]                = "staX";
      tStrings[staY]                = "staY";
      tStrings[staZ]                = "staZ";
      tStrings[staVX]               = "staVX";
      tStrings[staVY]               = "staVY";
      tStrings[staVZ]               = "staVZ";
      tStrings[staAX]               = "staAX";
      tStrings[staAY]               = "staAY";
      tStrings[staAZ]               = "staAZ";

      tStrings[staLat]              = "staLat";
      tStrings[staLon]              = "staLon";
      tStrings[staH]                = "staH";
      tStrings[staVLat]             = "staVLat";
      tStrings[staVLon]             = "staVLon";
      tStrings[staVH]               = "staVH";
      tStrings[staALat]             = "staALat";
      tStrings[staALon]             = "staALon";
      tStrings[staAH]               = "staAH";

      tStrings[cdtSta]              = "cdtSta";
      tStrings[cdtStaDot]           = "cdtStaDot";
      tStrings[cdtStaDot2]          = "cdtStaDot2";


      tStrings[satX]                = "satX";
      tStrings[satY]                = "satY";
      tStrings[satZ]                = "satZ";
      tStrings[satVX]               = "satVX";
      tStrings[satVY]               = "satVY";
      tStrings[satVZ]               = "satVZ";
      tStrings[satAX]               = "satAX";
      tStrings[satAY]               = "satAY";
      tStrings[satAZ]               = "satAZ";

      tStrings[satXJ2K]             = "satXJ2K";
      tStrings[satYJ2K]             = "satYJ2K";
      tStrings[satZJ2K]             = "satZJ2K";
      tStrings[satVXJ2K]            = "satVXJ2K";
      tStrings[satVYJ2K]            = "satVYJ2K";
      tStrings[satVZJ2K]            = "satVZJ2K";
      tStrings[satAXJ2K]            = "satAXJ2K";
      tStrings[satAYJ2K]            = "satAYJ2K";
      tStrings[satAZJ2K]            = "satAZJ2K";

      tStrings[cdtSat]              = "cdtSat";
      tStrings[cdtSatDot]           = "cdtSatDot";
      tStrings[cdtSatDot2]          = "cdtSatDot2";
      tStrings[cdtSatSum]           = "cdtSatSum";


      tStrings[dStaX]               = "dStaX";
      tStrings[dStaY]               = "dStaY";
      tStrings[dStaZ]               = "dStaZ";
      tStrings[dStaLat]             = "dStaLat";
      tStrings[dStaLon]             = "dStaLon";
      tStrings[dStaH]               = "dStaH";

      tStrings[dcdtSta]             = "dcdtSta";
      tStrings[dcdtStaDot]          = "dcdtStaDot";
      tStrings[dcdtStaDot2]         = "dcdtStaDot2";


      tStrings[dSatX]               = "dSatX";
      tStrings[dSatY]               = "dSatY";
      tStrings[dSatZ]               = "dSatZ";
      tStrings[dSatVX]              = "dSatVX";
      tStrings[dSatVY]              = "dSatVY";
      tStrings[dSatVZ]              = "dSatVZ";

      tStrings[dcdtSat]             = "dcdtSat";
      tStrings[dcdtSatDot]          = "dcdtSatDot";
      tStrings[dcdtSatDot2]         = "dcdtSatDot2";


      tStrings[SRPC1]               = "SRPC1";
      tStrings[SRPC2]               = "SRPC2";
      tStrings[SRPC3]               = "SRPC3";
      tStrings[SRPC4]               = "SRPC4";
      tStrings[SRPC5]               = "SRPC5";
      tStrings[SRPC6]               = "SRPC6";
      tStrings[SRPC7]               = "SRPC7";
      tStrings[SRPC8]               = "SRPC8";
      tStrings[SRPC9]               = "SRPC9";

      tStrings[dSRPC1]              = "dSRPC1";
      tStrings[dSRPC2]              = "dSRPC2";
      tStrings[dSRPC3]              = "dSRPC3";
      tStrings[dSRPC4]              = "dSRPC4";
      tStrings[dSRPC5]              = "dSRPC5";
      tStrings[dSRPC6]              = "dSRPC6";
      tStrings[dSRPC7]              = "dSRPC7";
      tStrings[dSRPC8]              = "dSRPC8";
      tStrings[dSRPC9]              = "dSRPC9";



      tStrings[xpole]               = "xpole";
      tStrings[ypole]               = "ypole";
      tStrings[xpoleRate]           = "xpoleRate";
      tStrings[ypoleRate]           = "ypoleRate";
      tStrings[UT1mUTC]             = "UT1mUTC";
      tStrings[LOD]                 = "LOD";

      tStrings[dxpole]              = "dxpole";
      tStrings[dypole]              = "dypole";
      tStrings[dxpoleRate]          = "dxpoleRate";
      tStrings[dypoleRate]          = "dypoleRate";
      tStrings[dUT1mUTC]            = "dUT1mUTC";
      tStrings[dLOD]                = "dLOD";



      tStrings[dummy0]              = "dummy0";
      tStrings[dummy1]              = "dummy1";
      tStrings[dummy2]              = "dummy2";
      tStrings[dummy3]              = "dummy3";
      tStrings[dummy4]              = "dummy4";
      tStrings[dummy5]              = "dummy5";
      tStrings[dummy6]              = "dummy6";
      tStrings[dummy7]              = "dummy7";
      tStrings[dummy8]              = "dummy8";
      tStrings[dummy9]              = "dummy9";
      tStrings[Last]                = "Last";
      tStrings[Placeholder]         = "Placeholder";
   }


      // Explicit constructor
   TypeID::TypeID(std::string name)
   {
       /// search tStrings
       std::map<ValueType, std::string>::const_iterator tStrings_it;
       for( tStrings_it = tStrings.begin();
            tStrings_it != tStrings.end();
            tStrings_it++ )
       {
           if( tStrings_it->second == name )
           {
               type = tStrings_it->first;
               return;
           }
       }


       /// search mapUserTypeID
       std::map<std::string, TypeID>::const_iterator map_user_it = mapUserTypeID.find(name);
       if( map_user_it != mapUserTypeID.end() )
       {
           type = map_user_it->second.type;
           return;
       }


       /// create new TypeID
       type = newValueType(name);
   }


      // Assignment operator
   TypeID TypeID::operator=(const TypeID& right)
   {
      if ( this == &right ) return (*this);
      (*this).type = right.type;
      return *this;
   }


      // Convenience output method
   std::ostream& TypeID::dump(std::ostream& s) const
   {
      s << TypeID::tStrings[type];

      return s;
   }


      // Returns true if this is a valid TypeID. Basically just
      // checks that the enum is defined
   bool TypeID::isValid() const
   {
      return !(type==Unknown);
   }


      /* Static method to add new TypeID's
       * @param string      Identifying string for the new TypeID
       */
   TypeID::ValueType TypeID::newValueType(const std::string& s)
   {
      ValueType newId =
         static_cast<ValueType>(TypeID::tStrings.rbegin()->first + 1);

      TypeID::tStrings[newId] = s;

      return newId;
   }


   namespace StringUtils
   {

         // convert this object to a string representation
      std::string asString(const TypeID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }

   }  // End of namespace StringUtils


      // stream output for TypeID
   std::ostream& operator<<(std::ostream& s, const TypeID& p)
   {
      p.dump(s);
      return s;
   }


   int GetCarrierBand(const RinexObsID& roi)
   {
       // 1 2 3 5 6 7 8 9

       if(roi.band == ObsID::cb1) return 1;
       if(roi.band == ObsID::cb2) return 2;
       if(roi.band == ObsID::cb3) return 3;
       if(roi.band == ObsID::cb5) return 5;
       if(roi.band == ObsID::cb6) return 6;
       if(roi.band == ObsID::cb7) return 7;
       if(roi.band == ObsID::cb8) return 8;
       if(roi.band == ObsID::cb9) return 9;

       return -1;
   }


   TypeID::ValueType ConvertToTypeID(const RinexObsID& roi,
                                     const RinexSatID& sat)
   {
      if(sat.system == SatID::systemGPS)
      {
          // GPS    L1      1575.42         C1C     L1C     D1C     S1C
          //                                C1S     L1S     D1S     S1S
          //                                C1L     L1L     D1L     S1L
          //                                C1X     L1X     D1X     S1X
          //                                C1P     L1P     D1P     S1P
          //                                C1W     L1W     D1W     S1W
          //                                C1Y     L1Y     D1Y     S1Y
          //                                C1M     L1M     D1M     S1M
          //                                        L1N     D1N     S1N
          //        L2      1227.60         C2C     L2C     D2C     S2C
          //                                C2D     L2D     D2D     S2D
          //                                C2S     L2S     D2S     S2S
          //                                C2L     L2L     D2L     S2L
          //                                C2X     L2X     D2X     S2X
          //                                C2P     L2P     D2P     S2P
          //                                C2W     L2W     D2W     S2W
          //                                C2Y     L2Y     D2Y     S2Y
          //                                C2M     L2M     D2M     S2M
          //                                        L2N     D2N     S2N
          //        L5      1176.45         C5I     L5I     D5I     S5I
          //                                C5Q     L5Q     D5Q     S5Q
          //                                C5X     L5X     D5X     S5X

          if(roi.band==ObsID::cb1)          // L1
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcC)        return TypeID::C1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::C1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::C1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::C1X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::C1P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::C1W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::C1Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::C1M;     // M
              }
              else if(roi.type == ObsID::otL)   // P
              {
                  if(roi.code == ObsID::tcC)        return TypeID::L1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::L1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::L1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::L1X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::L1P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::L1W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::L1Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::L1M;     // M
                  else if(roi.code == ObsID::tcN)   return TypeID::L1N;     // N
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcC)        return TypeID::D1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::D1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::D1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::D1X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::D1P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::D1W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::D1Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::D1M;     // M
                  else if(roi.code == ObsID::tcN)   return TypeID::D1N;     // N
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcC)        return TypeID::S1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::S1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::S1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::S1X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::S1P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::S1W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::S1Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::S1M;     // M
                  else if(roi.code == ObsID::tcN)   return TypeID::S1N;     // N
              }
          }
          else if(roi.band==ObsID::cb2)     // L2
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcC)        return TypeID::C2C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::C2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::C2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::C2X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::C2P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::C2W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::C2Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::C2M;     // M
              }
              else if(roi.type == ObsID::otL)   // P
              {
                  if(roi.code == ObsID::tcC)        return TypeID::L2C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::L2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::L2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::L2X;     // X
                  else if(roi.code == ObsID::tcP)   return TypeID::L2P;     // P
                  else if(roi.code == ObsID::tcW)   return TypeID::L2W;     // W
                  else if(roi.code == ObsID::tcY)   return TypeID::L2Y;     // Y
                  else if(roi.code == ObsID::tcM)   return TypeID::L2M;     // M
                  else if(roi.code == ObsID::tcN)   return TypeID::L2N;     // N
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcC)          return TypeID::D2C;   // C
                  else if(roi.code == ObsID::tcS)     return TypeID::D2S;   // S
                  else if(roi.code == ObsID::tcL)     return TypeID::D2L;   // L
                  else if(roi.code == ObsID::tcX)     return TypeID::D2X;   // X
                  else if(roi.code == ObsID::tcP)     return TypeID::D2P;   // P
                  else if(roi.code == ObsID::tcW)     return TypeID::D2W;   // W
                  else if(roi.code == ObsID::tcY)     return TypeID::D2Y;   // Y
                  else if(roi.code == ObsID::tcM)     return TypeID::D2M;   // M
                  else if(roi.code == ObsID::tcN)     return TypeID::D2N;   // N
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcC)          return TypeID::S2C;   // C
                  else if(roi.code == ObsID::tcS)     return TypeID::S2S;   // S
                  else if(roi.code == ObsID::tcL)     return TypeID::S2L;   // L
                  else if(roi.code == ObsID::tcX)     return TypeID::S2X;   // X
                  else if(roi.code == ObsID::tcP)     return TypeID::S2P;   // P
                  else if(roi.code == ObsID::tcW)     return TypeID::S2W;   // W
                  else if(roi.code == ObsID::tcY)     return TypeID::S2Y;   // Y
                  else if(roi.code == ObsID::tcM)     return TypeID::S2M;   // M
                  else if(roi.code == ObsID::tcN)     return TypeID::S2N;   // N
              }
          }
          else if(roi.band==ObsID::cb5)     // L5
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)         return TypeID::C5I;    // I
                  else if(roi.code == ObsID::tcQ)    return TypeID::C5Q;    // Q
                  else if(roi.code == ObsID::tcX)    return TypeID::C5X;    // X
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcI)         return TypeID::L5I;    // I
                  else if(roi.code == ObsID::tcQ)    return TypeID::L5Q;    // Q
                  else if(roi.code == ObsID::tcX)    return TypeID::L5X;    // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcI)         return TypeID::D5I;    // I
                  else if(roi.code == ObsID::tcQ)    return TypeID::D5Q;    // Q
                  else if(roi.code == ObsID::tcX)    return TypeID::D5X;    // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcI)         return TypeID::S5I;    // I
                  else if(roi.code == ObsID::tcQ)    return TypeID::S5Q;    // Q
                  else if(roi.code == ObsID::tcX)    return TypeID::S5X;    // X
              }
          }
      }
      else if(sat.system == SatID::systemGLONASS)
      {
          // GLONASS    G1      1602+k*9/16     C1C     L1C     D1C     S1C
          //                                    C1P     L1P     D1P     S1P
          //            G2      1246+k*716      C2C     L2C     D2C     S2C
          //                                    C2P     L2P     D2P     S2P
          //            G3      1202.025        C3I     L3I     D3I     S3I
          //                                    C3Q     L3Q     D3Q     S3Q
          //                                    C3X     L3X     D3X     S3X

          if(roi.band == ObsID::cb1)        // G1
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcC)        return TypeID::C1C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::C1P;     // P
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcC)        return TypeID::L1C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::L1P;     // P
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcC)        return TypeID::D1C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::D1P;     // P
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcC)        return TypeID::S1C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::S1P;     // P
              }
          }
          if(roi.band == ObsID::cb2)        // G2
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcC)        return TypeID::C2C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::C2P;     // P
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcC)        return TypeID::L2C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::L2P;     // P
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcC)        return TypeID::D2C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::D2P;     // P
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcC)        return TypeID::S2C;     // C
                  else if(roi.code == ObsID::tcP)   return TypeID::S2P;     // P
              }
          }
          if(roi.band == ObsID::cb3)        // G3
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C3I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C3Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C3X;     // X
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L3I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L3Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L3X;     // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D3I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D3Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D3X;     // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S3I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S3Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S3X;     // X
              }
          }
      }
      else if(sat.system == SatID::systemGalileo)
      {
          // Galileo    E1      1575.42         C1A     L1A     D1A     S1A
          //                                    C1B     L1B     D1B     S1B
          //                                    C1C     L1C     D1C     S1C
          //                                    C1X     L1X     D1X     S1X
          //                                    C1Z     L1Z     D1Z     S1Z
          //            E5a     1176.45         C5I     L5I     D5I     S5I
          //                                    C5Q     L5Q     D5Q     S5Q
          //                                    C5X     L5X     D5X     S5X
          //            E5b     1207.140        C7I     L7I     D7I     S7I
          //                                    C7Q     L7Q     D7Q     S7Q
          //                                    C7X     L7X     D7X     S7X
          //            E5      1191.795        C8I     L8I     D8I     S8I
          //                                    C8Q     L8Q     D8Q     S8Q
          //                                    C8X     L8X     D8X     S8X
          //            E6      1278.75         C6A     L6A     D6A     S6A
          //                                    C6B     L6B     D6B     S6B
          //                                    C6C     L6C     D6C     S6C
          //                                    C6X     L6X     D6X     S6X
          //                                    C6Z     L6Z     D6Z     S6Z

          if(roi.band == ObsID::cb1)        // E1
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcA)        return TypeID::C1A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::C1B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::C1C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::C1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::C1Z;     // Z

              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcA)        return TypeID::L1A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::L1B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::L1C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::L1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::L1Z;     // Z
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcA)        return TypeID::D1A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::D1B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::D1C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::D1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::D1Z;     // Z
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcA)        return TypeID::S1A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::S1B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::S1C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::S1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::S1Z;     // Z
              }
          }
          else if(roi.band == ObsID::cb5)   // E5a
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C5X;     // X

              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L5X;     // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D5X;     // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S5X;     // X
              }
          }
          else if(roi.band == ObsID::cb7)   // E5b
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C7I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C7Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C7X;     // X

              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L7I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L7Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L7X;     // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D7I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D7Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D7X;     // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S7I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S7Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S7X;     // X
              }
          }
          else if(roi.band == ObsID::cb8)   // E5
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C8I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C8Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C8X;     // X

              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L8I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L8Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L8X;     // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D8I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D8Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D8X;     // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S8I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S8Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S8X;     // X
              }
          }
          else if(roi.band == ObsID::cb6)   // E6
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcA)        return TypeID::C6A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::C6B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::C6C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::C6X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::C6Z;     // Z

              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcA)        return TypeID::L6A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::L6B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::L6C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::L6X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::L6Z;     // Z
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcA)        return TypeID::D6A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::D6B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::D6C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::D6X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::D6Z;     // Z
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcA)        return TypeID::S6A;     // A
                  else if(roi.code == ObsID::tcB)   return TypeID::S6B;     // B
                  else if(roi.code == ObsID::tcC)   return TypeID::S6C;     // C
                  else if(roi.code == ObsID::tcX)   return TypeID::S6X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::S6Z;     // Z
              }
          }
      }
      else if(sat.system == SatID::systemSBAS)
      {
          // SBAS   L1      1575.42         C1C     L1C     D1C     S1C
          //        L5      1176.42         C5I     L5I     D5I     S5I
          //                                C5Q     L5Q     D5Q     S5Q
          //                                C5X     L5X     D5X     S5X

          if(roi.band == ObsID::cb1)        // L1
          {
              if(roi.type == ObsID::otC)        // C
                  return TypeID::C1C;
              else if(roi.type == ObsID::otL)   // L
                  return TypeID::L1C;
              else if(roi.type == ObsID::otD)   // D
                  return TypeID::D1C;
              else if(roi.type == ObsID::otS)   // S
                  return TypeID::S1C;
          }
          else if(roi.band == ObsID::cb5)   // L5
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C5X;     // X
              }
              if(roi.type == ObsID::otL)        // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L5X;     // X
              }
              if(roi.type == ObsID::otD)        // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D5X;     // X
              }
              if(roi.type == ObsID::otS)        // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S5X;     // X
              }
          }
      }
      else if(sat.system == SatID::systemQZSS)
      {
          // QZSS   L1      1575.42         C1C     L1C     D1C     S1C
          //                                C1S     L1S     D1S     S1S
          //                                C1L     L1L     D1L     S1L
          //                                C1X     L1X     D1X     S1X
          //                                C1Z     L1Z     D1Z     S1Z
          //        L2      1227.60         C2S     L2S     D2S     S2S
          //                                C2L     L2L     D2L     S2L
          //                                C2X     L2X     D2X     S2X
          //        L5      1176.45         C5I     L5I     D5I     S5I
          //                                C5Q     L5Q     D5Q     S5Q
          //                                C5X     L5X     D5X     S5X
          //        L6      1278.75         C6L     L6L     D6L     S6L
          //                                C6X     L6X     D6X     S6X

          if(roi.band == ObsID::cb1)        // L1
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcC)        return TypeID::C1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::C1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::C1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::C1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::C1Z;     // Z
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcC)        return TypeID::L1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::L1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::L1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::L1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::L1Z;     // Z
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcC)        return TypeID::D1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::D1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::D1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::D1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::D1Z;     // Z
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcC)        return TypeID::S1C;     // C
                  else if(roi.code == ObsID::tcS)   return TypeID::S1S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::S1L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::S1X;     // X
                  else if(roi.code == ObsID::tcZ)   return TypeID::S1Z;     // Z
              }
          }
          else if(roi.band == ObsID::cb2)   // L2
          {

              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcS)        return TypeID::C2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::C2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::C2X;     // X
              }
              else if(roi.type == ObsID::otL)   // L
              {
                  if(roi.code == ObsID::tcS)        return TypeID::L2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::L2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::L2X;     // X
              }
              else if(roi.type == ObsID::otD)   // D
              {
                  if(roi.code == ObsID::tcS)        return TypeID::D2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::D2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::D2X;     // X
              }
              else if(roi.type == ObsID::otS)   // S
              {
                  if(roi.code == ObsID::tcS)        return TypeID::S2S;     // S
                  else if(roi.code == ObsID::tcL)   return TypeID::S2L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::S2X;     // X
              }
          }
          else if(roi.band == ObsID::cb5)   // L5
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcI)        return TypeID::C5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::C5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::C5X;     // X
              }
              if(roi.type == ObsID::otL)        // L
              {
                  if(roi.code == ObsID::tcI)        return TypeID::L5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::L5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::L5X;     // X
              }
              if(roi.type == ObsID::otD)        // D
              {
                  if(roi.code == ObsID::tcI)        return TypeID::D5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::D5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::D5X;     // X
              }
              if(roi.type == ObsID::otS)        // S
              {
                  if(roi.code == ObsID::tcI)        return TypeID::S5I;     // I
                  else if(roi.code == ObsID::tcQ)   return TypeID::S5Q;     // Q
                  else if(roi.code == ObsID::tcX)   return TypeID::S5X;     // X
              }
          }
          else if(roi.band == ObsID::cb6)   // L6
          {
              if(roi.type == ObsID::otC)        // C
              {
                  if(roi.code == ObsID::tcL)        return TypeID::C6L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::C6X;     // X
              }
              if(roi.type == ObsID::otL)        // L
              {
                  if(roi.code == ObsID::tcL)        return TypeID::L6L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::L6X;     // X
              }
              if(roi.type == ObsID::otD)        // D
              {
                  if(roi.code == ObsID::tcL)        return TypeID::D6L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::D6X;     // X
              }
              if(roi.type == ObsID::otS)        // S
              {
                  if(roi.code == ObsID::tcL)        return TypeID::S6L;     // L
                  else if(roi.code == ObsID::tcX)   return TypeID::S6X;     // X
              }
          }
      }
      else if(sat.system == SatID::systemBDS)
      {
         // BDS     B1      1561.098        C2I     L2I     D2I     S2I
         //                                 C2Q     L2Q     D2Q     S2Q
         //                                 C2X     L2X     D2X     S2X
         //         B2      1207.14         C7I     L7I     D7I     S7I
         //                                 C7Q     L7Q     D7Q     S7Q
         //                                 C7X     L7X     D7X     S7X
         //         B3      1268.52         C6I     L6I     D6I     S6I
         //                                 C6Q     L6Q     D6Q     S6Q
         //                                 C6X     L6X     D6X     S6X

         if(roi.band == ObsID::cb2)         // B1
         {
             if(roi.type == ObsID::otC)         // C
             {
                 if(roi.code == ObsID::tcI)         return TypeID::C2I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::C2Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::C2X;     // X
             }
             else if(roi.type == ObsID::otL)    // L
             {
                 if(roi.code == ObsID::tcI)         return TypeID::L2I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::L2Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::L2X;     // X
             }
             else if(roi.type == ObsID::otD)    // D
             {
                 if(roi.code == ObsID::tcI)         return TypeID::D2I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::D2Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::D2X;     // X
             }
             else if(roi.type == ObsID::otS)    // S
             {
                 if(roi.code == ObsID::tcI)         return TypeID::S2I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::S2Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::S2X;     // X
             }
         }
         else if(roi.band == ObsID::cb7)    // B2
         {
             if(roi.type == ObsID::otC)         // C
             {
                 if(roi.code == ObsID::tcI)         return TypeID::C7I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::C7Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::C7X;     // X
             }
             else if(roi.type == ObsID::otL)    // L
             {
                 if(roi.code == ObsID::tcI)         return TypeID::L7I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::L7Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::L7X;     // X
             }
             else if(roi.type == ObsID::otD)    // D
             {
                 if(roi.code == ObsID::tcI)         return TypeID::D7I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::D7Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::D7X;     // X
             }
             else if(roi.type == ObsID::otS)    // S
             {
                 if(roi.code == ObsID::tcI)         return TypeID::S7I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::S7Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::S7X;     // X
             }
         }
         else if(roi.band == ObsID::cb6)    // B3
         {
             if(roi.type == ObsID::otC)         // C
             {
                 if(roi.code == ObsID::tcI)         return TypeID::C6I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::C6Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::C6X;     // X
             }
             else if(roi.type == ObsID::otL)    // L
             {
                 if(roi.code == ObsID::tcI)         return TypeID::L6I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::L6Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::L6X;     // X
             }
             else if(roi.type == ObsID::otD)    // D
             {
                 if(roi.code == ObsID::tcI)         return TypeID::D6I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::D6Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::D6X;     // X
             }
             else if(roi.type == ObsID::otS)    // S
             {
                 if(roi.code == ObsID::tcI)         return TypeID::S6I;     // I
                 else if(roi.code == ObsID::tcQ)    return TypeID::S6Q;     // Q
                 else if(roi.code == ObsID::tcX)    return TypeID::S6X;     // X
             }
         }
      }
      else if(sat.system == SatID::systemIRNSS)
      {
         // IRNSS   L5      1176.45         C5A     L5A     D5A     S5A
         //                                 C5B     L5B     D5B     S5B
         //                                 C5C     L5C     D5C     S5C
         //                                 C5X     L5X     D5X     S5X
         //         S       2492.028        C9A     L9A     D9A     S9A
         //                                 C9B     L9B     D9B     S9B
         //                                 C9C     L9C     D9C     S9C
         //                                 C9X     L9X     D9X     S9X

         if(roi.band == ObsID::cb5)         // L5
         {
             if(roi.type == ObsID::otC)         // C
             {
                 if(roi.code == ObsID::tcA)         return TypeID::C5A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::C5B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::C5C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::C5X;     // X
             }
             else if(roi.type == ObsID::otL)    // L
             {
                 if(roi.code == ObsID::tcA)         return TypeID::L5A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::L5B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::L5C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::L5X;     // X
             }
             else if(roi.type == ObsID::otD)    // D
             {
                 if(roi.code == ObsID::tcA)         return TypeID::D5A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::D5B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::D5C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::D5X;     // X
             }
             else if(roi.type == ObsID::otS)    // S
             {
                 if(roi.code == ObsID::tcA)         return TypeID::S5A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::S5B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::S5C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::S5X;     // X
             }
         }
         else if(roi.band == ObsID::cb9)    // S
         {
             if(roi.type == ObsID::otC)         // C
             {
                 if(roi.code == ObsID::tcA)         return TypeID::C9A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::C9B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::C9C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::C9X;     // X
             }
             else if(roi.type == ObsID::otL)    // L
             {
                 if(roi.code == ObsID::tcA)         return TypeID::L9A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::L9B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::L9C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::L9X;     // X
             }
             else if(roi.type == ObsID::otD)    // D
             {
                 if(roi.code == ObsID::tcA)         return TypeID::D9A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::D9B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::D9C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::D9X;     // X
             }
             else if(roi.type == ObsID::otS)    // S
             {
                 if(roi.code == ObsID::tcA)         return TypeID::S9A;     // A
                 else if(roi.code == ObsID::tcB)    return TypeID::S9B;     // B
                 else if(roi.code == ObsID::tcC)    return TypeID::S9C;     // C
                 else if(roi.code == ObsID::tcX)    return TypeID::S9X;     // X
             }
         }

      }

      return TypeID::Unknown;
   }


   TypeID::ValueType ConvertToTypeID(std::string& str)
   {
      if(str == "C1C") return TypeID::C1C;
      if(str == "C1S") return TypeID::C1S;
      if(str == "C1L") return TypeID::C1L;
      if(str == "C1X") return TypeID::C1X;
      if(str == "C1P") return TypeID::C1P;
      if(str == "C1W") return TypeID::C1W;
      if(str == "C1Y") return TypeID::C1Y;
      if(str == "C1M") return TypeID::C1M;
      if(str == "C1A") return TypeID::C1A;
      if(str == "C1B") return TypeID::C1B;
      if(str == "C1Z") return TypeID::C1Z;

      if(str == "L1C") return TypeID::L1C;
      if(str == "L1S") return TypeID::L1S;
      if(str == "L1L") return TypeID::L1L;
      if(str == "L1X") return TypeID::L1X;
      if(str == "L1P") return TypeID::L1P;
      if(str == "L1W") return TypeID::L1W;
      if(str == "L1Y") return TypeID::L1Y;
      if(str == "L1M") return TypeID::L1M;
      if(str == "L1N") return TypeID::L1N;
      if(str == "L1A") return TypeID::L1A;
      if(str == "L1B") return TypeID::L1B;
      if(str == "L1Z") return TypeID::L1Z;

      if(str == "D1C") return TypeID::D1C;
      if(str == "D1S") return TypeID::D1S;
      if(str == "D1L") return TypeID::D1L;
      if(str == "D1X") return TypeID::D1X;
      if(str == "D1P") return TypeID::D1P;
      if(str == "D1W") return TypeID::D1W;
      if(str == "D1Y") return TypeID::D1Y;
      if(str == "D1M") return TypeID::D1M;
      if(str == "D1N") return TypeID::D1N;
      if(str == "D1A") return TypeID::D1A;
      if(str == "D1B") return TypeID::D1B;
      if(str == "D1Z") return TypeID::D1Z;

      if(str == "S1C") return TypeID::S1C;
      if(str == "S1S") return TypeID::S1S;
      if(str == "S1L") return TypeID::S1L;
      if(str == "S1X") return TypeID::S1X;
      if(str == "S1P") return TypeID::S1P;
      if(str == "S1W") return TypeID::S1W;
      if(str == "S1Y") return TypeID::S1Y;
      if(str == "S1M") return TypeID::S1M;
      if(str == "S1N") return TypeID::S1N;
      if(str == "S1A") return TypeID::S1A;
      if(str == "S1B") return TypeID::S1B;
      if(str == "S1Z") return TypeID::S1Z;



      if(str == "C2C") return TypeID::C2C;
      if(str == "C2D") return TypeID::C2D;
      if(str == "C2S") return TypeID::C2S;
      if(str == "C2L") return TypeID::C2L;
      if(str == "C2X") return TypeID::C2X;
      if(str == "C2P") return TypeID::C2P;
      if(str == "C2W") return TypeID::C2W;
      if(str == "C2Y") return TypeID::C2Y;
      if(str == "C2M") return TypeID::C2M;
      if(str == "C2I") return TypeID::C2I;
      if(str == "C2Q") return TypeID::C2Q;

      if(str == "L2C") return TypeID::L2C;
      if(str == "L2D") return TypeID::L2D;
      if(str == "L2S") return TypeID::L2S;
      if(str == "L2L") return TypeID::L2L;
      if(str == "L2X") return TypeID::L2X;
      if(str == "L2P") return TypeID::L2P;
      if(str == "L2W") return TypeID::L2W;
      if(str == "L2Y") return TypeID::L2Y;
      if(str == "L2M") return TypeID::L2M;
      if(str == "L2N") return TypeID::L2N;
      if(str == "L2I") return TypeID::L2I;
      if(str == "L2Q") return TypeID::L2Q;

      if(str == "D2C") return TypeID::D2C;
      if(str == "D2D") return TypeID::D2D;
      if(str == "D2S") return TypeID::D2S;
      if(str == "D2L") return TypeID::D2L;
      if(str == "D2X") return TypeID::D2X;
      if(str == "D2P") return TypeID::D2P;
      if(str == "D2W") return TypeID::D2W;
      if(str == "D2Y") return TypeID::D2Y;
      if(str == "D2M") return TypeID::D2M;
      if(str == "D2N") return TypeID::D2N;
      if(str == "D2I") return TypeID::D2I;
      if(str == "D2Q") return TypeID::D2Q;

      if(str == "S2C") return TypeID::S2C;
      if(str == "S2D") return TypeID::S2D;
      if(str == "S2S") return TypeID::S2S;
      if(str == "S2L") return TypeID::S2L;
      if(str == "S2X") return TypeID::S2X;
      if(str == "S2P") return TypeID::S2P;
      if(str == "S2W") return TypeID::S2W;
      if(str == "S2Y") return TypeID::S2Y;
      if(str == "S2M") return TypeID::S2M;
      if(str == "S2N") return TypeID::S2N;
      if(str == "S2I") return TypeID::S2I;
      if(str == "S2Q") return TypeID::S2Q;


      if(str == "C3I") return TypeID::C3I;
      if(str == "C3Q") return TypeID::C3Q;
      if(str == "C3X") return TypeID::C3X;

      if(str == "L3I") return TypeID::L3I;
      if(str == "L3Q") return TypeID::L3Q;
      if(str == "L3X") return TypeID::L3X;

      if(str == "D3I") return TypeID::D3I;
      if(str == "D3Q") return TypeID::D3Q;
      if(str == "D3X") return TypeID::D3X;

      if(str == "S3I") return TypeID::S3I;
      if(str == "S3Q") return TypeID::S3Q;
      if(str == "S3X") return TypeID::S3X;


      if(str == "C5I") return TypeID::C5I;
      if(str == "C5Q") return TypeID::C5Q;
      if(str == "C5X") return TypeID::C5X;
      if(str == "C5A") return TypeID::C5A;
      if(str == "C5B") return TypeID::C5B;
      if(str == "C5C") return TypeID::C5C;

      if(str == "L5I") return TypeID::L5I;
      if(str == "L5Q") return TypeID::L5Q;
      if(str == "L5X") return TypeID::L5X;
      if(str == "L5A") return TypeID::L5A;
      if(str == "L5B") return TypeID::L5B;
      if(str == "L5C") return TypeID::L5C;

      if(str == "D5I") return TypeID::D5I;
      if(str == "D5Q") return TypeID::D5Q;
      if(str == "D5X") return TypeID::D5X;
      if(str == "D5A") return TypeID::D5A;
      if(str == "D5B") return TypeID::D5B;
      if(str == "D5C") return TypeID::D5C;

      if(str == "S5I") return TypeID::S5I;
      if(str == "S5Q") return TypeID::S5Q;
      if(str == "S5X") return TypeID::S5X;
      if(str == "S5A") return TypeID::S5A;
      if(str == "S5B") return TypeID::S5B;
      if(str == "S5C") return TypeID::S5C;


      if(str == "C6A") return TypeID::C6A;
      if(str == "C6B") return TypeID::C6B;
      if(str == "C6C") return TypeID::C6C;
      if(str == "C6X") return TypeID::C6X;
      if(str == "C6Z") return TypeID::C6Z;
      if(str == "C6S") return TypeID::C6S;
      if(str == "C6L") return TypeID::C6L;
      if(str == "C6I") return TypeID::C6I;
      if(str == "C6Q") return TypeID::C6Q;

      if(str == "L6A") return TypeID::L6A;
      if(str == "L6B") return TypeID::L6B;
      if(str == "L6C") return TypeID::L6C;
      if(str == "L6X") return TypeID::L6X;
      if(str == "L6Z") return TypeID::L6Z;
      if(str == "L6S") return TypeID::L6S;
      if(str == "L6L") return TypeID::L6L;
      if(str == "L6I") return TypeID::L6I;
      if(str == "L6Q") return TypeID::L6Q;

      if(str == "D6A") return TypeID::D6A;
      if(str == "D6B") return TypeID::D6B;
      if(str == "D6C") return TypeID::D6C;
      if(str == "D6X") return TypeID::D6X;
      if(str == "D6Z") return TypeID::D6Z;
      if(str == "D6S") return TypeID::D6S;
      if(str == "D6L") return TypeID::D6L;
      if(str == "D6I") return TypeID::D6I;
      if(str == "D6Q") return TypeID::D6Q;

      if(str == "S6A") return TypeID::S6A;
      if(str == "S6B") return TypeID::S6B;
      if(str == "S6C") return TypeID::S6C;
      if(str == "S6X") return TypeID::S6X;
      if(str == "S6Z") return TypeID::S6Z;
      if(str == "S6S") return TypeID::S6S;
      if(str == "S6L") return TypeID::S6L;
      if(str == "S6I") return TypeID::S6I;
      if(str == "S6Q") return TypeID::S6Q;


      if(str == "C7I") return TypeID::C7I;
      if(str == "C7Q") return TypeID::C7Q;
      if(str == "C7X") return TypeID::C7X;

      if(str == "L7I") return TypeID::L7I;
      if(str == "L7Q") return TypeID::L7Q;
      if(str == "L7X") return TypeID::L7X;

      if(str == "D7I") return TypeID::D7I;
      if(str == "D7Q") return TypeID::D7Q;
      if(str == "D7X") return TypeID::D7X;

      if(str == "S7I") return TypeID::S7I;
      if(str == "S7Q") return TypeID::S7Q;
      if(str == "S7X") return TypeID::S7X;


      if(str == "C8I") return TypeID::C8I;
      if(str == "C8Q") return TypeID::C8Q;
      if(str == "C8X") return TypeID::C8X;

      if(str == "L8I") return TypeID::L8I;
      if(str == "L8Q") return TypeID::L8Q;
      if(str == "L8X") return TypeID::L8X;

      if(str == "D8I") return TypeID::D8I;
      if(str == "D8Q") return TypeID::D8Q;
      if(str == "D8X") return TypeID::D8X;

      if(str == "S8I") return TypeID::S8I;
      if(str == "S8Q") return TypeID::S8Q;
      if(str == "S8X") return TypeID::S8X;


      if(str == "C9A") return TypeID::C9A;
      if(str == "C9B") return TypeID::C9B;
      if(str == "C9C") return TypeID::C9C;
      if(str == "C9X") return TypeID::C9X;

      if(str == "L9A") return TypeID::L9A;
      if(str == "L9B") return TypeID::L9B;
      if(str == "L9C") return TypeID::L9C;
      if(str == "L9X") return TypeID::L9X;

      if(str == "D9A") return TypeID::D9A;
      if(str == "D9B") return TypeID::D9B;
      if(str == "D9C") return TypeID::D9C;
      if(str == "D9X") return TypeID::D9X;

      if(str == "S9A") return TypeID::S9A;
      if(str == "S9B") return TypeID::S9B;
      if(str == "S9C") return TypeID::S9C;
      if(str == "S9X") return TypeID::S9X;


      if(str == "LLI1") return TypeID::LLI1;
      if(str == "LLI2") return TypeID::LLI2;
      if(str == "LLI3") return TypeID::LLI3;
      if(str == "LLI5") return TypeID::LLI5;
      if(str == "LLI6") return TypeID::LLI6;
      if(str == "LLI7") return TypeID::LLI7;
      if(str == "LLI8") return TypeID::LLI8;
      if(str == "LLI9") return TypeID::LLI9;


      if(str == "SSI1") return TypeID::SSI1;
      if(str == "SSI2") return TypeID::SSI2;
      if(str == "SSI3") return TypeID::SSI3;
      if(str == "SSI5") return TypeID::SSI5;
      if(str == "SSI6") return TypeID::SSI6;
      if(str == "SSI7") return TypeID::SSI7;
      if(str == "SSI8") return TypeID::SSI8;
      if(str == "SSI9") return TypeID::SSI9;


      if(str == "C1") return TypeID::C1;
      if(str == "P1") return TypeID::P1;
      if(str == "L1") return TypeID::L1;
      if(str == "D1") return TypeID::D1;
      if(str == "S1") return TypeID::S1;

      if(str == "C2") return TypeID::C2;
      if(str == "P2") return TypeID::P2;
      if(str == "L2") return TypeID::L2;
      if(str == "D2") return TypeID::D2;
      if(str == "S2") return TypeID::S2;

//      if(str == "C3") return TypeID::C3;
//      if(str == "L3") return TypeID::L3;
//      if(str == "D3") return TypeID::D3;
//      if(str == "S3") return TypeID::S3;

      if(str == "C5") return TypeID::C5;
      if(str == "L5") return TypeID::L5;
      if(str == "D5") return TypeID::D5;
      if(str == "S5") return TypeID::S5;

      if(str == "C6") return TypeID::C6;
      if(str == "L6") return TypeID::L6;
      if(str == "D6") return TypeID::D6;
      if(str == "S6") return TypeID::S6;

      if(str == "C7") return TypeID::C7;
      if(str == "L7") return TypeID::L7;
      if(str == "D7") return TypeID::D7;
      if(str == "S7") return TypeID::S7;

      if(str == "C8") return TypeID::C8;
      if(str == "L8") return TypeID::L8;
      if(str == "D8") return TypeID::D8;
      if(str == "S8") return TypeID::S8;

//      if(str == "C9") return TypeID::C9;
//      if(str == "L9") return TypeID::L9;
//      if(str == "D9") return TypeID::D9;
//      if(str == "S9") return TypeID::S9;


      return TypeID::Unknown;

   }



      // Return the new TypeID
   TypeID TypeID::regByName(std::string name,std::string desc)
   {

      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);

      if(it != mapUserTypeID.end())
      {
         return it->second;
      }
      else
      {
         TypeID newID = TypeID::newValueType(desc);

         mapUserTypeID.insert(std::pair<std::string,TypeID>(name, newID));

         return newID;
      }

   }  // End of 'TypeID::registerTypeID(std::string name,std::string desc)'



      // unregister a TypeID by it's name string
   void TypeID::unregByName(std::string name)
   {
      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);

      if(it!=mapUserTypeID.end())
      {
         TypeID delID = it->second;

         std::map<TypeID::ValueType,std::string>::iterator it2 = TypeID::tStrings.find(delID.type);
         if(it2!=TypeID::tStrings.end())
         {
            TypeID::tStrings.erase(it2);
         }

         mapUserTypeID.erase(it);
      }
      else
      {
         // the TypeID have not been registered
         // we do nothing
      }

   } // End of 'TypeID::unregisterTypeID(std::string name)'



      // unregister all TypeIDs registered by name string
   void TypeID::unregAll()
   {
      std::map<std::string,TypeID>::iterator it = mapUserTypeID.begin();

      for(it=mapUserTypeID.begin(); it!=mapUserTypeID.end(); it++)
      {
         TypeID delID = it->second;

         std::map<TypeID::ValueType,std::string>::iterator it2 = TypeID::tStrings.find(delID.type);
         if(it2!=TypeID::tStrings.end())
         {
            TypeID::tStrings.erase(it2);
         }
      }
      mapUserTypeID.clear();

      bUserTypeIDRegistered = false;

   }  // End of 'TypeID::unregisterAll()'

      // get the user registered TypeID by name string
   TypeID TypeID::byName(std::string name)
      throw(InvalidRequest)
   {
      // registerMyTypeID();

      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);
      if(it != mapUserTypeID.end())
      {
         return it->second;
      }
      else
      {
         InvalidRequest e("There are no registered TypeID name as '"
            + name + "'.");
         GPSTK_THROW(e);
      }
   } // End of 'TypeID TypeID::byName(std::string name)'

} // End of namespace gpstk
