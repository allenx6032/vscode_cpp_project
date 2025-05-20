#include "DGE_Pch.hpp"
#include "DGE_Platform.hpp"

def::Platform::Platform()
{
}

void def::Platform::SetWindow(std::shared_ptr<Window> window)
{
    m_Window = window;
}

void def::Platform::SetInputHandler(std::shared_ptr<InputHandler> input)
{
    m_Input = input;
}
