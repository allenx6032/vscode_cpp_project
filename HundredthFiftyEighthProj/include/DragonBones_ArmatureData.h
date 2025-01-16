#ifndef ARMATUREDATA_H
#define ARMATUREDATA_H

#include <string>
#include <vector>
#include "DragonBones_BoneData.h"
#include "DragonBones_SkinData.h"
#include "DragonBones_AnimationData.h"

namespace DragonBones {

	class ArmatureData {
	public:
		std::vector<BoneData*> _boneDataList;
		std::vector<SkinData*> _skinDataList;
		std::vector<AnimationData*> _animationDataList;
		std::string name;

		std::vector<BoneData*> getBoneDataList();
		std::vector<SkinData*> getSkinDataList();
		std::vector<AnimationData*> getAnimationDataList();

		ArmatureData();
		~ArmatureData();

		BoneData* getBoneData(std::string boneName);
		SkinData* getSkinData(std::string skinName);
		AnimationData* getAnimationData(std::string animationName);

		void addBoneData(BoneData* boneData);
		void addSkinData(SkinData* skinData);
		void addAnimationData(AnimationData* animationData);

		void sortBoneDataList();


	};

};


#endif