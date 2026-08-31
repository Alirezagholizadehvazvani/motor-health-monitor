#pragma once
#include "ICurrentSensor.h"

// ============================================================================
// SimulatedCurrentSensor — Stage A's implementation of ICurrentSensor
// ============================================================================
//
// This class generates a realistic-looking AC current sine wave in software,
// instead of reading a real sensor. It implements ICurrentSensor, so from the
// point of view of any code that holds a `ICurrentSensor*`, this object is
// indistinguishable from a real one.
//
// SCENARIOS
// A real motor doesn't just run "normally" — the whole point of this project
// is detecting when it *doesn't*. So this simulator supports switching
// between the fault conditions you described:
//
//   NORMAL        - steady sine wave at rated RMS amperage
//   PHASE_LOSS    - amplitude collapses to ~0 (a phase conductor has opened,
//                   e.g. a blown fuse or a burned contactor point —
//                   "single-phasing")
//   IMBALANCE     - amplitude is reduced but not zero (e.g. a loose
//                   terminal causing high resistance on one phase, so it
//                   carries less current than the other two)
//   DRY_RUN       - amplitude is present but well below the expected
//                   loaded-motor current (a submersible pump spinning with
//                   no water load draws much less current than one actually
//                   pumping against pressure)
//
// A small amount of random noise is added on top of the ideal sine wave,
// because real sensor signal is never a perfectly clean waveform — modeling
// that now means your fault-detection thresholds (built in the next step)
// will already be noise-tolerant instead of only working on a mathematically
// perfect signal.

enum class Scenario {
    NORMAL,
    PHASE_LOSS,
    IMBALANCE,
    DRY_RUN
};

class SimulatedCurrentSensor : public ICurrentSensor {
public:
    // ratedAmps: the current this phase draws under full, healthy load —
    //            you'd get this from the motor's nameplate in real life.
    // mainsHz:   AC frequency, 50 or 60 depending on your grid (Azerbaijan
    //            runs 50 Hz, so that's the default here).
    // label:     e.g. "Phase A"
    SimulatedCurrentSensor(const char* label, float ratedAmps, float mainsHz = 50.0f);

    void begin() override;
    CurrentSample readSample() override;
    const char* name() const override;

    // Lets a test harness or fault-injection demo change what this
    // "sensor" is currently simulating, without touching real hardware.
    void setScenario(Scenario scenario);
    Scenario getScenario() const;

private:
    const char* label_;
    float ratedAmps_;
    float mainsHz_;
    Scenario scenario_ = Scenario::NORMAL;
    uint32_t startMillis_;

    // Returns the amplitude multiplier (0.0 - 1.0+) for the current scenario,
    // applied on top of ratedAmps_ to get the wave's peak.
    float amplitudeFactorForScenario() const;
};
