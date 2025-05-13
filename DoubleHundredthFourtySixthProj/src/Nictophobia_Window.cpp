#include "Nictophobia_Window.h"
#include "Nictophobia_InputProcessor.h"

Window::Window() {
	processor = new InputProcessor();
}

Window::~Window(){
	delete processor;
}
