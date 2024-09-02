/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
GacUI Reflection Helper

***********************************************************************/

#ifndef VCZH_PRESENTATION_GACUIREFLECTIONHELPER
#define VCZH_PRESENTATION_GACUIREFLECTIONHELPER

#include "Resources/GuiResourceManager.h"
#include "GraphicsElement/GuiGraphicsTextElement.h"
#include "GraphicsElement/GuiGraphicsDocumentElement.h"
#ifndef VCZH_PRESENTATION_COMPOSITION_INCLUDEALL
#define VCZH_PRESENTATION_COMPOSITION_INCLUDEALL
#include "../Application/GraphicsCompositions/GuiGraphicsComposition.h"
#include "../Application/GraphicsCompositions/GuiGraphicsWindowComposition.h"
#include "../Application/GraphicsCompositions/GuiGraphicsBoundsComposition.h"
#include "GuiGraphicsTableComposition.h"
#include "GuiGraphicsStackComposition.h"
#include "GuiGraphicsFlowComposition.h"
#include "GuiGraphicsSpecializedComposition.h"
#include "GuiGraphicsResponsiveComposition.h"
#include "GuiGraphicsSharedSizeComposition.h"
#include "GuiGraphicsRepeatComposition.h"
#include "GuiGraphicsAxis.h"
#endif
#include "Application/GraphicsHost/GuiGraphicsHost.h"
#ifndef VCZH_PRESENTATION_CONTROLS_INCLUDEALL
#define VCZH_PRESENTATION_CONTROLS_INCLUDEALL
#include "../Application/Controls/GuiBasicControls.h"
#include "../Application/Controls/GuiApplication.h"
#include "../Application/Controls/GuiLabelControls.h"
#include "../Application/Controls/GuiWindowControls.h"
#include "GuiButtonControls.h"
#include "GuiContainerControls.h"
#include "GuiDateTimeControls.h"
#include "GuiScrollControls.h"
#include "GuiDialogs.h"
#include "ListControlPackage/GuiListControls.h"
#include "ListControlPackage/GuiTextListControls.h"
#include "ListControlPackage/GuiListViewControls.h"
#include "ListControlPackage/GuiTreeViewControls.h"
#include "ListControlPackage/GuiComboControls.h"
#include "ListControlPackage/GuiDataGridControls.h"
#include "ListControlPackage/GuiBindableListControls.h"
#include "ListControlPackage/GuiBindableDataGrid.h"
#include "ListControlPackage/GuiDataGridExtensions.h"
#include "TextEditorPackage/GuiTextControls.h"
#include "TextEditorPackage/GuiDocumentViewer.h"
#include "ToolstripPackage/GuiMenuControls.h"
#include "ToolstripPackage/GuiToolstripMenu.h"
#include "ToolstripPackage/GuiRibbonControls.h"
#include "ToolstripPackage/GuiRibbonGalleryList.h"
#endif
#include "Controls/Templates/GuiAnimation.h"
#include "Controls/Templates/GuiCommonTemplates.h"
#include "Controls/Templates/GuiThemeStyleFactory.h"
#include "Utilities/FakeServices/GuiFakeDialogServiceBase.h"

namespace vl
{
	namespace presentation
	{
		namespace helper_types
		{
			struct SiteValue
			{
				vint					row = 0;
				vint					column = 0;
				vint					rowSpan = 1;
				vint					columnSpan = 1;

				auto operator<=>(const SiteValue&) const = default;
			};

			class LocalizedStrings
			{
			public:
				static WString			FirstOrEmpty(const collections::LazyList<WString>& formats);
			};
		}
	}

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Serialization
***********************************************************************/

			template<>
			struct TypedValueSerializerProvider<presentation::Color>
			{
				static presentation::Color GetDefaultValue();
				static bool Serialize(const presentation::Color& input, WString& output);
				static bool Deserialize(const WString& input, presentation::Color& output);
			};

			template<>
			struct TypedValueSerializerProvider<presentation::DocumentFontSize>
			{
				static presentation::DocumentFontSize GetDefaultValue();
				static bool Serialize(const presentation::DocumentFontSize& input, WString& output);
				static bool Deserialize(const WString& input, presentation::DocumentFontSize& output);
			};

			template<>
			struct TypedValueSerializerProvider<presentation::GlobalStringKey>
			{
				static presentation::GlobalStringKey GetDefaultValue();
				static bool Serialize(const presentation::GlobalStringKey& input, WString& output);
				static bool Deserialize(const WString& input, presentation::GlobalStringKey& output);
			};

/***********************************************************************
External Functions
***********************************************************************/

			extern Ptr<presentation::INativeImage>							INativeImage_Constructor(const WString& path);
			extern presentation::INativeCursor*								INativeCursor_Constructor1();
			extern presentation::INativeCursor*								INativeCursor_Constructor2(presentation::INativeCursor::SystemCursorType type);

			template<typename T>
			Ptr<T> Element_Constructor()
			{
				return Ptr(T::Create());
			}
			extern presentation::elements::text::TextLines*					GuiColorizedTextElement_GetLines(presentation::elements::GuiColorizedTextElement* thisObject);

			extern void														GuiTableComposition_SetRows(presentation::compositions::GuiTableComposition* thisObject, vint value);
			extern void														GuiTableComposition_SetColumns(presentation::compositions::GuiTableComposition* thisObject, vint value);
			extern void														IGuiAltActionHost_CollectAltActions(presentation::compositions::IGuiAltActionHost* host, collections::List<presentation::compositions::IGuiAltAction*>& actions);
		}
	}
}

#endif