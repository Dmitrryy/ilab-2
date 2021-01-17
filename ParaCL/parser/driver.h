#pragma once

#include "../Compiler/INode.h"

#include "gramar.tab.hh"
#include "scanner.h"

namespace yy
{

    class Driver {
    private:
        Scanner* lexer_ = nullptr;
        size_t m_numErrors = 0;
        std::vector< void* > m_data;

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
                    yylval->swap< std::string >(tmp);
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

        void insert(void* point) { m_data.push_back(point); }
        //void setResult(void* sc) { m_node = sc; }
        std::vector< void* > getData() const { return m_data; }

        bool parse () {
            parser parser (this);
            bool result = parser.parse ();
            return !result;
        }


        void error(parser::context const& ctx)
        {
            m_numErrors++;
            std::vector < parser::symbol_kind_type > expected_symbols(10);
            int size_es = ctx.expected_tokens(expected_symbols.data(), 10);

            std::cerr << *(ctx.location().begin.filename) << ':' << ctx.location().begin.line << ": "
                      << "Error: expected ";
            for (int i = 0; i < size_es; i++) {
                if (i != 0) { std::cerr << " or "; }
                std::cerr << parser::symbol_name(expected_symbols[i]);
            }
            std::cerr << " before " << parser::symbol_name(ctx.token());

            std::cerr << std::endl;
        }
    };

}//namespace yy