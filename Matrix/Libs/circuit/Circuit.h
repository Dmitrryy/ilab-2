#pragma once

#include <matrix/Matrix.h>
#include <vector>
#include <optional>
#include <sstream>
#include <set>

#include "Edge.h"

namespace ezg
{
    class Circuit
    {
        matrix::Matrix< int > m_graph;
        std::vector< Edge >    m_data;

    public:

        void connect(Edge edge)
        {
            //vertex id is index of the line in the graph
            m_graph.resize(std::max(m_graph.getLines(), static_cast<size_t>(std::max(edge.v1, edge.v2)) + 1), m_graph.getColumns() + 1);
            m_graph.at(edge.v1, m_graph.getColumns() - 1) = 1;
            m_graph.at(edge.v2, m_graph.getColumns() - 1) = 1;

            //id is index in the array m_data
            edge.id = m_data.size();
            m_data.emplace_back(edge);
        }

        std::vector< Edge > getData() const { return m_data; }


        void calculateCurrent()
        {
            const size_t mGlines = m_graph.getLines();
            const size_t mGcolumns = m_graph.getColumns();

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

            /*
             * We make a system of equations using the first and second Kirchhoff rules.
             */
            const size_t num_cycles = cycles.size();
            matrix::Matrix< double > LSystem(num_cycles + mGlines, m_data.size());
            std::vector< double > freeMembers(num_cycles + mGlines);
            for (size_t c = 0; c < num_cycles; c++)
            {//second rule
                auto line = secondRuleKirchhoff_(cycles[c]);
                assert(line.size() - 1 == LSystem.getColumns());

                for (size_t clm = 0; clm < mGcolumns; clm++) {
                    LSystem.at(c, clm) = line[clm];
                }
                freeMembers[c] = line.back();
            }

            //first rule
            for (size_t l = 0; l < mGlines; l++)
            {
                for(size_t c = 0; c < mGcolumns; c++)
                {
                    if (m_graph.at(l, c) != 0)
                    {
                        if (m_data[c].v1 != m_data[c].v2) {
                            LSystem.at(num_cycles + l, c) = ((m_data[c].v2 == l) ? 1.f : -1.f);
                        }
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


        std::vector< std::vector< Edge > > findCycles() const
        {
            if (!m_data.empty())
                return findCycles_(m_data[0].v1, m_data[0].v1, {});
            return {};
        }


    private:

        std::vector< float > secondRuleKirchhoff_(const std::vector< Edge >& cycle) const
        {
            double eds = 0;
            const size_t num_edges = cycle.size();
            std::vector< float > result(m_data.size() + 1);
            size_t pre1 = 0, pre2 = 0;
            for (size_t k = 0; k < num_edges; k++)
            {
                const Edge& cur = cycle[k];

                //is determined by the sign of the bypass circuit
                bool minus = false;
                if (k == 0) {
                    pre1 = cycle.back().v1;
                    pre2 = cycle.back().v2;
                }

                if (cur.v1 != pre1 && cur.v1 != pre2)
                    minus = true;
                pre1 = cur.v1;
                pre2 = cur.v2;

                eds += cur.eds.value_or(0) * ((minus) ? -1.f : 1.f);

                result.at(cur.id) = cur.resistance.value() * ((minus) ? -1.f : 1.f);
            }
            result.back() = eds;

            return result;
        }

        /*
         * searches for cycles in the graph.
         *
         * first_vertex - first vertex of the trace
         * cur - current vertex id
         * trace - story of the way(edges)
         */
        std::vector< std::vector< Edge > > findCycles_(size_t first_vert, size_t cur, std::vector< Edge > trace) const
        {
            std::vector< std::vector< Edge > > multi_res;

            /*
             * algorithm:
             * Figuratively speaking, it is a snake.
             * Recursively walk along the edges and if you stumble on your tail, fix the cycle.
             */

            const size_t columns = m_graph.getColumns();
            const size_t lines   = m_graph.getLines();
            const bool first_step = trace.empty();
            for (size_t c = 0; c < columns; c++)
            {
                if (m_graph.at(cur, c) == 1)
                {
                    const Edge next_edg = m_data[c];
                    const size_t next_vert = (cur == next_edg.v1) ? next_edg.v2 : next_edg.v1;
                    if (next_edg.v1 == next_edg.v2) {
                        multi_res.emplace_back(std::vector{ next_edg });
                    }
                    else if (first_step)
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

            //remove the same cycles
            removeSameExcludingOrder_< Edge >(multi_res);
            return multi_res;
        }


        template<typename T>
        static void removeSameExcludingOrder_(std::vector< std::vector< T > >& multi_vec)
        {
            std::vector< std::vector< Edge > > res;
            {
                std::set< std::set< Edge > > set;

                const size_t size = multi_vec.size();
                for (size_t i = 0; i < size; i++)
                {
                    const size_t old_size = set.size();
                    std::set< Edge > ts;
                    for (const auto& ed : multi_vec[i])
                    {
                        ts.insert(ed);
                    }
                    set.emplace(std::move(ts));
                    if (old_size < set.size())
                    {
                        res.emplace_back(std::move(multi_vec[i]));
                    }
                }
            }

            multi_vec = std::move(res);
        }

    };//class Circuit

}//namespace ezg