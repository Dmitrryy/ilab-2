/*************************************************************************************************
 *
 *   main_genScene.cpp
 *
 *   Created by dmitry
 *   31.03.2021
 *
 ***/

//
/// gen scene
///======================================================================================
/// this application is used to generate scene data files that can be parsed in the main
/// application.
/// This is where the description ends ... There, and so everything is displayed on the
/// console that it want from you.
///======================================================================================
///======================================================================================
//

#include <iostream>
#include <tinyxml2.h>
#include <filesystem>
#include <vector>

#include <OtherLibs/random.h>

int main()
{
    auto &&inPut = std::cin;
    auto &&outPut = std::cout;

    std::string commandForClean = "clear";

    tinyxml2::XMLDocument scene;

    scene.InsertEndChild(scene.NewDeclaration());


    {
        auto &&box = scene.NewElement("box");
        system(commandForClean.c_str());
        outPut << "set two points for box (limiting the world) in format: x1 y1 z1 x2 y2 z2" << std::endl;

        float coord = 0.f;
        inPut >> coord;
        box->SetAttribute("x1", coord);
        inPut >> coord;
        box->SetAttribute("y1", coord);
        inPut >> coord;
        box->SetAttribute("z1", coord);

        inPut >> coord;
        box->SetAttribute("x2", coord);
        inPut >> coord;
        box->SetAttribute("y2", coord);
        inPut >> coord;
        box->SetAttribute("z2", coord);

        scene.InsertEndChild(box);
    }

    {
        auto&& camera = scene.NewElement("camera");
        system(commandForClean.c_str());
        outPut << "set position of camera: x y z" << std::endl;

        float x = 0, y = 0, z = 0;
        inPut >> x >> y >> z;

        camera->InsertNewChildElement("position");
        camera->FirstChildElement("position")->SetAttribute("x", x);
        camera->FirstChildElement("position")->SetAttribute("y", y);
        camera->FirstChildElement("position")->SetAttribute("z", z);


        outPut << std::endl << "set direction of camera: x y z" << std::endl;
        inPut >> x >> y >> z;

        camera->InsertNewChildElement("direction");
        camera->FirstChildElement("direction")->SetAttribute("x", x);
        camera->FirstChildElement("direction")->SetAttribute("y", y);
        camera->FirstChildElement("direction")->SetAttribute("z", z);


        outPut << std::endl << "set speed of camera: v" << std::endl;
        inPut >> x;
        camera->SetAttribute("speed", x);

        scene.InsertEndChild(camera);
    }

    system(commandForClean.c_str());
    outPut << "set num of objects" << std::endl;
    size_t num = 0;
    inPut >> num;

    system(commandForClean.c_str());
    outPut << "chose path with model (.obj)" << std::endl;
    std::string pathModel;
    inPut >> pathModel;

    std::vector< std::string > modelFiles;
    const std::string extension(".obj");
    try {
        for (auto &p : std::filesystem::directory_iterator(pathModel)) {
            if (!std::filesystem::is_regular_file(p.status()))
                continue;

            std::string name(p.path().string());

            bool match = !name.compare(name.size() - extension.size(), extension.size(), extension);

            if (match) {
                modelFiles.emplace_back(name);
            }
        }
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    system(commandForClean.c_str());
    outPut << "detected " << modelFiles.size() << " files in path: " << pathModel << std::endl;
    outPut << "They are: " << std::endl;
    for (size_t i = 0, mi = modelFiles.size(); i < mi; ++i) {
        outPut << '[' << i << "] " << modelFiles.at(i) << std::endl;
    }
    outPut << std::endl << "do you want random gen of all objects? (y/n)" << std::endl;
    char c = 0;
    inPut >> c;
    if (c == '\n' || c == 'y')
    {
        ezg::Random rand_scale(0, 10);
        ezg::Random rand_model(0, (modelFiles.empty()) ? 0 : (modelFiles.size() - 1));
        ezg::Random rand_color(0, 256);

        ezg::Random rand_dirTravel(-100, 100);
        ezg::Random rand_dirRotate(-100, 100);
        ezg::Random rand_rotateSpeed(-100, 100);

        ezg::Random rand_stagesGrow(0, 1000);
        ezg::Random rand_distanceGrow(0, 10);

        auto &&objects = scene.NewElement("objects");
        for (size_t i = 0; i < num; ++i) {
            auto &&cur = scene.NewElement(("obj" + std::to_string(i)).c_str());

            size_t id = rand_model();
            cur->InsertNewChildElement("model");
            cur->FirstChildElement("model")->SetAttribute("file", modelFiles.at(id).c_str());

            cur->InsertNewChildElement("properties");

            float x = rand_scale(), y = rand_scale(), z = rand_scale();
            cur->FirstChildElement("properties")->InsertNewChildElement("scale");
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("z", z);

            x = rand_color() / 256.f, y = rand_color() / 256.f, z = rand_color() / 256.f;
            cur->FirstChildElement("properties")->InsertNewChildElement("color");
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("r", x);
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("g", y);
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("b", z);

            x = 0, y = 0, z = 0;//TODO
            cur->FirstChildElement("properties")->InsertNewChildElement("position");
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("z", z);

            x = rand_dirTravel(), y = rand_dirTravel(), z = rand_dirTravel();
            cur->FirstChildElement("properties")->InsertNewChildElement("dirTravel");
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("z", z);

            x = rand_dirRotate(), y = rand_dirRotate(), z = rand_dirRotate();
            cur->FirstChildElement("properties")->InsertNewChildElement("dirRotate");
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("z", z);

            x = rand_rotateSpeed();
            cur->FirstChildElement("properties")->InsertNewChildElement("speedRotation");
            cur->FirstChildElement("properties")->FirstChildElement("speedRotation")->SetAttribute("val", x);

            outPut << "number of growth stages" << std::endl;
            size_t n = rand_stagesGrow();
            cur->InsertNewChildElement("grow");
            cur->FirstChildElement("grow")->SetAttribute("stages", n);

            outPut << "average distance" << std::endl;
            float dist = rand_distanceGrow();
            cur->FirstChildElement("grow")->SetAttribute("distance", dist);

            objects->InsertEndChild(cur);
        }
        scene.InsertEndChild(objects);
    }
    else {
        auto &&objects = scene.NewElement("objects");
        for (size_t i = 0; i < num; ++i) {
            auto &&cur = scene.NewElement(("obj" + std::to_string(i)).c_str());

            system(commandForClean.c_str());
            outPut << "settings object with id: " << i << std::endl;

            outPut << "detected " << modelFiles.size() << " files in path: " << pathModel << std::endl;
            outPut << "They are: " << std::endl;
            for (size_t i = 0, mi = modelFiles.size(); i < mi; ++i) {
                outPut << '[' << i << "] " << modelFiles.at(i) << std::endl;
            }

            outPut << "chose id of file with model" << std::endl;
            size_t id = 0;
            inPut >> id;
            cur->InsertNewChildElement("model");
            cur->FirstChildElement("model")->SetAttribute("file", modelFiles.at(id).c_str());

            cur->InsertNewChildElement("properties");

            float x = 0, y = 0, z = 0;
            outPut << "set scale x y z" << std::endl;
            inPut >> x >> y >> z;
            cur->FirstChildElement("properties")->InsertNewChildElement("scale");
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("scale")->SetAttribute("z", z);

            outPut << "set color r g b" << std::endl;
            inPut >> x >> y >> z;
            cur->FirstChildElement("properties")->InsertNewChildElement("color");
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("r", x);
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("g", y);
            cur->FirstChildElement("properties")->FirstChildElement("color")->SetAttribute("b", z);

            outPut << "set position x y z" << std::endl;
            inPut >> x >> y >> z;
            cur->FirstChildElement("properties")->InsertNewChildElement("position");
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("position")->SetAttribute("z", z);

            outPut << "set direction of travel x y z" << std::endl;
            inPut >> x >> y >> z;
            cur->FirstChildElement("properties")->InsertNewChildElement("dirTravel");
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("dirTravel")->SetAttribute("z", z);

            outPut << "set direction of rotation x y z" << std::endl;
            inPut >> x >> y >> z;
            cur->FirstChildElement("properties")->InsertNewChildElement("dirRotate");
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("x", x);
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("y", y);
            cur->FirstChildElement("properties")->FirstChildElement("dirRotate")->SetAttribute("z", z);

            outPut << "set speed rotation x" << std::endl;
            inPut >> x;
            cur->FirstChildElement("properties")->InsertNewChildElement("speedRotation");
            cur->FirstChildElement("properties")->FirstChildElement("speedRotation")->SetAttribute("val", x);

            outPut << "number of growth stages" << std::endl;
            size_t n = 0;
            inPut >> n;
            cur->InsertNewChildElement("grow");
            cur->FirstChildElement("grow")->SetAttribute("stages", n);

            outPut << "average distance" << std::endl;
            float dist = 0;
            inPut >> dist;
            cur->FirstChildElement("grow")->SetAttribute("distance", dist);

            objects->InsertEndChild(cur);
        }
        scene.InsertEndChild(objects);
    }

    system(commandForClean.c_str());
    outPut << "file name for save xml:" << std::endl;
    std::string outFile;
    inPut >> outFile;

    scene.SaveFile(outFile.c_str());

    return 0;
}