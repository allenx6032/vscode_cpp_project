#ifndef SCENE_DATASTORE_H_
#define SCENE_DATASTORE_H_

#include "SLO_Misc.h"
#include <memory>
#include <string>
#include <vector>

namespace SDL_
{
class Image;
}

namespace GL_
{
class Texture;
}

enum class ImageId;
enum class StringId;
enum class InterpolatorType;
class Joystick;
class Resources;
class DataStore
{
public:
	UNCOPYABLE(DataStore);
	DataStore();
	virtual ~DataStore();

	std::shared_ptr<GL_::Texture> getTextureScreenBack() const { return textureScreenBack_; }

	int getWindowWidth() const;
	int getWindowHeight() const;
	int getScreenWidth() const;
	int getScreenHeight() const;

	const char *getFontFileName() const;
	std::shared_ptr<SDL_::Image> getImage(ImageId) const;
	const char *getString(StringId) const;

	int getRepeatDelay() const;
	int getRepeatInterval() const;
	int getDuration() const;
	InterpolatorType getInterpolatorType() const;
	std::shared_ptr<Joystick> getJoystick(int) const;
	int getEnterButton() const;
	int getCancelButton() const;

	void reload();

private:
	Resources &res_;
	std::shared_ptr<GL_::Texture> textureScreenBack_;
};

#endif // SCENE_DATASTORE_H_
