#include <iostream>
#include <list>
#include <vector>
#include <map>

#include "LAL/include/LAL.h"

void intersectionN_N    (std::vector< std::pair< la::Triangle, bool > > data);
void intersectionOctree (std::vector< std::pair< la::Triangle, bool > > data);


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

    return 0;
}


void intersectionN_N(std::vector< std::pair< la::Triangle, bool > > data)
{
    const size_t n = data.size();

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

    std::cout << "\nN = " << n << '\n'
        << "N * N = " << n * n << '\n'
        << "num cmp = " << counter_cmp << '\n';

    for (int i = 0; i < n; i++)
    {
        if (data[i].second) {
            std::cout << i << ' ';
        }
    }
    std::cout << std::endl;
}


void intersectionOctree(std::vector< std::pair< la::Triangle, bool > > data);