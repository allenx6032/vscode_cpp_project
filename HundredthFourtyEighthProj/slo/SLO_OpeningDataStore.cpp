#include "SLO_OpeningDataStore.h"
#include "SLO_Resources.h"

OpeningDataStore::OpeningDataStore()
	: DataStore()
	, listMenuItem_{
		getString(StringId::MenuItem2DMenuDemo),
		getString(StringId::MenuItem3DMenuDemo),
		getString(StringId::MenuItemEtcDemo),
	}
{
}

OpeningDataStore::~OpeningDataStore()
{
}

const char *OpeningDataStore::getStringTitle() const
{
	return getString(StringId::TitleOpeningScene);
}

const std::vector<const char *> &OpeningDataStore::getOpeningMenuItemList() const
{
	return listMenuItem_;
}

double OpeningDataStore::getRadius() const
{
	return 2.0;
}
