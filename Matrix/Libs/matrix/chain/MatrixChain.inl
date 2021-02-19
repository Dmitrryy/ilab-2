//===----------------------------------------------------------------------===//
///
///  Created by Dmitryyy (https://github.com/Dmitrryy)
///
///  Allow you to write off it.
///
//===----------------------------------------------------------------------===//


//
/// MatrixChain
///-----------
///
/// In this solution, we will consider approaches to multiplying a chain of matrices.
/// As a rule, the multiplication of matrices of order is not optimal. Let's say you
/// have a 10x30 A, 30 x 5V, and 5x60 S matrix.
///
/// Calculating their product as A (BC) would take(30 × 5 × 60) + (10 × 30 × 60) = 27000
/// multiplications. At the same time, their product as (A B) C will give the same result
/// in 4500 operations.
///
/// As a solution, it is proposed to reduce the task to subtasks. Denote the result of
/// multiplying the matrices A (i) * A(i+1) * ... * A(j), where i <= j. If i < j, then
/// there is a 'k' that splits this chain between the matrices A(k) and A(k+1), i <= k < j.
/// That is, to calculate A (i,..., j), first calculate A(i,..., k),
/// then A(k+1,..., j) and then multiply them.
/// Going through all 'k', we select such that the number of required operations is minimal.
///
/// Choosing 'k' is equivalent to placing parentheses.
/// For example, for some matrices, the following multiplication order is optimal:
/// ((A * B) * (C * D))
/// It is convenient to represent such a record as a binary tree.
///
/// Two types of trees can be found in algorithms:
/// 1. Index tree. A tree whose leaves contain the numbers of matrices (equal to the ID in
/// the array of matrices), and in the nodes the number of operations required for
/// multiplying children. It is constructed using methods buildOptimalOrderTree_
/// and buildDefaultOrderTree_.
/// 2. Matrix tree. it looks the same as the index tree, but the leaves contain the
/// matrices corresponding to the index in the index tree.
/// The space in the nodes is reserved for the result of multiplying children.
/// It is constructed using methods buildMatrixTree_.
///
///
//===----------------------------------------------------------------------===//

#pragma once


namespace ezg
{

    template< typename T >
    size_t MatrixChain< T >::addMatrix(const matrix_type& matrix)
    {
        if (!m_chain.empty() && matrix.getLines() != m_chain.back().getColumns()) {
            throw std::invalid_argument("invalid matrix size");
        }
        const size_t idMatrix = m_chain.size();

        //If the order was optimal, then recalculate the new one.
        if(m_isOptimal) {
            m_isOptimal = false;
            m_chain.push_back(matrix);
            setOptimalOrder();
        }
        else {
            if (m_chain.empty())
            {//the tree contains one node
                const auto idRoot = m_curOrderTree.createNode(idMatrix);
                m_curOrderTree.setRootId(idRoot);
            }
            else
            {//attach a new node with the number of the new matrix on the right.
                /*
                 *              newIdRoot
                 *             /        \
                 *         oldTree    idMatrixNode
                 */
                const size_t actions = m_curOrderTree.at(m_curOrderTree.getRootId())
                        + m_chain[0].getLines() * matrix.getLines() * matrix.getColumns();
                const auto newIdRoot = m_curOrderTree.createNode(actions);
                const auto IdMatrixNode = m_curOrderTree.createNode(idMatrix);

                m_curOrderTree.setLeft(newIdRoot, m_curOrderTree.getRootId());
                m_curOrderTree.setRight(newIdRoot, IdMatrixNode);

                m_curOrderTree.setRootId(newIdRoot);
            }

            m_chain.push_back(matrix);
        }

        return idMatrix;
    }


    template< typename T >
    void MatrixChain< T >::setOptimalOrder()
    {
        if (!m_isOptimal)
        {
            m_curOrderTree = buildOptimalOrderTree_();
            m_isOptimal = true;
        }
    }


    template< typename T >
    void MatrixChain< T >::setDefaultOrder()
    {
        if (m_isOptimal)
        {
            m_curOrderTree = buildDefaultOrderTree_();
            m_isOptimal = false;
        }
    }


    template< typename T >
    size_t MatrixChain< T >::getNumReqOperations() const
    {
        if (m_chain.size() <= 1u) {
            return 0;
        }

        return m_curOrderTree.at(m_curOrderTree.getRootId());
    }


    template< typename T >
    typename MatrixChain< T >::matrix_type MatrixChain< T >::multiplication() const
    {
        return multiplicationByTree_( buildMatrixTree_( m_curOrderTree ) );
    }


    //private:


