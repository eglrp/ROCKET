#pragma ident "$Id XY.CAO 2016-11-16 $"

/**
 * @file CodeGrossFilter.hpp
 * This class filters out satellites with psedoranges grossly out of bounds.
 * reference:
 * Gao Y, Lahaye F, Heroux P, et al. Modeling and estimation of C1¨CP1 bias in GPS 
 * receivers[J]. Journal of Geodesy, 2001, 74(9): 621-626.
 */

#ifndef GPSTK_CODEGROSSFILTER_HPP
#define GPSTK_CODEGROSSFILTER_HPP

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
//  Copywright(c) 2016, XY. CAO, Wuhan University 
//
//============================================================================



#include "ProcessingClass.hpp"


namespace gpstk
{


   class CodeGrossFilter : public ProcessingClass
   {
   public:

         /// Default constructor.
      CodeGrossFilter() : maxLimit(60.0)
      { };


         /** Returns a satTypeValueMap object, filtering the target
          *  observables.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException, SVNumException);


         /** Returns a gnnsSatTypeValue object, filtering the target
          *  observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException, SVNumException)
      { Process(gData.body); return gData; };



         /** Returns a gnnsRinex object, filtering the target observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException, SVNumException)
      { Process(gData.body); return gData; };



         /** Method to set the maximum limit.
          * @param max       Maximum limit (in meters).
          */
      virtual CodeGrossFilter& setMaxLimit(const double& max)
      { maxLimit = max; return (*this); };


         /// Method to get the maximum limit.
      virtual double getMaxLimit() const
      { return maxLimit; };


         // Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         // Destructor
      virtual ~CodeGrossFilter() {};


   protected:


         /** Checks that the value is within the given limits.
          * @param value     The value to be test
          *
          * @return
          *  True if check was OK.
          */
      virtual bool checkValue(const double& value) const
      { return ( ( std::abs(value) >= maxLimit ) ); };

         // Maximum value allowed for input data (in meters).
      double maxLimit;


   }; // End of class 'CodeGrossFilter'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_CODEGROSSFILTER_HPP
