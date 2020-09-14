#include "tilemap.h"

using namespace tilemap;

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
    renderer.batch_draw_quad_begin();
    for (uint32_t i = 0; i < (uint32_t)this->tiles.size(); i++) {
        uint16_t tile = this->tiles[i];
        uint32_t x =
            this->pos.x * this->chunk_size + (i % (uint32_t)chunk_size);
        uint32_t y =
            this->pos.y * this->chunk_size + (i / (uint32_t)chunk_size);
        if (tile) {
            Tile &tile_type = tilemap.get_tile_type(tile);
            renderer.batch_upload_transform(
                render::Transform3D()
                    .translate(render_size * ((float)x + 0.5f),
                               render_size * ((float)y + 0.5f), 0)
                    .translate(-16.0f / 9.0f, -1, 0)
                    .scale(render_size, render_size, render_size));
            renderer.batch_bind_texture(tile_type.texture);
            renderer.batch_draw_quad();
        }
    }
    renderer.batch_draw_quad_end();
}

TilemapComponent::TilemapComponent(core::Entity &entity, uint16_t chunk_size,
                                   float render_tile_size)
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