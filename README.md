# Description
After burning out an STM32 with this project and not having an extra, this repo holds the code to power a submersible water pump utilizing an ESP32 Base.

Add ons here would include a VEML7700 light sensor along with a soil sensor for autonomous releases.

In the works would be a Touch Screen along with customer 3D printed enclosure.

## Tools Utilized
ESP32 Board (Base)

IRLZ44 MOSFET Transistor

100Ω and 10kΩ Resistor

1N4007 Diode

Jumper Wires 

12v Submersible Water Pump + hoses

Air Purifier Shell

## Wiring Setup 
### Sumbersible Pump
ESP32 GPIO4 → 100Ω resistor → MOSFET Gate

10kΩ resistor → Gate to GND (pull-down)

MOSFET Source → GND (shared with ESP32 GND)

MOSFET Drain → Flyback Diode → Pump (−)

Pump (+) → Flyback Diode → 12V supply (+)

12V supply (−) → same GND rail as ESP32

### Water Sensor
Sensor GND → ESP32 GND

Sensor 3V3 → ESP32 3V3

Sensor S → GPIO 33 (ADC Channel 5)

### Light Sensor (VEML 7700)
SCL → GPIO 16

SDA → GPIO 17

GND → ESP32 GND

VDD → ESP32 3V3

## Peripherals Utilized
GPIO Output for the LED and Pump

GPIO Input for the Button

## To Do
- [x] GPIO initializing 
- [x] GPIO setup on main page
- [x] Setup Push Button Capabilities
- [x] Setup Wifi Capabilites for app control
- [ ] Enable status via Web
- [ ] Setup optional timed releases
- [ ] Enable custom timed releases
- [ ] Add on Buck Converter for standalone power
- [x] Add soil sensor for automated releases
- [x] Add on I2C Light Sensor
- [ ] Add on Display over SPI
- [ ] Setup touch display


## Phases
### Phase 1
~~Turning on the water pump at the push of a button~~

### Phase 2
~~Setup timers to release water at controlled intervals such as specific times~~

### Phase 3
~~Utilize the ESP32's built in wifi to remotely power the pump~~

### Phase 4
Install a DC to DC Buck Converter to allow the MCU to get power

~~Install a Soil Sensor and allow the MCU to water when dry~~

### Phase 5
~~Install I2C light sensor to allow for releases during daytime~~

### Phase 6
Install a touchscreen that shows data such as:
- [] Current Soil Wetness
- [] Last Watered
- [] Push Button Override
- [] Current Light Levels

### Phase 7 (Final)
- [] Customer Enclosure and 3D printed adapters
