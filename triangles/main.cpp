#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <fstream>

//#include "LAL/include/LAL.h"
#include "Octree/Octree.h"
#include "tests/test_gen.h"

//-1 -11 -11 5 -7 -17 -4 -5 -19 2 -5 -9 -1 -5 -14 0 -9 -17

//15 21 -11 21 13 -14 22 16 -10 11 17 -17 13 19 -14 12 21 -23

//9 1 -13 12 2 -18 9 7 -20 9 -4 -16 13 -5 -16 15 0 -18

//-18 -8 12 -13 -14 16 -19 -14 8 -14 -4 2 -17 -11 7 -20 -14 10

void intersectionN_N    (std::vector< std::pair< la::Triangle, bool > > data);
void intersectionOctree (std::vector< std::pair< la::Triangle, bool > > data);

extern size_t COUNT_TT_INTERSEC;


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

    //std::cout << data[26].first << ' ' << data[12].first << std::endl;

    std::vector< std::pair< ValId< la::Triangle, size_t >, ValId< la::Triangle, size_t > > > result_pair;
    std::set<size_t> res_id;

    size_t counter_cmp = 0;
    for (size_t i = 0; i < n; i++)
    {
        auto& lhs = data.at(i);
        if (lhs.second) {
            continue;
        }
        for (size_t k = i + 1; k < n; k++)
        {
            auto& rhs = data.at(k);
            if (lhs.second) {
                break;
            }
            if (rhs.second) {
                continue;
            }

            if (la::intersec(lhs.first, rhs.first))
            {
                result_pair.push_back(std::make_pair(ValId<la::Triangle, size_t>{lhs.first, i},
                    ValId<la::Triangle, size_t>{rhs.first, k}));
                res_id.insert(i);
                res_id.insert(k);
            }
            counter_cmp++;
        }
    }

    std::cout << "\nN*N\nN = " << n << '\n'
        << "N * N = " << n * n << '\n'
        << "num cmp = " << COUNT_TT_INTERSEC << '\n';

    //for (const auto& id : res_id)
    //{
    //    std::cout << id << ' ';
    //}
    for (const auto& r : result_pair)
    {
        std::cout << '(' << r.first.id << ", " << r.second.id << ") ";
    }
    std::cout << std::endl;
}


#define MIN(a, b, c) \
std::min(a, std::min(b, c))

#define MAX(a, b, c) \
std::max(a, std::max(b, c))

void intersectionOctree(std::vector< std::pair< la::Triangle, bool > > data)
{
    using std::min;
    using std::max;

    COUNT_TT_INTERSEC = 0u;

    size_t n = data.size();

    if (!data.empty())
    {
        la::Vector3f a = data.at(0).first.getA(), b = data.at(0).first.getB();

        for (size_t i = 0, n = data.size(); i < n; ++i)
        {
            const la::Triangle tr = data.at(i).first;
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
            tree.add({ data.at(i).first, i});
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
            << "num cmp: " << COUNT_TT_INTERSEC << '\n'
            << "size: " << tree.size() << '\n';

        //for (const int& k : id_res)
        //{
        //    std::cout << k << ' ';
        //}
        for (const auto& r : res)
        {
            std::cout << '(' << r.first.id << ", " << r.second.id << ") ";
        }
        std::cout << std::endl;
    }

}