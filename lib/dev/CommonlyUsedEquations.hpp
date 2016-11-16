#pragma ident "$Id$"

/**
 * @file CommonlyUsedEquations.hpp
 * This class defines commonly used equations.
 */

#ifndef GPSTK_COMMONLYUSEDEQUATIONS
#define GPSTK_COMMONLYUSEDEQUATIONS

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009//
//============================================================================
//
//  2016/10/24
//	 Create this class 
//	 Lei Zhao, SGG, WHU, China
//
//============================================================================

#include "Equation.hpp"
#include "Variable.hpp"
#include "StochasticModel.hpp"
#include "GNSSconstants.hpp"

namespace gpstk 
{

	// vvv
	// Example code
	// ^^^

	class CommonlyUsedEquations 
	{
	public:

			/// Default constructor
		CommonlyUsedEquations();

			/// Definition to describe HMW observable at master station
		Equation equMWMaster;

			/// Definition to describe HMW observable at reference station
		Equation equMWRef;

			/// Definition to describe BLC observable at msater station
		Equation equBLCMaster;

			/// Definition to describe BLC observable at reference station
		Equation equBLCRef;

			/// Definition to describe P1 observable at master station
		Equation equP1Master;

			/// Definition to describe P1 observable at reference station
		Equation equP1Ref;

			/// Definition to describe P2 observable at master station
		Equation equP2Master;

			/// Definition to describe P2 observable at reference station
		Equation equP2Ref;

			/// Definition to describe L1 observable at master station
		Equation equL1Master;

			/// Definition to describe L1 observable at receiver station
		Equation equL1Ref;

			/// Definition to describe L2 observable at master station
		Equation equL2Master;

			/// Definition to describe L2 observable at reference station
		Equation equL2Ref;

			/// Definition to describe Iono observable
		Equation equIono;

			/// Definition ro describe receiver P1P2 DCB observable 
		Equation equRecDCB;

	};  // End of ' class CommonlyUsedEquations '

}  // End of ' namespace gpstk '

#endif 
