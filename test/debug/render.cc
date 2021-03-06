#include <render/render.h>
#include <asset/asset.h>

#include <cstdlib>

int main() {
    logging::Logger logger;
    asset::Assets assets(logger, "../test_resources");
    asset::Image &pic = assets.load_image("test.png");
    render::Window window(640, 480, "hey", logger);
    render::Renderer renderer(window, logger);

    render::Texture tex(pic.get_width(), pic.get_height(), pic.get_components(),
                        (char *)pic.get_data());
    float ar = 640.0f / 480.0f;
    renderer.upload_ortho(-1 * ar, 1 * ar, -1, 1, 0.1f, 100);

    while (window.is_open()) {
        window.poll_inputs();

        renderer.clear();
        renderer.upload_transform(render::Transform3D()
                                      .translate(0.5f, 0.3f, 0)
                                      .scale(1, 1, 1)
                                      .rotate_z(0.3f));
        renderer.bind_texture(tex);
        renderer.draw_quad();

        window.swap_buffers();
    }
    tex.cleanup();
    return 0;
}
