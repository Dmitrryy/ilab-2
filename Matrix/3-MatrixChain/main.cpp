#include <iostream>

#include <matrix/chain/MatrixChain.h>

int main()
{
    boost::property_tree::basic_ptree< std::string, int > tree1;
    boost::property_tree::basic_ptree< std::string, int > tree2;


    tree1.put("1", 2);
    tree1.put("2", 1);


    tree2.put("1", 4);
    tree2.put("2", 5);



    tree1.put_child("left", tree2);

    std::cout << tree1.size() << std::endl;

    for(const auto& v : tree1) {
        std::cout << v.first << ' ';
        std::cout << v.second.size()<< '\n';
    }


    return 0;
}