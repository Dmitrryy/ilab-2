#pragma once

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace dbg 
{

    template<class T, class U>
    void AssertEqual(const T& t, const U& u, const std::string& hint = {}) 
    {
        if (!(t == u)) 
        {
            std::ostringstream os;
            os << "Assertion failed: " << t << " != " << u;
            if (!hint.empty()) {
                os << " hint: " << hint;
            }
            throw std::runtime_error(os.str());
        }
    }

    inline void Assert(bool b, const std::string& hint) {
        AssertEqual(b, true, hint);
    }

    class TestRunner 
    {
    public:
        template <class TestFunc>
        void RunTest(TestFunc func, const std::string& test_name) 
        {
            using std::cerr;
            using std::endl;
            try {
                func();
                cerr << test_name << " OK" << endl;
            }
            catch (const std::exception& e) {
                ++fail_count;
                cerr << test_name << " fail: " << e.what() << endl;
            }
        }

        ~TestRunner() 
        {
            if (fail_count > 0) {
                std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
                exit(1);
            }
        }

    private:

        int fail_count = 0;
    };//class TestRuner

#define ASSERT_EQUAL(x, y) {            \
  std::ostringstream os;                \
  os << #x << " != " << #y << ", "      \
    << __FILE__ << ":" << __LINE__;     \
  dbg::AssertEqual(x, y, os.str());     \
}

#define ASSERT(x) {                     \
  std::ostringstream os;                \
  os << #x << " is false, "             \
    << __FILE__ << ":" << __LINE__;     \
  dbg::Assert(x, os.str());             \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)

}//namespace dbg