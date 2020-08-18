// header for context creation and rendering API

#pragma once

#include <string>

namespace render
{
    class Window
    {
        void *window;

    public:
        Window(int width, int height, std::string title);
        void poll_inputs();
        void swap_buffers();
        bool is_open();
    };

    class Renderer
    {
    public:
        Renderer(Window &window);
        void clear();
    };
} // namespace render
