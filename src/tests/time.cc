#include "../util/timer.h"

#include <iostream>

int main(int argc, char const *argv[]) {
    render::Window win(640, 480, "æ");
    timer::Time t(win);
    while (1) {
        std::cout << t.current() << std::endl;
    }
    return 0;
}
