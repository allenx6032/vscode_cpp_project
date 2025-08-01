/****************************************************************************
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2020 cocos2d-lua.org

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


#ifndef __CC_TM_XML_PARSER__
#define __CC_TM_XML_PARSER__

/// @cond DO_NOT_SHOW

#include "cocos_CCGeometry.h"
#include "cocos_CCSAXParser.h"
#include "cocos_CCVector.h"
#include "cocos_CCValue.h"
#include "cocos_CCTMXObjectGroup.h" // needed for Vector<TMXObjectGroup*> for binding

#include <string>
#include <map>

NS_CC_BEGIN

class TMXLayerInfo;
class TMXTilesetInfo;

/** @file
* Internal TMX parser
*
* IMPORTANT: These classed should not be documented using doxygen strings
* since the user should not use them.
*
*/

/**
 * @addtogroup tilemap_parallax_nodes
 * @{
 */

enum {
    TMXLayerAttribNone = 1 << 0,
    TMXLayerAttribBase64 = 1 << 1,
    TMXLayerAttribGzip = 1 << 2,
    TMXLayerAttribZlib = 1 << 3,
    TMXLayerAttribCSV = 1 << 4,
};

enum {
    TMXPropertyNone,
    TMXPropertyMap,
    TMXPropertyLayer,
    TMXPropertyObjectGroup,
    TMXPropertyObject,
    TMXPropertyTile,
    TMXPropertyTileObject,
    TMXPropertyGroup,
    TMXPropertyImageLayer
};

typedef enum TMXTileFlags_ {
    kTMXTileHorizontalFlag  = 0x80000000,
    kTMXTileVerticalFlag    = 0x40000000,
    kTMXTileDiagonalFlag    = 0x20000000,
    kTMXFlipedAll           = (kTMXTileHorizontalFlag|kTMXTileVerticalFlag|kTMXTileDiagonalFlag),
    kTMXFlippedMask         = ~(kTMXFlipedAll)
} TMXTileFlags;

// Bits on the far end of the 32-bit global tile ID (GID's) are used for tile flags

/** @brief TMXLayerInfo contains the information about the layers like:
- Layer name
- Layer size
- Layer opacity at creation time (it can be modified at runtime)
- Whether the layer is visible (if it's not visible, then the CocosNode won't be created)

This information is obtained from the TMX file.
*/
class CC_DLL TMXLayerInfo : public Ref
{
public:
    /**
     * @js ctor
     */
    TMXLayerInfo();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXLayerInfo();

    void setProperties(ValueMap properties);
    ValueMap& getProperties();

    ValueMap            _properties;
    std::string         _name;
    Size                _layerSize;
    uint32_t            *_tiles;
    bool                _visible;
    unsigned char       _opacity;
    bool                _ownTiles;
    Vec2                _offset;
};

class CC_DLL TMXImageLayerInfo : public Ref
{
public:
    TMXImageLayerInfo();
    virtual ~TMXImageLayerInfo();

    void setProperties(ValueMap properties);
    ValueMap& getProperties();

    ValueMap            _properties;
    std::string         _name;
    bool                _visible;
    unsigned char       _opacity;
    Vec2                _offset;
    std::string         _sourceImage;
    //! size in pixels of the image
    Size                _imageSize; // set on image loaded
    std::string         _originSourceImage; // value in tmx
};

class CC_DLL TMXGroupInfo : public Ref
{
public:
    TMXGroupInfo();
    virtual ~TMXGroupInfo();

    void setProperties(ValueMap properties);
    ValueMap& getProperties();

    ValueMap            _properties;
    std::string         _name;
    bool                _visible;
    unsigned char       _opacity;
    Vec2                _offset;
    Vector<Ref*>        _children;
};

/** @brief TMXTilesetInfo contains the information about the tilesets like:
- Tileset name
- Tileset spacing
- Tileset margin
- size of the tiles
- Image used for the tiles
- Image size

This information is obtained from the TMX file. 
*/
class TMXTilesetImage
{
public:
    std::string sourceImage;
    Size imageSize;
    std::string originSourceImage;
};

class CC_DLL TMXTilesetInfo : public Ref
{
public:
    std::string _name;
    std::string _gridOrientation;
    int _firstGid;
    Size _tileSize; // use for tile show
    Size _gridSize; // use for count object's pos of groupobject
    int _spacing;
    int _margin;
    Vec2 _tileOffset;
    std::map<int, TMXTilesetImage*> _images; // gid -> image
    bool isCOI; // Collection of Images

public:
    TMXTilesetInfo();
    virtual ~TMXTilesetInfo();
    Rect getRectForGID(uint32_t gid);
    TMXTilesetImage *getImageForGID(uint32_t gid);
};

