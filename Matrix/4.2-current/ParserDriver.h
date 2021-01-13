#pragma once

#include <iostream>
#include <map>
#include <FlexLexer.h>
#include "parser.tab.hh"

#include <circuit/Edge.h>

namespace yy {

    class ParsDriver {
        FlexLexer *plex_;
        std::vector<ezg::Edge> m_data;

    public:
        explicit ParsDriver(FlexLexer *plex) : plex_(plex) {}

        parser::token_type yylex(parser::semantic_type *yylval) {
            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            return tt;
        }

        std::vector<ezg::Edge> getData() const { return m_data; }

        void insert(const ezg::Edge &e) { m_data.push_back(e); }

        bool parse() {
            parser parser(this);
            bool res = parser.parse();
            return !res;
        }

    };//class Driver

}//namespace yy