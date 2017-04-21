#include "ComputeTides.hpp"


namespace gpstk
{

   Triple ComputeTides::correct(const CommonTime time)
   {
      SolidTides solid;
      Triple solidCorr, oceanCorr, poleCorr;
      solidCorr = solid.getSolidTide( time, m_NominalPos );

      if( NULL != m_pBLQReader ) 
      {
         OceanLoading ocean(*m_pBLQReader);
         oceanCorr = ocean.getOceanLoading(m_StationName, time); 
      }

      if( NULL != m_pEOPDataStore ) 
      { 
         PoleTides pole(*m_pEOPDataStore);
         poleCorr = pole.getPoleTide(time,m_NominalPos); 
      }

      return solidCorr + oceanCorr + poleCorr;
   }



}
