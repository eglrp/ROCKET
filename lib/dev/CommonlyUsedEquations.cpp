#pragma ident "$Id$"

/**
 * @file CommonlyUsedEquations.cpp
 * This class defines commonly used equations
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

#include "CommonlyUsedEquations.hpp"

namespace gpstk
{

	CommonlyUsedEquations::CommonlyUsedEquations()
	{
			// Variables used in description of equMWMaster and equMWRef
				
				// Header type
		Variable MW( TypeID::MWubbena);

				// MW Receiver bias variable
		RecBiasRandomWalkModel updMWModel;
		Variable updMW( TypeID::updMW, &updMWModel, true, false, 9.0e10 );

				// Ambiguity variable
		PhaseAmbiguityModel ambWLModel;
		Variable ambWL( TypeID::BWL, &ambWLModel, true, true ); 

				// MW Satellite bias variable
		SatBiasRandomWalkModel updSatMWModel;
		updSatMWModel.setQprime(3.0e-7);
		Variable updSatMW( TypeID::updSatMW, &updSatMWModel, false, true);

			// Description of equMWMaster
		equMWMaster.header.indTerm = MW;
		equMWMaster.addVariable(updSatMW, true, -1.0);
		equMWMaster.addVariable(ambWL, true, -1*WL_WAVELENGTH_GPS); 
//		equMWMaster.setWeight(25.0);
		// We donot specify equationSource here!!!


			// Description of equMWRef
		equMWRef.header.indTerm = MW;
		equMWRef.addVariable(updMW, true);
		equMWRef.addVariable(updSatMW, true, -1.0);
		equMWRef.addVariable(ambWL, true, -1*WL_WAVELENGTH_GPS);
//		equMWRef.setWeight(25.0);
		equMWRef.header.equationSource = Variable::someSources;

			// Variables used in description of equMWMaster and equMWRef
	
				// Header type
		Variable BLC( TypeID::BLC );

			// LC Receiver bias variable
		RecBiasRandomWalkModel updLCModel;
		Variable updLC( TypeID::updLC, &updLCModel, true, false, 9.0e10 );

			// Ambiguity variable
		PhaseAmbiguityModel ambNLModel;
		Variable ambNL( TypeID::BL1, &ambNLModel, true, true );

			// LC satellite bias variable
		SatBiasRandomWalkModel updSatLCModel;
		Variable updSatLC( TypeID::updSatLC, &updSatLCModel, false, true );

			// Description of equBLCMaster
		equBLCMaster.header.indTerm = BLC;
		equBLCMaster.addVariable(updSatLC, true, -1.0);
		equBLCMaster.addVariable(ambNL, true, -1*NL_WAVELENGTH_GPS );

			// Description of equBLCRef
		equBLCRef.header.indTerm = BLC;
		equBLCRef.addVariable(updLC, true);
		equBLCRef.addVariable(updSatLC, true, -1.0);
		equBLCRef.addVariable(ambNL, true, -1*NL_WAVELENGTH_GPS);
		



		
	}  // End of ' CommonlyUsedEquations::CommonlyUsedEquations() '
}  // End of ' namespace gpstk '
