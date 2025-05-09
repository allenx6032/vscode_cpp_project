

#include "cocos2d-CCConfig.h"
#if CC_USE_UI > 0

/****************************************************************************
Copyright (c) 2015 Neo Kim (neo.kim@neofect.com)

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

#ifndef __UIPAGEVIEWINDICATOR_H__
#define __UIPAGEVIEWINDICATOR_H__

#include "cocos2d-UIPageView.h"
#include "cocos2d-CCSprite.h"

NS_CC_BEGIN
/**
 * @addtogroup ui
 * @{
 */

namespace ui {

class PageViewIndicator : public ProtectedNode
{

public:
    /**
     * Create a page view indicator with its parent page view.
     * @return A page view indicator instance.
     */
    static PageViewIndicator* create();

    PageViewIndicator();
    virtual ~PageViewIndicator();

    void setDirection(PageView::Direction direction);
    void reset(ssize_t numberOfTotalPages);
    void indicate(ssize_t index);
    void clear();
    void setSpaceBetweenIndexNodes(float spaceBetweenIndexNodes);
    float getSpaceBetweenIndexNodes() const { return _spaceBetweenIndexNodes; }
    void setSelectedIndexColor(const Color3B& color) { _currentIndexNode->setColor(color); }
    const Color3B& getSelectedIndexColor() const { return _currentIndexNode->getColor(); }
    void setIndexNodesColor(const Color3B& indexNodesColor);
    const Color3B& getIndexNodesColor() const { return _indexNodesColor; }
    void setIndexNodesScale(float indexNodesScale);
    float getIndexNodesScale() const { return _indexNodesScale; }
    
    /**
     * Sets texture for index nodes.
     *
     * @param fileName   File name of texture.
     * @param resType    @see TextureResType .
     */
    void setIndexNodesTexture(const std::string& texName,Widget::TextureResType texType = Widget::TextureResType::LOCAL);
    
protected:
    bool init() override;
    void increaseNumberOfPages();
    void decreaseNumberOfPages();
    void rearrange();

    PageView::Direction _direction;
    Vector<Sprite*> _indexNodes;
    Sprite* _currentIndexNode;
    float _spaceBetweenIndexNodes;
    float _indexNodesScale;
    Color3B _indexNodesColor;
    
    bool _useDefaultTexture;
    std::string _indexNodesTextureFile;
    Widget::TextureResType _indexNodesTexType;
};

}
// end of ui group
/// @}
NS_CC_END

#endif /* defined(__UIPAGEVIEWINDICATOR_H__) */


#endif // CC_USE_UI

