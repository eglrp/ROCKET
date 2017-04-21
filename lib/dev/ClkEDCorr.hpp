#pragma ident "$Id$"

/**
 * @file ClkEDCorr.hpp
 * This class check carrier phase and pesudorange
 * difference by epoch-difference.
 */

#ifndef GPSTK_CLK_ED_CORR_HPP
#define GPSTK_CLK_ED_CORR_HPP

//============================================================================
//
//  This file is part of ROCKET, the Real-time Orbit determination and ClocK 
//  estimation Toolkit. The ROCKET software is based on some open-source 
//  software, including GPSTK, GAMIT and RTKLIB and some other open-source 
//  resources. 
//
//  Until now, the Copyright follows the GNU Lesser General Public License 
//  as published by the Free Software Foundation; either version 2.1 of the 
//  License, or any later version.  You should have received a copy of the 
//  GNU Lesser General Public License.
//
//  Copyright(c)
//
//  Gao Kang, Wuhan University, 2017
//
//============================================================================
//
//  2017/03/26      Create this routine.
//
//============================================================================


#include "DataStructures.hpp"
#include "StateStore.hpp"
#include "ProcessingClass.hpp"
#include "Rinex3EphemerisStore.hpp"

namespace gpstk
{

   
   /** @addtogroup DataStructures */
   //@{

   class ClkEDCorr : public ProcessingClass
   {
     public:

      /// Default constructor for ClkPRCorr
      ClkEDCorr() 
         :m_pStateStore(NULL),m_pNavEphStore(NULL)
      {}    
       

      /**
       * set {@link StateStore} object. 
       */
      virtual ClkEDCorr& setStateStore( StateStore& stateStore )
      { m_pStateStore = &stateStore; return (*this); }
      
      /**
       * set {@link Rinex3EphemerisStore} object.
       */
      virtual ClkEDCorr& setNavEphStore( Rinex3EphemerisStore& navEphStore )
      { m_pNavEphStore = &navEphStore; return (*this);}
	
      /**
       * process satTypeValueMap.
       */
      virtual satTypeValueMap& Process( const CommonTime& time, satTypeValueMap& gData )
	    throw(ProcessingException);


      /// process gnssDataMap.
      virtual gnssDataMap& Process( gnssDataMap& gData )
         throw(ProcessingException);


      /// get class name.
      virtual std::string getClassName(void) const;
	
      
      /// destructor.
      virtual ~ClkEDCorr(){}

    private:  


         // holding data for last epoch.
         sourceDataMap mLastEpochData;

         // object to store navigation ephemeris
         Rinex3EphemerisStore* m_pNavEphStore;

         // object to store state.
         StateStore* m_pStateStore; 

         // target SourceID.
         SourceID m_targetSource;
      
   }; // End of class 'ClkEDCorr'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_CLK_ED_CORR_HPP
