// Stage A entry point -- runs on my Mac, no hardware needed.
//
// This is the only file that knows about "Simulated" anything -- it just
// wires a concrete sensor into the ICurrentSensor interface. When Stage B
// happens this gets replaced by main_esp32.cpp doing the same wiring but
// with a real SCT013CurrentSensor and MQTT over WiFi. Sensor/fault code
// underneath doesn't change.
//
// Keeping this first pass simple: one phase, one scenario, just printing
// raw samples to prove the chain works end to end. RMS + all three phases
// next.

#include "SimulatedCurrentSensor.h"
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // holding this as ICurrentSensor* on purpose -- this is how the fault
    // detection code will hold it too later, it'll never see "Simulated"
    SimulatedCurrentSensor phaseA("Phase A", /*ratedAmps=*/8.5f, /*mainsHz=*/50.0f);
    ICurrentSensor* sensor = &phaseA;

    sensor->begin();

    std::printf("Reading from %s (rated 8.5A, 50Hz simulated sine wave)\n", sensor->name());
    std::printf("timestamp_ms, instantaneous_amps\n");

    // sampling every 2ms so we get ~10 points per 20ms cycle (50Hz) --
    // enough to actually see the wave shape
    for (int i = 0; i < 40; ++i) {
        CurrentSample s = sensor->readSample();
        std::printf("%6u ms,  %+7.3f A\n", s.timestamp_ms, s.amps);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}
