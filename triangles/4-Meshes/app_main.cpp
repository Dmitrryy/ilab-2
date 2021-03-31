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

#include <tinyxml2.h>


int main()
{
    std::cout << "select file with scene data (.xml):" << std::endl;
    std::string fileName;
    std::cin >> fileName;


    ezg::AppLVL4 app;

    app.loadSceneFromXML(fileName);

    app.run();

    return 0;

}