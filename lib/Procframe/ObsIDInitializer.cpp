/// @file ObsIDInitializer.cpp This class exists to initialize maps for ObsID.
/// It was initally an inner class of ObsID but has been exported
/// so it can be wrapped by SWIG, as SWIG does not support
/// C++ inner classes as of summer 2013.

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S.
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software.
//
//Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#include "ObsIDInitializer.hpp"

namespace gpstk {

   ObsIDInitializer::ObsIDInitializer()
   {
      ObsID::otDesc[ObsID::otUnknown]   = "UnknownType";    //Rinex (sp)
      ObsID::otDesc[ObsID::otAny]       = "AnyType";        //Rinex *
      ObsID::otDesc[ObsID::otC]         = "code";           //Rinex C
      ObsID::otDesc[ObsID::otL]         = "phase";          //Rinex L
      ObsID::otDesc[ObsID::otD]         = "doppler";        //Rinex D
      ObsID::otDesc[ObsID::otS]         = "SNR";            //Rinex S
      ObsID::otDesc[ObsID::otI]         = "iono";           //Rinex I
      ObsID::otDesc[ObsID::otX]         = "channel";        //Rinex X
      ObsID::otDesc[ObsID::otUndefined] = "undefined";      //Rinex -

      ObsID::cbDesc[ObsID::cbUnknown]   = "UnknownBand";    //Rinex (sp)
      ObsID::cbDesc[ObsID::cbAny]       = "AnyBand";        //Rinex *
      ObsID::cbDesc[ObsID::cb1]         = "1";              //Rinex 1
      ObsID::cbDesc[ObsID::cb2]         = "2";              //Rinex 2
      ObsID::cbDesc[ObsID::cb3]         = "3";              //Rinex 3
      ObsID::cbDesc[ObsID::cb5]         = "5";              //Rinex 5
      ObsID::cbDesc[ObsID::cb6]         = "6";              //Rinex 6
      ObsID::cbDesc[ObsID::cb7]         = "7";              //Rinex 7
      ObsID::cbDesc[ObsID::cb8]         = "8";              //Rinex 8
      ObsID::cbDesc[ObsID::cb9]         = "9";              //Rinex 9
      ObsID::cbDesc[ObsID::cb0]         = "0";              //Rinex 0
      ObsID::cbDesc[ObsID::cbUndefined] = "undefined";      //Rinex -

      ObsID::tcDesc[ObsID::tcUnknown]   = "UnknownCode";    //Rinex (sp)
      ObsID::tcDesc[ObsID::tcAny]       = "AnyCode";        //Rinex *
      ObsID::tcDesc[ObsID::tcP]         = "P";              //Rinex P
      ObsID::tcDesc[ObsID::tcC]         = "C";              //Rinex C
      ObsID::tcDesc[ObsID::tcD]         = "D";              //Rinex D
      ObsID::tcDesc[ObsID::tcY]         = "Y";              //Rinex Y
      ObsID::tcDesc[ObsID::tcM]         = "M";              //Rinex M
      ObsID::tcDesc[ObsID::tcN]         = "N";              //Rinex N
      ObsID::tcDesc[ObsID::tcA]         = "A";              //Rine A
      ObsID::tcDesc[ObsID::tcB]         = "B";              //Rinex B
      ObsID::tcDesc[ObsID::tcI]         = "I";              //Rinex I
      ObsID::tcDesc[ObsID::tcQ]         = "Q";              //Rinex Q
      ObsID::tcDesc[ObsID::tcS]         = "S";              //Rinex S
      ObsID::tcDesc[ObsID::tcL]         = "L";              //Rinex L
      ObsID::tcDesc[ObsID::tcX]         = "X";              //Rinex X
      ObsID::tcDesc[ObsID::tcW]         = "W";              //Rinex W
      ObsID::tcDesc[ObsID::tcZ]         = "Z";              //Rinex Z
      ObsID::tcDesc[ObsID::tcUndefined] = "undefined";      //Rinex -

      if (ObsID::otDesc.size() != (int)ObsID::otLast)
         std::cerr << "Error in otDesc" << std::endl;
      if (ObsID::cbDesc.size() != (int)ObsID::cbLast)
         std::cerr << "Error in cbDesc" << std::endl;
      if (ObsID::tcDesc.size() != (int)ObsID::tcLast)
         std::cerr << "Error in tcDesc" << std::endl;

      // The following definitions really should only describe the items that are
      // in the Rinex 3 specification. If an application needs additional ObsID
      // types to be able to be translated to/from Rinex3, the additional types
      // must be added by the application.
      ObsID::char2ot[' '] = ObsID::otUnknown;
      ObsID::char2ot['*'] = ObsID::otAny;
      ObsID::char2ot['C'] = ObsID::otC;
      ObsID::char2ot['L'] = ObsID::otL;
      ObsID::char2ot['D'] = ObsID::otD;
      ObsID::char2ot['S'] = ObsID::otS;
      ObsID::char2ot['I'] = ObsID::otI;
      ObsID::char2ot['X'] = ObsID::otX;
      ObsID::char2ot['-'] = ObsID::otUndefined;

      ObsID::char2cb[' '] = ObsID::cbUnknown;
      ObsID::char2cb['*'] = ObsID::cbAny;
      ObsID::char2cb['1'] = ObsID::cb1;
      ObsID::char2cb['2'] = ObsID::cb2;
      ObsID::char2cb['3'] = ObsID::cb3;
      ObsID::char2cb['5'] = ObsID::cb5;
      ObsID::char2cb['6'] = ObsID::cb6;
      ObsID::char2cb['7'] = ObsID::cb7;
      ObsID::char2cb['8'] = ObsID::cb8;
      ObsID::char2cb['9'] = ObsID::cb9;
      ObsID::char2cb['0'] = ObsID::cb0;
      ObsID::char2cb['-'] = ObsID::cbUndefined;

      ObsID::char2tc[' '] = ObsID::tcUnknown;
      ObsID::char2tc['*'] = ObsID::tcAny;
      ObsID::char2tc['P'] = ObsID::tcP;
      ObsID::char2tc['C'] = ObsID::tcC;
      ObsID::char2tc['D'] = ObsID::tcD;
      ObsID::char2tc['Y'] = ObsID::tcY;
      ObsID::char2tc['M'] = ObsID::tcM;
      ObsID::char2tc['N'] = ObsID::tcN;
      ObsID::char2tc['A'] = ObsID::tcA;
      ObsID::char2tc['B'] = ObsID::tcB;
      ObsID::char2tc['I'] = ObsID::tcI;
      ObsID::char2tc['Q'] = ObsID::tcQ;
      ObsID::char2tc['S'] = ObsID::tcS;
      ObsID::char2tc['L'] = ObsID::tcL;
      ObsID::char2tc['X'] = ObsID::tcX;
      ObsID::char2tc['W'] = ObsID::tcW;
      ObsID::char2tc['Z'] = ObsID::tcZ;
      ObsID::char2tc['-'] = ObsID::tcUndefined;

      // Since some of the items in the enums don't have corresponding RINEX
      // definitions, make sure there is an entry for all values
      for(int i=ObsID::otUnknown; i<ObsID::otLast; i++)
         ObsID::ot2char[(ObsID::ObservationType)i] = ' ';
      for(int i=ObsID::cbUnknown; i<ObsID::cbLast; i++)
         ObsID::cb2char[(ObsID::CarrierBand)i] = ' ';
      for(int i=ObsID::tcUnknown; i<ObsID::tcLast; i++)
         ObsID::tc2char[(ObsID::TrackingCode)i] = ' ';

      // Here the above three maps are reversed to speed up the runtime
      for(std::map<char, ObsID::ObservationType>::const_iterator i=ObsID::char2ot.begin();
           i != ObsID::char2ot.end(); i++)
         ObsID::ot2char[i->second] = i->first;

      for(std::map<char, ObsID::CarrierBand>::const_iterator i=ObsID::char2cb.begin();
           i != ObsID::char2cb.end(); i++)
         ObsID::cb2char[i->second] = i->first;

      for(std::map<char, ObsID::TrackingCode>::const_iterator i=ObsID::char2tc.begin();
           i != ObsID::char2tc.end(); i++)
         ObsID::tc2char[i->second] = i->first;

      ObsID::validRinexSystems = "GRESJCI";

      ObsID::map1to3sys["G"] = "GPS";
      ObsID::map1to3sys["R"] = "GLO";
      ObsID::map1to3sys["E"] = "GAL";
      ObsID::map1to3sys["S"] = "SBA";
      ObsID::map1to3sys["J"] = "QZS";
      ObsID::map1to3sys["C"] = "BDS";
      ObsID::map1to3sys["I"] = "IRN";

      ObsID::map3to1sys["GPS"] = "G";
      ObsID::map3to1sys["GLO"] = "R";
      ObsID::map3to1sys["GAL"] = "E";
      ObsID::map3to1sys["SBA"] = "S";
      ObsID::map3to1sys["QZS"] = "J";
      ObsID::map3to1sys["BDS"] = "C";
      ObsID::map3to1sys["IRN"] = "I";

      ObsID::validRinexFrequencies = "12356789";

      // this defines the valid obs types
      // NB these tc characters are ORDERED ~best to worst
      ObsID::validRinexTrackingCodes['G']['1'] = "CWPYLMIQSXN* ";   // except no C1N
      ObsID::validRinexTrackingCodes['G']['2'] = "WXPYSLCDMIQN* ";  // except no C2N
      ObsID::validRinexTrackingCodes['G']['5'] = "XQI* ";

      ObsID::validRinexTrackingCodes['R']['1'] = "CP* ";
      ObsID::validRinexTrackingCodes['R']['2'] = "PC* ";
      ObsID::validRinexTrackingCodes['R']['3'] = "IQX* ";

      ObsID::validRinexTrackingCodes['E']['1'] = "XCABZ* ";
      ObsID::validRinexTrackingCodes['E']['5'] = "XQI* ";
      ObsID::validRinexTrackingCodes['E']['6'] = "ABCXZ* ";
      ObsID::validRinexTrackingCodes['E']['7'] = "XQI* ";
      ObsID::validRinexTrackingCodes['E']['8'] = "XQI* ";

      ObsID::validRinexTrackingCodes['S']['1'] = "C* ";
      ObsID::validRinexTrackingCodes['S']['5'] = "IQX* ";

      ObsID::validRinexTrackingCodes['J']['1'] = "CXSLZ* ";
      ObsID::validRinexTrackingCodes['J']['2'] = "XLS* ";
      ObsID::validRinexTrackingCodes['J']['5'] = "XQI* ";
      ObsID::validRinexTrackingCodes['J']['6'] = "SLX* ";

      //NB 24Jun2013 MGEX data uses 2!
      ObsID::validRinexTrackingCodes['C']['1'] = "IXQ* ";
      ObsID::validRinexTrackingCodes['C']['2'] = "IXQ* ";
      ObsID::validRinexTrackingCodes['C']['6'] = "IXQ* ";
      ObsID::validRinexTrackingCodes['C']['7'] = "IXQ* ";

      ObsID::validRinexTrackingCodes['I']['5'] = "ABCX* ";
      ObsID::validRinexTrackingCodes['I']['9'] = "ABCX* ";
   }

}
