#pragma ident "$Id$"

/**
 * @file RequireObservablesMGEX.cpp
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


#include "RequireObservablesMGEX.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string RequireObservablesMGEX::getClassName() const
   { return "RequireObservablesMGEX"; }



      /* Method to add a set of TypeID's to be required.
       *
       * @param typeSet    Set of TypeID's to be required.
       */
   RequireObservablesMGEX& RequireObservablesMGEX::addRequiredType(TypeIDSet& typeSet)
   {

      requiredTypeSet.insert( typeSet.begin(),
                              typeSet.end() );

      return (*this);

   }  // End of method 'RequireObservablesMGEX::addRequiredType()'


			/** Method to add a set of TypeID's to be required.
          *
          * @param typeSet    Set of TypeID's to be required.
          */
      RequireObservablesMGEX& RequireObservablesMGEX::addRequiredType(
									SatID::SatelliteSystem sys,
									const TypeID& type )
		{
			( requiredSysTypeIDSet[ sys ] ).insert( type );	

			return (*this);
		}





      // Returns a satTypeValueMap object, filtering the target observables.
      //
      // @param gData     Data object holding the data.
      //
   satTypeValueMap& RequireObservablesMGEX::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;


            // Loop through all the satellites
         for ( satTypeValueMap::iterator satIt = gData.begin();
               satIt != gData.end();
               ++satIt )
         {

					// Present sat	
				SatID sat( satIt -> first );

					// Find the TypeIDSat of present sat system 
				TypeIDSet requiredTypeSet2;

				std::map< SatID::SatelliteSystem, TypeIDSet >::iterator it = 
													requiredSysTypeIDSet.find( sat.system );

				if( it != requiredSysTypeIDSet.end() )
				{
					//requiredTypeSet2 = requiredSysTypeIDSet[ sat.system ];  
					requiredTypeSet2 = (*it).second;
				}
				else
				{
					Exception e("Satellite system is not found in requiredSysTypeIDSet");
					GPSTK_THROW(e);
//					satRejectedSet.insert(sat);
				}


               // Check all the indicated TypeID's
            for ( TypeIDSet::const_iterator typeIt = requiredTypeSet2.begin();
                  typeIt != requiredTypeSet2.end();
                  ++typeIt )
            {


                  // Try to find required type
               typeValueMap::iterator it( (*satIt).second.find(*typeIt) );

                  // Now, check if this TypeID exists in this data structure
						
						// Augmentation: 
						// Identify the existed type but with zero value
						// Added by Lei Zhao, 2016/07/21, WHU

               if ( it == (*satIt).second.end() || ( (*satIt).second(*typeIt) == 0.0 ) )
               {
						// Debug code vvv
					//	std::cout << "problem type: " << *typeIt << std::endl;
					//	std::cout << "(*satIt).second(*typeIt) = " << (*satIt).second(*typeIt) << std::endl;
						// Debug code ^^^

                     // If we couldn't find type, then schedule this
                     // satellite for removal
                  satRejectedSet.insert( (*satIt).first );

                     // It is not necessary to keep looking
                  //typeIt = requiredTypeSet.end();
                  //--typeIt;
				  break;
               }
					
            }

         }  // End of ' for ( satTypeValueMap::iterator satIt = gData.begin(); '

            // Let's remove satellites without all TypeID's
        gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of 'RequireObservablesMGEX::Process()'


} // End of namespace gpstk
