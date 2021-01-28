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
    auto source = readFile("tests/test4.txt");
#else
    yy::Driver driver(argv[1]);
    auto source = readFile(argv[1]);
#endif

    driver.setSourceString(source);

    size_t n_err = driver.parse();
    if (n_err != 0) {
        std::cerr << "Bad compilation. Errors: " << n_err << std::endl;
        return 1;
    }
    auto n = driver.getData();
    (n[0])->execute();

    return 0;
}