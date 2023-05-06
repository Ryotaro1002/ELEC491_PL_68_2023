# Required Libraries

This folder contains the following Arduino libraries required to run the collision detection and communication code in the "Integrated Sketch": 
1. **Adafruit_ADXL343, Adafruit_BusIO, Adafruit_Unified_Sensor:** libraries required for the accelerometer, Adafruit ADXL343 Evaluation Board, used for collision detection. 
2. **FreeRTOS_SAMD21:** FreeRTOS library ported for SAMD21 microcontroller faimily (which is used in Arduino MKR WiFi 1010). 
3. **RTCZero:** Real Time Counter (RTC) library used to keep track of time. 
4. **ThingSpeak:** API to communicate with ThingSpeak. 
5. **WiFiNINA:** library to use the WiFi module on the Arduino. 
6. **waveletDenoiserRev03:** custom-written library that performs wavelet denoising used in collision detection. See README in the folder for more detail. 
