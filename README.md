# Falcon_flight_stick
Flight Stick HW and SW files for the Falcon Aircraft

## Hardware
The latest version of hardware for this project is the Falcon Flight Stick Rev5, as previous versions were either not recorded in github, or previously in Falcon_Hardware/flight_stick

This design features new changes such as:
- a properly routed USB-C connector
- overhauled power distribution system
- A daisy-chained CAN communication network, linking all electronics on board
- a 1x128 Byte EEPROM chip to store calibration data

### Design Breakdown
<img width="946" height="737" alt="image" src="https://github.com/user-attachments/assets/04e3d467-ea19-4368-9c50-fca01469f35d" />

The flightstick is centred around the STM32F103CBU6 which manages almost all events on the board.
The MCU is programmed using an STLinkV3 via serial wire debug and uses a peripheral UART port to send and recieve messages to develop/debug firmware development.
The STM32 is also connected to two differential impedance controlled data lines to the USB-C connector which acts as both a virtual COM port or an HID "plug-and-play" joystick depending on the firmware flashed.
There are header holes around the inner diameter of the board, labelled R (roll), P (pitch), and Y (you guess). These holes are soldered to strain gauges that are fastened to a metal shaft on which the user applies force to. These strain gauge values change based on how much force the user is pushing the flight stick shaft in a given direction.
The STM32 is connected to a T.I. ADC that takes the signal of these three strain gauges and, with 32-bit precision, sends them to the MCU after triggering an event with its DRDY (data ready) output, starting an SPI read. 
These values are then processed and converted into meaningful, readable values and are either sent out to the servo controller board to change the position of the wings, or sent over USBC to control the plug-and-play joystick. 

The power system revolves around a Texas Instruments Stepdown converter, the TPS62177DQCR, and an ADI LDO, the LT1962-3.3V. Both take either the +12V power from the CAN daisy chain connected to the LiPo battery, or the +5V USB-C connected to a power source or a computer. Both 5V and 12V sources are protected in case of both power sources being connected at the same time. These power rails feed into both voltage converters, creating a digital 3V3 power line and Analog power line. These power lines (and their respective grounds) are both physically separated and used on different parts of the board to avoid noise and ground-reference shifting!
<img width="1230" height="766" alt="image" src="https://github.com/user-attachments/assets/f5297074-4fee-44f9-927d-339fe0df579f" />

### Schematic (All sheets)
![PCBSchematic](https://github.com/user-attachments/assets/4670dbb4-5875-4a96-b333-24631d5623e4)

### Layout
Full Layout:
<img width="972" height="695" alt="image" src="https://github.com/user-attachments/assets/a44c9da9-d550-4acb-bf7e-374860fbc576" />
<img width="863" height="566" alt="image" src="https://github.com/user-attachments/assets/dba7df1a-45fc-4ba4-8cf5-f24956843040" />
<img width="845" height="619" alt="image" src="https://github.com/user-attachments/assets/f3745a84-55aa-498d-80d0-7baa79fde328" />
<img width="891" height="593" alt="image" src="https://github.com/user-attachments/assets/cf7ae8ee-e237-467c-b778-f995c1fbce8f" />
<img width="832" height="524" alt="image" src="https://github.com/user-attachments/assets/6ab57140-dbd9-4f45-8b1c-8da6f2f88619" />

### 3D Render
<img width="759" height="527" alt="image" src="https://github.com/user-attachments/assets/be69b35a-b878-4c21-af88-f7dd6fc7a3a2" />
<img width="943" height="591" alt="image" src="https://github.com/user-attachments/assets/e57b2e52-91ee-42b5-b05f-f029a544abdc" />
