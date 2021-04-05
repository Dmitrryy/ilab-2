/****************************************************************************************
 *
 *   App.cpp
 *
 *   Created by dmitry
 *   22.02.2021
 *
 ***/

//
/// App
///======================================================================================
/// the tinyxml2 library is used to parse the input scene data file.
/// Unlike the previous version, the displayed promises are inherited from the class
/// that the engine can display.
/// More details in the implementation files
///======================================================================================
///======================================================================================
//


#include "App.h"
#include "Entity.hpp"

#include <Octree/Octree.h>


#include <OtherLibs/random.h>


#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH  1000


namespace ezg
{


    AppLVL4::AppLVL4()
    {
        //-----------------------------------------------
        // create window
        int res = glfwInit();
        if (res != GLFW_TRUE) {
            throw std::runtime_error("failed glfwInit!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LVL 4", nullptr, nullptr);
        if (m_pWindow == nullptr) {
            throw std::runtime_error("failed create window");
        }

        glfwSetWindowUserPointer(m_pWindow, this);
        glfwSetKeyCallback(m_pWindow, keyCallback);

        glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, 1);

        int wHeight = 0, wWidth = 0;
        glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

        glfwSetCursorPos(m_pWindow, wWidth / 2.0, wHeight / 2.0);
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        // end create window
        //-----------------------------------------------

        m_driver = std::make_unique< Engine >(m_pWindow);

        //-----------------------------------------------
        // init camera view
        m_cameraView.setAspect(static_cast< float >(wHeight) / static_cast< float >(wWidth));
        // end init camera view
        //-----------------------------------------------
    }

    AppLVL4::~AppLVL4() {
        glfwDestroyWindow(m_pWindow);

        std::transform(m_entities.begin(), m_entities.end(), m_entities.begin(), [&driver = m_driver](auto* pEnt) -> TriangleMesh* {
            driver->unload_mesh(*pEnt);
            delete pEnt;
            return nullptr;
        });

    }

    /****************************************************************************************
     *
     *   public methods
     *
     ***/

/*
    template< typename T, typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const std::string& name, T& res, Args &...args);

*/

