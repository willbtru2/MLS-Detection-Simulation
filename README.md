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
| A0 | 1 |
| 0 |  1|
| 2 | 1 |
| 5 | 1 |
| 7 | 1 |
| 9 | 1 |
| 10 | 1 |
| 11 |  1|
| 12 | 1 |
| 13 | 1 |
| 19 | 1 |

5. Connect the O-scope to the carrier MCU's output
6. Fine tune the trace to see the desired characteristics.

