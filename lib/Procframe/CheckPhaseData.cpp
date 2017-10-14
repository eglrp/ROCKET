#pragma ident "$Id: CheckPhaseData.cpp $"

/**
 * @file CheckPhaseData.cpp
 * This is a class to check the phase data.
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


#include "CheckPhaseData.hpp"

using namespace std;

namespace gpstk
{

      // Returns a string identifying this object.
   std::string CheckPhaseData::getClassName() const
   { return "CheckPhaseData"; }


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
   gnssSatTypeValue& CheckPhaseData::Process(gnssSatTypeValue& gData)
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
   gnssRinex& CheckPhaseData::Process(gnssRinex& gData)
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
    gnssDataMap& CheckPhaseData::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                Process( sdmIt->second );

                if(sourceRejected)
                {
                    sourceRejectedSet.insert( sdmIt->first );
                    sourceRejected = false;
                }
            }
        }

        gData.removeSourceID( sourceRejectedSet );

        return gData;

    }  // End of method 'CheckPhaseData::Process()'


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
    satTypeValueMap& CheckPhaseData::Process(satTypeValueMap& gData)
        throw(ProcessingException)
    {

        try
        {
            SatID sat;
            TypeID type1(TypeID::prefitLWithSatClock);
            TypeID type2(TypeID::weightL);

            bool isValid;

            Vector<SatID> allSat;
            int numSat;

            double solution;

            while( true )
            {
                isValid = true;

                allSat = gData.getVectorOfSatID();

                numSat = allSat.size();

                if(numSat <= 4)
                {
                    sourceRejected = true; break;
                }

                Vector<double> prefit(numSat,0.0);
                Vector<double> weight(numSat,0.0);

                for(int i=0; i<numSat; ++i)
                {
                    sat = allSat(i);

                    prefit(i) = gData.getValue(sat, type1);
                    weight(i) = gData.getValue(sat, type2);
                }

                solution = dot(weight,prefit)/sum(weight);
                Vector<double> postfit = prefit - solution;

                double sigma(0.0);

                for(int i=0; i<numSat; ++i)
                {
                    sigma += std::pow( postfit(i), 2 ) * weight(i);
                }

                sigma = std::sqrt( sigma / (numSat-1) );

                double v(0.0);

                for(int i=0; i<numSat; ++i)
                {
                    sat = allSat(i);

                    v = std::sqrt(weight(i)) * std::fabs(postfit(i));

                    if( v > 2.5*sigma )
                    {
                        isValid = false;
                        gData.removeSatID(sat);
                    }
                }

                if( isValid ) break;

            } // End of 'while( true )'

            return gData;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + u.what() );

            GPSTK_THROW(e);
        }

   }  // End of method 'CheckPhaseData::Process()'


}  // End of namespace gpstk
