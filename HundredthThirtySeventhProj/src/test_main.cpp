/* VLD (memory leak detector)  */
#ifdef _WIN32
#  define _USE_VLD 0
#  if _USE_VLD == 1
#    pragma comment(lib, "D:/CPP_INCLUDE/vld-2.7.0/lib/Win64/vld.lib")
#    include <D:/CPP_INCLUDE/vld-2.7.0/include/vld.h>
#  endif // _USE_VLD
#endif // _WIN32


#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale.h>
#include <random>
#include <string>
#include <vector>
#include <chrono>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/ioctl.h>
#  include <unistd.h>
#endif


#include "iclient.h"
#include "md5.h"
#include "json.hpp"
#include "tigr.h"
// #include "SDL.h"
#include "glad/glad.h"
#include "wz.h"

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 150
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
// class GUI
// {
// public:
// 	GUI(int windowWidth, int windowHeight, wz::IRenderer *renderer) : mainWindow(renderer, wz::MainWindowFlags::DockingEnabled | wz::MainWindowFlags::MenuEnabled)
// 	{
// 		mainWindow.setSize(windowWidth, windowHeight);
// 		mainWindow.createMenuButton("File");
// 		mainWindow.createMenuButton("Edit");
// 		mainWindow.createMenuButton("View");
// 		mainWindow.createMenuButton("Options");
// 		mainWindow.createMenuButton("Window");
// 		createButtonFrame();
// 		createCheckBoxFrame();
// 		createComboFrame();
// 		createGroupBoxFrame();
// 		createLabelFrame();
// 		createListFrame();
// 		createRadioButtonFrame();
// 		createScrollerFrame();
// 		createSpinnerFrame();
// 		createTabbedFrame();
// 		createTextEditFrame();
// 		createWindowFrame();
// 		createParentLayoutFrame();
// 		createStackLayoutFrame();
// 		createWidgetCategoryWindow();
// 		createWindow1();
// 		createWindow2();
// 		setFrame(0);
// 	}
// 	wz::MainWindow mainWindow;
// private:
// 	wz::Frame *createFrame(const char *label)
// 	{
// 		wz::Frame *frame = new wz::Frame();
// 		frame->setStretch(wz::Stretch::All);
// 		mainWindow.add(frame);
// 		WidgetCategoryListItem category;
// 		category.label = label;
// 		category.frame = frame;
// 		widgetCategories.push_back(category);
// 		return frame;
// 	}
// 	void createButtonFrame()
// 	{
// 		wz::Frame *frame = createFrame("Button");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		layout->add(new wz::Button("Button with a label"));
// 		layout->add(new wz::Button("Button with a label and icon", "../data/accept.png"));
// 		layout->add(new wz::Button("", "../data/accept.png"));
// 		wz::Button *paddedButton = new wz::Button("Custom padding");
// 		paddedButton->setPadding(20, 40, 20, 40);
// 		layout->add(paddedButton);
// 		wz::ToggleButton *toggleButton = new wz::ToggleButton("Toggle Button", std::string());
// 		layout->add(toggleButton);
// 	}
// 	void createCheckBoxFrame()
// 	{
// 		wz::Frame *frame = createFrame("CheckBox");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::CheckBox *checkbox = new wz::CheckBox("Toggle me!");
// 		layout->add(checkbox);
// 	}
// 	void createComboFrame()
// 	{
// 		wz::Frame *frame = createFrame("Combo");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::Combo *combo1 = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
// 		combo1->setPosition(800, 50);
// 		layout->add(combo1);
// 		wz::Combo *combo2 = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
// 		combo2->setFont("visitor1", 12);
// 		layout->add(combo2);
// 	}
// 	void createGroupBoxFrame()
// 	{
// 		wz::Frame *frame = createFrame("GroupBox");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::GroupBox *groupBox1 = new wz::GroupBox("With a Label");
// 		groupBox1->setContent(new wz::Label("Content gones here"));
// 		layout->add(groupBox1);
// 		wz::GroupBox *groupBox2 = new wz::GroupBox();
// 		groupBox2->setContent(new wz::Label("Without a label"));
// 		layout->add(groupBox2);
// 		wz::GroupBox *groupBox3 = new wz::GroupBox("With some more content");
// 		wz::StackLayout *layout2 = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout2->setSpacing(8);
// 		layout2->add(new wz::Label("A label"));
// 		layout2->add(new wz::RadioButton("Option 1"));
// 		layout2->add(new wz::RadioButton("Option 2"));
// 		layout2->add(new wz::RadioButton("Option 3"));
// 		layout2->add(new wz::Button("A button", "../data/accept.png"));
// 		groupBox3->setContent(layout2);
// 		layout->add(groupBox3);
// 	}
// 	void createLabelFrame()
// 	{
// 		wz::Frame *frame = createFrame("Label");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		layout->add(new wz::Label("Normal label"));
// 		wz::Label *multilineLabel = new wz::Label("Multiline label with color. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
// 		multilineLabel->setMultiline(true);
// 		multilineLabel->setTextColor(0, 0.5f, 0);
// 		multilineLabel->setWidth(400);
// 		layout->add(multilineLabel);
// 		wz::Label *customLabel = new wz::Label("Label with custom font and color");
// 		customLabel->setTextColor(1, 0.5f, 0.5f);
// 		customLabel->setFont("visitor1", 32);
// 		layout->add(customLabel);
// 	}
// 	void createListFrame()
// 	{
// 		wz::Frame *frame = createFrame("List");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Horizontal);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::List *list1 = new wz::List((uint8_t *)listData, sizeof(const char *), 17);
// 		list1->setSize(120, 200);
// 		layout->add(list1);
// 		wz::List *list2 = new wz::List((uint8_t *)listData, sizeof(const char *), 17);
// 		list2->setSize(240, 300);
// 		list2->setFont("visitor1", 32);
// 		layout->add(list2);
// 		wz::List *list3 = new wz::List((uint8_t *)customListData, sizeof(const char *), 3);
// 		list3->setItemHeight(40);
// 		list3->setDrawItemCallback(CustomDrawListItemCallback);
// 		list3->setSize(50, 200);
// 		layout->add(list3);
// 	}
// 	void createRadioButtonFrame()
// 	{
// 		wz::Frame *frame = createFrame("Radio Button");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		layout->add(new wz::RadioButton("Option 1"));
// 		layout->add(new wz::RadioButton("Option 2"));
// 		layout->add(new wz::RadioButton("Option 3"));
// 	}
// 	void createScrollerFrame()
// 	{
// 		wz::Frame *frame = createFrame("Scroller");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::Scroller *scroller1 = new wz::Scroller(wz::ScrollerDirection::Vertical);
// 		scroller1->setMaxValue(100);
// 		scroller1->setValue(20);
// 		scroller1->setStepValue(10);
// 		scroller1->setHeight(200);
// 		layout->add(scroller1);
// 		wz::Scroller *scroller2 = new wz::Scroller(wz::ScrollerDirection::Horizontal);
// 		scroller2->setMaxValue(100);
// 		scroller2->setValue(50);
// 		scroller2->setStepValue(10);
// 		scroller2->setWidth(200);
// 		layout->add(scroller2);
// 	}
// 	void createSpinnerFrame()
// 	{
// 		wz::Frame *frame = createFrame("Spinner");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::Spinner *spinner = new wz::Spinner();
// 		spinner->setValue(42);
// 		layout->add(spinner);
// 		wz::Spinner *spinner2 = new wz::Spinner();
// 		spinner2->setFont("visitor1", 32);
// 		layout->add(spinner2);
// 	}
// 	void createTabbedFrame()
// 	{
// 		wz::Frame *frame = createFrame("Tabbed");
// 		wz::Tabbed *tabbed = new wz::Tabbed();
// 		tabbed->setMargin(8);
// 		tabbed->setStretch(wz::Stretch::All);
// 		frame->add(tabbed);
// 		tabbed->add(new wz::Tab("Tab 1"));
// 		tabbed->add(new wz::Tab("With icon", "../data/accept.png"));
// 		tabbed->add(new wz::Tab("Another Tab"));
// 	}
// 	void createTextEditFrame()
// 	{
// 		wz::Frame *frame = createFrame("Text Edit");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		frame->add(layout);
// 		wz::TextEdit *textEdit1 = new wz::TextEdit(false, "this is a very long string so scrolling can be tested");
// 		textEdit1->setWidth(300);
// 		layout->add(textEdit1);
// 		wz::TextEdit *textEdit2 = new wz::TextEdit(false, "text edit with a custom font");
// 		textEdit2->setFont("visitor1", 32);
// 		textEdit2->setStretch(wz::Stretch::Width);
// 		layout->add(textEdit2);
// 		wz::TextEdit *textEdit3 = new wz::TextEdit(true, "NanoVG is small antialiased vector graphics rendering library for OpenGL. It has lean API modeled after HTML5 canvas API. It is aimed to be a practical and fun toolset for building scalable user interfaces and visualizations.");
// 		textEdit3->setSize(200, 100);
// 		layout->add(textEdit3);
// 	}
// 	void createWindowFrame()
// 	{
// 		wz::Frame *frame = createFrame("Window");
// 		wz::StackLayout *frameLayout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		frameLayout->setSpacing(8);
// 		frameLayout->setMargin(8);
// 		frameLayout->setStretch(wz::Stretch::All);
// 		frame->add(frameLayout);
// 		wz::CheckBox *showWindow1 = new wz::CheckBox("Show Window 1");
// 		showWindow1->addEventHandler(wz::EventType::ButtonClicked, this, &GUI::showWindow1Toggled);
// 		frameLayout->add(showWindow1);
// 		wz::CheckBox *showWindow2 = new wz::CheckBox("Show Window 2");
// 		showWindow2->addEventHandler(wz::EventType::ButtonClicked, this, &GUI::showWindow2Toggled);
// 		frameLayout->add(showWindow2);
// 	}
// 	void createParentLayoutFrame()
// 	{
// 		wz::Frame *rootFrame = createFrame("Parent Layout");
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		rootFrame->add(layout);
// 		{
// 			wz::Frame *frame = new wz::Frame();
// 			frame->setStretch(wz::Stretch::All);
// 			layout->add(frame);
// 			wz::Button *button = new wz::Button("Align Left");
// 			button->setAlign(wz::Align::Left);
// 			frame->add(button);
// 			button = new wz::Button("Align Center");
// 			button->setAlign(wz::Align::Center);
// 			frame->add(button);
// 			button = new wz::Button("Align Right");
// 			button->setAlign(wz::Align::Right);
// 			frame->add(button);
// 		}
// 		{
// 			wz::Frame *frame = new wz::Frame();
// 			frame->setStretch(wz::Stretch::All);
// 			layout->add(frame);
// 			wz::Button *button = new wz::Button("Align Top");
// 			button->setAlign(wz::Align::Top);
// 			frame->add(button);
// 			button = new wz::Button("Align Middle");
// 			button->setAlign(wz::Align::Middle);
// 			frame->add(button);
// 			button = new wz::Button("Align Bottom");
// 			button->setAlign(wz::Align::Bottom);
// 			frame->add(button);
// 		}
// 	}
// 	void createStackLayoutFrame()
// 	{
// 		wz::Frame *frame = createFrame("Stack Layout");
// 		wz::StackLayout *frameLayout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		frameLayout->setSpacing(8);
// 		frameLayout->setMargin(8);
// 		frameLayout->setStretch(wz::Stretch::All);
// 		frame->add(frameLayout);
// 		{
// 			wz::Label *label = new wz::Label("Horizontal Stack Layout");
// 			frameLayout->add(label);
// 			wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Horizontal);
// 			layout->setSpacing(8);
// 			layout->setStretch(wz::Stretch::Width);
// 			layout->setHeight(100);
// 			frameLayout->add(layout);
// 			wz::Button *button = new wz::Button("Default");
// 			layout->add(button);
// 			button = new wz::Button("Align Top");
// 			button->setAlign(wz::Align::Top);
// 			layout->add(button);
// 			button = new wz::Button("Align Middle");
// 			button->setAlign(wz::Align::Middle);
// 			layout->add(button);
// 			button = new wz::Button("Align Bottom");
// 			button->setAlign(wz::Align::Bottom);
// 			layout->add(button);
// 			button = new wz::Button("Stretch Width");
// 			button->setStretch(wz::Stretch::Width);
// 			layout->add(button);
// 			button = new wz::Button("Stretch Height");
// 			button->setStretch(wz::Stretch::Height);
// 			layout->add(button);
// 		}
// 		{
// 			wz::Label *label = new wz::Label("Vertical Stack Layout");
// 			label->setMargin(16, 0, 0, 0);
// 			frameLayout->add(label);
// 			wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 			layout->setSpacing(8);
// 			layout->setStretch(wz::Stretch::Height);
// 			layout->setWidth(300);
// 			frameLayout->add(layout);
// 			wz::Button *button = new wz::Button("Default");
// 			layout->add(button);
// 			button = new wz::Button("Align Left");
// 			button->setAlign(wz::Align::Left);
// 			layout->add(button);
// 			button = new wz::Button("Align Center");
// 			button->setAlign(wz::Align::Center);
// 			layout->add(button);
// 			button = new wz::Button("Align Right");
// 			button->setAlign(wz::Align::Right);
// 			layout->add(button);
// 			button = new wz::Button("Stretch Width");
// 			button->setStretch(wz::Stretch::Width);
// 			layout->add(button);
// 			button = new wz::Button("Stretch Height");
// 			button->setStretch(wz::Stretch::Height);
// 			layout->add(button);
// 		}
// 	}
// 	void createWidgetCategoryWindow()
// 	{
// 		wz::Window *window = new wz::Window("Widgets");
// 		window->setWidth(200);
// 		mainWindow.add(window);
// 		mainWindow.dockWindow(window, wz::DockPosition::West);
// 		wz::List *list = new wz::List((uint8_t *)&widgetCategories[0], sizeof(WidgetCategoryListItem), widgetCategories.size());
// 		list->setMargin(8);
// 		list->setStretch(wz::Stretch::All);
// 		list->setFontSize(18);
// 		list->setSelectedItem(0);
// 		list->addEventHandler(wz::EventType::ListItemSelected, this, &GUI::widgetCategoryChanged);
// 		window->add(list);
// 	}
// 	void createWindow1()
// 	{
// 		window1 = new wz::Window("Test Window");
// 		window1->setRect(650, 100, 300, 300);
// 		window1->setVisible(false);
// 		mainWindow.add(window1);
// 		wz::StackLayout *layout = new wz::StackLayout(wz::StackLayoutDirection::Vertical);
// 		layout->setSpacing(8);
// 		layout->setMargin(8);
// 		layout->setStretch(wz::Stretch::All);
// 		window1->add(layout);
// 		wz::TextEdit *textEdit = new wz::TextEdit(false, "this is a very long string so scrolling can be tested");
// 		textEdit->setStretch(wz::Stretch::Width);
// 		layout->add(textEdit);
// 		wz::Button *button = new wz::Button("Another Button", "../data/accept.png");
// 		button->setStretch(wz::Stretch::Width);
// 		layout->add(button);
// 		wz::CheckBox *checkbox = new wz::CheckBox("CheckBox");
// 		checkbox->setAlign(wz::Align::Center);
// 		layout->add(checkbox);
// 		wz::Combo *combo = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
// 		combo->setAlign(wz::Align::Right);
// 		combo->setFont("visitor1", 12);
// 		layout->add(combo);
// 		wz::Button *button2 = new wz::Button("Yet Another Button");
// 		button2->setStretch(wz::Stretch::All);
// 		layout->add(button2);
// 	}
// 	void createWindow2()
// 	{
// 		window2 = new wz::Window("Window with a long title");
// 		window2->setRect(590, 500, 200, 200);
// 		window2->setVisible(false);
// 		mainWindow.add(window2);
// 		wz::Tabbed *tabbed = new wz::Tabbed();
// 		tabbed->setMargin(8);
// 		tabbed->setStretch(wz::Stretch::All);
// 		window2->add(tabbed);
// 		wz::Tab *firstTab = new wz::Tab("Tab 1");
// 		tabbed->add(firstTab);
// 		wz::Tab *secondTab = new wz::Tab("Another Tab");
// 		tabbed->add(secondTab);
// 		tabbed->add(new wz::Tab("With icon", "../data/accept.png"));
// 		wz::Combo *combo = new wz::Combo((uint8_t *)listData, sizeof(const char *), 17);
// 		combo->setPosition(10, 10);
// 		firstTab->add(combo);
// 		secondTab->add(new wz::Button("Button Button Button"))->setPosition(10, 10);
// 	}
// 	void showWindow1Toggled(wz::Event e)
// 	{
// 		window1->setVisible(e.button.isSet);
// 	}
// 	void showWindow2Toggled(wz::Event e)
// 	{
// 		window2->setVisible(e.button.isSet);
// 	}
// 	void widgetCategoryChanged(wz::Event e)
// 	{
// 		setFrame(e.list.selectedItem);
// 	}
// 	void setFrame(int index)
// 	{
// 		for (size_t i = 0; i < widgetCategories.size(); i++)
// 		{
// 			widgetCategories[i].frame->setVisible((int)i == index);
// 		}
// 	}
// 	struct WidgetCategoryListItem
// 	{
// 		const char *label;
// 		wz::Frame *frame;
// 	};
// 	wz::Window *window1, *window2;
// 	std::vector<WidgetCategoryListItem> widgetCategories;
// 	bool showProfiling_;
// };
// static void ShowError(const char *message)
// {
// 	fprintf(stderr, "%s\n", message);
// 	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);
// }
// static Uint32 TextCursorBlinkCallback(Uint32 interval, void *param)
// {
// 	((wz::MainWindow *)param)->toggleTextCursor();

