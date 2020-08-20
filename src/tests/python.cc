#include "../script/python.h"

int main(int argc, char const *argv[]) {
    python::PythonInterface py_if;
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
    )py");
    auto components = py_if.load_components();
    for (auto &component : components) {
        component.second.init();
    }
    return 0;
}
