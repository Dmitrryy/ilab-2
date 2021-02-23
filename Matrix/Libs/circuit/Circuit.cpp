/*************************************************************************
 *
 * Circuit.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#include "Circuit.h"

namespace ezg
{
    //---------------------------------------------------------------------
    //public members

    void Circuit::connect(size_t v1, size_t v2, float resistance, float eds)
    {
        //vertex id is index of the line in the combinatorics
        m_graph.resize(std::max(m_graph.getLines(), std::max(v1, v2)) + 1, m_graph.getColumns() + 1);
        m_graph.at(v1, m_graph.getColumns() - 1) = 1;
        m_graph.at(v2, m_graph.getColumns() - 1) = 1;

        //id is index in the array m_data
        Edge edge(v1, v2);
        edge.id = m_data.size();
        edge.resistance = resistance;
        edge.eds = eds;

        m_data.push_back(edge);
    }


    void Circuit::calculateCurrent()
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
        std::cout << "\nline system:\n";
            std::cout << LSystem << std::endl;
#endif
        auto solv = LSystem.solve(freeMembers);
        //assert(solv.second.isZero());
#ifdef DEBUG
        std::cout << "\nSolution system:\n";
            std::cout << solv.first << std::endl;
#endif
        for (size_t c = 0; c < mGcolumns; c++) {
            m_data[c].current = solv.first.at(c, 0);
        }
    }


    std::vector< std::vector< Circuit::Edge > > Circuit::findCycles() const
    {
        //used to quickly find and remove vertices
        std::unordered_set< size_t > vertices;
        for (const auto& edg : m_data) {
            vertices.insert(edg.v1);
            vertices.insert(edg.v2);
        }

        std::vector< std::vector< Edge > > res;
        while(!vertices.empty()) {//in the case of a disconnected combinatorics
            auto part_res = findCycles_(*vertices.begin(), {}, vertices);

            //move the result to a common container
            res.insert(res.end()
                    , std::make_move_iterator(part_res.begin())
                    , std::make_move_iterator(part_res.end()));
        }

        return res;
    }


    void Circuit::printCurrents(std::ostream& out /*= std::cout*/) const
    {
        for (const auto& edge : m_data) {
            out << edge.v1 << " -- " << edge.v2 << ": " << ((std::abs(edge.current.value()) < EPSIL) ? 0.f : edge.current.value())
                << " A" << std::endl;
        }
    }

    //---------------------------------------------------------------------
    //private members

    std::vector< float > Circuit::secondRuleKirchhoff_(const std::vector< Edge >& cycle) const
    {
        double eds = 0;
        const size_t num_edges = cycle.size();
        std::vector< float > result(m_data.size() + 1);
        size_t pre = 0;
        float sum_resistance = 0.f;

        for (size_t k = 0; k < num_edges; k++)
        {
            const Edge& cur = cycle[k];

            //is determined by the sign of the bypass circuit
            float sign = 1;
            if (k == 0 && num_edges > 1) {
                pre = (cycle[0].v1 == cycle[1].v1 || cycle[0].v1 == cycle[1].v2) ? cycle[0].v2 : cycle[0].v1;
            }

            if (cur.v1 != pre) {
                pre = cur.v1;
                sign = -1;
            }
            else {
                sign = 1;
                pre = cur.v2;
            }

            eds += cur.eds.value_or(0) * sign;
            sum_resistance += cur.resistance.value() * sign;

            result.at(cur.id) = cur.resistance.value() * sign;
        }
        result.back() = eds;

        if (sum_resistance == 0 && eds != 0) {
            throw std::runtime_error("infinite current loop detected");
        }

        return result;
    }


    std::vector< std::vector< Circuit::Edge > > Circuit::findCycles_(size_t cur, std::vector< Edge > trace, std::unordered_set< size_t >& unused_vertices) const
    {
        std::vector< std::vector< Edge > > multi_res;

        /*
         * algorithm:
         * Figuratively speaking, it is a snake.
         * Recursively walk along the edges and if you stumble on your tail, fix the cycle.
         */
        unused_vertices.erase(cur);

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
                    auto re_res = findCycles_(next_vert, tmp_trace, unused_vertices);
                    multi_res.insert(multi_res.end(), re_res.begin(), re_res.end());
                }

            }

        }

        return multi_res;
    }



    Circuit::Edgelt Circuit::checkCircuitCloser_(Circuit::Edgelt begin, Circuit::Edgelt end) const
    {
        if (end - begin >= 1 && (end - 1)->v1 == (end - 1)->v2) {
            return end - 1;
        }
        else if (end - begin < 2 || (end - 1)->id == (end - 2)->id) {
            return end;
        }
        const Edge& last_edge = *(end - 1);
        const Edge& prev_last = *(end - 2);
        if ((last_edge.v1 == prev_last.v1 && last_edge.v2 == prev_last.v2)
            || (last_edge.v2 == prev_last.v1 && last_edge.v1 == prev_last.v2))
        {
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

}//namespace ezg