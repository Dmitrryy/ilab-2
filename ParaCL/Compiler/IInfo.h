#pragma once

#include "INode.h"

namespace ezg
{

    struct IInfo_t
    {
        enum class Type
        {
            Variable
            , Function //todo
        };

    public:

        virtual ~IInfo_t() = default;
        virtual Type type() const noexcept = 0;
    };//class IInfo_t


    struct VarInfo_t : public IInfo_t
    {
        int m_value = 0;

        Type type() const noexcept override { return Type::Variable; }
    };



    struct FuncInfo_t : public IInfo_t
    {
        INode* m_pFunc;
        size_t m_numArgs;

        Type type() const noexcept override { return Type::Function; }
    };



}//namespace ezg