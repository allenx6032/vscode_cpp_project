#ifndef SCENE_MENU_MENUDATASTORE_H_
#define SCENE_MENU_MENUDATASTORE_H_

#include "SLO_DataStore.h"
#include <memory>
#include <vector>

namespace GL_
{
class Texture;
}

class MenuInfo;
class Resources;
class MenuDataStore : public DataStore
{
public:
	MenuDataStore();
	virtual ~MenuDataStore() override;

	std::shared_ptr<GL_::Texture> getTextureTextBack() const { return textureTextBack_; }
	std::shared_ptr<GL_::Texture> getTextureButtonNormalBack() const { return textureButtonNormalBack_; }
	std::shared_ptr<GL_::Texture> getTextureButtonSelectBack() const { return textureButtonSelectBack_; }

	const char *getStringTitle() const;

//	float getCameraXEye() const { return eyeCameraX_; }
	float getCameraYEye() const { return eyeCameraY_; }
//	float getCameraZEye() const { return eyeCameraZ_; }
//	float getCameraXCenter() const { return centerCameraX_; }
	float getCameraYCenter() const { return centerCameraY_; }
//	float getCameraZCenter() const { return centerCameraZ_; }

//	void setCameraXEye(float val) { eyeCameraX_ = val; }
	void setCameraYEye(float val) { eyeCameraY_ = val; }
//	void setCameraZEye(float val) { eyeCameraZ_ = val; }
//	void setCameraXCenter(float val) { centerCameraX_ = val; }
	void setCameraYCenter(float val) { centerCameraY_ = val; }
//	void setCameraZCenter(float val) { centerCameraZ_ = val; }

	bool isBillboard() const { return isBillboard_; }
	void setBillboard(bool val) { isBillboard_ = val; }


	float getRadius() const { return radius_; }
	float getDegrees() const { return degrees_; }
	float getScale() const { return scale_; }
	float getZPos() const { return zpos_; }
//	float getYPos() const { return ypos_; }
	float getZOffset() const { return zoffset_; }
	float getViewAngle() const;

	void setRadius(float radius) { radius_ = radius; }
	void setDegrees(float degrees) { degrees_ = degrees; }
	void setScale(float scale) { scale_ = scale; }
	void setZPos(float zpos) { zpos_ = zpos; }
//	void setYPos(float ypos) { ypos_ = ypos; }
	void setZOffset(float zoffset) { zoffset_ = zoffset; }

	void addViewAngle(float);

	const std::vector<std::shared_ptr<MenuInfo>> &getMenuInfoList() const;

	int getFontSize() const;

private:
	Resources &res_;
	std::shared_ptr<GL_::Texture> textureTextBack_;
	std::shared_ptr<GL_::Texture> textureButtonNormalBack_;
	std::shared_ptr<GL_::Texture> textureButtonSelectBack_;

//	float eyeCameraX_;
	float eyeCameraY_;
//	float eyeCameraZ_;
//	float centerCameraX_;
	float centerCameraY_;
//	float centerCameraZ_;
	float radius_;
	float degrees_;
	float scale_;
//	float ypos_;
	float zpos_;
	float zoffset_;
	bool isBillboard_;
};

#endif // SCENE_MENU_MENUDATASTORE_H_
