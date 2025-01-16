#ifndef TRANSFORMFRAME_H
#define TRANSFORMFRAME_H

#include "DragonBones_DBTransform.h"
#include "DragonBones_ColorTransform.h"
#include "DragonBones_Vector2D.h"
#include "DragonBones_Frame.h"
#include <ymath.h>

namespace DragonBones {
	class TransformFrame : public Frame {
	public:
		float tweenEasing;
		float tweenRotate;
		int displayIndex;
		bool visible;
		int zOrder;

		DBTransform* global;
		DBTransform* transform;
		Vector2D* pivot;
		ColorTransform* color;


		TransformFrame() : Frame() {
			tweenEasing = 0.0f;//_Nan._Float;
			tweenRotate = 0;
			displayIndex =0;
			visible = true;
			zOrder = 0;

			position = 0;
			duration = 0;

			global = new DBTransform();
			transform = new DBTransform();
			pivot = new Vector2D(0,0);
			color = 0;
		}

		~TransformFrame() {
			delete[] global;
			delete[] transform;
			delete[] color;
		}

	};
};

#endif