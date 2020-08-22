// header for the time API

namespace time {
    class Time {
        double prev_frame_time;

       public:
        Time();
        double current();
        double delta_time();
        void _frame_complete();
    };
}