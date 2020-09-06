#include "../asset/asset.h"

#include <fstream>

int main(int argc, char const *argv[]) {
    asset::Assets assets("resources");
    asset::Image &img = assets.load_image("test.png");
    asset::Generic &script = assets.load_generic("script.py");
    asset::Generic &generic = assets.load_generic("generic.json");
    std::vector<unsigned char> data = assets.store_assets();
    asset::Assets assets2("resources", &(*data.begin()), &(*data.end()));
    std::ofstream out("test.pak");
    out.write((char *)&(*data.begin()), data.size());
    return 0;
}
