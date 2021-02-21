/*************************************************************************
 *
 * Circuit.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include <matrix/Matrix.h>
#include <vector>
#include <optional>
#include <sstream>
#include <set>
#include <unordered_set>


namespace ezg
{
    class Circuit
    {
        //////
        //structure storing information about an edge
        struct Edge
        {
            Edge() = default;


            Edge(size_t vLeft, size_t vRight)
                    : v1(vLeft)
                    , v2(vRight)
            { }

            size_t id = 0;
            size_t v1 = 0, v2 = 0;
            std::optional< float > resistance;
            std::optional< float > current;
            std::optional< float > eds;

            bool operator == (const Edge& that) const { return id == that.id; }
            bool operator != (const Edge& that) const { return !(*this == that); }
        };//struct Edge
        //////

        using Edgelt = std::vector< Edge >::const_iterator;

    private:
        //incidence combinatorics
        matrix::Matrix< int > m_graph;
        std::vector< Edge >   m_data;

    public:
        //adds an edge between 'v1' and 'v2' with 'resistance' and 'eds'
        void connect(size_t v1, size_t v2, float resistance, float eds);

        //counts the current in the circuit
        void calculateCurrent();

        //returns a vector of loops. cycles on the same edges,
        //but with different cyclic arrangement are considered different
        std::vector< std::vector< Edge > > findCycles() const;

        void printCurrents(std::ostream& out = std::cout) const;

        template< typename T >
        void dump(std::basic_ostream<T>& out) const
        {
            for (const auto& cur : m_data)
            {
                out << cur.v1 << " -- " << cur.v2 << ", " << cur.resistance.value_or(0) << ';';
                if (cur.eds.has_value()) {
                    out << ' ' << cur.eds.value_or(0) << " V; " << cur.current.value_or(0) << " A";
                }
                out << std::endl;
            }
            out << m_graph << std::endl;
        }

    private:

        std::vector< float > secondRuleKirchhoff_(const std::vector< Edge >& cycle) const;

        std::vector< std::vector< Edge > > findCycles_(size_t cur, std::vector< Edge > trace, std::unordered_set< size_t >& unused_vertices) const;
        Edgelt checkCircuitCloser_(Edgelt begin, Edgelt end) const;

    };//class Circuit

}//namespace ezg