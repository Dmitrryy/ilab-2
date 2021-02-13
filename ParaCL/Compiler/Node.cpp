#include "Node.h"
#include "VarTable.h"
#include <unordered_map>
#include <iostream>

#include <stack>

namespace ezg
{
    ScopeTable g_ScopeTable;

    bool g_needReturn = false;
    int g_returnValue = 0;


    std::optional<size_t> Scope::declareVariable(const std::string &var_name)
    {
        auto id = g_ScopeTable.declareName(m_idTable, var_name);
        if (!id.has_value()) {
            return {};
        }

        g_ScopeTable.addElem(m_idTable, id.value(), std::make_unique< VarInfo_t >());

        return id.value();
    }

    std::optional<size_t> Scope::visible(const std::string &name)
    {
        return g_ScopeTable.visibleName(m_idTable, name);
    }

    int Scope::execute()
    {
        entry();

        for (auto& curNode : m_nodes) {
            g_returnValue = curNode->execute();
            if (g_needReturn) {
                break;
            }
        }

        exit();

        return g_returnValue;
    }

    void Scope::entry()
    {
        g_ScopeTable.entryScope(m_idTable);
    }
    void Scope::exit()
    {
        assert(g_ScopeTable.getCurTableId() == m_idTable);
        g_ScopeTable.exitCurScope();
    }

    ////////////////////////////////////////////////////////////

    std::optional<size_t> FuncScope::declareVariable(const std::string &var_name)
    {
        auto id = g_ScopeTable.declareName(m_idTable, var_name);
        if (!id.has_value()) {
            return {};
        }

        g_ScopeTable.addElem(m_idTable, id.value(), std::make_unique< VarInfo_t >());

        return id.value();
    }

    int FuncScope::execute()
    {
        entry();

        for (auto& curNode : m_nodes) {
            g_returnValue = curNode->execute();
            if(g_needReturn) {
                g_needReturn = false;
                break;
            }
        }

        exit();

        return g_returnValue;
    }

    std::optional<size_t> FuncScope::visible(const std::string &name)
    {
        return g_ScopeTable.visibleName(m_idTable, name);
    }

    void FuncScope::entry()
    {
        g_ScopeTable.entryScope(m_idTable);
    }
    void FuncScope::exit()
    {
        assert(g_ScopeTable.getCurTableId() == m_idTable);
        g_ScopeTable.exitCurScope();
    }

    ////////////////////////////////////////////////////////////

    int Variable::getVal() const
    {
        VarInfo_t* pInfo = static_cast< VarInfo_t* >(g_ScopeTable.access(m_id).value());
        return pInfo->m_value;
    }
    void Variable::setVal(int v) const
    {
        VarInfo_t* pInfo = static_cast< VarInfo_t* >(g_ScopeTable.access(m_id).value());
        pInfo->m_value = v;
    }

    /////////////////////////////////////////////////////////////

    std::unique_ptr< IScope > IScope::make_separate()
    {
        size_t id = g_ScopeTable.createTable();
        return std::make_unique< Scope >(id);
    }

    std::unique_ptr< IScope > IScope::make_inside_current()
    {
        size_t cur_id = g_ScopeTable.getCurTableId();
        size_t id = g_ScopeTable.createTable(cur_id);
        return std::make_unique< Scope >(id);
    }



    ////////////////////////////////////////////////////////////

#define OpCase(name)             \
case Operator::name:             \
    res = std::make_unique< name >(left, right); \
    break;

    std::unique_ptr< INode > INode::make_op(Operator tOp, INode* left, INode* right)
    {
        if(left == nullptr || right == nullptr) { return nullptr; }
#ifdef DEBUG
        std::cout << "operator created: " << (int)tOp << std::endl;
        std::cout << "left:\n" << left->dumpStr() << "\nright:\n" << right->dumpStr() << std::endl;
#endif
        std::unique_ptr< INode > res = nullptr;
        switch (tOp) {
            OpCase(Greater);
            OpCase(Less);
            OpCase(LLess);
            OpCase(LGreater);
            OpCase(Equal);
            OpCase(NonEqual);
            OpCase(PLUS);
            OpCase(MINUS);
            OpCase(Mul);
            OpCase(Div);

            default:
                assert(0);
        }

        return res;
    }


    std::unique_ptr< INode > INode::make_unop(UnOp tOp, INode* arg)
    {
#ifdef DEBUG
        std::cout << "unary operator created: " << (int)tOp << std::endl;
        std::cout << "arg:\n" << arg->dumpStr() << std::endl;
#endif
        std::unique_ptr< INode > res = nullptr;
        switch (tOp) {
            case UnOp::MINUS:
                res = std::make_unique< UnMinus_t >(arg);
                break;

            case UnOp::PLUS:
                res = std::make_unique< UnPlus_t >(arg);
                break;

            default:
                assert(0);
        }

        return res;
    }


    std::unique_ptr< INode > INode::make_val(int val)
    {
#ifdef DEBUG
        std::cout << "value created: " << val << std::endl;
#endif
        return std::make_unique< Value >(val);
    }

    std::unique_ptr< INode > INode::make_assign(INode* var, INode* val)
    {
        if(var == nullptr || val == nullptr) { return nullptr; }
#ifdef DEBUG
        std::cout << "assign created:" << std::endl;
        std::cout << "var:\n" << var->dumpStr() << "\nval:\n" << val->dumpStr() << std::endl;
#endif
        return std::make_unique< Assign >(static_cast< Variable* >(var), val);
    }


    std::unique_ptr< INode > INode::make_var(size_t id)
    {
#ifdef DEBUG
        std::cout << "variable created: " << id << '(' << ')' << std::endl;
#endif
        return std::make_unique< Variable >(id);
    }

    std::unique_ptr< INode > INode::make_while(INode* condition, INode* scope)
    {
        if(condition == nullptr || scope == nullptr) { return nullptr; }
#ifdef DEBUG
        std::cout << "while created:" << std::endl;
        std::cout << "condition:\n" << condition->dumpStr() << "\nscope:\n" << scope->dumpStr() << std::endl;
#endif
        return std::make_unique< While_t >(condition, scope);
    }

    std::unique_ptr< INode > INode::make_if(INode* condition, INode* scope)
    {
        if(condition == nullptr || scope == nullptr) { return nullptr; }
#ifdef DEBUG
        std::cout << "if created:" << std::endl;
        std::cout << "condition:\n" << condition->dumpStr() << "\nscope:\n" << scope->dumpStr() << std::endl;
#endif
        return std::make_unique< If_t >(condition, scope);
    }

    std::unique_ptr< INode > INode::make_print(INode* param)
    {
        if(param == nullptr) { return nullptr; }
#ifdef DEBUG
        std::cout << "print created:" << std::endl;
        std::cout << "param:\n" << param->dumpStr() << std::endl;
#endif
        return std::make_unique< Print_t >(param);
    }

    std::unique_ptr< INode > INode::make_qmark()
    {
#ifdef DEBUG
        std::cout << "qmark created!" << std::endl;
#endif
        return std::make_unique< QMark_t >();
    }

}//namespace ezg