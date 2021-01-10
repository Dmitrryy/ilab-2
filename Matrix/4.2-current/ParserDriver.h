#pragma once

#include <iostream>
#include <map>
#include <FlexLexer.h>
#include "parser.tab.hh"
#include <matrix/Matrix.h>
#include <circuit/Circuit.h>

namespace yy {

    class ParsDriver {
        FlexLexer *plex_;
        ezg::Circuit m_circuit;

    public:
        explicit ParsDriver (FlexLexer *plex) : plex_(plex) {}

        parser::token_type yylex(parser::semantic_type* yylval) {
            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            return tt;
        }

        ezg::Circuit getData() const { return m_circuit; }

        bool parse() {
            parser parser(this);
            bool res = parser.parse();
            return !res;
        }


        void connect(size_t id1, size_t id2, float resistance, float eds)
        {
            ezg::Edge cr = {0, id1, id2, resistance};
            if (eds != 0.f)
                cr.eds = eds;

            m_circuit.connect(id1, id2, cr);
        }

        std::string dumpStr() const {
            return m_circuit.dumpStr();
        }
    };

}