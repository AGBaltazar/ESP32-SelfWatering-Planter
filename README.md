# Description
After burning uot an STM32 with this project and not having an extra, this repo holds the code to power a submersible water pump utilizing an ESP32 Base

## Tools Utilized
ESP32 Board (Base)

MOSFET Transistor

Various Resistors

Jumper Wires 

12v Submersible Water Pump + hoses

Air Purifier Shell

## Wiring Setup 
ESP32 GPIO4 → 100Ω resistor → MOSFET Gate

10kΩ resistor → Gate to GND (pull-down)

MOSFET Source → GND (shared with ESP32 GND)

MOSFET Drain → Pump (−)

Pump (+) → 12V supply (+)

12V supply (−) → same GND rail as ESP32


## Peripherals Utilized
GPIO Output for the LED and Pump

GPIO Input for the Button


## To Do
- [x] GPIO initializing 
- [x] GPIO setup on main page
- [x] Setup Push Button Capabilities
- []  Setup Wifi Capabilites for app control


## Phases
### Phase 1
~~Turning on the water pump at the push of a button~~

### Phase 2
Setup timers to release water at controlled intervals such as specific times

### Phase 3
Utilize the ESP32's built in wifi to remotely power the pump