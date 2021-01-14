#pragma once

#include "../Compiler/INode.h"

#include "gramar.tab.hh"
#include "scanner.h"

namespace yy
{

    class Driver {
    private:
        Scanner* lexer_ = nullptr;
        void* m_node = nullptr;

    public:
        explicit Driver (Scanner* lexer):
                lexer_ (lexer)
        {}
        parser::token_type yylex (parser::location_type* l, parser::semantic_type* yylval)
        {
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

            *l = lexer_->getLocation();
            //static int i = 0;
            //i++;
            //std::cout << '[' << i << "] (" << l->begin.line << " ," << l->begin.column << ") (" <<  l->end.line << " ," << l->end.column << ")\n";

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