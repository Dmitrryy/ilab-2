#pragma once

#include <unordered_map>

/*
 * double-side table
 */

namespace ezg
{

    template< typename Var1_, typename Var2_ >
    class dst
    {
        std::unordered_map< Var1_, Var2_ > m_1To2;
        std::unordered_map< Var2_, Var1_ > m_2To1;

    public:

        void add(const Var1_& lhs, const Var2_& rhs) { m_1To2[lhs] = rhs; /**/ m_2To1[rhs] = lhs; }

        size_t countLeft(const Var1_& id) const { return m_1To2.count(id); }
        size_t countRight(const Var2_& id) const { return m_2To1.count(id); }

        const Var2_& getRight(const Var1_& right) const { return m_1To2.at(right); }
        const Var1_& getLeft(const Var2_& right) const { return m_2To1.at(right); }
    };//class dst

}// namespace ezg