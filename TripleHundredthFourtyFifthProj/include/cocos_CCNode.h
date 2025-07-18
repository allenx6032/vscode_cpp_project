/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2009      Valentin Milea
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

#ifndef __CCNODE_H__
#define __CCNODE_H__

#include "cocos_ccMacros.h"
#include "cocos_CCVector.h"
#include "cocos_CCProtocols.h"
#include "cocos_CCScriptSupport.h"
#include "cocos_CCAffineTransform.h"
#include "cocos_CCMath.h"

#if CC_USE_PHYSICS
#include "cocos_CCPhysicsBody.h"
#endif

NS_CC_BEGIN

class GridBase;
class Touch;
class Action;
class LabelProtocol;
class Scheduler;
class ActionManager;
class Component;
class ComponentContainer;
class EventDispatcher;
class Scene;
class Renderer;
class GLProgram;
class GLProgramState;

/**
 * @addtogroup base_nodes
 * @{
 */

enum {
    kNodeOnEnter,
    kNodeOnExit,
    kNodeOnEnterTransitionDidFinish,
    kNodeOnExitTransitionDidStart,
    kNodeOnCleanup
};

bool CC_DLL nodeComparisonLess(Node* n1, Node* n2);

class EventListener;

/** @brief Node is the base element of the Scene Graph. Elements of the Scene Graph must be Node objects or subclasses of it.
 The most common Node objects are: Scene, Layer, Sprite, Menu, Label.

 The main features of a Node are:
 - They can contain other Node objects (`addChild`, `getChildByTag`, `removeChild`, etc)
 - They can schedule periodic callback (`schedule`, `unschedule`, etc)
 - They can execute actions (`runAction`, `stopAction`, etc)

 Subclassing a Node usually means (one/all) of:
 - overriding init to initialize resources and schedule callbacks
 - create callbacks to handle the advancement of time
 - overriding `draw` to render the node

 Properties of Node:
 - position (default: x=0, y=0)
 - scale (default: x=1, y=1)
 - rotation (in degrees, clockwise) (default: 0)
 - anchor point (default: x=0, y=0)
 - contentSize (default: width=0, height=0)
 - visible (default: true)

 Limitations:
 - A Node is a "void" object. If you want to draw something on the screen, you should use a Sprite instead. Or subclass Node and override `draw`.

 */

class CC_DLL Node : public Ref
{
public:
    /// Default tag used for all the nodes
    static const int INVALID_TAG = -1;

    enum {
        FLAGS_TRANSFORM_DIRTY = (1 << 0),
        FLAGS_CONTENT_SIZE_DIRTY = (1 << 1),

        FLAGS_DIRTY_MASK = (FLAGS_TRANSFORM_DIRTY | FLAGS_CONTENT_SIZE_DIRTY),
    };
    /// @{
    /// @name Constructor, Destructor and Initializers

    /**
     * Allocates and initializes a node.
     * @return A initialized node which is marked as "autorelease".
     */
    static Node * create();

    /**
     * Gets the description string. It makes debugging easier.
     * @return A string
     * @js NA
     * @lua NA
     */
    virtual std::string getDescription() const;

    /// @} end of initializers



    /// @{
    /// @name Setters & Getters for Graphic Peroperties

    /**
     LocalZOrder is the 'key' used to sort the node relative to its siblings.

     The Node's parent will sort all its children based ont the LocalZOrder value.
     If two nodes have the same LocalZOrder, then the node that was added first to the children's array will be in front of the other node in the array.
     
     Also, the Scene Graph is traversed using the "In-Order" tree traversal algorithm ( http://en.wikipedia.org/wiki/Tree_traversal#In-order )
     And Nodes that have LocalZOder values < 0 are the "left" subtree
     While Nodes with LocalZOder >=0 are the "right" subtree.
     
     @see `setGlobalZOrder`
     @see `setVertexZ`
     */
    virtual void setLocalZOrder(int localZOrder);

    /**
     * Gets the local Z order of this node.
     *
     * @see `setLocalZOrder(int)`
     *
     * @return The local (relative to its siblings) Z order.
     */
    virtual int getLocalZOrder() const { return _localZOrder; }

    /**
     Defines the oder in which the nodes are renderer.
     Nodes that have a Global Z Order lower, are renderer first.
     
     In case two or more nodes have the same Global Z Order, the oder is not guaranteed.
     The only exception if the Nodes have a Global Z Order == 0. In that case, the Scene Graph order is used.
     
     By default, all nodes have a Global Z Order = 0. That means that by default, the Scene Graph order is used to render the nodes.
     
     Global Z Order is useful when you need to render nodes in an order different than the Scene Graph order.
     
     Limitations: Global Z Order can't be used used by Nodes that have SpriteBatchNode as one of their acenstors.
     And if ClippingNode is one of the ancestors, then "global Z order" will be relative to the ClippingNode.

     @see `setLocalZOrder()`
     @see `setVertexZ()`

     @since v3.0
     */
    virtual void setGlobalZOrder(float globalZOrder);
    /**
     * Returns the Node's Global Z Order.
     *
     * @see `setGlobalZOrder(int)`
     *
     * @return The node's global Z order
     */
    virtual float getGlobalZOrder() const { return _globalZOrder; }

    /**
     * Sets the scale (x) of the node.
     *
     * It is a scaling factor that multiplies the width of the node and its children.
     *
     * @param scaleX   The scale factor on X axis.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setScaleX(float scaleX);
    /**
     * Returns the scale factor on X axis of this node
     *
     * @see setScaleX(float)
     *
     * @return The scale factor on X axis.
     */
    virtual float getScaleX() const;


    /**
     * Sets the scale (y) of the node.
     *
     * It is a scaling factor that multiplies the height of the node and its children.
     *
     * @param scaleY   The scale factor on Y axis.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setScaleY(float scaleY);
    /**
     * Returns the scale factor on Y axis of this node
     *
     * @see `setScaleY(float)`
     *
     * @return The scale factor on Y axis.
     */
    virtual float getScaleY() const;

    /**
     * Changes the scale factor on Z axis of this node
     *
     * The Default value is 1.0 if you haven't changed it before.
     *
     * @param scaleY   The scale factor on Y axis.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setScaleZ(float scaleZ);
    /**
     * Returns the scale factor on Z axis of this node
     *
     * @see `setScaleZ(float)`
     *
     * @return The scale factor on Z axis.
     */
    virtual float getScaleZ() const;


