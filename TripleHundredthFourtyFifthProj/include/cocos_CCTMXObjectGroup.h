/****************************************************************************
Copyright (c) 2010      Neophit
Copyright (c) 2010      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.

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
#ifndef __CCTMX_OBJECT_GROUP_H__
#define __CCTMX_OBJECT_GROUP_H__

#include "cocos_CCGeometry.h"
#include "cocos_CCValue.h"
#include "cocos_CCRef.h"

NS_CC_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

/** @brief TMXObjectGroup represents the TMX object group.
 * @since v0.99.0
 */
class CC_DLL TMXObjectGroup : public Ref
{
public:
    /**
     * @js ctor
     */
    TMXObjectGroup();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXObjectGroup();
    
    /** Get the group name. 
     *
     * @return The group name.
     */
    const std::string& getGroupName() const { return _groupName; }
    
    /** Set the group name. 
     *
     * @param groupName A string,it is used to set the group name.
     */
    void setGroupName(const std::string& groupName) { _groupName = groupName; }

    /** Return the value for the specific property name. 
     *
     * @param propertyName The specific property name.
     * @return Return the value for the specific property name.
     * @js NA
     */
    Value getProperty(const std::string& propertyName) const;

    /** Return the dictionary for the specific object name.
     * It will return the 1st object found on the array for the given name.
     *
     * @return Return the dictionary for the specific object name.
     */
    ValueMap getObject(const std::string& objectName) const;
    
    /** Gets the offset position of child objects. 
     *
     * @return The offset position of child objects.
     */
    const Vec2& getPositionOffset() const { return _positionOffset; }
    
    /** Sets the offset position of child objects. 
     *
     * @param offset The offset position of child objects.
     */
    void setPositionOffset(const Vec2& offset) { _positionOffset = offset; }
    
    /** Gets the list of properties stored in a dictionary. 
     *
     * @return The list of properties stored in a dictionary.
     */
    ValueMap& getProperties() { return _properties; }
    
    /** Sets the list of properties.
     *
     * @param properties The list of properties.
     */
    void setProperties(const ValueMap& properties) {
        _properties = properties;
    }
    
    /** Gets the array of the objects. 
     *
     * @return The array of the objects.
     */
    ValueVector& getObjects() { return _objects; }
    
    /** Sets the array of the objects.
     *
     * @param objects The array of the objects.
     */
    void setObjects(const ValueVector& objects) {
        _objects = objects;
    }
    
    unsigned char getOpacity() { return _opacity; }
    void setOpacity(unsigned char opacity) {
        _opacity = opacity;
    }
    
    bool isVisible() { return _visible; }
    void setVisible(bool visible) {
        _visible = visible;
    }
    
protected:
    /** name of the group */
    std::string _groupName;
    /** offset position of child objects */
    Vec2 _positionOffset;
    /** list of properties stored in a dictionary */
    ValueMap _properties;
    /** array of the objects */
    ValueVector _objects;
    unsigned char _opacity;
    bool _visible;
};

// end of tilemap_parallax_nodes group
/** @} */

NS_CC_END

#endif //__CCTMX_OBJECT_GROUP_H__
