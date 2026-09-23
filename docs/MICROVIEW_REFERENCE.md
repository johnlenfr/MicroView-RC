# SparkFun MicroView Hardware & Pin Mapping Reference

<!--
Search metadata
Project: MicroView RC
Category: open-source DIY radio control / RC transmitter and receiver
Core hardware: SparkFun MicroView, ATmega328P, ATtiny85, nRF24L01+
Applications: drone, multirotor, quadcopter, RC car, RC truck, RC boat, RC airplane, fixed-wing aircraft, rover, robot, robotics
Technologies: Arduino, RF24, nRF24L01, telemetry, PWM, servo, ESC, SoftSPI, EEPROM
Related adaptation platforms: ESP32, STM32, RP2040, Raspberry Pi Pico, other microcontrollers
Keywords: Arduino RC, Arduino remote control, Arduino radio control, DIY RC transmitter, DIY RC receiver, RC transmitter Arduino, RC receiver Arduino, open source RC, nRF24L01 RC, RF24 radio control, ATmega328P RC, SparkFun MicroView, ATtiny85, RC telemetry, drone remote control, quadcopter controller, RC car controller, RC boat controller, RC airplane controller, robot remote control, robotics remote control, ESP32 RC project, STM32 RC project, RP2040 RC project, Raspberry Pi Pico RC project
Note: ESP32/STM32/RP2040/Pico are related adaptation/search terms; current firmware targets the MicroView/ATmega328P reference hardware.
-->


This document is the hardware reference for the **SparkFun MicroView** used by the MicroView RC transmitter. It links the module's **16 physical pins** to the Arduino-style pin names used by the firmware and records the upstream SparkFun sources.

<p align="center">
  <img src="images/microview_module.webp" alt="SparkFun MicroView OLED Arduino module" width="300">
</p>

## MicroView in this project

MicroView RC uses the MicroView as its transmitter controller and display platform. The module is based on the **ATmega328P** and integrates a **64 x 48 OLED** in a compact 16-pin package.

For authoritative MicroView hardware information, use the upstream SparkFun sources:

- [SparkFun Learn — MicroView Overview](https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-for-microview/microview-overview-)
- [SparkFun MicroView product repository — v10](https://github.com/sparkfun/MicroView/tree/v10)
- [SparkFun MicroView Arduino Library](https://github.com/sparkfun/SparkFun_MicroView_Arduino_Library)

The **v10 repository** is included here as an important upstream hardware/project reference for builders who want the original SparkFun MicroView design files and project history.

## Pinout

<p align="center">
  <img src="images/microview_pinout.png" alt="SparkFun MicroView physical pin to Arduino code pin mapping" width="700">
</p>

Physical pin numbering proceeds around the 16-pin package. Always distinguish the **physical package pin number** from the **Arduino pin name** used in code.

## Physical pin to Arduino pin mapping

| Physical pin | Arduino pin / power | Functions shown on reference pinout |
|---:|---|---|
| 1 | RESET | Reset |
| 2 | A5 | ADC5, PCINT13, SCL |
| 3 | A4 | ADC4, PCINT12, SDA |
| 4 | A3 | ADC3, PCINT11 |
| 5 | A2 | ADC2, PCINT10 |
| 6 | A1 | ADC1, PCINT9 |
| 7 | A0 | ADC0, PCINT8 |
| 8 | GND | Ground |
| 9 | D0 | RXD, PCINT16 |
| 10 | D1 | TXD, PCINT17 |
| 11 | D2 | INT0, PCINT18 |
| 12 | D3 | INT1, PCINT19, PWM / OC2B |
| 13 | D5 | T1, PCINT21, PWM / OC0B |
| 14 | D6 | AIN0, PCINT22, PWM / OC0A |
| 15 | +5V | 5 V rail |
| 16 | VIN | Input supply |

## MicroView RC transmitter wiring

| MicroView RC function | Arduino pin | MicroView physical pin |
|---|---:|---:|
| nRF24L01+ CE | D0 | 9 |
| nRF24L01+ CSN | D1 | 10 |
| ATtiny85 one-wire data | D2 | 11 |
| nRF24L01+ MOSI (SoftSPI) | D3 | 12 |
| nRF24L01+ MISO (SoftSPI) | D5 | 13 |
| nRF24L01+ SCK (SoftSPI) | D6 | 14 |
| Ground | GND | 8 |
| 5 V rail | +5V | 15 |
| Raw/input supply | VIN | 16 |

> The nRF24L01+ link uses **software SPI** in MicroView RC, so D3/D5/D6 are project/library assignments.

## Builder notes

- Keep physical pin numbers and Arduino code pin names clearly separated in schematics and wiring diagrams.
- The nRF24L01+ radio requires an appropriate **3.3 V supply**; do not power the radio directly from the MicroView 5 V rail.
- Keep the existing SoftSPI assignments consistent with the firmware.
- The current MicroView RC firmware targets the MicroView / ATmega328P. Ports to ESP32, STM32, RP2040 or Raspberry Pi Pico would require firmware and hardware adaptation.

## Sources and attribution

The two reference images in this repository were supplied for documentation use and are attributed to the SparkFun MicroView ecosystem. Upstream reference URLs:

- https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-for-microview/microview-overview-
- https://github.com/sparkfun/MicroView/tree/v10
- https://github.com/sparkfun/SparkFun_MicroView_Arduino_Library

See also [`THIRD_PARTY_ASSETS.md`](THIRD_PARTY_ASSETS.md).
