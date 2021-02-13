
#include <unordered_map>

namespace ezg
{

    template< typename VT_ , typename ET_ >
    size_t Graph_t< VT_ , ET_>::addVertex(const VT_& data)
    {
        if (m_endVertId == m_vertCapacity) {
            resizeVertCapacity_(m_vertCapacity * 2);
        }
        m_graph[m_endVertId] = { 0, m_endVertId, m_endVertId };
        m_vertData[m_endVertId] = data;

        return m_endVertId++;
    }

    template< typename VT_ , typename ET_ >
    std::pair< size_t, size_t > Graph_t< VT_ , ET_>::addEdge(size_t idV1, size_t idV2, const ET_& to /*= {}*/, const ET_& from /*= {}*/)
    {
        if (idV1 >= m_endVertId || idV2 >= m_endVertId) {
            throw std::invalid_argument("vertex id doesn't exists");
        }
        const size_t left = addEssence_(idV1, to);
        const size_t right = addEssence_(idV2, from);

        return { left, right };
    }


    template< typename VT_, typename ET_ >
    const ET_& Graph_t< VT_, ET_ >::atEdgeData(size_t id) const
    {
        if (id < m_vertCapacity) {
            throw std::invalid_argument("out of range: " + std::to_string(id) + "  ∉ ["
            + std::to_string(m_vertCapacity) + ", " + std::to_string(m_graph.size() - 1) + ']');
        }
        return m_graph.at(id);
    }


    template< typename VT_ , typename ET_ >
    template< typename T >
    void Graph_t< VT_ , ET_>::dumpTable(std::basic_ostream< T >& out) const
    {
        const size_t widthColumn = 5;
        out << "Table:\n";
        out << "size = " << m_graph.size() << " capacity = " << m_graph.capacity() << std::endl;
        out << "vertCapacity = " << m_vertCapacity << " vertEndId = " << m_endVertId << std::endl;
        out << "t |";
        for (size_t i = 0, mi = m_graph.size(); i < mi; i++) {
            out << std::setw(widthColumn) << i;
        }
        out << "\na |";
        for (const auto& ess : m_graph) {
            out << std::setw(widthColumn) << ess.vert;
        }
        out << "\nn |";
        for (const auto& ess : m_graph) {
            out << std::setw(widthColumn) << ess.next;
        }
        out << "\np |";
        for (const auto& ess : m_graph) {
            out << std::setw(widthColumn) << ess.prev;
        }
        out << "\nd |";
        for (const auto& dt : m_vertData) {
            out << std::setw(widthColumn) << dt;
        }
        out << std::endl;
    }


    template< typename VT_ , typename ET_ >
    std::vector< size_t > Graph_t< VT_ , ET_>::getEdges(size_t vertId) const
    {
        if (vertId >= m_endVertId) {
            throw std::invalid_argument("vertex id doesn't exists");
        }
        std::vector< size_t > res;
        Essence nEss = m_graph[vertId];
        while (nEss.next != vertId)
        {
            res.push_back(m_graph[nEss.next ^ 1].vert);
            nEss = m_graph[nEss.next];
        }

        return res;
    }



    template< typename VT_ , typename ET_ >
    std::pair<bool, std::vector<size_t> > Graph_t< VT_ , ET_>::isDoublyConnected() const
    {
        //because it's so easy to follow the depth
        std::stack<std::stack<size_t> > nextVert;
        std::stack<size_t> trace;
        std::vector<size_t> deeps(m_endVertId, 0);

        nextVert.push({});
        trace.push(0);
        for (size_t k = m_endVertId; k != 0; k--) {
            nextVert.top().push(k - 1);
        }

        //result
        bool isDB = true;
        std::vector<size_t> oddCycle;

        while (!nextVert.empty() && isDB) {
            if (nextVert.top().empty()) {
                nextVert.pop();
                trace.pop();
                continue;
            }

            const size_t cur = nextVert.top().top();
            nextVert.top().pop();

            if (deeps[cur] == 0) {
                //nextVert.size() and depth are equal
                deeps[cur] = nextVert.size();
                trace.push(cur);
            } else {
                continue;
            }

            auto toVertices = getEdges(cur);
            nextVert.push({});
            for (size_t i = toVertices.size(); i != 0; i--) {
                const size_t nVert = toVertices[i - 1];
                if (deeps[nVert] == 0) {
                    nextVert.top().push(nVert);
                } else {
                    if ((deeps[nVert] - deeps[cur]) % 2 == 0)
                    {//cycle of odd length detected
                        while (trace.top() != nVert) {
                            oddCycle.push_back(trace.top());
                            trace.pop();
                        }
                        oddCycle.push_back(trace.top());
                        isDB = false;
                        break;
                    }
                }
            }

        }//while(!nextVert.empty() && isDB)

        return {isDB, oddCycle};
    }

