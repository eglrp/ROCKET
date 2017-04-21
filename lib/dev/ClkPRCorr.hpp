#pragma ident "$Id$"

/**
 * @file ClkPRCorr.hpp
 * This class calculate receiver clock bias and
 * check prefitResidual of pesudorange.
 */

#ifndef GPSTK_TIME_UPDATE_CORR_HPP
#define GPSTK_TIME_UPDATE_CORR_HPP

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
//  2017/03/11      Create this routine.
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

   class ClkPRCorr : public ProcessingClass
   {
     public:

      /// Default constructor for ClkPRCorr
      ClkPRCorr()
	:m_pStateStore(NULL),m_pNavEphStore(NULL)
      {}    
          
         /**
          * set {@link StateStore} object. 
          */
      virtual ClkPRCorr& setStateStore( StateStore& stateStore )
      { m_pStateStore = &stateStore; return (*this); }
      
      virtual ClkPRCorr& setNavEphStore( Rinex3EphemerisStore& navEphStore )
      { m_pNavEphStore = &navEphStore; return (*this);}
	
      virtual satTypeValueMap& Process( const CommonTime& time, satTypeValueMap& gData )
	 throw(ProcessingException);

      virtual gnssDataMap& Process( gnssDataMap& gData )
         throw(ProcessingException);

      virtual std::string getClassName(void) const;
	
      virtual ~ClkPRCorr(){}

    private:

	/// keep source tropsheric wet compoment.
	struct SourceTropData 
	{
	   /// timestamp for wetValue.
	   CommonTime validTime;

	   /// wet component value.
	   double wetValue;

	   SourceTropData()
	     :validTime(CommonTime::BEGINNING_OF_TIME),wetValue(0.0)
	   {}
	};	
	
	
	std::map<SourceID, SourceTropData>  mWetTrop;
		
        // object to store navigation ephemeris
        Rinex3EphemerisStore* m_pNavEphStore;

        // object to store state.
        StateStore* m_pStateStore; 

        // target SourceID.
        SourceID m_targetSource;

   }; // End of class 'TimeUpdateCorr'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_CLK_PR_CORR_HPP
