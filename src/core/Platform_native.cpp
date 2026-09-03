// Only compiled in the native (Mac) environment -- see platformio.ini.
// Once the ESP32 build exists this gets a sibling, Platform_esp32.cpp,
// with the same function just calling the real millis():
//
//   uint32_t nowMillis() { return millis(); }

#include "Platform.h"
#include <chrono>

uint32_t nowMillis() {
    using namespace std::chrono;
    static const auto start = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
    return static_cast<uint32_t>(elapsed.count());
}
