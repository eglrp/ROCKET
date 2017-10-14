#pragma ident "$Id$"

/**
 * @file ComputeSatPCenter.hpp
 * This class computes the satellite antenna phase correction, in meters.
 */

#ifndef GPSTK_COMPUTESATPCENTER_HPP
#define GPSTK_COMPUTESATPCENTER_HPP

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
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "MSCStore.hpp"
#include "AntexReader.hpp"
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
       *      // Create the input obs file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *   gnssRinex gRin;
       *
       *   ComputeSatPCenter svPcenter( SP3EphList,
       *                                nominalPos );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> svPcenter;
       *   }
       * @endcode
       *
       * The "ComputeSatPCenter" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * satellite antenna phase correction, in meters.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the "satPCenter" TypeID inserted in it.
       * Be warned that if a given satellite does not have the required data,
       * it will be summarily deleted from the data structure.
       *
       * \warning The ComputeSatPCenter objects generate corrections that are
       * interpreted as an "advance" in the signal, instead of a delay.
       * Therefore, those corrections always hava a negative sign.
       *
       */
   class ComputeSatPCenter : public ProcessingClass
   {
   public:

         /// Default constructor
      ComputeSatPCenter()
         : pEphStore(NULL), nominalPos(0.0, 0.0, 0.0),
           pMSCStore(NULL), pAntexReader(NULL)
      { };


         /** Common constructor
          *
          * @param ephem     Satellite ephemeris.
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeSatPCenter( XvtStore<SatID>& ephStore,
                         const Position& staPos )
         : pEphStore(&ephStore), nominalPos(staPos),
           pAntexReader(NULL), pMSCStore(NULL)
      { };


         /** Common constructor
          *
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeSatPCenter( const Position& stapos )
         : pEphStore(NULL), nominalPos(stapos),
           pAntexReader(NULL), pMSCStore(NULL)
      { };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param ephem     Satellite ephemeris.
          * @param stapos    Nominal position of receiver station.
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeSatPCenter( XvtStore<SatID>& ephStore,
                         const Position& staPos,
                         AntexReader& antexObj )
         : pEphStore(&ephStore), nominalPos(staPos),
           pAntexReader(&antexObj), pMSCStore(NULL)
      { };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param stapos    Nominal position of receiver station.
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeSatPCenter( const Position& staPos,
                         AntexReader& antexObj )
         : pEphStore(NULL), nominalPos(staPos),
           pAntexReader(&antexObj), pMSCStore(NULL)
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


         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual ComputeSatPCenter& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns a pointer to the satellite ephemeris object
         /// currently in use.
      virtual XvtStore<SatID> *getEphStore(void) const
      { return pEphStore; };


         /** Sets satellite ephemeris object to be used.
          *
          * @param ephem     Satellite ephemeris object.
          */
      virtual ComputeSatPCenter& setEphStore(XvtStore<SatID>& ephStore)
      { pEphStore = &ephStore; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
      virtual MSCStore *getMSCStore(void) const
      { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param msc     MSCStore object.
          */
      virtual ComputeSatPCenter& setMSCStore(MSCStore& mscStore)
      { pMSCStore = &mscStore; return (*this); };


         /// Returns a pointer to the AntexReader object currently in use.
      virtual AntexReader *getAntexReader(void) const
      { return pAntexReader; };


         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
      virtual ComputeSatPCenter& setAntexReader(AntexReader& antexObj)
      { pAntexReader = &antexObj; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeSatPCenter() {};


   private:


         /// Pointer to XvtStore<SatID> object
      XvtStore<SatID> *pEphStore;

         /// Station position
      Position nominalPos;

         /// Pointer to MSCStore object
      MSCStore* pMSCStore;

         /// Pointer to AntexReader object
      AntexReader* pAntexReader;


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


   }; // End of class 'ComputeSatPCenter'

      //@}

}  // End of namespace gpstk

#endif // GPSTK_COMPUTESATPCENTER_HPP
