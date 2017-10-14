#pragma ident "$Id$"

/**
 * @file ComputeTides.hpp
 * This is a class to compute the Tide effects!
 */

#ifndef GPSTK_COMPUTETIDES_HPP
#define GPSTK_COMPUTETIDES_HPP

#include "ProcessingClass.hpp"
#include "SolidTides.hpp"
#include "OceanLoading.hpp"
#include "PoleTides.hpp"


namespace gpstk
{

    class ComputeTides
    {
    public:
        ComputeTides()
            : m_pBLQReader(NULL),m_pEOPDataStore(NULL),
              m_NominalPos(0,0,0),m_StationName("")
        {}


        ComputeTides(BLQDataReader& blqReader, EOPDataStore2& eopDataStore)
            : m_NominalPos(0,0,0),m_StationName("")
        { m_pBLQReader = &blqReader; m_pEOPDataStore = &eopDataStore; }


        virtual Triple correct(const CommonTime time);


        virtual ComputeTides& setBLQReader( BLQDataReader& blqReader)
        { m_pBLQReader = &blqReader; return (*this); }


        virtual ComputeTides& setEOPDataStore( EOPDataStore2& eopDataStore )
        { m_pEOPDataStore = &eopDataStore; return (*this); }


        virtual ComputeTides& setNominalPosition(Position position)
        { m_NominalPos = position; return (*this); }


        virtual ComputeTides& setStationName( std::string name )
        { m_StationName = name; return (*this);}


        virtual ~ComputeTides() {}


    private:

        // blq data
        BLQDataReader* m_pBLQReader;

        // eop data
        EOPDataStore2* m_pEOPDataStore;

        // station position
        Position m_NominalPos;

        // station Name
        std::string m_StationName;
    };

}


#endif
