#pragma ident "$Id$"

/**
 * @file ComputeSatPCenter2.hpp
 * This class computes the satellite antenna phase correction, in meters.
 */

#ifndef COMPUTE_SATPCENTER2_HPP
#define COMPUTE_SATPCENTER2_HPP

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



#include <cmath>
#include <string>
#include <sstream>
#include "ProcessingClass.hpp"
#include "SunPosition.hpp"
#include "AntexReader.hpp"
#include "MSCStore.hpp"
#include "StringUtils.hpp"
#include "constants.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes the satellite antenna phase correction, in meters.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       * @endcode
       *
       * The "ComputeSatPCenter2" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * satellite antenna phase correction, in meters.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the "satPCenter" TypeID inserted in it.
       * Be warned that if a given satellite does not have the required data,
       * it will be summarily deleted from the data structure.
       *
       * \warning The ComputeSatPCenter2 objects generate corrections that are
       * interpreted as an "advance" in the signal, instead of a delay.
       * Therefore, those corrections always hava a negative sign.
       *
       */
    class ComputeSatPCenter2 : public ProcessingClass
    {
    public:

         /// Default constructor
        ComputeSatPCenter2() : pAntexReader(NULL)
        { };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param antexReader  AntexReader object containing satellite
          *                     antenna data.
          */
        ComputeSatPCenter2( AntexReader& antexReader )
            : pAntexReader(&antexReader)
        { };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssDataMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssDataMap& Process(gnssDataMap& gData)
            throw(ProcessingException);


         /// Returns a pointer to the AntexReader object currently in use.
        virtual AntexReader *getAntexReader(void) const
        { return pAntexReader; };


         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
        virtual ComputeSatPCenter2& setAntexReader(AntexReader& antexReader)
        { pAntexReader = &antexReader; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
        virtual MSCStore *getMSCStore(void) const
        { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param mscStore     MSCStore object.
          */
        virtual ComputeSatPCenter2& setMSCStore(MSCStore& mscStore)
        { pMSCStore = &mscStore; return (*this); };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~ComputeSatPCenter2() {};


    private:

         /// Pointer to AntexReader object
        AntexReader* pAntexReader;

        /// Pointer to MSCStore object.
        MSCStore* pMSCStore;

        /// Station position.
        Position nominalPos;


         /** Compute the value of satellite antenna phase correction, in meters
          * @param satid     Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Satellite antenna phase correction, in meters.
          */
        virtual Vector<double> getSatPCenter( const SatID& satid,
                                              const CommonTime& time,
                                              const Triple& satpos,
                                              const Triple& sunpos );


    }; // End of class 'ComputeSatPCenter2'

      //@}

}  // End of namespace gpstk

#endif // COMPUTE_SATPCENTER_HPP
