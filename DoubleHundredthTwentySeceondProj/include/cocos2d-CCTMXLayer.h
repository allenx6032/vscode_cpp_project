/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#ifndef __CCTMX_LAYER_H__
#define __CCTMX_LAYER_H__

#include "cocos2d-CCSpriteBatchNode.h"
#include "cocos2d-CCTMXXMLParser.h"
#include "cocos2d-CCCArray.h"
NS_CC_BEGIN

class TMXMapInfo;
class TMXLayerInfo;
class TMXTilesetInfo;
struct _ccCArray;

/**
 * @addtogroup _2d
 * @{
 */

/** @brief TMXLayer represents the TMX layer.
 * It is a subclass of SpriteBatchNode. By default the tiles are rendered using a TextureAtlas.
 * If you modify a tile on runtime, then, that tile will become a Sprite, otherwise no Sprite objects are created.
 * The benefits of using Sprite objects as tiles are:
 * - tiles (Sprite) can be rotated/scaled/moved with a nice API.
 * If the layer contains a property named "cc_vertexz" with an integer (in can be positive or negative),
 * then all the tiles belonging to the layer will use that value as their OpenGL vertex Z for depth.
 * On the other hand, if the "cc_vertexz" property has the "automatic" value, then the tiles will use an automatic vertex Z value.
 * Also before drawing the tiles, GL_ALPHA_TEST will be enabled, and disabled after drawing them. The used alpha func will be:
 * glAlphaFunc( GL_GREATER, value ).
 * "value" by default is 0, but you can change it from Tiled by adding the "cc_alpha_func" property to the layer.
 * The value 0 should work for most cases, but if you have tiles that are semi-transparent, then you might want to use a different
 * value, like 0.5.
 * For further information, please see the programming guide:
 * http://www.cocos2d-iphone.org/wiki/doku.php/prog_guide:tiled_maps
 * @since v0.8.1
 * Tiles can have tile flags for additional properties. At the moment only flip horizontal and flip vertical are used. These bit flags are defined in TMXXMLParser.h.
 * @since 1.1
 */

class CC_DLL TMXLayer : public SpriteBatchNode
{
public:
    /** Creates a TMXLayer with an tileset info, a layer info and a map info.
     *
     * @param tilesetInfo An tileset info.
     * @param layerInfo A layer info.
     * @param mapInfo A map info.
     * @return An autorelease object.
     */
    static TMXLayer * create(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo);
    /**
     * @js ctor
     */
    TMXLayer();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXLayer();

    /** Initializes a TMXLayer with a tileset info, a layer info and a map info.
     *
     * @param tilesetInfo An tileset info.
     * @param layerInfo A layer info.
     * @param mapInfo A map info.
     * @return If initializes successfully, it will return true.
     */
    bool initWithTilesetInfo(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo);

    /** Dealloc the map that contains the tile position from memory.
     * Unless you want to know at runtime the tiles positions, you can safely call this method.
     * If you are going to call layer->tileGIDAt() then, don't release the map.
     */
    void releaseMap();

    /** Returns the tile (Sprite) at a given a tile coordinate.
     * The returned Sprite will be already added to the TMXLayer. Don't add it again.
     * The Sprite can be treated like any other Sprite: rotated, scaled, translated, opacity, color, etc.
     * You can remove either by calling:
     * - layer->removeChild(sprite, cleanup);
     * - or layer->removeTileAt(Vec2(x,y));
     *
     * @param tileCoordinate A tile coordinate.
     * @return Returns the tile (Sprite) at a given a tile coordinate.
     */
    Sprite* getTileAt(const Vec2& tileCoordinate);
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE Sprite* tileAt(const Vec2& tileCoordinate) { return getTileAt(tileCoordinate); };
    
