#pragma ident "$Id: ComputeStaClock.cpp $"

/**
 * @file ComputeStaClock.cpp
 * This is a class to compute the station clock bias.
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
//  Kaifa Kuang - Wuhan University. 2017
//
//============================================================================


#include "ComputeStaClock.hpp"

using namespace std;

namespace gpstk
{

      // Returns a string identifying this object.
    std::string ComputeStaClock::getClassName() const
    { return "ComputeStaClock"; }


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
    gnssSatTypeValue& ComputeStaClock::Process(gnssSatTypeValue& gData)
        throw(ProcessingException)
    {
        try
        {
            // Build a gnssRinex object and fill it with data
            gnssRinex g1;
            g1.header = gData.header;
            g1.body = gData.body;

            // Call the Process() method with the appropriate input object
            Process(g1);

            // Update the original gnssSatTypeValue object with the results
            gData.body = g1.body;

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + u.what() );
            GPSTK_THROW(e);
        }
    }

      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
    gnssRinex& ComputeStaClock::Process(gnssRinex& gData)
        throw(ProcessingException)
    {

        try
        {
            Process(gData.body);

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + u.what() );
            GPSTK_THROW(e);
        }
    }


     /** Returns a gnssDataMap object, adding the new data generated
      *  when calling this object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& ComputeStaClock::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

        SourceID source;

        sourceClock.clear();

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                source = sdmIt->first;

                sourceRejected = false;

                Process( sdmIt->second );

                if(sourceRejected)
                {
                    sourceRejectedSet.insert(source);
                    continue;
                }
                else
                {
                    sourceClock[source] = clock;
                }
            }
        }

        gData.removeSourceID(sourceRejectedSet);

        return gData;

    }  // End of method 'ComputeStaClock::Process()'


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
    satTypeValueMap& ComputeStaClock::Process(satTypeValueMap& gData)
        throw(ProcessingException)
    {

        try
        {
            SatID sat;
            TypeID type1(TypeID::prefitCWithSatClock);
            TypeID type2(TypeID::weightC);

            Vector<SatID> allSat( gData.getVectorOfSatID() );
            int numSat( gData.numSats() );

            if(numSat < 4)
            {
                sourceRejected = true;
                return gData;
            }

            Vector<double> prefit( gData.getVectorOfTypeID(type1) );
            Vector<double> weight( gData.getVectorOfTypeID(type2) );

            clock = dot(weight,prefit)/sum(weight);

            Vector<double> staClks(numSat,clock);

            gData.insertTypeIDVector(TypeID::cdtSta, staClks);

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + u.what() );

            GPSTK_THROW(e);
        }

    }  // End of method 'ComputeStaClock::Process()'

}  // End of namespace gpstk
