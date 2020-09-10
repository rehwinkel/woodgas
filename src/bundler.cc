#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <asset/asset.h>

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

int main(int argc, char const* argv[]) {
    if (argc == 2) {
        std::string resource_folder_name(argv[1]);
        std::filesystem::path resource_path =
            std::filesystem::absolute(resource_folder_name);
        logging::Logger logger;
        logger.debug_stream()
            << resource_path << logging::COLOR_RS << std::endl;
        asset::Assets assets(logger, resource_folder_name);
        for (const auto& path :
             std::filesystem::recursive_directory_iterator(resource_path)) {
            if (!path.is_directory()) {
                std::string filename = path.path().string();
                std::string resource_name =
                    filename.substr(resource_path.string().length());
                if (endsWith(resource_name, ".py")) {
                    assets.load_python(resource_name);
                } else if (endsWith(resource_name, ".png")) {
                    assets.load_image(resource_name);
                } else {
                    assets.load_generic(resource_name);
                }
            }
        }
        std::vector<unsigned char> data = assets.store_assets();
        std::ofstream out_file("assets.c");
        out_file << "const char assets[" << data.size() << "] = {";
        for (size_t i = 0; i < data.size(); i++) {
            out_file << "0x" << std::uppercase << std::hex << (int)data[i];
            if (i + 1 < data.size()) out_file << ", ";
        }
        out_file << "};" << std::endl;
        out_file << "const unsigned long long assets_len = " << std::dec
                 << data.size() << ";" << std::endl;
        return 0;
    }
    std::cerr << "usage: " << argv[0] << " <folder>" << std::endl;
    return 1;
}