// 	SDL_Event ev;
//     ev.type = SDL_USEREVENT;
//     ev.user.type = SDL_USEREVENT;
//     ev.user.code = 0;
//     ev.user.data1 = NULL;
//     ev.user.data2 = NULL;
//     SDL_PushEvent(&ev);
//     return interval;
// }
// static wz::Key::Enum ConvertKey(SDL_Keycode sym)
// {
// 	static int keys[] =
// 	{
// 		SDLK_LEFT, wz::Key::LeftArrow,
// 		SDLK_RIGHT, wz::Key::RightArrow,
// 		SDLK_UP, wz::Key::UpArrow,
// 		SDLK_DOWN, wz::Key::DownArrow, 
// 		SDLK_HOME, wz::Key::Home,
// 		SDLK_END, wz::Key::End,
// 		SDLK_RETURN, wz::Key::Enter,
// 		SDLK_RETURN, wz::Key::Enter,
// 		SDLK_KP_ENTER, wz::Key::Enter,
// 		SDLK_DELETE, wz::Key::Delete,
// 		SDLK_BACKSPACE, wz::Key::Backspace,
// 		SDLK_LSHIFT, wz::Key::LeftShift,
// 		SDLK_RSHIFT, wz::Key::RightShift,
// 		SDLK_LCTRL, wz::Key::LeftControl,
// 		SDLK_RCTRL, wz::Key::RightControl
// 	};
// 	for (size_t i = 0; i < sizeof(keys) / sizeof(int); i += 2)
// 	{
// 		if (keys[i] == sym)
// 		{
// 			int key = keys[i + 1];

