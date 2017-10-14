#pragma ident "$Id$"

/**
 * @file ComputeStaTides.hpp
 * This class computes station tides.
 */

#ifndef COMPUTE_STA_TIDES_HPP
#define COMPUTE_STA_TIDES_HPP


#include <string>

#include "ProcessingClass.hpp"
#include "SolidTides.hpp"
#include "OceanLoading.hpp"
#include "PoleTides.hpp"


namespace gpstk
{

    class ComputeStaTides
    {
    public:

         /// Default constructor
        ComputeStaTides() {};


         /** Returns a sourceVectorMap object which contains the tides.
          *
          * @param time      epoch.
          * @param sData     source position.
          */
        sourceVectorMap Process( const CommonTime& time,
                                 const sourceVectorMap& sData )
            throw(ProcessingException);


        ComputeStaTides& setBLQStore( BLQDataReader& blqStore )
        { ocean.setBLQStore(blqStore); return (*this); };


        ComputeStaTides& setEOPStore( EOPDataStore2& eopStore )
        { pole.setEOPStore(eopStore); return (*this); };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~ComputeStaTides() {};


    private:

        SolidTides solid;

        OceanLoading ocean;

        PoleTides pole;

    }; // End of class 'ComputeStaTides'

    //@}

}  // End of namespace gpstk

#endif  // COMPUTE_STA_TIDES_HPP
