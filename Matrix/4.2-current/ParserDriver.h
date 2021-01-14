#pragma once

#include <iostream>
#include <map>

#include "parser.tab.hh"
#include "scanner.h"

#include <circuit/Edge.h>

namespace yy {

    class ParsDriver {
        Scanner *plex_;
        std::vector<ezg::Edge> m_data;
        size_t m_numErrors = 0;
        std::string m_fileName;

    public:
        explicit ParsDriver(Scanner *plex) : plex_(plex) {}

        parser::token_type yylex(parser::location_type* l, parser::semantic_type *yylval) {

            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            *l = plex_->getLocation();
            l->begin.filename = l->end.filename = &m_fileName;

            return tt;
        }

        std::vector<ezg::Edge> getData() const { return m_data; }

        void setFileName(const std::string& str) { m_fileName = str; }

        void insert(const ezg::Edge &e) { m_data.push_back(e); }

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

            std::cerr << *(ctx.location().begin.filename) << ':' << ctx.location().begin.line << ": "
                      << "Error: expected ";
            for (int i = 0; i < size_es; i++) {
                if (i != 0) { std::cerr << " or "; }
                std::cerr << parser::symbol_name(expected_symbols[i]);
            }
            std::cerr << " before " << parser::symbol_name(ctx.token());

            std::cerr << std::endl;
        }

    };//class Driver

}//namespace yy