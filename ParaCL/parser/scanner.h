#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "location.hh"

class Scanner : public yyFlexLexer
{
    yy::location m_loc;
    size_t m_storeLine = 1;

private:

    void updateLocation()
    {
        m_loc.lines(lineno() - m_storeLine);
        m_loc.begin = m_loc.end;

        if (lineno() - m_storeLine == 0)
            m_loc.columns(YYLeng());

        m_storeLine = lineno();
    }

public:

    virtual ~Scanner() = default;
    yy::location getLocation() const { return m_loc; }
    int yylex() override;
};