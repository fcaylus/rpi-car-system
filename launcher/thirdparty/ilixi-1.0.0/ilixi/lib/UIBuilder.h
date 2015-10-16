/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_UIBUILDER_H_
#define ILIXI_UIBUILDER_H_

#include <ui/Application.h>
#include <Reflex/Reflex.h>
#include <map>
#include <string>
#include <libxml/tree.h>

namespace ilixi
{

class UIBuilder
{
public:
    UIBuilder(Application* application);

    virtual
    ~UIBuilder();

    bool
    addFile(const char* file);

//	void addFromString(const std::string& buffer);
//
//	void getObject(Reflex::Object* object);
//
//	void connectSignal();
//
//	void setValueFromString(Reflex::Object* object, param, string);
private:
    Application* _app;
    Reflex::Object* _currentObject;
    Reflex::Type* _currentType;
    std::map<std::string, Reflex::Object> _objects;

    bool
    parseNode(xmlNodePtr node);

    bool
    parseWidget(xmlNodePtr node);

    bool
    parseChildren(xmlNodePtr node);

    bool
    parseProp(xmlNodePtr node);

    bool
    parseScript(xmlNodePtr node);

    bool
    parseConnect(xmlNodePtr node);
};

} /* namespace ilixi */
#endif /* ILIXI_UIBUILDER_H_ */
