

#include "cocos_ListViewReader.h"

#include "cocos_UIListView.h"
#include "cocos_CocoLoader.h"
#include "cocos_CSParseBinary_generated.h"
#include "cocos_FlatBuffersSerialize.h"

#include "tinyxml2.h"
#include "flatbuffers.h"

USING_NS_CC;
using namespace ui;
using namespace flatbuffers;

namespace cocostudio
{
    static const char* P_Direction = "direction";
    static const char* P_ItemMargin = "itemMargin";
    
    static ListViewReader* instanceListViewReader = nullptr;
    
    IMPLEMENT_CLASS_NODE_READER_INFO(ListViewReader)
    
    ListViewReader::ListViewReader()
    {
        
    }
    
    ListViewReader::~ListViewReader()
    {
        
    }
    
    ListViewReader* ListViewReader::getInstance()
    {
        if (!instanceListViewReader)
        {
            instanceListViewReader = new (std::nothrow) ListViewReader();
        }
        return instanceListViewReader;
    }
    
    void ListViewReader::destroyInstance()
    {
        CC_SAFE_DELETE(instanceListViewReader);
    }

    void ListViewReader::setPropsFromBinary(cocos2d::ui::Widget *widget, CocoLoader *cocoLoader, stExpCocoNode* cocoNode)
    {
        ScrollViewReader::setPropsFromBinary(widget, cocoLoader, cocoNode);
        
        ListView* listView = static_cast<ListView*>(widget);
        
        stExpCocoNode *stChildArray = cocoNode->GetChildArray(cocoLoader);
        
        for (int i = 0; i < cocoNode->GetChildNum(); ++i) {
            std::string key = stChildArray[i].GetName(cocoLoader);
            std::string value = stChildArray[i].GetValue(cocoLoader);
            
            if (key == P_Direction) {
                listView->setDirection((ScrollView::Direction)valueToInt(value));
            }
            else if(key == P_Gravity){
                listView->setGravity((ListView::Gravity)valueToInt(value));
            }else if(key == P_ItemMargin){
                listView->setItemsMargin(valueToFloat(value));
            }
            
        } //end of for loop
    }
    
    void ListViewReader::setPropsFromJsonDictionary(Widget *widget, const rapidjson::Value &options)
    {
        ScrollViewReader::setPropsFromJsonDictionary(widget, options);
        
        
        ListView* listView = static_cast<ListView*>(widget);
                
        int direction = DICTOOL->getFloatValue_json(options, P_Direction,2);
        listView->setDirection((ScrollView::Direction)direction);
        
        ListView::Gravity gravity = (ListView::Gravity)DICTOOL->getIntValue_json(options, P_Gravity,3);
        listView->setGravity(gravity);
        
        float itemMargin = DICTOOL->getFloatValue_json(options, P_ItemMargin);
        listView->setItemsMargin(itemMargin);
    }        
    
    Offset<Table> ListViewReader::createOptionsWithFlatBuffers(const tinyxml2::XMLElement *objectData,
                                                               flatbuffers::FlatBufferBuilder *builder)
    {
        auto temp = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
        auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);
        
        std::string path;
        std::string plistFile;
        int resourceType = 0;
        
        bool clipEnabled = false;
        Color3B bgColor;
        Color3B bgStartColor;
        Color3B bgEndColor;
        int colorType = 0;
        GLubyte bgColorOpacity = 255;
        Vec2 colorVector(0.0f, -0.5f);
        Rect capInsets;
        Size scale9Size;
        bool backGroundScale9Enabled = false;
        Size innerSize(200, 300);
        int direction = 0;
        bool bounceEnabled = false;
        int itemMargin = 0;
        std::string directionType;
        std::string horizontalType;
        std::string verticalType;
        
        // attributes
        auto attribute = objectData->FirstAttribute();
        while (attribute)
        {
            std::string name = attribute->Name();
            std::string value = attribute->Value();
            
            if (name == "ClipAble")
            {
                clipEnabled = (value == "True") ? true : false;
            }
            else if (name == "ComboBoxIndex")
            {
                colorType = atoi(value.c_str());
            }
            else if (name == "BackColorAlpha")
            {
                bgColorOpacity = atoi(value.c_str());
            }
            else if (name == "Scale9Enable")
            {
                if (value == "True")
                {
                    backGroundScale9Enabled = true;
                }
            }
            else if (name == "Scale9OriginX")
            {
                capInsets.origin.x = atof(value.c_str());
            }
            else if (name == "Scale9OriginY")
            {
                capInsets.origin.y = atof(value.c_str());
            }
            else if (name == "Scale9Width")
            {
                capInsets.size.width = atof(value.c_str());
            }
            else if (name == "Scale9Height")
            {
                capInsets.size.height = atof(value.c_str());
            }
            else if (name == "DirectionType")
            {
                directionType = value;
            }
            else if (name == "HorizontalType")
            {
                horizontalType = value;
            }
            else if (name == "VerticalType")
            {
                verticalType = value;
            }
            else if (name == "IsBounceEnabled")
            {
                bounceEnabled = (value == "True") ? true : false;
            }
            else if (name == "ItemMargin")
            {
                itemMargin = atoi(value.c_str());
            }
            
            attribute = attribute->Next();
        }
        
