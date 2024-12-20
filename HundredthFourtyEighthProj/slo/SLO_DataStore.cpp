#include "SLO_DataStore.h"
#include "SLO_Resources.h"
#include "SLO_GLTexture.h"

DataStore::DataStore()
	: res_(Resources::instance())
	, textureScreenBack_(std::make_shared<GL_::Texture>(res_.getImage(ImageId::ScreenBack)))
{
}

DataStore::~DataStore()
{
}

int DataStore::getWindowWidth() const
{
	return res_.getWindowWidth();
}

int DataStore::getWindowHeight() const
{
	return res_.getWindowHeight();
}

int DataStore::getScreenWidth() const
{
	return res_.getScreenWidth();
}

int DataStore::getScreenHeight() const
{
	return res_.getScreenHeight();
}

const char *DataStore::getFontFileName() const
{
	return res_.getFontFileName();
}

std::shared_ptr<SDL_::Image> DataStore::getImage(ImageId id) const
{
	return res_.getImage(id);
}

const char *DataStore::getString(StringId id) const
{
	return res_.getString(id).c_str();
}

int DataStore::getRepeatDelay() const
{
	return res_.getRepeatDelay();
}

int DataStore::getRepeatInterval() const
{
	return res_.getRepeatInterval();
}

int DataStore::getDuration() const
{
	return res_.getDuration();
}

InterpolatorType DataStore::getInterpolatorType() const
{
	return res_.getInterpolatorType();
}

std::shared_ptr<Joystick> DataStore::getJoystick(int index) const
{
	return res_.getJoystick(index);
}

int DataStore::getEnterButton() const
{
	return res_.getEnterButton();
}

int DataStore::getCancelButton() const
{
	return res_.getCancelButton();
}

void DataStore::reload()
{
	res_.reload();
}
