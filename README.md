# 3-Phase Motor Health Monitor

An embedded monitoring system for 3-phase induction motors, built around two
failure modes that are common — and expensive — in industrial settings such
as deep-well submersible water pumps: **phase imbalance / single-phasing**
and **dry-running**.

> **Status: Stage A, in progress.** This project is being built in two
> deliberate stages (see [Architecture](#architecture) below). Stage A
> (software simulation, no hardware required) is under active development.
> Stage B (real ESP32 + current clamp sensors) begins once hardware arrives.
> This is intentional engineering practice — build and prove out firmware
> logic against a simulated Hardware Abstraction Layer before hardware
> exists, then swap in the real sensor implementation without touching the
> business logic — not a limitation of the project.

## Problem Statement

In 3-phase motor-driven equipment — deep-well submersible pumps in
particular — two failure modes account for a large share of preventable
motor burnout:

- **Phase imbalance / single-phasing**: one phase conductor loses current
  relative to the others (a blown fuse, a burned contactor point, a loose
  terminal). The motor keeps attempting to run on the remaining phases,
  drawing excessive current on them and overheating the windings — often
  destroying the motor within minutes if not caught.
- **Dry-running**: a submersible pump loses its water load (well runs dry,
  intake is blocked) and spins with little resistance. Current drops well
  below the rated load current, but the pump keeps running — leading to
  bearing and seal damage from lack of the cooling/lubrication the pumped
  fluid normally provides.

Both conditions have a distinct, detectable current signature. This project
implements that detection logic in firmware, with the eventual goal of
publishing live readings and fault alerts over MQTT to a dashboard.

## Architecture

```
                    ┌─────────────────────────┐
                    │   Fault Detection Logic   │   <- doesn't know or care
                    │   (imbalance / phase-loss │      where readings come
                    │    / dry-run thresholds)  │      from
                    └────────────┬──────────────┘
                                 │
                    ┌────────────▼──────────────┐
                    │      ICurrentSensor        │   <- the HAL boundary
                    │        (interface)         │
                    └──────┬──────────────┬───────┘
                            │              │
              ┌─────────────▼───┐   ┌──────▼─────────────┐
              │ SimulatedCurrent │   │  SCT013Current      │
              │     Sensor       │   │     Sensor          │
              │   (Stage A)      │   │    (Stage B)        │
              │  generates a     │   │  reads a real        │
              │  realistic sine  │   │  SCT-013 clamp via   │
              │  wave in software│   │  the ESP32's ADC     │
              └──────────────────┘   └──────────────────────┘
```

Everything above the `ICurrentSensor` line — RMS calculation, fault
detection, MQTT publishing — is written once and never changes between
stages. Only the box below the line is swapped.

| | Stage A (now) | Stage B (once hardware arrives) |
|---|---|---|
| Sensor input | `SimulatedCurrentSensor`: software-generated sine wave with selectable fault scenarios | `SCT013CurrentSensor`: real current clamp readings via ESP32 ADC |
| Runs on | Your dev machine, via PlatformIO's `native` platform | Physical ESP32 |
| Purpose | Prove out fault-detection logic, MQTT layer, and dashboard end-to-end with zero hardware dependency | Validate the same logic against real electrical noise and real motor behavior |

## Current Progress

- [x] `ICurrentSensor` hardware abstraction interface
- [x] `SimulatedCurrentSensor` — sine wave generator with `NORMAL` /
      `PHASE_LOSS` / `IMBALANCE` / `DRY_RUN` scenarios
- [x] Single simulated phase reading, verified end-to-end
- [ ] RMS current calculation
- [ ] Multi-phase fault detection logic (imbalance, single-phasing, dry-run)
- [ ] MQTT publishing (readings + fault states)
- [ ] Node-RED / web dashboard
- [ ] Stage B: real SCT-013 + ESP32 implementation

## Setup & Usage

Requires [PlatformIO](https://platformio.org/) (`pip install platformio` or
`brew install platformio`).

```bash
git clone <your-repo-url>
cd motor-health-monitor
pio run -e native              # compiles for your host machine, no hardware needed
.pio/build/native/program       # runs the simulation
```

## Demo

_placeholder — screenshot / GIF of the live dashboard will go here once the
MQTT + dashboard stage is complete._

## Why Simulate First?

This project deliberately separates *firmware logic correctness* from
*hardware availability*. The `ICurrentSensor` interface means the fault
detection algorithms, MQTT publishing, and dashboard can all be built,
tested, and demonstrated on a laptop — and the exact same logic will run
unmodified on the real ESP32 once hardware is available. This mirrors how
real embedded teams work: hardware is often the long pole, and software
shouldn't sit idle waiting for it.

## Limitations & Future Work

- Currently simulation-only (Stage A); real hardware integration (Stage B)
  is planned as ESP32 + SCT-013 sensors become available
- No persistence/historical trending yet
- Future: ML-based predictive maintenance (trending toward failure before
  hard thresholds are crossed)
- Future: industrial Modbus/RS-485 integration for interfacing with existing
  plant SCADA/PLC systems

## License

MIT — see [LICENSE](LICENSE).