    template< typename T >
    ezg::binary_tree_t< size_t > MatrixChain< T >::buildOptimalOrderTree_() const
    {
        if (m_chain.empty()) { return tree< size_t >(); }


        tree< size_t > res_tree;
        //the matrix contains the indices of the nodes in the tree.
        //the node with coordinates i, j contains the optimal order
        //of multiplication of matrices Аi .... Aj.
        matrix::Matrix< size_t > indexTrees(m_chain.size(), m_chain.size());
        for (size_t l = 0, ml = indexTrees.getLines(); l < ml; l++) {
            for (size_t c = l, mc = indexTrees.getColumns(); c < mc; c++) {
                indexTrees.at(l, c) = res_tree.createNode(/*std::numeric_limits< size_t >::max()*/);
            }
        }

        //stores the results of calculations to avoid repeated.
        //O(n!) -> O(n^3)
        matrix::Matrix< std::optional< size_t > > minActions(m_chain.size(), m_chain.size(), matrix::Order::Column);

        //recursion stack.
        //  first - i
        //  second - j
        //      next step find minAction[i][j].
        std::stack< std::pair< size_t, size_t > > recursionStack;
        recursionStack.push({ 0, m_chain.size() - 1 });

        while(!recursionStack.empty())
        {
            const size_t oldStackSize = recursionStack.size();

            const std::pair< size_t, size_t > curPair = recursionStack.top();
            const size_t left = curPair.first;
            const size_t right = curPair.second;

            if (minActions.at(left, right).has_value())
            {//the value has already been calculated.
                recursionStack.pop();
                continue;
            }

            if (left == right)
            {//the range of length zero is the matrix. zero operations required.
                minActions.at(left, left) = 0;
                res_tree.at(indexTrees.at(left, left)) = left;

                recursionStack.pop();
                continue;
            }

            for (size_t k = left; k < right; k++)
            {//add to the stack all available permutations in the current range.
                if(!minActions.at(left, k).has_value()) {
                    recursionStack.push({ left, k });
                }
                if(!minActions.at(k + 1, right).has_value()) {
                    recursionStack.push({ k + 1, right });
                }
            }

            if (recursionStack.size() == oldStackSize)
            {//all elements are ready
                size_t sought_k = 0;
                size_t resActions = std::numeric_limits< size_t >::max();
                const size_t leftSize = m_chain.at(left).getLines();
                const size_t rightSize = m_chain.at(right).getColumns();

                for (size_t k = left; k < right; k++)
                {
                    const size_t curActions = minActions.at(left, k).value()
                                              + minActions.at(k + 1, right).value()
                                              + leftSize * m_chain.at(k).getColumns() * rightSize;

                    if (curActions < resActions) {
                        resActions = curActions;
                        sought_k = k;
                    }
                }
                const size_t idLR = indexTrees.at(left, right);
                assert(resActions != std::numeric_limits< size_t >::max());
                assert(!res_tree.getLeftOptional(idLR).has_value());
                assert(!res_tree.getRightOptional(idLR).has_value());

                minActions.at(left, right) = resActions;
                res_tree.at(idLR) = resActions;


                res_tree.setLeft(idLR, indexTrees.at(left, sought_k));
                res_tree.setRight(idLR, indexTrees.at(sought_k + 1, right));
            }
        }

        assert(minActions.at(0, m_chain.size() - 1).has_value());

        //std::cout << minActions << std::endl;

        res_tree.setRootId(indexTrees.at(0, m_chain.size() - 1));

        return res_tree;
    }


    template< typename T >
    ezg::binary_tree_t< size_t > MatrixChain< T >::buildDefaultOrderTree_() const
    {
        tree< size_t > result;
        if (m_chain.size() == 0) { return result; }

        result.createNode(0);
        if (m_chain.size() == 1) { return result; }


        result.setLeft(0, result.createNode(0));
        result.setRight(0, result.createNode(1));
        result.at(0) = m_chain[0].getLines() * m_chain[0].getColumns() * m_chain[1].getColumns();

        for(size_t k = 2, mk = m_chain.size(); k < mk; k++)
        {
            const size_t actions = result.at(result.getRootId()) + m_chain[0].getLines() * m_chain[k].getColumns() * m_chain[k].getLines();

            const size_t idNewNode = result.createNode(actions);
            result.setLeft(idNewNode, result.getRootId());
            result.setRight(idNewNode, result.createNode(k));

            result.setRootId(idNewNode);
        }

        return result;
    }


    template< typename T >
    typename MatrixChain< T >::matrix_type MatrixChain< T >::multiplicationByTree_(matrix_tree tree) const
    {
        std::stack< size_t > recursionStack;
        recursionStack.push(tree.getRootId());

        while(!recursionStack.empty())
        {
            const size_t curNode = recursionStack.top();
            matrix_type& curMatrix = tree.at(curNode);

            //assert(curNode->size() == 2 || curNode->empty());

            if (curMatrix.empty() && tree.haveChildren(curNode))
            {
                const size_t left = tree.getLeftOptional(curNode).value();
                const size_t right = tree.getRightOptional(curNode).value();

                if (tree.at(left).empty()) {
                    recursionStack.push(left);
                    continue;
                }
                if (tree.at(right).empty()) {
                    recursionStack.push(right);
                    continue;
                }

                curMatrix = std::move(tree.at(left));
                curMatrix.multiplication(tree.at(right));

                recursionStack.pop();
            }

        }

        return tree.at(tree.getRootId());
    }


    template< typename T >
    typename MatrixChain< T >::matrix_tree MatrixChain< T >::buildMatrixTree_(const tree< size_t >& index_tree) const
    {
        if (m_chain.empty()) { return {}; }

        matrix_tree result;
        result.resize(index_tree.size());

        const size_t nNodes = index_tree.size();

        for (size_t k = 0; k < nNodes; k++)
        {
            if(!index_tree.haveChildren(k)) {
                result.at(k) = m_chain[index_tree.at(k)];
            }
            else
            {
                result.setLeft(k, index_tree.getLeftOptional(k).value());
                result.setRight(k, index_tree.getRightOptional(k).value());
            }
        }
        result.setRootId(index_tree.getRootId());

        return result;
    }
}//namespace ezg