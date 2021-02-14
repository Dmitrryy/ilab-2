#pragma once

#include <vector>
#include <optional>

namespace ezg
{

    template< typename T >
    class binary_tree_t
    {

        class binary_tree_node_t
        {
        public:

            using val_type = T;

        private:

            std::optional< size_t > m_idLeft;
            std::optional< size_t > m_idRight;

            val_type                m_data;

        public:

            binary_tree_node_t() = default;
            binary_tree_node_t(const val_type& val)
                    : m_data(val)
            {}

            std::optional< size_t > getLeftOptional  () const noexcept { return m_idLeft; }
            std::optional< size_t > getRightOptional () const noexcept { return m_idRight; }

            bool haveChildren() const { return m_idRight.has_value() || m_idLeft.has_value(); }

            void setLeft  (size_t id) noexcept { m_idLeft = id; }
            void setRight (size_t id) noexcept { m_idRight = id; }

            void unLinkLeft  () noexcept { m_idLeft.reset(); }
            void unLinkRight () noexcept { m_idRight.reset(); }

            val_type&       data() noexcept { return const_cast< val_type& >(static_cast< binary_tree_node_t* >(this)->data()); }
            const val_type& data() const noexcept { return m_data; }
        };//class binary_tree_node_t


        using val_type = T;

        std::vector< binary_tree_node_t > m_nodes;
        size_t                            m_root = 0;

    public:

        void reserve(size_t capacity) { m_nodes.reserve(capacity); }
        void resize(size_t size) { m_nodes.resize(size); }
        size_t size() const noexcept { return m_nodes.size(); }

        size_t createNode(const val_type& val = {}) { m_nodes.emplace_back(val); return m_nodes.size() - 1; }

        bool haveChildren(size_t idNode) const { return m_nodes.at(idNode).haveChildren(); }

        void setLeft  (size_t idNode, size_t idLeft) noexcept { m_nodes.at(idNode).setLeft(idLeft); }
        void setRight (size_t idNode, size_t idRight) noexcept { m_nodes.at(idNode).setRight(idRight); }

        std::optional< size_t > getLeftOptional  (size_t idNode) const noexcept { return m_nodes.at(idNode).getLeftOptional(); }
        std::optional< size_t > getRightOptional (size_t idNode) const noexcept { return m_nodes.at(idNode).getRightOptional(); }

        void unLinkLeft  (size_t idNode) noexcept { m_nodes.at(idNode).unLinkLeft(); }
        void unLinkRight (size_t idNode) noexcept { m_nodes.at(idNode).unLinkRight(); }


        size_t getRootId() const noexcept { return m_root; }
        void setRootId(size_t id) noexcept { assert(m_root < m_nodes.size()); m_root = id;}

        val_type&       at(size_t id) noexcept { return const_cast< val_type& >(static_cast< const binary_tree_t* >(this)->at(id)); }
        const val_type& at(size_t id) const noexcept { return m_nodes.at(id).data(); }
    };//class binary_tree_t

}//namespace ezg