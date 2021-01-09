#pragma once

#include <iostream>
#include <map>
#include <FlexLexer.h>
#include "parser.tab.hh"
#include "../Libs/matrix/Matrix.h"

struct Edge {
    bool connect = false;
    float resistance = 0;
    float voltage = 0;
};

namespace yy {

    class ParsDriver {
        FlexLexer *plex_;
        //std::vector<std::vector<std::vector<float>>> vvv_;
        matrix::Matrix< Edge > m_graph;

    public:
        explicit ParsDriver (FlexLexer *plex) : plex_(plex) {}

        parser::token_type yylex(parser::semantic_type* yylval) {
            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            return tt;
        }

        bool parse() {
            parser parser(this);
            bool res = parser.parse();
            return !res;
        }

        void addVertex(int id)
        {
            if (m_graph.getLines() < id)
            {
                m_graph.resize(id, id);
            }
        }

        void connect(int id1, int id2, float resistance, float eds)
        {
            addVertex(id1 + 1);
            addVertex(id2 + 1);
            m_graph.at(id1, id2) = Edge{ true, resistance, eds };
            m_graph.at(id2, id1) = Edge{ true, resistance, eds };
        }

        void printout() const {
            const size_t lines = m_graph.getLines();
            const size_t columns = m_graph.getColumns();
            for (size_t l = 0; l < lines; l++) {
                for (size_t c = l + 1; c < columns; c++)
                {
                    Edge cur = m_graph.at(l, c);
                    if (cur.connect) {
                        std::cout << l << " -- " << c << ", " << cur.resistance << ';';
                        if (cur.voltage != 0) {
                            std::cout << ' ' << cur.voltage;
                        }
                        std::cout << std::endl;
                    }
                }
            }

        }
    };

}