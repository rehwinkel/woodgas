#include "../util/timer.h"

#include <iostream>

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    render::Window win(640, 480, "æ", logger);
    timer::Time t(win);
    while (1) {
        std::cout << t.current() << std::endl;
    }
    return 0;
}
