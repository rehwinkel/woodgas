#include "../render/render.h"

#include <cstdlib>

#include "../input/input.h"

int main(int argc, char const *argv[]) {
    render::Window window(640, 480, "hey");
    render::Renderer renderer(window);
    input::Input input(window);

    render::Texture tex(16, 16, 4, (char *)std::malloc(1024));
    float ar = 640.0 / 480.0;
    renderer.upload_ortho(-1 * ar, 1 * ar, -1, 1, 0.1, 100);

    while (window.is_open()) {
        window.poll_inputs();

        renderer.clear();
        renderer.upload_transform(render::Transform3D()
                                      .translate(0.5, 0.3, 0)
                                      .scale(1, 1, 1)
                                      .rotate_z(0.3));
        renderer.bind_texture(tex);
        renderer.draw_quad();

        window.swap_buffers();
    }
    return 0;
}
