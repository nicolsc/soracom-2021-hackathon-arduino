# Soracom hackathon 2021

Arduino code using : 
* Soracom Air SIM card
* MKRGSM board
* Grove Temperature Sensor
* Grove GPS Module

## Assembling

First, insert your Soracom SIM card in the dedicated slot under the board

Then assemble your board & sensors as in the picture below: 
![setup](./HW setup.jpg)

* Plug the Grove GPS Sensor on the SERIAL plug
* Plug the Grove Temperature Sensor on the D4 plug

## Configuration 

You need to set up the Soracom APN and login/passwords.
2 ways to do it : 
1/ Create a `secrets.h` file
2/ Remove the first line `#include "secrets.h"`and add your own configuration

Configuration (either in `secrets.h` or `arduino.ino` should look like
```
#define APN "apn"
#define LOGIN "login"
#define PWD "password"
```

Ask me for the actual values

## Run

* Open the folder in the [Arduino IDE](http://arduino.cc)
* ⇨ / Sketch > Upload
* Open the  🔎 Serial Monitor

