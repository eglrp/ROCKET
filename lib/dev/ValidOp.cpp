#pragma ident "$Id$"

/**
 * @file ValidOp.cpp
 * Class to define and handle 'descriptions' of GNSS variables.
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


#include "ValidOp.hpp"


namespace gpstk
{


   std::string ValidOp::getClassName(void) const
   { return "ValidOp"; }


      /** valid gnssDataMap
       *
       * @param gData   the data need to valid
       *
       */
   gnssDataMap& ValidOp::Process( gnssDataMap& gData )
      throw(NotFoundMaster, MasterLessSatNum)
   {
      
      bool findMaster = false;
      bool lessSatNum = false;
      
      // name of master station.
      std::string masterName = m_pStateStore->getMasterName();

      for( gnssDataMap::iterator gdsIter = gData.begin();
           gdsIter != gData.end(); gdsIter++ )
      {
         for( sourceDataMap::iterator sdmIter = gdsIter->second.begin();
              sdmIter != gdsIter->second.end();)
         {
            
            if( !m_pBLQStore->isValid( sdmIter->first.sourceName ) )
            {
               gdsIter->second.erase( sdmIter++ );
               continue;
            }

            if( sdmIter->first.sourceName == masterName )
            {
               findMaster = true;
               if( sdmIter->second.numSats() <= 0 )
               {
                  lessSatNum = true;
               }
            }

            sdmIter++;
         }
      }
      
      if( !findMaster )
      {
         NotFoundMaster notFoundMaster("Not found master station");
         GPSTK_THROW(notFoundMaster);
      }

      if( lessSatNum )
      {
         MasterLessSatNum masterLessSatNum("master station has less number of satellite!");
         GPSTK_THROW(masterLessSatNum);
      }

      return gData;
   }

}  // End of namespace gpstk
