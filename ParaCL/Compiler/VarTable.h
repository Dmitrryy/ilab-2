
#pragma once

#include <unordered_map>
#include <vector>
#include <cassert>
#include <optional>
#include <stack>
#include <memory>

//#include "INode.h"
#include "IInfo.h"
#include "DSTable.h"

namespace ezg
{

    class ScopeNode_t final
    {
        ScopeNode_t*                         m_parent = nullptr;
        std::vector< ScopeNode_t* >          m_children;

        std::unordered_map< size_t, std::unique_ptr< IInfo_t > > m_data;

    public:

        void setParent(ScopeNode_t* pNode) { m_parent = pNode; }

        //ScopeNode_t* getParent() const { return m_parent; }
        void addChild(ScopeNode_t* pNode) { pNode->setParent(this); /**/ m_children.push_back(pNode); }

        void addObj(size_t id, std::unique_ptr< IInfo_t >&& data)
        {
            assert(m_data.count(id) == 0);
            m_data[id] = std::move(data);
        }

        std::optional< IInfo_t* > access(size_t id) const
        {
            if (m_parent == nullptr) {
                return access_cur_(id);
            }
            auto res = access_cur_(id);
            if (!res.has_value()) {
                res = m_parent->access(id);
            }
            return res;
        }

    private:

        std::optional< IInfo_t* > access_cur_(size_t id) const
        {
            auto res = m_data.find(id);
            if (res == m_data.end()) {
                return {};
            }
            return {res->second.get()};
        }
    };


    class ScopeTable final
    {
        std::vector< ScopeNode_t* > m_tables;
        std::stack< size_t > m_scopeIdStore;

    public:

        ScopeTable() = default;

        //not supported
        ScopeTable                   (const ScopeTable&) = delete;
        const ScopeTable& operator = (const ScopeTable&) = delete;
        ScopeTable                   (ScopeTable&&     ) = delete;
        const ScopeTable& operator = (ScopeTable&&     ) = delete;

        ~ScopeTable()
        {
            for(const auto& pN : m_tables) {
                delete pN;
            }
        }

        void entryScope(size_t id)
        {
            assert(id < m_tables.size());
            m_scopeIdStore.push(id);
        }
        void exitCurScope()
        {
            m_scopeIdStore.pop();
        }

        size_t getCurTableId() const noexcept { return m_scopeIdStore.top(); }

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

        std::optional< IInfo_t* > access(size_t id) const
        {
            return m_tables[m_scopeIdStore.top()]->access(id);
        }

        void addElem(size_t idTable, size_t idElem, std::unique_ptr< IInfo_t >&& data) { m_tables[idTable]->addObj(idElem, std::move(data)); }
    };


}//namespace ezg