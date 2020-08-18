#include "render/render.h"
#include "input/input.h"

int main(int argc, char const *argv[])
{
    render::Window window(640, 480, "hey");
    render::Renderer renderer(window);
    input::Input input(window);

    while (window.is_open())
    {
        window.poll_inputs();

        renderer.clear();
        renderer.draw_quad();

        window.swap_buffers();
    }
    return 0;
}
