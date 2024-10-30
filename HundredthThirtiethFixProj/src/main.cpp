#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <memory>

#include <Windows.h>
#include "httplib.h"
//using namespace httplib;
using namespace std;

// #include "logger.h"
#include "logger.hpp"

#include<iostream>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<locale.h>
#include<random>
#include<string>
#include<vector>
#include <chrono>
#include"md5.h"
#include"json.hpp"
#include"conversion_wstring_string.h"
#include"conversion_utf8_gbk.h"
// #include"wordmanager.h"

// #include "glad.h"
// #include "gl3w.h"
// #include "GL/glew.h"
#ifdef _WIN32
#include <winsock2.h>
// #ifdef _MSC_VER
#include <gl2.h>
// #else
// #include <GL/gl.h>
// #endif
// #elif defined __linux__
// #include <GL/gl.h>
// #else
// #define GL_SILENCE_DEPRECATION
// #include <OpenGL/gl3.h>
#endif


#include <tigr.h>
#include <SDL.h>
#include <wz.h>
#include <wz_renderer_nanovg.h>
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 150
class TigrRenderer : public wz::IRenderer
{
public:
	TigrRenderer(Tigr *screen) : screen(screen) {}
	virtual void drawButton(wz::Button *button, wz::Rect /*clip*/)
	{
		const wz::Rect rect = button->getAbsoluteRect();
		// Change color when hovering.
		const TPixel color = button->getHover() ? tigrRGB(128, 255, 128) : tigrRGB(255, 255, 255);
		// Draw background and border.
		tigrFill(screen, rect.x, rect.y, rect.w, rect.h, tigrRGB(64, 64, 64));
		tigrRect(screen, rect.x, rect.y, rect.w, rect.h, color);
		// Draw centered text.
		wz::Size textSize;
		textSize.w = tigrTextWidth(tfont, button->getLabel());
		textSize.h = tigrTextHeight(tfont, button->getLabel());
		tigrPrint(screen, tfont, rect.x + rect.w / 2 - textSize.w / 2, rect.y + rect.h / 2 - textSize.h / 2, color, button->getLabel());
	}
	
