/*************************************************************************************************
 *
 *   tinyxml2+.hpp
 *
 *   Created by dmitry
 *   11.04.2021
 *
 ***/
#pragma once

#include "tinyxml2.h"

namespace tinyxml2p
{

    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, Args&& ...args);


    static void getAttribute(tinyxml2::XMLElement*) { /* nop */ }


    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const std::string& name, float& res, Args&& ...args)
    {
        res = xmlElement->FloatAttribute(name.c_str());
        getAttribute(xmlElement, std::forward< Args >(args)...);
    }

    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const char* name, float& res, Args&& ...args)
    {
        res = xmlElement->FloatAttribute(name);
        getAttribute(xmlElement, std::forward< Args >(args)...);
    }


    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const std::string& name, int& res, Args&& ...args)
    {
        res = xmlElement->IntAttribute(name.c_str());
        getAttribute(xmlElement, std::forward< Args >(args)...);
    }

    template< typename ...Args >
    void getAttribute(tinyxml2::XMLElement* xmlElement, const char* name, int& res, Args&& ...args)
    {
        res = xmlElement->IntAttribute(name);
        getAttribute(xmlElement, std::forward< Args >(args)...);
    }

    /*
     * specializations for other types of attribute
     * ...
     */

}//tinyxml2p