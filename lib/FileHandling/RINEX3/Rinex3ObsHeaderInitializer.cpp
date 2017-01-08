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

#include "Rinex3ObsHeaderInitializer.hpp"

namespace gpstk
{
	Rinex3ObsHeaderInitializer::Rinex3ObsHeaderInitializer()
	{
		// The priority table reference to RTKLIB2.4.3
			
			// GPS, a slightly different from RTKLIB2.4.3
				// L1: PYWCMNSL
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcP] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcY] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcW] = 13;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcCA] = 12;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcM] = 11;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcN] = 10;
		//Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcC2M] = 9;
		//Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL1][ObsID::tcC2L] = 8;
				// L2: PYWCMNDSLX
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcP] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcY] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcW] = 13;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcCA] = 12;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcM] = 11;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcN] = 9;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcD] = 8;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcC2M] = 7;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcC2L] = 6;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL2][ObsID::tcC2LM] = 5;
				// L5: IQX
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL5][ObsID::tcIE5] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL5][ObsID::tcQE5] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["G"][ObsID::cbL5][ObsID::tcIQ5] = 13;

			// GLONASS
				// G1: PC
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG1][ObsID::tcGP] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG1][ObsID::tcGCA] = 14;
				// G2: PC
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG2][ObsID::tcGP] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG2][ObsID::tcGCA] = 14;
				// G3: IQX
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG3][ObsID::tcIR3] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG3][ObsID::tcQR3] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["R"][ObsID::cbG3][ObsID::tcIQR3] = 13;

			// GALILEO
				// E1-L1: CABXZ
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL1][ObsID::tcC] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL1][ObsID::tcA] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL1][ObsID::tcB] = 13;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL1][ObsID::tcBC] = 12;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL1][ObsID::tcABC] = 11;
				// E5a-L5: IQX
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL5][ObsID::tcIE5] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL5][ObsID::tcQE5] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbL5][ObsID::tcIQE5] = 13;
				// E5b: IQX
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5b][ObsID::tcIE5] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5b][ObsID::tcQE5] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5b][ObsID::tcIQE5] = 13;
				// E5ab: IQX
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5ab][ObsID::tcIE5] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5ab][ObsID::tcQE5] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE5ab][ObsID::tcIQE5] = 13;
				// E6: ABCXZ
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE6][ObsID::tcA] = 15;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE6][ObsID::tcB] = 14;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE6][ObsID::tcC] = 13;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE6][ObsID::tcBC] = 12;
		Rinex3ObsHeader::sysCbTcPriMap["E"][ObsID::cbE6][ObsID::tcABC] = 11;

		






	}  // End of ' Rinex3ObsHeaderInitializer:: ... '

}  // End of ' namespace gpstk ... '
