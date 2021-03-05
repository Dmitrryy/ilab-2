///=================================================================//
///
///  Created by Dmitryyy (https://github.com/Dmitrryy)
///
///  Allow you to write off it.
///
///=================================================================//


//
/// Graph
///------
///
/// Here we consider the representation of a directed graph in the form of a table
/// (on a single vector) and the advantages of this representation.
///
/// Let there be a graph of n vertices and m edges, then
/// the graph is represented as a table(4 * (n + m)), the columns of which are:
/// idEssence - индекс массива, которому соответствует данная сущность.
/// vertex    - index of the vertex
/// next      - index of the next entity
/// prev      - index of the previous entity
///
/// Now in more detail and with an example.
/// graph:
///              -- 2 --
///            /    |    \
///         --      |      --
///      1          |         4
///         --      |      --
///            \    |    /
///              -- 3 --
///
/// might be represented by:
///                                v1 and v3       v2 and v4
///      |<---vertices--->|        |<----->|       |<----->|
/// id   | 0  1   2   3   :  4   5   6   7   8   9   10  11  12  13
/// vert | 0  0   0   0   :  1   2   1   3   2   3   2   4   3   4
/// next | 5  4   6   10  :  9   7   8   0   13  11  12  1   3   2
/// prev | 7  11  13  12  :  1   0   2   5   6   4   3   9   10  8
///                       |<----->|        |<----->|       |<----->|
///                  edge on v1 and v2     v2 and v3       v3 and v4
///
/// There's not much to say about the 'id'. [0, n) (in our case, n == 4) these are the
/// indices of the vertices, then these are the indices of the entities(edges).
///
/// Vert:
/// The first n are zero and do not represent anything. Further refers to the vertex id,
/// from which there is an edge. Entities can be divided into pairs.
/// One pair is two edges on two vertices (in one direction and in the other).
///
/// next and prev:
/// it should be considered as a doubly connected list whose beginning and end are
/// the same entity - the entity of the vertex from which these edges come.
///
//===----------------------------------------------------------------------===//


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
    std::pair<bool, std::vector<size_t> > Graph_t< VT_ , ET_>::isBipartite() const
    {
        const size_t MaxVal = std::numeric_limits< size_t >::max();

        std::vector< size_t > colors (m_endVertId, MaxVal);
        //the [id] contains the index from which vertex we got to the id vertex.
        std::vector< size_t > parents(m_endVertId, MaxVal);

        //results
        bool res = true;
        std::vector< size_t > oddCycle;


        for (size_t i = 0; i < m_endVertId && res; ++i)
        {
            //B3 begin
            //===================================
            // i === w
            if (colors[i] != MaxVal) {
                continue;
            }

            colors[i] = 0;

            std::stack< size_t > recursionStack;
            recursionStack.push(i);
            //===================================
            //B3 end

            while(!recursionStack.empty() && res)
            {
                size_t curVert = recursionStack.top();
                recursionStack.pop();

                size_t curEdge = m_graph[curVert].next;

                while(curEdge != curVert && res)
                {
                    size_t nextVert = m_graph[curEdge ^ 1].vert;
                    if (curVert == nextVert) {
                        res = false;
                        oddCycle.push_back(curVert);
                        break;
                    }

                    if (colors[nextVert] == MaxVal)
                    {
                        // if the neighbor is not colored, then we paint and put it on the stack.
                        colors[nextVert] = 1 - colors[curVert];
                        parents[nextVert] = curVert;

                        recursionStack.push(nextVert);
                    }
                    else if (colors[nextVert] == colors[curVert])
                    {
                        // terminate unsuccessfully;
                        // to get a cycle of odd length, we go through the array of
                        // parents until we meet the next vertex.
                        res = false;
                        size_t pVec = parents[curVert];
                        oddCycle.push_back(curVert);
                        while(pVec != parents[nextVert])
                        {
                            oddCycle.push_back(pVec);
                            pVec = parents[pVec];
                        }
                        oddCycle.push_back(pVec);
                        oddCycle.push_back(nextVert);
                    }

                    curEdge = m_graph[curEdge].next;
                }

            }
        }

        return { res, oddCycle };
    }

    template< typename VT_ , typename ET_ >
    Graph_t< VT_, ET_ >& Graph_t< VT_ , ET_>::paint(const std::vector< VT_ >& colors, size_t startVert /* = 0 */)&
    {
        if (colors.empty() || m_endVertId == 0) {
            return *this;
        }

        const size_t MaxVal = std::numeric_limits< size_t >::max();

        std::vector< size_t > VecColors (m_endVertId, MaxVal);


        size_t curColorId = 0;
        const size_t colorsNum = colors.size();

        for (size_t i = 0; i < m_endVertId; ++i)
        {
            //B3 begin
            //===================================
            // i === w
            if (VecColors[i] != MaxVal) {
                continue;
            }

            std::stack< size_t > recursionStack;
            recursionStack.push(i);
            //===================================
            //B3 end

            while(!recursionStack.empty())
            {
                size_t curVert = recursionStack.top();
                recursionStack.pop();

                if (VecColors[curVert] != MaxVal) {
                    continue;
                }
                atVertData(curVert) = colors.at(curColorId);
                VecColors[curVert] = 1;


                size_t curEdge = m_graph[curVert].next;

                while(curEdge != curVert)
                {
                    size_t nextVert = m_graph[curEdge ^ 1].vert;

                    if (VecColors[nextVert] == MaxVal)
                    {
                        recursionStack.push(nextVert);
                    }

                    curEdge = m_graph[curEdge].next;
                }

                curColorId = (curColorId + 1) % colorsNum;
            }
            curColorId = 0;
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