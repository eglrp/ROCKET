#pragma ident "$Id: $"

/**
 * @file ComputeElevWeights.hpp
 * This class computes satellites weights simply based on the elevation .
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
//  Modifications
//  -------------
//
//  2012.06.07  Create this program
//
//  Copyright
//  ---------
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include "ComputeElevWeights.hpp"
#include "constants.hpp"


namespace gpstk
{

    // Returns a string identifying this object.
    std::string ComputeElevWeights::getClassName() const
    { return "ComputeElevWeights"; }



      /* Returns a satTypeValueMap object, adding the new data
       * generated when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeElevWeights::Process( const CommonTime& time,
                                                  satTypeValueMap& gData )
        throw(ProcessingException)
    {

        try
        {

            SatIDSet satRejectedSet;

            SatID sat;

            // Loop through all the satellites
            for( satTypeValueMap::iterator it = gData.begin();
                 it != gData.end();
                 ++it )
            {
                sat = it->first;

                double elev( 0.0 );

                try
                {
                    elev = gData.getValue( sat, TypeID::elevation );
                }
                catch(...)
                {
                    satRejectedSet.insert( sat );
                    continue;
                }

                double weight;

                // Compute the weight according to elevation
                if( elev > 30.0 )
                {
                    weight = 1.0;
                }
                else
                {
                    // Transform the unit of elevation from degree to radian
                    elev = elev*DEG_TO_RAD;

                    // Compute the elevation weight
                    weight = std::pow(2.0*std::sin(elev), expotential);
                }

                if(sat.system == SatID::systemGPS)
                    weight *= weightOfGPS;
                else if(sat.system == SatID::systemGLONASS)
                    weight *= weightOfGLO;
                else if(sat.system == SatID::systemGalileo)
                    weight *= weightOfGAL;
                else if(sat.system == SatID::systemBDS)
                    weight *= weightOfBDS;

                (*it).second[TypeID::weightC] = weight;
                (*it).second[TypeID::weightL] = weight*1e4;

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

    }  // End of method 'ComputeElevWeights:Process()'



      /* Returns a gnssSatTypeValue object, adding the new data
       * generated when calling this object.
       *
       * @param gData    Data object holding the data.
       */
    gnssSatTypeValue& ComputeElevWeights::Process(gnssSatTypeValue& gData)
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

    }  // End of method 'ComputeElevWeights::Process()'



      /* Returns a gnssRinex object, adding the new data generated
       * when calling this object.
       *
       * @param gData    Data object holding the data.
       */
    gnssRinex& ComputeElevWeights::Process(gnssRinex& gData)
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

    }  // End of method 'ComputeElevWeights::Process()'



     /** Returns a gnssDataMap object, adding the new data generated
      *  when calling this object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& ComputeElevWeights::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                Process( gdmIt->first, sdmIt->second );
            }
        }

        return gData;

    }  // End of method 'ComputeElevWeights::Process()'


}  // End of namespace gpstk
