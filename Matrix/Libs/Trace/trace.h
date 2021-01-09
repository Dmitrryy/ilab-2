#pragma once

#include <map>
#include <list>
#include <vector>

#include <algorithm>

namespace ezg
{

    template <typename T>
    class Trace
    {
        std::map<T, typename std::vector< T >::iterator > m_tToList;
        std::vector< T > m_data;

    public:

        Trace(size_t size = 100)
        {
            m_data.reserve(size);
        }

        void step(const T& val)
        {
            m_data.push_back(val);
            m_tToList[val] = m_data.end() - 1;
        }

        bool contain(const T& val) const
        {
            auto it = m_tToList.find(val);

            if (it == m_tToList.end())
            {
                return false;
            }
            return true;
        }

        bool empty() const noexcept { return m_data.empty(); }

        std::vector< T > getWay(const T& a, const T& b) const
        {
            auto itA = m_tToList.find(a);
            auto itB = m_tToList.find(b);

            std::vector < T > result;

            if (itA != m_tToList.end() && itB != m_tToList.end())
            {
                int i = itB->second - itA->second;
                result.insert(result.end(), itA->second, itB->second);
            }

            return result;
        }

        T lastStep() const { return m_data.back(); }
    };
}