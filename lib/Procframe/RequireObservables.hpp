#pragma ident "$Id$"

/**
 * @file RequireObservables.hpp
 * This class filters out satellites with observations grossly out of bounds.
 */

#ifndef GPSTK_REQUIREOBSERVABLES_HPP
#define GPSTK_REQUIREOBSERVABLES_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//============================================================================
//  Revision
//
//  2016/06/08, add the TypeIDSet for Glonass, Galileo and BeiDou
//
//
//============================================================================

#include "ProcessingClass.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class filters out satellites that don't have the required
       *  observations.
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
       *
       *   TypeIDSet requiredSet;
       *   requiredSet.insert(TypeID::C1);
       *   requiredSet.insert(TypeID::C2);
       *
       *   RequireObservables requireTypes(requiredSet);
       *
       *   while(rin >> gRin)
       *   {
       *         // Satellites without P1 and P2 observations will be deleted
       *      gRin >> requireTypes;
       *   }
       * @endcode
       *
       * The "RequireObservables" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will check that the previously
       * given list of observation types is meet by each satellite.
       *
       * Be warned that if a given satellite does not have ALL the observations
       * required, the full satellite record will be summarily deleted from the
       * data structure.
       *
       */
   class RequireObservables : public ProcessingClass
   {
   public:

         /// Default constructor.
      RequireObservables()
      { };


         /** Explicit constructor
          *
          * @param sys       System to be required.
          * @param type      TypeID to be required.
          */
      RequireObservables(const SatID::SatelliteSystem& sys,
                         const TypeID& type)
      { setRequiredType(sys,type); };


         /** Explicit constructor
          *
          * @param typeSet   Set of TypeID's to be required.
          */
      RequireObservables(const SatID::SatelliteSystem& sys,
                         const TypeIDSet& typeSet)
      {
          if(sys == SatID::systemGPS)
          {
              requiredTypeSetOfGPS = typeSet;
          }
          else if(sys == SatID::systemGLONASS)
          {
              requiredTypeSetOfGLO = typeSet;
          }
          else if(sys == SatID::systemGalileo)
          {
              requiredTypeSetOfGAL = typeSet;
          }
          else if(sys == SatID::systemBDS)
          {
              requiredTypeSetOfBDS = typeSet;
          }
      };


         /** Returns a satTypeValueMap object, checking the required
          *  observables.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException);


         /** Method to add a TypeID to be required.
          *
          * @param type      Extra TypeID to be required.
          */
      virtual RequireObservables& addRequiredType(const SatID::SatelliteSystem& sys,
                                                  const TypeID& type)
      {
          if(sys == SatID::systemGPS)
          {
              requiredTypeSetOfGPS.insert(type);
          }
          else if(sys == SatID::systemGLONASS)
          {
              requiredTypeSetOfGLO.insert(type);
          }
          else if(sys == SatID::systemGalileo)
          {
              requiredTypeSetOfGAL.insert(type);
          }
          else if(sys == SatID::systemBDS)
          {
              requiredTypeSetOfBDS.insert(type);
          }

          return (*this);
      };


         /** Method to add a set of TypeID's to be required.
          *
          * @param typeSet    Set of TypeID's to be required.
          */
      virtual RequireObservables& addRequiredType(const SatID::SatelliteSystem& sys,
                                                  const TypeIDSet& typeSet);


         /** Method to set a TypeID to be required. This method will erase
          *  previous required types.
          *
          * @param type      TypeID to be required.
          */
      virtual RequireObservables& setRequiredType(const SatID::SatelliteSystem& sys,
                                                  const TypeID& type)
      {
          if(sys == SatID::systemGPS)
          {
              requiredTypeSetOfGPS.clear();
              requiredTypeSetOfGPS.insert(type);
          }
          else if(sys == SatID::systemGLONASS)
          {
              requiredTypeSetOfGLO.clear();
              requiredTypeSetOfGLO.insert(type);
          }
          else if(sys == SatID::systemGalileo)
          {
              requiredTypeSetOfGAL.clear();
              requiredTypeSetOfGAL.insert(type);
          }
          else if(sys == SatID::systemBDS)
          {
              requiredTypeSetOfBDS.clear();
              requiredTypeSetOfBDS.insert(type);
          }

          return (*this);
      };


         /** Method to set the TypeID's to be required. This method will erase
          *  previous types.
          *
          * @param typeSet       Set of TypeID's to be required.
          */
      virtual RequireObservables& setRequiredType(const SatID::SatelliteSystem& sys,
                                                  const TypeIDSet& typeSet)
      {
          if(sys == SatID::systemGPS)
          {
              requiredTypeSetOfGPS.clear();
              requiredTypeSetOfGPS = typeSet;
          }
          else if(sys == SatID::systemGLONASS)
          {
              requiredTypeSetOfGLO.clear();
              requiredTypeSetOfGLO = typeSet;
          }
          else if(sys == SatID::systemGalileo)
          {
              requiredTypeSetOfGAL.clear();
              requiredTypeSetOfGAL = typeSet;
          }
          else if(sys == SatID::systemBDS)
          {
              requiredTypeSetOfBDS.clear();
              requiredTypeSetOfBDS = typeSet;
          }

          return (*this);
      };


         /// Method to get the set of TypeID's to be required.
      virtual TypeIDSet getRequiredType(const SatID::SatelliteSystem& sys) const
      {
          TypeIDSet ret;

          if(sys == SatID::systemGPS)
          {
              ret = requiredTypeSetOfGPS;
          }
          else if(sys == SatID::systemGLONASS)
          {
              ret = requiredTypeSetOfGLO;
          }
          else if(sys == SatID::systemGalileo)
          {
              ret = requiredTypeSetOfGAL;
          }
          else if(sys == SatID::systemBDS)
          {
              ret = requiredTypeSetOfBDS;
          }

          return ret;
      };


         /** Returns a gnssSatTypeValue object, checking the required
          *  observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };



         /** Returns a gnssRinex object, checking the required observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Returns a gnssDataMap object, checking the required observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData)
         throw(ProcessingException);


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~RequireObservables() {};


   private:

         /// Set of types to be required
      TypeIDSet requiredTypeSetOfGPS;
      TypeIDSet requiredTypeSetOfGLO;
      TypeIDSet requiredTypeSetOfGAL;
      TypeIDSet requiredTypeSetOfBDS;

   }; // End of class 'RequireObservables'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_REQUIREOBSERVABLES_HPP
