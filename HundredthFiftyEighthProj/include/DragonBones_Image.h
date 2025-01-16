#ifndef TEXTURE_H
#define TEXTURE_H

#include "DragonBones_DisplayObject.h"
#include "DragonBones_ColorTransform.h"

class Texture;

namespace DragonBones {
	class Image : public DisplayObject {
	public:
		Texture* texture;

		Image(Texture* tex);
		void render();
		ColorTransform* color;
	};
}

#endif