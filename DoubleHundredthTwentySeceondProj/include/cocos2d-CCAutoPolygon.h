﻿/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef COCOS_2D_CCAUTOPOLYGON_H__
#define COCOS_2D_CCAUTOPOLYGON_H__

#include <string>
#include <vector>
#include "cocos2d-CCImage.h"
#include "cocos2d-CCTrianglesCommand.h"

#include "cocos2d-CCPolygonInfo.h"

NS_CC_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

/**
 * AutoPolygon is a helper Object
 * AutoPolygon's purpose is to process an image into 2d polygon mesh in runtime
 * It has functions for each step in the process, from tracing all the points, to triangulation
 * the result can be then passed to Sprite::create() to create a Polygon Sprite
 */
class CC_DLL AutoPolygon
{
public:
    /**
     * create an AutoPolygon and initialize it with an image file
     * the image must be a 32bit PNG for current version 3.7
     * @param   filename    a path to image file, e.g., "scene1/monster.png".
     * @return  an AutoPolygon object;
     */
    AutoPolygon(const std::string &filename);

    /**
     * Destructor of AutoPolygon.
     */
    ~AutoPolygon();

    /**
     * trace all the points along the outline of the image,
     * @warning must create AutoPolygon with filename to use this function
     * @param   rect    a texture rect for specify an area of the image
     * @param   threshold   the value when alpha is greater than this value will be counted as opaque, default to 0.0
     * @return  a vector of vec2 of all the points found in clockwise order
     * @code
     * auto ap = AutoPolygon("grossini.png");
     * auto rect = Rect(100, 100, 200, 200);
     * std::vector<Vec2> points = ap.trace(rect);//default threshold is 0.0
     * @endcode
     */
     std::vector<Vec2> trace(const cocos2d::Rect& rect, float threshold = 0.0f);

    /**
     * reduce the amount of points so its faster for GPU to process and draw
     * based on Ramer-Douglas-Peucker algorithm
     * @param   points  a vector of Vec2 points as input
     * @param   rect    a texture rect for specify an area of the image to avoid over reduction
     * @param   epsilon the perpendicular distance where points smaller than this value will be discarded
     * @return  a vector of Vec2 of the remaining points in clockwise order
     * @code
     * auto ap = AutoPolygon();
     * std::vector<Vec2> reduced = ap.reduce(inputPoints, rect);//default epsilon is 2
     * @endcode
     */
    std::vector<Vec2> reduce(const std::vector<Vec2>& points, const Rect& rect, float epsilon = 2.0f);

    /**
     * expand the points along their edge, useful after you reduce the points that cuts into the sprite
     * using ClipperLib
     * @param   points  a vector of Vec2 points as input
     * @param   rect    a texture rect for specify an area of the image, the expanded points will be clamped in this rect, ultimately resulting in a quad if the expansion is too great
     * @param   epsilon the distance which the edges will expand
     * @return  a vector of Vec2 as the result of the expansion
     * @code
     * auto ap = AutoPolygon();
     * std::vector<Vec2> expanded = ap.expand(inputPoints, rect, 2.0);
     * @endcode
     */
    std::vector<Vec2> expand(const std::vector<Vec2>& points, const Rect& rect, float epsilon);

    /**
     * Triangulate the input points into triangles for rendering
     * using poly2tri
     * @warning points must be closed loop, cannot have 2 points sharing the same position and cannot intersect itself
     * @param   points  a vector of vec2 points as input
     * @return  a Triangles object with points and indices
     * @code
     * auto ap = AutoPolygon();
     * TrianglesCommand::Triangles myPolygons = ap.triangulate(myPoints);
     * @endcode
     */
    TrianglesCommand::Triangles triangulate(const std::vector<Vec2>& points);

    /**
     * calculate the UV coordinates for each points based on a texture rect
     * @warning This method requires the AutoPolygon object to know the texture file dimension
     * @param   rect    a texture rect to specify where to map the UV
     * @param   verts   a pointer to the verts array, served both as input and output verts
     * @param   count   the count for the verts array
     * @code
     * auto ap = AutoPolygon("grossini.png");
     * TrianglesCommand::Triangles myPolygons = ap.triangulate(myPoints);
     * ap.calculateUV(rect, myPolygons.verts, 20);
     * @endcode
     */
    void calculateUV(const Rect& rect, V2F_C4B_T2F* verts, ssize_t count);

    /**
     * a helper function, packing trace, reduce, expand, triangulate and calculate uv in one function
     * @param   rect    texture rect, use Rect::ZERO for the size of the texture, default is Rect::ZERO
     * @param   epsilon the value used to reduce and expand, default to 2.0
     * @param   threshold   the value where bigger than the threshold will be counted as opaque, used in trace
     * @return  a PolygonInfo, to use with sprite
     * @code
     * auto ap = AutoPolygon("grossini.png");
     * PolygonInfo myInfo = ap.generateTriangles();//use all default values
     * auto sp1 = Sprite::create(myInfo);
     * polygonInfo myInfo2 = ap.generateTriangles(Rect::ZERO, 5.0, 0.1);//ap can be reused to generate another set of PolygonInfo with different settings
     * auto sp2 = Sprite::create(myInfo2);
     * @endcode
     */
    PolygonInfo generateTriangles(const Rect& rect = Rect::ZERO, float epsilon = 2.0f, float threshold = 0.05f);

    /**
     * a helper function, packing autoPolygon creation, trace, reduce, expand, triangulate and calculate uv in one function
     * @warning if you want to repetitively generate polygons, consider create an AutoPolygon object, and use generateTriangles function, as it only reads the file once
     * @param   filename     A path to image file, e.g., "scene1/monster.png".
     * @param   rect    texture rect, use Rect::ZERO for the size of the texture, default is Rect::ZERO
     * @param   epsilon the value used to reduce and expand, default to 2.0
     * @param   threshold   the value where bigger than the threshold will be counted as opaque, used in trace
     * @return  a PolygonInfo, to use with sprite
     * @code
     * auto sp = Sprite::create(AutoPolygon::generatePolygon("grossini.png"));
     * @endcode
     */
    static PolygonInfo generatePolygon(const std::string& filename, const Rect& rect = Rect::ZERO, float epsilon = 2.0f, float threshold = 0.05f);
protected:
    Vec2 findFirstNoneTransparentPixel(const Rect& rect, float threshold);
    std::vector<cocos2d::Vec2> marchSquare(const Rect& rect, const Vec2& first, float threshold);
    unsigned int getSquareValue(unsigned int x, unsigned int y, const Rect& rect, float threshold);

    unsigned char getAlphaByIndex(unsigned int i);
    unsigned char getAlphaByPos(const Vec2& pos);

    int getIndexFromPos(unsigned int x, unsigned int y) { return y*_width+x; }
    cocos2d::Vec2 getPosFromIndex(unsigned int i) { return cocos2d::Vec2(static_cast<float>(i%_width), static_cast<float>(i/_width)); }

    std::vector<cocos2d::Vec2> rdp(const std::vector<cocos2d::Vec2>& v, float optimization);
    float perpendicularDistance(const cocos2d::Vec2& i, const cocos2d::Vec2& start, const cocos2d::Vec2& end);

    //real rect is the size that is in scale with the texture file
    Rect getRealRect(const Rect& rect);

    Image* _image;
    unsigned char * _data;
    std::string _filename;
    unsigned int _width;
    unsigned int _height;
    float _scaleFactor;
    unsigned int _threshold;
};

NS_CC_END

#endif // #ifndef COCOS_2D_CCAUTOPOLYGON_H__
