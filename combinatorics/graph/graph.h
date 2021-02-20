///=================================================================//
///
///  Created by Dmitryyy (https://github.com/Dmitrryy)
///
///  Allow you to write off it.
///
///=================================================================//

#pragma once

#include <vector>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stack>
#include <deque>

#define MIN_VERT_CAPACITY 4

namespace ezg
{
    /// Class Graph_t
    /// Implements the graph representation from Knuth's book TAOCP 4a.
    /// For a detailed description, see the file 'graph.inl'.
    ///
    /// \tparam VT_ - vertex data
    /// \tparam ET_ - edge data
    template< typename VT_ = size_t, typename ET_ = size_t >
    class Graph_t
    {
        /// Essence
        /// is a structure that defines the contents of a column in a table
        struct Essence
        {
            /// the number of the vertex from which the edge goes.
            /// If there are n vertices in the graph, then the first n Essence.vert are zero.
            size_t vert = 0;
            /// id of the next Essence
            size_t next = 0;
            /// id of the prev Essence
            size_t prev = 0;

            /// data of the Edge
            ET_    data = {};
        };

        std::vector< VT_ >     m_vertData;

        /// Table of the graph
        std::vector< Essence > m_graph;
        /// The place allocated for vertexes
        size_t                 m_vertCapacity = MIN_VERT_CAPACITY;
        /// Indicates a free id for vertexes
        size_t                 m_endVertId    = 0;

    public:

        /// Default constructor
        Graph_t()
        {
            m_vertData.resize(m_vertCapacity);
            m_graph.reserve  (m_vertCapacity * 2);
            m_graph.resize   (m_vertCapacity);
        }

    public:

        /// Add a vertex to the table
        /// \param data - data of the new vertex
        /// \complexity at best O (1), at worst O (N + M), where N - num vertices, M - num Edges.
        /// \return id of the new vertex
        size_t addVertex(const VT_& data);
        /// Add a Edge to the table
        /// \param idV1     - id first vertex
        /// \param idV2     - id second vertex
        /// \param dataTo   - data of the edge v1 to v2
        /// \param dataFrom - data of the edge v2 to v1
        /// \complexity at best O (1), at worst O (N + M), where N - num vertices, M - num Edges.
        /// \return first - id new edge v1 to v2
        /// \return second - id new edge v2 to v1
        std::pair< size_t, size_t > addEdge  (size_t idV1, size_t idV2, const ET_& dataTo = {}, const ET_& dataFrom = {});

        /// Get access to vertex data.
        /// \param id - the number of the vertex that returned the addVertex call.
        /// \return reference to data of the vertex with 'id'
        const VT_& atVertData(size_t id) const { return m_vertData.at(id); }
        VT_&       atVertData(size_t id) { return const_cast< VT_& >(static_cast< const Graph_t* >(this)->atVertData(id)); }

        /// Get access to vertex data.
        /// \param id - the number of the vertex that returned the addEdge call.
        /// \return reference to data of the edge with 'id'
        const ET_& atEdgeData(size_t id) const;
        ET_&       atEdgeData(size_t id) { return const_cast< VT_& >(static_cast< const Graph_t* >(this)->atEdgeData(id)); }

        /////////////////////////////get/////////////////////////////////
        size_t vertCapacity() const noexcept { return m_vertCapacity; }
        size_t vertSize    () const noexcept { return m_endVertId; }
        size_t edgeCapacity() const noexcept { return m_graph.capacity() - m_vertCapacity; }
        size_t edgeSize    () const noexcept { return m_graph.size() - m_vertCapacity; }
        /////////////////////////////////////////////////////////////////

        /// \param  vertId - the vertex id of the sending point
        /// \return vector of vertices that can be accessed from a vertex with vertId
        std::vector< size_t > getEdges(size_t vertId) const;

        /// Checks whether the graph is bipartite.
        /// \return first - boolean variable indicating whether the graph is bipartite.
        /// \return second - if the graph is not bipartite(first == false), then in this
        /// field the vector of vertices forming a cycle of odd length is.
        std::pair< bool, std::vector< size_t > > isBipartite() const;

        /// Paints the vertices in colors. If the graph is bipartite (for example),
        /// then there will be no connected vertices with the same color.
        /// \param colors - vector of colors to paint the graph in.
        /// \param startVert - the initial position of the painting (default == 0).
        /// \return reference to the current graph.
        Graph_t< VT_, ET_ >& paint(const std::vector< VT_ >& colors, size_t startVert = 0)&;

    public:

        template< typename T >
        void dumpTable(std::basic_ostream< T >& out) const;

    private:

        void resizeVertCapacity_(size_t nCap);

        size_t moveEdges_(size_t offset, size_t new_size, size_t new_capacity);

        size_t addEssence_(size_t vId, const ET_& data);
    };//class Graph

}//namespace ezg

#include "graph.inl"