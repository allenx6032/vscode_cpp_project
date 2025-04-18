#ifndef SPRITERMODEL_H
#define SPRITERMODEL_H

#include <vector>

#include "spriterengine_file.h"
#include "spriterengine_entity.h"

#include "spriterengine_loader.h"

#include "spriterengine_atlas.h"

namespace SpriterEngine
{

	class FileReference;
	class EntityInstance;
	class FileFactory;
	class ObjectFactory;

	typedef std::vector<File*> FileVector;
	typedef std::vector<Entity*> EntityVector;

	typedef std::vector<int> EntityIdVector;
	
	typedef std::vector<FileReference*> FileReferenceVector;
	typedef std::vector<std::string> StringVector;

	class SpriterModel
	{
	public:
		SpriterModel(const std::string &fileName, FileFactory *newFileFactory, ObjectFactory *newObjectFactory = 0);
		SpriterModel(FileFactory *newFileFactory, ObjectFactory *newObjectFactory=0);
		~SpriterModel();

		EntityInstance *getNewEntityInstance(int entityId);
		EntityInstance *getNewEntityInstance(EntityIdVector *entityIds);
		EntityInstance *getNewEntityInstance(std::string entityName);
		void appendEntityToInstanceByName(EntityInstance * entityInstance, std::string entityName);
		void setupFileReferences(FileReferenceVector *fileReferences);

		Entity *pushBackEntity(std::string entityName);
		void pushBackImageFile(std::string initialFilePath, point initialDefaultPivot, atlasdata atlasData);
		void pushBackSoundFile(std::string initialFilePath);
		void pushBackAtlas(std::string initialFilePath);
		void addAtlasFrameData(std::string initialFilePath, atlasframedata initialData);
		void pushBackTag(std::string newTag);

		int fileCount();

		File * getFileAtIndex(int fileIndex);
		FileFactory* getFileFactory() {
			return fileFactory;
		}

		const std::string *getTag(int tagIndex);

		void loadFile(const std::string &fileName);

	private:
		FileVector files;
		FileVector atlasFiles;
		EntityVector entities;
		StringVector tags;

		Loader loader;

		FileFactory *fileFactory;
		ObjectFactory *objectFactory;

		Entity *getEntity(int entityId);
		Entity *getEntity(std::string entityName);
	};

}

#endif // SPRITERMODEL_H
