// header for the time API

#pragma once

#include "../render/render.h"

namespace timer {
    class Time {
        double prev_frame_time;

       public:
        Time(render::Window &window);
        double current();
        double delta_time();
        void _frame_complete();
    };
}