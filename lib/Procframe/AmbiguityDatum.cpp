#pragma ident "$Id: AmbiguityDatum.cpp$"

/**
 * @file AmbiguityDatum.cpp
 * Class to define and handle complex equation systems for solvers.
 */

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
//  Copyright
//  ---------
//
//  Shjzhang, Wuhan University, 2014
//
//============================================================================
//
//  History
//
//  - Modify of the Method "getPhiQ()". The gnssRinex is needed to 'Prepare'
//    the stochastic model. When the variable is source-indexed, the gnssRinex
//    can be given by finding the source in gdsMap, but when the variable is
//    only satellite-indexed, the gnssRinex is the first gnssRinex which "SEE"
//    this satellite. (2012.06.19)
//
//  - Change
//    "varUnknowns.insert( currentUnknowns.begin(), currentUnknowns.end() );"
//    to
//    "varUnknowns = currentUnknowns;"
//
//============================================================================


#include "SystemTime.hpp"
#include "AmbiguityDatum.hpp"
#include <iterator>

using namespace gpstk::StringUtils;

namespace gpstk
{


      /* Add a new ambiguity type to be managed.
       *
       * @param ambType   Ambiguity variable to be added.
       *
       */
   AmbiguityDatum& AmbiguityDatum::setAmbType( const Variable& inType)
   {

         // Add "ambType" to "ambTypeList"
      ambType = inType;

         // We must "Prepare()" this "AmbiguityDatum"
      isPrepared = false;

      return (*this);

   }  // End of method 'AmbiguityDatum::setAmbType()'



      /* Prepare this object to carry out its work.
       *
       * @param gData   GNSS data structure (GDS).
       */
   AmbiguityDatum& AmbiguityDatum::Prepare( gnssRinex& gData )
   {

         // First, create a temporary gnssDataMap
      gnssDataMap myGDSMap;

         // Get gData into myGDSMap
      myGDSMap.addGnssRinex( gData );

         // Call the map-enabled method, and return the result
      return (Prepare(myGDSMap));

   }  // End of method 'AmbiguityDatum::Prepare()'



      /* Prepare this object to carry out its work.
       *
       * @param gdsMap     Map of GNSS data structures (GDS), indexed
       *                   by SourceID.
       */
   AmbiguityDatum& AmbiguityDatum::Prepare( gnssDataMap& gdsMap )
   {

         // Get current epoch
      currentEpoch = (*gdsMap.begin()).first;

         // Store the independent ambiguity unknowns of the previous epoch
      oldIndepEdges = currentIndepEdges;

         // Prepare set of current unknowns and list of current constraints
      currentEdges = prepareCurrentEdges(gdsMap);

         // Prepare set of current unknowns and list of current constraints
      currentIndepEdges = prepareCurrentIndepEdges();

         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'AmbiguityDatum::Prepare()'



      // Prepare set of current unknowns and list of current constraints
   EdgeSet AmbiguityDatum::prepareCurrentEdges( gnssDataMap& gdsMap )
   {

         // Let's create current edge set
      EdgeSet currentEdgeSet;

         // The ambiguity fixing method
      ARRound ambRes(1000,0.2,0.2);

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
              sdmIter != (*it).second.end();
              ++sdmIter )
         {
               // Finally, iterate through corresponding 'satTypeValueMap'
            for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                 stvmIter != (*sdmIter).second.end();
                 stvmIter++ )
            {

                  // New variable for the given 'ambType'
               Variable var( ambType );
               var.setSource( (*sdmIter).first );
               var.setSatellite( (*stvmIter).first );

               double amb, ambVar, weight, elev;
               int satArc;

               amb = ambMap[var];
               ambVar = ambVarMap[ var ][ var ];

                  // Weight for the edge
               weight = 1.0/ambVar;

                  // Get the arc for this edge
               elev = (*stvmIter).second.getValue( TypeID::elevation );
               satArc = (*stvmIter).second.getValue( TypeID::satArc );

                  // Create a new edge
               Edge edge( (*sdmIter).first, (*stvmIter).first, satArc, weight, elev );

                  // New constraint is complete: Add it to 'currentEdgeSet'
               currentEdgeSet.insert( edge );

                  // Calculating the fixing decision
               double sigma = std::sqrt(ambVar);
               double decision = ambRes.getDecision(amb, sigma);
               double ambFixed = std::floor(amb+0.5);
               if(decision>1000.0)
               {
                  ambFixedMap[ var ] = ambFixed;
               }

            }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

         // Return set of current edges
      return currentEdgeSet;

   }  // End of method 'AmbiguityDatum::prepareCurrentEdges()'