// 			if (SDL_GetModState() & KMOD_SHIFT)
// 			{
// 				key |= wz::Key::ShiftBit;
// 			}
// 			if (SDL_GetModState() & KMOD_CTRL)
// 			{
// 				key |= wz::Key::ControlBit;
// 			}
// 			return (wz::Key::Enum)key;
// 		}
// 	}
// 	return wz::Key::Unknown;
// }

int g_term_width;
int g_last_symbol_count = 0;
int g_total_symbol_count = 0;

const char* g_test_download_file_url = "https://i-blog.csdnimg.cn/direct/c96ecd7d2a23496b895e7f5bdec7f3ae.jpeg";

void test_url();
void test_http_get();
void test_http_post();
void test_http_put();
void test_download();
void test_download_resume();
void test_download_range();
void test_download_speed_limit();
void get_terminal_info();

/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

int main() {

//     Tigr* win1 = tigrWindow(200, 150, "Window1", 0);
//     Tigr* win2 = tigrWindow(200, 150, "Window2", 0);
//     while (win1 || win2) {
//         if (win1) {
//             tigrClear(win1, tigrRGB(0x80, 0x90, 0xa0));
//             tigrPrint(win1, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #1.");
//             tigrUpdate(win1);
//             if (tigrClosed(win1) || tigrKeyDown(win1, TK_ESCAPE)) {
//                 tigrFree(win1);
//                 win1 = NULL;
//             }
//         }
//         if (win2) {
//             if (tigrBeginOpenGL(win2)) {
//                 glClearColor(1, 0, 1, 1);
//                 glClear(GL_COLOR_BUFFER_BIT);
//             }
//             tigrClear(win2, tigrRGBA(0x00, 0x00, 0x00, 0x00));
//             tigrPrint(win2, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #2.");
//             tigrUpdate(win2);
//             if (tigrClosed(win2) || tigrKeyDown(win2, TK_ESCAPE)) {
//                 tigrFree(win2);
//                 win2 = NULL;
//             }
//         }
//     }
//     if (win1)
//         tigrFree(win1);
//     if (win2)
//         tigrFree(win2);

// #ifdef _WIN32
//     system("pause");
// #endif

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

#ifdef _WIN32
    system("pause");
#endif

    while (1) {
        get_terminal_info();

        printf("0. quit\n");
        printf("1. test_url\n");
        printf("2. test_http_get\n");
        printf("3. test_http_post\n");
        printf("4. test_http_put\n");
        printf("5. test_download\n");
        printf("6. test_download_resume\n");
        printf("7. test_download_range\n");
        printf("8. test_download_speed_limit\n");
        printf("Choose item(0-8): ");
        fflush(stdout);

        int id;
        int n = scanf("%d", &id);
        if (n <= 0) {
            printf("Invalid input!\n");
            return 1;
        }
        printf("------------------------------\n\n");

        switch (id) {
            case 0:  return 0;
            case 1: test_url(); break;
            case 2: test_http_get(); break;
            case 3: test_http_post(); break;
            case 4: test_http_put(); break;
            case 5: test_download(); break;
            case 6: test_download_resume(); break;
            case 7: test_download_range(); break;
            case 8: test_download_speed_limit(); break;
            default: printf("Invalid input!\n"); break;
        }
    }
#ifdef _WIN32
    system("pause");
#endif

//     if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
// 	{
// 		ShowError(SDL_GetError());
// 		SDL_ClearError();
// 		return 1;
// 	}
// 	atexit(SDL_Quit);
// 	SDL_Window *window = SDL_CreateWindow("WidgetZero Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
// 	if (!window)
// 	{
// 		ShowError(SDL_GetError());
// 		SDL_ClearError();
// 		return 1;
// 	}
// 	SDL_GLContext glContext = SDL_GL_CreateContext(window);
// 	if (!glContext)
// 	{
// 		ShowError(SDL_GetError());
// 		SDL_ClearError();
// 		return 1;
// 	}
// #ifdef _MSC_VER
// 	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
// 	{
// 		ShowError("ogl_LoadFunctions failed");
// 		return 1;
// 	}
// #endif
// 	int windowWidth, windowHeight;
// 	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
// 	glViewport(0, 0, windowWidth, windowHeight);
// 	// Setup cursors.
// 	SDL_Cursor *cursors[wz::Cursor::NumCursors];
// 	cursors[wz::Cursor::Default] = SDL_GetDefaultCursor();
// 	cursors[wz::Cursor::Ibeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
// 	cursors[wz::Cursor::Resize_N_S] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
// 	cursors[wz::Cursor::Resize_E_W] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
// 	cursors[wz::Cursor::Resize_NE_SW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
// 	cursors[wz::Cursor::Resize_NW_SE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
// 	// Create the renderer.
// 	wz::TigrRenderer *renderer = new wz::TigrRenderer(nvgCreateGL2, nvgDeleteGL2, 0, "../data", "DejaVuSans", 16.0f);
// 	if (renderer->getError())
// 	{
// 		ShowError(renderer->getError());
// 		return 1;
// 	}
// 	const wz::Color clearColor = renderer->getClearColor();
// 	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
// 	GUI gui(windowWidth, windowHeight, renderer);
// 	const SDL_TimerID textCursorTimer = SDL_AddTimer(textCursorBlinkInterval, TextCursorBlinkCallback, &gui.mainWindow);
// 	for (;;)
// 	{
// 		SDL_Event e;
// 		SDL_WaitEvent(&e);
// 		if (e.type == SDL_QUIT)
// 		{
// 			SDL_RemoveTimer(textCursorTimer);
// 			break;
// 		}
// 		else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
// 		{
// 			glViewport(0, 0, e.window.data1, e.window.data2);
// 			gui.mainWindow.setSize(e.window.data1, e.window.data2);
// 		}
// 		else if (e.type == SDL_MOUSEMOTION)
// 		{
// 			gui.mainWindow.mouseMove(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
// 		}
// 		else if (e.type == SDL_MOUSEBUTTONDOWN)
// 		{
// 			gui.mainWindow.mouseButtonDown(e.button.button, e.button.x, e.button.y);
// 		}
// 		else if (e.type == SDL_MOUSEBUTTONUP)
// 		{
// 			gui.mainWindow.mouseButtonUp(e.button.button, e.button.x, e.button.y);
// 		}
// 		else if (e.type == SDL_MOUSEWHEEL)
// 		{
// 			gui.mainWindow.mouseWheelMove(e.wheel.x, e.wheel.y);
// 		}
// 		else if (e.type == SDL_KEYDOWN)
// 		{
// 			gui.mainWindow.keyDown(ConvertKey(e.key.keysym.sym));
// 		}
// 		else if (e.type == SDL_KEYUP)
// 		{
// 			gui.mainWindow.keyUp(ConvertKey(e.key.keysym.sym));
// 		}
// 		else if (e.type == SDL_TEXTINPUT)
// 		{
// 			gui.mainWindow.textInput(e.text.text);
// 		}
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
// 		gui.mainWindow.drawFrame();
// 		SDL_GL_SwapWindow(window);
// 		SDL_SetCursor(cursors[gui.mainWindow.getCursor()]);
// 	}
// 	delete renderer;

// #ifdef _WIN32
//     system("pause");
// #endif

    return 0;
}


