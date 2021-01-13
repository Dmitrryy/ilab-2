#pragma once

#include <FlexLexer.h>

#include "../Compiler/INode.h"
#include "gramar.tab.hh"

namespace yy
{

    class Driver {
    private:
        FlexLexer* lexer_ = nullptr;
        void* m_node = nullptr;

    public:
        Driver (FlexLexer* lexer):
                lexer_ (lexer)
        {}
        parser::token_type yylex (parser::semantic_type* yylval) {
            parser::token_type tokenType = static_cast <parser::token_type> (lexer_->yylex ());
            switch (tokenType) {
                case yy::parser::token_type::NUMBER: {
                    yylval->as <double> () = std::stod (lexer_->YYText ());
                    break;
                }

                case yy::parser::token_type::VARIABLE: {
                    std::string word(lexer_->YYText());
                    parser::semantic_type tmp;
                    tmp.as<std::string>() = word;
                    yylval->copy<std::string>(tmp);
                    break;
                }
                default: {
                    break;
                }
            }
            return tokenType;
        }


        void setResult(void* sc) { m_node = sc; }
        void* getNode() { return m_node; }

        bool parse () {
            parser parser (this);
            bool result = parser.parse ();
            return !result;
        }
    };

}//namespace yy