/** @brief TMXMapInfo contains the information about the map like:
- Map orientation (hexagonal, isometric or orthogonal)
- Tile size
- Map size

And it also contains:
- Layers (an array of TMXLayerInfo objects)
- Tilesets (an array of TMXTilesetInfo objects)
- ObjectGroups (an array of TMXObjectGroupInfo objects)

This information is obtained from the TMX file.

*/
class CC_DLL TMXMapInfo : public Ref, public SAXDelegator
{    
public:    
    /** creates a TMX Format with a tmx file */
    static TMXMapInfo * create(const std::string& tmxFile);
    /** creates a TMX Format with an XML string and a TMX resource path */
    static TMXMapInfo * createWithXML(const std::string& tmxString, const std::string& resourcePath);
    
    TMXMapInfo();
    virtual ~TMXMapInfo();
    
    /** initializes a TMX format with a  tmx file */
    bool initWithTMXFile(const std::string& tmxFile);
    /** initializes a TMX format with an XML string and a TMX resource path */
    bool initWithXML(const std::string& tmxString, const std::string& resourcePath);
    /** initializes parsing of an XML file, either a tmx (Map) file or tsx (Tileset) file */
    bool parseXMLFile(const std::string& xmlFilename);
    /* initializes parsing of an XML string, either a tmx (Map) string or tsx (Tileset) string */
    bool parseXMLString(const std::string& xmlString);

    ValueMapIntKey& getTileProperties() { return _tileProperties; };
    void setTileProperties(const ValueMapIntKey& tileProperties) {
        _tileProperties = tileProperties;
    }

    /// map orientation
    int getOrientation() const { return _orientation; }
    
    /// map staggeraxis
    int getStaggerAxis() const { return _staggerAxis; }
    
    /// map stagger index
    int getStaggerIndex() const { return _staggerIndex; }

    /// map hexsidelength
    int getHexSideLength() const { return _hexSideLength; }

    /// map width & height
    const Size& getMapSize() const { return _mapSize; }

    /// tiles width & height
    const Size& getTileSize() const { return _tileSize; }
    
    /// layers
    Vector<TMXLayerInfo*>& getLayers() { return _layers; }

    /// tilesets
    Vector<TMXTilesetInfo*>& getTilesets() { return _tilesets; }

    /// objectGroups
    Vector<TMXObjectGroup*>& getObjectGroups() { return _objectGroups; }
    
    /// children
    Vector<Ref*>& getChildren() { return _children; }

    /// layer attribs
    int getLayerAttribs() const { return _layerAttribs; }

    /// is storing characters?
    bool isStoringCharacters() const { return _storingCharacters; }

    /// properties
    ValueMap& getProperties() { return _properties; }
    
    // implement pure virtual methods of SAXDelegator
    void startElement(void *ctx, const char *name, const char **atts) override;
    void endElement(void *ctx, const char *name) override;
    void textHandler(void *ctx, const char *ch, size_t len) override;
    
    const std::string& getCurrentString() const { return _currentString; }
    const std::string& getTMXFileName() const { return _TMXFileName; }
    void setTMXFileName(const std::string& fileName){ _TMXFileName = fileName; }

protected:
    void internalInit(const std::string& tmxFileName, const std::string& resourcePath);

    /// map orientation
    int    _orientation;
    ///map staggerAxis
    int    _staggerAxis;
    ///map staggerIndex
    int    _staggerIndex;
    ///map hexsidelength
    int    _hexSideLength;
    /// map width & height
    Size _mapSize;
    /// tiles width & height
    Size _tileSize;
    /// layers
    Vector<TMXLayerInfo*> _layers;
    /// tilesets
    Vector<TMXTilesetInfo*> _tilesets;
    /// objectGroups
    Vector<TMXObjectGroup*> _objectGroups;
    // internal use for setting group's children
    Vector<TMXGroupInfo*> _groupStack;
    // internal use for set property
    TMXImageLayerInfo * _curImageLayer;
    /// children
    Vector<Ref*> _children;
    /// parent element
    int _parentElement;
    /// parent GID
    int _parentGID;
    /// layer attribs
    int _layerAttribs;
    /// is storing characters?
    bool _storingCharacters;
    /// properties
    ValueMap _properties;
    //! xml format tile index
    int _xmlTileIndex;
    
    //! tmx filename
    std::string _TMXFileName;
    // tmx resource path
    std::string _resources;
    //! current string
    std::string _currentString;
    //! tile properties
    ValueMapIntKey _tileProperties;
    int _currentFirstGID;
    bool _recordFirstGID;
};

// end of tilemap_parallax_nodes group
/// @}

NS_CC_END

/// @endcond
#endif
