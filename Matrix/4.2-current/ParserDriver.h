#pragma once

#include <iostream>
#include <map>

#include "parser.tab.hh"
#include "scanner.h"

namespace yy {

    struct Elem
    {
        size_t v1, v2;
        float res;
        float eds;
    };

    class ParsDriver {
        Scanner *plex_;
        std::vector< Elem > m_data;
        size_t m_numErrors = 0;
        std::string m_fileName;

    public:
        explicit ParsDriver()
            : plex_(new Scanner) {}

        parser::token_type yylex(parser::location_type* l, parser::semantic_type *yylval) {

            auto tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            *l = plex_->getLocation();
            l->begin.filename = l->end.filename = &m_fileName;

            return tt;
        }

        std::vector< Elem > getData() const { return m_data; }

        void setFileName(const std::string& str) { m_fileName = str; }

        void connect(size_t v1, size_t v2, float res, float eds)
        {
            m_data.emplace_back(Elem{v1, v2, res, eds});
        }

        bool parse() {
            parser parser(this);
            bool res = parser.parse();
            if (res) {
                std::cerr << "compile terminate!" << std::endl;
            }
            return m_numErrors == 0;
        }

        void error(parser::context const& ctx)
        {
            m_numErrors++;
            std::vector < parser::symbol_kind_type > expected_symbols(10);
            int size_es = ctx.expected_tokens(expected_symbols.data(), 10);

            std::cerr << ctx.location() << ": "
                      << "Error: expected ";
            for (int i = 0; i < size_es; i++) {
                if (i != 0) { std::cerr << " or "; }
                std::cerr << parser::symbol_name(expected_symbols[i]);
            }
            std::cerr << " before \'" << parser::symbol_name(ctx.token()) << "\'" << std::endl;

            std::cerr << std::endl;
        }

    };//class Driver

}//namespace yy