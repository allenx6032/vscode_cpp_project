#ifndef ATLASFILE_H
#define ATLASFILE_H

#include <string>

#include "spriterengine_file.h"

#include "spriterengine_global.h"
#include "spriterengine_atlas.h"

namespace SpriterEngine
{
	class UniversalObjectInterface;
	class AtlasFile;

	class AtlasFile : public File
	{
	public:
		AtlasFile(std::string initialFilePath);

		AtlasFile *atlasFile() override;

		virtual void renderSprite(UniversalObjectInterface *spriteInfo, const atlasframedata data);
	};

}

#endif // ATLASFILE_H
