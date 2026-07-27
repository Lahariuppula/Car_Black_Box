## Car Black Box

An Embedded C based **Car Black Box System** developed using the **PIC16F877A** microcontroller. The system records important driving events along with real-time speed and timestamp, providing a simple event data recorder similar to an automotive black box.

## Features

- Displays current time using DS1307 RTC
- Displays vehicle speed using ADC
- Logs driving events with timestamp and speed
- Supports Gear Change and Collision event logging
- Password protected login
- View stored event logs
- Download logs through UART
- Clear event logs
- Change password
- Set RTC time
- Automatic logout after inactivity
- User blocking after multiple incorrect password attempts

## Hardware Used

- PIC16F877A
- CLCD (16x2)
- DS1307 RTC
- External EEPROM (24C02)
- Digital Keypad
- Potentiometer (Speed Simulation)
- UART Interface

## Software Used

- MPLAB X IDE
- XC8 Compiler
- Embedded C
- Git & GitHub

## Project Modules

- Dashboard Screen
- Login Screen
- Main Menu
- View Logs
- Download Logs
- Clear Logs
- Change Password
- Set Time
- EEPROM Driver
- DS1307 Driver
- UART Driver
- ADC Driver
- I2C Driver
- CLCD Driver
- Timer & Interrupt Handling

## Event Format

HHMMSS  EVENT  SPEED

Example:
103015  G1     25
103225  C      40
103455  G2     35

## Project Flow

Dashboard --> Login --> Main Menu -- View Logs
                                  -- Clear Logs
                                  -- Download Logs
                                  -- Set Time
                                  -- Change Password

## Learning Outcomes

- Embedded C Programming
- PIC16F877A Programming
- EEPROM Memory Handling
- RTC Communication (DS1307)
- I2C Protocol
- UART Communication
- ADC
- Interrupt Handling
- Menu Driven Embedded Applications

## Author: Soundharya Lahari Uppula
