/****************************************************************************************
 *
 *   App.cpp
 *
 *   Created by dmitry
 *   22.02.2021
 *
 ***/

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

    void AppLVL3::Entity::update(float time)
    {
        m_liveTimeSec -= time;

        if (m_liveTimeSec > 0.f) {
            m_angle += m_speedRotation * std::min(time, m_liveTimeSec);
        }
    }

    glm::mat4 AppLVL3::Entity::getModelMatrix() const
    {
        //todo scale
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), m_position);
        glm::mat4 res = glm::rotate(translationMatrix, glm::radians(m_angle), m_dirRotation);
        return res;
    }


    void AppLVL3::addTriangle(glm::vec3 pos_a, glm::vec3 pos_b, glm::vec3 pos_c, glm::vec3 position
                              , glm::vec3 direction_rotation, float rotation_speed, float live_time_sec)
    {
        const size_t entityId = m_entities.size();

        Entity entity;
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

        entity.m_vertices.push_back(vertex[0]);
        entity.m_vertices.push_back(vertex[1]);
        entity.m_vertices.push_back(vertex[2]);

        entity.m_position = position;
        entity.m_dirRotation = direction_rotation;
        entity.m_speedRotation = rotation_speed;
        entity.m_liveTimeSec = live_time_sec;

        m_entities.push_back(entity);
    }



    void AppLVL3::run()
    {
        init_();

        m_time.reset();

        int wHeight = 0, wWidth = 0;
        glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

        glfwSetCursorPos(m_pWindow, wWidth / 2.0, wHeight / 2.0);
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        m_cameraView.setAspect(wHeight /(float) wWidth);

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

    void AppLVL3::init_()
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
        // end create window
        //-----------------------------------------------

        m_driver.Init(m_pWindow);
        //todo
    }



    void AppLVL3::init_camera_()
    {

    }



    void AppLVL3::update_entities_(float time)
    {
        for(auto& ent : m_entities) {
            ent.update(time);
        }
    }



    void AppLVL3::load_entity_data_in_driver_()
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



    void AppLVL3::update_camera_(float time)
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

    /*static*/ void AppLVL3::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        auto app = static_cast<AppLVL3 *>(glfwGetWindowUserPointer(window));

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

    /*static*/ void AppLVL3::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = static_cast<AppLVL3*>(glfwGetWindowUserPointer(window));
        app->m_driver.detectFrameBufferResized();
        app->m_cameraView.setAspect(width /(float) height);
    }

    /***
     *
     *   window event response functions (end)
     *
     *********************************************************************************************/

}//namespace ezg