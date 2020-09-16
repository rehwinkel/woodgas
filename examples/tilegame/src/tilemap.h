#pragma once

#include "camera.h"

#include <util/math.h>

#include <unordered_map>

namespace tilemap {
    class Tile {
       public:
        render::TextureRef texture;
        Tile(render::TextureRef texture);
        bool operator==(const Tile &other) const;
    };

    struct TileHash {
        size_t operator()(const Tile &k) const {
            size_t hash = (size_t)k.texture.texture.get_texture();
            if (k.texture.size.first >= 0 && k.texture.size.second >= 0) {
                math::hash_combine(hash, k.texture.size.first);
                math::hash_combine(hash, k.texture.size.second);
            }
            if (k.texture.offset.first >= 0 && k.texture.offset.second >= 0) {
                math::hash_combine(hash, k.texture.offset.first);
                math::hash_combine(hash, k.texture.offset.second);
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
        size_t camera_id;
        std::unordered_map<uint16_t, Tile> ids_to_tiles;
        std::unordered_map<Tile, uint16_t, TileHash> tiles_to_ids;
        std::unordered_map<size_t, TilemapChunk> chunks;
        ChunkPos get_chunk_pos_from_pos(uint32_t x, uint32_t y);
        CameraComponent &get_camera(core::Game &game);
        bool should_chunk_render(ChunkPos pos, core::Game &game,
                                 logging::Logger &logger);

       public:
        TilemapComponent(uint16_t chunk_size, float render_tile_size,
                         size_t camera_id);
        virtual void update(core::Interface &interface);
        virtual void init(core::Interface &interface);
        virtual bool is_unique();
        void add_tile_type(Tile tile);
        Tile &get_tile_type(uint16_t tile);
        void set_tile(uint32_t x, uint32_t y, Tile &tile);
    };
}