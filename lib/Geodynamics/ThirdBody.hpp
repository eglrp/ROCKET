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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================


/**
 * @file ThirdBody.hpp
 * Class to do ThirdBody Gravitation calculation.
 */

#ifndef THIRD_BODY_HPP
#define THIRD_BODY_HPP

#include "ForceModel.hpp"
#include "SolarSystem.hpp"
#include "ReferenceSystem.hpp"

namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do ThirdBody Gravitation calculation.
     */
    class ThirdBody : public ForceModel
    {
    public:

        /// Default constructor
        ThirdBody()
            : pSolSys(NULL), pRefSys(NULL)
        {
            bSun     = false;
            bMoon    = false;
            bMercury = false;
            bVenus   = false;
            bMars    = false;
            bJupiter = false;
            bSaturn  = false;
            bUranus  = false;
            bNeptune = false;
            bPluto   = false;
        };

        /// Constructor
        ThirdBody(SolarSystem&     sol,
                  ReferenceSystem& ref)
        { pSolSys = &sol; pRefSys = &ref; };

        /// Default destructor
        virtual ~ThirdBody() {};


        /// Set SolarSystem
        inline ThirdBody& setSolarSystem(SolarSystem& sol)
        { pSolSys = &sol; return (*this); };

        /// Get SolarSystem
        inline SolarSystem* getSolarSystem() const
        { return pSolSys; };


        /// Set ReferenceSystem
        inline ThirdBody& setReferenceSystem(ReferenceSystem& ref)
        { pRefSys = &ref; return (*this); };

        /// Get ReferenceSystem
        inline ReferenceSystem* getReferenceSystem() const
        { return pRefSys; };

        /// Enable Sun in ThirdBody
        inline ThirdBody& enableSun()
        { bSun = true; return (*this); };

        /// Disable Sun in ThirdBody
        inline ThirdBody& disableSun()
        { bSun = false; return (*this); };


        /// Enable Moon in ThirdBody
        inline ThirdBody& enableMoon()
        { bMoon = true; return (*this); };

        /// Disable Moon in ThirdBody
        inline ThirdBody& disableMoon()
        { bMoon = false; return (*this); };


        /// Enable Mercury in ThirdBody
        inline ThirdBody& enableMercury()
        { bMercury = true; return (*this); };

        /// Disable Mercury in ThirdBody
        inline ThirdBody& disableMercury()
        { bMercury = false; return (*this); };


        /// Enable Venus in ThirdBody
        inline ThirdBody& enableVenus()
        { bVenus = true; return (*this); };

        /// Disable Venus in ThirdBody
        inline ThirdBody& disableVenus()
        { bVenus = false; return (*this); };

        /// Enable Mars in ThirdBody
        inline ThirdBody& enableMars()
        { bMars = true; return (*this); };

        /// Disable Mars in ThirdBody
        inline ThirdBody& disableMars()
        { bMars = false; return (*this); };


        /// Enable Jupiter in ThirdBody
        inline ThirdBody& enableJupiter()
        { bJupiter = true; return (*this); };

        /// Disable Jupiter in ThirdBody
        inline ThirdBody& disableJupiter()
        { bJupiter = false; return (*this); };


        /// Enable Saturn in ThirdBody
        inline ThirdBody& enableSaturn()
        { bSaturn = true; return (*this); };

        /// Disable Saturn in ThirdBody
        inline ThirdBody& disableSaturn()
        { bSaturn = false; return (*this); };


        /// Enable Uranus in ThirdBody
        inline ThirdBody& enableUranus()
        { bUranus = true; return (*this); };

        /// Disable Uranus in ThirdBody
        inline ThirdBody& disableUranus()
        { bUranus = false; return (*this); };


        /// Enable Nepture in ThirdBody
        inline ThirdBody& enableNeptune()
        { bNeptune = true; return (*this); };

        /// Disable Nepture in ThirdBody
        inline ThirdBody& disableNeptune()
        { bNeptune = false; return (*this); };


        /// Enable Pluto in ThirdBody
        inline ThirdBody& enablePluto()
        { bPluto = true; return (*this); };

        /// Disable Pluto in ThirdBody
        inline ThirdBody& disablePluto()
        { bPluto = false; return (*this); };


        /// Enable all planets in ThirdBody
        inline ThirdBody& enableAllPlanets()
        {
            bSun     = true;
            bMoon    = true;
            bMercury = true;
            bVenus   = true;
            bMars    = true;
            bJupiter = true;
            bSaturn  = true;
            bUranus  = true;
            bNeptune = true;
            bPluto   = true;

            return (*this);
        };

        /// Disable all planets in ThirdBody
        inline ThirdBody& disableAllPlanets()
        {
            bSun     = false;
            bMoon    = false;
            bMercury = false;
            bVenus   = false;
            bMars    = false;
            bJupiter = false;
            bSaturn  = false;
            bUranus  = false;
            bNeptune = false;
            bPluto   = false;

            return (*this);
        };

        /** Compute acceleration (and related partial derivatives) of ThirdBody.
         *
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits );


        /// Return the force model name
        inline virtual std::string modelName() const
        { return "ThirdBody"; };


    private:

        /// SolarSystem
        SolarSystem* pSolSys;

        /// ReferenceSystem
        ReferenceSystem* pRefSys;

        /// Planets to be considered
        bool bSun;
        bool bMoon;
        bool bMercury;
        bool bVenus;
        bool bMars;
        bool bJupiter;
        bool bSaturn;
        bool bUranus;
        bool bNeptune;
        bool bPluto;

    }; // End of class 'ThirdBody'

    // @}

}  // End of namespace 'gpstk'

#endif  // THIRD_BODY_HPP
