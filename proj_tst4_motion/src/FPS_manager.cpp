#include "FPS_manager.hpp"

#include <chrono>
using std::chrono::nanoseconds;
using std::chrono::seconds;
using Clock = std::chrono::high_resolution_clock;
#include <thread> // sleep

#include "logs.hpp"

constexpr unsigned default_fps {60};

FPS_manager::FPS_manager()
: cap_frames{true}
, count{0}
, tgt_dur{1}
, real_dur{1}
, prev_end{Clock::now()}
, end{Clock::now()}
{
    this->set_fps(default_fps);
}

auto FPS_manager::get_fps() -> unsigned
{
    return std::chrono::duration_cast<nanoseconds>(seconds(1)) / this->real_dur;
}

auto FPS_manager::get_delta_seconds() -> double
{
    return (
        1.0 /
        std::chrono::duration_cast<nanoseconds>(seconds(1)).count() *
        this->real_dur.count());
}


auto FPS_manager::set_fps(unsigned fps) -> void
{
    // compensating for rounding inaccuracy
    fps += 1;

    this->tgt_dur = std::chrono::duration_cast<nanoseconds>(seconds(1)) / fps;
    this->cap_frames = true;
}

auto FPS_manager::end_frame() -> void
{
    this->end = Clock::now();
    this->real_dur = this->end - this->prev_end;

    if (this->cap_frames) {
        std::this_thread::sleep_for(this->tgt_dur - this->real_dur);
        this->real_dur = Clock::now() - this->prev_end;
    }

    this->prev_end = Clock::now();
}

auto FPS_manager::toggle_frame_cap() -> void
{
    this->cap_frames = !this->cap_frames;
    if (this->cap_frames) {
        DBG(9, "cap frames");
    } else {
        DBG(9, "uncap frames");
    }
}
