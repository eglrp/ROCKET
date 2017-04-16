#pragma ident "$Id XY.CAO 2016-11-16 $"

/**
* @file SimpleFilter.cpp
* This class filters out satellites with observations grossly out of bounds.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//  Copywright(c) 2016, XY. CAO, Wuhan University 
//============================================================================


#include "CodeGrossFilter.hpp"


namespace gpstk
{

	// Returns a string identifying this object.
	std::string CodeGrossFilter::getClassName() const
	{ return "CodeGrossFilter"; }


	// Returns a satTypeValueMap object, filtering the target observables.
	//
	// @param gData     Data object holding the data.
	//
	satTypeValueMap& CodeGrossFilter::Process(satTypeValueMap& gData)
		throw(ProcessingException, SVNumException)
	{

		try
		{
			// Now, Let's check the range of the code combinations PI.
			// if not located in the given range, the satellite
			// will be removed directly.

			SatIDSet satRejectedSet;

			// Loop through all the satellites
			for ( satTypeValueMap::iterator satIt = gData.begin();
				satIt != gData.end();
				++satIt )
			{
				// no matter which satellite system.

				// PI = P2 - P1
				double PIvalue(0.0);

				try
				{
					PIvalue = (*satIt).second[TypeID::PI];

					// std::cout << (*satIt).first <<  " :  " << PIvalue << std::endl; 

				}
				catch(...)
				{
					// If some value is missing, then schedule this satellite
					// for removal
					satRejectedSet.insert( (*satIt).first );

					continue;
				}

				// Now, check that the value is within bounds
				if ( checkValue(PIvalue) )
				{
					// If value is out of bounds, then schedule this
					// satellite for removal
					std::cout << (*satIt).first << ": " << PIvalue << std::endl;

					satRejectedSet.insert( (*satIt).first );
				}


			}// end satellite loop


			gData.removeSatID(satRejectedSet);

			return gData;

		}
		catch(SVNumException& s)
		{
			// Rethrow the SVNumException
			GPSTK_RETHROW(s);
		}
		catch(Exception& u)
		{
			// Throw an exception if something unexpected happens
			ProcessingException e( getClassName() + ":"
				+ u.what() );

			GPSTK_THROW(e);

		}

	}  // End of 'CodeGrossFilter::Process()'


} // End of namespace gpstk
