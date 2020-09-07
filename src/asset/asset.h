// header for asset loading and storing API

#pragma once

#include <string>
#include <vector>
#include <map>
#include <string_view>

#include "../util/logging.h"

namespace asset {
    std::vector<unsigned char> read_file_to_vector(std::ifstream &in_file,
                                                   std::string &resource);

    class Image {
        size_t width, height;
        int components;
        std::vector<unsigned char> data;

       public:
        Image();
        Image(size_t width, size_t height, int components,
              std::vector<unsigned char> data);
        unsigned char *get_data();
        size_t get_width();
        size_t get_height();
        int get_components();
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

       public:
        Assets(logging::Logger &logger, std::string path);
        Assets(logging::Logger &logger, std::string path, void *data_start,
               void *data_end);
        Image &load_image(std::string resource);
        Generic &load_generic(std::string resource);
        Generic &load_python(std::string resource);
        Generic &load_vector(std::string resource,
                             std::vector<unsigned char> data);
        void deallocate();
        std::vector<unsigned char> store_assets();
    };
}