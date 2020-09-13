#include <asset/asset.h>
#include <core/core.h>
#include <render/render.h>
#include <input/input.h>
#include <util/timer.h>

#include <unordered_map>

extern "C" const char assets[];
extern "C" const size_t assets_len;

class Tile {
   public:
    render::TextureRef texture;
    Tile(render::TextureRef texture);
    bool operator==(const Tile &other) const;
};

template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct TileHash {
    size_t operator()(const Tile &k) const {
        size_t hash = (size_t)k.texture.texture.get_texture();
        if (k.texture.size) {
            hash_combine(hash, k.texture.size.value().first);
            hash_combine(hash, k.texture.size.value().second);
        }
        if (k.texture.offset) {
            hash_combine(hash, k.texture.offset.value().first);
            hash_combine(hash, k.texture.offset.value().second);
        }
        return hash;
    }
};

class ChunkPos {
   public:
    uint32_t x;
    uint32_t y;
    ChunkPos(uint32_t x, uint32_t y);
    ChunkPos(size_t in_long);
    size_t as_long();
};

class TilemapComponent;

class TilemapChunk {
    ChunkPos pos;
    uint16_t chunk_size;
    std::vector<uint16_t> tiles;

   public:
    TilemapChunk(ChunkPos pos, uint16_t chunk_size);
    void set_tile(uint16_t x, uint16_t y, uint16_t tile);
    void render(TilemapComponent &tilemap, render::Renderer &renderer,
                float render_size);
};

class TilemapComponent : public core::Component {
    uint16_t chunk_size;
    float render_tile_size;
    std::unordered_map<uint16_t, Tile> ids_to_tiles;
    std::unordered_map<Tile, uint16_t, TileHash> tiles_to_ids;
    std::unordered_map<size_t, TilemapChunk> chunks;
    ChunkPos get_chunk_pos_from_pos(uint32_t x, uint32_t y);

   public:
    TilemapComponent(uint16_t chunk_size, float render_tile_size);
    virtual void update(core::Interface &interface);
    virtual void init(core::Interface &interface);
    virtual bool is_unique();
    void add_tile_type(Tile tile);
    Tile &get_tile_type(uint16_t tile);
    void set_tile(uint32_t x, uint32_t y, Tile &tile);
};

Tile::Tile(render::TextureRef texture) : texture(texture) {}

bool Tile::operator==(const Tile &other) const {
    return this->texture == other.texture;
}

ChunkPos::ChunkPos(uint32_t x, uint32_t y) : x(x), y(y) {}
ChunkPos::ChunkPos(size_t in_long)
    : x((uint32_t)(in_long >> 32)), y((uint32_t)in_long) {}

size_t ChunkPos::as_long() {
    return ((size_t)this->x) << 32 | ((size_t)this->y);
}

TilemapChunk::TilemapChunk(ChunkPos pos, uint16_t chunk_size)
    : pos(pos), chunk_size(chunk_size), tiles(chunk_size * chunk_size, 0) {}

void TilemapChunk::set_tile(uint16_t x, uint16_t y, uint16_t tile) {
    uint32_t index = ((uint32_t)y * (uint32_t)chunk_size) + (uint32_t)x;
    tiles[(size_t)index] = tile;
}

void TilemapChunk::render(TilemapComponent &tilemap, render::Renderer &renderer,
                          float render_size) {
    for (uint32_t i = 0; i < (uint32_t)this->tiles.size(); i++) {
        uint16_t tile = this->tiles[i];
        uint32_t x =
            this->pos.x * this->chunk_size + (i % (uint32_t)chunk_size);
        uint32_t y =
            this->pos.y * this->chunk_size + (i / (uint32_t)chunk_size);
        if (tile) {
            Tile &tile_type = tilemap.get_tile_type(tile);
            renderer.upload_transform(
                render::Transform3D()
                    .translate(x * render_size, y * render_size, 0)
                    .scale(render_size, render_size, render_size));
            renderer.bind_texture(tile_type.texture);
            renderer.draw_quad();
        }
    }
}

