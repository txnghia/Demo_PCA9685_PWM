# UNO Q Universal Actuator Controller

## Overview

The **UNO Q Universal Actuator Controller** is a demonstration and development platform built around:

* Arduino UNO Q
* PCA9685 16-channel PWM controller
* UNO Q WebUI
* Socket.IO browser communication
* RouterBridge MPU-to-MCU communication
* I2C hardware control

The purpose of the project is to demonstrate how a single UNO Q and PCA9685 can control many different types of actuators and output devices from a browser-based graphical interface.

The project can also serve as a reusable hardware and software foundation for future robotics and automation systems.

---

# 1. System Architecture

The complete communication path is:

```text
Browser
   |
   | Socket.IO
   v
UNO Q MPU / Linux
Python WebUI
   |
   | Bridge.call()
   v
UNO Q MCU
Arduino RouterBridge
   |
   | I2C
   v
PCA9685
   |
   +--> Servos
   +--> Motor PWM
   +--> LED PWM
   +--> Filtered Analog Outputs
   +--> Relay Driver Signals
   +--> TRIAC Interface Signals
```

The system separates high-level application functions from low-level hardware control.

```text
MPU
High-level control

WebUI
User interface
Networking
Patterns
Automation
AI integration
Logging

            ↓ RouterBridge

MCU
Low-level hardware

PCA9685
I2C
PWM generation
Real-time actuator control
```

---

# 2. Project Goals

The controller demonstrates:

* Four servo motors
* Two DC motor PWM outputs
* Two high-power LED PWM outputs
* Two PWM-to-analog outputs
* Two sinewave generators
* Two TRIAC control signals
* Two relay control signals
* Automatic demonstration patterns
* Live status reporting
* Diagnostics

The system is intended both as a PCA9685 learning platform and as a reusable UNO Q actuator controller.

---

# 3. PCA9685 Channel Assignment

The initial channel map is:

| PCA9685 Channel | Function              |
| --------------- | --------------------- |
| CH0             | Servo 1               |
| CH1             | Servo 2               |
| CH2             | Servo 3               |
| CH3             | Servo 4               |
| CH4             | Left DC Motor PWM     |
| CH5             | Right DC Motor PWM    |
| CH6             | High-Brightness LED 1 |
| CH7             | High-Brightness LED 2 |
| CH8             | Analog Output 1       |
| CH9             | Analog Output 2       |
| CH10            | TRIAC Signal 1        |
| CH11            | TRIAC Signal 2        |
| CH12            | Relay Signal 1        |
| CH13            | Relay Signal 2        |
| CH14            | Spare                 |
| CH15            | Spare                 |

This uses 14 of the 16 available PCA9685 outputs.

Two channels remain available for future expansion.

---

# 4. WebUI Layout

The WebUI is divided into several panels:

```text
UNO Q Universal Actuator Controller

├── Servo Panel
├── PWM Panel
├── Analog Panel
├── Function Generator
├── Relay Panel
├── TRIAC Panel
├── Demo Panel
├── Status Panel
└── Diagnostics Panel
```

Each panel demonstrates a different function of the PCA9685 and UNO Q architecture.

---

# 5. Servo Panel

The Servo Panel controls four standard RC servo motors.

```text
Servo 1   [ slider 0° ───────── 180° ]
Servo 2   [ slider 0° ───────── 180° ]
Servo 3   [ slider 0° ───────── 180° ]
Servo 4   [ slider 0° ───────── 180° ]
```

Possible additional buttons:

```text
[0°] [90°] [180°]

[Center All]

[Sweep]
```

Servo channel mapping:

```text
CH0 Servo 1
CH1 Servo 2
CH2 Servo 3
CH3 Servo 4
```

Typical servo PWM frequency:

```text
50 Hz
```

One period is:

```text
20 ms
```

Typical servo pulse widths are approximately:

```text
0°   ≈ 500 µs

90°  ≈ 1500 µs

180° ≈ 2500 µs
```

The MCU converts servo angle into PCA9685 counts before sending the command over I2C.

---

# 6. PWM Panel

The PWM Panel demonstrates general-purpose duty-cycle control.

It includes two DC motor outputs and two LED outputs.

## DC Motors

```text
Motor Left

PWM:  [0% ─────────────── 100%]


Motor Right

PWM:  [0% ─────────────── 100%]
```

Channel assignment:

```text
CH4 = Left Motor PWM
CH5 = Right Motor PWM
```

The PCA9685 does not drive the motors directly.

The proper signal path is:

```text
PCA9685
   |
PWM
   v
MOSFET / Motor Driver
   |
   v
DC Motor
```

For simple one-direction motor control:

```text
PWM → MOSFET → Motor
```

For bidirectional motor operation, a proper H-bridge motor driver should be used.

---

# 7. High-Brightness LED PWM

