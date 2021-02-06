#include "pico/stdlib.h"
#include "pico_servo.h"

int main()
{
    stdio_init_all();

    servo_init();
    servo_attach(2);
    servo_move_to(2, 250);

    while (1)
    {
        servo_move_to(2, 250);
        sleep_ms(1000);
        servo_move_to(2, 1250);
        sleep_ms(1000);
    }
    return 0;
}