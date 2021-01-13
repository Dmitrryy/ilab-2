#include "Node.h"
#include "VarTable.h"
#include <unordered_map>
#include <iostream>

#include <stack>

namespace ezg {

    std::unordered_map<std::string, size_t> gStrToId;
    size_t gCurFreeId = 1;

    ScopeTable gScopeTable;
    VarTable gVarTable;

    std::optional<size_t> Scope::declareVar(const std::string &var_name)
    {
        if (gStrToId.count(var_name) != 0) {
            return {};
        }
        size_t idVar = gCurFreeId++;
        gStrToId[var_name] = idVar;

        gScopeTable.addElem(m_idTable, idVar, nullptr);

        return {idVar};
    }

    std::optional<size_t> Scope::visible(const std::string &var_name)
    {
        auto it = gStrToId.find(var_name);
        if (it == gStrToId.end()) {
            return {};
        }
        if (gScopeTable.access(it->second).has_value()) {
            return it->second;
        }
        return {};
    }

    int Scope::execute()
    {
        gScopeTable.entryScope(m_idTable);

        for (auto& curNode : m_nodes) {
            curNode->execute();
        }
        assert(gScopeTable.getCurTableId() == m_idTable);
        gScopeTable.exitCurScope();

        return 0;
    }

    void Scope::entry()
    {
        gScopeTable.entryScope(m_idTable);
    }
    void Scope::exit()
    {
        assert(gScopeTable.getCurTableId() == m_idTable);
        gScopeTable.exitCurScope();
    }

    ////////////////////////////////////////////////////////////

    int Variable::getVal() const
    {
        return gVarTable.access(m_id);
    }
    void Variable::setVal(int v) const
    {
        gVarTable.access(m_id) = v;
    }

    /////////////////////////////////////////////////////////////

    IScope* IScope::make_separate()
    {
        size_t id = gScopeTable.createTable();
        IScope* res = new Scope(id);
        return res;
    }

    IScope* IScope::make_inside_current()
    {
        size_t cur_id = gScopeTable.getCurTableId();
        size_t id = gScopeTable.createTable(cur_id);
        IScope* res = new Scope(id);
        return res;
    }



    ////////////////////////////////////////////////////////////

#define OpCase(name)             \
case Operator::name:             \
    res = new name(left, right); \
    break;

    INode* INode::make_op(Operator tOp, INode* left, INode* right)
    {
#ifdef DEBUG
        std::cout << "operator created: " << (int)tOp << std::endl;
        std::cout << "left:\n" << left->dumpStr() << "\nright:\n" << right->dumpStr() << std::endl;
#endif
        INode* res = nullptr;
        switch (tOp) {
            OpCase(Greater);
            OpCase(Less);
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

    INode* INode::make_val(int val)
    {
#ifdef DEBUG
        std::cout << "value created: " << val << std::endl;
#endif
        INode* res = new Value(val);
        return res;
    }

    INode* INode::make_assign(INode* var, INode* val)
    {
#ifdef DEBUG
        std::cout << "assign created:" << std::endl;
        std::cout << "var:\n" << var->dumpStr() << "\nval:\n" << val->dumpStr() << std::endl;
#endif
        INode* res = new Assign(dynamic_cast< Variable* >(var), val);
        return res;
    }


    INode* INode::make_var(size_t id)
    {
#ifdef DEBUG
        std::cout << "variable created: " << id << '(' << ')' << std::endl;
#endif
        INode* res = new Variable(id);
        return res;
    }

    INode* INode::make_while(INode* condition, INode* scope)
    {
#ifdef DEBUG
        std::cout << "while created:" << std::endl;
        std::cout << "condition:\n" << condition->dumpStr() << "\nscope:\n" << scope->dumpStr() << std::endl;
#endif
        INode* res = new While_t(condition, scope);
        return res;
    }

    INode* INode::make_if(INode* condition, INode* scope)
    {
#ifdef DEBUG
        std::cout << "if created:" << std::endl;
        std::cout << "condition:\n" << condition->dumpStr() << "\nscope:\n" << scope->dumpStr() << std::endl;
#endif
        INode* res = new If_t(condition, scope);
        return res;
    }

    INode* INode::make_print(INode* param)
    {
#ifdef DEBUG
        std::cout << "print created:" << std::endl;
        std::cout << "param:\n" << param->dumpStr() << std::endl;
#endif
        INode* res = new Print_t(param);
        return res;
    }

    INode* INode::make_qmark()
    {
#ifdef DEBUG
        std::cout << "qmark created!" << std::endl;
#endif
        INode* res = new QMark_t();
        return res;
    }

}//namespace ezg