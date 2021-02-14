#pragma once

#include <vector>
#include <stack>
#include <optional>
#include <algorithm>

#include <matrix/Matrix.h>

//this is how my acquaintance with boost library
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#define PRINT_STAT_MULTIPLICATION

#define LEFT_CHILD_NAME "0"
#define RIGHT_CHILD_NAME "1"

namespace ezg
{


    template< typename T >
    class MatrixChain
    {
    public:

        template< typename Val_ >
        using tree = boost::property_tree::basic_ptree< std::string, Val_ >;

        using matrix_type = matrix::Matrix< T >;
        using matrix_tree = tree< matrix_type >;

    private:

        std::vector< matrix_type > m_chain;

    public:

        size_t add(const matrix_type& matrix)
        {
            if (!m_chain.empty() && matrix.getLines() != m_chain.back().getColumns()) {
                throw std::invalid_argument("invalid matrix size");
            }
            m_chain.push_back(matrix);
            return m_chain.size() - 1;
        }

        tree< size_t > optimalOrderId() const
        {
            if (m_chain.empty()) { return {}; }

            matrix::Matrix< std::optional< size_t > > minActions(m_chain.size(), m_chain.size());
            matrix::Matrix< tree< size_t > > indexTrees(m_chain.size(), m_chain.size());

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
                    indexTrees.at(left, left).data() = left;

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
                    assert(resActions != std::numeric_limits< size_t >::max());
                    assert(indexTrees.at(left, right).empty());

                    minActions.at(left, right) = resActions;
                    indexTrees.at(left, right).data() = resActions;
                    indexTrees.at(left, right).put_child(LEFT_CHILD_NAME , indexTrees.at(left, sought_k));
                    indexTrees.at(left, right).put_child(RIGHT_CHILD_NAME, indexTrees.at(sought_k + 1, right));
                }

            }

            assert(minActions.at(0, m_chain.size() - 1).has_value());
            assert(indexTrees.at(0, m_chain.size() - 1).size() == 2);

            return indexTrees.at(0, m_chain.size() - 1);
        }

        matrix_type optimalMultiplication() const;

        matrix_type defaultMultiplication() const;


    private:

        matrix_tree buildMatrixTree(const tree< size_t >& index_tree) const
        {
            if (m_chain.empty()) { return {}; }

            matrix_tree result;

            std::stack< std::pair< matrix_tree *, tree<size_t> *> > recursionStack;
            recursionStack.push({&result, &index_tree});

            while (!recursionStack.empty())
            {
                const auto curPair = recursionStack.top();
                matrix_tree *mTree = curPair.first;
                tree<size_t> *idTree = curPair.second;

                assert(idTree->empty() || idTree->size() == 2);

                if(idTree->empty())
                {
                    mTree->data() = m_chain[idTree->data()];

                    recursionStack.pop();
                    continue;
                }
                else
                {
                    recursionStack.pop();

                    recursionStack.push( {&(mTree->put_child(LEFT_CHILD_NAME, {})), &(idTree->get_child(LEFT_CHILD_NAME))} );
                    recursionStack.push( {&(mTree->put_child(RIGHT_CHILD_NAME, {})), &(idTree->get_child(RIGHT_CHILD_NAME))} );
                }
            }

            return result;
        }

    };//class MatrixChain


}//namespace ezg


#include "MatrixChain.inl"