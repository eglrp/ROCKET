#pragma ident "$Id$"

/**
 * @file ValidOp.hpp
 * Class to define and handle 'descriptions' of GNSS variables.
 */

#ifndef GPSTK_VALIDOP_HPP
#define GPSTK_VALIDOP_HPP

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
//  Gao Kang, Wuhan University, 2014-2016
//
//============================================================================
//
//  2016/12/05      Create this routine.
//
//============================================================================


#include "DataStructures.hpp"
#include "BLQDataReader.hpp"
#include "StateStore.hpp"
#include "ProcessingClass.hpp"

namespace gpstk
{

      /// Thrown when master station is not found
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(NotFoundMaster, gpstk::Exception);
   
      /// Thrown when master station have zero number of sat
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(MasterLessSatNum, gpstk::Exception);
      
   
   /** @addtogroup DataStructures */
      //@{

   class ValidOp : public ProcessingClass
   {
     public:

      /// Default constructor for ValidOp
      ValidOp():m_pBLQStore(NULL),m_pStateStore(NULL)
      {}    

         /** set blq data store object
          *
          * @param blqStore   the object of blq Store.
          *
          * @return this object
          */
      virtual ValidOp& setBLQStore( BLQDataReader& blqStore )
      { m_pBLQStore = &blqStore; return (*this); }

          
         /** set state store object
          * 
          * @param stateStore   the object of  StateStore 
          *
          * @return this object
          */
      virtual ValidOp& setStateStore( StateStore& stateStore )
      { m_pStateStore = &stateStore; return (*this); }
      

        /** valid gnssDataMap  
         *
         * @param gData   the data need to valid
         *
         */
      virtual gnssDataMap& Process( gnssDataMap& gData )
         throw(NotFoundMaster, MasterLessSatNum);
      
      
      virtual std::string getClassName(void) const;


      virtual ~ValidOp(){}

    private:
      
      // object to store blq data
      BLQDataReader* m_pBLQStore;
      
      // object to store state.
      StateStore* m_pStateStore; 
      
   }; // End of class 'ValidOp'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_VALIDOP_HPP
