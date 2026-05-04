# MLS-Detection-Simulation
Simulate a system designed to assist pilots landing airplanes on runways.

### Description
This project is a simple simulation of a real aircraft landing system that was developed to assist pilots in low visiblity conditions.
The scope of the implementation was drastically reduced to allow a team of unfamiliar junior engineering students to piece the product together in a short time frame.


### Equipment needed:
- 2 MCUs, Arduino or equivalent development board
- Oscilloscope
- Breadboard and wires

### How to run:
1. Upload carrier.ino onto one MCU
2. Upload example_timing.ino onto another MCU
3. Connect the two MCUs IAW:

| Pin (MCU) | Connect To |
|---------|---------|
| A0 (carrier)| O-scope |
| 0 (timing)| 1 (carrier)|
| 5 (timing)| 1 (carrier)|
| 7 (timing)| 7 (carrier)|
| 9 (timing)| 9 (carrier)|
| 10 (timing)| 10 (carrier)|
| 11 (timing)| 11 (carrier)|
| 12 (timing)| 12 (carrier)|
| 13 (timing)| 13 (carrier)|
| 19 (timing)| 19 (carrier)|

5. Connect the O-scope to the carrier MCU's output
6. Fine tune the trace to see the desired characteristics.

