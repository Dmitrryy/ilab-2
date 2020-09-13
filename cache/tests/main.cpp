#include <iostream>
#include <fstream>
#include <functional>
#include <random>

#include "test_generator.hpp"

template <typename T>
std::ostream& operator << (std::ostream& stream, std::vector<T> _vec)
{
    for (size_t i = 0; i != _vec.size(); i++) {
        stream << _vec[i] << ' ';
    }

    return stream;
}

int main()
{
    Random r(0, 100);
    
    size_t size = 0u, num = 0u;
    std::cin >> size >> num;

    std::ofstream out("cache/tests/006.txt", std::ios::trunc);

    if (out.is_open()) 
    {
        out << size << ' ' << num << '\n';

        for (size_t i = 0; i < num; i++) {
            out << r.get() << ' ';
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