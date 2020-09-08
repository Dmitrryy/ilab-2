
#include <iostream>
#include <cassert>

#include "cash_LFU.h"

enum class Typ
{
    one
    , two
    , three
    , four
};

int main()
{
    cash_LFU<int, Typ> test(3);

    test.add(Typ::one, []() { return new int(1); });
    test.add(Typ::two, []() { return new int(2); });
    test.add(Typ::three, []() { return new int(3); });
    test.add(Typ::four, []() { return new int(4); });

    int a = test.get(Typ::two);
    assert(a == 2);
    a = test.get(Typ::two);
    assert(a == 2);
    a = test.get(Typ::one);
    assert(a == 1);
    a = test.get(Typ::three);
    assert(a == 3);
    a = test.get(Typ::four);
    assert(a == 4);

    std::cout << test.size() << std::endl;
    assert(test.size() == 3);

    return 0;
}