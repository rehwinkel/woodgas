#include "../asset/asset.h"

#include <fstream>

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    asset::Assets assets(logger, "resources");
    asset::Image &img = assets.load_image("test.png");
    asset::Generic &scriptasset = assets.load_python("script.py");
    asset::Generic &scriptasset2 = assets.load_python("script2.py");
    asset::Generic &generic = assets.load_generic("generic.json");
    std::vector<unsigned char> data = assets.store_assets();
    asset::Assets assets2(logger, "resources", &(*data.begin()),
                          &(*data.end()));
    std::ofstream out("test.pak");
    out.write((char *)&(*data.begin()), data.size());
    return 0;
}
