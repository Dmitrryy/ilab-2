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

}//namespace ezg