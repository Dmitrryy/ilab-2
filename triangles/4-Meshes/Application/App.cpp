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
/// This application visualizes the intersection of triangles that can move.
///
/// The vulkan api is used for visualization. In the current version, all objects
/// (triangles) must be known before the driver is initialized. To create a window, use
/// the glfw library.
///
///
/// learn more about what happens in the loop located in the run method:
///
/// - first of all, the window events are checked(button clicks, cursor movement, etc.).
///
/// - then the object states are updated. in this case, the triangle is moved, a collision
/// is detected, and when it intersects, it is repainted in red. Since the entity class
/// does not store (store but does not update itself) data about the world position of
/// vertices, they are taken from the driver (more precisely, from the vertex shader).
///
/// - after that, the data in the vulkan driver is updated. The world matrices and the
/// color for each triangle are sent.
///
/// - Next comes the update of the camera position.
///
/// - and finally, the scene render command is sent to the driver.
///
/// Then the next iteration begins. Exit the loop if the window was closed.
///
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

    /****************************************************************************************
     *
     *   public methods
     *
     ***/


    tinyxml2::XMLError AppLVL4::loadSceneFromXML(const std::string& fileName)
    {
        tinyxml2::XMLDocument doc;
        auto&& res = doc.LoadFile(fileName.c_str());
        if (res != tinyxml2::XML_SUCCESS) {
            return res;
        }

        auto&& boxXML = doc.FirstChildElement("box");
        if (boxXML != nullptr)
        {
            float x1, x2, y1, y2, z1, z2;
            x1 = boxXML->FloatAttribute("x1");
            y1 = boxXML->FloatAttribute("y1");
            z1 = boxXML->FloatAttribute("z1");

            x2 = boxXML->FloatAttribute("x2");
            y2 = boxXML->FloatAttribute("y2");
            z2 = boxXML->FloatAttribute("z2");

            m_box.reup({x1, y1, z1}, {x2, y2, z2});
        }

        tinyxml2::XMLElement* objectXML = doc.FirstChildElement("objects")->FirstChildElement();//todo
        while(objectXML != nullptr)
        {
            auto&& entity = new TriangleMesh();

            if(!entity->load_from_obj(objectXML->FirstChildElement("model")->Attribute("file"))) {
                std::cerr << "cant load model" << std::endl;
                continue;
            }
            entity->m_position.x = objectXML->FirstChildElement("properties")->FirstChildElement("position")->FloatAttribute("x");
            entity->m_position.y = objectXML->FirstChildElement("properties")->FirstChildElement("position")->FloatAttribute("y");
            entity->m_position.z = objectXML->FirstChildElement("properties")->FirstChildElement("position")->FloatAttribute("z");

            entity->m_color.x = objectXML->FirstChildElement("properties")->FirstChildElement("color")->FloatAttribute("r");
            entity->m_color.y = objectXML->FirstChildElement("properties")->FirstChildElement("color")->FloatAttribute("g");
            entity->m_color.z = objectXML->FirstChildElement("properties")->FirstChildElement("color")->FloatAttribute("b");

            entity->m_scale.x = objectXML->FirstChildElement("properties")->FirstChildElement("scale")->FloatAttribute("x");
            entity->m_scale.y = objectXML->FirstChildElement("properties")->FirstChildElement("scale")->FloatAttribute("y");
            entity->m_scale.z = objectXML->FirstChildElement("properties")->FirstChildElement("scale")->FloatAttribute("z");

            entity->m_dirTravel.x = objectXML->FirstChildElement("properties")->FirstChildElement("dirTravel")->FloatAttribute("x");
            entity->m_dirTravel.y = objectXML->FirstChildElement("properties")->FirstChildElement("dirTravel")->FloatAttribute("y");
            entity->m_dirTravel.z = objectXML->FirstChildElement("properties")->FirstChildElement("dirTravel")->FloatAttribute("z");

            entity->m_dirRotation.x = objectXML->FirstChildElement("properties")->FirstChildElement("dirRotate")->FloatAttribute("x");
            entity->m_dirRotation.y = objectXML->FirstChildElement("properties")->FirstChildElement("dirRotate")->FloatAttribute("y");
            entity->m_dirRotation.z = objectXML->FirstChildElement("properties")->FirstChildElement("dirRotate")->FloatAttribute("z");

            entity->m_speedRotation = objectXML->FirstChildElement("properties")->FirstChildElement("speedRotation")->FloatAttribute("val");


            auto&& grow = objectXML->FirstChildElement("grow");
            if (grow != nullptr)
            {
                size_t stages = grow->Unsigned64Attribute("stages");
                float max_dist = grow->FloatAttribute("distance");

                const size_t Prost = 100000;
                const auto max = static_cast<uint>(max_dist * Prost);
                ezg::Random randDist(static_cast<uint>(max / 6), max);

                for (size_t i = 0; i < stages; ++i)
                {
                    ezg::Random randTriangle(0, entity->vertices.size() / 3 - 1);

                    entity->grow(randTriangle(), static_cast<float>(randDist()) / Prost);
                }
            }

            m_entities.push_back(entity);

            objectXML = objectXML->NextSiblingElement();
        }

        return tinyxml2::XML_SUCCESS;
    }


    void AppLVL4::run()
    {
        init_();

        for (auto&& o : m_entities){
            m_driver.upload_mesh(*o);
        }


        m_cameraView.setPosition({1.f, 1.f, 1.f});
        m_cameraView.setDirection({-1.f, -1.f, -1.f});

        m_time.reset();

        Timer frameTimer;
        while(!glfwWindowShouldClose(m_pWindow))
        {
            float dTime = frameTimer.elapsed();
            frameTimer.reset();

            glfwPollEvents();

            update_entities_(dTime);

            update_camera_(dTime);
            m_driver.setCameraView(m_cameraView);

            m_driver.render_meshes(m_entities);
        }
        m_entities.clear();
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

    void AppLVL4::init_()
    {
        //-----------------------------------------------
        // create window
        int res = glfwInit();
        if (res != GLFW_TRUE) {
            throw std::runtime_error("failed glfwInit!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
        if (m_pWindow == nullptr) {
            throw std::runtime_error("failed create window");
        }

        glfwSetWindowUserPointer(m_pWindow, this);
        glfwSetFramebufferSizeCallback(m_pWindow, framebufferResizeCallback);
        glfwSetKeyCallback(m_pWindow, keyCallback);

        glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, 1);

        int wHeight = 0, wWidth = 0;
        glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

        glfwSetCursorPos(m_pWindow, wWidth / 2.0, wHeight / 2.0);
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        // end create window
        //-----------------------------------------------

        m_driver.Init(m_pWindow);

        //-----------------------------------------------
        // init camera view
        m_cameraView.setAspect(wHeight /(float) wWidth);
        // end init camera view
        //-----------------------------------------------
    }



    void AppLVL4::update_entities_(float time)
    {
        for(size_t i = 0, mi = m_entities.size(); i < mi; ++i)
        {
            auto&& curEntity = static_cast<TriangleMesh*>(m_entities.at(i));

            //we detected the exit of the figure outside the box
            if(curEntity->m_position.x < m_box.getA().x || curEntity->m_position.x > m_box.getB().x) {
                curEntity->m_dirTravel.x = std::abs(curEntity->m_dirTravel.x) * ((m_box.getA().x - curEntity->m_position.x) / std::abs(m_box.getA().x - curEntity->m_position.x));
            }
            if(curEntity->m_position.y < m_box.getA().y || curEntity->m_position.y > m_box.getB().y) {
                curEntity->m_dirTravel.y = std::abs(curEntity->m_dirTravel.y) * ((m_box.getA().y - curEntity->m_position.y) / std::abs(m_box.getA().y - curEntity->m_position.y));
            }
            if(curEntity->m_position.z < m_box.getA().z || curEntity->m_position.z > m_box.getB().z) {
                curEntity->m_dirTravel.z = std::abs(curEntity->m_dirTravel.z) * ((m_box.getA().z - curEntity->m_position.z) / std::abs(m_box.getA().z - curEntity->m_position.z));
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


            if (forward || back)
            {
                m_cameraView.moveAlongDirection(time * m_speed * ((forward) ? 1.f : -1.f));
            }
            if (left || right)
            {
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

    /*static*/ void AppLVL4::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = static_cast<AppLVL4*>(glfwGetWindowUserPointer(window));
        app->m_driver.detectFrameBufferResized();
        app->m_cameraView.setAspect(width /(float) height);
    }

    /***
     *
     *   window event response functions (end)
     *
     *********************************************************************************************/

}//namespace ezg