    /**
     * Sets the scale (x,y,z) of the node.
     *
     * It is a scaling factor that multiplies the width, height and depth of the node and its children.
     *
     * @param scale     The scale factor for both X and Y axis.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setScale(float scale);
    /**
     * Gets the scale factor of the node,  when X and Y have the same scale factor.
     *
     * @warning Assert when `_scaleX != _scaleY`
     * @see setScale(float)
     *
     * @return The scale factor of the node.
     */
    virtual float getScale() const;

     /**
     * Sets the scale (x,y) of the node.
     *
     * It is a scaling factor that multiplies the width and height of the node and its children.
     *
     * @param scaleX     The scale factor on X axis.
     * @param scaleY     The scale factor on Y axis.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setScale(float scaleX, float scaleY);

    /**
     * Sets the position (x,y) of the node in its parent's coordinate system.
     *
     * Usually we use `Vec2(x,y)` to compose Vec2 object.
     * This code snippet sets the node in the center of screen.
     @code
     Size size = Director::getInstance()->getWinSize();
     node->setPosition(size.width/2, size.height/2)
     @endcode
     *
     * @param position  The position (x,y) of the node in OpenGL coordinates
     */
    virtual void setPosition(const Vec2 &position);

    /** Sets the position (x,y) using values between 0 and 1.
     The positions in pixels is calculated like the following:
     @code
     // pseudo code
     void setNormalizedPosition(Vec2 pos) {
       Size s = getParent()->getContentSize();
       _position = pos * s;
     }
     @endcode
     */
    virtual void setNormalizedPosition(const Vec2 &position);

    /**
     * Gets the position (x,y) of the node in its parent's coordinate system.
     *
     * @see setPosition(const Vec2&)
     *
     * @return The position (x,y) of the node in OpenGL coordinates
     * @code
     * In js and lua return value is table which contains x,y
     * @endcode
     */
    virtual const Vec2& getPosition() const;

    /** returns the normalized position */
    virtual const Vec2& getNormalizedPosition() const;

    /**
     * Sets the position (x,y) of the node in its parent's coordinate system.
     *
     * Passing two numbers (x,y) is much efficient than passing Vec2 object.
     * This method is bound to Lua and JavaScript.
     * Passing a number is 10 times faster than passing a object from Lua to c++
     *
     @code
     // sample code in Lua
     local pos  = node::getPosition()  -- returns Vec2 object from C++
     node:setPosition(x, y)            -- pass x, y coordinate to C++
     @endcode
     *
     * @param x     X coordinate for position
     * @param y     Y coordinate for position
     */
    virtual void setPosition(float x, float y);
    /**
     * Gets position in a more efficient way, returns two number instead of a Vec2 object
     *
     * @see `setPosition(float, float)`
     * In js,out value not return
     */
    virtual void getPosition(float* x, float* y) const;
    /**
     * Gets/Sets x or y coordinate individually for position.
     * These methods are used in Lua and Javascript Bindings
     */
    virtual void  setPositionX(float x);
    virtual float getPositionX(void) const;
    virtual void  setPositionY(float y);
    virtual float getPositionY(void) const;

    /**
     * Sets the position (X, Y, and Z) in its parent's coordinate system
     */
    virtual void setPosition3D(const Vec3& position);
    /**
     * returns the position (X,Y,Z) in its parent's coordinate system
     */
    virtual Vec3 getPosition3D() const;

    /**
     * Sets the 'z' coordinate in the position. It is the OpenGL Z vertex value.
     *
     * The OpenGL depth buffer and depth testing are disabled by default. You need to turn them on
     * in order to use this property correctly.
     *
     * `setPositionZ()` also sets the `setGlobalZValue()` with the positionZ as value.
     *
     * @see `setGlobalZValue()`
     *
     * @param vertexZ  OpenGL Z vertex of this node.
     */
    virtual void setPositionZ(float positionZ);

    /**
     * Gets position Z coordinate of this node.
     *
     * @see setPositionZ(float)
     *
     * @return the position Z coordinate of this node.
     */
    virtual float getPositionZ() const;

    /**
     * Changes the X skew angle of the node in degrees.
     *
     * The difference between `setRotationalSkew()` and `setSkew()` is that the first one simulate Flash's skew functionality
     * while the second one uses the real skew function.
     *
     * This angle describes the shear distortion in the X direction.
     * Thus, it is the angle between the Y coordinate and the left edge of the shape
     * The default skewX angle is 0. Positive values distort the node in a CW direction.
     *
     * @param skewX The X skew angle of the node in degrees.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setSkewX(float skewX);
    /**
     * Returns the X skew angle of the node in degrees.
     *
     * @see `setSkewX(float)`
     *
     * @return The X skew angle of the node in degrees.
     */
    virtual float getSkewX() const;


    /**
     * Changes the Y skew angle of the node in degrees.
     *
     * The difference between `setRotationalSkew()` and `setSkew()` is that the first one simulate Flash's skew functionality
     * while the second one uses the real skew function.
     *
     * This angle describes the shear distortion in the Y direction.
     * Thus, it is the angle between the X coordinate and the bottom edge of the shape
     * The default skewY angle is 0. Positive values distort the node in a CCW direction.
     *
     * @param skewY    The Y skew angle of the node in degrees.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setSkewY(float skewY);
    /**
     * Returns the Y skew angle of the node in degrees.
     *
     * @see `setSkewY(float)`
     *
     * @return The Y skew angle of the node in degrees.
     */
    virtual float getSkewY() const;


    /**
     * Sets the anchor point in percent.
     *
     * anchorPoint is the point around which all transformations and positioning manipulations take place.
     * It's like a pin in the node where it is "attached" to its parent.
     * The anchorPoint is normalized, like a percentage. (0,0) means the bottom-left corner and (1,1) means the top-right corner.
     * But you can use values higher than (1,1) and lower than (0,0) too.
     * The default anchorPoint is (0.5,0.5), so it starts in the center of the node.
     * @note If node has a physics body, the anchor must be in the middle, you cann't change this to other value.
     *
     * @param anchorPoint   The anchor point of node.
     */
    virtual void setAnchorPoint(const Vec2& anchorPoint);
    /**
     * Returns the anchor point in percent.
     *
     * @see `setAnchorPoint(const Vec2&)`
     *
     * @return The anchor point of node.
     */
    virtual const Vec2& getAnchorPoint() const;
    /**
     * Returns the anchorPoint in absolute pixels.
     *
     * @warning You can only read it. If you wish to modify it, use anchorPoint instead.
     * @see `getAnchorPoint()`
     *
     * @return The anchor point in absolute pixels.
     */
    virtual const Vec2& getAnchorPointInPoints() const;


