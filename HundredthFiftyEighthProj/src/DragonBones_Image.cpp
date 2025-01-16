#include "DragonBones_Image.h"
#include "DragonBones_Texture.h"
#include "DragonBones_TransformUtil.h"
#include "DragonBones_DisplayObjectContainer.h"

namespace DragonBones {
	Image::Image(Texture* tex) : DisplayObject(){
		texture = tex;
		visible = true;
	}

	void Image::render() {
		if(visible) {
			if(texture) {

				if(parent)
				{
					texture->render(this->parent->rec*getTransformationMatrix());
				} else {

					texture->render(getTransformationMatrix());

				}

			}
		}
	}
}