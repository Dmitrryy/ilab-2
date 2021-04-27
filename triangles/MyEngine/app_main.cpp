/****************************************************************************************
 *
 *   app_main.cpp
 *
 *   Created by dmitry
 *   28.03.2021
 *
 ***/


//
/// LVL 4
///======================================================================================
/// This is the 4th level of triangles.
/// Objective: to draw a mesh of triangles using the Vulkan API; Implement mesh growth
/// (before rendering).
///
/// The input is an .xml file with the scene data (information about the camera, objects
/// and bounding box).
/// The output is the rendering of the scene.
///======================================================================================
///======================================================================================
//

#include "Application/App.h"



int main()
{
    std::cout << "select file with scene data (.xml):" << std::endl;
    std::string fileName;
    std::cin >> fileName;

    try {
        ezg::AppLVL4 app;

        if(app.loadSceneFromXML(fileName) != tinyxml2::XML_SUCCESS) {
            std::cerr << "cant load scene from file: " << fileName << std::endl;
            return 1;
        }

        app.run();
    }
    catch (std::exception& ex) {
        std::cerr << "What: " << ex.what() << std::endl;
        return 1;
    }

    return 0;

}