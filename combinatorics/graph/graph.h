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

    template< typename VT_ = size_t, typename ET_ = size_t >
    class Graph_t
    {
        struct Essence
        {
            size_t vert = 0;
            size_t next = 0;
            size_t prev = 0;

            ET_    data = {};
        };

        std::vector< VT_ >     m_vertData;

        std::vector< Essence > m_graph;
        size_t                 m_vertCapacity = MIN_VERT_CAPACITY;
        size_t                 m_endVertId    = 0;

    public:

        Graph_t()
        {
            m_vertData.resize(m_vertCapacity);
            m_graph.reserve  (m_vertCapacity * 2);
            m_graph.resize   (m_vertCapacity);
        }

    public:

        size_t addVertex(const VT_& data);
        std::pair< size_t, size_t > addEdge  (size_t idV1, size_t idV2, const ET_& to = {}, const ET_& from = {});


        const VT_& atVertData(size_t id) const { return m_vertData.at(id); }
        VT_&       atVertData(size_t id) { return const_cast< VT_& >(static_cast< const Graph_t* >(this)->atVertData(id)); }

        const ET_& atEdgeData(size_t id) const;
        ET_&       atEdgeData(size_t id) { return const_cast< VT_& >(static_cast< const Graph_t* >(this)->atEdgeData(id)); }

        size_t vertCapacity() const noexcept { return m_vertCapacity; }
        size_t vertSize    () const noexcept { return m_endVertId; }
        size_t edgeCapacity() const noexcept { return m_graph.capacity() - m_vertCapacity; }
        size_t edgeSize    () const noexcept { return m_graph.size() - m_vertCapacity; }

        size_t freeId      () const noexcept { return m_endVertId; }

        std::vector< size_t > getEdges(size_t vertId) const;

        std::pair< bool, std::vector< size_t > > isDoublyConnected() const;

        Graph_t< VT_ >& paint(const std::vector< VT_ >& colors, size_t startVert = 0)&;

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