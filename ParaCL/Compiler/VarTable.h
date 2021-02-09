
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


    class ScopeTable final
    {

        class Node_t final
        {
            Node_t*                         m_parent = nullptr;
            std::vector< Node_t* >          m_children;

            std::unordered_map< size_t, std::unique_ptr< IInfo_t > > m_data;

            std::unordered_map< std::string, size_t > m_nameTable;
            //size_t m_freeNameId = 0;

        public:

            void setParent(Node_t* pNode) { m_parent = pNode; }

            //Node_t* getParent() const { return m_parent; }
            void addChild(Node_t* pNode) { pNode->setParent(this); /**/ m_children.push_back(pNode); }

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

            std::optional< size_t > declareName(const std::string& name)
            {
                static size_t g_freeNameId = 0;
                if (visibleName(name).has_value()) {
                    return {};
                }

                m_nameTable[name] = g_freeNameId;
                return g_freeNameId++;
            }

            std::optional< size_t > visibleName(const std::string& name) const
            {
                auto res = visibleNameCur_(name);
                if (m_parent != nullptr && !res.has_value()) {
                    res = m_parent->visibleName(name);
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

            std::optional< size_t > visibleNameCur_(const std::string& name) const
            {
                if (m_nameTable.count(name) == 0) {
                    return {};
                }
                return {m_nameTable.at(name)};
            }
        };//class Node_t


        std::vector< Node_t* > m_tables;
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
            auto* newNode = new Node_t();
            m_tables.push_back(newNode);

            newNode->setParent(m_tables[prevId]);
            m_tables[prevId]->addChild(newNode);

            return newId;
        }

        size_t createTable()
        {
            size_t newId = m_tables.size();
            auto* newNode = new Node_t();

            m_tables.push_back(newNode);

            return newId;
        }

        std::optional< IInfo_t* > access(size_t id) const
        {
            return m_tables[m_scopeIdStore.top()]->access(id);
        }

        std::optional< size_t > visibleName(size_t idTable, const std::string& name) const
        {
            return m_tables.at(idTable)->visibleName(name);
        }

        void addElem(size_t idTable, size_t idElem, std::unique_ptr< IInfo_t >&& data) { m_tables[idTable]->addObj(idElem, std::move(data)); }
        std::optional< size_t > declareName(size_t idTable, const std::string& name) { return m_tables[idTable]->declareName(name); }
    };//class ScopeTable


}//namespace ezg