#pragma once

#include <vector>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cassert>

namespace ezg
{

    template< typename VT_ = size_t >
    class Graph_t
    {
        struct Essence
        {
            size_t vert = 0;
            size_t next = 0;
            size_t prev = 0;
        };

        std::vector< VT_ >     m_vertData;

        std::vector< Essence > m_graph;
        size_t                 m_vertCapacity;
        size_t                 m_endVertId = 0;

    public:

        Graph_t()
            : m_vertCapacity(4)
            , m_endVertId   (0)
        {
            m_vertData.resize(m_vertCapacity);
            m_graph.reserve  (m_vertCapacity * 2);
            m_graph.resize   (m_vertCapacity);
        }

    public:

        size_t addVertex(const VT_& data);
        void   addEdge  (size_t idV1, size_t idV2);

        const VT_& atVertData(size_t id) const { return m_vertData.at(id); }
        VT_&       atVertData(size_t id) { return const_cast< VT_& >(static_cast< const Graph_t* >(this)->atVertData(id)); }

        size_t vertCapacity() const noexcept { return m_vertCapacity; }
        size_t vertSize    () const noexcept { return m_endVertId; }
        size_t edgeCapacity() const noexcept { return m_graph.capacity() - m_vertCapacity; }
        size_t edgeSize    () const noexcept { return m_graph.size() - m_vertCapacity; }

        size_t freeId      () const noexcept { return m_endVertId; }

        std::vector< size_t > getEdges(size_t vertId) const;

        std::pair< bool, Graph_t< VT_ > > isDoublyConnected(const VT_& color1
                                                            , const VT_& color2
                                                            , const VT_& notColor) const;

    public:

        template< typename T >
        void dumpTable(std::basic_ostream< T >& out) const;

    private:

        void resizeVertCapacity_(size_t nCap);

        size_t moveEdges_(size_t offset, size_t new_size, size_t new_capacity);

        void addEssence_(size_t vId);

        bool paintDB_(size_t vert, const VT_& curColor, const VT_& nextColor, const VT_& nonColor);

/*        template< typename T >
        void copyTable(const Graph_t< T >& that);*/
    };//class Graph

}//namespace ezg

#include "graph.inl"