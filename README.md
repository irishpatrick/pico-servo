# pico-servo

A simple C library for controlling servos using a Raspberry Pi Pico<br>
This is still a work-in-progress!

## Contributing
Please raise an issue or a pull request for any problems/bugs/features/fixes!

## Installing Dependencies
This library depends on [pico-libfixmath](git@github.com:irishpatrick/pico-servo.git). To install it, run the following command from the root of the repository:<br>
`git submodule update --init --recursive`

## How to Build
Once you have installed the required depedencies, you can build the library with the following commmands:<br>
`mkdir build && cd build`<br>
`cmake .. && make`

Note that `PICO_SDK_PATH` must point to where your SDK is installed. This can be achieved by setting the environment variable with<br>
`export PICO_SDK_PATH=/wherever/your/sdk/is`

## How to Use

`#include "pico_servo.h"` in your code or copy-paste the file into your include directory.<br>
Pass `-lpico-servo` when compiling.

You may need to copy these files to the location of your project, and find the library in cmake. See the following cmake example snippet for what could be added to your CMakeLists.txt file.

```
find_library(PICO_SERVO_LIB NAMES pico-servo HINTS "/path/to/libpico-servo.a)

...

target_link_libraries(YOUR_PROJECT ... ${PICO_SERVO_LIB})
```