The same PWM Panel can control two high-power LEDs.

```text
LED 1

Brightness:
[0% ─────────────── 100%]


LED 2

Brightness:
[0% ─────────────── 100%]
```

Channel assignment:

```text
CH6 = LED 1
CH7 = LED 2
```

The output path is:

```text
PCA9685 PWM
      |
      v
MOSFET / Current Driver
      |
      v
High-Power LED
```

The PCA9685 must not directly supply high-current LEDs.

---

# 8. Analog Panel

PCA9685 is a PWM controller, not a DAC.

However, a PWM signal can be converted into an approximate analog voltage with a low-pass filter.

Channel assignment:

```text
CH8 = Analog Output 1
CH9 = Analog Output 2
```

The signal path is:

```text
PCA9685 PWM
       |
       R
       |
       +------ Analog Output
       |
       C
       |
      GND
```

For example:

```text
R = 3.3 kΩ

C = 1 µF
```

The output voltage depends approximately on the PWM duty cycle.

With a 5 V PWM supply:

```text
0% duty   ≈ 0 V

25% duty  ≈ 1.25 V

50% duty  ≈ 2.5 V

75% duty  ≈ 3.75 V

100% duty ≈ 5 V
```

The WebUI can therefore display:

```text
Analog Output 1

Voltage:
[0.0 V ─────────────── 5.0 V]


Analog Output 2

Voltage:
[0.0 V ─────────────── 5.0 V]
```

An op-amp buffer can be added after the RC filter if the analog output must drive a significant load.

---

# 9. Function Generator Panel

The analog outputs can also be used as simple low-frequency waveform generators.

The controller changes PCA9685 duty cycle over time.

After the RC low-pass filter, the changing duty cycle becomes a varying analog voltage.

The initial function generator supports:

```text
Sinewave 1

Frequency: [     ] Hz
Amplitude: [     ]
Offset:    [     ]

[START] [STOP]
```

and:

```text
Sinewave 2

Frequency: [     ] Hz
Amplitude: [     ]
Offset:    [     ]

[START] [STOP]
```

Typical output:

```text
0 V
  \
   \
    2.5 V
       \
        \
         5 V
```

For a full-range 0–5 V sine:

```text
Vout = 2.5 + 2.5 × sin(2πft)
```

The MCU periodically calculates the desired instantaneous voltage.

That voltage is converted into PCA9685 PWM duty cycle.

The RC filter smooths the PWM into an analog waveform.

This function generator is intended for low-frequency demonstrations rather than precision signal generation.

Possible future waveform modes include:

```text
Sine

Triangle

Sawtooth

Square

Ramp
```

---

# 10. Relay Panel

Two outputs are reserved for relay control.

```text
CH12 = Relay 1
CH13 = Relay 2
```

WebUI:

```text
Relay 1

[ON] [OFF]


Relay 2

[ON] [OFF]
```

PCA9685 must not directly drive the relay coil.

Use:

```text
PCA9685
   |
   v
Transistor / MOSFET
   |
   v
Relay Coil
```

For electromagnetic relays, include a flyback diode across the relay coil.

Example:

```text
                +5V
                 |
               Relay
                 |
                 +------ MOSFET
                           |
                          GND

PCA9685 ------ Gate Driver
```

The relay contacts may then control an isolated external load.

---

# 11. TRIAC Panel

Two channels are reserved for AC lamp control interfaces.

```text
CH10 = TRIAC Signal 1
CH11 = TRIAC Signal 2
```

The WebUI can initially provide:

```text
AC Lamp 1
[ON] [OFF]

AC Lamp 2
[ON] [OFF]
```

The PCA9685 output must never connect directly to mains voltage.

The low-voltage controller should be isolated using an opto-triac interface.

Conceptually:

```text
PCA9685
   |
Low-voltage logic
   |
Opto-isolator
   |
TRIAC
   |
120 VAC Lamp
```

For simple ON/OFF AC switching, zero-cross opto-triac devices may be appropriate.

For actual AC phase-angle dimming, a separate zero-cross detector and precisely timed MCU trigger system are required.

PCA9685 free-running PWM is not a substitute for proper phase-angle AC dimming.

---

# 12. Demo Panel

The Demo Panel provides automatic sequences that demonstrate multiple channels at once.

Available modes include:

```text
[Wave]

[Fade]

[Blink]

[Dance]

[All Off]
```

---

# 13. Wave Demo

The Wave mode primarily demonstrates coordinated servo control.

Example sequence:

```text
Servo 1: 40° → 140°

Servo 2: 140° → 40°

Servo 3: 40° → 140°

Servo 4: 140° → 40°
```

Then the motion reverses.

The effect resembles a mechanical wave.

This demonstrates that several PCA9685 channels can be changed independently.

---

# 14. Fade Demo

