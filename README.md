# Swarm Robot Controller 
This project uses a webpage to control an individual swarm robot from its hosted access point. Once the code is runnning an the robot's ESP32, a laptop or phone must connect to the hosted network. 

## Installation 
This project is dependent on the platformio framework (used in VS Code) to compile and upload the files to the ESP32. To install platformio:
* Follow this tutorial up until 2:50 (We just need to install dependencies, not create a new project): `https://youtu.be/5edPOlQQKmo`
* Once platformio is loaded with the ESP32 dependencies, the files can be uploaded

## Uploading to ESP32
After your computer is connected to the ESP32 by cable, there are two upload options:
* Under 'General' `upload and monitor` will compile and upload the main C++ scripts to the ESP32 (Sometimes necessary to hold the boot button while this is happening)
* Under 'Platform' `Upload Filesystem Image (esp32dev)` will compiles and upload the HTML, CSS, and javascript files that run the webpage

## Main Code Breakdown
Within the main scripts, found in `src/main.cpp`, there are several important features to edit
* The pins for each robot are listed at the top of the code and need to be correct for each robot
* The server name and password should be unique for each robot