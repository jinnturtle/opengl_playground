#ifndef SRC_DBG_HPP_
#define SRC_DBG_HPP_

#include <iostream>
#include <sstream>
#include <string>

namespace logs {
    // returns current time in [HH::MM:SS.ssssss] format
    auto timestamp() -> std::string;

    // general logging print
    template<typename... Ts>
    auto log_print(Ts... args) -> void
    {
        // buffering message before print in case there are multiple threads
        std::stringstream buf;
        buf << timestamp();
        (buf << ... << args);

        std::cout << buf.str() << std::endl;
    }

    // print info message
    template<typename... Ts>
    auto info(Ts... args) -> void
    {
        log_print("[INFO] ", args...);
    }

    // print error message
    template<typename... Ts>
    auto err(Ts... args) -> void
    {
        log_print("[ERROR] ", args...);
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
            log_print("[DBG", lvl, "] ", args...);
    }
} // namespace logs

#else
#   define DBG(...) // nothing here if not a debug build
#endif // DEBUG

#endif // SRC_DBG_HPP_
