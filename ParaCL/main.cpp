#include <iostream>
#include <fstream>
#include <sstream>
#include "parser/driver.h"


std::string readFile(const std::string &fileName) {
    std::ifstream f(fileName);
    if (!f) {
        throw std::runtime_error("cant open file: " + fileName);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


int main(int argc, char* argv[])
{

#ifdef DEBUG
    yy::Driver driver("tests/test2.txt");
#else
    yy::Driver driver(argv[1]);
#endif

    auto source = readFile("tests/test2.txt");
    driver.setSourceString(source);

    driver.parse ();
    auto n = driver.getData();
    (n[0])->execute();

    return 0;
}