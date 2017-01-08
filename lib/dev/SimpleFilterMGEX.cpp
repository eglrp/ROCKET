#pragma ident "$Id$"

/**
 * @file SimpleFilterMGEX.cpp
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
//============================================================================


#include "SimpleFilterMGEX.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string SimpleFilterMGEX::getClassName() const
   { return "SimpleFilterMGEX"; }

		/// Default constructor
	SimpleFilterMGEX::SimpleFilterMGEX()
	{
			// GPS 
		sysMinLimit[ SatID::systemGPS ] = 15000000.0;
		sysMaxLimit[ SatID::systemGPS ] = 30000000.0;

			// Galileo
		sysMinLimit[ SatID::systemGalileo ] = 15000000.0;
		sysMaxLimit[ SatID::systemGalileo ] = 30000000.0;


	}  // End of ' SimpleFilterMGEX() '


			/** Checks that the value is within the given limits.
			 * @param				sat system
          * @param value     The value to be test
          *
          * @return
          *  True if check was OK.
          */
      bool SimpleFilterMGEX::checkValue(
										SatID::SatelliteSystem sys, 
										const double& value ) const
		{
			double minLimit(0.0);
			double maxLimit(0.0);

				// Find the limits
			std::map< SatID::SatelliteSystem, double>::const_iterator itMin = 
																	sysMinLimit.find( sys );
			if( itMin != sysMinLimit.end() )
			{
				minLimit = itMin -> second;
			}

			std::map< SatID::SatelliteSystem, double>::const_iterator itMax = 
																	sysMaxLimit.find( sys );
			if( itMax != sysMaxLimit.end() )
			{
				maxLimit = itMax -> second;
			}

			return ( (value>=minLimit) && (value<=maxLimit) );


		}  // End of ' bool checkValue( '



      // Returns a satTypeValueMap object, filtering the target observables.
      //
      // @param gData     Data object holding the data.
      //
   satTypeValueMap& SimpleFilterMGEX::Process(satTypeValueMap& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {
            // Now, Let's check the range of the code observables.
            // if not located in the given range, the satellite
            // will be removed directly.

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it) 
         {
            try
            {
						// Sat
					SatID sat( it->first );

						// Find the TypeIDSet of present sat system
					TypeIDSet filterTypeSet2;

					std::map< SatID::SatelliteSystem, TypeIDSet >::iterator ite =
														sysFilterTypeSet.find( sat.system );

					if( ite != sysFilterTypeSet.end() )
					{
						filterTypeSet2 = ite->second;
					}
//					else
//					{
//						Exception e("Satellite system is not found in sysDilterTypeIDSet");
//						GPSTK_THROW(e);
//					}

						// Check all the indicated TypeID's
					TypeIDSet::const_iterator pos;
					for ( pos = filterTypeSet2.begin(); 
						   pos != filterTypeSet2.end();
							++pos)
					{

						double value(0.0);
                  
							// Try to extract the values
						value = (*it).second(*pos);

	                  // Now, check that the value is within bounds
	               if ( !( checkValue( sat.system, value) ) )
	               {
	                     // If value is out of bounds, then schedule this
	                     // satellite for removal
	                   satRejectedSet.insert( (*it).first );
	               }
					}  // End of 'for ( pos = filterTypeSet2.begin(); ... '
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
            }
         }  // End of 'for (it = gData.begin(); ... '

            // Before checking next TypeID, let's remove satellites with
            // data out of bounds
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

   }  // End of 'SimpleFilterMGEX::Process()'


} // End of namespace gpstk
