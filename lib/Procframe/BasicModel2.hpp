#pragma ident "$Id$"

/**
 * @file BasicModel2.hpp
 */

#ifndef BASIC_MODEL2_HPP
#define BASIC_MODEL2_HPP

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



#include "ProcessingClass.hpp"
#include "ReferenceSystem.hpp"
#include "MSCStore.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

    class BasicModel2 : public ProcessingClass
    {
    public:

        /// Default constructor.
        BasicModel2()
            : minElev(10.0), pRefSys(NULL)
        {};


        /// Explicit constructor.
        BasicModel2( double elevation,
                     const satVectorMap& state,
                     ReferenceSystem& refSys,
                     MSCStore& mscStore )
        {
            minElev = elevation;
            satState = state;
            pRefSys = &refSys;
            pMSCStore = &mscStore;
        };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssDataMap object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssDataMap& Process(gnssDataMap& gData)
            throw(ProcessingException);


         /// Method to get cut-off elevation. Default 10 degrees.
        virtual double getMinElev() const
        { return minElev; };


         /// Method to set cut-off elevation. Default 10 degrees.
        virtual BasicModel2& setMinElev(double newElevation)
        { minElev = newElevation; return (*this); };



         /// Method to get sat state to be used.
        virtual satVectorMap getSatState() const
        { return satState; };


         /** Method to set the sat orbit to be used.
          *
          * @param state    satVectorMap object to be used
          */
        virtual BasicModel2& setSatState(const satVectorMap& state)
        { satState = state; return (*this); };


         /// Method to get source clock to be used.
        virtual sourceValueMap getSourceClock() const
        { return sourceClock; };


         /** Method to set the station clock to be used.
          *
          * @param clockStore   sourceValueMap object to be used
          */
        virtual BasicModel2& setSourceClock(const sourceValueMap& clockStore)
        { sourceClock = clockStore; return (*this); };


         /// Method to get a pointer to the ReferenceSystem to be used
         /// with GNSS data structures.
        virtual ReferenceSystem* getReferenceSystem() const
        { return pRefSys; };


         /** Method to set the ReferenceSystem to be used with GNSS
          *  data structures.
          *
          * @param refSys     ReferenceSystem object to be used
          */
        virtual BasicModel2& setReferenceSystem(ReferenceSystem& refSys)
        { pRefSys = &refSys; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
        virtual MSCStore *getMSCStore(void) const
        { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param msc  MSCStore object.
          */
        virtual BasicModel2& setMSCStore(MSCStore& msc)
        { pMSCStore = &msc; return (*this); };


        /// Get satellite pos in ECI.
        virtual satVectorMap getSatPosECI() const
        { return satPosECI; };


        /// Get satellite pos in ECEF.
        virtual satVectorMap getSatPosECEF() const
        { return satPosECEF; };


        /// Get source pos in ECI.
        virtual sourceVectorMap getSourcePosECI() const
        { return sourcePosECI; };


        /// Get source pos in ECEF.
        virtual sourceVectorMap getSourcePosECEF() const
        { return sourcePosECEF; };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor.
        virtual ~BasicModel2() {};


    protected:

        /// Cut-off elevation, default 10 degrees
        double minElev;

        /// Satellite Orbit
        satVectorMap satState;

        satVectorMap satPosECI;
        satVectorMap satPosECEF;

        Vector<double> posSatECI;
        Vector<double> posSatECEF;

        Vector<double> velSatECI;
        Vector<double> velSatECEF;

        /// Source Clock
        sourceValueMap sourceClock;

        double cdtSource;

        sourceVectorMap sourcePosECI;
        sourceVectorMap sourcePosECEF;

        Vector<double> posSourceECI;
        Vector<double> posSourceECEF;

        /// ReferenceSystem
        ReferenceSystem* pRefSys;

        /// Source Position
        MSCStore* pMSCStore;

        Position posSource;

    }; // End of class 'BasicModel2'

    //@}

}  // End of namespace gpstk

#endif   // BASIC_MODEL2_HPP
