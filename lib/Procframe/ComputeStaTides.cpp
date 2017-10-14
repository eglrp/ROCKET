#pragma ident "$Id$"

/**
 * @file ComputeStaTides.cpp
 * This class computes station tides.
 */


#include "ComputeStaTides.hpp"

using namespace std;

namespace gpstk
{

    // Returns a string identifying this object.
    std::string ComputeStaTides::getClassName() const
    { return "ComputeStaTides"; }



      /** Returns a sourceVectorMap object which contains the tides.
       *
       * @param time      epoch.
       * @param sData     source position.
       */
    sourceVectorMap ComputeStaTides::Process( const CommonTime& time,
                                              const sourceVectorMap& sData )
        throw(ProcessingException)
    {

        try
        {
            sourceVectorMap sourceTides;
            Triple solidCorr, oceanCorr, poleCorr;
            Vector<double> corr(3,0.0);

            SourceID source;
            string station;
            Vector<double> data;

            for(sourceVectorMap::const_iterator it = sData.begin();
                it != sData.end();
                ++it)
            {
                source = it->first;
                station = source.sourceName;
                data = it->second;

                Position position(data(0),data(1),data(2));

                solidCorr = solid.getSolidTide(time,position);
                oceanCorr = ocean.getOceanLoading(station,time);
                poleCorr = pole.getPoleTide(time,position);

                // UEN
                corr(0) = solidCorr[0] + oceanCorr[0] + poleCorr[0];
                corr(1) = solidCorr[1] + oceanCorr[1] + poleCorr[1];
                corr(2) = solidCorr[2] + oceanCorr[2] + poleCorr[2];

                sourceTides[source] = corr;
            }

            return sourceTides;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );

            GPSTK_THROW(e);
        }

    }  // End of method 'ComputeStaTides::Process()'


}  // End of namespace gpstk
