#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <Windows.h>

#include "cache_LFU.h"
#include "cache_tests.h"

struct Page
{
    Page(size_t _id = 0)
        : m_id(_id)
    {
        //type heavy object...
        Sleep(10);
    }

private:
    size_t m_id;
};

int main()
{
    RunTest<cache_LFU<Page, int>>();

    return 0;
}