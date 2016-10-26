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

//		// Initialization for static members
//	Variable CommonlyUsedEquations::MW = Variable(TypeID::MWubbena);
//	RecBiasRandomWalkModel CommonlyUsedEquations::updMWModel
//				= RecBiasRandomWalkModel();
//	Variable CommonlyUsedEquations::updMW 
//				= Variable( TypeID::updMW, &updMWModel, true, false, 9.0e10 );
//	PhaseAmbiguityModel CommonlyUsedEquations::ambWLModel 
//				= PhaseAmbiguityModel();
//	Variable CommonlyUsedEquations::ambWL 
//				= Variable( TypeID::BWL, &ambWLModel, true, true );
//	SatBiasRandomWalkModel CommonlyUsedEquations::updSatMWModel
//				= SatBiasRandomWalkModel( 3.0e-7 );
//	Variable CommonlyUsedEquations::updSatMW
//				= Variable( TypeID::updSatMW, &updSatMWModel, false, true);
//	Variable CommonlyUsedEquations::BLC
//				= Variable( TypeID::BLC );
//	RecBiasRandomWalkModel CommonlyUsedEquations::updLCModel
//				= RecBiasRandomWalkModel();
//	Variable CommonlyUsedEquations::updLC
//				= Variable( TypeID::updLC, &updLCModel, true, false, 9.0e10 );
//	PhaseAmbiguityModel CommonlyUsedEquations::ambNLModel 
//				= PhaseAmbiguityModel();
//	Variable CommonlyUsedEquations::ambNL
//				= Variable( TypeID::BL1, &ambNLModel, true, true );
//
//	SatBiasRandomWalkModel CommonlyUsedEquations::updSatLCModel
//				= SatBiasRandomWalkModel();
//	Variable CommonlyUsedEquations::updSatLC
//				= Variable( TypeID::updSatLC, &updSatLCModel, false, true );

	CommonlyUsedEquations::CommonlyUsedEquations()
	{
			// Variables used in description of equMWMaster and equMWRef
				
				// Header type
//		Variable MW( TypeID::MWubbena);

//				// MW Receiver bias variable
//		RecBiasRandomWalkModel updMWModel;
//		Variable updMW( TypeID::updMW, &updMWModel, true, false, 9.0e10 );
//
//				// Ambiguity variable
//		PhaseAmbiguityModel ambWLModel;
//		Variable ambWL( TypeID::BWL, &ambWLModel, true, true ); 
//
//				// MW Satellite bias variable
//		SatBiasRandomWalkModel updSatMWModel;
//		updSatMWModel.setQprime(3.0e-7);
//		Variable updSatMW( TypeID::updSatMW, &updSatMWModel, false, true);

			// Description of equMWMaster
		equMWMaster.header.indTerm = Variable::MW;
		equMWMaster.addVariable(Variable::updSatMW, true, -1.0);
		equMWMaster.addVariable(Variable::ambWL, true, -1*WL_WAVELENGTH_GPS); 
//		equMWMaster.setWeight(25.0);
		// We donot specify equationSource here!!!


			// Description of equMWRef
		equMWRef.header.indTerm = Variable::MW;
		equMWRef.addVariable(Variable::updMW, true);
		equMWRef.addVariable(Variable::updSatMW, true, -1.0);
		equMWRef.addVariable(Variable::ambWL, true, -1*WL_WAVELENGTH_GPS);
		equMWRef.setWeight(25.0);
		equMWRef.header.equationSource = Variable::someSources;

 		// Variables used in description of equMWMaster and equMWRef
 
 			// Header type
//		Variable BLC( TypeID::BLC );
//
//			// LC Receiver bias variable
//		RecBiasRandomWalkModel updLCModel;
//		Variable updLC( TypeID::updLC, &updLCModel, true, false, 9.0e10 );
//
//			// Ambiguity variable
//		PhaseAmbiguityModel ambNLModel;
//		Variable ambNL( TypeID::BL1, &ambNLModel, true, true );
//
//			// LC satellite bias variable
//		SatBiasRandomWalkModel updSatLCModel;
//		Variable updSatLC( TypeID::updSatLC, &updSatLCModel, false, true );

			// Description of equBLCMaster
		equBLCMaster.header.indTerm = Variable::BLC;
		equBLCMaster.addVariable(Variable::updSatLC, true, -1.0);
		equBLCMaster.addVariable(Variable::ambNL, true, -1*NL_WAVELENGTH_GPS );

			// Description of equBLCRef
		equBLCRef.header.indTerm = Variable::BLC;
		equBLCRef.addVariable(Variable::updLC, true);
		equBLCRef.addVariable(Variable::updSatLC, true, -1.0);
		equBLCRef.addVariable(Variable::ambNL, true, -1*NL_WAVELENGTH_GPS);
		



		
	}  // End of ' CommonlyUsedEquations::CommonlyUsedEquations() '
}  // End of ' namespace gpstk '
