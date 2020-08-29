#include "../render/render.h"
#include "../script/python.h"

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    render::Window windex(640, 480, "Title of Window", logger);
    render::Renderer renderer(windex, logger);
    python::PythonInterface py_if(logger, renderer);
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
        render.draw_quad()
    )py");
    py_if.start_main();
    auto components = py_if.load_components();
    for (auto &component : components) {
        component.second.init();
    }
    return 0;
}
