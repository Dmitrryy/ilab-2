#include "Node.h"
#include "VarTable.h"
#include <unordered_map>
#include <iostream>

#include <stack>

namespace ezg {
    size_t gCurFreeId = 1;

    ScopeTable g_ScopeTable;
    dst< std::string, size_t > g_StrToId;
    //VarTable g_VarTable;

    std::optional<size_t> Scope::declareVar(const std::string &var_name)
    {
        if (g_StrToId.countLeft(var_name) != 0) {
            return {};
        }
        size_t idVar = gCurFreeId++;
        g_StrToId.add(var_name, idVar);

        g_ScopeTable.addElem(m_idTable, idVar, std::make_unique< VarInfo_t >());

        return {idVar};
    }

    std::optional<size_t> Scope::visible(const std::string &name)
    {
        auto it = g_StrToId.countLeft(name);
        if (it == 0) {
            return {};
        }
        size_t id = g_StrToId.getRight(name);
        if (g_ScopeTable.access(id).has_value()) {
            return id;
        }
        return {};
    }

    int Scope::execute()
    {
        entry();

        for (auto& curNode : m_nodes) {
            curNode->execute();
        }

        exit();

        return 0;
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
            OpCase(Add);
            OpCase(Sub);
            OpCase(Mul);
            OpCase(Div);

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