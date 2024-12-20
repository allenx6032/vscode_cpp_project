#ifndef RESOURCES_MENUINFO_H_
#define RESOURCES_MENUINFO_H_

#include "SLO_Misc.h"
#include <string>

class MenuInfo
{
public:
	UNCOPYABLE(MenuInfo);
	MenuInfo(const char *title, const char *path);
	~MenuInfo();

	const char *getTitle() const { return menuTitle_.c_str(); }
	const char *getPath() const { return menuIconPath_.c_str(); }

private:
	std::string menuTitle_;
	std::string menuIconPath_;
};

#endif // RESOURCES_MENUINFO_H_
