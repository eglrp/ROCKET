//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S.
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software.
//
//Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/**
* @file EGMModel.hpp
* Class to do Earth Gravitation calculation.
*
*/

#ifndef EGM_MODEL_HPP
#define EGM_MODEL_HPP

#include "ForceModel.hpp"
#include "ReferenceSystem.hpp"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Earth Gravitation calculation.
     */
    class EGMModel : public ForceModel
    {
    public:

        /// Struct to hold EGM data
        struct EGMData
        {
            std::string modelName;

            double GM;
            double ae;

            bool includesPermTide;

            double refMJD;

            int maxDegree;

            Matrix<double> normalizedCS;
        };

    public:

        /** Constructor.
         * @param n   Desired degree
         * @param m   Desired order
         */
        EGMModel(int n=0, int m=0)
            : desiredDegree(n), desiredOrder(m), pRefSys(NULL),
              pSolidTide(NULL), pOceanTide(NULL), pPoleTide(NULL),
              satGravimetry(false)
        {
            int size( 20*(20+1)/2 + (20+1) );
            egmData.normalizedCS.resize(size,4,0.0);
        };


        /// Default destructor
        virtual ~EGMModel() {};


        /// Load file
        virtual void loadFile(const std::string& file)
            throw(FileMissingException)
        {};


        /// Set desired degree and order
        /// Warning: If you call method setDesiredDegreeOrder(), then you MUST
        ///          call method loadFile() to reinitialize gmData struct.
        EGMModel& setDesiredDegreeOrder(const int& n, const int& m)
        {
            desiredDegree  =  n;

            if(n >= m)
            {
                desiredOrder   =  m;
            }
            else
            {
                desiredOrder   =  n;
            }

            return (*this);
        };


        /// Get desired degree and order
        inline void getDesiredDegreeOrder(int& n, int& m) const
        { n = desiredDegree; m = desiredOrder; };


        /// Set reference system
        inline EGMModel& setReferenceSystem(ReferenceSystem& ref)
        { pRefSys = &ref; return (*this); };

        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        { return pRefSys; };

        /// Set earth solid tide
        inline EGMModel& setEarthSolidTide(EarthSolidTide& solidTide)
        { pSolidTide = &solidTide; return (*this); };

        /// Get earth solid tide
        inline EarthSolidTide* getEarthSolidTide() const
        { return pSolidTide; };


        /// Set earth ocean tide
        inline EGMModel& setEarthOceanTide(EarthOceanTide& oceanTide)
        { pOceanTide = &oceanTide; return (*this); };


        /// Get earth ocean tide
        inline EarthOceanTide* getEarthOceanTide() const
        { return pOceanTide; };


        /// Set earth pole tide
        inline EGMModel setEarthPoleTide(EarthPoleTide& poleTide)
        { pPoleTide = &poleTide; return (*this); };

        /// Get earth pole tide
        inline EarthPoleTide* getEarthPoleTide() const
        { return pPoleTide; };


        /// Set satellite gravimetry
        inline EGMModel& setSatGravimetry(const bool& sg)
        { satGravimetry = sg; return (*this); };


        /// Get satellite gravimetry
        inline bool getSatGravimetry() const
        { return satGravimetry; };



        /** Compute acceleration (and related partial derivatives) of Earth
         *  Gravitation.
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits )
        {};


        /// Return the force model name
        inline virtual std::string forceModelName() const
        { return "EGMModel"; }

    protected:

        /// Desired degree and order
        int desiredDegree;
        int desiredOrder;

        /// Reference system
        ReferenceSystem*  pRefSys;

        /// Earth tides
        EarthSolidTide*   pSolidTide;
        EarthOceanTide*   pOceanTide;
        EarthPoleTide*    pPoleTide;

        /// EGM data
        EGMData egmData;

        /// Satellite gravimetry
        bool satGravimetry;

    }; // End of class 'EGMModel'

    // @}

}  // End of namespace 'gpstk'

#endif   // EGM_MODEL_HPP