	virtual wz::Size measureButton(wz::Button *button)
	{
		// Measure text.
		wz::Size size;
		size.w = tigrTextWidth(tfont, button->getLabel());
		size.h = tigrTextHeight(tfont, button->getLabel());
		// Add padding.
		const wz::Border padding = button->getPadding();
		size.w += padding.left + padding.right;
		size.h += padding.top + padding.bottom;
		return size;
	}
private:
	Tigr *screen;
};
static const int textCursorBlinkInterval = 500;
static const char *customListData[3] =
{
	"./data/accept.png",
	"./data/delete.png",
	"./data/error.png"
};
static const char *listData[17] =
{
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday",
	"One",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight",
	"Nine",
	"Ten"
};
static void CustomDrawListItemCallback(wz::IRenderer *renderer, wz::Rect clip, const wz::List * /*list*/, const char * /*fontFace*/, float /*fontSize*/, int /*itemIndex*/, const uint8_t *itemData)
{
	int image, width, height;
	wz::Rect rect;
	wz::NVGRenderer *nvg = (wz::NVGRenderer *)renderer;
	image = nvg->createImage((const char *)itemData, &width, &height);
	rect.x = clip.x + (int)(clip.w / 2.0f - width / 2.0f);
	rect.y = clip.y + (int)(clip.h / 2.0f - height / 2.0f);
	rect.w = width;
	rect.h = height;
	nvg->drawImage(rect, image);
}
class GUI
{
public:
	GUI(int windowWidth, int windowHeight, wz::IRenderer *renderer) : mainWindow(renderer, wz::MainWindowFlags::DockingEnabled | wz::MainWindowFlags::MenuEnabled)
	{
		mainWindow.setSize(windowWidth, windowHeight);
		mainWindow.createMenuButton("File");
		mainWindow.createMenuButton("Edit");
		mainWindow.createMenuButton("View");
		mainWindow.createMenuButton("Options");
		mainWindow.createMenuButton("Window");
		createButtonFrame();
		createCheckBoxFrame();
		createComboFrame();
		createGroupBoxFrame();
		createLabelFrame();
		createListFrame();
		createRadioButtonFrame();
		createScrollerFrame();
		createSpinnerFrame();
		createTabbedFrame();
		createTextEditFrame();
		createWindowFrame();
		createParentLayoutFrame();
		createStackLayoutFrame();
		createWidgetCategoryWindow();
		createWindow1();
		createWindow2();
		setFrame(0);
	}
	wz::MainWindow mainWindow;
private:
	wz::Frame *createFrame(const char *label)
	{
		wz::Frame *frame = new wz::Frame();
		frame->setStretch(wz::Stretch::All);
		mainWindow.add(frame);
		WidgetCategoryListItem category;
		category.label = label;
		category.frame = frame;
		widgetCategories.push_back(category);
		return frame;
	}
	void createButtonFrame()
	{
		wz::Frame *frame = createFrame("Button");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		layout->add(new wz::Button("Button with a label"));
		layout->add(new wz::Button("Button with a label and icon", "../data/accept.png"));
		layout->add(new wz::Button("", "../data/accept.png"));
		wz::Button *paddedButton = new wz::Button("Custom padding");
		paddedButton->setPadding(20, 40, 20, 40);
		layout->add(paddedButton);
		wz::ToggleButton *toggleButton = new wz::ToggleButton("Toggle Button", std::string());
		layout->add(toggleButton);
	}
	void createCheckBoxFrame()
	{
		wz::Frame *frame = createFrame("CheckBox");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::CheckBox *checkbox = new wz::CheckBox("Toggle me!");
		layout->add(checkbox);
	}
	void createComboFrame()
	{
		wz::Frame *frame = createFrame("Combo");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::Combo *combo1 = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
		combo1->setPosition(800, 50);
		layout->add(combo1);
		wz::Combo *combo2 = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
		combo2->setFont("visitor1", 12);
		layout->add(combo2);
	}
	void createGroupBoxFrame()
	{
		wz::Frame *frame = createFrame("GroupBox");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::GroupBox *groupBox1 = new wz::GroupBox("With a Label");
		groupBox1->setContent(new wz::Label("Content gones here"));
		layout->add(groupBox1);
		wz::GroupBox *groupBox2 = new wz::GroupBox();
		groupBox2->setContent(new wz::Label("Without a label"));
		layout->add(groupBox2);
		wz::GroupBox *groupBox3 = new wz::GroupBox("With some more content");
		wz::StackLayout *layout2 = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout2->setSpacing(8);
		layout2->add(new wz::Label("A label"));
		layout2->add(new wz::RadioButton("Option 1"));
		layout2->add(new wz::RadioButton("Option 2"));
		layout2->add(new wz::RadioButton("Option 3"));
		layout2->add(new wz::Button("A button", "../data/accept.png"));
		groupBox3->setContent(layout2);
		layout->add(groupBox3);
	}
	void createLabelFrame()
	{
		wz::Frame *frame = createFrame("Label");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		layout->add(new wz::Label("Normal label"));
		wz::Label *multilineLabel = new wz::Label("Multiline label with color. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
		multilineLabel->setMultiline(true);
		multilineLabel->setTextColor(0, 0.5f, 0);
		multilineLabel->setWidth(400);
		layout->add(multilineLabel);
		wz::Label *customLabel = new wz::Label("Label with custom font and color");
		customLabel->setTextColor(1, 0.5f, 0.5f);
		customLabel->setFont("visitor1", 32);
		layout->add(customLabel);
	}
	void createListFrame()
	{
		wz::Frame *frame = createFrame("List");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Horizontal);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::List *list1 = new wz::List((uint8_t *)listData, sizeof(const char *), 17);
		list1->setSize(120, 200);
		layout->add(list1);
		wz::List *list2 = new wz::List((uint8_t *)listData, sizeof(const char *), 17);
		list2->setSize(240, 300);
		list2->setFont("visitor1", 32);
		layout->add(list2);
		wz::List *list3 = new wz::List((uint8_t *)customListData, sizeof(const char *), 3);
		list3->setItemHeight(40);
		list3->setDrawItemCallback(CustomDrawListItemCallback);
		list3->setSize(50, 200);
		layout->add(list3);
	}
	void createRadioButtonFrame()
	{
		wz::Frame *frame = createFrame("Radio Button");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		layout->add(new wz::RadioButton("Option 1"));
		layout->add(new wz::RadioButton("Option 2"));
		layout->add(new wz::RadioButton("Option 3"));
	}
	void createScrollerFrame()
	{
		wz::Frame *frame = createFrame("Scroller");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::Scroller *scroller1 = new wz::Scroller(wz::ScrollerDirection::Vertical);
		scroller1->setMaxValue(100);
		scroller1->setValue(20);
		scroller1->setStepValue(10);
		scroller1->setHeight(200);
		layout->add(scroller1);
		wz::Scroller *scroller2 = new wz::Scroller(wz::ScrollerDirection::Horizontal);
		scroller2->setMaxValue(100);
		scroller2->setValue(50);
		scroller2->setStepValue(10);
		scroller2->setWidth(200);
		layout->add(scroller2);
	}
	void createSpinnerFrame()
	{
		wz::Frame *frame = createFrame("Spinner");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::Spinner *spinner = new wz::Spinner();
		spinner->setValue(42);
		layout->add(spinner);
		wz::Spinner *spinner2 = new wz::Spinner();
		spinner2->setFont("visitor1", 32);
		layout->add(spinner2);
	}
	void createTabbedFrame()
	{
		wz::Frame *frame = createFrame("Tabbed");
		wz::Tabbed *tabbed = new wz::Tabbed();
		tabbed->setMargin(8);
		tabbed->setStretch(wz::Stretch::All);
		frame->add(tabbed);
		tabbed->add(new wz::Tab("Tab 1"));
		tabbed->add(new wz::Tab("With icon", "../data/accept.png"));
		tabbed->add(new wz::Tab("Another Tab"));
	}
	void createTextEditFrame()
	{
		wz::Frame *frame = createFrame("Text Edit");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		frame->add(layout);
		wz::TextEdit *textEdit1 = new wz::TextEdit(false, "this is a very long string so scrolling can be tested");
		textEdit1->setWidth(300);
		layout->add(textEdit1);
		wz::TextEdit *textEdit2 = new wz::TextEdit(false, "text edit with a custom font");
		textEdit2->setFont("visitor1", 32);
		textEdit2->setStretch(wz::Stretch::Width);
		layout->add(textEdit2);
		wz::TextEdit *textEdit3 = new wz::TextEdit(true, "NanoVG is small antialiased vector graphics rendering library for OpenGL. It has lean API modeled after HTML5 canvas API. It is aimed to be a practical and fun toolset for building scalable user interfaces and visualizations.");
		textEdit3->setSize(200, 100);
		layout->add(textEdit3);
	}
	void createWindowFrame()
	{
		wz::Frame *frame = createFrame("Window");
		wz::StackLayout *frameLayout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		frameLayout->setSpacing(8);
		frameLayout->setMargin(8);
		frameLayout->setStretch(wz::Stretch::All);
		frame->add(frameLayout);
		wz::CheckBox *showWindow1 = new wz::CheckBox("Show Window 1");
		showWindow1->addEventHandler(wz::EventType::ButtonClicked, this, &GUI::showWindow1Toggled);
		frameLayout->add(showWindow1);
		wz::CheckBox *showWindow2 = new wz::CheckBox("Show Window 2");
		showWindow2->addEventHandler(wz::EventType::ButtonClicked, this, &GUI::showWindow2Toggled);
		frameLayout->add(showWindow2);
	}
	void createParentLayoutFrame()
	{
		wz::Frame *rootFrame = createFrame("Parent Layout");
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		rootFrame->add(layout);
		{
			wz::Frame *frame = new wz::Frame();
			frame->setStretch(wz::Stretch::All);
			layout->add(frame);
			wz::Button *button = new wz::Button("Align Left");
			button->setAlign(wz::Align::Left);
			frame->add(button);
			button = new wz::Button("Align Center");
			button->setAlign(wz::Align::Center);
			frame->add(button);
			button = new wz::Button("Align Right");
			button->setAlign(wz::Align::Right);
			frame->add(button);
		}
		{
			wz::Frame *frame = new wz::Frame();
			frame->setStretch(wz::Stretch::All);
			layout->add(frame);
			wz::Button *button = new wz::Button("Align Top");
			button->setAlign(wz::Align::Top);
			frame->add(button);
			button = new wz::Button("Align Middle");
			button->setAlign(wz::Align::Middle);
			frame->add(button);
			button = new wz::Button("Align Bottom");
			button->setAlign(wz::Align::Bottom);
			frame->add(button);
		}
	}
	void createStackLayoutFrame()
	{
		wz::Frame *frame = createFrame("Stack Layout");
		wz::StackLayout *frameLayout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		frameLayout->setSpacing(8);
		frameLayout->setMargin(8);
		frameLayout->setStretch(wz::Stretch::All);
		frame->add(frameLayout);
		{
			wz::Label *label = new wz::Label("Horizontal Stack Layout");
			frameLayout->add(label);
			wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Horizontal);
			layout->setSpacing(8);
			layout->setStretch(wz::Stretch::Width);
			layout->setHeight(100);
			frameLayout->add(layout);
			wz::Button *button = new wz::Button("Default");
			layout->add(button);
			button = new wz::Button("Align Top");
			button->setAlign(wz::Align::Top);
			layout->add(button);
			button = new wz::Button("Align Middle");
			button->setAlign(wz::Align::Middle);
			layout->add(button);
			button = new wz::Button("Align Bottom");
			button->setAlign(wz::Align::Bottom);
			layout->add(button);
			button = new wz::Button("Stretch Width");
			button->setStretch(wz::Stretch::Width);
			layout->add(button);
			button = new wz::Button("Stretch Height");
			button->setStretch(wz::Stretch::Height);
			layout->add(button);
		}
		{
			wz::Label *label = new wz::Label("Vertical Stack Layout");
			label->setMargin(16, 0, 0, 0);
			frameLayout->add(label);
			wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
			layout->setSpacing(8);
			layout->setStretch(wz::Stretch::Height);
			layout->setWidth(300);
			frameLayout->add(layout);
			wz::Button *button = new wz::Button("Default");
			layout->add(button);
			button = new wz::Button("Align Left");
			button->setAlign(wz::Align::Left);
			layout->add(button);
			button = new wz::Button("Align Center");
			button->setAlign(wz::Align::Center);
			layout->add(button);
			button = new wz::Button("Align Right");
			button->setAlign(wz::Align::Right);
			layout->add(button);
			button = new wz::Button("Stretch Width");
			button->setStretch(wz::Stretch::Width);
			layout->add(button);
			button = new wz::Button("Stretch Height");
			button->setStretch(wz::Stretch::Height);
			layout->add(button);
		}
	}
	void createWidgetCategoryWindow()
	{
		wz::Window *window = new wz::Window("Widgets");
		window->setWidth(200);
		mainWindow.add(window);
		mainWindow.dockWindow(window, wz::DockPosition::West);
		wz::List *list = new wz::List((uint8_t *)&widgetCategories[0], sizeof(WidgetCategoryListItem), widgetCategories.size());
		list->setMargin(8);
		list->setStretch(wz::Stretch::All);
		list->setFontSize(18);
		list->setSelectedItem(0);
		list->addEventHandler(wz::EventType::ListItemSelected, this, &GUI::widgetCategoryChanged);
		window->add(list);
	}
	void createWindow1()
	{
		window1 = new wz::Window("Test Window");
		window1->setRect(650, 100, 300, 300);
		window1->setVisible(false);
		mainWindow.add(window1);
		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
		layout->setSpacing(8);
		layout->setMargin(8);
		layout->setStretch(wz::Stretch::All);
		window1->add(layout);
		wz::TextEdit *textEdit = new wz::TextEdit(false, "this is a very long string so scrolling can be tested");
		textEdit->setStretch(wz::Stretch::Width);
		layout->add(textEdit);
		wz::Button *button = new wz::Button("Another Button", "../data/accept.png");
		button->setStretch(wz::Stretch::Width);
		layout->add(button);
		wz::CheckBox *checkbox = new wz::CheckBox("CheckBox");
		checkbox->setAlign(wz::Align::Center);
		layout->add(checkbox);
		wz::Combo *combo = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
		combo->setAlign(wz::Align::Right);
		combo->setFont("visitor1", 12);
		layout->add(combo);
		wz::Button *button2 = new wz::Button("Yet Another Button");
		button2->setStretch(wz::Stretch::All);
		layout->add(button2);
	}
	void createWindow2()
	{
		window2 = new wz::Window("Window with a long title");
		window2->setRect(590, 500, 200, 200);
		window2->setVisible(false);
		mainWindow.add(window2);
		wz::Tabbed *tabbed = new wz::Tabbed();
		tabbed->setMargin(8);
		tabbed->setStretch(wz::Stretch::All);
		window2->add(tabbed);
		wz::Tab *firstTab = new wz::Tab("Tab 1");
		tabbed->add(firstTab);
		wz::Tab *secondTab = new wz::Tab("Another Tab");
		tabbed->add(secondTab);
		tabbed->add(new wz::Tab("With icon", "../data/accept.png"));
		wz::Combo *combo = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
		combo->setPosition(10, 10);
		firstTab->add(combo);
		secondTab->add(new wz::Button("Button Button Button"))->setPosition(10, 10);
	}
	void showWindow1Toggled(wz::Event e)
	{
		window1->setVisible(e.button.isSet);
	}
	void showWindow2Toggled(wz::Event e)
	{
		window2->setVisible(e.button.isSet);
	}
	void widgetCategoryChanged(wz::Event e)
	{
		setFrame(e.list.selectedItem);
	}
	void setFrame(int index)
	{
		for (size_t i = 0; i < widgetCategories.size(); i++)
		{
			widgetCategories[i].frame->setVisible((int)i == index);
		}
	}
	struct WidgetCategoryListItem
	{
		const char *label;
		wz::Frame *frame;
	};
	wz::Window *window1, *window2;
	std::vector<WidgetCategoryListItem> widgetCategories;
	bool showProfiling_;
};
static void ShowError(const char *message)
{
	fprintf(stderr, "%s\n", message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);
}
static Uint32 TextCursorBlinkCallback(Uint32 interval, void *param)
{
	((wz::MainWindow *)param)->toggleTextCursor();

	SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.type = SDL_USEREVENT;
    ev.user.code = 0;
    ev.user.data1 = NULL;
    ev.user.data2 = NULL;
    SDL_PushEvent(&ev);
    return interval;
}
static wz::Key::Enum ConvertKey(SDL_Keycode sym)
{
	static int keys[] =
	{
		SDLK_LEFT, wz::Key::LeftArrow,
		SDLK_RIGHT, wz::Key::RightArrow,
		SDLK_UP, wz::Key::UpArrow,
		SDLK_DOWN, wz::Key::DownArrow, 
		SDLK_HOME, wz::Key::Home,
		SDLK_END, wz::Key::End,
		SDLK_RETURN, wz::Key::Enter,
		SDLK_RETURN, wz::Key::Enter,
		SDLK_KP_ENTER, wz::Key::Enter,
		SDLK_DELETE, wz::Key::Delete,
		SDLK_BACKSPACE, wz::Key::Backspace,
		SDLK_LSHIFT, wz::Key::LeftShift,
		SDLK_RSHIFT, wz::Key::RightShift,
		SDLK_LCTRL, wz::Key::LeftControl,
		SDLK_RCTRL, wz::Key::RightControl
	};
	for (size_t i = 0; i < sizeof(keys) / sizeof(int); i += 2)
	{
		if (keys[i] == sym)
		{
			int key = keys[i + 1];

			if (SDL_GetModState() & KMOD_SHIFT)
			{
				key |= wz::Key::ShiftBit;
			}
			if (SDL_GetModState() & KMOD_CTRL)
			{
				key |= wz::Key::ControlBit;
			}
			return (wz::Key::Enum)key;
		}
	}
	return wz::Key::Unknown;
}



