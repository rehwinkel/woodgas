#include "../render/render.h"
#include "../input/input.h"
#include "../util/timer.h"

#include <cstdlib>

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    render::Window window(640, 480, "hey", logger);
    render::Renderer renderer(window, logger);
    input::Input inputs(window, logger);
    timer::Time time(window);

    render::Texture tex(64, 64, 4, (char *)std::malloc(16384));
    float ar = 640.0 / 480.0;
    renderer.upload_ortho(-1 * ar, 1 * ar, -1, 1, 0.1, 100);

    float r = 0;
    while (window.is_open()) {
        window.poll_inputs();

        if (inputs.is_key_down(input::Key::KEY_LEFT_SHIFT)) {
            r += 0.5 * time.delta_time();
        }

        renderer.clear();
        renderer.upload_transform(render::Transform3D()
                                      .translate(0.5, 0.3, 0)
                                      .scale(1, 1, 1)
                                      .rotate_z(0.3 + r));
        renderer.bind_texture(tex);
        renderer.draw_quad();

        time._frame_complete();
        window.swap_buffers();
    }
    return 0;
}
