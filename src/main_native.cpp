// ============================================================================
// main_native.cpp — Stage A entry point (runs on your Mac, no hardware)
// ============================================================================
//
// This file's only job is to be the "wiring" that connects a concrete
// sensor object to the outside world. Notice it's the ONLY file so far that
// knows the word "Simulated" — everything else just deals in
// ICurrentSensor*. When Stage B arrives, this file gets replaced by
// main_esp32.cpp, which does the same wiring but creates an
// SCT013CurrentSensor instead and publishes to MQTT over real WiFi. The
// sensor and fault-detection code underneath won't change.
//
// For this first step we're deliberately keeping it simple: one phase, one
// scenario, printing raw instantaneous samples to prove the whole chain
// (interface -> simulated implementation -> reading -> output) works.
// Next step will add RMS calculation and all three phases.

#include "SimulatedCurrentSensor.h"
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // We hold this through the ICurrentSensor* interface on purpose, even
    // though we know it's actually a SimulatedCurrentSensor underneath.
    // This is exactly how the fault-detection code (written in the next
    // step) will hold it too -- it will never see the word "Simulated".
    SimulatedCurrentSensor phaseA("Phase A", /*ratedAmps=*/8.5f, /*mainsHz=*/50.0f);
    ICurrentSensor* sensor = &phaseA;

    sensor->begin();

    std::printf("Reading from %s (rated 8.5A, 50Hz simulated sine wave)\n", sensor->name());
    std::printf("timestamp_ms, instantaneous_amps\n");

    // Sample fast enough to actually see the sine wave shape: at 50Hz the
    // wave completes a cycle every 20ms, so sampling every 2ms gives us
    // ~10 points per cycle -- enough to see it rise, peak, and fall.
    for (int i = 0; i < 40; ++i) {
        CurrentSample s = sensor->readSample();
        std::printf("%6u ms,  %+7.3f A\n", s.timestamp_ms, s.amps);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}