// int HTTP_GET(CURL *curl_handle, char *url, char *header[], int header_length) {
//     if (!curl_handle) {
//         return CURLE_FAILED_INIT;
//     }
//     struct curl_slist *list = NULL;
//     for (int i = 0; i < header_length; ++i) {
//         list = curl_slist_append(list, header[i]);
//     }
//     curl_easy_setopt(curl_handle, CURLOPT_URL, url);
//     curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
//     curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
//     curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
//     int res = curl_easy_perform(curl_handle);
//     curl_slist_free_all(list);
//     return res;
// }
// int HTTP_POST(CURL *curl_handle, char *url, char *data, int data_length, void *cb, void *userp) {
//     if (!curl_handle) {
//         return CURLE_FAILED_INIT;
//     }
//     curl_easy_setopt(curl_handle, CURLOPT_URL, url);
//     curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, 1L);
//     curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
//     curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long) data_length);
//     curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
//     if (cb) {
//         curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, cb);
//     }
//     if (userp) {
//         curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) userp);
//     }
//     int res = curl_easy_perform(curl_handle);
//     free(data);
//     return res;
// }

// struct memory {
//     char *response;
//     size_t size;
// };

// static size_t callBack(void *data, size_t size, size_t nmemb, void *userp) {
//     size_t realsize = size * nmemb;
//     struct memory *mem = (struct memory *) userp;
//     char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
//     if (ptr == NULL)
//         return 0;  /* out of memory! */
//     mem->response = ptr;
//     memcpy(&(mem->response[mem->size]), data, realsize);
//     mem->size += realsize;
//     mem->response[mem->size] = 0;
//     return realsize;
// }

