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
#ifndef __LUA_SCRIPT_HANDLER_MGR_H__
#define __LUA_SCRIPT_HANDLER_MGR_H__


extern "C" {
#include "tolua++.h"
}


#include "cocos_CCRef.h"
#include "cocos_ccMacros.h"
#include "cocos_CCActionInstant.h"
#include <vector>
#include <map>

NS_CC_BEGIN

class ScheduleHandlerDelegate;


typedef std::vector<ScheduleHandlerDelegate*> VecShedule;
typedef std::map<cocos2d::Node*,VecShedule> MapNodeSchedules;

class ScheduleHandlerDelegate:public cocos2d::Ref
{
public:
    ScheduleHandlerDelegate():_isUpdateSchedule(false)
    {}
    virtual ~ScheduleHandlerDelegate()
    {}
    
    static ScheduleHandlerDelegate* create();
    
    void scheduleFunc(float elapse);
    
    virtual void update(float elapse);
    
    void setUpdateSchedule(bool isUpdateSchedule){ _isUpdateSchedule = isUpdateSchedule; }
    bool isUpdateSchedule(){ return _isUpdateSchedule; }
private:
    bool _isUpdateSchedule;
};

class LuaCallFunc:public cocos2d::CallFuncN
{
public:
    LuaCallFunc():_functionLua(nullptr)
    {}
    virtual ~LuaCallFunc()
    {}
    
    static LuaCallFunc* create(const std::function<void(void* self,Node*)>& func);
    bool initWithFunction(const std::function<void(void* self,Node*)>& func);
    virtual LuaCallFunc* clone() const override;
    virtual void execute() override;
protected:
    /**
     */
    std::function<void(void* self,Node*)> _functionLua;
    
};

class ScriptHandlerMgr
{
public:
    enum class HandlerType: int
    {
        NODE = 0,
        CALLFUNC,
        
        WEBSOCKET_OPEN,
        WEBSOCKET_MESSAGE,
        WEBSOCKET_CLOSE,
        WEBSOCKET_ERROR,
        
        GL_NODE_DRAW,
        
        ARMATURE_EVENT,
        
        EVENT_ACC,
        EVENT_CUSTIOM,
        
        EVENT_KEYBOARD_PRESSED,
        EVENT_KEYBOARD_RELEASED,
        
        EVENT_TOUCH_BEGAN,
        EVENT_TOUCH_MOVED,
        EVENT_TOUCH_ENDED,
        EVENT_TOUCH_CANCELLED,
        
        EVENT_TOUCHES_BEGAN,
        EVENT_TOUCHES_MOVED,
        EVENT_TOUCHES_ENDED,
        EVENT_TOUCHES_CANCELLED,
        
        EVENT_MOUSE_DOWN,
        EVENT_MOUSE_UP,
        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_SCROLL,
        
        EVENT_PHYSICS_CONTACT_BEGIN,
        EVENT_PHYSICS_CONTACT_PRESOLVE,
        EVENT_PHYSICS_CONTACT_POSTSOLVE,
        EVENT_PHYSICS_CONTACT_SEPERATE,
        
        EVENT_FOCUS,
        
        EVENT_SPINE_ANIMATION_START,
        EVENT_SPINE_ANIMATION_INTERRUPT,
        EVENT_SPINE_ANIMATION_END,
        EVENT_SPINE_ANIMATION_COMPLETE,
        EVENT_SPINE_ANIMATION_DISPOSE,
        EVENT_SPINE_ANIMATION_EVENT,
        EVENT_SPINE_ANIMATION_PREUPDATE,
        EVENT_SPINE_ANIMATION_POSTUPDATE,
        
        EVENT_CUSTOM_BEGAN = 10000,
        EVENT_CUSTOM_ENDED = 11000,
    };

    typedef int Handler;
    typedef std::pair<HandlerType, Handler> HandlerPair;
    typedef std::vector<HandlerPair> VecHandlerPairs;
    typedef std::map<void*,VecHandlerPairs> MapObjectHandlers;
    
    ScriptHandlerMgr(void);
    virtual ~ScriptHandlerMgr(void);
    static ScriptHandlerMgr* getInstance(void);
    static void destroyInstance(void);

    void addObjectHandler(void* object,int handler,ScriptHandlerMgr::HandlerType handlerType);
    void removeObjectHandler(void* object,ScriptHandlerMgr::HandlerType handlerType);
    int  getObjectHandler(void* object,ScriptHandlerMgr::HandlerType handlerType);
    void removeObjectAllHandlers(void* object);
    ScriptHandlerMgr::HandlerType addCustomHandler(void* object, int handler);
    
private:
    void init(void);
    static ScriptHandlerMgr* _scriptHandlerMgr;
    MapObjectHandlers _mapObjectHandlers;
};

NS_CC_END

TOLUA_API int tolua_script_handler_mgr_open(lua_State* tolua_S);

#endif //__LUA_SCRIPT_HANDLER_MGR_H__