        // child elements
        const tinyxml2::XMLElement* child = objectData->FirstChildElement();
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "InnerNodeSize")
            {
                auto attributeInnerNodeSize = child->FirstAttribute();
                while (attributeInnerNodeSize)
                {
                    name = attributeInnerNodeSize->Name();
                    std::string value = attributeInnerNodeSize->Value();
                    
                    if (name == "Width")
                    {
                        innerSize.width = atof(value.c_str());
                    }
                    else if (name == "Height")
                    {
                        innerSize.height = atof(value.c_str());
                    }
                    
                    attributeInnerNodeSize = attributeInnerNodeSize->Next();
                }
            }
            else if (name == "Size" && backGroundScale9Enabled)
            {
                auto attributeSize = child->FirstAttribute();
                
                while (attributeSize)
                {
                    name = attributeSize->Name();
                    std::string value = attributeSize->Value();
                    
                    if (name == "X")
                    {
                        scale9Size.width = atof(value.c_str());
                    }
                    else if (name == "Y")
                    {
                        scale9Size.height = atof(value.c_str());
                    }
                    
                    attributeSize = attributeSize->Next();
                }
            }
            else if (name == "SingleColor")
            {
                auto attributeSingleColor = child->FirstAttribute();
                
                while (attributeSingleColor)
                {
                    name = attributeSingleColor->Name();
                    std::string value = attributeSingleColor->Value();
                    
                    if (name == "R")
                    {
                        bgColor.r = atoi(value.c_str());
                    }
                    else if (name == "G")
                    {
                        bgColor.g = atoi(value.c_str());
                    }
                    else if (name == "B")
                    {
                        bgColor.b = atoi(value.c_str());
                    }
                    
                    attributeSingleColor = attributeSingleColor->Next();
                }
            }
            else if (name == "EndColor")
            {
                auto attributeEndColor = child->FirstAttribute();
                
                while (attributeEndColor)
                {
                    name = attributeEndColor->Name();
                    std::string value = attributeEndColor->Value();
                    
                    if (name == "R")
                    {
                        bgEndColor.r = atoi(value.c_str());
                    }
                    else if (name == "G")
                    {
                        bgEndColor.g = atoi(value.c_str());
                    }
                    else if (name == "B")
                    {
                        bgEndColor.b = atoi(value.c_str());
                    }
                    
                    attributeEndColor = attributeEndColor->Next();
                }
            }
            else if (name == "FirstColor")
            {
                auto attributeFirstColor = child->FirstAttribute();
                
                while (attributeFirstColor)
                {
                    name = attributeFirstColor->Name();
                    std::string value = attributeFirstColor->Value();
                    
                    if (name == "R")
                    {
                        bgStartColor.r = atoi(value.c_str());
                    }
                    else if (name == "G")
                    {
                        bgStartColor.g = atoi(value.c_str());
                    }
                    else if (name == "B")
                    {
                        bgStartColor.b = atoi(value.c_str());
                    }
                    
                    attributeFirstColor = attributeFirstColor->Next();
                }
            }
            else if (name == "ColorVector")
            {
                auto attributeColorVector = child->FirstAttribute();
                while (attributeColorVector)
                {
                    name = attributeColorVector->Name();
                    std::string value = attributeColorVector->Value();
                    
                    if (name == "ScaleX")
                    {
                        colorVector.x = atof(value.c_str());
                    }
                    else if (name == "ScaleY")
                    {
                        colorVector.y = atof(value.c_str());
                    }
                    
                    attributeColorVector = attributeColorVector->Next();
                }
            }
            else if (name == "FileData")
            {
                std::string texture;
                std::string texturePng;
                
                auto attributeFileData = child->FirstAttribute();
                
                while (attributeFileData)
                {
                    name = attributeFileData->Name();
                    std::string value = attributeFileData->Value();
                    
                    if (name == "Path")
                    {
                        path = value;
                    }
                    else if (name == "Type")
                    {
                        resourceType = getResourceType(value);
                    }
                    else if (name == "Plist")
                    {
                        plistFile = value;
                        texture = value;
                    }
                    
                    attributeFileData = attributeFileData->Next();
                }
                
                if (resourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));                    
                }
            }
            
            child = child->NextSiblingElement();
        }
        
        Color f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
        Color f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
        Color f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
        ColorVector f_colorVector(colorVector.x, colorVector.y);
        CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
        FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
        FlatSize f_innerSize(innerSize.width, innerSize.height);
        
        auto options = CreateListViewOptions(*builder,
                                             widgetOptions,
                                             CreateResourceData(*builder,
                                                                builder->CreateString(path),
                                                                builder->CreateString(plistFile),
                                                                resourceType),
                                             clipEnabled,
                                             &f_bgColor,
                                             &f_bgStartColor,
                                             &f_bgEndColor,
                                             colorType,
                                             bgColorOpacity,
                                             &f_colorVector,
                                             &f_capInsets,
                                             &f_scale9Size,
                                             backGroundScale9Enabled,
                                             &f_innerSize,
                                             direction,
                                             bounceEnabled,
                                             itemMargin,
                                             builder->CreateString(directionType),
                                             builder->CreateString(horizontalType), 
                                             builder->CreateString(verticalType));
        
        return *(Offset<Table>*)(&options);
    }
    
    void ListViewReader::setPropsWithFlatBuffers(cocos2d::Node *node, const flatbuffers::Table *listViewOptions)
    {
        ListView* listView = static_cast<ListView*>(node);
        auto options = (ListViewOptions*)listViewOptions;
        
        bool clipEnabled = options->clipEnabled() != 0;
        listView->setClippingEnabled(clipEnabled);
        
        bool backGroundScale9Enabled = options->backGroundScale9Enabled() != 0;
        listView->setBackGroundImageScale9Enabled(backGroundScale9Enabled);
        
        
        auto f_bgColor = options->bgColor();
        Color3B bgColor(f_bgColor->r(), f_bgColor->g(), f_bgColor->b());
        auto f_bgStartColor = options->bgStartColor();
        Color3B bgStartColor(f_bgStartColor->r(), f_bgStartColor->g(), f_bgStartColor->b());
        auto f_bgEndColor = options->bgEndColor();
        Color3B bgEndColor(f_bgEndColor->r(), f_bgEndColor->g(), f_bgEndColor->b());
        
        auto f_colorVecor = options->colorVector();
        Vec2 colorVector(f_colorVecor->vectorX(), f_colorVecor->vectorY());
        listView->setBackGroundColorVector(colorVector);
        
        int bgColorOpacity = options->bgColorOpacity();
        
        int colorType = options->colorType();
        listView->setBackGroundColorType(Layout::BackGroundColorType(colorType));
        
        listView->setBackGroundColor(bgStartColor, bgEndColor);
        listView->setBackGroundColor(bgColor);
        listView->setBackGroundColorOpacity(bgColorOpacity);
        
        
        auto imageFileNameDic = options->backGroundImageData();
        int imageFileNameType = imageFileNameDic->resourceType();
        std::string imageFileName = imageFileNameDic->path()->c_str();
        listView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        
        auto widgetOptions = options->widgetOptions();
        auto f_color = widgetOptions->color();
        Color3B color(f_color->r(), f_color->g(), f_color->b());
        listView->setColor(color);
        
        int opacity = widgetOptions->alpha();
        listView->setOpacity(opacity);
        
        auto f_innerSize = options->innerSize();
        Size innerSize(f_innerSize->width(), f_innerSize->height());
        listView->setInnerContainerSize(innerSize);
        bool bounceEnabled = options->bounceEnabled() != 0;
        listView->setBounceEnabled(bounceEnabled);
        
        std::string directionType = options->directionType()->c_str();
        if (directionType == "")
        {
            listView->setDirection(ListView::Direction::HORIZONTAL);
            std::string verticalType = options->verticalType()->c_str();
            if (verticalType == "")
            {
                listView->setGravity(ListView::Gravity::TOP);
            }
            else if (verticalType == "Align_Bottom")
            {
                listView->setGravity(ListView::Gravity::BOTTOM);
            }
            else if (verticalType == "Align_VerticalCenter")
            {
                listView->setGravity(ListView::Gravity::CENTER_VERTICAL);
            }
        }
        else if (directionType == "Vertical")
        {
            listView->setDirection(ListView::Direction::VERTICAL);
            std::string horizontalType = options->horizontalType()->c_str();
            if (horizontalType == "")
            {
                listView->setGravity(ListView::Gravity::LEFT);
            }
            else if (horizontalType == "Align_Right")
            {
                listView->setGravity(ListView::Gravity::RIGHT);
            }
            else if (horizontalType == "Align_HorizontalCenter")
            {
                listView->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
            }
        }
        
        float itemMargin = options->itemMargin();
        listView->setItemsMargin(itemMargin);
        
        auto widgetReader = WidgetReader::getInstance();
        widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());
        
        if (backGroundScale9Enabled)
        {
            auto f_capInsets = options->capInsets();
            Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
            listView->setBackGroundImageCapInsets(capInsets);
            
            auto f_scale9Size = options->scale9Size();
            Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
            listView->setContentSize(scale9Size);
        }
        else
        {
            if (!listView->isIgnoreContentAdaptWithSize())
            {
                Size contentSize(widgetOptions->size()->width(), widgetOptions->size()->height());
                listView->setContentSize(contentSize);
            }
        }
    }
    
    Node* ListViewReader::createNodeWithFlatBuffers(const flatbuffers::Table *listViewOptions)
    {
        ListView* listView = ListView::create();
        
        setPropsWithFlatBuffers(listView, (Table*)listViewOptions);
        
        return listView;
    }
    
    int ListViewReader::getResourceType(std::string key)
    {
        if(key == "Normal" || key == "Default")
        {
            return 	0;
        }
        
        FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
        if(fbs->_isSimulator)
        {
            if(key == "MarkedSubImage")
            {
                return 0;
            }
        }
        return 1;
    }
    
}