    /**
     * Sets the untransformed size of the node.
     *
     * The contentSize remains the same no matter the node is scaled or rotated.
     * All nodes has a size. Layer and Scene has the same size of the screen.
     *
     * @param contentSize   The untransformed size of the node.
     */
    virtual void setContentSize(const Size& contentSize);
    /**
     * Returns the untransformed size of the node.
     *
     * @see `setContentSize(const Size&)`
     *
     * @return The untransformed size of the node.
     */
    virtual const Size& getContentSize() const;


    /**
     * Sets whether the node is visible
     *
     * The default value is true, a node is default to visible
     *
     * @param visible   true if the node is visible, false if the node is hidden.
     */
    virtual void setVisible(bool visible);
    /**
     * Determines if the node is visible
     *
     * @see `setVisible(bool)`
     *
     * @return true if the node is visible, false if the node is hidden.
     */
    virtual bool isVisible(bool checkParent = false) const;

    /**
     * Sets the rotation (angle) of the node in degrees.
     *
     * 0 is the default rotation angle.
     * Positive values rotate node clockwise, and negative values for anti-clockwise.
     *
     * @param rotation     The rotation of the node in degrees.
     */
    virtual void setRotation(float rotation);
    /**
     * Returns the rotation of the node in degrees.
     *
     * @see `setRotation(float)`
     *
     * @return The rotation of the node in degrees.
     */
    virtual float getRotation() const;

    /**
     * Sets the rotation (X,Y,Z) in degrees.
     * Useful for 3d rotations
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setRotation3D(const Vec3& rotation);
    /**
     * returns the rotation (X,Y,Z) in degrees.
     */
    virtual Vec3 getRotation3D() const;

    /**
     * Sets the X rotation (angle) of the node in degrees which performs a horizontal rotational skew.
     *
     * The difference between `setRotationalSkew()` and `setSkew()` is that the first one simulate Flash's skew functionality
     * while the second one uses the real skew function.
     *
     * 0 is the default rotation angle.
     * Positive values rotate node clockwise, and negative values for anti-clockwise.
     *
     * @param rotationX    The X rotation in degrees which performs a horizontal rotational skew.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setRotationSkewX(float rotationX);

    /**
     * Gets the X rotation (angle) of the node in degrees which performs a horizontal rotation skew.
     *
     * @see `setRotationSkewX(float)`
     *
     * @return The X rotation in degrees.
     */
    virtual float getRotationSkewX() const;

    /**
     * Sets the Y rotation (angle) of the node in degrees which performs a vertical rotational skew.
     *
     * The difference between `setRotationalSkew()` and `setSkew()` is that the first one simulate Flash's skew functionality
     * while the second one uses the real skew function.
     *
     * 0 is the default rotation angle.
     * Positive values rotate node clockwise, and negative values for anti-clockwise.
     *
     * @param rotationY    The Y rotation in degrees.
     *
     * @warning The physics body doesn't support this.
     */
    virtual void setRotationSkewY(float rotationY);

    /**
     * Gets the Y rotation (angle) of the node in degrees which performs a vertical rotational skew.
     *
     * @see `setRotationSkewY(float)`
     *
     * @return The Y rotation in degrees.
     */
    virtual float getRotationSkewY() const;

    /**
     * Sets the arrival order when this node has a same ZOrder with other children.
     *
     * A node which called addChild subsequently will take a larger arrival order,
     * If two children have the same Z order, the child with larger arrival order will be drawn later.
     *
     * @warning This method is used internally for localZOrder sorting, don't change this manually
     *
     * @param orderOfArrival   The arrival order.
     */
    void setOrderOfArrival(int orderOfArrival);
    /**
     * Returns the arrival order, indicates which children is added previously.
     *
     * @see `setOrderOfArrival(unsigned int)`
     *
     * @return The arrival order.
     */
    int getOrderOfArrival() const;
    /**
     * Sets whether the anchor point will be (0,0) when you position this node.
     *
     * This is an internal method, only used by Layer and Scene. Don't call it outside framework.
     * The default value is false, while in Layer and Scene are true
     *
     * @param ignore    true if anchor point will be (0,0) when you position this node
     * @todo This method should be renamed as setIgnoreAnchorPointForPosition(bool) or something with "set"
     */
    virtual void ignoreAnchorPointForPosition(bool ignore);
    /**
     * Gets whether the anchor point will be (0,0) when you position this node.
     *
     * @see `ignoreAnchorPointForPosition(bool)`
     *
     * @return true if the anchor point will be (0,0) when you position this node.
     */
    virtual bool isIgnoreAnchorPointForPosition() const;

    /// @}  end of Setters & Getters for Graphic Properties


    /// @{
    /// @name Children and Parent

