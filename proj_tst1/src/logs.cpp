#include "logs.hpp"

#include <array>
#include <sstream>

#if 0
auto logs::timestamp() -> std::string
{
    auto now = std::chrono::high_resolution_clock::now();
    auto now_tse = now.time_since_epoch();

    time_t tt = std::chrono::high_resolution_clock::to_time_t(now);

    // takes a bit long
    int nano =
    (
        std::chrono::duration_cast<std::chrono::nanoseconds>(now_tse) -
        std::chrono::duration_cast<std::chrono::seconds>(now_tse)).count();

    // takes a bit long
    std::stringstream buf;
    buf << "["
    << std::put_time(std::localtime(&tt), "%T") << "." << nano << "]";

    return buf.str();
}
#endif

// roughly 25% faster than the pure c++ implementation (above) main costs in c++
// implementation: getting nanoseconds past the second (involves a
// duration.count(), duration casts and a substraction operation between
// durations) and formatting with stringstream
auto logs::timestamp() -> std::string
{
    // uninitialized for a bit of speed
    struct timespec ts; // NOLINT
    timespec_get(&ts, TIME_UTC);
    // uninitialized for a bit of speed
    // TODO no sense in allocating this every time, could just keep it
    std::array<char, 32> buf; // NOLINT
    size_t rc = strftime(&buf[0], buf.size(),
            "[%Y-%m-%d %T", localtime(&ts.tv_sec));
    // NOLINTNEXTLINE
    snprintf(&buf[rc], sizeof buf - rc, ".%09ld]", ts.tv_nsec);

    return std::string(&buf[0]);
}
