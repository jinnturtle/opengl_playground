#ifndef SRC_DBG_HPP_
#define SRC_DBG_HPP_

#include <iostream>
#include <sstream>
#include <string>

namespace logs {
    // returns current time in [HH::MM:SS.ssssss] format
    auto timestamp() -> std::string;

    // print error message
    template<typename... Ts>
    auto err(Ts... args) -> void
    {
        std::stringstream buf;
        buf << "ERROR" << timestamp() << " ";
        (buf << ... << args);

        std::cout << buf.str() << std::endl;
    }
} // namespace logs

#ifdef DEBUG
#define DBG(verbocity, ...) if ((DEBUG) >= (verbocity) || (DEBUG) == -1) {\
    logs::dbg(verbocity, __VA_ARGS__);\
}

namespace logs {
    // print debug messages up to DEBUG verbocity level
    template<typename... Ts>
    auto dbg(int lvl, Ts... args) -> void // NOLINT(misc-unused-parameters)
    {
            // buffering message before print in case there are multiple threads
            std::stringstream buf;
            buf << "DBG(" << lvl << ")" << timestamp() << " ";
            (buf << ... << args);

            std::cout << buf.str() << std::endl;
    }
} // namespace logs

#else
#   define DBG(...) // nothing here if not a debug build
#endif // DEBUG

#endif // SRC_DBG_HPP_