    /**
     * Adds a child to the container with z-order as 0.
     *
     * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
     *
     * @param child A child node
     */
    virtual void addChild(Node * child);
    /**
     * Adds a child to the container with a local z-order
     *
     * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
     *
     * @param child     A child node
     * @param zOrder    Z order for drawing priority. Please refer to `setLocalZOrder(int)`
     */
    virtual void addChild(Node * child, int localZOrder);
    /**
     * Adds a child to the container with z order and tag
     *
     * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
     *
     * @param child     A child node
     * @param zOrder    Z order for drawing priority. Please refer to `setLocalZOrder(int)`
     * @param tag       An integer to identify the node easily. Please refer to `setTag(int)`
     * 
     * Please use `addChild(Node* child, int localZOrder, const std::string &name)` instead.
     */
     virtual void addChild(Node* child, int localZOrder, int tag);
    /**
     * Adds a child to the container with z order and tag
     *
     * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
     *
     * @param child     A child node
     * @param zOrder    Z order for drawing priority. Please refer to `setLocalZOrder(int)`
     * @param name      A string to identify the node easily. Please refer to `setName(int)`
     *
     */
    virtual void addChild(Node* child, int localZOrder, const std::string &name);
    /**
     * Gets a child from the container with its tag
     *
     * @param tag   An identifier to find the child node.
     *
     * @return a Node object whose tag equals to the input parameter
     *
     * Please use `getChildByName()` instead
     */
     virtual Node * getChildByTag(int tag) const;
    /**
     * Gets a child from the container with its name
     *
     * @param name   An identifier to find the child node.
     *
     * @return a Node object whose name equals to the input parameter
     *
     * @since v3.2
     */
    virtual Node* getChildByName(const std::string& name) const;
    /**
     * Gets a child from the container with its name that can be cast to Type T
     *
     * @param name   An identifier to find the child node.
     *
     * @return a Node with the given name that can be cast to Type T
    */
    template <typename T>
    inline T getChildByName(const std::string& name) const { return static_cast<T>(getChildByName(name)); }
    /** Search the children of the receiving node to perform processing for nodes which share a name.
     *
     * @param name The name to search for, supports c++11 regular expression.
     * Search syntax options:
     * `//`: Can only be placed at the begin of the search string. This indicates that it will search recursively.
     * `..`: The search should move up to the node's parent. Can only be placed at the end of string
     * `/` : When placed anywhere but the start of the search string, this indicates that the search should move to the node's children.
     *
     * @code
     * enumerateChildren("//MyName", ...): This searches the children recursively and matches any node with the name `MyName`.
     * enumerateChildren("[[:alnum:]]+", ...): This search string matches every node of its children.
     * enumerateChildren("A[[:digit:]]", ...): This searches the node's children and returns any child named `A0`, `A1`, ..., `A9`
     * enumerateChildren("Abby/Normal", ...): This searches the node's grandchildren and returns any node whose name is `Normal`
     * and whose parent is named `Abby`.
     * enumerateChildren("//Abby/Normal", ...): This searches recursively and returns any node whose name is `Normal` and whose
     * parent is named `Abby`.
     * @endcode
     *
     * @warning Only support alpha or number for name, and not support unicode
     *
     * @param callback A callback function to execute on nodes that match the `name` parameter. The function takes the following arguments:
     *  `node` 
     *      A node that matches the name
     *  And returns a boolean result. Your callback can return `true` to terminate the enumeration.
     *
     * @since v3.2
     */
    virtual void enumerateChildren(const std::string &name, std::function<bool(Node* node)> callback) const;
    /**
     * Returns the array of the node's children
     *
     * @return the array the node's children
     */
    virtual Vector<Node*>& getChildren();
    virtual const Vector<Node*>& getChildren() const { return _children; }
    
    /** 
     * Returns the amount of children
     *
     * @return The amount of children.
     */
    virtual ssize_t getChildrenCount() const;

    /**
     * Sets the parent node
     *
     * @param parent    A pointer to the parent node
     */
    virtual void setParent(Node* parent);
    /**
     * Returns a pointer to the parent node
     *
     * @see `setParent(Node*)`
     *
     * @returns A pointer to the parent node
     */
    virtual Node* getParent() { return _parent; }
    virtual const Node* getParent() const { return _parent; }


    ////// REMOVES //////

    /**
     * Removes this node itself from its parent node with a cleanup.
     * If the node orphan, then nothing happens.
     * @see `removeFromParentAndCleanup(bool)`
     */
    virtual void removeFromParent();
    /**
     * Removes this node itself from its parent node.
     * If the node orphan, then nothing happens.
     * @param cleanup   true if all actions and callbacks on this node should be removed, false otherwise.
     * @js removeFromParent
     * @lua removeFromParent
     */
    virtual void removeFromParentAndCleanup(bool cleanup);

    /**
     * Removes a child from the container. It will also cleanup all running actions depending on the cleanup parameter.
     *
     * @param child     The child node which will be removed.
     * @param cleanup   true if all running actions and callbacks on the child node will be cleanup, false otherwise.
     */
    virtual void removeChild(Node* child, bool cleanup = true);

    /**
     * Removes a child from the container by tag value. It will also cleanup all running actions depending on the cleanup parameter
     *
     * @param tag       An interger number that identifies a child node
     * @param cleanup   true if all running actions and callbacks on the child node will be cleanup, false otherwise.
     *
     * Please use `removeChildByName` instead.
     */
     virtual void removeChildByTag(int tag, bool cleanup = true);
    /**
     * Removes a child from the container by tag value. It will also cleanup all running actions depending on the cleanup parameter
     *
     * @param name       A string that identifies a child node
     * @param cleanup   true if all running actions and callbacks on the child node will be cleanup, false otherwise.
     */
    virtual void removeChildByName(const std::string &name, bool cleanup = true);
    /**
     * Removes all children from the container with a cleanup.
     *
     * @see `removeAllChildrenWithCleanup(bool)`
     */
    virtual void removeAllChildren();
    /**
     * Removes all children from the container, and do a cleanup to all running actions depending on the cleanup parameter.
     *
     * @param cleanup   true if all running actions on all children nodes should be cleanup, false oterwise.
     * @js removeAllChildren
     * @lua removeAllChildren
     */
    virtual void removeAllChildrenWithCleanup(bool cleanup);

    /**
     * Reorders a child according to a new z value.
     *
     * @param child     An already added child node. It MUST be already added.
     * @param localZOrder Z order for drawing priority. Please refer to setLocalZOrder(int)
     */
    virtual void reorderChild(Node * child, int localZOrder);

    /**
     * Sorts the children array once before drawing, instead of every time when a child is added or reordered.
     * This appraoch can improves the performance massively.
     * @note Don't call this manually unless a child added needs to be removed in the same frame
     */
    virtual void sortAllChildren();

    /// @} end of Children and Parent
    
    /// @{
    /// @name Tag & User data

    /**
     * Returns a tag that is used to identify the node easily.
     *
     * @return An integer that identifies the node.
     *
     * Please use `getTag()` instead.
     */
     virtual int getTag() const;
    /**
     * Changes the tag that is used to identify the node easily.
     *
     * Please refer to getTag for the sample code.
     *
     * @param tag   A integer that identifies the node.
     *
     * Please use `setName()` instead.
     */
     virtual void setTag(int tag);
    
    /** Returns a string that is used to identify the node.
     * @return A string that identifies the node.
     * 
     * @since v3.2
     */
    virtual std::string getName() const;
    /** Changes the name that is used to identify the node easily.
     * @param name A string that identifies the node.
     *
     * @since v3.2
     */
    virtual void setName(const std::string& name);

    
    /**
     * Returns a custom user data pointer
     *
     * You can set everything in UserData pointer, a data block, a structure or an object.
     *
     * @return A custom user data pointer
     * @js NA
     * @lua NA
     */
    virtual void* getUserData() { return _userData; }
    /**
    * @js NA
    * @lua NA
    */
    virtual const void* getUserData() const { return _userData; }

