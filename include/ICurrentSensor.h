#pragma once
#include <cstdint>

// ============================================================================
// ICurrentSensor — the Hardware Abstraction Layer (HAL) for this project
// ============================================================================
//
// WHAT IS A "HAL" AND WHY DOES IT MATTER HERE?
//
// In embedded systems, a Hardware Abstraction Layer is a thin interface that
// separates "what data do I need" from "how do I physically get it".
// Everything else in this firmware (RMS calculation, fault detection, MQTT
// publishing) will only ever talk to this interface. It will never know or
// care whether the numbers came from:
//   - Stage A: a simulated sine wave generated in software, or
//   - Stage B: a real SCT-013 clamp sensor being read through the ESP32's
//     ADC (Analog-to-Digital Converter — the peripheral that turns a real
//     voltage on a physical pin into a number your code can use).
//
// This is the single most important design decision in this project, because
// it's what lets you swap simulation for real hardware later by writing ONE
// new class (SCT013CurrentSensor) and changing ONE line in main.cpp — nothing
// in your fault-detection logic has to change, and nothing in your fault
// logic needs to know an ADC exists.
//
// WHY A "SAMPLE" AND NOT JUST "GIVE ME THE CURRENT RIGHT NOW"?
//
// AC current is a sine wave that swings positive and negative 50-60 times a
// second (mains frequency). A single instantaneous reading is close to
// meaningless on its own — it could be near zero even while the motor is
// pulling full load current, just because you happened to sample it at the
// moment the wave crosses zero. That's exactly why RMS (Root Mean Square)
// exists: it's a way of converting many instantaneous samples across a wave
// into one meaningful "effective" value. We'll build that in the next step.
// For now, this interface's job is only to hand back ONE instantaneous
// sample at a time, tagged with when it was taken.

struct CurrentSample {
    float amps;            // Instantaneous current reading, in amps
    uint32_t timestamp_ms; // Milliseconds since the sensor was started
};

class ICurrentSensor {
public:
    virtual ~ICurrentSensor() = default;

    // Called once at startup. For a real sensor this is where you'd
    // configure the ADC (set resolution, attenuation, etc). For the
    // simulated sensor it just resets the internal clock.
    virtual void begin() = 0;

    // Returns one instantaneous current sample. In Stage B, this is what
    // will eventually call analogRead() on the ESP32 under the hood and
    // convert the raw ADC count into amps using the SCT-013's calibration
    // ratio. In Stage A, it computes a point on a simulated sine wave.
    virtual CurrentSample readSample() = 0;

    // Human-readable label, e.g. "Phase A", "Phase B", "Phase C" —
    // used in logging and later in MQTT topic names.
    virtual const char* name() const = 0;
};
