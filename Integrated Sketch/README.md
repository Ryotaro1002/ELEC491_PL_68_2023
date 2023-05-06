# Integrated Sketch

(Updated May 5, 2023 by Ryotaro)

This folder contains the integrated Arduino sketches that: 
1. perform collision detection using an accelerometer, 
2. perform periodic fall detection task to check if the accelerometer fell, and 
3. periodically upload collision data to ThingSpeak via WiFi. 

There are two versions of the sketches: 
1. `Every_Five_Minutes.ino`: send collision data to ThingSpeak every 5 minutes (if any). If no collision is detected during a 5-minute period, Arduino will NOT send data to ThingSpeak to avoid unneccesary communication. 
2. `Once_Per_Day.ino`: send collision data to ThingSpeak once a day at midnight (if any). Just like the other version, Arduino will NOT send data to ThingSpeak if no collision is detected. 

Pick the version that suits your needs, and upload it to Arduino. 

## Required Libraries

Required libraries are saved in **libraries** folder in this repository. 
