/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2018 DragonBones team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DRAGONBONES_CC_FACTORY_H
#define DRAGONBONES_CC_FACTORY_H

#include "cocos_DragonBonesHeaders.h"
#include "cocos_cocos2d.h"
#include "cocos_dragonBonesCCArmatureDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN
/**
 * The Cocos2d factory.
 * @version DragonBones 3.0
 * @language en_US
 */
/**
 * Cocos2d 工厂。
 * @version DragonBones 3.0
 * @language zh_CN
 */
class CCFactory : public BaseFactory
{
    DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(CCFactory)

private:
    static DragonBones* _dragonBonesInstance;
    static CCFactory* _factory;

public:
    /**
     * A global factory instance that can be used directly.
     * @version DragonBones 4.7
     * @language en_US
     */
    /**
     * 一个可以直接使用的全局工厂实例。
     * @version DragonBones 4.7
     * @language zh_CN
     */
    static CCFactory* getFactory()
    {
        if (CCFactory::_factory == nullptr) 
        {
            CCFactory::_factory = new CCFactory();
        }

        return CCFactory::_factory;
    }

protected:
    std::string _prevPath;

public:
    /**
     * @inheritDoc
     */
    CCFactory() :
        _prevPath()
    {
        if (_dragonBonesInstance == nullptr)
        {
            const auto eventManager = CCArmatureDisplay::create();
            eventManager->retain();

            _dragonBonesInstance = new DragonBones(eventManager);
            _dragonBonesInstance->yDown = false;

            cocos2d::Director::getInstance()->getScheduler()->schedule(
                [&](float passedTime)
                {
                    _dragonBonesInstance->advanceTime(passedTime);
                },
                this, 0.0f, false, "dragonBonesClock"
            );
        }

        _dragonBones = _dragonBonesInstance;
    }
    virtual ~CCFactory() 
    {
        clear();
    }

protected:
    virtual TextureAtlasData* _buildTextureAtlasData(TextureAtlasData* textureAtlasData, void* textureAtlas) const override;
    virtual Armature* _buildArmature(const BuildArmaturePackage& dataPackage) const override;
    virtual Slot* _buildSlot(const BuildArmaturePackage& dataPackage, const SlotData* slotData, Armature* armature) const override;

public:
    virtual DragonBonesData* loadDragonBonesData(const std::string& filePath, const std::string& name = "", float scale = 1.0f);
    /**
     * - Load and parse a texture atlas data and texture from the local and cache them to the factory.
     * @param  filePath - The file path of texture atlas data.
     * @param name - Specify a cache name for the instance so that the instance can be obtained through this name. (If not set, use the instance name instead)
     * @param scale - Specify a scaling value for the map set. (Not scaled by default)
     * @returns The TextureAtlasData instance.
     * @version DragonBones 4.5
     * @example
     * <pre>
     *     factory.loadTextureAtlasData("hero_tex.json");
     * </pre>
     * @language en_US
     */
    /**
     * - 从本地加载并解析一个贴图集数据和贴图并缓存到工厂中。
     * @param filePath - 贴图集数据文件路径。
     * @param name - 为该实例指定一个缓存名称，以便可以通过此名称获取该实例。 （如果未设置，则使用该实例中的名称）
     * @param scale - 为贴图集指定一个缩放值。 （默认不缩放）
     * @returns TextureAtlasData 实例。
     * @version DragonBones 4.5
     * @example
     * <pre>
     *     factory.loadTextureAtlasData("hero_tex.json");
     * </pre>
     * @language zh_CN
     */
    virtual TextureAtlasData* loadTextureAtlasData(const std::string& filePath, const std::string& name = "", float scale = 1.0f);
    /**
     * - Create a armature from cached DragonBonesData instances and TextureAtlasData instances, then use the {@link #clock} to update it.
     * The difference is that the armature created by {@link #buildArmature} is not WorldClock instance update.
     * @param armatureName - The armature data name.
     * @param dragonBonesName - The cached name of the DragonBonesData instance. (If not set, all DragonBonesData instances are retrieved, and when multiple DragonBonesData instances contain a the same name armature data, it may not be possible to accurately create a specific armature)
     * @param skinName - The skin name, you can set a different ArmatureData name to share it's skin data. (If not set, use the default skin data)
     * @returns The armature display container.
     * @version DragonBones 4.5
     * @example
     * <pre>
     *     let armatureDisplay = factory.buildArmatureDisplay("armatureName", "dragonBonesName");
     * </pre>
     * @language en_US
     */
    /**
     * - 通过缓存的 DragonBonesData 实例和 TextureAtlasData 实例创建一个骨架，并用 {@link #clock} 更新该骨架。
     * 区别在于由 {@link #buildArmature} 创建的骨架没有 WorldClock 实例驱动。
     * @param armatureName - 骨架数据名称。
     * @param dragonBonesName - DragonBonesData 实例的缓存名称。 （如果未设置，将检索所有的 DragonBonesData 实例，当多个 DragonBonesData 实例中包含同名的骨架数据时，可能无法准确的创建出特定的骨架）
     * @param skinName - 皮肤名称，可以设置一个其他骨架数据名称来共享其皮肤数据。 （如果未设置，则使用默认的皮肤数据）
     * @returns 骨架的显示容器。
     * @version DragonBones 4.5
     * @example
     * <pre>
     *     let armatureDisplay = factory.buildArmatureDisplay("armatureName", "dragonBonesName");
     * </pre>
     * @language zh_CN
     */
    virtual CCArmatureDisplay* buildArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName = "", const std::string& skinName = "", const std::string& textureAtlasName = "") const;
    /**
     * - Create the display object with the specified texture.
     * @param textureName - The texture data name.
     * @param textureAtlasName - The texture atlas data name. (Of not set, all texture atlas data will be searched)
     * @version DragonBones 3.0
     * @language en_US
     */
    /**
     * - 创建带有指定贴图的显示对象。
     * @param textureName - 贴图数据名称。
     * @param textureAtlasName - 贴图集数据名称。 （如果未设置，将检索所有的贴图集数据）
     * @version DragonBones 3.0
     * @language zh_CN
     */
    virtual cocos2d::Sprite* getTextureDisplay(const std::string& textureName, const std::string& dragonBonesName = "") const;
    /**
     * - A global sound event manager.
     * Sound events can be listened to uniformly from the manager.
     * @version DragonBones 4.5
     * @language en_US
     */
    /**
     * - 全局声音事件管理器。
     * 声音事件可以从该管理器统一侦听。
     * @version DragonBones 4.5
     * @language zh_CN
     */
    virtual CCArmatureDisplay* getSoundEventManager() const
    {
        return dynamic_cast<CCArmatureDisplay*>(static_cast<IArmatureProxy*>(_dragonBones->getEventManager()));
    }

    /**
     * Deprecated, please refer to {@link #clock}.
     * @deprecated
     * @language en_US
     */
    /**
     * 已废弃，请参考 {@link #clock}。
     * @deprecated
     * @language zh_CN
     */
    static WorldClock* getClock()
    {
        return _dragonBonesInstance->getClock();
    }
};

DRAGONBONES_NAMESPACE_END
#endif // DRAGONBONES_CC_FACTORY_H
