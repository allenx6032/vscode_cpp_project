
#include "cocos2d-CCConfig.h"
#if CC_USE_CCS > 0

/****************************************************************************
Copyright (c) 2015 Chukong Technologies Inc.
 
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

#ifndef _SKELETONNODEREADER_H_
#define _SKELETONNODEREADER_H_

#include "cocos2d-BoneNodeReader.h"

class SkeletonNodeReader : public  BoneNodeReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:

    SkeletonNodeReader();
    ~SkeletonNodeReader();

    static SkeletonNodeReader* getInstance();
    /** @deprecated Use method destroyInstance() instead */
    CC_DEPRECATED_ATTRIBUTE static void purge();
    static void destroyInstance();

    cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* boneOptions) override;
};
#endif //_SKELETONNODEREADER_H_
#endif // CC_USE_CCS
