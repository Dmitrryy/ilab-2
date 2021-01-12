#pragma once

#include "INode.h"

#include <sstream>
#include <iostream>
#include <map>

namespace ezg
{

    struct Value final : public INode
    {
        const int val;

        explicit Value(int nVal)
            : val(nVal)
        {}

        int execute() override { return val; }

        std::string dumpStr() const override
        {
            std::ostringstream out;
            out << "Type: Value" << std::endl
            << "Val = " << val << std::endl;

            return out.str();
        }
    };


    class Scope final : public IScope
    {
        std::vector< INode* > m_nodes;
        size_t m_idTable;

    public:

        explicit Scope(size_t idTable)
            : m_idTable(idTable)
        {}

        std::optional< size_t > visible(const std::string& var_name) override;
        std::optional< size_t > declareVar(const std::string& var_name) override;

        void addNode(INode* nNode) override { m_nodes.push_back(nNode); }
        void insertNode(const std::vector< INode* > &vec) override { m_nodes.insert(m_nodes.end(), vec.begin(), vec.end()); }

        void entry() override;
        void exit() override;

        int execute() override;

        std::string dumpStr() const override
        {
            std::ostringstream out;
            out << "Scope.\n"
                << "size = " << m_nodes.size() << '\n'
                <<  "They are:\n";
            for (auto m_node : m_nodes)
            {
                out << m_node->dumpStr() << std::endl;
            }
            return out.str();
        }
    };


    class Variable final : public INode
    {
        size_t m_id;

    public:

        explicit Variable(size_t id)
            : m_id(id)
        {}

        int execute() override { return getVal(); }
        int getVal() const;
        void setVal(int v) const;

        std::string dumpStr() const override
        {
        }
    };

    ///////////////////////////////////////////////////////////////////

    class While_t final : public INode
    {
        INode* m_condition;
        INode* m_scope;

    public:

        While_t(INode* condition, INode* scope)
        : m_condition(condition)
        , m_scope(scope)
        {}

        int execute() override
        {
            while(m_condition->execute())
            {
                m_scope->execute();
            }
        }

        std::string dumpStr() const override
        {
        }
    };


    class If_t final : public INode
    {
        INode* m_condition;
        INode* m_scope;

    public:

        If_t(INode* condition, INode* scope)
                : m_condition(condition)
                , m_scope(scope)
        {}

        int execute() override
        {
            if(m_condition->execute())
            {
                m_scope->execute();
            }
            return 0;
        }

        std::string dumpStr() const override
        {
        }
    };


    class Print_t final : public INode
    {
        INode* m_target;

    public:

        explicit Print_t(INode* target)
                : m_target(target)
        {}

        int execute() override
        {
            std::cout << m_target->execute() << std::endl;
            return 0;
        }

        std::string dumpStr() const override
        {
        }
    };


    class QMark_t final : public INode
    {
    public:

        int execute() override {
            int val = 0;
            std::cin >> val;
            return val;
        }

        std::string dumpStr() const override
        {
        }
    };

    //////////////////////////////////////////////////////////////////


    class Assign : public INode
    {
        Variable* m_pVariable;
        INode* m_pValue;

    public:

        Assign(Variable* var, INode* val)
        : m_pVariable(var)
        , m_pValue(val)
        {}

        int execute() override {
            m_pVariable->setVal(m_pValue->execute());
            return 0;
        }

        std::string dumpStr() const override
        {
        }
    };


    class Op_t : public INode
    {
    protected:
        INode* m_left;
        INode* m_right;

    public:

        Op_t(INode* left, INode* right)
        : m_left(left)
        , m_right(right)
        {}

        std::string dumpStr() const override
        {
        }
    };


#define OPERATION_TEMPLATE(name, op)                                                \
    class name final : public Op_t                                              \
    {                                                                               \
    public:                                                                         \
        name(INode* left, INode* right)                                             \
            : Op_t(left, right)                                                 \
        {}                                                                          \
        int execute() override { return m_left->execute() op m_right->execute();}   \
    };

    OPERATION_TEMPLATE(Greater,   >);
    OPERATION_TEMPLATE(Less,      <);
    OPERATION_TEMPLATE(Equal,    ==);
    OPERATION_TEMPLATE(NonEqual, !=);
    OPERATION_TEMPLATE(Add, +);
    OPERATION_TEMPLATE(Sub, -);
    OPERATION_TEMPLATE(Mul, *);
    OPERATION_TEMPLATE(Div, /);

}//namespace ezg