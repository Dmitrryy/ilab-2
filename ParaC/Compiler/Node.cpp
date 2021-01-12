#include "Node.h"
#include "VarTable.h"
#include <unordered_map>

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
        return it->second;
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

    IScope* IScope::make(size_t prev_scope)
    {
        size_t id = gScopeTable.createTable(prev_scope);

        IScope* res = new Scope(id);

        return res;
    }
    IScope* IScope::make()
    {
        size_t id = gScopeTable.createTable();

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
        INode* res = nullptr;
        switch (tOp) {
            OpCase(Greater);
            OpCase(Less);
            OpCase(Equal);
            OpCase(NonEqual);
            OpCase(Add);
            OpCase(Sub);
            OpCase(Mul);
            OpCase(Dev);

            default:
                assert(0);
        }

        return res;
    }

    INode* INode::make_val(int val)
    {
        INode* res = new Value(val);
        return res;
    }

    INode* INode::make_var(size_t id)
    {
        INode* res = new Variable(id);
        return res;
    }

    INode* INode::make_while(INode* condition, INode* scope)
    {
        INode* res = new While_t(condition, scope);
        return res;
    }

    INode* INode::make_if(INode* condition, INode* scope)
    {
        INode* res = new If_t(condition, scope);
        return res;
    }

    INode* INode::make_print(INode* param)
    {
        INode* res = new Print_t(param);
        return res;
    }

    INode* INode::make_qmark()
    {
        INode* res = new QMark_t();
        return res;
    }

}//namespace ezg