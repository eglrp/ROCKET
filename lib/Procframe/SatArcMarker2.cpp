#pragma ident "$Id: SatArcMarker2.cpp 2513 2011-02-20 09:29:32Z yanweignss $"

/**
 * @file SatArcMarker2.cpp
 * This class keeps track of satellite arcs caused by cycle slips.
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
//============================================================================
//
//  Modifications:
//
//  2013/06/30   This satellite arc marker is different from SatArcMarker, and
//               it will delete unstable periods of each beginning arc, even
//               the first arc, which means this satellite is new.
//               by shjzhang.
//
//  2014/03/10   when cycle slip happens, delete the first satellite, if you
//               want to delete satellites in unstable period. by shjzhang.
//
//
//============================================================================


#include "SatArcMarker2.hpp"

using namespace std;

namespace gpstk
{

      // Returns a string identifying this object.
   std::string SatArcMarker2::getClassName() const
   { return "SatArcMarker2"; }



      /* Common constructor
       *
       * @param watchFlag        Cycle slip flag to be watched.
       * @param delUnstableSats  Whether unstable satellites will be deleted.
       * @param unstableTime     Number of seconds since last arc change
       *                         that a satellite will be considered as
       *                         unstable.
       */
   SatArcMarker2::SatArcMarker2( const TypeID& watchFlag,
                                 const bool delUnstableSats,
                                 const double unstableTime )
      : watchCSFlag(watchFlag), deleteUnstableSats(delUnstableSats)
   {

         // Set unstableTime value
      setUnstablePeriod( unstableTime );

   }  // End of method 'SatArcMarker2::SatArcMarker2()'



      /* Method to set the number of seconds since last arc change that a
       *  satellite will be considered as unstable.
       *
       * @param unstableTime     Number of seconds since last arc change
       *                         that a satellite will be considered as
       *                         unstable.
       */
   SatArcMarker2& SatArcMarker2::setUnstablePeriod(const double unstableTime)
   {

         // Check unstableTime value
      if (unstableTime > 0.0)
      {
         unstablePeriod = unstableTime;
      }
      else
      {
         unstablePeriod = 0.0;
      }

      return (*this);

   }  // End of method 'SatArcMarker2::setUnstablePeriod()'



      /* Method to get the arc changed epoch.
       * @param sat              Interested SatID.
       */
   CommonTime SatArcMarker2::getArcChangedEpoch(const SatID& sat)
   {

      SatArcData::const_iterator satArcDataIt = m_satArcData.find(sat);

      if( satArcDataIt != m_satArcData.end() )
      {
         return satArcDataIt->second.arcChangeTime;
      }
      else
      {
         return CommonTime(CommonTime::BEGINNING_OF_TIME);
      }

   }  // End of method 'SatArcMarker2::getArcChangedEpoch()'


      /* Method to get the arc changed epoch.
       * @param source           Interested SourceID.
       * @param sat              Interested SatID.
       */
   CommonTime SatArcMarker2::getArcChangedEpoch(const SourceID& source,
                                                const SatID& sat)
   {

      SatArcDataMap::const_iterator satArcDataMapIt = m_satArcDataMap.find(source);

      if( satArcDataMapIt != m_satArcDataMap.end() )
      {
         m_satArcData = satArcDataMapIt->second;

         return getArcChangedEpoch( sat );
      }
      else
      {
         return CommonTime( CommonTime::BEGINNING_OF_TIME );
      }

   }


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& SatArcMarker2::Process( const CommonTime& epoch,
                                            satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

         double flag(0.0);

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         for ( satTypeValueMap::iterator it = gData.begin();
               it != gData.end();
               ++it )
         {
            if( (*it).first.system == SatID::systemBDS )
            {
                watchCSFlag = TypeID::CSL2;
            }

            try
            {
                  // Try to extract the CS flag value
               flag = (*it).second(watchCSFlag);
            }
            catch(...)
            {
                  // If flag is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

            cout << "debug" << endl;

               // Check if satellite currently has entries
            SatArcData::const_iterator satArcIt = m_satArcData.find(it->first);

            if( satArcIt == m_satArcData.end() )
            {
               m_satArcData[it->first].arcNum = 0.0;
               m_satArcData[it->first].arcChangeTime = CommonTime::BEGINNING_OF_TIME;
            }

               // Check if there was a cycle slip
            if ( flag > 0.0 )
            {
                  // Increment the value of "TypeID::satArc"
               m_satArcData[it->first].arcNum = m_satArcData[it->first].arcNum + 1.0;

                  // Update arc change epoch
               m_satArcData[it->first].arcChangeTime = epoch;

                  // If we want to delete unstable satellites, we must do it
                  // also when arc changes, whether or not this SV is new
               if ( deleteUnstableSats )
               {
                  satRejectedSet.insert( (*it).first );
               }
            }

               // Check if we are inside unstable period
            bool insideUnstable(std::abs(epoch-m_satArcData[(*it).first].arcChangeTime)
                                                                    <= unstablePeriod );

               // Test if we want to delete unstable satellites. Only do if this
               // satellite is inside the unstable period.
               // whether or not the satellite is new
            if ( insideUnstable &&
                 deleteUnstableSats )
            {
               satRejectedSet.insert( (*it).first );
            }

               // We will insert satellite arc number
            (*it).second[TypeID::satArc] = m_satArcData[ (*it).first ].arcNum;

         }

         cout << satRejectedSet.size() << endl;

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         watchCSFlag = TypeID::CSL1;

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SatArcMarker2::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data generated
       *  when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SatArcMarker2::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SatArcMarker2::Process()'



      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SatArcMarker2::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SatArcMarker2::Process()'


     /** Returns a gnssDataMap object, adding the new data generated when
      *  calling this object.
      *
      * @param gData    Data object holding the data.
      */
   gnssDataMap& SatArcMarker2::Process(gnssDataMap& gData)
       throw(ProcessingException)
   {
        SourceID source;

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                source = sdmIt->first;

                SatArcDataMap::iterator satArcDataIt = m_satArcDataMap.find(source);

                if( satArcDataIt != m_satArcDataMap.end() )
                {
                    m_satArcData = satArcDataIt->second;
                }
                else
                {
                    m_satArcData = SatArcData();
                }

                Process( gdmIt->first, sdmIt->second );

                m_satArcDataMap[source] = m_satArcData;
            }
        }

        return gData;

   }  // End of method 'SatArcMarker2::Process()'


}  // End of namespace gpstk
