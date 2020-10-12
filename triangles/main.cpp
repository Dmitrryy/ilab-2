#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <fstream>

//#include "LAL/include/LAL.h"
#include "Octree/Octree.h"
#include "tests/test_gen.h"

void intersectionN_N    (std::vector< std::pair< la::Triangle, bool > > data);
void intersectionOctree (std::vector< std::pair< la::Triangle, bool > > data);

extern size_t COUNT_TT_INTERSEC;

int main()
{
    int n = 0;
    std::cin >> n;
    std::vector< std::pair< la::Triangle, bool > > data;
    data.reserve(n);
    for (int i = 0; i < n; i++)
    {
        la::Vector3f a, b, c;
        std::cin >> a.x >> a.y >> a.z
            >> b.x >> b.y >> b.z
            >> c.x >> c.y >> c.z;
        data.push_back(std::make_pair(la::Triangle(a, b, c), false));
    }
    intersectionN_N(data);
    intersectionOctree(data);

    //size_t num = 0u;
    //double ax, ay, az, bx, by, bz;
    //std::cin >> num >> ax >> ay >> az >> bx >> by >> bz;
    //std::ofstream out("tests/005.txt", std::ios::trunc);
    //if (out.is_open())
    //{
    //    out << num << '\n';
    //    auto res = testGenTriangles(la::Square({ ax, ay, az }, { bx, by, bz }), num, 10);
    //    for (const auto& i : res)
    //    {
    //        out << i << ' ';
    //    }
    //    out << std::endl;
    //    out.close();
    //}
    //else
    //{
    //    std::cerr << "cant open\n" << std::endl;
    //}

    return 0;
}


void intersectionN_N(std::vector< std::pair< la::Triangle, bool > > data)
{
    const size_t n = data.size();

    COUNT_TT_INTERSEC = 0u;

    size_t counter_cmp = 0;
    for (int i = 0; i < n; i++)
    {
        auto& lhs = data[i];
        if (lhs.second) {
            continue;
        }
        for (int k = i + 1; k < n; k++)
        {
            auto& rhs = data[k];
            if (lhs.second) {
                break;
            }
            if (rhs.second) {
                continue;
            }

            lhs.second = rhs.second = la::intersec(lhs.first, rhs.first);
            counter_cmp++;
        }
    }

    std::cout << "\nN*N\nN = " << n << '\n'
        << "N * N = " << n * n << '\n'
        << "num cmp = " << COUNT_TT_INTERSEC << '\n';

    for (int i = 0; i < n; i++)
    {
        if (data[i].second) {
            std::cout << i << ' ';
        }
    }
    std::cout << std::endl;
}


#define MIN(a, b, c) \
std::min(a, std::min(b, c))

#define MAX(a, b, c) \
std::max(a, std::max(b, c))

template <typename Val_, typename Key_ = int>
struct ValId
{
    Val_ val;
    Key_ id;

    la::Square getArea() const { return val.getArea(); }
};
bool intersec(const ValId<la::Triangle, size_t>& _lhs, const ValId<la::Triangle, size_t>& _rhs)
{
    return la::intersec(_lhs.val, _rhs.val);
}

void intersectionOctree(std::vector< std::pair< la::Triangle, bool > > data)
{
    using std::min;
    using std::max;

    COUNT_TT_INTERSEC = 0u;

    size_t n = data.size();

    if (!data.empty())
    {
        la::Vector3f a = data[0].first.getA(), b = data[0].first.getB();

        for (size_t i = 0, n = data.size(); i < n; ++i)
        {
            const la::Triangle tr = data[i].first;
            a.x = min(a.x, MIN(tr.getA().x, tr.getB().x, tr.getC().x));
            a.y = min(a.y, MIN(tr.getA().y, tr.getB().y, tr.getC().y));
            a.z = min(a.z, MIN(tr.getA().z, tr.getB().z, tr.getC().z));
            b.x = max(b.x, MAX(tr.getA().x, tr.getB().x, tr.getC().x));
            b.y = max(b.y, MAX(tr.getA().y, tr.getB().y, tr.getC().y));
            b.z = max(b.z, MAX(tr.getA().z, tr.getB().z, tr.getC().z));
        }

        la::Octree< ValId< la::Triangle, size_t > > tree(la::Square(a, b));

        for (size_t i = 0, n = data.size(); i < n; ++i)
        {
            tree.add({ data[i].first, i});
        }

        tree.msplit();
        auto res = tree.getIntersections();

        std::set<int> id_res;
        for (const auto& p : res)
        {
            id_res.insert(p.first.id);
            id_res.insert(p.second.id);
        }

        std::cout << "\nOctree\nN = " << n << '\n'
            << "N * N = " << n * n << '\n'
            << "num pair: " << res.size() << '\n'
            << "num cmp: " << COUNT_TT_INTERSEC << '\n';

        for (const int& k : id_res)
        {
            std::cout << k << ' ';
        }
        std::cout << std::endl;
    }

}