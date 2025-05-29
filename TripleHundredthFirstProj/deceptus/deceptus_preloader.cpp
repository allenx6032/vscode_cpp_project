#include "deceptus_preloader.h"

#include <iostream>

#include "deceptus_log.h"
#include "deceptus_detonationanimation.h"

void preloadDetonationAnimations()
{
   Log::Info() << "preloading detonation animations";
   DetonationAnimation::getFrameData(DetonationAnimation::DetonationType::Big);
}

void Preloader::preload()
{
   preloadDetonationAnimations();
}
