#pragma once
#include "ICurrentSensor.h"

// Fake sensor for Stage A -- generates a sine wave in software instead of
// reading real hardware. Implements ICurrentSensor so it's a drop-in for
// anything that expects a real sensor.
//
// Scenarios are the different fault conditions I need to be able to test
// without a real motor:
//   NORMAL     - steady current at rated amps
//   PHASE_LOSS - amplitude basically collapses to 0 (blown fuse / burned
//                contactor point -- single-phasing)
//   IMBALANCE  - reduced but not zero (loose terminal, high resistance on
//                one leg)
//   DRY_RUN    - present but noticeably below rated (pump spinning with no
//                water load draws less current than one under real load)
//
// Also added a bit of random noise on top of the ideal sine wave since a
// perfectly clean signal isn't realistic and I don't want my fault
// thresholds later to only work on a textbook-perfect waveform.

enum class Scenario {
    NORMAL,
    PHASE_LOSS,
    IMBALANCE,
    DRY_RUN
};

class SimulatedCurrentSensor : public ICurrentSensor {
public:
    // ratedAmps = full healthy load current for this phase (from the motor
    // nameplate in real life). mainsHz = 50 or 60 depending on grid --
    // defaulting to 50 since that's what we run here.
    SimulatedCurrentSensor(const char* label, float ratedAmps, float mainsHz = 50.0f);

    void begin() override;
    CurrentSample readSample() override;
    const char* name() const override;

    // switch which fault condition this "sensor" is currently simulating
    void setScenario(Scenario scenario);
    Scenario getScenario() const;

private:
    const char* label_;
    float ratedAmps_;
    float mainsHz_;
    Scenario scenario_ = Scenario::NORMAL;
    uint32_t startMillis_;

    // amplitude multiplier for the current scenario, applied to ratedAmps_
    float amplitudeFactorForScenario() const;
};
