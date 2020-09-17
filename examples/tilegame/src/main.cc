#include <asset/asset.h>
#include <core/core.h>
#include <render/render.h>
#include <input/input.h>
#include <util/timer.h>
#include <util/math.h>

#include "tilemap.h"

extern "C" const char assets[];
extern "C" const size_t assets_len;

class GenerateWorldComponent : public core::Component {
   private:
    int seed;
    tilemap::Tile &grass_tile;
    tilemap::Tile &dirt_tile;
    tilemap::Tile &stone_tile;

   public:
    GenerateWorldComponent(int seed, tilemap::Tile &grass_tile,
                           tilemap::Tile &dirt_tile, tilemap::Tile &stone_tile);
    virtual void init(core::Interface &interface);
    virtual void update(core::Interface &interface);
    virtual bool is_unique();
};

GenerateWorldComponent::GenerateWorldComponent(int seed,
                                               tilemap::Tile &grass_tile,
                                               tilemap::Tile &dirt_tile,
                                               tilemap::Tile &stone_tile)
    : seed(seed),
      grass_tile(grass_tile),
      dirt_tile(dirt_tile),
      stone_tile(stone_tile) {}

bool GenerateWorldComponent::is_unique() { return true; }

void GenerateWorldComponent::update(core::Interface &interface) {}

void GenerateWorldComponent::init(core::Interface &interface) {
    tilemap::TilemapComponent &tilemap_comp =
        (tilemap::TilemapComponent &)this->entity
            ->get_single_component<tilemap::TilemapComponent>();

    math::SimplexNoise noise(this->seed);
    for (uint32_t i = 0; i < 0x1000; i++) {
        float val = (noise.get(0, (float)i * 3.0f) + 1.0f) * (float)0x20;
        uint32_t height = (uint32_t)val;
        for (uint32_t y = 0; y < height; y++) {
            if (y + 1 == height) {
                tilemap_comp.set_tile(i, y, grass_tile);
            } else if (y > height - 4) {
                tilemap_comp.set_tile(i, y, dirt_tile);
            } else {
                tilemap_comp.set_tile(i, y, stone_tile);
            }
        }
    }
}

int main() {
    logging::Logger logger;
    asset::Assets game_assets(logger, "res", assets, assets + assets_len);
    render::Window window(1280, 720, "tilegame", logger);
    render::Renderer renderer(window, logger);
    input::Input inputs(window, logger);
    timer::Time time(window);
    core::Game game;

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

    std::vector<render::TextureRef> blocks = render::Texture::create_atlas(
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

    core::Entity camera_entity = game.create_entity();
    camera_entity.add_component(
        std::make_unique<CameraComponent>(1280.0f / 720.0f));
    ((CameraComponent &)camera_entity.get_single_component<CameraComponent>())
        .move(0, 1);
    size_t camera_id = camera_entity.get_id();
    game.add_entity(std::move(camera_entity));

    core::Entity tilemap_entity = game.create_entity();
    tilemap::TilemapComponent tilemap_comp(64, 0.05f, camera_id);
    tilemap::Tile dirt_tile(blocks[0]);
    tilemap::Tile grass_tile(blocks[1]);
    tilemap::Tile stone_tile(blocks[2]);
    tilemap_comp.add_tile_type(dirt_tile);
    tilemap_comp.add_tile_type(grass_tile);
    tilemap_comp.add_tile_type(stone_tile);
    GenerateWorldComponent generate_comp(12345, grass_tile, dirt_tile,
                                         stone_tile);
    tilemap_entity.add_component(
        std::make_unique<tilemap::TilemapComponent>(tilemap_comp));
    tilemap_entity.add_component(
        std::make_unique<GenerateWorldComponent>(generate_comp));
    game.add_entity(std::move(tilemap_entity));

    core::Interface interface(logger, renderer, time, game);

    float frame_time_sum = 0;
    size_t frame_count = 0;
    double last_fps_time = 0.0;
    game.init(interface);
    while (window.is_open()) {
        window.poll_inputs();

        renderer.clear();

        game.update(interface);

        ((CameraComponent &)game.get_entity(camera_id)
             .get_single_component<CameraComponent>())
            .move(0.5f * (float)time.delta_time(), 0);
        frame_time_sum += (float)time.delta_time();

        time._frame_complete();
        window.swap_buffers();
        frame_count++;
        if (time.current() - last_fps_time > 1.0) {
            logger.debug_stream()
                << "FPS: " << frame_count << ", Time: " << frame_time_sum
                << logging::COLOR_RS << std::endl;
            frame_count = 0;
            frame_time_sum = 0;
            last_fps_time = time.current();
        }
    }
    return 0;
}
