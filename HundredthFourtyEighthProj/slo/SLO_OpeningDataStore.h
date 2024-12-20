#ifndef SCENE_OPENING_OPENINGDATASTORE_H_
#define SCENE_OPENING_OPENINGDATASTORE_H_

#include "SLO_DataStore.h"

class OpeningDataStore : public DataStore
{
public:
	OpeningDataStore();
	virtual ~OpeningDataStore() override;

	int getMenuFontSize() const { return 48; }
	int getTitleFontSize() const { return 96; }

	const char *getStringTitle() const;
	double getRadius() const;

	const std::vector<const char *> &getOpeningMenuItemList() const;

private:
	std::vector<const char *> listMenuItem_;
};

#endif // SCENE_OPENING_OPENINGDATASTORE_H_
