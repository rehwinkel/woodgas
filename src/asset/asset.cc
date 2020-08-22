#include "asset.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <zlib.h>
#include <system_error>
#include <filesystem>
#include <cstring>

#include "serialize.cc"

using namespace asset;

Image::Image() {}

Image::Image(size_t width, size_t height, int components,
             std::vector<unsigned char> data)
    : width(width),
      height(height),
      components(components),
      data(std::move(data)) {}

unsigned char *Image::get_data() { return &(*this->data.begin()); }
size_t Image::get_width() { return this->width; }
size_t Image::get_height() { return this->height; }
int Image::get_components() { return this->components; }

Assets::Assets(std::string path) : path(path), next_asset_index(0) {}

Assets::Assets(std::string path, void *data_start, void *data_end)
    : path(path) {
    std::vector<unsigned char> compressed((unsigned char *)data_start,
                                          (unsigned char *)data_end);
    std::vector<unsigned char> data = this->decompress(compressed);
    void *dest = &(*data.begin());
    void *start = dest;
    this->next_asset_index = serialize::read_value<size_t>(&dest);
    size_t resource_to_index_map_size = serialize::read_value<size_t>(&dest);
    for (size_t i = 0; i < resource_to_index_map_size; i++) {
        std::string name = serialize::read_string(&dest);
        this->resource_to_index_map[name] =
            serialize::read_value<size_t>(&dest);
    }
    size_t images_size = serialize::read_value<size_t>(&dest);
    for (size_t i = 0; i < images_size; i++) {
        size_t index = serialize::read_value<size_t>(&dest);
        images[index] = serialize::read_image(&dest);
    }
}

Image &Assets::load_image(std::string resource) {
    if (this->resource_to_index_map.find(resource) !=
        this->resource_to_index_map.end()) {
        throw std::runtime_error("duplicate resource found: " + resource);
    } else {
        size_t index = this->next_asset_index++;
        this->resource_to_index_map[resource] = index;
        std::filesystem::path file_path =
            std::filesystem::path(this->path) / std::filesystem::path(resource);
        int width, height, components;
        unsigned char *data =
            stbi_load(file_path.c_str(), &width, &height, &components, 4);
        if (!data) {
            throw std::runtime_error("failed to load image: " + resource);
        }
        this->images[index] =
            Image(width, height, 4,
                  std::vector<unsigned char>{data, data + width * height * 4});
        stbi_image_free(data);
        return this->images[index];
    }
}

std::vector<unsigned char> Assets::compress(std::vector<unsigned char> &data) {
    size_t uncompressed_length = data.size();
    std::vector<unsigned char> compressed;
    compressed.resize(sizeof(size_t) + compressBound(uncompressed_length));
    size_t out_length;
    compress2(&(*compressed.begin()) + sizeof(size_t), &out_length,
              &(*data.begin()), uncompressed_length, Z_BEST_COMPRESSION);
    compressed.resize(sizeof(size_t) + out_length);
    std::memcpy(&(*compressed.begin()), &uncompressed_length, sizeof(size_t));
    return std::move(compressed);
}

std::vector<unsigned char> Assets::decompress(
    std::vector<unsigned char> &compressed) {
    std::vector<unsigned char> data;
    size_t uncompressed_length;
    std::memcpy(&uncompressed_length, &(*compressed.begin()), sizeof(size_t));
    data.resize(uncompressed_length);
    if (uncompress(&(*data.begin()), &uncompressed_length,
                   &(*compressed.begin()) + sizeof(size_t),
                   compressed.size() - sizeof(size_t)) != Z_OK) {
        throw std::runtime_error("failed to decompress data");
    }
    return std::move(data);
}

std::vector<unsigned char> Assets::store_assets() {
    size_t size = sizeof(this->next_asset_index) +
                  serialize::size_string(this->path) +
                  sizeof(this->resource_to_index_map.size()) +
                  sizeof(this->images.size());
    for (auto &entry : this->resource_to_index_map) {
        size += serialize::size_string(entry.first) + sizeof(entry.second);
    }
    for (auto &entry : this->images) {
        size += sizeof(entry.first) + serialize::size_image(entry.second);
    }
    std::vector<unsigned char> data;
    data.resize(size);
    void *dest = &(*data.begin());
    void *start = dest;
    serialize::write_value(&dest, this->next_asset_index);
    serialize::write_value(&dest, this->resource_to_index_map.size());
    for (auto &entry : this->resource_to_index_map) {
        serialize::write_string(&dest, entry.first);
        serialize::write_value(&dest, entry.second);
    }
    serialize::write_value(&dest, this->images.size());
    for (auto &entry : this->images) {
        serialize::write_value(&dest, entry.first);
        serialize::write_image(&dest, entry.second);
    }
    return std::move(this->compress(data));
}

void Assets::deallocate() { this->images.clear(); }