#pragma ident "$Id: Kruskal.cpp 2596 2011-12-19 11:22:04Z shjzhang $"

/**
 * @file Kruskal.cpp
 * Class to find the edges of the minimal spanning tree of the 'observed' network.
 * using Kruskal algorithm.
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
//  Shoujian Zhang - School of Geodesy and Geomatics, Wuhan University, 2011
//
//============================================================================


#include "Kruskal.hpp"

namespace gpstk
{
     
   using namespace std;


      // Creating the minimum spanning trees from the edges of the network.
   EdgeSet Kruskal::createMST() 
   {
         // Temporary edges set
      EdgeSet indepEdges;

         // Make the component from current edges
      makeComponent(currentEdges);


         // Set initial component according to the initial vertexs
      mergeVertexs();


         // Clear current independent edges
      currentIndepEdges.clear(); 


         // Clear the indepEdges 
      indepEdges.clear();

         // Choose the 'light' edges from the 'initialEdges'
      indepEdges = chooseEdges(initialEdges);

         // Insert the indepEdges into 'currentIndepEdges'
      currentIndepEdges.insert( indepEdges.begin(), indepEdges.end() );


         // Clear the indepEdges again
      indepEdges.clear();

         // Choose the 'light' edges from the 'currentEdges'
      indepEdges = chooseEdges(currentEdges);

         // Insert the indepEdges into 'currentIndepEdges'
      currentIndepEdges.insert( indepEdges.begin(), indepEdges.end() );

         // Return the independent edges
      return currentIndepEdges;

   }  // End of "void createMininumSpanningTree()"


      /* Move the given inital vertexs to the same 'group', by 
       * changing their components to the same value.
       */
   void Kruskal::mergeVertexs() 
   {

         // Initial component
      int initialComponent;

         // Change the component of the inital vertex to 
         // the initial value
      for( VertexSet::iterator itVtx = initialVertexs.begin();
           itVtx != initialVertexs.end();
           ++itVtx )
      {
            // Get the component for the first vertex
         if(itVtx == initialVertexs.begin())
         {
            initialComponent = vtxCMPValueMap[*itVtx] ;
         }
         else
         {
            vtxCMPValueMap[*itVtx] =  initialComponent;
         }
      }

   }  // End of method "Kruskal::mergeComponents()"



      /** Make the component for the vertex in the observation graph.
       *
       * @param vertex       vertex 
       */
   void Kruskal::makeComponent(const EdgeSet& edgeSet) 
   {

         // Firstly, extract the vertex from current edges 
      for( EdgeSet::const_iterator it = edgeSet.begin();
           it != edgeSet.end();
           ++it )
      {
            // Get source vertex from Edge and insert Vertex into vertexSet
         Vertex vSource = (*it).getSource();

            // Insert the 'source' into vertexSet
         vertexSet.insert( vSource );

            // Get satellite vertex from Edge and insert Vertex into vertexSet
         Vertex vSat = (*it).getSatellite();

            // Insert the 'source' into vertexSet
         vertexSet.insert( vSat );

      }  // End of 'for( EdgeSet:: const_iterator it = ....'
      

         // Secondly, Set the component value
      int c(1);
      for( VertexSet::const_iterator it = vertexSet.begin();
           it != vertexSet.end();
           ++it )
      {
            // The component of this vertex is 'c'
         vtxCMPValueMap[*it] = c;
          
            // Component increment
         c++;
      }

   }  // End of method 'makeComponent()'



      /* Choose the MST edges from current edges
       *
       * @param edgeSet       Vertex of the graph 
       */
   EdgeSet Kruskal::chooseEdges(const EdgeSet& edgeSet)
   {
         // Independent edge set
      EdgeSet indepEdges;

         // Component number for source
      int cmpSource;

         // Component number for satellite
      int cmpSat;

         // Loop the 'edgeSet' to create independent edges
      for( EdgeSet::const_iterator it = edgeSet.begin();
           it != edgeSet.end();
           ++it )
      {
            // Get SourceID from Edge
         Vertex vSource = (*it).getSource();

            // Get SatID from Edge
         Vertex vSat = (*it).getSatellite();

            // Source Vertex component 
         cmpSource = findComponent( vSource ); 

            // Satellite Vertex component 
         cmpSat    = findComponent( vSat ); 

            // Merge the vertex into the same set 
         if( cmpSource != cmpSat)
         {
               // Add this edge into current independent edges 
            indepEdges.insert(*it);

               // Merge the component of source and satellite vertex
            mergeComponents(vSource, vSat);

         } // End of 'if ( cmpSource != comSat )'

      }  // End of 'for ( EdgeSet::const_iterator it = ...'

         // Return the independent edges
      return indepEdges;

   } // End of method "Kruskal::chooseEdges()"



      /** find the component number of the given vertex.
       *
       * @param vertex       vertex 
       */
   int Kruskal::findComponent(const Vertex& vertex) 
   {

         // Component value
      int component ;

         // The iterator for the 'vStart' and 'vEnd'
      VertexValueMap::iterator it = vtxCMPValueMap.find(vertex);

         // If found, then return 
      if( it != vtxCMPValueMap.end() )
      {
         component = (*it).second;
      }

         // Returen 
      return component;

   }  // End of 'findComponent( )'



      /* Merge the component of the source vertex and satellite vertex 
       * if the component for each other is differenet 
       *
       * @param vSource      Source Vertex of the graph 
       * @param vSat         Satellite Vertex of the graph
       *
       */
   void Kruskal::mergeComponents(const Vertex& vSource,
                                 const Vertex& vSat) 
   {
         // Get the component value of the vSource
      int cmpSource( findComponent( vSource) ) ;

         // Get the component value of the vSource
      int cmpSat( findComponent( vSat)) ;

         // Change the component of the source and satellite vertex
      if( cmpSource < cmpSat )
      {
            // Set all the vertex' components, which equals 
            // 'cmpSat' to 'cmpSource'
         for( VertexSet::iterator itVtx = vertexSet.begin();
              itVtx != vertexSet.end();
              ++itVtx )
         {
            if(vtxCMPValueMap[*itVtx] == cmpSat)
            {
               vtxCMPValueMap[*itVtx] =  cmpSource;
            }
         }

      }
      else
      {
            // Set all the vertex' components, which equal with 
            // 'cmpSource' to 'cmpSat'
         for( VertexSet::iterator itVtx = vertexSet.begin();
              itVtx != vertexSet.end();
              ++itVtx )
         {
            if(vtxCMPValueMap[*itVtx] == cmpSource)
            {
               vtxCMPValueMap[*itVtx] =  cmpSat;
            }
         }

      }  // End of 'if( cmpSource < cmpSat )' 

   }  // End of method "Kruskal::mergeComponents()"


}  // End of namespace gpstk
