/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
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

#ifndef __CC_RAY_H_
#define __CC_RAY_H_

#include "cocos_CCMath.h"
#include "cocos_CCAABB.h"
#include "cocos_CCOBB.h"

NS_CC_BEGIN

class CC_DLL Ray
{
public:
    /**
     * Constructor.
     */
    Ray();

    /**
     * Constructor.
     */
    Ray(const Ray& ray);
    
    /**
     * Constructs a new ray initialized to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
    Ray(const Vec3& origin, const Vec3& direction);

    /**
     * Destructor.
     */
    ~Ray();

    /**
     * Check whether this ray intersects the specified bounding box.
     */
    bool intersects(const AABB& aabb) const;

    /**
     * Check whether this ray intersects the specified obb.
     */
    bool intersects(const OBB& obb) const;

    /**
     * Sets this ray to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
    void set(const Vec3& origin, const Vec3& direction);

    /**
     * Transforms this ray by the given transformation matrix.
     *
     * @param matrix The transformation matrix to transform by.
     */
    void transform(const Mat4& matrix);

    Vec3 _origin;        // The ray origin position.
    Vec3 _direction;     // The ray direction vector.
};

NS_CC_END

#endif
