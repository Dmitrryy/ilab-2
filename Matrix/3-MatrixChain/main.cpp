#include <iostream>

#include <matrix/chain/MatrixChain.h>

int main()
{
    ezg::MatrixChain< double > chain;
    chain.add(matrix::Matrix< double >(10, 30));
    chain.add(matrix::Matrix< double >(30, 5));
    chain.add(matrix::Matrix< double >(5, 60));

    auto tree = chain.optimalOrderId();

    //boost::property_tree::xml_parser::write_xml(std::cout, tree);



    return 0;
}