    /**
     * Sets a custom user data pointer
     *
     * You can set everything in UserData pointer, a data block, a structure or an object, etc.
     * @warning Don't forget to release the memory manually,
     *          especially before you change this data pointer, and before this node is autoreleased.
     *
     * @param userData  A custom user data pointer
     * @js NA
     * @lua NA
     */
    virtual void setUserData(void *userData);

    /**
     * Returns a user assigned Object
     *
     * Similar to userData, but instead of holding a void* it holds an object
     *
     * @return A user assigned Object
     * @js NA
     * @lua NA
     */
    virtual Ref* getUserObject() { return _userObject; }
    /**
    * @js NA
    * @lua NA
    */
    virtual const Ref* getUserObject() const { return _userObject; }

    /**
     * Returns a user assigned Object
     *
     * Similar to UserData, but instead of holding a void* it holds an object.
     * The UserObject will be retained once in this method,
     * and the previous UserObject (if existed) will be released.
     * The UserObject will be released in Node's destructor.
     *
     * @param userObject    A user assigned Object
     */
    virtual void setUserObject(Ref *userObject);

    /// @} end of Tag & User Data


    /// @{
    /// @name GLProgram
    /**
     * Return the GLProgram (shader) currently used for this node
     *
     * @return The GLProgram (shader) currently used for this node
     */
    GLProgram* getGLProgram() const;

    GLProgramState *getGLProgramState() const;
    virtual void setGLProgramState(GLProgramState *glProgramState);

    /**
     * Sets the shader program for this node
     *
     * Since v2.0, each rendering node must set its shader program.
     * It should be set in initialize phase.
     @code
     node->setGLrProgram(GLProgramCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
     @endcode
     *
     * @param shaderProgram The shader program
     */
    virtual void setGLProgram(GLProgram *glprogram);
    /// @} end of Shader Program


    /**
     * Returns whether or not the node is "running".
     *
     * If the node is running it will accept event callbacks like onEnter(), onExit(), update()
     *
     * @return Whether or not the node is running.
     */
    virtual bool isRunning() const;

    /**
     * Schedules for lua script.
     * @js NA
     */
    void scheduleUpdateWithPriorityLua(int handler, int priority);

    /// @}  end Script Bindings


    /// @{
    /// @name Event Callbacks

    /**
     * Event callback that is invoked every time when Node enters the 'stage'.
     * If the Node enters the 'stage' with a transition, this event is called when the transition starts.
     * During onEnter you can't access a "sister/brother" node.
     * If you override onEnter, you shall call its parent's one, e.g., Node::onEnter().
     * @js NA
     * @lua NA
     */
    virtual void onEnter();

    /** Event callback that is invoked when the Node enters in the 'stage'.
     * If the Node enters the 'stage' with a transition, this event is called when the transition finishes.
     * If you override onEnterTransitionDidFinish, you shall call its parent's one, e.g. Node::onEnterTransitionDidFinish()
     * @js NA
     * @lua NA
     */
    virtual void onEnterTransitionDidFinish();

    /**
     * Event callback that is invoked every time the Node leaves the 'stage'.
     * If the Node leaves the 'stage' with a transition, this event is called when the transition finishes.
     * During onExit you can't access a sibling node.
     * If you override onExit, you shall call its parent's one, e.g., Node::onExit().
     * @js NA
     * @lua NA
     */
    virtual void onExit();

    /**
     * Event callback that is called every time the Node leaves the 'stage'.
     * If the Node leaves the 'stage' with a transition, this callback is called when the transition starts.
     * @js NA
     * @lua NA
     */
    virtual void onExitTransitionDidStart();

    /// @} end of event callbacks.


    /**
     * Stops all running actions and schedulers
     */
    virtual void cleanup();

    /**
     * Override this method to draw your own node.
     * The following GL states will be enabled by default:
     * - `glEnableClientState(GL_VERTEX_ARRAY);`
     * - `glEnableClientState(GL_COLOR_ARRAY);`
     * - `glEnableClientState(GL_TEXTURE_COORD_ARRAY);`
     * - `glEnable(GL_TEXTURE_2D);`
     * AND YOU SHOULD NOT DISABLE THEM AFTER DRAWING YOUR NODE
     * But if you enable any other GL state, you should disable it after drawing your node.
     */
    virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
    virtual void draw() final;

    /**
     * Visits this node's children and draw them recursively.
     */
    virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
    virtual void visit() final;


    /** Returns the Scene that contains the Node.
     It returns `nullptr` if the node doesn't belong to any Scene.
     This function recursively calls parent->getScene() until parent is a Scene object. The results are not cached. It is that the user caches the results in case this functions is being used inside a loop.
     */
    virtual Scene* getScene() const;

    /**
     * Returns an AABB (axis-aligned bounding-box) in its parent's coordinate system.
     *
     * @return An AABB (axis-aligned bounding-box) in its parent's coordinate system
     */
    virtual Rect getBoundingBox() const;

    virtual void setEventDispatcher(EventDispatcher* dispatcher);
    virtual EventDispatcher* getEventDispatcher() const { return _eventDispatcher; };

    /// @{
    /// @name Actions

    /**
     * Sets the ActionManager object that is used by all actions.
     *
     * @warning If you set a new ActionManager, then previously created actions will be removed.
     *
     * @param actionManager     A ActionManager object that is used by all actions.
     */
    virtual void setActionManager(ActionManager* actionManager);
    /**
     * Gets the ActionManager object that is used by all actions.
     * @see setActionManager(ActionManager*)
     * @return A ActionManager object.
     */
    virtual ActionManager* getActionManager() { return _actionManager; }
    virtual const ActionManager* getActionManager() const { return _actionManager; }

    /**
     * Executes an action, and returns the action that is executed.
     *
     * This node becomes the action's target. Refer to Action::getTarget()
     * @warning Actions don't retain their target.
     *
     * @return An Action pointer
     */
    Action* runAction(Action* action);

    /**
     * Stops and removes all actions from the running action list .
     */
    void stopAllActions();

    /**
     * Stops and removes an action from the running action list.
     *
     * @param action    The action object to be removed.
     */
    void stopAction(Action* action);

    /**
     * Removes an action from the running action list by its tag.
     *
     * @param tag   A tag that indicates the action to be removed.
     */
    void stopActionByTag(int tag);
    
