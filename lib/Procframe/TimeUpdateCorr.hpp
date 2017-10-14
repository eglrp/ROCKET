#pragma ident "$Id$"

/**
 * @file ClkPRCorr.hpp
 * This class correct observations after TimeUpdate process.
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
//  2017/03/06      Create this routine.
//
//============================================================================


#include "DataStructures.hpp"
#include "StateStore.hpp"
#include "ProcessingClass.hpp"
#include "Equation.hpp"
#include <list>

namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{

    class TimeUpdateCorr : public ProcessingClass
    {
    public:

        /// Default constructor.
        TimeUpdateCorr()
          : m_pStateStore(NULL)
        {};


        /**
         * set {@link StateStore} object.
         */
        virtual TimeUpdateCorr& setStateStore( StateStore& stateStore )
        { m_pStateStore = &stateStore; return (*this); };


        /**
         * add Equation to equation list.
         */
        virtual TimeUpdateCorr& addEquation( Equation& equ )
        {  m_Equations.push_back(equ); return (*this); };


        /** Return a reference to a satTypeValueMap object after
         *  solving the previously defined equation system.
         *
         * @param gData    Data object holding the data.
         */
        virtual satTypeValueMap& Process( satTypeValueMap& gData )
            throw(ProcessingException);


        /** Return a reference to a gnssDataMap object after solving
         *  the previously defined equation system.
         *
         * @param gdsMap    Data object holding the data.
         */
        virtual gnssDataMap& Process( gnssDataMap& gData )
            throw(ProcessingException);


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor.
        virtual ~TimeUpdateCorr() {};

    private:

      // equation for model.
      std::list<Equation> m_Equations;

      // object to store state.
      StateStore* m_pStateStore;

      // target SourceID.
      SourceID m_targetSource;

   }; // End of class 'TimeUpdateCorr'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_TIME_UPDATE_CORR_HPP
