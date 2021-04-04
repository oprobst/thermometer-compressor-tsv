# thermometer-compressor-tsv

Basicly this is a thermometer. It measures up to 3 values within a hirh range of (min. -50 to 350 degrees) using Thermoelement sensors. Additionally a port for DS18B20 is implemented (usually -55 to 125 degrees).
It is equiped with a display and a hard coded alarm limit.

Major requirement is to keep it simple (pure arduino code only) and modular (modules should be easy replaceable).

# Software

I kept the whole logic in this single script to make it easily adjustable for beginners. So there is just one arduino file.

## Arduino libraries in use
* U8g2 (version 2.28.10) Control LCD Display with ST7820 driver.
* OneWire (version 2.3.5) OneWire Bus protocol and driver.
* DallasTemperature (version 3.9.0) Helper library to maintain DS18S20 Temperature sensor.
* Adafruit MAX31855K (version 1.3.0) For reading the MAX 31855K Temperature sensors
* Adafruit BusIO (version 1.7.2) dependency of Adafruit MAX31855 library.

## Functionality

In general, this script consists of 3 major parts:

  1. Get temperature from a bunch of Dallas DS18B20 OneWires sensors. 
     These sensors usually range from -55 ... +85 °C, which is fine for room and outdoor temperature.
  2. Get up to 3 temperatures from K-Typ Thermoelements connected to the µController by a MAX31855 Chip . 
     The design focus on breakout modules easily be added to the ciruit and are replaceable in case of a defect.
     Range depends on type, up to > +1000 °C.
  3. A UI part for a display via the U8g2 lib.


## Components
Goal is to keep the amount of soldered component small and to use standardized plug-in modules. They can be replaced in case of a damage.

### Moduls
* [Arduino Nano (or clone)](https://www.reichelt.de/arduino-nano-v3-atmega-328-mini-usb-arduino-nano-p142943.html?PROVID=2788&gclid=Cj0KCQjw3duCBhCAARIsAJeFyPWbRqnDvxeFE9hGWIxeH9UpAeioCHqXIbZ7PTT7xyMbCtL9xDiokTYaAkQkEALw_wcB)
* [LCD Display 20x4 Character ](https://eckstein-shop.de/Graphic-128x64-LCD-Display-Module-12864-White-on-Blue-5V-Header-Strip?curr=EUR&gclid=Cj0KCQjw3duCBhCAARIsAJeFyPV9fqUd5TVanYePv4DA6xzIegzKWH7t7PSGK8yXDp_RgOqrNXzDfOYaAqsnEALw_wcB)
* 3x [Thermosensor MAX31855 Module](https://www.androegg.de/?product=max31855-k-type-temperatur-sensor-spi-konverter-modul)
* 3x [Rohranlegefühler Typ K (or similar)](https://www.sensorshop24.de/rohranlegefuehler-mit-alu-prisma-mit-nicr-ni-typ-k)
* [12V buzzer](https://www.conrad.de/de/p/tru-components-1567766-piezo-signalgeber-geraeusch-entwicklung-105-db-spannung-12-v-intervallton-1-st-1567766.html)

### On Board
|  ID  | Description |
| --- | -------- |
| R1 | 4,7 kOhm |
| R2 | 150 Ohm  |
| R3 | 10 kOhm  |
| R4 | 10 kOhm Potentiometer (optional) If display circuit has none included |
| U1 | [N-Channel Mosfet](https://www.conrad.de/de/p/infineon-technologies-irlz44npbf-mosfet-1-hexfet-110-w-to-220-162877.html) with 5V Logic-Level Gate |
| IC1 | 7805 Linear Regulator or 5V Output DC/DC Regulator |
| C1 | 330 nF Ceramic Condensator |
| C2 | 100 nF Ceramic Condensator |
| C4 | 220 µF Electrolyte Condensator |
| D1 | 1N4007 Diode |