// int getGoogleResult(CURL *curl_handle, char *src, char *dst, char *content, int content_length) {
//     struct memory chunk = {0};char front[] = {"<span id=\"tw-answ-target-text\">"};
//     char rear[] = {"</span><span id=\"tw-answ-target-text-feminine\">"};
//     char *url = "www.google.com/async/translate";
//     char *value = "async=translate,sl:%s,tl:%s,st:%s,id:1672056488960,qc:true,ac:true,_id:tw-async-translate,_pms:s,_fmt:pc";
//     char *encode_content = curl_easy_escape(curl_handle, content, content_length);
//     int data_length = strlen(value) + strlen(dst) + strlen(encode_content) + strlen(src) - strlen("%s") * 3;
//     char *data = (char *)malloc(data_length);
//     sprintf(data, value, src, dst, encode_content);
//     HTTP_POST(curl_handle, url, data, data_length, (void *)&callBack,  (void *)&chunk);
//     char *start = strstr(chunk.response,front) + strlen(front);
//     char *end = strstr(chunk.response,rear);
//     char * p = end;
//     *p = '\0';
//     printf("������:\n");
//     printf("%s\n",start);
//     return 0;
// }

struct StopWatch {
  StopWatch(const string &label) : label_(label) {
    start_ = chrono::system_clock::now();
  }
  ~StopWatch() {
    auto end = chrono::system_clock::now();
    auto diff = end - start_;
    auto count = chrono::duration_cast<chrono::milliseconds>(diff).count();
    cout << label_ << ": " << count << " millisec." << endl;
  }
  string label_;
  chrono::system_clock::time_point start_;
};