void get_terminal_info() {
#ifdef _WIN32
    system("chcp 65001");
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    GetConsoleScreenBufferInfo(hOutput, &bInfo);
    g_term_width = bInfo.srWindow.Right - bInfo.srWindow.Left;
#else
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    g_term_width = size.ws_col;
#endif // _WIN32

    if (g_term_width > 132) {
        g_total_symbol_count = 100;
    }
    else if (g_term_width > 52) {
        g_total_symbol_count = g_term_width - 32;
    }
    else {
        g_total_symbol_count = 0;
    }

    g_last_symbol_count = 0;
}

void format_data_size(long long data_size, char* output_str, int output_str_size) {
    const int KB = 1024;
    const int MB = 1024 * 1024;
    const int GB = 1024 * 1024 * 1024;
    if (data_size < KB) {
        snprintf(output_str, output_str_size, "%lldB", data_size);
    }
    else if (data_size < 1024 * 1024) {
        snprintf(output_str, output_str_size, "%.2lfKB", data_size / static_cast<double>(KB));
    }
    else if (data_size < 1024 * 1024 * 1024) {
        snprintf(output_str, output_str_size, "%.2lfMB", data_size / static_cast<double>(MB));
    }
    else {
        snprintf(output_str, output_str_size, "%.2lfGB", data_size / static_cast<double>(GB));
    }
}

/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes)
{
    if (download_total_bytes > 0 && g_total_symbol_count > 0) {
        double percent = static_cast<double>(download_now_bytes) / download_total_bytes;
        int curr_symbol_count = static_cast<int>(g_total_symbol_count * percent);
        if (curr_symbol_count > g_last_symbol_count) {
            printf("\r[");
            for (int i = 0; i < curr_symbol_count - 1; ++i) {
                printf("=");
            }
            printf(">");
            for (int i = curr_symbol_count; i < g_total_symbol_count; ++i) {
                printf(" ");
            }
            char dl_now_bytes_str[32], dl_total_bytes_str[32];
            format_data_size(download_now_bytes, dl_now_bytes_str, sizeof(dl_now_bytes_str));
            format_data_size(download_total_bytes, dl_total_bytes_str, sizeof(dl_total_bytes_str));
            printf("] %9s/%-9s %5.2lf%%",  dl_now_bytes_str, dl_total_bytes_str, percent * 100);
            fflush(stdout);
            g_last_symbol_count = curr_symbol_count;
            if (download_now_bytes >= download_total_bytes) {
                printf("\n");
            }
        }
    }
    return true;
}

void reset_progress_bar() {
    g_last_symbol_count = 0;
}