    template< typename T = void >
    void getAttribute(tinyxml2::XMLElement*) {}

    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const std::string& name, float& res, Args &...args)
    {
        res = xmlElement->FloatAttribute(name.c_str());
        getAttribute(xmlElement, args...);
    }
    /*
     * specializations for other types of attribute
     * ...
     */


    tinyxml2::XMLError AppLVL4::loadSceneFromXML(const std::string &fileName)
    {
        tinyxml2::XMLDocument doc;
        auto &&res = doc.LoadFile(fileName.c_str());
        if (res != tinyxml2::XML_SUCCESS) {
            return res;
        }

        auto &&boxXML = doc.FirstChildElement("box");
        if (boxXML != nullptr) {
            float x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0;
            getAttribute(boxXML, "x1", x1, "y1", y1, "z1", z1);
            getAttribute(boxXML, "x2", x2, "y2", y2, "z2", z2);

            m_box.reup({x1, y1, z1}, {x2, y2, z2});
        }

        auto &&cameraXML = doc.FirstChildElement("camera");
        if (cameraXML != nullptr) {
            m_speed = cameraXML->FloatAttribute("speed");

            auto&& posXML = cameraXML->FirstChildElement("position");
            if (posXML != nullptr) {
                getAttribute(posXML, "x", m_cameraView.m_position.x
                                , "y", m_cameraView.m_position.y
                                , "z", m_cameraView.m_position.z);
            }

            auto&& dirXML = cameraXML->FirstChildElement("direction");
            if (dirXML != nullptr) {
                getAttribute(dirXML, "x", m_cameraView.m_direction.x
                            , "y", m_cameraView.m_direction.y
                            , "z", m_cameraView.m_direction.z);
            }
        }

        tinyxml2::XMLElement *objectXML = doc.FirstChildElement("objects");
        if (objectXML != nullptr) {
            objectXML = objectXML->FirstChildElement();
            while (objectXML != nullptr) {
                auto &&entity = new TriangleMesh();
                m_entities.push_back(entity);

                auto&& modelXML = objectXML->FirstChildElement("model");
                if (modelXML == nullptr || !entity->load_from_obj(modelXML->Attribute("file"))) {
                    std::cerr << "cant load model" << std::endl;
                    continue;
                }

                auto &&prop = objectXML->FirstChildElement("properties");
                if (prop != nullptr)
                {
                    auto&& posXML = prop->FirstChildElement("position");
                    if (posXML != nullptr) {
                        getAttribute(posXML, "x", entity->m_position.x, "y", entity->m_position.y, "z", entity->m_position.z);
                    }

                    auto&& colorXML = prop->FirstChildElement("color");
                    if (colorXML != nullptr) {
                        getAttribute(colorXML, "r", entity->m_color.x, "g", entity->m_color.y, "b", entity->m_color.z);
                    }

                    auto&& scaleXML = prop->FirstChildElement("scale");
                    if (scaleXML != nullptr) {
                        getAttribute(scaleXML, "x", entity->m_scale.x, "y", entity->m_scale.y, "z", entity->m_scale.z);
                    }

                    auto&& dirTravelXML = prop->FirstChildElement("dirTravel");
                    if (dirTravelXML != nullptr) {
                        getAttribute(dirTravelXML, "x", entity->m_dirTravel.x, "y", entity->m_dirTravel.y, "z", entity->m_dirTravel.z);
                    }

                    auto&& dirRotateXML = prop->FirstChildElement("dirRotate");
                    if (dirRotateXML != nullptr) {
                        getAttribute(dirRotateXML, "x", entity->m_dirRotation.x, "y", entity->m_dirRotation.y, "z", entity->m_dirRotation.z);
                    }

                    auto&& speedRotationXML = prop->FirstChildElement("speedRotation");
                    if (speedRotationXML != nullptr) {
                        entity->m_speedRotation = speedRotationXML->FloatAttribute("val");
                    }
                }


                auto &&grow = objectXML->FirstChildElement("grow");
                if (grow != nullptr) {
                    size_t stages = grow->Unsigned64Attribute("stages");
                    float max_dist = grow->FloatAttribute("distance");

                    entity->vertices.reserve(entity->vertices.size() + stages * 2);

                    // the random number generator can only generate integers.
                    // To get a random non-integer number, we multiply the limits
                    // by accuracy and, during the generation itself, divide by accuracy
                    const size_t accuracy = 100000;
                    const uint max = static_cast<uint>(max_dist * accuracy);
                    ezg::Random randDist(max / 6, max);

                    for (size_t i = 0; i < stages; ++i) {
                        ezg::Random randTriangle(0, entity->vertices.size() / 3 - 1);

                        entity->grow(randTriangle(), static_cast<float>(randDist()) / accuracy);
                    }
                }

                objectXML = objectXML->NextSiblingElement();
            }
        }

        m_sceneIsLoaded = true;
        return tinyxml2::XML_SUCCESS;
    }


    void AppLVL4::run()
    {
        if (!m_sceneIsLoaded) {
            throw std::runtime_error("scene isn't loaded!");
        }

        for (auto &&o : m_entities) {
            m_driver->upload_mesh(*o);
        }

        m_time.reset();

        Timer frameTimer;
        while (!glfwWindowShouldClose(m_pWindow))
        {
            float dTime = frameTimer.elapsed();
            frameTimer.reset();

            glfwPollEvents();

            update_entities_(dTime);

            update_camera_(dTime);
            m_driver->setCameraView(m_cameraView);

            m_driver->render_meshes(m_entities);
        }

        std::cout << "App closed!" << std::endl;
    }

    /***
     *
     *   public methods (end)
     *
     ****************************************************************************************/
    /****************************************************************************************
     *
     *   private methods
     *
     ***/


    void AppLVL4::update_entities_(float time)
    {
        for (auto &&entitie : m_entities) {
            auto &&curEntity = static_cast<TriangleMesh *>(entitie);

            //we detected the exit of the figure outside the box
            if (curEntity->m_position.x < m_box.getA().x || curEntity->m_position.x > m_box.getB().x) {
                curEntity->m_dirTravel.x = std::abs(curEntity->m_dirTravel.x)
                                           * ((m_box.getA().x - curEntity->m_position.x) /
                                              std::abs(m_box.getA().x - curEntity->m_position.x));
            }
            if (curEntity->m_position.y < m_box.getA().y || curEntity->m_position.y > m_box.getB().y) {
                curEntity->m_dirTravel.y = std::abs(curEntity->m_dirTravel.y)
                                           * ((m_box.getA().y - curEntity->m_position.y) /
                                              std::abs(m_box.getA().y - curEntity->m_position.y));
            }
            if (curEntity->m_position.z < m_box.getA().z || curEntity->m_position.z > m_box.getB().z) {
                curEntity->m_dirTravel.z = std::abs(curEntity->m_dirTravel.z)
                                           * ((m_box.getA().z - curEntity->m_position.z) /
                                              std::abs(m_box.getA().z - curEntity->m_position.z));
            }
            curEntity->update(time);
        }

    }


    void AppLVL4::update_camera_(float time)
    {
        int wHeight = 0, wWidth = 0;
        glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);
        if (glfwGetKey(m_pWindow, GLFW_KEY_TAB) != GLFW_PRESS) {
            double x = 0, y = 0;
            glfwGetCursorPos(m_pWindow, &x, &y);
            const float dx = wWidth / 2.f - x;
            const float dy = wHeight / 2.f - y;

            m_cameraView.turnInHorizontalPlane(glm::radians(90.f * dx / wWidth));
            m_cameraView.turnInVerticalPlane(glm::radians(90.f * dy / wHeight));

            glfwSetCursorPos(m_pWindow, wWidth / 2.0, wHeight / 2.0);
        }

        {
            bool forward = glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS;
            bool back = glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS;
            if (forward && back) { forward = back = false; }

            bool left = glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS;
            bool right = glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS;
            if (left && right) { left = right = false; }


            if (forward || back) {
                m_cameraView.moveAlongDirection(time * m_speed * ((forward) ? 1.f : -1.f));
            }
            if (left || right) {
                m_cameraView.movePerpendicularDirection(time * m_speed * ((right) ? 1.f : -1.f));
            }
        }

        {
            bool down = glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            bool up = glfwGetKey(m_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS;
            if (down && up) { down = up = false; }

            if (down || up) {
                m_cameraView.moveVertical(time * m_speed * ((down) ? -1.f : 1.f));
            }
        }
    }


    /****************************************************************************************
     *
     *   window event response functions
     *
     ***/

    /*static*/ void AppLVL4::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        auto app = static_cast<AppLVL4 *>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_TAB) {
            if (action == GLFW_PRESS) {
                glfwSetInputMode(app->m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else if (action == GLFW_RELEASE) {
                int wHeight = 0, wWidth = 0;
                glfwGetWindowSize(app->m_pWindow, &wWidth, &wHeight);
                glfwSetCursorPos(app->m_pWindow, wWidth / 2.0, wHeight / 2.0);
                glfwSetInputMode(app->m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            }
        }
    }

    /***
     *
     *   window event response functions (end)
     *
     *********************************************************************************************/

}//namespace ezg