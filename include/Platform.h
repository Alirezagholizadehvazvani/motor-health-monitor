#pragma once
#include <cstdint>

// Small wrapper so only ONE file in the whole project has to know whether
// we're compiling for the Mac or for the ESP32.
//
// Arduino/ESP32 code has a built-in millis() for "ms since boot" -- our
// native build doesn't have that since it's just a normal program running
// under macOS. Rather than putting #ifdef ARDUINO checks everywhere,
// I'm hiding that one difference behind nowMillis(). Every other file just
// calls this and doesn't need to know or care what platform it's on.

uint32_t nowMillis();
