#include <asset/asset.h>

#include <fstream>
#include <nlohmann/json.hpp>

using namespace nlohmann;

int main() {
    logging::Logger logger;
    asset::Assets assets(logger, "../test_resources");
    asset::Image &img = assets.load_image("test.png");
    asset::Generic &scriptasset = assets.load_python("script.py");
    asset::Generic &scriptasset2 = assets.load_python("script2.py");
    asset::Generic &generic = assets.load_generic("generic.json");
    (void)(img);
    (void)(scriptasset);
    (void)(scriptasset2);

    std::string text = generic.get_string();
    auto parsed = json::parse(text);
    logger.info_stream() << "JSON:\n"
                         << parsed.dump(2) << logging::COLOR_RS << std::endl;

    std::vector<unsigned char> data = assets.store_assets();
    asset::Assets assets2(logger, "../test_resources", &(*data.begin()),
                          &(*data.end()));
    std::ofstream out("test.pak");
    out.write((char *)&(*data.begin()), data.size());
    return 0;
}
