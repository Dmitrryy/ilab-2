#include <iostream>
#include <cassert>
#include <chrono>

#include "cash_LFU.h"

enum class Typ
{
    one
    , two
    , three
    , four
};

std::ostream& operator << (std::ostream& stream, Typ _tp) {
    stream << static_cast<int>(_tp);
    return stream;
}

int main()
{
    cash_LFU<int, Typ> test(3);
    assert(test.size() == 0);
    assert(test.mSize() == 3);

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

    std::cout << test.debugString() << std::endl;

    test.reloadAll();
    assert(test.size() == 0);

    std::cout << test.debugString() << std::endl;

    return 0;
}