TilemapComponent::TilemapComponent(uint16_t chunk_size, float render_tile_size)
    : chunk_size(chunk_size), render_tile_size(render_tile_size) {}

bool TilemapComponent::is_unique() { return true; }

void TilemapComponent::init(core::Interface &interface) {}

void TilemapComponent::update(core::Interface &interface) {
    render::Renderer &renderer = interface.get_renderer();
    for (auto &chunk_pair : this->chunks) {
        chunk_pair.second.render(*this, renderer, this->render_tile_size);
    }
}

void TilemapComponent::add_tile_type(Tile tile) {
    bool found = false;
    for (auto &pair : this->ids_to_tiles) {
        if (pair.second == tile) {
            found = true;
            break;
        }
    }
    if (!found) {
        size_t id = this->ids_to_tiles.size() + 1;
        if (id > 0xFFFF) {
            throw std::runtime_error(
                "reached maximum amount of tiles for this tilemap");
        }
        this->ids_to_tiles.insert({id, tile});
        this->tiles_to_ids.insert({tile, id});
    } else {
        throw std::runtime_error("duplicate tile in tilemap found");
    }
}

Tile &TilemapComponent::get_tile_type(uint16_t tile) {
    return this->ids_to_tiles.at(tile);
}

ChunkPos TilemapComponent::get_chunk_pos_from_pos(uint32_t x, uint32_t y) {
    return ChunkPos(x / (uint32_t)this->chunk_size,
                    y / (uint32_t)this->chunk_size);
}

void TilemapComponent::set_tile(uint32_t x, uint32_t y, Tile &tile) {
    uint16_t tile_id = this->tiles_to_ids.at(tile);
    ChunkPos chunk_pos = this->get_chunk_pos_from_pos(x, y);
    if (this->chunks.find(chunk_pos.as_long()) == this->chunks.end()) {
        this->chunks.insert(
            {chunk_pos.as_long(), TilemapChunk(chunk_pos, this->chunk_size)});
    }
    TilemapChunk &chunk = this->chunks.at(chunk_pos.as_long());
    chunk.set_tile(((uint16_t)x) % this->chunk_size,
                   ((uint16_t)y) % this->chunk_size, tile_id);
}

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

    TilemapComponent tilemap(16, 0.1f);
    Tile dirt_tile(render::TextureRef(blocks, 0, 0, 3, 3));
    Tile grass_tile(render::TextureRef(blocks, 1, 0, 3, 3));
    tilemap.add_tile_type(dirt_tile);
    tilemap.add_tile_type(grass_tile);

    tilemap.set_tile(0, 0, dirt_tile);
    tilemap.set_tile(1, 0, grass_tile);
    tilemap.set_tile(2, 0, dirt_tile);
    tilemap.set_tile(0, 1, grass_tile);
    tilemap.set_tile(1, 1, dirt_tile);
    tilemap.set_tile(2, 1, grass_tile);
    tilemap.set_tile(0, 2, dirt_tile);
    tilemap.set_tile(1, 2, grass_tile);
    tilemap.set_tile(2, 2, dirt_tile);
    tilemap.set_tile(20, 20, dirt_tile);

    core::Game game;
    core::Entity tilemap_entity = game.create_entity();
    tilemap_entity.add_component(std::make_unique<TilemapComponent>(tilemap));
    game.add_entity(std::move(tilemap_entity));

    core::Interface interface(logger, renderer);

    float ar = 1280.0f / 720.0f;
    renderer.upload_ortho(-1 * ar, 1 * ar, -1, 1, 0.1f, 100);
    renderer.upload_transform(render::Transform3D());

    game.init(interface);
    while (window.is_open()) {
        window.poll_inputs();

        renderer.clear();

        game.update(interface);

        time._frame_complete();
        window.swap_buffers();
    }
    return 0;
}
