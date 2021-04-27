/*************************************************************************************************
 *
 *   DeleteonQueue.hpp
 *
 *   Created by dmitry
 *   05.04.2021
 *
 ***/

#pragma once

#include <functional>
#include <vector>


namespace ezg
{

    class DeletionQueue_t final
    {
        std::vector< std::function< void() > > m_queue;

    public:

        ~DeletionQueue_t() {
            flush();
        }

        void push(const std::function< void() >& func)
        {
            m_queue.push_back(func);
        }

        void flush()
        {
            for(auto&& end = m_queue.rend(), it = m_queue.rbegin(); it != end; ++it) {
                (*it)();
            }
            m_queue.clear();
        }
    };//class DeletionQueue_t

}//namespace ezg