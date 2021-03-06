#include "asset.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <zlib.h>
#include <system_error>

#ifdef __MACOS__
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

#include <cstring>
#include <fstream>
#include <iterator>

#include "serialize.cc"
#include "../script/python.h"

using namespace asset;

Image::Image() {}

Image::Image(uint16_t width, uint16_t height, uint8_t components,
             std::vector<unsigned char> data)
    : width(width),
      height(height),
      components(components),
      data(std::move(data)) {}

unsigned char *Image::get_data() { return &(*this->data.begin()); }
uint16_t Image::get_width() { return this->width; }
uint16_t Image::get_height() { return this->height; }
uint8_t Image::get_components() { return this->components; }

Generic::Generic() {}

Generic::Generic(std::vector<unsigned char> data) : data(data) {}

unsigned char *Generic::get_data() { return &(*this->data.begin()); }
size_t Generic::size() { return this->data.size(); }
std::string_view Generic::get_string_view() {
    return std::string_view((char *)this->get_data(), this->size());
}
std::string Generic::get_string() {
    return std::string((char *)this->get_data(), this->size());
}

json Generic::get_json() { return json::parse((char *)this->get_data()); }

Assets::Assets(logging::Logger &logger, std::string path)
    : logger(logger), path(path), next_asset_index(0) {
    logger.debug("creating new empty assets...");
}

Assets::Assets(logging::Logger &logger, std::string path,
               const void *data_start, const void *data_end)
    : logger(logger), path(path) {
    logger.debug("creating assets from binary data...");
    std::vector<unsigned char> compressed((unsigned char *)data_start,
                                          (unsigned char *)data_end);
    std::vector<unsigned char> data = this->decompress(compressed);
    void *dest = &(*data.begin());
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
    size_t generics_size = serialize::read_value<size_t>(&dest);
    for (size_t i = 0; i < generics_size; i++) {
        size_t index = serialize::read_value<size_t>(&dest);
        generics[index] = Generic(std::move(serialize::read_array(&dest)));
    }
}

std::vector<unsigned char> asset::read_file_to_vector(std::ifstream &in_file,
                                                      std::string &resource) {
    if (!in_file.is_open()) {
        throw std::runtime_error("failed to load file to vector: " + resource);
    }
    std::streamsize size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);
    std::vector<unsigned char> data(size);
    if (!in_file.read((char *)data.data(), size)) {
        throw std::runtime_error("failed to load file to vector: " + resource);
    }
    in_file.close();
    return data;
}

std::pair<size_t, std::optional<std::vector<unsigned char>>> Assets::load_file(
    std::string resource, std::string &&resource_type_name) {
    std::replace(resource.begin(), resource.end(), '\\', '/');
    std::filesystem::path file_path = std::filesystem::absolute(
        std::filesystem::path(this->path) / std::filesystem::path(resource));
    this->logger.debug_stream()
        << "loading " << resource_type_name << " resource '" << resource
        << "'..." << logging::COLOR_RS << std::endl;
    auto resource_to_index_it = this->resource_to_index_map.find(resource);
    if (resource_to_index_it != this->resource_to_index_map.end()) {
        auto *pair = &(*resource_to_index_it);
        size_t index = pair->second;
        return {index, std::nullopt};
    } else {
        size_t index = this->next_asset_index++;
        this->resource_to_index_map[resource] = index;
        std::ifstream in_file(file_path, std::ios::binary | std::ios::ate);
        std::vector<unsigned char> raw_data =
            read_file_to_vector(in_file, resource);
        return {index, raw_data};
    }
}

Image &Assets::load_image(std::string resource) {
    auto [index, data_opt] = this->load_file(resource, "image");
    if (data_opt) {
        int width, height, components;
        unsigned char *data =
            stbi_load_from_memory(data_opt->data(), (int)data_opt->size(),
                                  &width, &height, &components, 4);
        if (!data) {
            logger.error_stream()
                << "failed to load image (reason: " << stbi_failure_reason()
                << "): " << resource << logging::COLOR_RS << std::endl;
            throw std::runtime_error("failed to load image");
        }
        this->images[index] =
            Image((uint16_t)width, (uint16_t)height, 4,
                  std::vector<unsigned char>{data, data + width * height * 4});
        stbi_image_free(data);
    }
    return this->images[index];
}

Generic &Assets::load_generic(std::string resource) {
    auto [index, data_opt] = this->load_file(resource, "generic");
    if (data_opt) {
        std::vector<unsigned char> data = std::move(data_opt.value());
        this->generics[index] = Generic(std::move(data));
    }
    return this->generics[index];
}

Generic &Assets::load_python(std::string resource) {
    auto [index, data_opt] = this->load_file(resource, "python");
    if (data_opt) {
        std::string script(data_opt->begin(), data_opt->end());
        std::vector<unsigned char> compiled_data =
            python::compile_python(this->logger, script, resource);
        this->generics[index] = Generic(std::move(compiled_data));
    }
    return this->generics[index];
}

std::vector<unsigned char> Assets::compress(std::vector<unsigned char> &data) {
    size_t uncompressed_length = data.size();
    std::vector<unsigned char> compressed;
    compressed.resize(sizeof(size_t) +
                      compressBound((uLong)uncompressed_length));
    uLongf out_length;
    compress2(&(*compressed.begin()) + sizeof(size_t), &out_length,
              &(*data.begin()), (uLong)uncompressed_length, Z_BEST_COMPRESSION);
    compressed.resize(sizeof(size_t) + out_length);
    std::memcpy(compressed.data(), &uncompressed_length, sizeof(size_t));
    return compressed;
}

std::vector<unsigned char> Assets::decompress(
    std::vector<unsigned char> &compressed) {
    std::vector<unsigned char> data;
    size_t uncompressed_length;
    std::memcpy(&uncompressed_length, &(*compressed.begin()), sizeof(size_t));
    data.resize(uncompressed_length);
    if (uncompress(&(*data.begin()), (uLongf *)&uncompressed_length,
                   &(*compressed.begin()) + sizeof(size_t),
                   (uLong)(compressed.size() - sizeof(size_t))) != Z_OK) {
        throw std::runtime_error("failed to decompress data");
    }
    return data;
}

std::vector<unsigned char> Assets::store_assets() {
    this->logger.debug("serializing assets...");
    size_t size = sizeof(this->next_asset_index) +
                  serialize::size_string(this->path) +
                  sizeof(this->resource_to_index_map.size()) +
                  sizeof(this->images.size()) + sizeof(this->generics.size());
    for (auto &entry : this->resource_to_index_map) {
        size += serialize::size_string(entry.first) + sizeof(entry.second);
    }
    for (auto &entry : this->images) {
        size += sizeof(entry.first) + serialize::size_image(entry.second);
    }
    for (auto &entry : this->generics) {
        size +=
            sizeof(entry.first) + serialize::size_array(entry.second.size());
    }
    std::vector<unsigned char> data;
    data.resize(size);
    void *dest = &(*data.begin());
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
    serialize::write_value(&dest, this->generics.size());
    for (auto &entry : this->generics) {
        serialize::write_value(&dest, entry.first);
        serialize::write_array(&dest, entry.second.size(),
                               entry.second.get_data());
    }
    return std::move(this->compress(data));
}

void Assets::deallocate() {
    this->images.clear();
    this->generics.clear();
}