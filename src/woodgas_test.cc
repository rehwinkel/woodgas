#include "render/render.h"
#include "input/input.h"

#include <cstdlib>

int main(int argc, char const *argv[])
{
    render::Window window(640, 480, "hey");
    render::Renderer renderer(window);
    input::Input input(window);

    render::Texture tex(16, 16, 4, (char *)std::malloc(1024));

    while (window.is_open())
    {
        window.poll_inputs();

        renderer.clear();
        renderer.bind_texture(tex);
        renderer.draw_quad();

        window.swap_buffers();
    }
    return 0;
}
