#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

#include "cache_LFU/cache_LFU.hpp"
#include "cache_2Q/cache_2Q.hpp"
#include "cache_LFU/LFU_container.hpp"

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

    LRU_container<int> cache(size);

    for (size_t i = 0; i < n; i++) {
        size_t key;
        std::cin >> key;

        cache.lookup(key);
    }

    std::cout << "hit: " << cache.hit() << std::endl;

    return 0;

}