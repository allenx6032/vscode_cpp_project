#ifndef TRANSFORMTIMELINE_H
#define TRANSFORMTIMELINE_H

#include "DragonBones_TimeLine.h"
#include "DragonBones_DBTransform.h"
#include "DragonBones_Vector2D.h"

namespace DragonBones {
	class TransformTimeline : public Timeline {
	private:
		float _offset;
		~TransformTimeline()  {
			delete[] originTransform;
		}
	public:
		bool transformed;
		DBTransform* originTransform;
		Vector2D* originPivot;
		static TransformTimeline* HIDE_TIMELINE;

		float getOffset(){return _offset;}

		void  setOffset(float value) {
			_offset = fmod(value,1);
			if(_offset < 0) {
				_offset += 1;
			}
		}

		TransformTimeline() : Timeline() {
			originTransform = new DBTransform();
			originPivot = new Vector2D(0,0);
			_offset = 0;
		}

		void dispose() {
			if(this == HIDE_TIMELINE)
				return;
			delete[] this;
		}
	};
};

#endif