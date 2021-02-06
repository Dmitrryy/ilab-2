#include <iostream>
#include <unordered_map>
#include <vector>
#include <random>


class Random
{
public:

    Random(unsigned _min = std::mt19937::min(), unsigned _max = std::mt19937::max())
            : m_rd()
            , m_gen(m_rd())
            , m_dist{ static_cast<int>(_min), static_cast<int>(_max) }
    {}

    int operator()() {
        return get();
    }

    int get() { return m_dist(m_gen); }

private:

    std::random_device m_rd;
    std::mt19937 m_gen;

    std::uniform_int_distribution<> m_dist;
};


template< typename T >
static void genDoublyConnected(std::basic_ostream<T>& outTest
                               , std::basic_ostream<T>& outAns
                               , size_t numVert
                               , size_t numEdges)
{
    std::unordered_map< size_t, size_t > uniqVertices;
    std::vector< size_t > idToVert;

    size_t center = numVert / 2;
    Random randLeft(0, center);
    Random randRight(center + 1, numVert);

    for (size_t e = 0; e < numEdges; e++)
    {
        const size_t v1 = randLeft();
        const size_t v2 = randRight();

        if (uniqVertices.count(v1) == 0) {
            uniqVertices[v1] = 0;
            idToVert.push_back(v1);
        }
        if (uniqVertices.count(v2) == 0) {
            uniqVertices[v2] = 0;
            idToVert.push_back(v2);
        }
        outTest << v1 << " -- " << v2 << ", " << 0 << std::endl;
    }

    for (const unsigned long & i : idToVert) {
        outAns << i << ' ';
        if (i <= center) {
            outAns << 'b';
        }
        else {
            outAns << 'r';
        }
        outAns << ' ';
    }
}