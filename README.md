# ESP32 Telegram Remote Control

Control any device (Light, Fan, Motor, Pump,etc.) from anywhere using Telegram.

### Features
- Control from Telegram Bot
- ON/OFF Inline Buttons
- Admin Protection - Only you can control
- Auto WiFi Reconnect
- Works with any 3.3V Relay Module

### Hardware Required
- ESP32 Dev Board
- 1 Channel 3.3V Relay Module
- Jumper Wires

### Wiring

| Relay Module | ESP32 |
| :--- | :--- |
| VCC | 3.3V |
| GND | GND |
| IN | GPIO 23 |

> ⚠️ ESP32 is 3.3V only. Never connect 220V directly to ESP32. Always use Relay Module.

### How to Flash .ino File - Steps

**1. Install Arduino IDE**
Download from arduino.cc

**2. Add ESP32 Board**
File > Preferences > Additional Boards Manager URLs > Paste this: