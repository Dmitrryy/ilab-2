
#pragma once

#include <unordered_map>
#include <vector>
#include <cassert>
#include <optional>

#include "INode.h"


namespace ezg
{

    class VarTable final
    {
        std::unordered_map< size_t, int > m_data;

    public:

        int& access(size_t id) { return const_cast< int& >(static_cast< const VarTable* >(this)->access(id)); }
        const int& access(size_t id) const { return m_data.at(id); }

        void loadUp(const std::vector< size_t >& vId)
        {
            for (const auto& id : vId)
            {
                m_data[id] = 0;
            }
        }

        void unload(const std::vector< size_t >& vId)
        {
            for (const auto& id : vId)
            {
                assert(m_data.count(id) == 1);
                m_data.erase(id);
            }
        }
    };


    class ScopeNode_t final
    {
        ScopeNode_t*                         m_parent = nullptr;
        std::vector< ScopeNode_t* >          m_children;

        std::unordered_map< size_t, INode* > m_data;

    public:

        void setParent(ScopeNode_t* pNode) { m_parent = pNode; }
        void addChild(ScopeNode_t* pNode) { m_children.push_back(pNode); }
        void addObj(size_t id, INode* data)
        {
            assert(m_data.count(id) == 0);
            m_data[id] = data;
        }

        std::optional< INode* > access(size_t id) const
        {
            if (m_parent == nullptr) {
                return access_cur_(id);
            }
            return { access_cur_(id).value_or(m_parent->access(id).value_or(nullptr)) };
        }

    private:

        std::optional< INode* > access_cur_(size_t id) const
        {
            auto res = m_data.find(id);
            if (res == m_data.end()) {
                return {};
            }
            return {res->second};
        }
    };


    class ScopeTable final
    {
        std::vector< ScopeNode_t* > m_tables;
        size_t                      m_curTableId = 0;

    public:

        void setCurTable(size_t nId)
        {
            assert(nId < m_tables.size());
            m_curTableId = nId;
        }
        size_t getCurTableId() const noexcept { return m_curTableId; }
        size_t createTable(size_t prevId)
        {
            assert(prevId < m_tables.size());
            size_t newId = m_tables.size();
            auto* newNode = new ScopeNode_t();
            m_tables.push_back(newNode);

            newNode->setParent(m_tables[prevId]);
            m_tables[prevId]->addChild(newNode);

            return newId;
        }
        size_t createTable()
        {
            size_t newId = m_tables.size();
            auto* newNode = new ScopeNode_t();

            m_tables.push_back(newNode);

            return newId;
        }

        std::optional< INode* > access(size_t id) const
        {
            return m_tables[m_curTableId]->access(id);
        }

        void addElem(size_t idTable, size_t idElem, INode* data) { m_tables[idTable]->addObj(idElem, data); }

        ~ScopeTable()
        {
            for(const auto& pN : m_tables) {
                delete pN;
            }
        }
    };


}//namespace ezg