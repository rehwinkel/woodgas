#include "../asset/asset.h"

#include <fstream>

int main(int argc, char const *argv[]) {
    asset::Assets assets("resources");
    assets.load_image("test1.png");
    assets.load_image("test2.png");
    assets.load_image("test3.png");
    assets.load_image("test4.png");
    assets.load_image("test5.png");
    assets.load_image("test6.png");
    assets.load_image("test7.png");
    assets.load_image("test8.png");
    std::vector<unsigned char> data = assets.store_assets();
    asset::Assets assets2("resources", &(*data.begin()), &(*data.end()));
    std::ofstream out("test.pak");
    out.write((char *)&(*data.begin()), data.size());
    return 0;
}