      // Prepare set of current unknowns and list of current constraints
   EdgeSet AmbiguityDatum::prepareCurrentIndepEdges()
   {
         // Current independent edge set
      EdgeSet currentIndepEdgeSet;

         // Continuous independent edges
      EdgeSet continuousIndepEdges;

clock_t start,finish;
double totaltime;
start=clock();

         // Find the edge of the oldIndepEdges in currentEdges
      for( EdgeSet::const_iterator it1 = oldIndepEdges.begin();
           it1 != oldIndepEdges.end();
           ++it1 )
      {
            // Arc information of the previous edge
         Arc oldArc( (*it1) );

            // Current edge information
         for( EdgeSet::const_iterator it2 = currentEdges.begin();
              it2 != currentEdges.end();
              ++it2)
         {
               // Arc information of current edge
            Arc newArc( (*it2) );

               // Add the old independent edge into the 'continuousIndepEdges'
            if( newArc == oldArc)
            {
               continuousIndepEdges.insert( (*it2) );
            }
         }
      }

         // Class of the Kruskal algorithm, which will generate independent edges
      Kruskal kruskal(currentEdges);

         // Set initial independent edges
      kruskal.setInitialEdges(continuousIndepEdges);

         // Create the Minimum Spanning Tree from the 'currentEdges'
      currentIndepEdgeSet = kruskal.createMST();

         /**
          * Now, Let's fix the independent ambiguities into integers
          */

         // Visit each "Edge" in "currentIndepEdgeSet"
      for( EdgeSet::const_iterator itEdge = currentIndepEdgeSet.begin();
           itEdge != currentIndepEdgeSet.end();
           ++itEdge)
      {

            // We will work with a copy of current Variable
         Variable var( ambType );

            // Set SourceID
         var.setSource( (*itEdge).getSource() );

            // Set satellite
         var.setSatellite( (*itEdge).getSatellite() );

            // Ambiguity value
         double amb = ambMap[ var ];

            // Get the neareast integer ambiguity
         double ambFixed = std::floor(amb+0.5);

            // Store the fixed ambiguity value into 'ambFixedMap'
            // Warning: we will insert this satellite into 'ambFixedMap'
            //          directly whether it can be fixed or not, because
            //          this satellite is the independent ambiguity datum.
         ambFixedMap[ var ] = ambFixed;

      }  // End of 'for( EdgeSet::const_iterator itEdge = ...'

         // Return set of current unknowns
      return currentIndepEdgeSet;

   }  // End of method 'AmbiguityDatum::prepareCurrentIndepEdges()'


      // Get current sources (SourceID's) and satellites (SatID's)
   void AmbiguityDatum::prepareCurrentSourceSat( gnssDataMap& gdsMap )
   {

         // Clear "currentSatSet"
      currentSatSet.clear();

         // Clear "currentSourceSet"
      currentSourceSet.clear();

         // Insert the corresponding SatID's in "currentSatSet"
      currentSatSet = gdsMap.getSatIDSet();

         // Insert the corresponding SourceID's in "currentSourceSet"
      currentSourceSet = gdsMap.getSourceIDSet();

         // Let's return
      return;

   }  // End of method 'AmbiguityDatum::prepareCurrentSourceSat()'



      /** Get the ambiguity datum which are fixed directly.
       */
   VariableDataMap AmbiguityDatum::getAmbFixedMap( void )
      throw(InvalidAmbiguityDatum)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidAmbiguityDatum("AmbiguityDatum is not prepared"));
      }

      return ambFixedMap;

   }  // End of method 'AmbiguityDatum::getAmbFixedMap()'



}  // End of namespace gpstk
