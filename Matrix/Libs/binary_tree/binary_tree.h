///=================================================================//
///
///  Created by Dmitryyy (https://github.com/Dmitrryy)
///
///  Allow you to write off it.
///
///=================================================================//
#pragma once

#include <vector>
#include <optional>

namespace ezg
{
    /// Class that implements a binary tree.
    /// \Features
    /// - The nodes are in a vector (Cache locality).
    ///         Instead of pointers to nodes, the cell numbers in the vector are used.
    /// - The tree can be unbound, but the root (m_root) is only one.
    /// - Сopying/moving is equivalent to copying/moving a single vector
    ///
    /// \Example
    ///  Nodes: |1|2|3|4|5|6|7|8|
    ///
    ///   Tree:
    ///                  5                8
    ///                /   \            /   \
    ///               1     2(root)    5    nop
    ///             /  \   |   \
    ///           nop   3  4    7
    ///
    /// \TODO the Union of two different trees
    ///
    /// \tparam T - type of Node's data
    template< typename T >
    class binary_tree_t
    {
        ///node class.
        ///Contains data and two numbers for children.
        class binary_tree_node_t
        {
        public:

            using val_type = T;

        private:

            ///number of the left child
            std::optional< size_t > m_idLeft;
            ///number of the right child
            std::optional< size_t > m_idRight;

            val_type                m_data = {};

        public:

            ///default constructor
            binary_tree_node_t() = default;

            ///constructor:
            ///set data = val;
            explicit binary_tree_node_t(const val_type& val)
                    : m_data(val)
            {}

            ///get the number of the left child
            std::optional< size_t > getLeftOptional  () const noexcept { return m_idLeft; }
            ///get the number of the right child
            std::optional< size_t > getRightOptional () const noexcept { return m_idRight; }

            bool haveChildren() const { return m_idRight.has_value() || m_idLeft.has_value(); }

            ///set the number of the left child
            void setLeft  (size_t id) noexcept { m_idLeft = id; }
            ///set the number of the right child
            void setRight (size_t id) noexcept { m_idRight = id; }

            ///detach the left child
            void unLinkLeft  () noexcept { m_idLeft.reset(); }
            ///detach the right child
            void unLinkRight () noexcept { m_idRight.reset(); }

            val_type&       data() noexcept { return const_cast< val_type& >(static_cast< binary_tree_node_t* >(this)->data()); }
            const val_type& data() const noexcept { return m_data; }
        };//class binary_tree_node_t

    private:

        using val_type = T;

        std::vector< binary_tree_node_t > m_nodes;
        ///number of the root node in the tree
        size_t                            m_root = 0;

    public:

        ///reserve 'capacity' memory for nodes
        void reserve (size_t capacity) { m_nodes.reserve(capacity); }

        ///if the new size is larger than the current size, new nodes will be available.
        ///     now available [0, size);
        ///else the last created nodes are overwritten,
        ///and the identifiers pointing to them are not valid
        void resize  (size_t size)     { m_nodes.resize(size); }

        ///get the number of nodes in the tree (counting unrelated)
        size_t size  () const noexcept { return m_nodes.size(); }

        ///makes a push back to the node vector with the value 'val'
        size_t createNode(const val_type& val = {}) { m_nodes.emplace_back(val); return m_nodes.size() - 1; }

        ///get the current root node id
        size_t getRootId () const noexcept { return m_root; }
        ///set the root node id
        void   setRootId (size_t id) noexcept { assert(m_root < m_nodes.size()); m_root = id;}

        ///-----------------------begin_node_interface---------------------------
        bool haveChildren(size_t idNode) const { return m_nodes.at(idNode).haveChildren(); }

        void setLeft  (size_t idNode, size_t idLeft) noexcept { m_nodes.at(idNode).setLeft(idLeft); }
        void setRight (size_t idNode, size_t idRight) noexcept { m_nodes.at(idNode).setRight(idRight); }

        std::optional< size_t > getLeftOptional  (size_t idNode) const noexcept { return m_nodes.at(idNode).getLeftOptional(); }
        std::optional< size_t > getRightOptional (size_t idNode) const noexcept { return m_nodes.at(idNode).getRightOptional(); }

        void unLinkLeft  (size_t idNode) noexcept { m_nodes.at(idNode).unLinkLeft(); }
        void unLinkRight (size_t idNode) noexcept { m_nodes.at(idNode).unLinkRight(); }

        val_type&       at(size_t id) noexcept { return const_cast< val_type& >(static_cast< const binary_tree_t* >(this)->at(id)); }
        const val_type& at(size_t id) const noexcept { return m_nodes.at(id).data(); }
        ///------------------------end_node_interface----------------------------
    };//class binary_tree_t

}//namespace ezg