    /** Returns the tile gid at a given tile coordinate. It also returns the tile flags.
     * This method requires the tile map has not been previously released (eg. don't call [layer releaseMap]).
     * 
     * @param tileCoordinate The tile coordinate.
     * @param flags Tile flags.
     * @return Returns the tile gid at a given tile coordinate. It also returns the tile flags.
     */
	uint32_t getTileGIDAt(const Vec2& tileCoordinate, TMXTileFlags* flags = nullptr);
    /**
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE uint32_t tileGIDAt(const Vec2& tileCoordinate, TMXTileFlags* flags = nullptr){
        return getTileGIDAt(tileCoordinate, flags);
    }

    /** Sets the tile gid (gid = tile global id) at a given tile coordinate.
     * The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
     * If a tile is already placed at that position, then it will be removed.
     *
     * @param gid The tile gid.
     * @param tileCoordinate The tile coordinate.
     */
    void setTileGID(uint32_t gid, const Vec2& tileCoordinate);

    /** Sets the tile gid (gid = tile global id) at a given tile coordinate.
     * The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
     * If a tile is already placed at that position, then it will be removed.
     * Use withFlags if the tile flags need to be changed as well.
     * 
     * @param gid The tile gid.
     * @param tileCoordinate The tile coordinate.
     * @param flags The tile flags.
     */

    void setTileGID(uint32_t gid, const Vec2& tileCoordinate, TMXTileFlags flags);

    /** Removes a tile at given tile coordinate. 
     *
     * @param tileCoordinate The tile coordinate.
     */
    void removeTileAt(const Vec2& tileCoordinate);

    /** Returns the position in points of a given tile coordinate.
     *
     * @param tileCoordinate The tile coordinate.
     * @return The position in points of a given tile coordinate.
     */
	Vec2 getPositionAt(const Vec2& tileCoordinate);
    /**
    * @js NA
    */
    CC_DEPRECATED_ATTRIBUTE Vec2 positionAt(const Vec2& tileCoordinate) { return getPositionAt(tileCoordinate); };

    /** Return the value for the specific property name.
     *
     * @param propertyName The specific property name.
     * @return Return the value for the specific property name.
     */
	Value getProperty(const std::string& propertyName) const;
    /**
    * @js NA
    */
    CC_DEPRECATED_ATTRIBUTE Value propertyNamed(const std::string& propertyName) const { return getProperty(propertyName); };

    /** Creates the tiles. */
    void setupTiles();
    
    /** Get the layer name. 
     *
     * @return The layer name.
     */
    const std::string& getLayerName() { return _layerName; }
    
    /** Set the layer name.
     *
     * @param layerName The layer name.
     */
    void setLayerName(const std::string& layerName) { _layerName = layerName; }

    /** Size of the layer in tiles.
     *
     * @return Size of the layer in tiles.
     */
    const Size& getLayerSize() const { return _layerSize; }
    
    /** Set size of the layer in tiles.
     *
     * @param size Size of the layer in tiles.
     */
    void setLayerSize(const Size& size) { _layerSize = size; }
    
    /** Size of the map's tile (could be different from the tile's size).
     *
     * @return The size of the map's tile.
     */
    const Size& getMapTileSize() const { return _mapTileSize; }
    
    /** Set the size of the map's tile.
     *
     * @param size The size of the map's tile.
     */
    void setMapTileSize(const Size& size) { _mapTileSize = size; }
    
    /** Pointer to the map of tiles.
     * @js NA
     * @lua NA
     * @return Pointer to the map of tiles.
     */
    uint32_t* getTiles() const { return _tiles; };
    
    /** Set a pointer to the map of tiles.
     *
     * @param tiles A pointer to the map of tiles.
     */
    void setTiles(uint32_t* tiles) { _tiles = tiles; };
    
    /** Tileset information for the layer. 
     *
     * @return Tileset information for the layer.
     */
    TMXTilesetInfo* getTileSet() const { return _tileSet; }
    
