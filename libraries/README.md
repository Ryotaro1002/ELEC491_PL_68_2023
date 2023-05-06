# Required Libraries

This folder contains the following Arduino libraries required to run the collision detection and communication code in the "Integrated Sketch": 
1. **Adafruit_ADXL343, Adafruit_BusIO, Adafruit_Unified_Sensor:** libraries required for the accelerometer, Adafruit ADXL343 Evaluation Board, used for collision detection. 
2. **FreeRTOS_SAMD21:** ***modified*** version of FreeRTOS library ported for SAMD21 microcontroller faimily (which is used in Arduino MKR WiFi 1010). See [below](https://github.com/Ryotaro1002/ELEC491_PL_68/blob/main/libraries/README.md#modification-in-freertos_samd21-library) for more detail.
3. **RTCZero:** Real Time Counter (RTC) library used to keep track of time. 
4. **ThingSpeak:** API to communicate with ThingSpeak. 
5. **WiFiNINA:** library to use the WiFi module on the Arduino. 
6. **waveletDenoiserRev03:** custom-written library that performs wavelet denoising used in collision detection. See README in the folder for more detail. 

## Licenses

Libraries 1-5 are copies or a modified version of the libraries written by manufacturers and third parties. They are redistributed here as permitted by their respective licenses. 

## About Library Versions

Newer versions of the libraries may be released by the manufacturers in the future. The code in the "Integrated Sketch" folder is run and tested with the versions of the libraries saved in this repository, and is not guaranteed to run correctly with the older/newer versions. 

You can check the library versions used by opening the "**library.properties**" file in each library folder. 

If you are using the older/newer versions of **FreeRTOS_SAMD21** library, make sure to modify the config file as outlined [below](https://github.com/Ryotaro1002/ELEC491_PL_68/blob/main/libraries/README.md#modification-in-freertos_samd21-library). 

## Modification in FreeRTOS_SAMD21 Library

All third-party libraries are unmodified except for FreeRTOS_SAMD21 library. One modification is made for our application: 
- In FreeRTOS_SAMD21/src/**FreeRTOSConfig.h**, `configTICK_RATE_HZ` is changed to `4000` (default is `1000`) in line 52. 