    /**
     * Removes all actions from the running action list by its tag.
     *
     * @param tag   A tag that indicates the action to be removed.
     */
    void stopAllActionsByTag(int tag);

    /**
     * Gets an action from the running action list by its tag.
     *
     * @see `setTag(int)`, `getTag()`.
     *
     * @return The action object with the given tag.
     */
    Action* getActionByTag(int tag);

    /**
     * Returns the numbers of actions that are running plus the ones that are schedule to run (actions in actionsToAdd and actions arrays).
     *
     * Composable actions are counted as 1 action. Example:
     *    If you are running 1 Sequence of 7 actions, it will return 1.
     *    If you are running 7 Sequences of 2 actions, it will return 7.
     * @todo Rename to getNumberOfRunningActions()
     *
     * @return The number of actions that are running plus the ones that are schedule to run
     */
    ssize_t getNumberOfRunningActions() const;
    /// @} end of Actions


    /// @{
    /// @name Scheduler and Timer

    /**
     * Sets a Scheduler object that is used to schedule all "updates" and timers.
     *
     * @warning If you set a new Scheduler, then previously created timers/update are going to be removed.
     * @param scheduler     A Shdeduler object that is used to schedule all "update" and timers.
     */
    virtual void setScheduler(Scheduler* scheduler);
    /**
     * Gets a Sheduler object.
     *
     * @see setScheduler(Scheduler*)
     * @return A Scheduler object.
     */
    virtual Scheduler* getScheduler() { return _scheduler; }
    virtual const Scheduler* getScheduler() const { return _scheduler; }


    /**
     * Checks whether a selector is scheduled.
     *
     * @param selector      A function selector
     * @return Whether the funcion selector is scheduled.
     * @js NA
     * @lua NA
     */
    bool isScheduled(SEL_SCHEDULE selector);

    /**
     * Checks whether a lambda function is scheduled.
     *
     * @param key      key of the callback
     * @return Whether the lambda function selector is scheduled.
     * @js NA
     * @lua NA
     */
    bool isScheduled(const std::string &key);

    /**
     * Schedules the "update" method.
     *
     * It will use the order number 0. This method will be called every frame.
     * Scheduled methods with a lower order value will be called before the ones that have a higher order value.
     * Only one "update" method could be scheduled per node.
     * @js NA
     * @lua NA
     */
    void scheduleUpdate(void);

    /**
     * Schedules the "update" method with a custom priority.
     *
     * This selector will be called every frame.
     * Scheduled methods with a lower priority will be called before the ones that have a higher value.
     * Only one "update" selector could be scheduled per node (You can't have 2 'update' selectors).
     * @js NA
     * @lua NA
     */
    void scheduleUpdateWithPriority(int priority);

    /*
     * Unschedules the "update" method.
     * @see scheduleUpdate();
     */
    void unscheduleUpdate(void);

