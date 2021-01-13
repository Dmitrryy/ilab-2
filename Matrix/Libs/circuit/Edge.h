#pragma once


namespace ezg
{

    struct Edge
    {
        Edge(size_t vLeft, size_t vRight)
            : v1(vLeft)
            , v2(vRight)
        { }

        size_t id = 0;
        size_t v1 = 0, v2 = 0;
        std::optional< float > resistance;
        std::optional< float > current;
        std::optional< float > eds;

        bool operator == (const Edge& that) const {
            return (v1 == that.v1 && v2 == that.v2);
        }
        bool operator != (const Edge& that) const { return !(*this == that); }
        bool operator < (const Edge& that) const { return id < that.id; }
    };

}//namespace Edge