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
            , m_endVertId(0)
        {
            m_vertData.resize(m_vertCapacity);
            m_graph.reserve  (m_vertCapacity * 2);
            m_graph.resize   (m_vertCapacity);
        }

    public:

        size_t addVertex(const VT_& data);
        void addEdge(size_t idV1, size_t idV2);

        size_t freeId() const noexcept { return m_endVertId; }

    public:

        template< typename T >
        void dumpTable(std::basic_ostream< T >& out) const;

    private:

        void resizeVertCapacity_(size_t nCap);

        size_t moveEdges_(size_t offset, size_t new_size, size_t new_capacity);

        void addEssence_(size_t vId);

    };//class Graph

}//namespace ezg

#include "graph.inl"