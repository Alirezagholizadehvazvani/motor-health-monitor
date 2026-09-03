#pragma once
#include <cstdint>

// Interface for a current sensor on one phase. This is the boundary between
// "how do I actually get a current reading" and everything else (RMS calc,
// fault detection, MQTT). As long as something implements this, the rest of
// the code doesn't care if it's my simulated sine wave or a real SCT-013
// clamp on the ESP32 later. That's the whole point of doing it this way --
// when the hardware shows up I just write a new class that implements this
// same interface, swap it in main.cpp, and nothing else needs to change.
//
// One thing that tripped me up at first: why return one sample at a time
// instead of just "give me the current now"? Because AC current is a sine
// wave -- a single reading could land near zero just by bad timing (right
// at the zero crossing) even if the motor is pulling full load. RMS fixes
// that by combining a bunch of samples into one meaningful number. That's
// the next step -- this file just defines how to grab one raw sample.

struct CurrentSample {
    float amps;            // instantaneous reading in amps
    uint32_t timestamp_ms; // ms since sensor start
};

class ICurrentSensor {
public:
    virtual ~ICurrentSensor() = default;

    // setup -- for the real sensor later this configures the ADC, for now
    // it just resets the clock
    virtual void begin() = 0;

    // grab one instantaneous sample
    virtual CurrentSample readSample() = 0;

    // e.g. "Phase A" -- used for logging / later MQTT topics
    virtual const char* name() const = 0;
};
