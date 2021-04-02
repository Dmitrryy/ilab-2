/****************************************************************************************
 *
 *   app_main.cpp
 *
 *   Created by dmitry
 *   01.11.2020
 *
 ***/


//
///======================================================================================

///======================================================================================
///======================================================================================
//

#include "Application/App.h"



int main()
{
    std::cout << "select file with scene data (.xml):" << std::endl;
    std::string fileName;
    std::cin >> fileName;


    ezg::AppLVL4 app;

    if(app.loadSceneFromXML(fileName) != tinyxml2::XML_SUCCESS) {
        std::cerr << "cant load scene from file: " << fileName << std::endl;
        return 1;
    }

    try {
        app.run();
    }
    catch (std::exception& ex) {
        std::cerr << "What: " << ex.what() << std::endl;
        return 1;
    }

    return 0;

}