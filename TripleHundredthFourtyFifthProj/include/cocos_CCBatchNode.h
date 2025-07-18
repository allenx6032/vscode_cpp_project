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

#ifndef __CCBATCHNODE_H__
#define __CCBATCHNODE_H__

#include "cocos_CCNode.h"
#include "cocos_CCArmatureDefine.h"
#include "cocos_CocosStudioExport.h"

namespace cocos2d {
    class GroupCommand;
}

namespace cocostudio {

class CC_STUDIO_DLL BatchNode : public cocos2d::Node
{
public:
    static BatchNode *create();
public:
	/**
     * @js ctor
     */
    BatchNode();
    /**
     * @ js NA
     * @ lua NA
     */
    ~BatchNode();
    /**
     *  @js NA
     */
    virtual bool init() override;
    using Node::addChild;
    virtual void addChild(cocos2d::Node *pChild, int zOrder, int tag) override;
    virtual void addChild(cocos2d::Node *pChild, int zOrder, const std::string &name) override;
    virtual void removeChild(cocos2d::Node* child, bool cleanup) override;
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    
protected:
    void generateGroupCommand();

    cocos2d::GroupCommand* _groupCommand;
};

}

#endif /*__CCBATCHNODE_H__*/
