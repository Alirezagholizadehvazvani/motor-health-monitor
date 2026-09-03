#include "SimulatedCurrentSensor.h"
#include "Platform.h"
#include <cmath>
#include <cstdlib>

// M_PI isn't guaranteed to exist across compilers, defining it myself
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
            return 0.02f;       // conductor's open, only stray current left
        case Scenario::IMBALANCE:
            return 0.55f;       // carrying noticeably less than the other phases
        case Scenario::DRY_RUN:
            return 0.35f;       // unloaded motor draws well below rated current
    }
    return 1.0f;
}

CurrentSample SimulatedCurrentSensor::readSample() {
    uint32_t nowMs = nowMillis();
    uint32_t elapsedMs = nowMs - startMillis_;

    // figure out where we are in the sine cycle: angle = 2*PI*freq*time
    float timeSeconds = static_cast<float>(elapsedMs) / 1000.0f;
    float angle = 2.0f * PI_F * mainsHz_ * timeSeconds;

    float peakAmps = ratedAmps_ * amplitudeFactorForScenario();
    float idealValue = peakAmps * std::sin(angle);

    // small amount of noise (+/- ~2% of rated) so it's not a perfectly
    // clean signal -- real sensors are never this tidy
    float noiseAmplitude = ratedAmps_ * 0.02f;
    float noise = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * noiseAmplitude;

    CurrentSample sample;
    sample.amps = idealValue + noise;
    sample.timestamp_ms = nowMs;
    return sample;
}
