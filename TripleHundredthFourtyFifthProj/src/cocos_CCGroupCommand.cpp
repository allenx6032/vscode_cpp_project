/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/


#include "cocos_CCGroupCommand.h"
#include "cocos_CCRenderer.h"
#include "cocos_CCDirector.h"

NS_CC_BEGIN

GroupCommandManager::GroupCommandManager()
{

}

GroupCommandManager::~GroupCommandManager()
{
    
}

bool GroupCommandManager::init()
{
    //0 is the default render group
    _groupMapping[0] = true;
    return true;
}

int GroupCommandManager::getGroupID()
{
    //Reuse old id
    for(auto it = _groupMapping.begin(); it != _groupMapping.end(); ++it)
    {
        if(!it->second)
        {
            _groupMapping[it->first] = true;
            return it->first;
        }
    }

    //Create new ID
//    int newID = _groupMapping.size();
    int newID = Director::getInstance()->getRenderer()->createRenderQueue();
    _groupMapping[newID] = true;

    return newID;
}

void GroupCommandManager::releaseGroupID(int groupID)
{
    _groupMapping[groupID] = false;
}

GroupCommand::GroupCommand()
{
    _type = RenderCommand::Type::GROUP_COMMAND;
    _renderQueueID = Director::getInstance()->getRenderer()->getGroupCommandManager()->getGroupID();
}

void GroupCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
    auto manager = Director::getInstance()->getRenderer()->getGroupCommandManager();
    manager->releaseGroupID(_renderQueueID);
    _renderQueueID = manager->getGroupID();
}

GroupCommand::~GroupCommand()
{
    Director::getInstance()->getRenderer()->getGroupCommandManager()->releaseGroupID(_renderQueueID);
}

NS_CC_END
