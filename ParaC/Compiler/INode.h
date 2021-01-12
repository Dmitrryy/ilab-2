#pragma once

#include <string>
#include <vector>
#include <optional>

namespace ezg
{

    enum class Operator
    {
          Add
        , Sub
        , Mul
        , Dev

        , Greater
        , Less
        , Equal
        , NonEqual

        , While
        , If
        , Print
        , QMark

/*        , LPar
        , RPar
        , LBrake
        , RBrake

        , SColon*/
    };


    class INode
    {
        INode* m_parent;

    public:

        virtual ~INode() = default;

        virtual int execute() = 0;
        virtual std::string dumpStr() const = 0;

    public:

        static INode* make_op(Operator tOp, INode* left, INode* right);
        static INode* make_val(int val);
        static INode* make_var(size_t id);
        static INode* make_while(INode* condition, INode* scope);
        static INode* make_if(INode* condition, INode* scope);
        static INode* make_print(INode* param);
        static INode* make_qmark();
    };


    class IScope : public INode
    {
    public:

        virtual void addNode(INode* node) = 0;
        virtual std::optional< size_t > visible(const std::string& var_name) = 0;
        virtual std::optional< size_t > declareVar(const std::string& var_name) = 0;

    public:

        static IScope* make(size_t prev_scope);
        static IScope* make();
    };

}//namespace ezg