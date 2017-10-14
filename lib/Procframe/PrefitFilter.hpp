#pragma ident "$Id$"

/**
 * @file PrefitFilter.hpp
 * This class filters out satellites, the prefit residual of which exceeds
 * the predefined threshold.
 */

#ifndef PREFIT_FILTER_HPP
#define PREFIT_FILTER_HPP

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
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#include "ProcessingClass.hpp"
#include "Stats.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class filters out satellites with prefit-residual grossly out of
       *  threshold.
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
       *   ....
       *
       *   BasicModel basic(nominalPos, SP3EphList);
       *
       *   ComputeLinear linear2(comb.pcCombination);
       *
       *   ComputeLinear prefitResidual(comb.pcPrefit);
       *
       *   PrefitFilter myFilter;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> basic >> linear2 >> prefitResidual >> myFilter;
       *   }
       * @endcode
       *
       * The "PrefitFilter" object will visit every satellite in the GNSS data
       * structure that is "gRin" and then taking the median values of all the
       * reciever clock error as the "true" receiver clock,  after that the 'true'
       * prefit-residual are computed by back-substituting the dt, and if the value
       * of the 'true' prefit-residual exceeds the thesold, the satellite will be
       * deleted.
       *
       * Be warned that if a given satellite does not have the observations
       * required, or if the true prefit-residual are out of threshold, the full
       * satellite record will be summarily deleted from the data structure.
       *
       * @author Shoujian Zhang
       *
       */
   class PrefitFilter : public ProcessingClass
   {
   public:

         /// Default constructor.
      PrefitFilter()
         : codeType(TypeID::prefitC), threshold(15.0)
      {};


         /** Explicit constructor
          *
          * @param type      TypeID to be filtered.
          * @param min       Minimum limit (in meters).
          * @param max       Maximum limit (in meters).
          */
      PrefitFilter( TypeID& codeT,
                    const double& limit )
         : codeType(codeT), threshold(limit)
      {};


         /** Returns a satTypeValueMap object, filtering the target
          *  observables.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, filtering the target
          *  observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };



         /** Returns a gnssRinex object, filtering the target observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Returns a gnssDataMap object, filtering the target observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData)
         throw(ProcessingException);


         /** Method to set the threshold value.
          * @param threshold       threshold value (in meters).
          */
      virtual PrefitFilter& setThreshold(const double& limit)
      { threshold = limit; return (*this); };


         /// Method to get the threshold.
      virtual double getThreshold() const
      { return threshold; };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~PrefitFilter() {};


   private:


         /// Type of code observation to be used.
      TypeID codeType;


         /// threshold (in meters).
      double threshold;

   }; // End of class 'PrefitFilter'

      //@}

}
#endif  // PREFITRESFILTERPC_HPP
