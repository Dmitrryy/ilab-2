

namespace ezg
{

    template< typename T >
    size_t MatrixChain< T >::add(const matrix_type& matrix)
    {
        if (!m_chain.empty() && matrix.getLines() != m_chain.back().getColumns()) {
            throw std::invalid_argument("invalid matrix size");
        }
        m_chain.push_back(matrix);
        return m_chain.size() - 1;
    }


    template< typename T >
    ezg::binary_tree_t< size_t > MatrixChain< T >::optimalOrderId() const
    {
        if (m_chain.empty()) { return tree< size_t >(); }

        matrix::Matrix< std::optional< size_t > > minActions(m_chain.size(), m_chain.size(), matrix::Order::Column);

        tree< size_t > res_tree;
        matrix::Matrix< size_t > indexTrees(m_chain.size(), m_chain.size());
        for (size_t l = 0, ml = indexTrees.getLines(); l < ml; l++) {
            for (size_t c = l, mc = indexTrees.getColumns(); c < mc; c++) {
                indexTrees.at(l, c) = res_tree.createNode();
            }
        }

        //recursion stack
        std::stack< std::pair< size_t, size_t > > recursionStack;
        recursionStack.push({ 0, m_chain.size() - 1 });

        while(!recursionStack.empty())
        {
            const size_t oldStackSize = recursionStack.size();

            const std::pair< size_t, size_t > curPair = recursionStack.top();
            const size_t left = curPair.first;
            const size_t right = curPair.second;

            if (minActions.at(left, right).has_value()) {
                recursionStack.pop();
                continue;
            }

            if (left == right) {
                minActions.at(left, left) = 0;
                res_tree.at(indexTrees.at(left, left)) = left;

                recursionStack.pop();
                continue;
            }

            for (size_t k = left; k < right; k++)
            {
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
                /////
                //need optimisation
                //indexTrees.at(left, right).add_child(LEFT_CHILD_NAME , indexTrees.at(left, sought_k));
                //indexTrees.at(left, right).add_child(RIGHT_CHILD_NAME, indexTrees.at(sought_k + 1, right));
                /////
            }
        }

        assert(minActions.at(0, m_chain.size() - 1).has_value());
        //assert(indexTrees.at(0, m_chain.size() - 1).size() == 2);

        //std::cout << minActions << std::endl;

        res_tree.setRootId(indexTrees.at(0, m_chain.size() - 1));

        return res_tree;
    }


    template< typename T >
    ezg::binary_tree_t< size_t > MatrixChain< T >::defaultOrderId() const
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
    typename MatrixChain< T >::matrix_type MatrixChain< T >::optimalMultiplication() const
    {
        matrix_tree tree = buildMatrixTree(optimalOrderId());

        return multiplicationByTree_(tree);
    }


    template< typename T >
    typename MatrixChain< T >::matrix_type MatrixChain< T >::defaultMultiplication() const
    {
        matrix_tree tree = buildMatrixTree(defaultOrderId());

        return multiplicationByTree_(tree);
    }


    //private:

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
    typename MatrixChain< T >::matrix_tree MatrixChain< T >::buildMatrixTree(const tree< size_t >& index_tree) const
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
        /*



        std::stack< std::pair< size_t, size_t > > recursionStack;
        recursionStack.push({ result.getRootId(), index_tree.getRootId()});

        while (!recursionStack.empty())
        {
            const auto curPair = recursionStack.top();
            const size_t mTree = curPair.first;
            const size_t idTree = curPair.second;

            //assert(idTree->empty() || idTree->size() == 2);

            if(!index_tree.getLeftOptional(idTree).has_value() && !index_tree.getRightOptional(idTree).has_value())
            {
                result.at(mTree) = m_chain[index_tree.at(idTree)];

                recursionStack.pop();
                continue;
            }
            else
            {
                recursionStack.pop();

                const size_t nLeft = result.createNode({});
                const size_t nRight = result.createNode({});
                result.setLeft(mTree, nLeft);
                result.setRight(mTree, nRight);

                recursionStack.push( { nLeft, index_tree.getLeftOptional(idTree).value() } );
                recursionStack.push( { nRight, index_tree.getRightOptional(idTree).value() } );

*//*                recursionStack.push( {&(mTree->put_child(LEFT_CHILD_NAME, {})), &(idTree->get_child(LEFT_CHILD_NAME))} );
                recursionStack.push( {&(mTree->put_child(RIGHT_CHILD_NAME, {})), &(idTree->get_child(RIGHT_CHILD_NAME))} );*//*
            }
        }*/

        return result;
    }
}//namespace ezg