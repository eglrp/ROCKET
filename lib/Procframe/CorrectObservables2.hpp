#pragma ident "$Id$"

/**
 * @file CorrectObservables2.hpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
 */

#ifndef CORRECT_OBSERVABLES2_HPP
#define CORRECT_OBSERVABLES2_HPP

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



#include <string>

#include "ProcessingClass.hpp"
#include "MSCStore.hpp"
#include "Antenna.hpp"
#include "ComputeTides.hpp"
#include "constants.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class corrects observables from effects such as antenna
       *  excentricity, difference in phase centers, offsets due to
       *  tidal effects, etc.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       * @endcode
       *
       * The "CorrectObservables2" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will correct the
       * corresponding observables from the given effects.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the observables corrected. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       */
    class CorrectObservables2 : public ProcessingClass
    {
    public:

         /// Default constructor
        CorrectObservables2()
            : pMSCStore(NULL), useAzimuth(false),
              L1PhaseCenter(0.0, 0.0, 0.0), L2PhaseCenter(0.0, 0.0, 0.0),
              L3PhaseCenter(0.0, 0.0, 0.0), L5PhaseCenter(0.0, 0.0, 0.0),
              L6PhaseCenter(0.0, 0.0, 0.0), L7PhaseCenter(0.0, 0.0, 0.0),
              L8PhaseCenter(0.0, 0.0, 0.0), L9PhaseCenter(0.0, 0.0, 0.0),
              monument(0.0, 0.0, 0.0), tide(0.0, 0.0, 0.0)
        {};


         /** Common constructor
          *
          * @param mscData  MSCStore.
          *
          */
        CorrectObservables2( MSCStore& mscStore )
            : pMSCStore(&mscStore), useAzimuth(true),
              L1PhaseCenter(0.0, 0.0, 0.0), L2PhaseCenter(0.0, 0.0, 0.0),
              L3PhaseCenter(0.0, 0.0, 0.0), L5PhaseCenter(0.0, 0.0, 0.0),
              L6PhaseCenter(0.0, 0.0, 0.0), L7PhaseCenter(0.0, 0.0, 0.0),
              L8PhaseCenter(0.0, 0.0, 0.0), L9PhaseCenter(0.0, 0.0, 0.0),
              monument(0.0, 0.0, 0.0), tide(0.0, 0.0, 0.0)
        {};


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssDataMap object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssDataMap& Process(gnssDataMap& gData)
            throw(ProcessingException);


         /// Returns whether azimuth-dependent antenna patterns are being used.
         /// When an Antenna is set, this parameter is true by default.
        virtual bool getUseAzimuth(void) const
        { return useAzimuth; };


         /** Sets whether azimuth-dependent antenna patterns will be used.
          *
          * @param useAzimuthPattern   Whether azimuth patterns will be used.
          */
        virtual CorrectObservables2& setUseAzimuth(bool useAzimuthPattern)
        { useAzimuth = useAzimuthPattern; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
        virtual MSCStore *getMSCStore(void) const
        { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param mscStore     MSCStore object.
          */
        virtual CorrectObservables2& setMSCStore(MSCStore& mscStore)
        { pMSCStore = &mscStore; return (*this); };


         /// Returns a pointer to the sourceMonumentMap object currently
         /// in use.
        virtual std::map<SourceID,Triple> getSourceMonument(void) const
        { return sourceMonument; };


         /** Sets sourceMonumentMap object to be used.
          *
          * @param monument     sourceMonumentMap object.
          */
        virtual CorrectObservables2& setSourceMonument(
                                const std::map<SourceID,Triple>& monument )
        { sourceMonument = monument; return (*this); };


         /// Returns a pointer to the sourceAntennaMap object currently
         /// in use.
        virtual std::map<SourceID,Antenna> getSourceAntenna(void) const
        { return sourceAntenna; };


         /** Sets sourceAntennaMap object to be used.
          *
          * @param antenna     sourceAntennaMap object.
          */
        virtual CorrectObservables2& setSourceAntenna(
                                const std::map<SourceID,Antenna>& antenna )
        { sourceAntenna = antenna; return (*this); };


         /** Sets ComputeTides object to be used.
          *
          * @param tides     ComputeTides object.
          */
        virtual CorrectObservables2& setTideCorr( const ComputeTides& tides )
        { tideCorr = tides; return (*this); };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~CorrectObservables2() {};


    private:

        /// Pointer to MSCStore object.
        MSCStore* pMSCStore;

        /// Station position.
        Position nominalPos;

        /// Station monument map.
        std::map<SourceID,Triple> sourceMonument;

        /// Station monument.
        Triple monument;

        /// Station antenna map.
        std::map<SourceID,Antenna> sourceAntenna;

        /// Station antenna.
        Antenna antenna;

         /// Correct tide effects.
        ComputeTides tideCorr;

         /// Station tidal displacement.
        Triple tide;

         /// Whether azimuth-dependent antenna patterns will be used or not
        bool useAzimuth;

         /// Position of antenna L1 phase center with respect to ARP ([UEN]).
        Triple L1PhaseCenter;

         /// Position of antenna L2 phase center with respect to ARP ([UEN]).
        Triple L2PhaseCenter;

         /// Position of antenna L3 phase center with respect to ARP ([UEN]).
        Triple L3PhaseCenter;

         /// Position of antenna L5 phase center with respect to ARP ([UEN]).
        Triple L5PhaseCenter;

         /// Position of antenna L6 phase center with respect to ARP ([UEN]).
        Triple L6PhaseCenter;

         /// Position of antenna L7 phase center with respect to ARP ([UEN]).
        Triple L7PhaseCenter;

         /// Position of antenna L8 phase center with respect to ARP ([UEN]).
        Triple L8PhaseCenter;

         /// Position of antenna L9 phase center with respect to ARP ([UEN]).
        Triple L9PhaseCenter;

    }; // End of class 'CorrectObservables2'

    //@}

}  // End of namespace gpstk

#endif  // CORRECT_OBSERVABLES_HPP
