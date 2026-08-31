#pragma once
#include <cstdint>

// ============================================================================
// Platform.h — the ONE place in this codebase allowed to know which
// environment (native Mac vs real ESP32) we're compiling for.
// ============================================================================
//
// Arduino/ESP32 firmware has a built-in function called millis() that
// returns "milliseconds since boot" — it's how embedded code keeps track of
// time without an operating system. Our native (Mac) build has no such
// function, because it's just a normal program running under macOS, which
// already has its own OS-level clock.
//
// Rather than sprinkling #ifdef ARDUINO checks throughout the sensor and
// fault-detection code (which would defeat the purpose of the abstraction
// layer), we isolate that one difference here, behind a function called
// nowMillis(). Every other file in this project — simulated sensor, real
// sensor (later), RMS calculator, fault detector — calls nowMillis() and
// never needs to know or care which platform it's running on.
//
// This is the same pattern as ICurrentSensor, just applied to "get the
// time" instead of "get a current reading". Small, boring, isolated
// platform differences behind one interface is the core idea of embedded
// portability.

uint32_t nowMillis();