    template< typename VT_ , typename ET_ >
    Graph_t< VT_ >& Graph_t< VT_ , ET_>::paint(const std::vector< VT_ >& colors, size_t startVert /* = 0 */)&
    {
        if (colors.empty() || m_endVertId == 0) {
            return *this;
        }

        assert(startVert < m_endVertId);

        //TODO come up with a suitable workaround function to avoid copy-paste

        //because it's so easy to follow the depth
        std::stack< std::stack<size_t> > nextVert;
        std::stack< size_t > trace;
        std::vector< size_t > deeps(m_endVertId, 0);

        nextVert.push({});
        trace.push(0);
        for (size_t k = m_endVertId; k != 0; k--) {
            nextVert.top().push(k - 1);
        }
        nextVert.top().push(startVert);

        const size_t colorsSize = colors.size();

        while (!nextVert.empty())
        {
            if (nextVert.top().empty()) {
                nextVert.pop();
                trace.pop();
                continue;
            }

            const size_t cur = nextVert.top().top();
            nextVert.top().pop();

            if (deeps[cur] == 0) {
                //nextVert.size() and depth are equal
                const size_t depth = nextVert.size();
                deeps[cur] = depth;
                atVertData(cur) = colors.at((depth - 1) % colorsSize);
                trace.push(cur);
            } else {
                continue;
            }

            auto toVertices = getEdges(cur);
            nextVert.push({});
            for (size_t i = toVertices.size(); i != 0; i--) {
                const size_t nVert = toVertices[i - 1];
                if (deeps[nVert] == 0) {
                    nextVert.top().push(nVert);
                }
            }

        }

        return *this;
    }


    //private:


    template< typename VT_ , typename ET_ >
    void Graph_t< VT_ , ET_>::resizeVertCapacity_(size_t nCap)
    {
        assert(nCap > m_vertCapacity);

        const size_t nCapacity = m_graph.capacity() + nCap - m_vertCapacity;
        const size_t nSize = m_graph.size() + nCap - m_vertCapacity;

        m_vertCapacity += moveEdges_(nCap - m_vertCapacity, nSize, nCapacity);
        m_vertData.resize(m_vertCapacity);
    }

    template< typename VT_ , typename ET_ >
    size_t Graph_t< VT_ , ET_>::moveEdges_(size_t offset, size_t new_size, size_t new_capacity)
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

            newEss.vert = oldEss.vert;
            newEss.next = oldEss.next + ((oldEss.next >= m_vertCapacity) ? offset : 0);
            newEss.prev = oldEss.prev + ((oldEss.prev >= m_vertCapacity) ? offset : 0);
        }

        m_graph = std::move(newGraph);

        return offset;
    }

    template< typename VT_ , typename ET_ >
    size_t Graph_t< VT_ , ET_>::addEssence_(size_t vId, const ET_& data)
    {
        assert(vId < m_endVertId);
        if (m_graph.capacity() == m_graph.size()) {
            m_graph.reserve((m_graph.size() - m_vertCapacity) * 2 + m_vertCapacity);
        }
        assert(m_graph.capacity() != m_graph.size());

        Essence& curVert = m_graph[vId];
        const size_t idEss = m_graph.size();
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
        m_graph.back().data = data;

        return idEss;
    }

}//namespace ezg