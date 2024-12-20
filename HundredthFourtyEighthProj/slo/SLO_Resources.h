#ifndef RESOURCES_H_
#define RESOURCES_H_

#include "SLO_Misc.h"
#include "SLO_ImageId.h"
#include "SLO_StringId.h"
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace SDL_
{
class Image;
}

enum class InterpolatorType;
struct SDL_JoyDeviceEvent;
class Joystick;
class MenuInfo;
class Resources {
public:
	UNCOPYABLE(Resources);
	~Resources();

	void setWindowWidth(int width) { windowWidth_ = width; }
	void setWindowHeight(int height) { windowHeight_ = height; }
	int getWindowWidth() const { return windowWidth_; }
	int getWindowHeight() const { return windowHeight_; }
	int getScreenWidth() const { return screenWidth_; }
	int getScreenHeight() const { return screenHeight_; }
	int getEnterButton() const { return codeEnterButton_; }
	int getCancelButton() const { return codeCancelButton_; }
	int getDuration() const { return duration_; }
	int getRepeatDelay() const { return delay_; }
	int getRepeatInterval() const { return interval_; }
	InterpolatorType getInterpolatorType() const { return interpolatorType_; }

	float getViewAngle() const { return viewAngle_; }
	void setViewAngle(float viewAngle) { viewAngle_ = viewAngle; }

	const std::vector<std::shared_ptr<MenuInfo>> &getMenuInfoList() const { return listMenuInfo_; }

	const char *getFontFileName() const;

	void addJoyDevice(const SDL_JoyDeviceEvent &);
	void removeJoyDevice(const SDL_JoyDeviceEvent &);

	std::shared_ptr<SDL_::Image> getImage(ImageId) const;
	const std::string &getString(StringId) const;
	std::shared_ptr<Joystick> getJoystick(int) const;

	void loadString();
	void clearString();
	void loadImage();
	void clearImage();
	void reload();

	static Resources &instance();

private:
	Resources();

	int windowWidth_;
	int windowHeight_;
	int screenWidth_;
	int screenHeight_;
	int keyRepeatDelay_;
	int keyRepeatInterval_;
	int codeEnterButton_;
	int codeCancelButton_;
	int duration_;
	int delay_;
	int interval_;
	InterpolatorType interpolatorType_;
	std::map<ImageId, std::shared_ptr<SDL_::Image>> mapImage_;
	std::map<StringId, std::string> mapString_;
	std::map<int32_t, std::shared_ptr<Joystick>> mapJoystick_;

	std::vector<std::shared_ptr<MenuInfo>> listMenuInfo_;
	std::string empty_;

	float viewAngle_;
};

#endif // RESOURCES_H_