Fade demonstrates PWM control.

For example:

```text
LED 1

0% → 100% → 0%
```

while:

```text
LED 2

100% → 0% → 100%
```

The analog outputs can follow the same pattern:

```text
Analog 1

0 V → 5 V


Analog 2

5 V → 0 V
```

This clearly demonstrates the relationship between:

```text
PWM duty cycle
        ↓
LED brightness
        ↓
Filtered analog voltage
```

---

# 15. Blink Demo

Blink toggles several outputs periodically.

Example:

```text
LED 1 ON
LED 2 ON

Relay 1 ON
Relay 2 ON

        ↓

300 ms

        ↓

LED 1 OFF
LED 2 OFF

Relay 1 OFF
Relay 2 OFF
```

This sequence can repeat several times.

---

# 16. Dance Demo

Dance combines several actuator types.

For example:

```text
Servo motion

+

Motor PWM

+

LED PWM
```

A typical sequence might be:

```text
Servo 1 → 40°
Servo 2 → 140°

LED 1 → 100%
LED 2 → 0%

Motor Left → 60%
Motor Right → 20%
```

Then:

```text
Servo 1 → 140°
Servo 2 → 40°

LED 1 → 0%
LED 2 → 100%

Motor Left → 20%
Motor Right → 60%
```

The cycle repeats to create a coordinated actuator demonstration.

---

# 17. All Off

The All Off button puts the system in a safe idle state.

Typical behavior:

```text
Motors = 0%

LEDs = 0%

Analog outputs = 0 V

Sine generators = OFF

Relays = OFF

TRIAC commands = OFF

Servos = 90°
```

This is useful both operationally and during debugging.

---

# 18. Status Panel

The Status Panel displays the current controller state.

Example:

```text
Socket: Connected

MCU: Connected

PCA9685: Active

Servo 1: 90°
Servo 2: 90°
Servo 3: 90°
Servo 4: 90°

Motor L: 0%
Motor R: 0%

LED 1: 25%
LED 2: 60%

Analog 1: 2.50 V
Analog 2: 4.00 V

Sine 1: ON
Sine 2: OFF

Relay 1: OFF
Relay 2: ON
```

Python maintains a state structure such as:

```python
state = {
    "servo": [90, 90, 90, 90],

    "motor_l": 0,
    "motor_r": 0,

    "led1": 0,
    "led2": 0,

    "analog1": 0.0,
    "analog2": 0.0,

    "sine1": 0,
    "sine2": 0,

    "relay1": 0,
    "relay2": 0,

    "triac1": 0,
    "triac2": 0,

    "last_demo": "none"
}
```

Python sends this information to the browser using:

```python
ui.send_message(
    "status_update",
    state
)
```

The browser receives the message using Socket.IO.

---

# 19. Diagnostics Panel

The Diagnostics Panel is intended to make hardware and communication problems easier to identify.

Recommended information includes:

```text
WebUI Socket

CONNECTED / DISCONNECTED


RouterBridge

CONNECTED / ERROR


PCA9685

I2C Address: 0x40


PWM Frequency

50 Hz


I2C Status

OK / ERROR


Commands Received

1234


Last Command

servo_set


Last MCU Result

OK servo
```

Possible future diagnostics include:

```text
I2C scan

PCA9685 register check

MCU uptime

MPU uptime

Bridge error counter

Socket reconnect counter

PCA9685 oscillator calibration

Servo supply voltage

Servo current
```

---

# 20. Browser-to-MPU Communication

The browser uses Socket.IO.

The page loads:

```html
<script src="libs/socket.io.min.js"></script>
```

JavaScript creates the connection:

```javascript
socket = io(
    `http://${window.location.host}`
);
```

A servo command might be:

```javascript
socket.emit(
    "servo_set",
    {
        servo: 0,
        angle: 120
    }
);
```

Python registers the corresponding handler:

```python
ui.on_message(
    "servo_set",
    servo_set
)
```

The communication therefore becomes:

```text
Browser

socket.emit("servo_set")

        ↓

Python

servo_set(client, data)
```

---

# 21. MPU-to-MCU Communication

Python communicates with the MCU using RouterBridge.

Example:

```python
Bridge.call(
    "servo",
    "0,120"
)
```

The MCU registers:

```cpp
Bridge.provide(
    "servo",
    rpc_servo
);
```

Therefore:

```text
Python

Bridge.call("servo", "0,120")

        ↓

RouterBridge

        ↓

MCU

rpc_servo("0,120")
```

The MCU then converts the command into a PCA9685 I2C transaction.

---

# 22. MCU-to-PCA9685 Communication

The MCU controls PCA9685 with I2C.

Initialization:

```cpp
Wire.begin();

pca.begin();

