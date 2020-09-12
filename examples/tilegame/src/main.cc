#include <asset/asset.h>
#include <core/core.h>
#include <render/render.h>
#include <input/input.h>
#include <util/timer.h>

extern "C" const char assets[];
extern "C" const size_t assets_len;

class Tile {
    render::TextureRef texture;
    size_t tile_id;
};

class TilemapChunk {
    std::vector<Tile> tiles;
};

class TilemapComponent : public core::Component {
    uint16_t chunk_size;
    std::vector<TilemapChunk> chunks;

   public:
    TilemapComponent();
    virtual void update();
    virtual void init();
    virtual bool is_unique();
};

bool TilemapComponent::is_unique() { return true; }

void TilemapComponent::init() {}

int main() {
    logging::Logger logger;
    asset::Assets game_assets(logger, "res", assets, assets + assets_len);
    render::Window window(1280, 720, "tilegame", logger);
    render::Renderer renderer(window, logger);
    input::Input inputs(window, logger);
    timer::Time time(window);

    // asset::Generic &data = game_assets.load_generic("test.json");
    asset::Image &dirt = game_assets.load_image("tiles/dirt.png");
    asset::Image &grass = game_assets.load_image("tiles/grass.png");
    asset::Image &stone = game_assets.load_image("tiles/stone.png");
    asset::Image &planks = game_assets.load_image("tiles/planks.png");
    asset::Image &log = game_assets.load_image("tiles/log.png");
    asset::Image &leaves = game_assets.load_image("tiles/leaves.png");
    asset::Image &sand = game_assets.load_image("tiles/sand.png");
    asset::Image &redstone = game_assets.load_image("tiles/redstone.png");
    asset::Image &gravel = game_assets.load_image("tiles/gravel.png");

    render::Texture blocks = render::Texture::create_atlas(
        {{dirt.get_width(), dirt.get_height(), dirt.get_components(),
          (char *)dirt.get_data()},
         {grass.get_width(), grass.get_height(), grass.get_components(),
          (char *)grass.get_data()},
         {stone.get_width(), stone.get_height(), stone.get_components(),
          (char *)stone.get_data()},
         {planks.get_width(), planks.get_height(), planks.get_components(),
          (char *)planks.get_data()},
         {log.get_width(), log.get_height(), log.get_components(),
          (char *)log.get_data()},
         {leaves.get_width(), leaves.get_height(), leaves.get_components(),
          (char *)leaves.get_data()},
         {sand.get_width(), sand.get_height(), sand.get_components(),
          (char *)sand.get_data()},
         {redstone.get_width(), redstone.get_height(),
          redstone.get_components(), (char *)redstone.get_data()},
         {gravel.get_width(), gravel.get_height(), gravel.get_components(),
          (char *)gravel.get_data()}});

    float ar = 1280.0f / 720.0f;
    renderer.upload_ortho(-1 * ar, 1 * ar, -1, 1, 0.1f, 100);

    while (window.is_open()) {
        window.poll_inputs();

        renderer.clear();
        renderer.upload_transform(
            render::Transform3D().translate(0, 0, 0).scale(1, 1, 1).rotate_z(
                0));
        renderer.bind_texture(blocks);
        renderer.draw_quad();

        time._frame_complete();
        window.swap_buffers();
    }
    return 0;
}
