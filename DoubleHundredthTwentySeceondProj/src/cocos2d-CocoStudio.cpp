

#include "cocos2d-CCConfig.h"
#if CC_USE_CCS > 0


#include "cocos2d-CocoStudio.h"

#include "cocos2d-NodeReader.h"
#include "cocos2d-SingleNodeReader.h"
#include "cocos2d-SpriteReader.h"
#include "cocos2d-ParticleReader.h"
#include "cocos2d-GameMapReader.h"
#include "cocos2d-ProjectNodeReader.h"
#include "cocos2d-ComAudioReader.h"

#include "cocos2d-ButtonReader.h"
#include "cocos2d-CheckBoxReader.h"
#include "cocos2d-ImageViewReader.h"
#include "cocos2d-TextBMFontReader.h"
#include "cocos2d-TextReader.h"
#include "cocos2d-TextFieldReader.h"
#include "cocos2d-TextAtlasReader.h"
#include "cocos2d-LoadingBarReader.h"
#include "cocos2d-SliderReader.h"
#include "cocos2d-LayoutReader.h"
#include "cocos2d-ScrollViewReader.h"
#include "cocos2d-PageViewReader.h"
#include "cocos2d-ListViewReader.h"
#include "cocos2d-ArmatureNodeReader.h"

// 3D
//#include "cocos2d-Node3DReader/Node3DReader.h"
//#include "cocos2d-Sprite3DReader/Sprite3DReader.h"
//#include "cocos2d-UserCameraReader/UserCameraReader.h"
//#include "cocos2d-Particle3DReader/Particle3DReader.h"


namespace cocostudio
{
    void destroyCocosStudio()
    {        
        NodeReader::destroyInstance();
        SingleNodeReader::destroyInstance();
        SpriteReader::destroyInstance();
        ParticleReader::destroyInstance();
        GameMapReader::destroyInstance();
        ProjectNodeReader::destroyInstance();
        ComAudioReader::destroyInstance();
        
        WidgetReader::destroyInstance();
        ButtonReader::destroyInstance();
        CheckBoxReader::destroyInstance();
        ImageViewReader::destroyInstance();
        TextBMFontReader::destroyInstance();
        TextReader::destroyInstance();
        TextFieldReader::destroyInstance();
        TextAtlasReader::destroyInstance();
        LoadingBarReader::destroyInstance();
        SliderReader::destroyInstance();
        LayoutReader::destroyInstance();
        ScrollViewReader::destroyInstance();
        PageViewReader::destroyInstance();
        ListViewReader::destroyInstance();
        
        ArmatureNodeReader::destroyInstance();

        // 3D
//        Node3DReader::destroyInstance();
//        Sprite3DReader::destroyInstance();
//        UserCameraReader::destroyInstance();
//        Particle3DReader::destroyInstance();

        cocos2d::CSLoader::destroyInstance();
        
        
        ArmatureDataManager::destroyInstance();
        SceneReader::destroyInstance();
        ActionManagerEx::destroyInstance();
        GUIReader::destroyInstance();
    }
}


#endif // CC_USE_CCS

