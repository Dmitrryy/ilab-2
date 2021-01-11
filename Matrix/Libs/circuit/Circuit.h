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
        bool operator != (const Edge& that) const { return !(*this == that); }
        bool operator < (const Edge& that) const { return id < that.id; }
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
            return findCycles_(1, 1, {});
        }


        void calculateCurrent()
        {
            const size_t mGlines = m_graph.getLines();
            const size_t mGcolumns = m_graph.getColumns();

            //fixme
            auto cycles = findCycles();

#ifdef DEBUG
            {
                int i = 0;
                for (const auto &c : cycles) {
                    std::cout << '[' << i << "] ";
                    for (const auto &elem : c) {
                        // std::cout << elem.v1 << ' ' << elem.v2 << ' ';
                        std::cout << elem.id + 1 << ' ';
                    }
                    std::cout << std::endl;
                    i++;
                }
            }
#endif
            assert(m_data.size() == mGcolumns);

            const size_t num_cycles = cycles.size();
            matrix::Matrix< double > LSystem(num_cycles + mGlines, m_data.size());
            std::vector< double > freeMembers(num_cycles + mGlines);
            for (size_t c = 0; c < num_cycles; c++)
            {
                double eds = 0;
                const size_t num_edges = cycles[c].size();
                size_t pre1 = 0, pre2 = 0;
                for (size_t k = 0; k < num_edges; k++)
                {
                    Edge& cur = cycles[c][k];

                    bool minus = false;
                    if (k == 0) {
                        pre1 = cycles[c].back().v1;
                        pre2 = cycles[c].back().v2;
                    }

                    if (cur.v1 != pre1 && cur.v1 != pre2)
                        minus = true;
                    pre1 = cur.v1;
                    pre2 = cur.v2;

                    eds += cur.eds.value_or(0) * ((minus) ? -1.f : 1.f);

                    LSystem.at(c, cur.id) = cur.resistance.value() * ((minus) ? -1.f : 1.f);
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
#ifdef DEBUG
            std::cout << "line system:\n";
            std::cout << LSystem << std::endl;
#endif
            auto solv = LSystem.solve(freeMembers);

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


        std::vector< std::vector< Edge > > findCycles_(size_t first_vert, size_t cur, std::vector< Edge > trace) const
        {
            std::vector< std::vector< Edge > > multi_res;

            const size_t columns = m_graph.getColumns();
            const size_t lines   = m_graph.getLines();
            const bool first_step = trace.empty();
            for (size_t c = 0; c < columns; c++)
            {
                if (m_graph.at(cur, c) == 1)
                {
                    const Edge next_edg = m_data[c];
                    const size_t next_vert = (cur == next_edg.v1) ? next_edg.v2 : next_edg.v1;
                    if (first_step)
                    {
                        auto tmp_trace = trace;
                        tmp_trace.push_back(next_edg);
                        auto re_res = findCycles_(first_vert, next_vert, tmp_trace);
                        multi_res.insert(multi_res.end(), re_res.begin(), re_res.end());
                    }
                    else if (trace.back() != next_edg)
                    {
                        if (next_vert == first_vert)
                        {
                            auto tmp_trace = trace;
                            tmp_trace.push_back(next_edg);
                            multi_res.emplace_back(std::move(tmp_trace));
                        }
                        else
                        {
                            bool secondly = false;
                            auto it = std::find_if(trace.begin(), trace.end(), [&secondly, next_vert](const Edge& e) {
                                bool result = false;

                                if (e.v1 == next_vert || e.v2 == next_vert)
                                {
                                    if (secondly) {
                                        result = true;
                                    }
                                    else {
                                        secondly = true;
                                    }
                                }
                                else if (secondly) { assert(0);}

                                return result;
                            });

                            if (it == trace.end())
                            {
                                auto tmp_trace = trace;

                                tmp_trace.push_back(next_edg);
                                auto tmp = findCycles_(first_vert, next_vert, std::move(tmp_trace));
                                multi_res.insert(multi_res.end(), tmp.begin(), tmp.end());
                            }
                            else
                            {
                                multi_res.emplace_back(it, trace.end());
                                multi_res.back().push_back(next_edg);
                            }

                        }
                    }

                }

            }

            std::vector< std::vector< Edge > > res;
            {
                std::set< std::set< Edge > > set;

                const size_t size = multi_res.size();
                for (size_t i = 0; i < size; i++)
                {
                    const size_t old_size = set.size();
                    std::set< Edge > ts;
                    for (const auto& ed : multi_res[i])
                    {
                        ts.insert(ed);
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