    /** Set tileset information for the layer.
     *
     * @param info The tileset information for the layer.
     * @js NA
     */
    void setTileSet(TMXTilesetInfo* info) {
        CC_SAFE_RETAIN(info);
        CC_SAFE_RELEASE(_tileSet);
        _tileSet = info;
    }
    
    /** Layer orientation, which is the same as the map orientation.
     *
     * @return Layer orientation, which is the same as the map orientation.
     */
    int getLayerOrientation() const { return _layerOrientation; }
    
    /** Set layer orientation, which is the same as the map orientation.
     *
     * @param orientation Layer orientation,which is the same as the map orientation.
     */
    void setLayerOrientation(int orientation) { _layerOrientation = orientation; }
    
    /** Properties from the layer. They can be added using Tiled.
     *
     * @return Properties from the layer. They can be added using Tiled.
     */
    const ValueMap& getProperties() const { return _properties; }
    
    /** Properties from the layer. They can be added using Tiled.
     *
     * @return Properties from the layer. They can be added using Tiled.
     */
    ValueMap& getProperties() { return _properties; }
    
    /** Set an Properties from to layer.
     *
     * @param properties It is used to set the layer Properties.
     */
    void setProperties(const ValueMap& properties) {
        _properties = properties;
    }
    //
    // Override
    //
    /** TMXLayer doesn't support adding a Sprite manually.
     @warning addChild(z, tag); is not supported on TMXLayer. Instead of setTileGID.
     */
    using SpriteBatchNode::addChild;
    virtual void addChild(Node * child, int zOrder, int tag) override;
    // super method
    void removeChild(Node* child, bool cleanup) override;
    /**
    * @js NA
    */
    virtual std::string getDescription() const override;

protected:
    Vec2 getPositionForIsoAt(const Vec2& pos);
    Vec2 getPositionForOrthoAt(const Vec2& pos);
    Vec2 getPositionForHexAt(const Vec2& pos);
    Vec2 getPositionForStaggeredAt(const Vec2& pos);
    Vec2 calculateLayerOffset(const Vec2& offset);

    /* optimization methods */
    Sprite* appendTileForGID(uint32_t gid, const Vec2& pos);
    Sprite* insertTileForGID(uint32_t gid, const Vec2& pos);
    Sprite* updateTileForGID(uint32_t gid, const Vec2& pos);

    intptr_t getZForPos(const Vec2& pos) const;

    /* The layer recognizes some special properties, like cc_vertexz */
    void parseInternalProperties();
    void setupTileSprite(Sprite* sprite, const Vec2& pos, uint32_t gid);
    Sprite* reusedTileWithRect(const Rect& rect);
    int getVertexZForPos(const Vec2& pos);

    // index
    ssize_t atlasIndexForExistantZ(int z);
    ssize_t atlasIndexForNewZ(int z);


    //! name of the layer
    std::string _layerName;
    //! TMX Layer supports opacity
    unsigned char _opacity;
    
    //! Only used when vertexZ is used
    int _vertexZvalue;
    bool _useAutomaticVertexZ;

    //! used for optimization
    Sprite *_reusedTile;
    ccCArray *_atlasIndexArray;
    
    // used for retina display
    float _contentScaleFactor;
    
    /** size of the layer in tiles */
    Size _layerSize;
    /** size of the map's tile (could be different from the tile's size) */
    Size _mapTileSize;
    /** pointer to the map of tiles */
    uint32_t* _tiles;
    /** Tileset information for the layer */
    TMXTilesetInfo* _tileSet;
    /** Layer orientation, which is the same as the map orientation */
    int _layerOrientation;
    /** Stagger Axis */
    int _staggerAxis;
    /** Stagger Index */
    int _staggerIndex;
    /** Hex side length*/
    int _hexSideLength;
    /** properties from the layer. They can be added using Tiled */
    ValueMap _properties;
};

// end of tilemap_parallax_nodes group
/** @} */

NS_CC_END

#endif //__CCTMX_LAYER_H__

