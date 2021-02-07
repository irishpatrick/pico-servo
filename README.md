# pico-servo

A simple C library for controlling servos using a Raspberry Pi Pico<br>
This is still a work-in-progress!

## How to Build
`mkdir build && cd build`<br>
`cmake .. && make`

Note that PICO_SDK_PATH must point to where your SDK is installed.

## How to Use

`#include "pico_servo.h"` In your code.<br>
pass `-lpico-servo` when compiling
