// header for asset loading and storing API

#pragma once

#include <string>
#include <vector>
#include <map>
#include <string_view>
#include <nlohmann/json.hpp>
#include <optional>

#include "../util/logging.h"

using json = nlohmann::json;

namespace asset {
    std::vector<unsigned char> read_file_to_vector(std::ifstream &in_file,
                                                   std::string &resource);

    class Image {
        uint16_t width, height;
        uint8_t components;
        std::vector<unsigned char> data;

       public:
        Image();
        Image(uint16_t width, uint16_t height, uint8_t components,
              std::vector<unsigned char> data);
        unsigned char *get_data();
        uint16_t get_width();
        uint16_t get_height();
        uint8_t get_components();
    };

    class Generic {
        std::vector<unsigned char> data;

       public:
        Generic();
        Generic(std::vector<unsigned char> data);
        unsigned char *get_data();
        size_t size();
        std::string_view get_string_view();
        std::string get_string();
        json get_json();
    };

    class Assets {
        logging::Logger &logger;
        std::string path;
        size_t next_asset_index;
        std::map<std::string, size_t> resource_to_index_map;
        std::map<size_t, Image> images;
        std::map<size_t, Generic> generics;
        std::vector<unsigned char> compress(std::vector<unsigned char> &data);
        std::vector<unsigned char> decompress(std::vector<unsigned char> &data);
        std::pair<size_t, std::optional<std::vector<unsigned char>>> load_file(
            std::string resource, std::string &&resource_type_name);

       public:
        Assets(logging::Logger &logger, std::string path);
        Assets(logging::Logger &logger, std::string path,
               const void *data_start, const void *data_end);
        Image &load_image(std::string resource);
        Generic &load_generic(std::string resource);
        Generic &load_python(std::string resource);
        void deallocate();
        std::vector<unsigned char> store_assets();
    };
}