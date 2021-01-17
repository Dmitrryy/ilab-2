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

        void connect(size_t v1, size_t v2, float resistance, float eds)
        {
            //vertex id is index of the line in the graph
            m_graph.resize(std::max(m_graph.getLines(), static_cast<size_t>(std::max(v1, v2)) + 1), m_graph.getColumns() + 1);
            m_graph.at(v1, m_graph.getColumns() - 1) = 1;
            m_graph.at(v2, m_graph.getColumns() - 1) = 1;

            //id is index in the array m_data
            Edge edge(v1, v2);
            edge.id = m_data.size();
            edge.resistance = resistance;
            edge.eds = eds;

            m_data.push_back(edge);
        }

        //std::vector< Edge > getData() const { return m_data; }
        matrix::Matrix< float > getCurrents() const
        {
            matrix::Matrix< float > res(m_graph.getLines(), m_graph.getLines());

            for(const auto& ed : m_data)
            {
                res.at(ed.v1, ed.v2) = ed.current.value();
            }

            return res;
        }

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
            //assert(solv.second.isZero());
            //std::cout << solv.first << std::endl;
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
                    out << ' ' << cur.eds.value_or(0) << " V; " << cur.current.value_or(0) << " A";
                }
                out << std::endl;
            }
            out << m_graph << std::endl;

            return out.str();
        }


        std::vector< std::vector< Edge > > findCycles() const
        {
            if (!m_data.empty())
                return findCycles_(m_data[0].v1, {});
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
        std::vector< std::vector< Edge > > findCycles_(size_t cur, std::vector< Edge > trace) const
        {
            std::vector< std::vector< Edge > > multi_res;

            /*
             * algorithm:
             * Figuratively speaking, it is a snake.
             * Recursively walk along the edges and if you stumble on your tail, fix the cycle.
             */

            const size_t columns = m_graph.getColumns();
            for (size_t c = 0; c < columns; c++)
            {
                if (m_graph.at(cur, c) == 1)
                {
                    const Edge next_edg = m_data[c];
                    if (!trace.empty() && next_edg == trace.back()) {
                        continue;
                    }

                    const size_t next_vert = (cur == next_edg.v1) ? next_edg.v2 : next_edg.v1;
                    auto tmp_trace = trace;
                    tmp_trace.push_back(next_edg);
                    auto it = checkCircuitCloser_(tmp_trace.cbegin(), tmp_trace.cend());

                    if (it != tmp_trace.cend()) {
                        multi_res.emplace_back(it, tmp_trace.cend());
                    }
                    else {
                        auto re_res = findCycles_(next_vert, tmp_trace);
                        multi_res.insert(multi_res.end(), re_res.begin(), re_res.end());
                    }

                }

            }

            //remove the same cycles
            removeSameExcludingOrder_< Edge >(multi_res);
            return multi_res;
        }



        static std::vector< Edge >::const_iterator
        checkCircuitCloser_(std::vector< Edge >::const_iterator begin, std::vector< Edge >::const_iterator end)
        {
            if (end - begin == 1 && begin->v1 == begin->v2) {
                return begin;
            }
            else if (end - begin < 2 || (end - 1)->id == (end - 2)->id) {
                return end;
            }
            const Edge& last_edge = *(end - 1);
            const Edge& prev_last = *(end - 2);
            if (std::set< size_t >{ last_edge.v1, last_edge.v2 } == std::set< size_t >{ prev_last.v1, prev_last.v2 }) {
                //loop on two vertices
                return end - 2;
            }

            const size_t last_vert = (last_edge.v1 == prev_last.v1 || last_edge.v1 == prev_last.v2) ? last_edge.v2 : last_edge.v1;
            auto it = std::find_if(begin, end, [last_vert](const Edge& cur) {
                return cur.v1 == last_vert || cur.v2 == last_vert;
            });
            if (end - it > 2)
            {
                auto tmp_it = it + 1;
                if (tmp_it->v1 == last_vert || tmp_it->v2 == last_vert) {
                    it = tmp_it;
                }
            }
            if (end - it > 2) {
                return it;
            }
            return end;
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

            ///

            multi_vec = std::move(res);
        }

    };//class Circuit

}//namespace ezg