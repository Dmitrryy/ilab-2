#include <iostream>
#include <fstream>
#include <functional>
#include <random>

#include "test_generator.hpp"

int main()
{
    Random r(1, 100);
    
    size_t size = 0u, num = 0u;
    std::cin >> size >> num;

    std::ofstream out("cache/tests/002.txt", std::ios::trunc);
    if (out.is_open()) {
    out << size << ' ' << num << '\n';

    for (size_t i = 0; i != num; i++) {
        out << r() << ' ';
    }
    out << std::endl;
    out.close();
    }
    else
    {
        std::cerr << "cant open\n" << std::endl;
    }
    

    return 0;
}