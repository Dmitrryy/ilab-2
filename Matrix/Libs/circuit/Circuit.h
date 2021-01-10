#pragma once

#include <matrix/Matrix.h>
#include <vector>
#include <optional>
#include <sstream>
#include <set>

namespace ezg
{
    struct Edge {
        size_t id = 0;
        size_t v1 = 0, v2 = 0;
        std::optional< float > resistance;
        std::optional< float > current;
        std::optional< float > eds;

        bool operator == (const Edge& that) const
        {
            return (v1 == that.v1 && v2 == that.v2) || (v1 == that.v2 && v2 == that.v1);
        }
    };


    class Circuit
    {
        matrix::Matrix< int > m_graph;
        std::vector< Edge >    m_data;

    public:

        void connect(size_t id1, size_t id2, Edge edge)
        {
            m_graph.resize(std::max(m_graph.getLines(), static_cast<size_t>(std::max(id1, id2)) + 1), m_graph.getColumns() + 1);
            m_graph.at(id1, m_graph.getColumns() - 1) = 1;
            m_graph.at(id2, m_graph.getColumns() - 1) = 1;

            edge.id = m_data.size();
            m_data.emplace_back(edge);
        }

        std::vector< Edge > getData() { return m_data; }

        std::vector< std::vector< Edge > > findCycles() const
        {
            return findCycles_(1, {});
        }


        void calculateCurrent()
        {
            const size_t mGlines = m_graph.getLines();
            const size_t mGcolumns = m_graph.getColumns();

            //fixme
            auto cycles = findCycles();

            assert(m_data.size() == mGcolumns);

            const size_t num_cycles = cycles.size();
            matrix::Matrix< double > LSystem(num_cycles + mGlines, m_data.size());
            std::vector< double > freeMembers(num_cycles + mGlines);
            for (size_t c = 0; c < num_cycles; c++)
            {
                double eds = 0;
                const size_t num_edges = cycles[c].size();
                for (size_t k = 0; k < num_edges; k++)
                {
                    Edge& cur = cycles[c][k];
                    eds += cur.eds.value_or(0) * ((cur.v1 > cur.v2) ? -1.f : 1.f);

                    LSystem.at(c, cur.id) = cur.resistance.value();
                }
                freeMembers[c] = eds;
            }

            for (size_t l = 0; l < mGlines; l++)
            {
                for(size_t c = 0; c < mGcolumns; c++)
                {
                    if (m_graph.at(l, c) != 0)
                    {
                        LSystem.at(num_cycles + l, c) = ((m_data[c].v2 == l) ? 1.f : -1.f);
                    }
                }
            }
            std::cout << LSystem << std::endl;
            auto solv = LSystem.solve(freeMembers);
            //std::cout << solv.first << solv.second;
            assert(solv.second.isZero());

            for (size_t c = 0; c < mGcolumns; c++) {
                m_data[c].current = solv.first.at(c, 0);
            }
        }


        std::string dumpStr() const
        {
            std::ostringstream out;
            for (const auto& cur : m_data)
            {
                out << cur.v1 << " -- " << cur.v2 << ", " << cur.resistance.value_or(0) << ';';
                if (cur.eds.has_value()) {
                    out << ' ' << cur.eds.value();
                }
                out << std::endl;
            }
            out << m_graph << std::endl;

            return out.str();
        }

    //private:

        /*std::map< Edge, std::vector < Edge > > equalCurrent_() const
        {
            const size_t lines = m_graph.getLines();
            const size_t columns = m_graph.getColumns();

            std::map< Edge, std::vector < Edge > > result;

            for (size_t l = 0; l < lines; l++)
            {
                size_t count = 0;
                size_t el[2] = {};
                for(size_t c = 0; c < columns; c++)
                {
                    if (m_graph.at(l, c) != 1) {
                        if (count == 2) {
                            count = 0;
                            break;
                        }
                        el[count] = c;
                        count++;
                    }
                }
                if (count == 2) {
                    result[m_data[el[0]]].push_back(m_data[el[1]]);
                    result[m_data[el[1]]].push_back(m_data[el[0]]);
                }
            }

            return result;
        }*/

        std::vector< std::vector< Edge > > findCycles_(size_t cur, std::vector< Edge > trace) const
        {
            std::vector< std::vector< Edge > > multi_res;

            const size_t columns = m_graph.getColumns();
            const size_t lines = m_graph.getLines();
            for (size_t c = 0; c < columns; c++)
            {
                if (m_graph.at(cur, c) == 1)
                {
                    for (size_t next_v = 0; next_v < lines; next_v++)
                    {
                        if (m_graph.at(next_v, c) == 1 && next_v != cur)
                        {
                            auto it = std::find_if(trace.begin(), trace.end(), [next_v](const Edge& e){
                                static size_t pre1 = 0, pre2 = 0, store_nv = next_v;
                                if (store_nv != next_v) {
                                    pre1 = pre2 = 0;
                                    store_nv = next_v;
                                }
                                bool res = false;
                                if (e.v1 == pre1 || e.v1 == pre2) {
                                    res = e.v2 == next_v;
                                }
                                else if (e.v2 == pre1 || e.v2 == pre2) {
                                    res = e.v1 == next_v;
                                } else  {
                                    res = e.v1 == next_v || e.v2 == next_v;
                                }
                                pre1 = e.v1;
                                pre2 = e.v2;

                                return res;
                            });

                            if (it == trace.end())
                            {
                                auto tmp_trace = trace;

                                tmp_trace.push_back(m_data[c]);
                                auto tmp = findCycles_(next_v, std::move(tmp_trace));
                                multi_res.insert(multi_res.end(), tmp.begin(), tmp.end());
                            }
                            else
                            {
                                if (trace.back().v1 != next_v && trace.back().v2 != next_v)
                                {
                                    multi_res.emplace_back(it, trace.end());

                                    multi_res.back().push_back(m_data[c]);
                                }
                            }
                        }

                    }
                }

            }

            std::vector< std::vector< Edge > > res;
            {
                std::set< std::set< int > > set;

                const size_t size = multi_res.size();
                for (size_t i = 0; i < size; i++)
                {
                    const size_t old_size = set.size();
                    std::set< int > ts;
                    for (const auto& ed : multi_res[i])
                    {
                        ts.insert(ed.v1);
                        ts.insert(ed.v2);
                    }
                    set.emplace(std::move(ts));
                    if (old_size < set.size())
                    {
                        res.emplace_back(std::move(multi_res[i]));
                    }
                }
            }

            return res;
        }

    };//class Circuit

}//namespace ezg