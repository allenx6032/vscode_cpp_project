/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#include "cocos2d-CCAutoPolygon.h"
#include <algorithm>
#include <math.h>

USING_NS_CC;

static unsigned short quadIndices9[]={
    0+4*0,1+4*0,2+4*0, 3+4*0,2+4*0,1+4*0,
    0+4*1,1+4*1,2+4*1, 3+4*1,2+4*1,1+4*1,
    0+4*2,1+4*2,2+4*2, 3+4*2,2+4*2,1+4*2,
    0+4*3,1+4*3,2+4*3, 3+4*3,2+4*3,1+4*3,
    0+4*4,1+4*4,2+4*4, 3+4*4,2+4*4,1+4*4,
    0+4*5,1+4*5,2+4*5, 3+4*5,2+4*5,1+4*5,
    0+4*6,1+4*6,2+4*6, 3+4*6,2+4*6,1+4*6,
    0+4*7,1+4*7,2+4*7, 3+4*7,2+4*7,1+4*7,
    0+4*8,1+4*8,2+4*8, 3+4*8,2+4*8,1+4*8,
};

static unsigned short quadIndices[]={0,1,2, 3,2,1};

PolygonInfo::PolygonInfo()
: _rect(Rect::ZERO)
, _filename("")
, _isVertsOwner(true)
{
    triangles.verts = nullptr;
    triangles.indices = nullptr;
    triangles.vertCount = 0;
    triangles.indexCount = 0;
};

PolygonInfo::PolygonInfo(const PolygonInfo& other)
: triangles()
, _rect()
, _isVertsOwner(true)
{
    _filename = other._filename;
    _isVertsOwner = true;
    _rect = other._rect;
    triangles.verts = new (std::nothrow) V2F_C4B_T2F[other.triangles.vertCount];
    triangles.indices = new (std::nothrow) unsigned short[other.triangles.indexCount];
    CCASSERT(triangles.verts && triangles.indices, "not enough memory");
    triangles.vertCount = other.triangles.vertCount;
    triangles.indexCount = other.triangles.indexCount;
    memcpy(triangles.verts, other.triangles.verts, other.triangles.vertCount * sizeof(other.triangles.verts[0]));
    memcpy(triangles.indices, other.triangles.indices, other.triangles.indexCount * sizeof(other.triangles.indices[0]));
};

PolygonInfo& PolygonInfo::operator= (const PolygonInfo& other)
{
    if(this != &other)
    {
        releaseVertsAndIndices();
        _filename = other._filename;
        _isVertsOwner = true;
        _rect = other._rect;
        triangles.verts = new (std::nothrow) V2F_C4B_T2F[other.triangles.vertCount];
        triangles.indices = new (std::nothrow) unsigned short[other.triangles.indexCount];
        CCASSERT(triangles.verts && triangles.indices, "not enough memory");
        triangles.vertCount = other.triangles.vertCount;
        triangles.indexCount = other.triangles.indexCount;
        memcpy(triangles.verts, other.triangles.verts, other.triangles.vertCount * sizeof(other.triangles.verts[0]));
        memcpy(triangles.indices, other.triangles.indices, other.triangles.indexCount * sizeof(other.triangles.indices[0]));
    }
    return *this;
}

PolygonInfo::~PolygonInfo()
{
    releaseVertsAndIndices();
}

void PolygonInfo::setQuad(V2F_C4B_T2F_Quad *quad)
{
    releaseVertsAndIndices();
    _isVertsOwner = false;
    triangles.indices = quadIndices9;
    triangles.vertCount = 4;
    triangles.indexCount = 6;
    triangles.verts = (V2F_C4B_T2F*)quad;
}

void PolygonInfo::setQuads(V2F_C4B_T2F_Quad *quad, int numberOfQuads)
{
    CCASSERT(numberOfQuads >= 1 && numberOfQuads <= 9, "Invalid number of Quads");

    releaseVertsAndIndices();
    _isVertsOwner = false;
    triangles.indices = quadIndices9;
    triangles.vertCount = 4 * numberOfQuads;
    triangles.indexCount = 6 * numberOfQuads;
    triangles.verts = (V2F_C4B_T2F*)quad;
}

void PolygonInfo::setTriangles(const TrianglesCommand::Triangles& other)
{
    this->releaseVertsAndIndices();
    _isVertsOwner = false;

    this->triangles.vertCount = other.vertCount;
    this->triangles.indexCount = other.indexCount;
    this->triangles.verts = other.verts;
    this->triangles.indices = other.indices;
}

void PolygonInfo::releaseVertsAndIndices()
{
    if(_isVertsOwner)
    {
        if(nullptr != triangles.verts)
        {
            CC_SAFE_DELETE_ARRAY(triangles.verts);
        }

        if(nullptr != triangles.indices)
        {
            CC_SAFE_DELETE_ARRAY(triangles.indices);
        }
    }
}

const unsigned int PolygonInfo::getVertCount() const
{
    return (unsigned int)triangles.vertCount;
}

const unsigned int PolygonInfo::getTriaglesCount() const
{
    return (unsigned int)triangles.indexCount / 3;
}

const float PolygonInfo::getArea() const
{
    float area = 0;
    V2F_C4B_T2F *verts = triangles.verts;
    unsigned short *indices = triangles.indices;
    for(int i = 0; i < triangles.indexCount; i+=3)
    {
        auto A = verts[indices[i]].vertices;
        auto B = verts[indices[i+1]].vertices;
        auto C = verts[indices[i+2]].vertices;
        area += (A.x*(B.y-C.y) + B.x*(C.y-A.y) + C.x*(A.y - B.y))/2;
    }
    return area;
}

const Rect& PolygonInfo::getRect() const
{
    return _rect;
}

void PolygonInfo::setRect(const Rect& rect)
{
    _rect = rect;
}

const std::string& PolygonInfo::getFilename() const
{
    return _filename;
}

void PolygonInfo::setFilename(const std::string& filename )
{
    _filename = filename;
}
