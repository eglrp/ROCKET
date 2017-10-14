#pragma ident "$Id$"

/**
 * @file ComputeWindUp.cpp
 * This class computes the wind-up effect on the phase observables, in radians.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
//
//============================================================================


#include "ComputeWindUp.hpp"

using namespace std;

namespace gpstk
{

    // Returns a string identifying this object.
    std::string ComputeWindUp::getClassName() const
    { return "ComputeWindUp"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeWindUp::Process( const CommonTime& time,
                                             satTypeValueMap& gData )
        throw(ProcessingException)
    {
        try
        {
            // Compute Sun position at this epoch
            SunPosition sunPosition;
            Triple sunPos(sunPosition.getPosition(time));

            // Define a Triple that will hold satellite position, in ECEF
            Triple svPos(0.0, 0.0, 0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for ( satTypeValueMap::iterator it = gData.begin();
                  it != gData.end();
                  ++it )
            {
                // First check if this satellite has previous arc information
                if( m_satPhaseData.find( (*it).first ) == m_satPhaseData.end() )
                {
                    // If it doesn't have an entry, insert one
                    m_satPhaseData[ (*it).first ].arcNum = 0.0;
                }

                // Then, check both if there is arc information, and if current
                // arc number is different from arc number in storage (which
                // means a cycle slip happened)
                if ( (*it).second.find(TypeID::satArc) != (*it).second.end() &&
                     (*it).second(TypeID::satArc) != m_satPhaseData[ (*it).first ].arcNum )
                {
                    // If different, update satellite arc in storage
                    m_satPhaseData[ (*it).first ].arcNum = (*it).second(TypeID::satArc);

                    // Reset phase information
                    m_satPhaseData[ (*it).first ].satPreviousPhase = 0.0;
                    m_satPhaseData[ (*it).first ].staPreviousPhase = 0.0;
                }


                // Use ephemeris if satellite position is not already computed
                if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
                {

                    if(pEphStore==NULL)
                    {
                        // If ephemeris is missing, then remove all satellites
                        satRejectedSet.insert( (*it).first );
                        continue;
                    }
                    else
                    {
                        // Try to get satellite position
                        // if it is not already computed
                        try
                        {
                            // For our purposes, position at receive time
                            // is fine enough
                            Xvt svPosVel(pEphStore->getXvt( (*it).first, time ));

                            // If everything is OK, then continue processing.
                            svPos[0] = svPosVel.x.theArray[0];
                            svPos[1] = svPosVel.x.theArray[1];
                            svPos[2] = svPosVel.x.theArray[2];
                        }
                        catch(...)
                        {
                            // If satellite is missing, then schedule it
                            // for removal
                            satRejectedSet.insert( (*it).first );
                            continue;
                        }
                    }
                }
                else
                {
                    // Get satellite position out of GDS
                    svPos[0] = (*it).second[TypeID::satX];
                    svPos[1] = (*it).second[TypeID::satY];
                    svPos[2] = (*it).second[TypeID::satZ];

                }  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'


                // Let's get wind-up value in radians, and insert it
                // into GNSS data structure.
                (*it).second[TypeID::windUp] =
                                    getWindUp((*it).first, time, svPos, sunPos);

            }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

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

    }  // End of method 'ComputeWindUp::Process()'



     /** Returns a gnssDataMap object, adding the new data generated
      *  when calling this object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& ComputeWindUp::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

        SourceID source;
        string station;

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            CommonTime epoch( gdmIt->first );
            epoch.setTimeSystem( TimeSystem::Unknown );

            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                source = sdmIt->first;
                station = source.sourceName;

                if(pMSCStore == NULL)
                {
                    sourceRejectedSet.insert( source );
                    continue;
                }

                MSCData mscData;
                try
                {
                    mscData = pMSCStore->findMSC(station,epoch);
                }
                catch(...)
                {
                    sourceRejectedSet.insert( source );
                    continue;
                }

                nominalPos = mscData.coordinates;

                SatPhaseDataMap::iterator satPhaseDataIt = m_satPhaseDataMap.find(source);

                if(satPhaseDataIt != m_satPhaseDataMap.end())
                {
                    m_satPhaseData = satPhaseDataIt->second;
                }
                else
                {
                    m_satPhaseData = SatPhaseData();
                }

                Process( gdmIt->first, sdmIt->second );

                m_satPhaseDataMap[source] = m_satPhaseData;
            }
        }

        gData.removeSourceID( sourceRejectedSet );

        return gData;

    }  // End of method 'ComputeWindUp::Process()'



      /* Sets name of "PRN_GPS"-like file containing satellite data.
       * @param name      Name of satellite data file.
       */
   ComputeWindUp& ComputeWindUp::setFilename(const string& name)
   {

      fileData = name;
      satData.open(fileData);

      return (*this);

   }  // End of method 'ComputeWindUp::setFilename()'



      /* Compute the value of the wind-up, in radians.
       * @param sat       Satellite IDmake
       * @param time      Epoch of interest
       * @param satpos    Satellite position, as a Triple
       * @param sunpos    Sun position, as a Triple
       *
       * @return Wind-up computation, in radians
       */
   double ComputeWindUp::getWindUp( const SatID& satid,
                                    const CommonTime& time,
                                    const Triple& satPos,
                                    const Triple& sunPos )
   {

         // Get satellite rotation angle

         // Get vector from Earth mass center to receiver
      Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

         // Vector from SV to Sun center of mass
      Triple sat_sun( sunPos-satPos );

         // Define rk: Unitary vector from satellite to Earth mass center
      Triple rk( ( (-1.0)*(satPos.unitVector()) ) );

         // Define rj: rj = rk x sat_sun, then make sure it is unitary
      Triple rj( (rk.cross(sat_sun)).unitVector() );

         // Define ri: ri = rj x rk, then make sure it is unitary
         // Now, ri, rj, rk form a base in the satellite body reference
         // frame, expressed in the ECEF reference frame
      Triple ri( (rj.cross(rk)).unitVector() );


         // Compute unitary vector from satellite to receiver
      Triple rrho( (rxPos-satPos).unitVector() );

         // Projection of "rk" vector to line of sight vector (rrho)
      double zk(rrho.dot(rk));

         // Get a vector without components on rk (i.e., belonging
         // to ri, rj plane)
      Triple dpp(rrho-zk*rk);

         // Compute dpp components in ri, rj plane
      double xk(dpp.dot(ri));
      double yk(dpp.dot(rj));

         // Compute satellite rotation angle, in radians
      double alpha1(std::atan2(yk,xk));


         // Get receiver rotation angle

         // Redefine rk: Unitary vector from Receiver to Earth mass center
      rk = (-1.0)*(rxPos.unitVector());

         // Let's define a NORTH unitary vector in the Up, East, North
         // (UEN) topocentric reference frame
      Triple delta(0.0, 0.0, 1.0);

         // Rotate delta to XYZ reference frame
      delta =
         (delta.R2(nominalPos.geodeticLatitude())).R3(-nominalPos.longitude());


         // Computation of reference trame unitary vectors for receiver
         // rj = rk x delta, and make it unitary
      rj = (rk.cross(delta)).unitVector();

         // ri = rj x rk, and make it unitary
      ri = (rj.cross(rk)).unitVector();

         // Projection of "rk" vector to line of sight vector (rrho)
      zk = rrho.dot(rk);

         // Get a vector without components on rk (i.e., belonging
         // to ri, rj plane)
      dpp = rrho-zk*rk;

         // Compute dpp components in ri, rj plane
      xk = dpp.dot(ri);
      yk = dpp.dot(rj);

         // Compute receiver rotation angle, in radians
      double alpha2(std::atan2(yk,xk));

      double wind_up(0.0);

      alpha1 = alpha1 + wind_up;

      double da1(alpha1-m_satPhaseData[satid].satPreviousPhase);

      double da2(alpha2-m_satPhaseData[satid].staPreviousPhase);

         // Let's avoid problems when passing from 359 to 0 degrees.
      m_satPhaseData[satid].satPreviousPhase += std::atan2( std::sin(da1),
                                                            std::cos(da1) );

      m_satPhaseData[satid].staPreviousPhase += std::atan2( std::sin(da2),
                                                            std::cos(da2) );

         // Compute wind up effect in radians
      wind_up = m_satPhaseData[satid].satPreviousPhase -
                m_satPhaseData[satid].staPreviousPhase;

      return wind_up;

   }  // End of method 'ComputeWindUp::getWindUp()'



}  // End of namespace gpstk
