# Heart Rate Sensor on Arduino Mega 2560

A microcontroller-based system for real-time heart rate monitoring using photoplethysmography (PPG) with Arduino Mega 2560.

## Key Features
- Non-invasive optical heart rate measurement (PPG technique)
- Signal processing with low-pass digital filtering
- Real-time BPM calculation and graphical visualization
- Custom-built libraries for I2C, display, ADC, and pulse processing
- Bare-metal implementation (no Arduino.h dependency)

## Hardware Components
- Arduino Mega 2560 (ATmega2560)
- IDUINO SE049 optical pulse sensor
- SH1106 1.3" OLED display (128×64, I2C)
- Start/Stop button with hardware interrupt

## Technical Highlights
- **Optimized Performance**:
  - Custom I2C library (400kHz Fast Mode)
  - Bitmap display buffer with 5×8 font PROGMEM optimization
  - Recursive IIR filter for signal processing (α=0.2)
  - Timer0-based millis() implementation

- **Algorithms**:
  - Rising-edge detection for pulse peaks
  - Dynamic signal scaling for visualization
  - Automatic timeout (3s) for signal loss detection

## Key Connections:
| Component       | Arduino Pin | Notes               |
|-----------------|-------------|---------------------|
| Pulse Sensor    | A0          | 5V power, GND       |
| OLED (SH1106)   | SDA/SCL     | I2C (0x3C address)  |
| Button          | PD2         | INT4                |
