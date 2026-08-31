// This file only gets compiled in the `native` PlatformIO environment
// (see platformio.ini's build_src_filter for env:native).
//
// When we add env:esp32dev in Stage B, we'll add a sibling file,
// Platform_esp32.cpp, containing just:
//
//     #include "Platform.h"
//     #include <Arduino.h>
//     uint32_t nowMillis() { return millis(); }
//
// and that environment's build_src_filter will pull in that file instead
// of this one. Same declared function, two one-line-different bodies,
// selected automatically by which environment you build.

#include "Platform.h"
#include <chrono>

uint32_t nowMillis() {
    using namespace std::chrono;
    static const auto start = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
    return static_cast<uint32_t>(elapsed.count());
}
