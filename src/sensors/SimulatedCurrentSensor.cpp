#include "SimulatedCurrentSensor.h"
#include "Platform.h"
#include <cmath>
#include <cstdlib>

// M_PI isn't guaranteed to exist in every compiler's <cmath> (it's a
// historical POSIX extension, not standard C++), so we define it ourselves
// to keep this file portable across native and ESP32 toolchains.
constexpr float PI_F = 3.14159265358979323846f;

SimulatedCurrentSensor::SimulatedCurrentSensor(const char* label, float ratedAmps, float mainsHz)
    : label_(label), ratedAmps_(ratedAmps), mainsHz_(mainsHz), startMillis_(0) {}

void SimulatedCurrentSensor::begin() {
    startMillis_ = nowMillis();
}

const char* SimulatedCurrentSensor::name() const {
    return label_;
}

void SimulatedCurrentSensor::setScenario(Scenario scenario) {
    scenario_ = scenario;
}

Scenario SimulatedCurrentSensor::getScenario() const {
    return scenario_;
}

float SimulatedCurrentSensor::amplitudeFactorForScenario() const {
    switch (scenario_) {
        case Scenario::NORMAL:
            return 1.0f;        // full rated current
        case Scenario::PHASE_LOSS:
            return 0.02f;       // conductor open — only stray/leakage current left
        case Scenario::IMBALANCE:
            return 0.55f;       // one phase carrying noticeably less than the others
        case Scenario::DRY_RUN:
            return 0.35f;       // motor spinning unloaded draws well below rated current
    }
    return 1.0f;
}

CurrentSample SimulatedCurrentSensor::readSample() {
    uint32_t nowMs = nowMillis();
    uint32_t elapsedMs = nowMs - startMillis_;

    // Convert elapsed time into "where are we in the sine wave cycle".
    // A sine wave at mainsHz_ (e.g. 50 Hz) completes one full cycle every
    // (1000 / mainsHz_) milliseconds. We convert elapsed milliseconds into
    // radians so std::sin() can use it directly:
    //   angle = 2*PI * frequency_hz * time_seconds
    float timeSeconds = static_cast<float>(elapsedMs) / 1000.0f;
    float angle = 2.0f * PI_F * mainsHz_ * timeSeconds;

    float peakAmps = ratedAmps_ * amplitudeFactorForScenario();
    float idealValue = peakAmps * std::sin(angle);

    // Add small random noise (+/- ~2% of rated current) so this doesn't
    // look like a mathematically perfect signal. rand() isn't
    // cryptographically anything, but for simulating sensor jitter it's
    // perfectly fine and has zero dependencies.
    float noiseAmplitude = ratedAmps_ * 0.02f;
    float noise = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * noiseAmplitude;

    CurrentSample sample;
    sample.amps = idealValue + noise;
    sample.timestamp_ms = nowMs;
    return sample;
}
