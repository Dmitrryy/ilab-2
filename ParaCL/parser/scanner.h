#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>

#include <utility>
#endif

#include "location.hh"

class Scanner : public yyFlexLexer
{
    yy::location    m_loc;
    size_t          m_storeLine = 1;
    std::string     m_name;

private:

    void updateLocation()
    {
        m_loc.lines(lineno() - m_storeLine);
        m_loc.begin = m_loc.end;

        m_loc.begin.filename = m_loc.end.filename = &m_name;

        if (lineno() - m_storeLine == 0)
            m_loc.columns(YYLeng());

        m_storeLine = lineno();
    }

public:

    Scanner(std::string  name)
            : m_name(std::move(name))
    {}

    yy::location getLocation() const { return m_loc; }
    int yylex() override;
};