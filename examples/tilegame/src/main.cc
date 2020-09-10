#include <asset/asset.h>

extern "C" const char assets[];
extern "C" const size_t assets_len;

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    asset::Assets game_assets(logger, "res", assets, assets + assets_len);
    asset::Generic &data = game_assets.load_generic("test.json");
    logger.debug(data.get_string());
    return 0;
}
