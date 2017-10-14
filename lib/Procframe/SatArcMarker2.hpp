#pragma ident "$Id: SatArcMarker2.hpp 2513 2013-05-30 09:29:32Z shjzhang $"

/**
 * @file SatArcMarker2.hpp
 * This class keeps track of satellite arcs caused by cycle slips.
 */

#ifndef GPSTK_SATARCMARKER2_HPP
#define GPSTK_SATARCMARKER2_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
//============================================================================
//
//  Revision
//
//  2014/03/10      modify from 'SatArcMarker'
//
//
//============================================================================



#include "ProcessingClass.hpp"



namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This class keeps track of satellite arcs caused by cycle slips.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *   LICSDetector markCSLI;
       *   SatArcMarker2 markArc;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> markCSLI >> markArc;
       *   }
       * @endcode
       *
       * The "SatArcMarker2" object will visit every satellite in the GNSS
       * data structure that is "gRin" and, if a cycle slip has happened
       * (indicated by the corresponding CS flag), it will increase the
       * value of the corresponding "TypeID::satArc" type.
       *
       * By default, the "SatArcMarker2" objects will only watch the
       * "TypeID::CSL1" cycle slip flag. This may be changed, although it is
       * rarely necessary because CS detectors raise all flags when a cycle
       * slip happens.
       *
       * An important feature of "SatArcMarker2" objects is that you can
       * set a period after arc change when the affected satellite will be
       * considered "unstable". By default, unstable period is set to 31 s,
       * but it may be changed using the appropriate constructor or using
       * the "setUnstablePeriod()" method.
       *
       * Likewise, SatArcMarker2" objects are also able to delete unstable
       * satellites from GDS. This feature is disabled by default, but may
       * be activated using the appropriate constructor or using the
       * "setDeleteUnstableSats()".
       *
       * \warning Be aware that this class DOES NOT apply cycle slip detection
       * algorithms, so you MUST invoke CS detection objects BEFORE calling
       * SatArcMarker2 objects.
       *
       * \warning Objects in this class store their internal state, so you
       * MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       * @sa LICSDetector.hpp and MWCSDetector.hpp for CS detection classes.
       *
       */
   class SatArcMarker2 : public ProcessingClass
   {
      public:

         /// Default constructor. It will only watch "TypeID::CSL1" flag.
      SatArcMarker2()
         : watchCSFlag(TypeID::CSL1), deleteUnstableSats(false),
           unstablePeriod(31.0)
      {};


         /** Common constructor
          *
          * @param watchFlag        Cycle slip flag to be watched.
          * @param delUnstableSats  Whether unstable satellites will be deleted.
          * @param unstableTime     Number of seconds since last arc change
          *                         that a satellite will be considered as
          *                         unstable.
          */
      SatArcMarker2( const TypeID& watchFlag,
                    const bool delUnstableSats,
                    const double unstableTime );


         /// Method to get the default CS flag type being used.
      virtual TypeID getCSFlag() const
      { return watchCSFlag; };


         /** Method to set the default CS flag type to be used.
          *
          * @param watchFlag     Cycle slip flag to be watched.
          */
      virtual SatArcMarker2& setCSFlag(const TypeID& watchFlag)
      { watchCSFlag = watchFlag; return (*this); };


         /// Method to known if unstable satellites will be deleted.
      virtual bool getDeleteUnstableSat() const
      { return deleteUnstableSats; };


         /** Method to set if unstable satellites will be deleted.
          *
          * @param delUnstableSats  Whether unstable satellites will be deleted.
          */
      virtual SatArcMarker2& setDeleteUnstableSats(const bool delUnstableSats)
      { deleteUnstableSats = delUnstableSats; return (*this); };


         /// Method to get the number of seconds since last arc change that a
         /// satellite will be considered as unstable.
      virtual double getUnstablePeriod() const
      { return unstablePeriod; };


         /** Method to set the number of seconds since last arc change that a
          *  satellite will be considered as unstable.
          *
          * @param unstableTime     Number of seconds since last arc change
          *                         that a satellite will be considered as
          *                         unstable.
          */
      virtual SatArcMarker2& setUnstablePeriod(const double unstableTime);


         /** Method to get the arc changed epoch.
          * @param sat              Interested SatID.
          */
      virtual CommonTime getArcChangedEpoch(const SatID& sat);


         /** Method to get the arc changed epoch.
          * @param source           Interested SourceID.
          * @param sat              Interested SatID.
          */
      virtual CommonTime getArcChangedEpoch(const SourceID& source,
                                            const SatID& sat);


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& epoch,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a gnssRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /** Returns a gnssDataMap object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData)
         throw(ProcessingException);


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~SatArcMarker2() {};


   private:


         /// Cycle slip flag to be watched.
      TypeID watchCSFlag;


         /// Flag indicating if unstable satellites will be deleted.
      bool deleteUnstableSats;


         /// Number of seconds since arc change that a satellite will be
         /// considered as unstable.
      double unstablePeriod;


         /// A structure for holding Satellite Arc Data information
      struct arcData
      {
         arcData(): arcChangeTime(CommonTime::BEGINNING_OF_TIME),
                    arcNum(0)
         {};

         CommonTime arcChangeTime;
         double arcNum;
      };


      typedef std::map<SatID, arcData> SatArcData;
      typedef std::map<SourceID, SatArcData> SatArcDataMap;


         /// Map holding information regarding every satellite
      SatArcData m_satArcData;

         /// Map holding information for gnssDataMap
      SatArcDataMap m_satArcDataMap;


   }; // End of class 'SatArcMarker2'


   //@}

}  // End of namespace gpstk

#endif   // GPSTK_SATARCMARKER_HPP
