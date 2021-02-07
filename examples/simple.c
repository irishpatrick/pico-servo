#include "pico/stdlib.h"
#include "pico_servo.h"
#include <stdio.h>

#define A 2
#define B 12

int main()
{
    stdio_init_all();
    
    servo_init();
    servo_attach(A);
    servo_attach(B);

    while (1)
    {
        servo_move_to(A, 0);
        servo_move_to(B, 0);
        sleep_ms(1000);
        servo_move_to(A, 180);
        servo_move_to(B, 90);
        sleep_ms(1000);
    }
    return 0;
}