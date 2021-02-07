// MIT License

// Copyright (c) 2021 Patrick Roche

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "pico/stdlib.h"
#include "pico_servo.h"
#include <stdio.h>

#define A 2
#define B 12

int main()
{
    stdio_init_all();

    servo_init();
    servo_clock_auto();

    servo_attach(A);
    servo_attach(B);

    while (1)
    {
        servo_move_to(A, 0);
        servo_move_to(B, 0);
        sleep_ms(500);
        servo_move_to(A, 180);
        servo_move_to(B, 90);
        sleep_ms(500);
    }
    return 0;
}