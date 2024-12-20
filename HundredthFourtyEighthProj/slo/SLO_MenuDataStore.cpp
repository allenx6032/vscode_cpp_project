#include "SLO_MenuDataStore.h"
#include "SLO_Resources.h"
#include "SLO_GLTexture.h"

MenuDataStore::MenuDataStore()
	: DataStore()
	, res_(Resources::instance())
	, textureTextBack_(std::make_shared<GL_::Texture>(getImage(ImageId::TextBack)))
	, textureButtonNormalBack_(std::make_shared<GL_::Texture>(getImage(ImageId::ButtonNormalBack)))
	, textureButtonSelectBack_(std::make_shared<GL_::Texture>(getImage(ImageId::ButtonSelectBack)))
	, eyeCameraY_(0.0f)
	, centerCameraY_(0.0f)
	, radius_(20.0f)
	, degrees_(0.0f)
	, scale_(1.0f)
	//, ypos_(4.0f)
	, zpos_(0.0f)
	, zoffset_(0.0f)
	, isBillboard_(true)
{
}

MenuDataStore::~MenuDataStore()
{
}

const char *MenuDataStore::getStringTitle() const
{
	return getString(StringId::TitleMenuScene);
}

const std::vector<std::shared_ptr<MenuInfo>> &MenuDataStore::getMenuInfoList() const
{
	return res_.getMenuInfoList();
}

int MenuDataStore::getFontSize() const
{
	return 46;
}

void MenuDataStore::addViewAngle(float viewAngle)
{
	res_.setViewAngle(res_.getViewAngle() + viewAngle);
}

float MenuDataStore::getViewAngle() const
{
	return res_.getViewAngle();
}
