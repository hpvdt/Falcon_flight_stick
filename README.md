# Falcon Flight Stick
A custom-designed **flight control stick** for the Falcon Aircraft project — designing both **hardware and firmware** for force sensing, CAN communication, and USB HID joystick interfacing.

<img width="600" height="407" alt="image" src="https://github.com/user-attachments/assets/eb6e8dac-e693-49a5-8a84-b895c92fd1f2" />
<img width="1500" alt="image" src="https://github.com/user-attachments/assets/07c229ae-1129-41b0-90cb-bf1da6967c3e" />

---

## Overview
The **Falcon Flight Stick** is a force-sensing joystick that measures user input via strain gauges and communicates with the aircraft’s control system through **CAN bus** or to a computer host via **USB-C**.  
This project demonstrates my ability to proficiently design, route, and program an embedded system from the ground up — including power delivery, signal integrity, and firmware architecture.

---

## Key Features
- **MCU Core:** STM32F103CBU6 managing all board-level events with an external 8Mhz Oscillator
- **Sensing System:** Three strain gauges read via a precision Texas Instruments ADS131M03IRUK ADC (32-bit SPI interface)  
- **Connectivity:**  
  - USB-C (HID joystick or virtual COM port)  
  - CAN bus daisy-chain communication  
- **Memory:** 1×128-byte EEPROM for calibration data 
- **Power System:**  
  - TI TPS62177 step-down converter for higher current ditial power
  - ADI LT1962 LDO (3.3V)  for low noise, low current analog power
  - Automatic protection between USB-C and LiPo (12V) sources 
- **PCB Stackup:** 4-layer board with separated analog/digital planes for signal integrity
- **Breakout Button Matrix:**
  - button matrix allowing for a hotswappable and fully customizable button interface on the joystick
  - Up to 25 individual buttons, all allowable for interrupts

---

## Hardware Design

### Hardware Repository
The latest hardware version is **Falcon Flight Stick Rev5**.  
Previous revisions were experimental or undocumented.

### Design Breakdown
<img src="https://github.com/user-attachments/assets/4b1ab422-8b88-4cf3-a601-986f41056db9" width="800"/>

The STM32 MCU is programmed through **ST-LinkV3 (SWD)** and communicates via **UART** for debug and development.  
Differential USB data lines enable HID or COM functionality, depending on firmware.  

Strain gauge signals are processed through a TI ADC, which triggers SPI reads via a DRDY interrupt.  
Processed values are sent to the servo controller board (via CAN) or USB interface.  

---

## Power Architecture
<img src="https://github.com/user-attachments/assets/f5297074-4fee-44f9-927d-339fe0df579f" width="800"/>

The power subsystem ensures stable operation from multiple sources:
- **12V LiPo (CAN chain)** or **5V USB-C**
- Both inputs are protected from backfeed
- Outputs:  
  - 3.3V Digital (MCU, logic)  
  - 3.3V Analog (ADC, strain gauge front-end)

These rails are physically isolated to minimize noise and ground-reference drift.

---

## Schematics
Full schematic set:  
![Schematic](https://github.com/user-attachments/assets/4670dbb4-5875-4a96-b333-24631d5623e4)

---

## PCB Layout

### Summary
- 4-layer stackup for power and noise isolation  
- Separated analog/digital ground and power planes  
- Power trace widths sized by current draw  
- Labelled parts for developer readability  

### Views
**Full Layout:**  
<img src="https://github.com/user-attachments/assets/a44c9da9-d550-4acb-bf7e-374860fbc576" width="800"/>

**Layer 1: Signal**  
<img src="https://github.com/user-attachments/assets/dba7df1a-45fc-4ba4-8cf5-f24956843040" width="800"/>

**Layer 2: Power**  
<img src="https://github.com/user-attachments/assets/f3745a84-55aa-498d-80d0-7baa79fde328" width="800"/>

**Layer 3: GND / AGND**  
<img src="https://github.com/user-attachments/assets/cf7ae8ee-e237-467c-b778-f995c1fbce8f" width="800"/>

**Layer 4: Signal 2**  
<img src="https://github.com/user-attachments/assets/6ab57140-dbd9-4f45-8b1c-8da6f2f88619" width="800"/>

---

## 3D Model
<img src="https://github.com/user-attachments/assets/be69b35a-b878-4c21-af88-f7dd6fc7a3a2" width="600"/>
<img src="https://github.com/user-attachments/assets/e57b2e52-91ee-42b5-b05f-f029a544abdc" width="600"/>

---

## Firmware (Planned / In Progress)

Example outline:
- USB HID joystick emulation via STM32 HAL  
- CAN message handling for actuator control  
- ADC data acquisition and digital filtering  
- EEPROM calibration storage  

---


## Tools Used
- **KiCad 8.0** – PCB and schematic design  
- **STM32CubeIDE** – Firmware development  & MCU pinout planning
- **JLCPCB** – Fabrication and assembly  
- **SolidWorks** – 3D mechanical integration  
- **GitHub / Git** – Version control  

---
