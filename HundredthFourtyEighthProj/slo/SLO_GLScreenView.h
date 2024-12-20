#ifndef GLSCREENVIEW_H_
#define GLSCREENVIEW_H_

#include "SLO_GLGroupView.h"

namespace GL_
{

class ScreenView final : public GroupView
{
public:
	ScreenView();
	virtual ~ScreenView() override = default;
};

} // GL_

#endif // GLSCREENVIEW_H_
