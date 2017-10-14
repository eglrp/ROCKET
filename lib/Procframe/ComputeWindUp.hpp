#pragma ident "$Id$"

/**
 * @file ComputeWindUp.hpp
 * This class computes the wind-up effect on the phase observables, in radians.
 */

#ifndef GPSTK_COMPUTEWINDUP_HPP
#define GPSTK_COMPUTEWINDUP_HPP

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
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "MSCStore.hpp"
#include "SatDataReader.hpp"
#include "AntexReader.hpp"
#include "constants.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes the wind-up effect on the phase observables,
       *  in radians.
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
       *   ComputeWindUp windup( SP3EphList,
       *                         nominalPos );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> windup;
       *   }
       * @endcode
       *
       * The "ComputeWindUp" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * receiver-satellite wind-up effect, in radians.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the wind-up inserted in it. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       * \warning ComputeWindUp objects store their internal state, so
       * you MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       * \warning It is recommended that ComputeWindUp objects are used after
       * calling a SatArcMarker object, because they work better when cycle
       * slips are properly tracked.
       *
       */
   class ComputeWindUp : public ProcessingClass
   {
   public:

         /// Default constructor
      ComputeWindUp()
         : pEphStore(NULL), nominalPos(0.0, 0.0, 0.0), pMSCStore(NULL),
           satData("PRN_GPS"), fileData("PRN_GPS"), pAntexReader(NULL)
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
      ComputeWindUp( XvtStore<SatID>& ephStore,
                     const Position& staPos,
                     std::string filename="PRN_GPS" )
         : pEphStore(&ephStore), nominalPos(staPos),
           pMSCStore(NULL), satData(filename),
           fileData(filename), pAntexReader(NULL)
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
      ComputeWindUp( XvtStore<SatID>& ephStore,
                     const Position& staPos,
                     MSCStore& mscStore,
                     AntexReader& antexObj )
         : pEphStore(&ephStore), nominalPos(staPos),
           pMSCStore(&mscStore), pAntexReader(&antexObj)
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
      ComputeWindUp( const Position& staPos,
                     AntexReader& antexObj )
         : pEphStore(NULL), nominalPos(staPos),
           pMSCStore(NULL), pAntexReader(&antexObj)
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


         /// Returns name of "PRN_GPS"-like file containing satellite data.
      virtual std::string getFilename(void) const
      { return fileData; };


         /** Sets name of "PRN_GPS"-like file containing satellite data.
          * @param name      Name of satellite data file.
          */
      virtual ComputeWindUp& setFilename(const std::string& name);


         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets  nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual ComputeWindUp& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns a pointer to the satellite ephemeris object
         /// currently in use.
      virtual XvtStore<SatID> *getEphStore(void) const
      { return pEphStore; };


         /** Sets satellite ephemeris object to be used.
          * @param ephem     Satellite ephemeris object.
          */
      virtual ComputeWindUp& setEphStore(XvtStore<SatID>& ephStore)
      { pEphStore = &ephStore; return (*this); };


         /// Returns a pointer to the AntexReader object currently in use.
      virtual AntexReader *getAntexReader(void) const
      { return pAntexReader; };


         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
      virtual ComputeWindUp& setAntexReader(AntexReader& antexObj)
      { pAntexReader = &antexObj; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
      virtual MSCStore *getMSCStore(void) const
      { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param msc  MSCStore object.
          */
      virtual ComputeWindUp& setMSCStore(MSCStore& msc)
      { pMSCStore = &msc; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeWindUp() {};


   private:


         /// Satellite ephemeris to be used
      XvtStore<SatID> *pEphStore;


         /// Receiver position
      Position nominalPos;


         /// Pointer to MSCStore object
      MSCStore* pMSCStore;


         /// Object to read satellite data file (PRN_GPS)
      SatDataReader satData;


         /// Pointer to object containing satellite antenna data, if available.
      AntexReader* pAntexReader;


         /// Name of "PRN_GPS"-like file containing satellite data.
      std::string fileData;


         /// A structure used to store phase data.
      struct phaseData
      {
            // Default constructor initializing the data in the structure
         phaseData() : satPreviousPhase(0.0),
                       staPreviousPhase(0.0),
                       arcNum(0.0)
         {};

         double satPreviousPhase;      ///< Previous phase for satellite.
         double staPreviousPhase;      ///< Previous phase for station.
         double arcNum;                ///< Satellite arc number.
      };


      typedef std::map<SatID, phaseData> SatPhaseData;
      typedef std::map<SourceID, SatPhaseData> SatPhaseDataMap;

         /// Map to store satellite phase data
      SatPhaseData m_satPhaseData;

         /// Map to store station phase date
      SatPhaseDataMap m_satPhaseDataMap;


         /** Compute the value of the wind-up, in radians.
          * @param sat       Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Wind-up computation, in radians
          */
      virtual double getWindUp( const SatID& sat,
                                const CommonTime& time,
                                const Triple& satpos,
                                const Triple& sunpos );


   }; // End of class 'ComputeWindUp'

      //@}

}  // End of namespace gpstk

#endif // GPSTK_COMPUTEWINDUP_HPP
