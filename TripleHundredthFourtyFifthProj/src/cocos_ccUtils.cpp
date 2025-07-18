/****************************************************************************
Copyright (c) 2010      cocos2d-x.org
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

#include <stdlib.h>
#include "cocos_ccUtils.h"
#include "cocos_CCFileUtils.h"

NS_CC_BEGIN

int ccNextPOT(int x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

namespace utils
{
std::vector<Node*> findChildren(const Node &node, const std::string &name)
{
    std::vector<Node*> vec;
    
    node.enumerateChildren(name, [&vec](Node* nodeFound) -> bool {
        vec.push_back(nodeFound);
        return false;
    });

    return vec;
}

#define MAX_ITOA_BUFFER_SIZE 256
double atof(const char* str)
{
    if (str == nullptr)
    {
        return 0.0;
    }
    
    char buf[MAX_ITOA_BUFFER_SIZE];
    strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);
    
    // strip string, only remain 7 numbers after '.'
    char* dot = strchr(buf, '.');
    if (dot != nullptr && dot - buf + 8 <  MAX_ITOA_BUFFER_SIZE)
    {
        dot[8] = '\0';
    }
    
    return ::atof(buf);
}

double gettime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return (double)tv.tv_sec + (double)tv.tv_usec/1000000;
}

Rect getCascadeBoundingBox(Node *node)
{
    Rect cbb;
    Size contentSize = node->getContentSize();
    
    // check all childrens bounding box, get maximize box
    Node* child = nullptr;
    bool merge = false;
    for(auto object : node->getChildren())
    {
        child = dynamic_cast<Node*>(object);
        if (!child->isVisible()) continue;
        
        const Rect box = getCascadeBoundingBox(child);
        if (box.size.width <= 0 || box.size.height <= 0) continue;
        
        if (!merge)
        {
            cbb = box;
            merge = true;
        }
        else
        {
            cbb.merge(box);
        }
    }
    
    // merge content size
    if (contentSize.width > 0 && contentSize.height > 0)
    {
        const Rect box = RectApplyAffineTransform(Rect(0, 0, contentSize.width, contentSize.height), node->getNodeToWorldAffineTransform());
        if (!merge)
        {
            cbb = box;
        }
        else
        {
            cbb.merge(box);
        }
    }
    
    return cbb;
}
    
}

NS_CC_END
