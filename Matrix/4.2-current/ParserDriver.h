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
        matrix::Matrix< Edge > m_data;
        matrix::Matrix< int > m_IGraph;

    public:
        explicit ParsDriver (FlexLexer *plex) : plex_(plex) {}

        parser::token_type yylex(parser::semantic_type* yylval) {
            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
            if (tt == yy::parser::token_type::NUMBER)
                yylval->as<float>() = std::stof(plex_->YYText());

            return tt;
        }

        matrix::Matrix< Edge > getData() const { return m_data; }
        matrix::Matrix< int > getIGraph() const { return m_IGraph; }

        bool parse() {
            parser parser(this);
            bool res = parser.parse();
            return !res;
        }

        void addVertex(int id)
        {
            if (m_data.getLines() < id)
            {
                m_data.resize(id, id);
            }

        }

        void connect(int id1, int id2, float resistance, float eds)
        {
            addVertex(id1 + 1);
            addVertex(id2 + 1);
            m_data.at(id1, id2) = Edge{true, resistance, eds };
            m_data.at(id2, id1) = Edge{true, resistance, eds };

            m_IGraph.resize(std::max(m_IGraph.getLines(), static_cast<size_t>(std::max(id1, id2)) + 1), m_IGraph.getColumns() + 1);
            m_IGraph.at(id1, m_IGraph.getColumns() - 1) = 1;
            m_IGraph.at(id2, m_IGraph.getColumns() - 1) = 1;
        }

        void dumpStr() const {
            const size_t lines = m_data.getLines();
            const size_t columns = m_data.getColumns();
            for (size_t l = 0; l < lines; l++) {
                for (size_t c = l + 1; c < columns; c++)
                {
                    Edge cur = m_data.at(l, c);
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