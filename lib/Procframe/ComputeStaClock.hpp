#pragma ident "$Id: ComputeStaClock.hpp $"

/**
 * @file ComputeStaClock.hpp
 * This is a class to compute the station clock bias.
 */

#ifndef GPSTK_COMPUTE_STATION_CLOCK_HPP
#define GPSTK_COMPUTE_STATION_CLOCK_HPP

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
//  Kaifa Kuang, Wuhan University, 2017
//
//============================================================================


#include "ProcessingClass.hpp"
#include "DataStructures.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This class computes the station clock bias.
       *
       *  A typical way to use this class follows:
       *
       * @code
       *
       * @endcode
       *
       */
    class ComputeStaClock : public ProcessingClass
    {
    public:

        /// Default constructor.
        ComputeStaClock() {};


        /** Returns a satTypeValueMap object, adding the new data generated
         *  when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
            throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException);


         /** Returns a gnssRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException);


         /** Returns a gnssDataMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssDataMap& Process(gnssDataMap& gData)
            throw(ProcessingException);


        /// Get source clock map.
        virtual sourceValueMap getSourceClock() const
        { return sourceClock; };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor.
        virtual ~ComputeStaClock() {};


    private:

        /// If this source to be rejected
        bool sourceRejected;

        /// station clock
        double clock;

        /// station clock map
        sourceValueMap sourceClock;

    }; // End of class 'ComputeStaClock'

    //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTE_STATION_CLOCK_HPP
