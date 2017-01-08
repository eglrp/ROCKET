#pragma ident "$Id$"

/**
 * @file ComputeLinearMGEX.cpp
 * This class computes linear combinations of GDS data.
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


#include "ComputeLinearMGEX.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeLinearMGEX::getClassName() const
   { return "ComputeLinearMGEX"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeLinearMGEX::Process( const CommonTime& time,
                                            satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for( it = gData.begin(); it != gData.end(); ++it )
         {

					// SatID
				SatID sat( it->first );

					// LinearCombList
				LinearCombList linearList2;

					// Find the related list 
				std::map< SatID::SatelliteSystem, LinearCombList > ::iterator itSysList
						= sysLinearList.find( sat.system );
				if( itSysList != sysLinearList.end() )
				{
					linearList2 = (*itSysList).second;					
				}
				else {
						// For orther sat systems
						// we currently skip them !!! 
					continue;
				}  // End of ' if( itSysList != sysLinearList.end() ) '

               // Loop through all the defined linear combinations
            LinearCombList::const_iterator pos;
            for( pos = linearList2.begin(); pos != linearList2.end(); ++pos )
            {

               double result(0.0);

                  // Read the information of each linear combination
               typeValueMap::const_iterator iter;
               for(iter = pos->body.begin(); iter != pos->body.end(); ++iter)
               {
                  double temp(0.0);

                  TypeID type(iter->first);

                  if( (*it).second.find(type) != (*it).second.end() )
                  {
                     temp = (*it).second[type];
                  }
                  else
                  {
                     temp = 0.0;
                  }

                  result = result + (*iter).second * temp;
               }

                  // Store the result in the proper place
               (*it).second[pos->header] = result;

            }

         }

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeLinearMGEX::Process()'


} // End of namespace gpstk
