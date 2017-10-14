#pragma ident "$Id$"

/**
 * @file SatArcMarker.cpp
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================


#include "SatArcMarker.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string SatArcMarker::getClassName() const
   { return "SatArcMarker"; }



      /* Common constructor
       *
       * @param watchFlag        Cycle slip flag to be watched.
       * @param delUnstableSats  Whether unstable satellites will be deleted.
       * @param unstableTime     Number of seconds since last arc change
       *                         that a satellite will be considered as
       *                         unstable.
       */
   SatArcMarker::SatArcMarker( const TypeID& watchFlag,
                               const bool delUnstableSats,
                               const double unstableTime )
      : watchCSFlag(watchFlag), deleteUnstableSats(delUnstableSats)
   {

         // Set unstableTime value
      setUnstablePeriod( unstableTime );

   }  // End of method 'SatArcMarker::SatArcMarker()'



      /* Method to set the number of seconds since last arc change that a
       *  satellite will be considered as unstable.
       *
       * @param unstableTime     Number of seconds since last arc change
       *                         that a satellite will be considered as
       *                         unstable.
       */
   SatArcMarker& SatArcMarker::setUnstablePeriod(const double unstableTime)
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

   }  // End of method 'SatArcMarker::setUnstablePeriod()'


      /* Method to get the arc changed epoch.
       * @param sat              Interested SatID.
       */
   CommonTime SatArcMarker::getArcChangedEpoch(const SatID& sat)
   {
      SatArcData::iterator it = m_satArcData.find(sat);
      if(it != m_satArcData.end())
      {
         return it->second.arcChangeTime;
      }
      else
      {
         return CommonTime::BEGINNING_OF_TIME;
      }

   }  // End of method 'SatArcMarker::getArcChangedEpoch()'


      /* Method to get the arc changed epoch.
       * @param source           Interested SourceID.
       * @param sat              Interested SatID.
       */
   CommonTime SatArcMarker::getArcChangedEpoch(const SourceID& source,
                                               const SatID& sat)
   {
      SatArcDataMap::iterator it = m_satArcDataMap.find(source);

      if(it != m_satArcDataMap.end())
      {
         m_satArcData = it->second;

         return getArcChangedEpoch( sat );
      }
      else
      {
         return CommonTime::BEGINNING_OF_TIME;
      }

   }  // End of method 'SatArcMarker::getArcChangedEpoch()'


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& SatArcMarker::Process( const CommonTime& epoch,
                                            satTypeValueMap& gData )
        throw(ProcessingException)
    {
        try
        {
            double flag(0.0);

            SatIDSet satRejectedSet;

            SatID sat;

            // Loop through all the satellites
            for ( satTypeValueMap::iterator it = gData.begin();
                  it != gData.end();
                  ++it )
            {
                sat = (*it).first;

                if(sat.system == SatID::systemGPS)
                {
                    watchCSFlag = TypeID::CSL1;
                }
                else if(sat.system == SatID::systemGalileo)
                {
                    watchCSFlag = TypeID::CSL1;
                }
                else if( sat.system == SatID::systemBDS )
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
                    satRejectedSet.insert( sat );
                    continue;
                }

                // Check if satellite currently has entries
                SatArcData::const_iterator itArc( m_satArcData.find( sat ) );
                if( itArc == m_satArcData.end() )
                {
                    // If it doesn't have an entry, insert one
                    m_satArcData[sat].arcChangeTime = CommonTime::BEGINNING_OF_TIME;
                    m_satArcData[sat].arcNum = 0.0;

                    // This is a new satellite
                    m_satArcData[sat].arcNew = true;
                }

                // Check if we are inside unstable period
                double dt( std::abs(epoch-m_satArcData[sat].arcChangeTime) );
                bool insideUnstable( dt <= unstablePeriod );

                // Satellites can be new only once, and having at least once a
                // flag > 0.0 outside 'unstablePeriod' will make them old.
                if( m_satArcData[sat].arcNew && !insideUnstable && flag <= 0.0 )
                {
                    m_satArcData[sat].arcNew = false;
                }


                // Check if there was a cycle slip
                if ( flag > 0.0 )
                {
                    // Increment the value of "TypeID::satArc"
                    m_satArcData[sat].arcNum = m_satArcData[sat].arcNum + 1.0;

                    // Update arc change epoch
                    m_satArcData[sat].arcChangeTime = epoch;

                    // If we want to delete unstable satellites, we must do it
                    // also when arc changes, but only if this SV is not new
                    if ( deleteUnstableSats  && (!m_satArcData[sat].arcNew) )
                    {
                        satRejectedSet.insert( sat );
                    }
                }


                // Test if we want to delete unstable satellites. Only do it
                // if satellite is NOT new and we are inside unstable period
                if ( insideUnstable && deleteUnstableSats && (!m_satArcData[sat].arcNew) )
                {
                    satRejectedSet.insert( sat );
                }

                // We will insert satellite arc number
                (*it).second[TypeID::satArc] = m_satArcData[sat].arcNum;

            }

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );

            GPSTK_THROW(e);
        }

    }  // End of method 'SatArcMarker::Process()'



      /* Returns a gnssSatTypeValue object, adding the new data generated
       *  when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SatArcMarker::Process(gnssSatTypeValue& gData)
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
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SatArcMarker::Process()'



      /* Returns a gnssRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SatArcMarker::Process(gnssRinex& gData)
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
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SatArcMarker::Process()'


     /** Returns a gnssDataMap object, adding the new data generated when
      *  calling this object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& SatArcMarker::Process(gnssDataMap& gData)
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

    }  // End of method 'SatArcMarker::Process()'

}  // End of namespace gpstk
