#pragma once

#include <vector>
#include <stack>
#include <optional>
#include <algorithm>

#include <matrix/Matrix.h>

#include "../../binary_tree/binary_tree.h"

#define LEFT_CHILD_NAME "0"
#define RIGHT_CHILD_NAME "1"

namespace ezg
{

    template< typename T >
    class MatrixChain
    {
    public:

        template< typename Val_ >
        using tree = ezg::binary_tree_t< Val_ >;

        using matrix_type = matrix::Matrix< T >;
        using matrix_tree = tree< matrix_type >;

    private:

        std::vector< matrix_type > m_chain;

    public:

        size_t add(const matrix_type& matrix);

        tree< size_t > optimalOrderId() const;

        tree< size_t > defaultOrderId() const;

        matrix_type optimalMultiplication() const;

        matrix_type defaultMultiplication() const;


    private:

        matrix_tree buildMatrixTree(const tree< size_t >& index_tree) const;

        matrix_type multiplicationByTree_(matrix_tree mTree) const;
    };//class MatrixChain


}//namespace ezg


#include "MatrixChain.inl"