    /**
     * Schedules a custom selector.
     *
     * If the selector is already scheduled, then the interval parameter will be updated without scheduling it again.
     @code
     // firstly, implement a schedule function
     void MyNode::TickMe(float dt);
     // wrap this function into a selector via schedule_selector marco.
     this->schedule(CC_SCHEDULE_SELECTOR(MyNode::TickMe), 0, 0, 0);
     @endcode
     *
     * @param selector  The SEL_SCHEDULE selector to be scheduled.
     * @param interval  Tick interval in seconds. 0 means tick every frame. If interval = 0, it's recommended to use scheduleUpdate() instead.
     * @param repeat    The selector will be excuted (repeat + 1) times, you can use CC_REPEAT_FOREVER for tick infinitely.
     * @param delay     The amount of time that the first tick will wait before execution.
     * @lua NA
     */
    void schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay);

    /**
     * Schedules a custom selector with an interval time in seconds.
     * @see `schedule(SEL_SCHEDULE, float, unsigned int, float)`
     *
     * @param selector      The SEL_SCHEDULE selector to be scheduled.
     * @param interval      Callback interval time in seconds. 0 means tick every frame,
     * @lua NA
     */
    void schedule(SEL_SCHEDULE selector, float interval);

    /**
     * Schedules a selector that runs only once, with a delay of 0 or larger
     * @see `schedule(SEL_SCHEDULE, float, unsigned int, float)`
     *
     * @param selector      The SEL_SCHEDULE selector to be scheduled.
     * @param delay         The amount of time that the first tick will wait before execution.
     * @lua NA
     */
    void scheduleOnce(SEL_SCHEDULE selector, float delay);

    /**
     * Schedules a lambda function that runs only once, with a delay of 0 or larger
     *
     * @param callback      The lambda function to be scheduled
     * @param delay         The amount of time that the first tick will wait before execution.
     * @param key           The key of the lambda function. To be used if you want to unschedule it
     * @lua NA
     */
    void scheduleOnce(const std::function<void(float)>& callback, float delay, const std::string &key);

    /**
     * Schedules a custom selector, the scheduled selector will be ticked every frame
     * @see schedule(SEL_SCHEDULE, float, unsigned int, float)
     *
     * @param selector      A function wrapped as a selector
     * @lua NA
     */
    void schedule(SEL_SCHEDULE selector);

    /**
     * Schedules a lambda function. The scheduled lambda function will be called every frame
     *
     * @param callback      The lambda function to be scheduled
     * @param key           The key of the lambda function. To be used if you want to unschedule it
     * @lua NA
     */
    void schedule(const std::function<void(float)>& callback, const std::string &key);

    /**
     * Schedules a lambda function. The scheduled lambda function will be called every "interval" seconds
     *
     * @param callback      The lambda function to be scheduled
     * @param interval      Callback interval time in seconds. 0 means every frame,
     * @param key           The key of the lambda function. To be used if you want to unschedule it
     * @lua NA
     */
    void schedule(const std::function<void(float)>& callback, float interval, const std::string &key);

    /**
     * Schedules a lambda function.
     *
     * @param callback  The lambda function to be schedule
     * @param interval  Tick interval in seconds. 0 means tick every frame.
     * @param repeat    The selector will be executed (repeat + 1) times, you can use CC_REPEAT_FOREVER for tick infinitely.
     * @param delay     The amount of time that the first tick will wait before execution.
     * @param key       The key of the lambda function. To be used if you want to unschedule it
     * @lua NA
     */
    void schedule(const std::function<void(float)>& callback, float interval, unsigned int repeat, float delay, const std::string &key);

    /**
     * Unschedules a custom selector.
     * @see `schedule(SEL_SCHEDULE, float, unsigned int, float)`
     *
     * @param selector      A function wrapped as a selector
     * @lua NA
     */
    void unschedule(SEL_SCHEDULE selector);

    /**
     * Unschedules a lambda function
     *
     * @param key      The key of the lambda function to be unscheduled
     * @lua NA
     */
    void unschedule(const std::string &key);

    /**
     * Unschedule all scheduled selectors and lambda functions: custom selectors, and the 'update' selector and lambda functions
     * Actions are not affected by this method.
     * @lua NA
     */
    void unscheduleAllCallbacks();

    /**
     * Resumes all scheduled selectors, actions and event listeners.
     * This method is called internally by onEnter
     */
    virtual void resume(void);
    /**
     * Pauses all scheduled selectors, actions and event listeners..
     * This method is called internally by onExit
     */
    virtual void pause(void);

    /*
     * Update method will be called automatically every frame if "scheduleUpdate" is called, and the node is "live"
     */
    virtual void update(float delta);

    /// @} end of Scheduler and Timer

    /// @{
    /// @name Transformations

    /**
     * Calls children's updateTransform() method recursively.
     *
     * This method is moved from Sprite, so it's no longer specific to Sprite.
     * As the result, you apply SpriteBatchNode's optimization on your customed Node.
     * e.g., `batchNode->addChild(myCustomNode)`, while you can only addChild(sprite) before.
     */
    virtual void updateTransform();

    /**
     * Returns the matrix that transform the node's (local) space coordinates into the parent's space coordinates.
     * The matrix is in Pixels.
     */
    virtual const Mat4& getNodeToParentTransform() const;
    virtual AffineTransform getNodeToParentAffineTransform() const;

    /** 
     * Sets the Transformation matrix manually.
     */
    virtual void setNodeToParentTransform(const Mat4& transform);

    /**
     * Returns the matrix that transform parent's space coordinates to the node's (local) space coordinates.
     * The matrix is in Pixels.
     */
    virtual const Mat4& getParentToNodeTransform() const;
    virtual AffineTransform getParentToNodeAffineTransform() const;

    /**
     * Returns the world affine transform matrix. The matrix is in Pixels.
     */
    virtual Mat4 getNodeToWorldTransform() const;
    virtual AffineTransform getNodeToWorldAffineTransform() const;

    /**
     * Returns the inverse world affine transform matrix. The matrix is in Pixels.
     */
    virtual Mat4 getWorldToNodeTransform() const;
    virtual AffineTransform getWorldToNodeAffineTransform() const;

    /// @} end of Transformations


    /// @{
    /// @name Coordinate Converters

    /**
     * Converts a Vec2 to node (local) space coordinates. The result is in Points.
     */
    Vec2 convertToNodeSpace(const Vec2& worldPoint) const;

    /**
     * Converts a Vec2 to world space coordinates. The result is in Points.
     */
    Vec2 convertToWorldSpace(const Vec2& nodePoint) const;

    /**
     * Converts a Vec2 to node (local) space coordinates. The result is in Points.
     * treating the returned/received node point as anchor relative.
     */
    Vec2 convertToNodeSpaceAR(const Vec2& worldPoint) const;

    /**
     * Converts a local Vec2 to world space coordinates.The result is in Points.
     * treating the returned/received node point as anchor relative.
     */
    Vec2 convertToWorldSpaceAR(const Vec2& nodePoint) const;

    /**
     * convenience methods which take a Touch instead of Vec2
     */
    Vec2 convertTouchToNodeSpace(Touch * touch) const;

    /**
     * converts a Touch (world coordinates) into a local coordinate. This method is AR (Anchor Relative).
     */
    Vec2 convertTouchToNodeSpaceAR(Touch * touch) const;

	/**
     *  Sets an additional transform matrix to the node.
     *
     *  In order to remove it, call it again with the argument `nullptr`
     *
     *  @note The additional transform will be concatenated at the end of getNodeToParentTransform.
     *        It could be used to simulate `parent-child` relationship between two nodes (e.g. one is in BatchNode, another isn't).
     */
    void setAdditionalTransform(Mat4* additionalTransform);
    void setAdditionalTransform(const AffineTransform& additionalTransform);

    /// @} end of Coordinate Converters

      /// @{
    /// @name component functions
    /**
     *   gets a component by its name
     */
    Component* getComponent(const std::string& name);

    /**
     *   adds a component
     */
    virtual bool addComponent(Component *component);

    /**
     *   removes a component by its name
     */
    virtual bool removeComponent(const std::string& name);

    /** 
     *   removes a component by its pointer      
     */
    virtual bool removeComponent(Component *component);
    /**
     *   removes all components
     */
    virtual void removeAllComponents();
    /// @} end of component functions
    
    // overrides
    virtual GLubyte getOpacity() const;
    virtual GLubyte getDisplayedOpacity() const;
    virtual void setOpacity(GLubyte opacity);
    virtual void updateDisplayedOpacity(GLubyte parentOpacity);
    virtual bool isCascadeOpacityEnabled() const;
    virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
    
    virtual const Color3B& getColor() const;
    virtual const Color3B& getDisplayedColor() const;
    virtual void setColor(const Color3B& color);
    virtual void updateDisplayedColor(const Color3B& parentColor);
    virtual bool isCascadeColorEnabled() const;
    virtual void setCascadeColorEnabled(bool cascadeColorEnabled);
    
    virtual void setOpacityModifyRGB(bool value) {CC_UNUSED_PARAM(value);}
    virtual bool isOpacityModifyRGB() const { return false; };

    void setOnEnterCallback(const std::function<void()>& callback) { _onEnterCallback = callback; }
    const std::function<void()>& getOnEnterCallback() const { return _onEnterCallback; }   
    void setOnExitCallback(const std::function<void()>& callback) { _onExitCallback = callback; }
    const std::function<void()>& getOnExitCallback() const { return _onExitCallback; }   
    void setonEnterTransitionDidFinishCallback(const std::function<void()>& callback) { _onEnterTransitionDidFinishCallback = callback; }
    const std::function<void()>& getonEnterTransitionDidFinishCallback() const { return _onEnterTransitionDidFinishCallback; }   
    void setonExitTransitionDidStartCallback(const std::function<void()>& callback) { _onExitTransitionDidStartCallback = callback; }
    const std::function<void()>& getonExitTransitionDidStartCallback() const { return _onExitTransitionDidStartCallback; }
    
    /** get & set camera mask, the node is visible by the camera whose camera flag & node's camera mask is true */
    unsigned short getCameraMask() const { return _cameraMask; }
    virtual void setCameraMask(unsigned short mask, bool applyChildren = true);

