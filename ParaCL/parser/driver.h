#pragma once

#include <sstream>
#include <iomanip>

#include "../Compiler/INode.h"

#include "compiler.cpp.h"
#include "scanner.h"

namespace yy
{

    class Driver {
    private:
        Scanner* m_lexer = nullptr;
        mutable size_t m_numErrors = 0;
        std::vector< ezg::INode* > m_data;

        std::istringstream m_strInput;

        std::vector< std::string > m_sourceByLine;

    public:
        Driver (const std::string& name):
                m_lexer(new Scanner(name))
        {}

        //not supported
        Driver                   (const Driver&) = delete;
        const Driver& operator = (const Driver&) = delete;
        Driver                   (Driver&&     ) = delete;
        const Driver& operator = (Driver&&     ) = delete;

        ~Driver() {
            for (auto elem : m_data) {
                delete elem;
            }

            delete m_lexer;
        }

        parser::token_type yylex (parser::location_type* l, parser::semantic_type* yylval)
        {
            parser::token_type tokenType = static_cast <parser::token_type> (m_lexer->yylex ());
            switch (tokenType) {
                case yy::parser::token_type::NUMBER: {
                    yylval->as <double> () = std::stod (m_lexer->YYText ());
                    break;
                }

                case yy::parser::token_type::VARIABLE: {
                    std::string word(m_lexer->YYText());
                    parser::semantic_type tmp;
                    tmp.as<std::string>() = word;
                    yylval->swap< std::string >(tmp);
                    break;
                }

                default: {
                    break;
                }
            }

            *l = m_lexer->getLocation();
            return tokenType;
        }

        void insert(ezg::INode* point)
        { m_data.push_back(point);
        }

        std::vector< ezg::INode* > getData() const { return m_data; }

        size_t parse () {
            parser parser (this);
            bool res = parser.parse ();
            if (res) {
                std::cerr << "compile terminate!" << std::endl;
            }
            return m_numErrors;
        }


        void setSourceString(const std::string& source)
        {
            m_strInput.str(source);
            while(m_strInput) {
                std::string line;
                std::getline(m_strInput, line);
                m_sourceByLine.emplace_back(std::move(line));
            }

            m_strInput.str(source);
            m_lexer->switch_streams(m_strInput, std::cout);
        }


        void error(parser::context const& ctx, const std::string& msg) const
        {
            m_numErrors++;
            std::vector < parser::symbol_kind_type > expected_symbols(10);
            int size_es = ctx.expected_tokens(expected_symbols.data(), 10);

            auto loc = ctx.location();
            std::cerr << loc << ":Error: " << msg << std::endl;
            if (size_es > 0) {
                std::cerr << "Expected: ";
                for (int i = 0; i < size_es; i++) {
                    if (i != 0) { std::cerr << " or "; }
                    std::cerr << "\'" << parser::symbol_name(expected_symbols[i]) << "\'";
                }
                std::cerr << " before \'" << parser::symbol_name(ctx.token()) << "\'" << std::endl;
            }

            std::cerr << "context: " << std::endl;
            for (size_t begl = loc.begin.line; begl <= loc.end.line; begl++) {
                std::cerr << begl << ' ' << m_sourceByLine[begl - 1] << std::endl;
            }

            const size_t ws = std::to_string(loc.end.line).size() + 1;
            std::cerr << std::setw(ws) << ' ';

            for (size_t i = 0; i < loc.end.column; i++) {
                if (i == ctx.lookahead().location.begin.column - 1) {
                    std::cerr << '^';
                }
                else {
                    std::cerr << '~';
                }
            }
            std::cerr << std::endl << std::endl;

        }
    };

}//namespace yy