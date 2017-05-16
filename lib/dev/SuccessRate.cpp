#pragma ident "Id$"

/**
 * @file SuccessRate.hpp 
 * This is a class to compute success rate of different integer estimators
 * in the presence or absence of bias vector (ie. ionospheric bias ). 
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
//============================================================================
//  Date :			2017/05/16 - 2017/05/17
//  Author(s):		Lei Zhao, a Ph.D. candiate
//  School of Geodesy and Geomatics, Wuhan University 
//============================================================================

#include "SuccessRate.hpp"


namespace gpstk
{

		// Return computed success Rate 
	double SuccessRate::getSuccessRate()
	{
		switch( intEstimator )
		{
			case IR: return SRRound( Qahat, srt, decor, bias );		break;
			case IB: return SRBoot( Qahat, srt, decor, bias );			break;
		};
	}

}   // End of namespace gpstk 