CC_CONSTRUCTOR_ACCESS:
    // Nodes should be created using create();
    Node();
    virtual ~Node();

    virtual bool init();

protected:
    /// lazy allocs
    void childrenAlloc(void);
    
    /// helper that reorder a child
    void insertChild(Node* child, int z);

    /// Removes a child, call child->onExit(), do cleanup, remove it from children array.
    void detachChild(Node *child, ssize_t index, bool doCleanup);

    /// Convert cocos2d coordinates to UI windows coordinate.
    Vec2 convertToWindowSpace(const Vec2& nodePoint) const;

    Mat4 transform(const Mat4 &parentTransform);
    uint32_t processParentFlags(const Mat4& parentTransform, uint32_t parentFlags);

    virtual void updateCascadeOpacity();
    virtual void disableCascadeOpacity();
    virtual void updateCascadeColor();
    virtual void disableCascadeColor();
    virtual void updateColor() {}
    
    bool doEnumerate(std::string name, std::function<bool (Node *)> callback) const;
    bool doEnumerateRecursive(const Node* node, const std::string &name, std::function<bool (Node *)> callback) const;
    
    //check whether this camera mask is visible by the current visiting camera
    bool isVisitableByVisitingCamera() const;
    
private:
    void addChildHelper(Node* child, int localZOrder, int tag, const std::string &name, bool setTag);
    
protected:

    float _rotationX;               ///< rotation on the X-axis
    float _rotationY;               ///< rotation on the Y-axis

    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    float _rotationZ_X;             ///< rotation angle on Z-axis, component X
    float _rotationZ_Y;             ///< rotation angle on Z-axis, component Y

    float _scaleX;                  ///< scaling factor on x-axis
    float _scaleY;                  ///< scaling factor on y-axis
    float _scaleZ;                  ///< scaling factor on z-axis

    Vec2 _position;                ///< position of the node
    float _positionZ;               ///< OpenGL real Z position
    Vec2 _normalizedPosition;
    bool _usingNormalizedPosition;
    bool _normalizedPositionDirty;

    float _skewX;                   ///< skew angle on x-axis
    float _skewY;                   ///< skew angle on y-axis

    Vec2 _anchorPointInPoints;     ///< anchor point in points
    Vec2 _anchorPoint;             ///< anchor point normalized (NOT in points)

    Size _contentSize;              ///< untransformed size of the node
    bool _contentSizeDirty;         ///< whether or not the contentSize is dirty

    Mat4 _modelViewTransform;    ///< ModelView transform of the Node.

    // "cache" variables are allowed to be mutable
    mutable Mat4 _transform;      ///< transform
    mutable bool _transformDirty;   ///< transform dirty flag
    mutable Mat4 _inverse;        ///< inverse transform
    mutable bool _inverseDirty;     ///< inverse transform dirty flag
    mutable Mat4 _additionalTransform; ///< transform
    bool _useAdditionalTransform;   ///< The flag to check whether the additional transform is dirty
    bool _transformUpdated;         ///< Whether or not the Transform object was updated since the last frame

    int _localZOrder;               ///< Local order (relative to its siblings) used to sort the node
    float _globalZOrder;            ///< Global order used to sort the node

    Vector<Node*> _children;        ///< array of children nodes
    Node *_parent;                  ///< weak reference to parent node

    int _tag;                         ///< a tag. Can be any number you assigned just to identify this node
    
    std::string _name;               ///<a string label, an user defined string to identify this node
    size_t _hashOfName;            ///<hash value of _name, used for speed in getChildByName

    void *_userData;                ///< A user assingned void pointer, Can be point to any cpp object
    Ref *_userObject;               ///< A user assigned Object

    GLProgramState *_glProgramState; ///< OpenGL Program State

    int _orderOfArrival;            ///< used to preserve sequence while sorting children with the same localZOrder

    Scheduler *_scheduler;          ///< scheduler used to schedule timers and updates

    ActionManager *_actionManager;  ///< a pointer to ActionManager singleton, which is used to handle all the actions

    EventDispatcher* _eventDispatcher;  ///< event dispatcher used to dispatch all kinds of events

    bool _running;                  ///< is running

    bool _visible;                  ///< is this node visible

    bool _ignoreAnchorPointForPosition; ///< true if the Anchor Vec2 will be (0,0) when you position the Node, false otherwise.
                                          ///< Used by Layer and Scene.

    bool _reorderChildDirty;          ///< children order dirty flag
    bool _isTransitionFinished;       ///< flag to indicate whether the transition was finished

#if CC_ENABLE_SCRIPT_BINDING
    int _scriptHandler;               ///< script handler for onEnter() & onExit(), used in Javascript binding and Lua binding.
    int _updateScriptHandler;         ///< script handler for update() callback per frame, which is invoked from lua & javascript.
    ccScriptType _scriptType;         ///< type of script binding, lua or javascript
#endif
    
    ComponentContainer *_componentContainer;        ///< Dictionary of components
    
    // opacity controls
    GLubyte		_displayedOpacity;
    GLubyte     _realOpacity;
    Color3B	    _displayedColor;
    Color3B     _realColor;
    bool		_cascadeColorEnabled;
    bool        _cascadeOpacityEnabled;

    static int s_globalOrderOfArrival;
    
    // camera mask, it is visible only when _cameraMask & current camera' camera flag is true
    unsigned short _cameraMask;
    
    std::function<void()> _onEnterCallback;
    std::function<void()> _onExitCallback;
    std::function<void()> _onEnterTransitionDidFinishCallback;
    std::function<void()> _onExitTransitionDidStartCallback;

	//Physics:remaining backwardly compatible  
#if CC_USE_PHYSICS
	PhysicsBody* _physicsBody;
public:
	void setPhysicsBody(PhysicsBody* physicsBody)
	{
		if (_physicsBody != nullptr)
		{
			removeComponent(_physicsBody);
		}

		addComponent(physicsBody);
	}
	PhysicsBody* getPhysicsBody() const { return _physicsBody; }

	friend class PhysicsBody;
#endif

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Node);
};

// end of base_node group
/// @}

NS_CC_END

#endif // __CCNODE_H__
