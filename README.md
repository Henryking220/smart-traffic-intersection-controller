# 🚦 Smart Traffic Intersection Controller

An Arduino-based smart traffic intersection prototype featuring two-road traffic control, countdown timing, pedestrian crossing requests, emergency vehicle priority, audible alerts, LCD status display, and keypad control.

## Project Overview

This project models a two-road traffic intersection using an Arduino UNO. The controller manages the traffic lights automatically while allowing pedestrian requests, emergency priority, manual mode, and system reset through a 4×4 keypad.

The physical prototype is built on a cardboard intersection model with LED traffic signals.

## Features

- 🚦 Two-road traffic signal control
- 🔴🟡🟢 Six traffic LEDs
- ⏱️ LCD countdown timers
- 🚶 Pedestrian crossing request
- 🔊 Audible pedestrian and emergency alerts
- 🚨 Emergency priority for Road A or Road B
- ⌨️ 4×4 keypad control
- 🛑 All-red safety transitions
- 🎛️ Manual mode
- 🔄 Automatic traffic cycling
- 🔁 System reset
- Non-blocking state-machine based traffic operation

## Hardware

- Arduino UNO R3
- LCD1602 with I²C backpack
- 4×4 matrix keypad
- 2 × red LEDs
- 2 × yellow LEDs
- 2 × green LEDs
- 6 × 220 Ω resistors
- Passive buzzer
- Breadboard
- Jumper wires
- USB cable
- Cardboard/cardboard sheet for the intersection model

## Pin Configuration

| Component | Arduino Pin |
|---|---:|
| Road A Red | D2 |
| Road A Yellow | D3 |
| Road A Green | D4 |
| Road B Red | D5 |
| Road B Yellow | D6 |
| Road B Green | D7 |
| Passive Buzzer | D8 |
| Keypad Row 1 | D9 |
| Keypad Row 2 | D10 |
| Keypad Row 3 | D11 |
| Keypad Row 4 | D12 |
| Keypad Column 1 | D13 |
| Keypad Column 2 | A0 |
| Keypad Column 3 | A1 |
| Keypad Column 4 | A2 |
| LCD SDA | A4 |
| LCD SCL | A5 |
| LCD VCC | 5V |
| LCD GND | GND |

Each traffic LED is connected through a 220 Ω current-limiting resistor.

## Keypad Controls

| Key | Function |
|---|---|
| **1** | Queue pedestrian crossing request |
| **A** | Emergency priority — Road A |
| **B** | Emergency priority — Road B |
| **C** | Cancel emergency / return to automatic |
| **D** | Manual mode |
| **#** | Return to automatic mode |
| ***** | System reset |

## Automatic Sequence

The normal traffic cycle is:

```text
Road A GREEN / Road B RED
          ↓
Road A YELLOW / Road B RED
          ↓
ALL RED
          ↓
Road A RED / Road B GREEN
          ↓
Road A RED / Road B YELLOW
          ↓
ALL RED
          ↓
Repeat
```

The all-red states provide a safety transition between traffic directions.

## Pedestrian Mode

Press **1** to queue a pedestrian crossing request.

The controller waits for a safe transition, turns both traffic signals red, starts a pedestrian countdown, and produces audible crossing beeps. The final seconds use faster beeps.

## Emergency Mode

Press **A** for Road A emergency priority or **B** for Road B emergency priority.

The controller enters an emergency state, gives the selected road priority, displays the emergency status and countdown on the LCD, and activates an audible warning.

Press **C** to cancel emergency mode and return to automatic operation.

## Manual Mode

Press **D** to enter manual mode. The prototype holds Road A green and Road B red until automatic mode is restored with **#**.

## Software

The controller is written in Arduino C++ and uses:

- `LiquidCrystal_I2C`
- `Keypad`
- `Wire`

The traffic logic is implemented as a state machine using `millis()` rather than long blocking delays for the main traffic cycle.

## Repository Structure

```text
smart-traffic-intersection-controller/
├── Arduino/
│   ├── SmartTrafficController/
│   │   └── SmartTrafficController.ino
│   └── HardwareTest/
│       └── HardwareTest.ino
└── README.md
```

## How to Run

1. Install the **LiquidCrystal_I2C** and **Keypad** libraries in Arduino IDE.
2. Open `Arduino/SmartTrafficController/SmartTrafficController.ino`.
3. Select **Arduino UNO** as the board.
4. Select the correct COM port.
5. Upload the sketch.
6. Power the controller through USB or an appropriate 5 V supply.
7. Test the automatic sequence and keypad functions.

## Project Status

**Working prototype ✅**

The complete electronic controller has been assembled and tested successfully, including all six LEDs, LCD, passive buzzer, keypad, automatic sequence, pedestrian request, emergency priority, manual mode, and reset.

## Future Improvements

Possible future upgrades include:

- Vehicle detection sensors
- Adaptive traffic timing
- Dedicated pedestrian signal LEDs
- Seven-segment or larger countdown displays
- Emergency vehicle detection
- Wireless monitoring using ESP32
- Web/mobile dashboard
- More advanced manual traffic control
- Cleaner custom PCB implementation

## License

MIT License
