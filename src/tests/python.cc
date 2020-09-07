#include "../render/render.h"
#include "../script/python.h"
#include "../asset/asset.h"

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    asset::Assets assets(logger, "../test_resources");
    asset::Generic &script = assets.load_python("script.py");
    render::Window windex(640, 480, "Title of Window", logger);
    render::Renderer renderer(windex, logger);
    python::PythonInterface py_if(logger, renderer);
    py_if.add_code(script);
    /*
    py_if.add_code(R"py(
class MyPyComponent(Component):
    def init():
        print("initing mypy")

    def update():
        print("updaten")

class MySecondPyComponent(Component):
    def init():
        print("initing second mypy")

    def update():
        print("updaten")

def main():
    while True:
        logger.set_log_level(logger.DEBUG)
        logger.log(logger.ERROR, 1, 2, 3, sep="_")
        logger.log(logger.WARN, locals())
        logger.info("hey", "heya", 1, 2, 3)
        logger.debug("hey", "heya", 1, 2, 3, sep = ", ")
        render.draw_quad()
    )py");
    */
    py_if.start_main();
    py_if.print_error();
    auto components = py_if.load_components();
    for (auto &component : components) {
        component.second.init();
    }
    return 0;
}