// std::string translate(std::string str) {
// 	std::srand(time(0));
// 	std::string from = "en", to = "zh", appid = "20240419002029316", salt = std::to_string(rand()), key = "wV4UyQo7nPAYd1YMUled";
// 	std::string temURL = "http://api.fanyi.baidu.com";
// 	std::string sign = appid + str + salt + key;
// 	sign = MD5(sign).toStr();
// 	std::string URL = "/api/trans/vip/translate?q=" + str + "&from=" + from + "&to=" + to + "&appid=" + appid + "&salt=" + salt + "&sign=" + sign;
// 	httplib::Client cli(temURL);
// 	auto res = cli.Get(URL);
// 	if (res->status != 200)return "internet error:" + std::to_string(res->status);
// 	std::string ret = res->body;
// 	nlohmann::json js = nlohmann::json::parse(ret);
// 	if (js.find("error_code") != js.end())return "error_code:" + js["error_code"].get<std::string>() + ",error_msg:" + js["error_msg"].get<std::string>();
// 	ret = js["trans_result"][0]["dst"].get<std::string>();
// 	return ret;
// }

// std::string readchinese()
// {
// 	std::wstring s;
// 	std::wcin >> s;
// 	std::string ret = to_byte_string(s);
// 	return ret;
// }

#undef main
int main(int argc, char *argv[]) {

    // if (argc < 2) {
    //     printf("������Ҫ������ı�!!!\n");
    //     return 0;
    // }

    // LoggerInit();
    // printf("\nERROR, WARNING, SUCCESS LOGS\n");
    // printf("========\n");
    // LoggerSetLevel(LOGGER_LEVEL_ERROR | LOGGER_LEVEL_WARNING |  LOGGER_LEVEL_SUCCESS);
    // LoggerError("Error one : %d two : %f three : %s", 1, 2.0, "three");
    // LoggerWarning("warning one : %d two : %f three : %s", 1, 2.0, "three");
    // LoggerSuccess("success one : %d two : %f three : %s", 1, 2.0, "three");
    // LoggerInfo("Info one : %d two : %f three : %s", 1, 2.0, "three");
    // LoggerDebug("Debug one : %d two : %f three : %s", 1, 2.0, "three");
    // printf("\nALL LOGS\n");
    // printf("========\n");
    // LoggerSetLevel(LOGGER_LEVEL_ALL);
    // LoggerError("assets path not found %s", "../assets/image.png");
    // LoggerWarning("retry fetch, network error");
    // LoggerSuccess("data fetched %dms", 230);
    // LoggerInfo("init queue");
    // LoggerDebug("user email is : %s", "user@test.test");
    // LoggerClose();
    
    // test_pool();
    // test_mempool();
    // test_malloc();

	// printf("������Ҫ������ı�!!!\n");

    // httplib::Server svr;
    // svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
    //     res.set_content("Hello World!", "text/plain");
    //     // break;
    // });
    // svr.listen("0.0.0.0", 8080);
    
    
    //�޲ε���POST�ӿ�
    //httplib::Result Client::Post(const char *path);
    //����������POST�ӿڣ� ����һ����Json��ʽ��
    //httplib::Result Client::Post(const char *path, const Params &params);	
    //��Header����������POST�ӿ�
    //httplib::Result Client::Post(const char *path, const Headers &headers,const Params &params);
    //����Header��������������POST�ӿ�
    //httplib::Result Client::Post(const char *path, const MultipartFormDataItems &items);
    //��Header��������������Post����
    //httplib::Result Client::Post(const char *path, const Headers &headers, const MultipartFormDataItems &items); 

    string body(1024 * 5, 'a');
    httplib::Client clip("httpbin.org", 80);
    httplib::Params params;
    params.emplace("name", "john");
    params.emplace("note", "coder");
    for (int i = 0; i < 1; i++) {
        StopWatch sw(to_string(i).c_str());
        // auto res = clip.Post("/post", params, "application/octet-stream");
        auto res = clip.Post("/post", body, "application/octet-stream");
        assert(res->status == httplib::StatusCode::OK_200);
    }

    httplib::Client clig("httpbin.org"); 
    auto res = clig.Get("/get"); 
    if (res && res->status == 200) { 
        // cout << "Response body:" << endl; 
		printf("Response body:\n");
        for (auto &line : res->body) { 
        	//cout << line << endl;
			cout << line;  
			// printf("%s",line);
        } 
		printf("\n");
    } else { 
        cerr << "Request failed!" << endl; 
    } 

    // httplib::Headers header={
    //     { "Accept-Encoding", "gzip, deflate" },
    //     {"User-Agent","Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36"},
    //     {"Content-type", "application/x-www-form-urlencoded;charset=UTF-8"}
    // };

    // SetConsoleOutputCP(CP_UTF8);
    // translate("��˵�ĸ����˹ھ������Ŀ������������յ�");

    // int content_length = 0;
    // for (int i = 1; i < argc; ++i) {
    //     content_length+= strlen(argv[i]);
    //     printf("%s\n",argv[i]);
    // }
    // char * content = (char *) malloc(content_length);
    // for (int i = 1; i < argc; ++i) {
    //     strcat(content, argv[i]);
    //     printf("%s\n",argv[i]);
    //     strcat(content, " ");
    // }
    // CURL *curl_handle;
    // curl_global_init(CURL_GLOBAL_ALL);
    // curl_handle = curl_easy_init();
    // struct curl_slist *list = NULL;
    // char *header[] = {
    //         "User-Agent:Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36",
    //         "Content-type: application/x-www-form-urlencoded;charset=UTF-8"};
    // for (int i = 0; i < sizeof(header) / sizeof(header[0]); ++i) {
    //     list = curl_slist_append(list, header[i]);
    // }
    // curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
    // getGoogleResult(curl_handle, "en", "zh-CN", content, content_length);


    // Tigr* win1 = tigrWindow(200, 150, "Window1", 0);
    // Tigr* win2 = tigrWindow(200, 150, "Window2", 0);
    // while (win1 || win2) {
    //     if (win1) {
    //         tigrClear(win1, tigrRGB(0x80, 0x90, 0xa0));
    //         tigrPrint(win1, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #1.");
    //         tigrUpdate(win1);
    //         if (tigrClosed(win1) || tigrKeyDown(win1, TK_ESCAPE)) {
    //             tigrFree(win1);
    //             win1 = NULL;
    //         }
    //     }
    //     if (win2) {
    //         if (tigrBeginOpenGL(win2)) {
    //             glClearColor(1, 0, 1, 1);
    //             glClear(GL_COLOR_BUFFER_BIT);
    //         }
    //         tigrClear(win2, tigrRGBA(0x00, 0x00, 0x00, 0x00));
    //         tigrPrint(win2, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #2.");
    //         tigrUpdate(win2);
    //         if (tigrClosed(win2) || tigrKeyDown(win2, TK_ESCAPE)) {
    //             tigrFree(win2);
    //             win2 = NULL;
    //         }
    //     }
    // }
    // if (win1)
    //     tigrFree(win1);
    // if (win2)
    //     tigrFree(win2);


    //Create the tigr window.
	Tigr *tigrScreen = tigrWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WidgetZero Example - Custom Renderer", 0);
	// Create the custom renderer.
	TigrRenderer *tigrRenderer = new TigrRenderer(tigrScreen);
	// Create the main window.
	wz::MainWindow *mainWindow = new wz::MainWindow(tigrRenderer);
	mainWindow->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	// Create a button.
	wz::Button *button = new wz::Button("Click me!");
	button->setAlign(wz::Align::Center | wz::Align::Middle);
	mainWindow->add(button);
	// For tracking input state changes.
	int lastMouseX = 0, lastMouseY = 0, lastMouseButtons = 0;
	while (!tigrClosed(tigrScreen))
	{
		// Handle mouse movement.
		int mouseX, mouseY, mouseButtons;
		tigrMouse(tigrScreen, &mouseX, &mouseY, &mouseButtons);
		if (mouseX != lastMouseX || mouseY != lastMouseY)
		{
			mainWindow->mouseMove(mouseX, mouseY, mouseX - lastMouseX, mouseY - lastMouseY);
		}
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		lastMouseButtons = mouseButtons;
		// Draw.
		tigrClear(tigrScreen, tigrRGB(192, 192, 192));
		mainWindow->draw();
		tigrUpdate(tigrScreen);
	}
	tigrFree(tigrScreen);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		ShowError(SDL_GetError());
		SDL_ClearError();
		return 1;
	}
	atexit(SDL_Quit);
	SDL_Window *window = SDL_CreateWindow("WidgetZero Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		ShowError(SDL_GetError());
		SDL_ClearError();
		return 1;
	}
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext)
	{
		ShowError(SDL_GetError());
		SDL_ClearError();
		return 1;
	}
