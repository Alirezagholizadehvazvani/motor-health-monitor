# 3-Phase Motor Health Monitor

Software system for monitoring 3-phase induction motors, focused on two common and expensive failure modes in industrial pumps: phase imbalance / single-phasing and dry-running.

This repository contains Stage A of the project — a complete software simulation with a clean architecture. The next stage will move the same logic onto real ESP32 hardware with current sensors.

## Why this project

In industrial environments, especially with submersible pumps, two problems cause a lot of motor damage:

- **Phase imbalance / single-phasing**: One phase loses current. The motor continues running on the remaining phases, overheats, and often burns out within minutes.
- **Dry-running**: The pump loses its water load. Current drops, but the motor keeps spinning and damages bearings and seals.

Both conditions have clear current signatures. This project detects them in software.

I previously worked as a technician in industrial settings. That experience made it clear how important reliable detection is, and how often software systems fail when they don’t account for real-world conditions. This project is built with those lessons in mind.

## Architecture

The system is built around a simple Hardware Abstraction Layer:

```text
Fault Detection Logic
        │
        ▼
  ICurrentSensor (interface)
        │
   ┌────┴────┐
   │         │
Simulated   Real sensor
(Stage A)   (Stage B)
```

Everything above the interface (RMS calculation, fault detection, future MQTT publishing) stays the same. Only the sensor implementation changes between stages.

This approach lets me develop and test the core logic properly before hardware is available.

## Current Status (Stage A)

**Completed:**
- Hardware abstraction interface (`ICurrentSensor`)
- Simulated current sensor with realistic scenarios (normal, phase loss, imbalance, dry-run)
- Single-phase reading verified end-to-end
- Basic project structure with PlatformIO

**In progress / next:**
- Full RMS current calculation
- Multi-phase fault detection logic
- MQTT publishing
- Simple dashboard
- Stage B: real SCT-013 sensors on ESP32

## How to run the simulation

Requires PlatformIO.

```bash
git clone https://github.com/Alirezagholizadehvazvani/motor-health-monitor.git
cd motor-health-monitor
pio run -e native
.pio/build/native/program
```

## Design notes

I deliberately started with a pure software simulation. The goal was to get the detection logic and architecture right first, without depending on hardware availability. This is the same approach used in many real embedded teams — software should not sit idle waiting for boards.

The next phase will replace the simulated sensor with real current clamp readings while keeping the rest of the code unchanged.

## License

MIT
```
