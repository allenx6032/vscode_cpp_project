#include "SLO_Resources.h"
#include "SLO_MenuInfo.h"
#include "SLO_SDLImage.h"
#include "sol.hpp"
#include "SLO_SDLJoystick.h"
#include <SDL_events.h>

Resources::Resources()
	: windowWidth_()
	, windowHeight_()
	, screenWidth_()
	, screenHeight_()
	, keyRepeatDelay_()
	, keyRepeatInterval_()
	, codeEnterButton_()
	, codeCancelButton_()
	, duration_()
	, delay_()
	, interval_()
	, interpolatorType_()
	, mapImage_()
	, mapString_()
	, mapJoystick_()
	, listMenuInfo_()
	, empty_()
	, viewAngle_(45.0f)
{
	reload();
}

Resources::~Resources()
{
}

std::shared_ptr<SDL_::Image> Resources::getImage(ImageId id) const
{
	auto i = mapImage_.find(id);
	if (i != mapImage_.end()) {
		return i->second;
	}
	else {
		return nullptr;
	}
}

const std::string &Resources::getString(StringId id) const
{
	auto i = mapString_.find(id);
	if (i != mapString_.end()) {
		return i->second;
	}
	else {
		return empty_;
	}
}

const char *Resources::getFontFileName() const
{
	return "res/font/ShipporiMincho-Regular.otf";
	//return "res/font/mouhitu.ttf";
}

Resources &Resources::instance()
{
	static Resources res;
	return res;
}

void Resources::addJoyDevice(const SDL_JoyDeviceEvent &jdevice)
{
	mapJoystick_.insert(std::make_pair(jdevice.which, std::make_shared<Joystick>(jdevice.which)));
}

void Resources::removeJoyDevice(const SDL_JoyDeviceEvent &jdevice)
{
	for (auto &pair : mapJoystick_) {
		if(pair.second->getInstanceID() == jdevice.which) {
			mapJoystick_.erase(pair.first);
			break;
		}
	}
}

std::shared_ptr<Joystick> Resources::getJoystick(int index) const
{
	auto i = mapJoystick_.find(index);
	if (i != mapJoystick_.end()) {
		return i->second;
	}
	else {
		return nullptr;
	}
}

void Resources::loadString()
{
	mapString_.insert(std::make_pair(StringId::TitleOpeningScene, "SDL2&OpenGL動作デモ"));
	mapString_.insert(std::make_pair(StringId::TitleMenuScene, "最初の画面"));
	mapString_.insert(std::make_pair(StringId::TitleSettingScene, "次の画面"));
	mapString_.insert(std::make_pair(StringId::MenuItem3DMenuDemo, "3Dメニュー　動作デモ"));
	mapString_.insert(std::make_pair(StringId::MenuItem2DMenuDemo, "2Dメニュー　動作デモ"));
	mapString_.insert(std::make_pair(StringId::MenuItemEtcDemo, "その他動作デモ"));
}

void Resources::clearString()
{
	mapString_.clear();
}

void Resources::loadImage()
{
	mapImage_.insert(std::make_pair(ImageId::ScreenBack, std::make_shared<SDL_::Image>("res/image/wallpaper_logo.png")));
	mapImage_.insert(std::make_pair(ImageId::TextBack, std::make_shared<SDL_::Image>("res/image/TextBack.png")));
	mapImage_.insert(std::make_pair(ImageId::ButtonNormalBack, std::make_shared<SDL_::Image>("res/image/ButtonNormalBack.png")));
	mapImage_.insert(std::make_pair(ImageId::ButtonSelectBack, std::make_shared<SDL_::Image>("res/image/ButtonSelectBack.png")));
}

void Resources::clearImage()
{
	mapImage_.clear();
}

void Resources::reload()
{
	clearString();
	loadString();
	clearImage();
	loadImage();

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script_file("res/lua/system.lua");

	windowWidth_ = lua["window"]["width"].get<int>();
	windowHeight_ = lua["window"]["height"].get<int>();
	screenWidth_ = lua["screen"]["width"].get<int>();
	screenHeight_ = lua["screen"]["height"].get<int>();

	keyRepeatDelay_ = lua["keyRepeat"]["delay"].get_or(300);
	keyRepeatInterval_ = lua["keyRepeat"]["interval"].get_or(50);

	codeEnterButton_ = lua["keyAssing"]["delay"].get_or(0);
	codeCancelButton_ = lua["keyAssing"]["delay"].get_or(1);

	interpolatorType_ = static_cast<InterpolatorType>(lua["animation"]["interpolator"].get<int>());
	duration_ = lua["animation"]["duration"].get<int>();

//	posCameraX_ = lua["camera"]["x"].get<float>();
//	posCameraY_ = lua["camera"]["y"].get<float>();
//	posCameraZ_ = lua["camera"]["z"].get<float>();
//	translateCameraX_ = lua["camera"]["translateX"].get<float>();
//	translateCameraY_ = lua["camera"]["translateY"].get<float>();
//	translateCameraZ_ = lua["camera"]["translateZ"].get<float>();
//	radius_ = lua["camera"]["radius"].get<float>();

	delay_ = lua["keyRepeat"]["delay"].get_or(300);
	interval_ = lua["keyRepeat"]["interval"].get_or(50);

	lua.script_file("res/lua/data/top_menu.lua");
	listMenuInfo_.clear();
	for (int i = 1; lua["topMenu"][i].valid(); ++i) {
		listMenuInfo_.push_back(std::make_shared<MenuInfo>(lua["topMenu"][i]["title"].get<const char *>(), lua["topMenu"][i]["path"].get<const char *>()));
	}

}
