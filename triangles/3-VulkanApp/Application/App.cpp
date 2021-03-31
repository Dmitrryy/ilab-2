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
#include "../../Octree/Octree.h"


#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH  1000


namespace ezg
{

    /****************************************************************************************
     *
     *   public methods
     *
     ***/

    void AppLVL4::addTriangle(glm::vec3 pos_a, glm::vec3 pos_b, glm::vec3 pos_c, glm::vec3 position
                              , glm::vec3 direction_rotation, float rotation_speed, float live_time_sec)
    {
        if (direction_rotation == glm::vec3(0.f)) {
            return ;
        }
        const size_t entityId = m_entities.size();

        Triangle entity;
        vks::Vertex vertex[3];

        glm::vec3 normal = glm::normalize(glm::cross(pos_b - pos_a, pos_c - pos_a));

        vertex[0].pos = pos_a;
        vertex[0].normal = normal;
        vertex[0].entityId = entityId;
        vertex[1].pos = pos_b;
        vertex[1].normal = normal;
        vertex[1].entityId = entityId;
        vertex[2].pos = pos_c;
        vertex[2].normal = normal;
        vertex[2].entityId = entityId;

        entity.m_vertices.at(0) = (vertex[0]);
        entity.m_vertices.at(1) = (vertex[1]);
        entity.m_vertices.at(2) = (vertex[2]);

        entity.m_position = position;
        entity.m_dirRotation = direction_rotation;
        entity.m_speedRotation = rotation_speed;
        entity.m_liveTimeSec = live_time_sec;

        m_entities.push_back(entity);
    }



    void AppLVL4::run()
    {
        init_();

        //TODO
        //===-------------------------------------------
        // first initialization of buffers for each frame
        m_driver.render();
        m_driver.render();
        m_driver.render();
        m_driver.render();
        m_driver.render();
        //===-------------------------------------------

        m_time.reset();

        Timer frameTimer;
        while(!glfwWindowShouldClose(m_pWindow))
        {
            float dTime = frameTimer.elapsed();
            frameTimer.reset();

            glfwPollEvents();

            update_entities_(dTime);
            load_entity_data_in_driver_();

            update_camera_(dTime);
            m_driver.setCameraView(m_cameraView);

            m_driver.render();
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

    void AppLVL4::init_()
    {
        //-----------------------------------------------
        //add object in driver
        for (const auto& ent : m_entities)
        {
            vks::ObjectInfo info;
            info.vertices = ent.m_vertices;
            info.color = ent.m_color;
            info.model_matrix = ent.getModelMatrix();

            m_driver.addObject(info);
        }
        //end objects init
        //-----------------------------------------------


        //-----------------------------------------------
        // create window
        int res = glfwInit();
        if (res != GLFW_TRUE) {
            throw std::runtime_error(DEBUG_MSG("failed glfwInit!"));
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
        if (m_pWindow == nullptr) {
            throw std::runtime_error(DEBUG_MSG("failed create window"));
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

        glm::vec3 a(std::numeric_limits<double>::max()), b(std::numeric_limits<double>::min());
        for (const auto& cur : m_entities)
        {
            for (const auto& vert : cur.m_vertices)
            {
                a.x = std::min(a.x, vert.pos.x + cur.m_position.x);
                a.y = std::min(a.y, vert.pos.y + cur.m_position.y);
                a.z = std::min(a.z, vert.pos.z + cur.m_position.z);

                b.x = std::max(b.x, vert.pos.x + cur.m_position.x);
                b.y = std::max(b.y, vert.pos.y + cur.m_position.y);
                b.z = std::max(b.z, vert.pos.z + cur.m_position.z);
            }
        }

        m_cameraView.setPosition(b);
        m_cameraView.setDirection(((a + b) / 2.f) - m_cameraView.getPosition());
        // end init camera view
        //-----------------------------------------------
    }



    void AppLVL4::update_entities_(float time)
    {
        la::Vector3f a(std::numeric_limits<double>::max()), b(std::numeric_limits<double>::min());

        for(size_t i = 0, mi = m_entities.size(); i < mi; ++i) {
            Entity& curEntity = m_entities.at(i);
            curEntity.update(time);

            curEntity.m_coordsInWorld = m_driver.getWorldCoords(i);

/*
            std::cout << '[' << i << ']' << curEntity.m_coordsInWorld[0].x << ", " << curEntity.m_coordsInWorld[0].y << ", " << curEntity.m_coordsInWorld[0].z << " | "
                                         << curEntity.m_coordsInWorld[1].x << ", " << curEntity.m_coordsInWorld[1].y << ", " << curEntity.m_coordsInWorld[1].z << " | "
                                         << curEntity.m_coordsInWorld[2].x << ", " << curEntity.m_coordsInWorld[2].y << ", " << curEntity.m_coordsInWorld[2].z << std::endl;
*/

            curEntity.updateArea();
            if (curEntity.type() == Entity::Type::Triangle)
            {
                static_cast< Triangle& >(curEntity).updateLaTriangle();
            }


            a.x = std::min(a.x, curEntity.m_area.getA().x);
            a.y = std::min(a.y, curEntity.m_area.getA().y);
            a.z = std::min(a.z, curEntity.m_area.getA().z);

            b.x = std::max(b.x, curEntity.m_area.getB().x);
            b.y = std::max(b.y, curEntity.m_area.getB().y);
            b.z = std::max(b.z, curEntity.m_area.getB().z);
        }



        la::Octree< Entity* > octTree({a, b});
        for (auto& ent : m_entities) {
            ent.m_color = { 0.f, 1.f, 0.f };
            octTree.add(&ent);
        }


        octTree.msplit();

        octTree.getIntersections();
    }



    void AppLVL4::load_entity_data_in_driver_()
    {
        for (size_t i = 0, mi = m_entities.size(); i < mi; ++i)
        {
            const auto& ent = m_entities.at(i);

            vks::ObjectInfo info;
            info.color = ent.m_color;
            info.model_matrix = ent.getModelMatrix();

            m_driver.setObjectInfo(i, info);
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