#ifdef _MSC_VER
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		ShowError("ogl_LoadFunctions failed");
		return 1;
	}
#endif
	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	// Setup cursors.
	SDL_Cursor *cursors[wz::Cursor::NumCursors];
	cursors[wz::Cursor::Default] = SDL_GetDefaultCursor();
	cursors[wz::Cursor::Ibeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	cursors[wz::Cursor::Resize_N_S] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	cursors[wz::Cursor::Resize_E_W] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	cursors[wz::Cursor::Resize_NE_SW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	cursors[wz::Cursor::Resize_NW_SE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	// Create the renderer.
	wz::NVGRenderer *renderer = new wz::NVGRenderer(nvgCreateGL2, nvgDeleteGL2, 0, "../data", "DejaVuSans", 16.0f);
	if (renderer->getError())
	{
		ShowError(renderer->getError());
		return 1;
	}
	const wz::Color clearColor = renderer->getClearColor();
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	GUI gui(windowWidth, windowHeight, renderer);
	const SDL_TimerID textCursorTimer = SDL_AddTimer(textCursorBlinkInterval, TextCursorBlinkCallback, &gui.mainWindow);
	for (;;)
	{
		SDL_Event e;
		SDL_WaitEvent(&e);
		if (e.type == SDL_QUIT)
		{
			SDL_RemoveTimer(textCursorTimer);
			break;
		}
		else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			glViewport(0, 0, e.window.data1, e.window.data2);
			gui.mainWindow.setSize(e.window.data1, e.window.data2);
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			gui.mainWindow.mouseMove(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			gui.mainWindow.mouseButtonDown(e.button.button, e.button.x, e.button.y);
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			gui.mainWindow.mouseButtonUp(e.button.button, e.button.x, e.button.y);
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			gui.mainWindow.mouseWheelMove(e.wheel.x, e.wheel.y);
		}
		else if (e.type == SDL_KEYDOWN)
		{
			gui.mainWindow.keyDown(ConvertKey(e.key.keysym.sym));
		}
		else if (e.type == SDL_KEYUP)
		{
			gui.mainWindow.keyUp(ConvertKey(e.key.keysym.sym));
		}
		else if (e.type == SDL_TEXTINPUT)
		{
			gui.mainWindow.textInput(e.text.text);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		gui.mainWindow.drawFrame();
		SDL_GL_SwapWindow(window);
		SDL_SetCursor(cursors[gui.mainWindow.getCursor()]);
	}
	delete renderer;

    return 0;
}