pca.setPWMFreq(50);
```

Changing a PWM channel:

```cpp
pca.setPWM(
    channel,
    0,
    pwm_count
);
```

Communication path:

```text
MCU
 |
 | SDA / SCL
 v
PCA9685
```

The PCA9685 then continuously generates PWM without requiring the MCU to manually toggle every output.

This is one of the major advantages of using PCA9685.

---

# 23. Why PCA9685 Is Useful

Without PCA9685, the MCU would need to generate many output waveforms itself.

With PCA9685:

```text
MCU

send I2C command once

        ↓

PCA9685

continues generating PWM
```

The MCU is then free to handle:

```text
Sensors

Bridge communication

Control loops

Safety logic

Timing

Other peripherals
```

while the MPU handles:

```text
Web interface

Networking

Files

AI

User interaction

High-level automation
```

---

# 24. Power Architecture

The system should use separate power paths.

```text
UNO Q
 |
 +---- Logic power


PCA9685 VCC
 |
 +---- Logic supply


Servo / Motor / LED Supply
 |
 +---- Power devices
```

All low-voltage control grounds should normally share a common reference:

```text
UNO Q GND
PCA9685 GND
Servo power GND
Motor driver GND
LED driver GND
```

High-power loads must not be powered from the UNO Q.

The PCA9685 output pins should be treated as control signals, not power outputs.

---

# 25. Safety

Special care is required with:

```text
120 VAC

TRIAC circuits

Relay contacts

High-current motors

High-power LEDs
```

The low-voltage UNO Q/PCA9685 system should remain electrically isolated from mains voltage wherever possible.

Use appropriate:

```text
Opto-isolation

Fuses

Enclosures

Terminal spacing

Wire ratings

Grounding

Power supplies
```

The TRIAC portion of this project should first be tested with low-voltage indicators before mains hardware is attached.

---

# 26. Debugging Strategy

The recommended troubleshooting sequence is bottom-up.

## Stage 1

Test:

```text
MCU
↓
I2C
↓
PCA9685
↓
One Servo
```

This verifies the hardware bus.

---

## Stage 2

Test:

```text
Python
↓
Bridge.call()
↓
MCU
↓
PCA9685
```

Example:

```python
Bridge.call(
    "pca/test",
    ""
)
```

This verifies RouterBridge.

---

## Stage 3

Test:

```text
Browser
↓
Socket.IO
↓
Python
```

Use a simple button and Python print statement.

---

## Stage 4

Combine:

```text
Browser
↓
Socket.IO
↓
Python
↓
RouterBridge
↓
MCU
↓
I2C
↓
PCA9685
↓
Hardware
```

This layered approach proved very useful during development of the project.

---

# 27. Project Directory

A typical project layout is:

```text
pca9685_demo/

├── sketch/
│   └── sketch.ino
│
├── python/
│   └── main.py
│
└── web/
    ├── index.html
    ├── app.js
    ├── style.css
    │
    └── libs/
        └── socket.io.min.js
```

The important Socket.IO library path in this UNO Q environment is:

```html
<script src="libs/socket.io.min.js"></script>
```

---

# 28. Future Improvements

The Universal Actuator Controller can later add:

```text
PWM frequency control

Per-channel calibration

Servo limits

Servo motion profiles

Motor ramping

LED breathing

Heartbeat lighting

Triangle wave

Sawtooth wave

Waveform amplitude control

Waveform DC offset control

Web oscilloscope

ADC feedback

Current sensing

Voltage sensing

Emergency stop

Configuration storage

AI control
```

---

# 29. Connection to Future Projects

The Universal Actuator Controller is designed as a reusable test platform.

For example, the future Đàn Bầu Robot can reuse:

```text
UNO Q WebUI

Socket.IO

RouterBridge

PCA9685 driver

Servo control

Calibration

Status reporting

Demo sequencing
```

The application layer then changes from:

```text
Servo 1
Servo 2
Servo 3
```

to:

```text
Pick Sweep

Pick Lift

Harmonic H2

Harmonic H3

Harmonic H4

Harmonic H5

Harmonic H6

Harmonic H8

Mute

Bend
```

The communication architecture remains the same.

---

# 30. Summary

The UNO Q Universal Actuator Controller demonstrates how UNO Q can combine Linux-level intelligence with MCU-level hardware control.

The complete architecture is:

```text
Browser
   |
Socket.IO
   |
WebUI / Python
   |
RouterBridge
   |
UNO Q MCU
   |
I2C
   |
PCA9685
   |
16 PWM Outputs
```

Those PWM outputs can then control multiple kinds of hardware through appropriate interface circuits:

```text
Servos

DC Motors

High-Power LEDs

Filtered Analog Outputs

Waveform Outputs

Relays

TRIAC Interfaces
```

The project is therefore both:

1. A PCA9685 demonstration platform.
2. A reusable UNO Q actuator-control framework for future robotics and automation projects.
