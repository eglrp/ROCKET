#pragma ident "$Id: Kruskal.hpp 2596 2011-12-19 11:22:04Z shjzhang $"

/**
 * @file Kruskal.hpp
 * Class to find the edges of the minimal spanning tree of the 'observed' network.
 * using Kruskal algorithm.
 */

#ifndef GPSTK_KRUSKAL_HPP
#define GPSTK_KRUSKAL_HPP

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
//  Shoujian Zhang - School of Geodesy and Geomatics, Wuhan University, 2011
//
//============================================================================
//
//  Modifications 
//  -------------
//
//  - Create this program, 2011-12-19
//
//  - Modify the Vertex/Arc/Edge/Kruskal classes to be more compact, readable,
//    2012/06/16, Shoujian Zhang
//
//============================================================================


#include "Vertex.hpp"
#include "Edge.hpp"


using namespace std;

namespace gpstk
{

      /** @defgroup Graph */

      //@{
      

      /** This is a class to take use of the Kruskal algorithm to create
       *  a minimum spanning tree.
       *
       * In the network GNSS zero-difference parameter estimation, the 
       * independent ambiguities should be selected to avoid rank-defect of
       * the observation equation. AND the choice of the independent ambiguities
       * can be equivalent to the creation of the minmum spanning tree from
       * the observation graph.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *       // Define current edges, which can be generated from 'gdsMap'
       *    EdgeSet currentEdges;
       *
       *       // Define current independent edges, which are the edges
       *       // of the minimum spanning tree.
       *    EdgeSet currentIndepEdges;
       *
       *       // Define a Kruskal object
       *    Kruskal kruskal;
       *
       *       // Set the edges for the kruskal object
       *    kruskal.setEdges(currentEdges);
       *
       *       // Set the edges for the kruskal object
       *    currentIndepEdges = kruskal.createMinimumSpanningTree();
       *
       * @endcode
       *
       * Commonly, the GNSS network have multiple epoch observation data, and
       * in the network solutions, the ambiguity datum must be continuous. 
       * And in order to keep the consistence of the ambiguity datum, the 
       * independent edges from last epoch, which still be "saw" at current
       * epoch, should be forced as the "edge" of current MST by setting 
       * the initialEdges using the method "setInitialEdges()". 
       *
       * @sa AmbiguityConstraints.hpp
       *
       */

   class Kruskal 
   {
   public:


         /// Default constructor for Kruskal
      Kruskal()
      {};


         /* Common constructor for Kruskal.
          *
          * @param edgeSet      Observed edges of the network.
          */
      Kruskal(const EdgeSet& edgeSet)
      { 
         currentEdges = edgeSet;
      };

         /* Set the initial vertexs, which will be assigned as
          * the same component value .
          *
          * @param initVertexs    Initial vertexs   
          */
      virtual Kruskal& setInitialVertexs(const VertexSet& initVertexs)
      {
         initialVertexs = initVertexs;

         return (*this);
      };


         /* Move the given inital vertexs to the same 'group', by 
          * changing their components to the same value.
          */
      virtual void mergeVertexs() ;


         /* Set the initial edges, which will be selected as 
          * part of the "minimum spanning tree" firstly
          *
          * @param initEdges    Initial edges for the Kruskal  
          */
      virtual Kruskal& setInitialEdges(const EdgeSet& initEdges)
      {
         initialEdges = initEdges;

         return (*this);
      };


         /* Set the edges for the 'graph'
          *
          * @param edgeSet      Edge set .
          */
      virtual Kruskal& setEdges(const EdgeSet& edgeSet)
      {
         currentEdges = edgeSet;
         return (*this);
      };


         /* Create minimum spanning trees from the given edges
          */
      virtual EdgeSet createMST(); 


         /* Choosing the MST edges from current edges
          *
          * @param edgeSet       Vertex of the graph 
          */
      virtual EdgeSet chooseEdges(const EdgeSet& edgeSet) ;


         /** Make the component for the observed vertex 
          *  in the observation graph.
          *
          * @param vertex       vertex 
          */
      virtual void makeComponent(const EdgeSet& edgeSet) ;


         /* Find the component value for the given vertex.
          *
          * @param vertex        Vertex of the graph 
          */
      virtual int findComponent(const Vertex& vertex) ;


         /* Merge the component of the source vertex and satellite vertex 
          * if the component for each other is differenet 
          *
          * @param vertex        Vertex of the graph 
          */
      virtual void mergeComponents(const Vertex& vSource,
                                   const Vertex& vSat ) ;


         /// Destructor
      virtual ~Kruskal() {};


   private:


         /// Vector for the a priori independent Edges
      EdgeSet initialEdges;


         /// Vector for all the current Edges
      EdgeSet currentEdges;


         /// Vector for the a priori independent Edges
      VertexSet initialVertexs;


         /// Set for the vertex of the observed network
      VertexSet vertexSet;


         /// Map for the connected component of the 'Vertex' 
      VertexValueMap vtxCMPValueMap;


         /// Vector for the Edges of the minimum spanning trees
      EdgeSet currentIndepEdges;


   }; // End of class 'Kruskal'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_KRUSKAL_HPP
