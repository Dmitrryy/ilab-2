//
// Created by dmitry on 10.11.2020.
//
#include <cassert>

#include "WindowControl.h"

namespace vks
{

    /*bool XCBControl::Init(uint32_t width_, uint32_t height_)
    {
        m_pXCBConn = xcb_connect(nullptr, nullptr);

        int error = xcb_connection_has_error(m_pXCBConn);
        if  (error) {
            printf("Error opening xcb connection error %d\n", error);
            assert(0);
        }

        printf("XCB connection opened\n");

        const xcb_setup_t *pSetup = xcb_get_setup(m_pXCBConn);

        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(pSetup);

        m_pXCBScreen = iter.data;

        printf("XCB screen %p\n", m_pXCBScreen);

        m_xcbWindow = xcb_generate_id(m_pXCBConn);


        xcb_create_window( m_pXCBConn,             // the connection to the XWindow server
                           XCB_COPY_FROM_PARENT,                     // color depth - copy from parent window
                           m_xcbWindow,                              // XID of the new window
                           m_pXCBScreen->root,                       // parent window of the new window
                           0,                                     // X coordinate
                           0,                                     // Y coordinate
                           width_,                                   // window width
                           height_,                                  // window height
                           0,                            // border width
                           XCB_WINDOW_CLASS_INPUT_OUTPUT,      // window class - couldn't find any documentation on it
                           m_pXCBScreen->root_visual,                // the visual describes the color mapping
                           0,
                           0);

        xcb_map_window(m_pXCBConn, m_xcbWindow);
        xcb_flush (m_pXCBConn);

        return true;
    }


    VkSurfaceKHR XCBControl::createSurface(VkInstance &inst_)
    {
        VkXcbSurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.connection = m_pXCBConn;
        surfaceInfo.
    }*/

}//namespace vks