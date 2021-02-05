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
            m_graph.reserve(m_vertCapacity * 2);
            m_graph.resize(m_vertCapacity);
        }

    public:

        size_t addVertex(const VT_& data)
        {
            if (m_endVertId == m_vertCapacity) {
                resizeVertCapacity_(m_vertCapacity * 2);
            }
            m_graph[m_endVertId] = { 0, m_endVertId, m_endVertId };
            m_vertData[m_endVertId] = data;

            return m_endVertId++;
        }

        void addEdge(size_t idV1, size_t idV2)
        {
            if (idV1 >= m_endVertId || idV2 >= m_endVertId) {
                throw std::invalid_argument("vertex id doesn't exists");
            }
            addEssence_(idV1);
            addEssence_(idV2);
        }

        size_t freeId() const noexcept { return m_endVertId; }

    public:

        template< typename T >
        void dumpTable(std::basic_ostream< T >& out) const
        {
            out << "Table:\n";
            out << "size = " << m_graph.size() << " capacity = " << m_graph.capacity() << std::endl;
            out << "vertCapacity = " << m_vertCapacity << " vertEndId = " << m_endVertId << std::endl;
            out << "t ";
            for (size_t i = 0, mi = m_graph.size(); i < mi; i++) {
                out << std::setw(5) << i;
            }
            out << "\na ";
            for (const auto& ess : m_graph) {
                out << std::setw(5) << ess.vert;
            }
            out << "\nn ";
            for (const auto& ess : m_graph) {
                out << std::setw(5) << ess.next;
            }
            out << "\np ";
            for (const auto& ess : m_graph) {
                out << std::setw(5) << ess.prev;
            }
        }

    private:

        void resizeVertCapacity_(size_t nCap)
        {
            assert(nCap > m_vertCapacity);

            const size_t nCapacity = m_graph.capacity() + nCap - m_vertCapacity;
            const size_t nSize = m_graph.size() + nCap - m_vertCapacity;

            m_vertCapacity += moveEdges_(nCap - m_vertCapacity, nSize, nCapacity);
        }

        size_t moveEdges_(size_t offset, size_t new_size, size_t new_capacity)
        {
            if (offset % 2) { offset++; }

            std::vector< Essence > newGraph;
            newGraph.reserve(new_capacity);
            newGraph.resize(new_size);

            //vertices
            for (size_t k = 0; k < m_vertCapacity; k++)
            {
                const Essence& oldEss = m_graph[k];
                Essence& newEss = newGraph[k];

                newEss.next = oldEss.next + ((oldEss.next >= m_vertCapacity) ? offset : 0);
                newEss.prev = oldEss.prev + ((oldEss.prev >= m_vertCapacity) ? offset : 0);
            }

            //edges
            for (size_t k = m_vertCapacity, mk = m_graph.size(); k < mk; k++)
            {
                const Essence& oldEss = m_graph[k];
                Essence& newEss = newGraph[k + offset];

                newEss.next = oldEss.next + ((oldEss.next >= m_vertCapacity) ? offset : 0);
                newEss.prev = oldEss.prev + ((oldEss.prev >= m_vertCapacity) ? offset : 0);
            }

            m_graph = std::move(newGraph);

            return offset;
        }

        void addEssence_(size_t vId)
        {
            assert(vId < m_endVertId);
            if (m_graph.capacity() == m_graph.size()) {
                m_graph.reserve((m_graph.size() - m_vertCapacity) * 2 + m_vertCapacity);
            }
            assert(m_graph.capacity() != m_graph.size());

            Essence& curVert = m_graph[vId];
            size_t idEss = m_graph.size();
            if (curVert.next == vId)
            {
                m_graph.push_back({ vId, vId, vId });
                curVert.next = curVert.prev = idEss;
            }
            else
            {
                m_graph.push_back({ vId, vId, curVert.prev });

                Essence& prevEdge = m_graph[curVert.prev];
                prevEdge.next = idEss;
                curVert.prev = idEss;
            }
        }

    };//class Graph

}//namespace ezg