#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "cache_2Q.hpp"

struct Page
{
    Page(size_t _id = 0)
        : m_id(_id)
    {

    }

private:
    size_t m_id;
};

int main()
{
    size_t size = 0u, n = 0u;
    std::cin >> size >> n;

    cache_2Q<int, int> cache(size);

    for (size_t i = 0; i < n; i++) {
        size_t key;
        std::cin >> key;

        if (!cache.existence(key)) {
            cache.add(key, []() {return new int(); });
        }
        cache.get(key);
    }

    std::cout << "hit: " << cache.hit() << std::endl;

    return 0;
}