#include "components.h"

using namespace components;

TransformComponent::TransformComponent() : x(0), y(0) {}

TransformComponent::TransformComponent(float x, float y) : x(x), y(y) {}

void TransformComponent::init(core::Interface &interface) { (void)(interface); }

void TransformComponent::update(core::Interface &interface) {
    (void)(interface);
}

bool TransformComponent::is_unique() { return true; }

float TransformComponent::get_x() { return this->x; }

float TransformComponent::get_y() { return this->y; }

void TransformComponent::move(float x, float y) {
    this->x += x;
    this->y += y;
}

bool CameraComponent::is_unique() { return true; }

CameraComponent::CameraComponent(float aspect_ratio, float scale)
    : aspect_ratio(aspect_ratio), scale(scale) {}

void CameraComponent::init(core::Interface &interface) {
    this->transform = &this->entity->get_single_component<TransformComponent>();
    interface.get_renderer().upload_ortho(
        -1 * this->aspect_ratio, 1 * this->aspect_ratio, -1, 1, 0.1f, 100);
}

void CameraComponent::update(core::Interface &interface) {
    float x = this->transform->get_x();
    float y = this->transform->get_y();
    interface.get_renderer().upload_view(x, y, 0, 1.0f / this->scale);
}

float CameraComponent::get_aspect_ratio() { return this->aspect_ratio; }

float CameraComponent::get_scale() { return this->scale; }

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
                    .scale(render_size, render_size, render_size));
            renderer.batch_bind_texture(tile_type.texture);
            renderer.batch_draw_quad();
        }
    }
    renderer.batch_draw_quad_end();
}

TilemapComponent::TilemapComponent(uint16_t chunk_size, float render_tile_size,
                                   size_t camera_id)
    : chunk_size(chunk_size),
      render_tile_size(render_tile_size),
      camera_id(camera_id) {}

bool TilemapComponent::is_unique() { return true; }

void TilemapComponent::init(core::Interface &interface) {
    core::Entity &camera_entity =
        interface.get_game().get_entity(this->camera_id);
    this->cam_transform =
        &camera_entity.get_single_component<TransformComponent>();
    this->camera = &camera_entity.get_single_component<CameraComponent>();
}

bool TilemapComponent::should_chunk_render(ChunkPos pos) {
    float minx =
        this->render_tile_size * (float)this->chunk_size * (float)pos.x;
    float miny =
        this->render_tile_size * (float)this->chunk_size * (float)pos.y;
    float maxx =
        this->render_tile_size * (float)this->chunk_size * (float)(pos.x + 1);
    float maxy =
        this->render_tile_size * (float)this->chunk_size * (float)(pos.y + 1);
    float camx = this->cam_transform->get_x();
    float camy = this->cam_transform->get_y();
    float sc = this->camera->get_scale();
    float ar = this->camera->get_aspect_ratio() * sc;
    /*
    logger.debug_stream() << "minx: " << minx << ", "
                          << "miny: " << miny << ", "
                          << "maxx: " << maxx << ", "
                          << "maxy: " << maxy << ", "
                          << "camx: " << camx << ", "
                          << "camy: " << camy << logging::COLOR_RS << std::endl;
    */
    return (std::abs(minx - camx) <= ar || std::abs(maxx - camx) <= ar) &&
           (std::abs(miny - camy) <= sc || std::abs(maxy - camy) <= sc);
}

void TilemapComponent::update(core::Interface &interface) {
    render::Renderer &renderer = interface.get_renderer();
    // size_t chunk_draw_count = 0;
    for (auto &chunk_pair : this->chunks) {
        if (this->should_chunk_render(ChunkPos(chunk_pair.first))) {
            chunk_pair.second.render(*this, renderer, this->render_tile_size);
            // chunk_draw_count++;
        }
    }
    // interface.get_logger().info(std::to_string(chunk_draw_count));
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