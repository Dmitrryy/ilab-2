#pragma once


namespace ezg
{


    class BaseOperation
    {
    public:

        enum class Type
        {
            Main
            //, Function
            , Var
            , Num
            , Section
            , BinaryOperation
            , UnaryOperation
            , While
            //, For
            , If
            , Scan
            , Print
            , SpecialForParse
        };

    public:

        virtual Type getType() const = 0;
    };



    class Main_t final : public BaseOperation
    {
        //std::vector< BaseOperation* > m_other_function;

        BaseOperation* m_next;

    public:

        Type getType() const override { return Type::Main; }
    };

    class SpecialForParse_t final : public BaseOperation
    {
        enum class Name
        {
            RoundBracketOpen
            , RoundBracketClose
            , BraceOpen
            , BraceClose
            , Semicolon
            , Comma
        };

        Name m_name;

    public:

        explicit SpecialForParse_t(Name name_)
                : m_name(name_)
        {}

    public:

        Name getName() const { return m_name; }
        Type getType() const override { return Type::SpecialForParse; }
    };

}//namespace ezg